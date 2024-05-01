#pragma once;
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <Eigen/Dense>

// ���ڼ��CUDA���õĺ�
#define CUDA_CHECK(call) \
    do { \
        cudaError_t error = call; \
        if (error != cudaSuccess) { \
            printf("Error: %s:%d, ", __FILE__, __LINE__); \
            printf("code:%d, reason: %s\n", error, cudaGetErrorString(error)); \
            exit(1); \
        } \
    } while (0)

// ���ڼ��cuSolver���õĺ�
#define CUSOLVER_CHECK(call) \
    do { \
        cusolverStatus_t status = call; \
        if (status != CUSOLVER_STATUS_SUCCESS) { \
            printf("Error: %s:%d, ", __FILE__, __LINE__); \
            printf("status:%d\n", status); \
            exit(1); \
        } \
    } while (0)


#define CUBLAS_CHECK(call) \
    do { \
        cublasStatus_t status = call; \
        if (status != CUBLAS_STATUS_SUCCESS) { \
            fprintf(stderr, "Error: %s:%d, ", __FILE__, __LINE__); \
            fprintf(stderr, "status:%d\n", status); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
void solveMatrix(Eigen::MatrixXd &kMatrix, Eigen::MatrixXd & fMatrix, Eigen::MatrixXd & uvMatrix);
__global__ void assignMemory(Eigen::MatrixXd &matrix, double *a);