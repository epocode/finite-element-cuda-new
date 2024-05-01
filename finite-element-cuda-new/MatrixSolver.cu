#include "device_launch_parameters.h"
#include "cuda_runtime.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <cusolverDn.h>
#include "cusolver_utils.h"



__global__ void assignMemory(double  matrix[], double a[], int rows, int cols)//都是列优先
{
    int row = threadIdx.y + blockIdx.y * blockDim.y;
    int col = threadIdx.x + blockIdx.x * blockDim.x;
    if (row >= rows || col >= cols) {
        return;
    }
    double temp = matrix[col * rows + row];
    a[col * rows + row] = temp;
}

extern "C" void solveMatrix(Eigen::MatrixXd & kMatrix, Eigen::MatrixXd & fMatrix, Eigen::MatrixXd & uvMatrix) {
    std::ofstream logs("logs.txt", std::ios::app);
    int size = kMatrix.rows();
    const int ldk = size; // 矩阵k的leading dimension
    const int ldf = size; //矩阵F的leading dimension
    double* h_K, * h_F;
    CUDA_CHECK(cudaMallocManaged(&h_K, size * size * sizeof(double)));
    CUDA_CHECK(cudaMallocManaged(&h_F, size * sizeof(double)));
    //将eigen矩阵赋值给新创建的数组
    dim3 blockSize(32, 32);
    dim3 gridSize((size + blockSize.x - 1) / blockSize.x, (size + blockSize.y - 1) / blockSize.y);
    double* kMatrixData = kMatrix.data();
    double* deviceKMatrixData;
    CUDA_CHECK(cudaMallocManaged(&deviceKMatrixData, kMatrix.size() * sizeof(double)));
    CUDA_CHECK(cudaMemcpy(deviceKMatrixData, kMatrixData, kMatrix.size() * sizeof(double), cudaMemcpyHostToDevice));
    assignMemory << <gridSize, blockSize >> > (deviceKMatrixData, h_K, kMatrix.rows(), kMatrix.cols());
    cudaDeviceSynchronize();
    std::ofstream f("h_K.txt");
    if (f.is_open()) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                double temp = h_K[j * size + i];
                f << temp << "\t";
            }
            f << "\n";
        }
        f.close();
    }
    else {
        logs << "h_K 打开失败" << std::endl;
    }
    
    dim3 blockSizeF = dim3(1, 32);
    dim3 gridSizeF =  dim3(1, (size + blockSize.y - 1) / blockSize.y);
    double* fMatrixData = fMatrix.data();
    double* deviceFMatrixData;
    CUDA_CHECK(cudaMallocManaged(&deviceFMatrixData, fMatrix.size() * sizeof(double)));
    CUDA_CHECK(cudaMemcpy(deviceFMatrixData, fMatrixData, fMatrix.size() * sizeof(double), cudaMemcpyHostToDevice));
    assignMemory << <gridSizeF, blockSizeF >> > (deviceFMatrixData, h_F, fMatrix.rows(), fMatrix.cols());
    cudaDeviceSynchronize();
    f = std::ofstream("h_F.txt");
    if (f.is_open()) {
        for (int i = 0; i < size; i++) {
            double temp = h_F[i];
            f << temp << "\n";
        }
        f.close();
    }
    else {
        logs << "h_F 打开失败" << std::endl;
    }
      
    cusolverDnHandle_t cusolverH = NULL;
    cudaStream_t stream = NULL;

    const int m = size;
    const int lda = m;
    const int ldb = m;


    std::vector<double> X(m, 0);
    std::vector<double> LU(lda * m, 0);
    std::vector<int> Ipiv(m, 0);
    int info = 0;

    double* d_A = nullptr; /* device copy of A */
    double* d_B = nullptr; /* device copy of B */
    int* d_Ipiv = nullptr; /* pivoting sequence */
    int* d_info = nullptr; /* error info */

    int lwork = 0;            /* size of workspace */
    double* d_work = nullptr; /* device workspace for getrf */

    const int pivot_on = 0;

    if (pivot_on) {
        printf("pivot is on : compute P*A = L*U \n");
    }
    else {
        printf("pivot is off: compute A = L*U (not numerically stable)\n");
    }


    /* step 1: create cusolver handle, bind a stream */
    CUSOLVER_CHECK(cusolverDnCreate(&cusolverH));

    CUDA_CHECK(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
    CUSOLVER_CHECK(cusolverDnSetStream(cusolverH, stream));

    /* step 2: copy A to device */
    CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_A), sizeof(double) * size * size));
    CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_B), sizeof(double) * size));
    CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_Ipiv), sizeof(int) * Ipiv.size()));
    CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_info), sizeof(int)));

    CUDA_CHECK(
        cudaMemcpyAsync(d_A, h_K, sizeof(double) * size * size, cudaMemcpyHostToDevice, stream));
    CUDA_CHECK(
        cudaMemcpyAsync(d_B, h_F, sizeof(double) *  size, cudaMemcpyHostToDevice, stream));

    /* step 3: query working space of getrf */
    CUSOLVER_CHECK(cusolverDnDgetrf_bufferSize(cusolverH, m, m, d_A, lda, &lwork));

    CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_work), sizeof(double) * lwork));

    /* step 4: LU factorization */
    if (pivot_on) {
        CUSOLVER_CHECK(cusolverDnDgetrf(cusolverH, m, m, d_A, lda, d_work, d_Ipiv, d_info));
    }
    else {
        CUSOLVER_CHECK(cusolverDnDgetrf(cusolverH, m, m, d_A, lda, d_work, NULL, d_info));
    }

    if (pivot_on) {
        CUDA_CHECK(cudaMemcpyAsync(Ipiv.data(), d_Ipiv, sizeof(int) * Ipiv.size(),
            cudaMemcpyDeviceToHost, stream));
    }
    CUDA_CHECK(
        cudaMemcpyAsync(LU.data(), d_A, sizeof(double) * size * size, cudaMemcpyDeviceToHost, stream));
    CUDA_CHECK(cudaMemcpyAsync(&info, d_info, sizeof(int), cudaMemcpyDeviceToHost, stream));

    CUDA_CHECK(cudaStreamSynchronize(stream));

    if (0 > info) {
        printf("%d-th parameter is wrong \n", -info);
        exit(1);
    }
    if (pivot_on) {
        printf("pivoting sequence, matlab base-1\n");
        for (int j = 0; j < m; j++) {
            printf("Ipiv(%d) = %d\n", j + 1, Ipiv[j]);
        }
    }



    if (pivot_on) {
        CUSOLVER_CHECK(cusolverDnDgetrs(cusolverH, CUBLAS_OP_N, m, 1, /* nrhs */
            d_A, lda, d_Ipiv, d_B, ldb, d_info));
    }
    else {
        CUSOLVER_CHECK(cusolverDnDgetrs(cusolverH, CUBLAS_OP_N, m, 1, /* nrhs */
            d_A, lda, NULL, d_B, ldb, d_info));
    }

    CUDA_CHECK(
        cudaMemcpyAsync(X.data(), d_B, sizeof(double) * X.size(), cudaMemcpyDeviceToHost, stream));
    CUDA_CHECK(cudaStreamSynchronize(stream));

    uvMatrix = Eigen::MatrixXd::Zero(size, 1);
    f = std::ofstream("h_UV.txt");
    if (f.is_open()) {
        for (int i = 0; i < size; i++) {
            double temp = X[i];
            uvMatrix(i, 0) = temp;
            f << temp << "\n";
        }
        f.close();
    }

    /* free resources */
    CUDA_CHECK(cudaFree(d_A));
    CUDA_CHECK(cudaFree(d_B));
    CUDA_CHECK(cudaFree(d_Ipiv));
    CUDA_CHECK(cudaFree(d_info));
    CUDA_CHECK(cudaFree(d_work));

    CUSOLVER_CHECK(cusolverDnDestroy(cusolverH));

    CUDA_CHECK(cudaStreamDestroy(stream));

    CUDA_CHECK(cudaDeviceReset());
    logs.close();

}


