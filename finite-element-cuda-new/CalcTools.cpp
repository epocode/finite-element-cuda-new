#include "CalcTools.h"
#include "MshInformation.h"
#include <Eigen/Dense>
#include <qdebug.h>
#include <unordered_map>
#include <math.h>
#include "publicElement.h"
#include <math.h>
#include <cfloat>


using namespace Eigen;
extern MshInformation mshInfo;

namespace CalcTools {
    bool calcKMatrix() {
        //计算k矩阵
        for (int index = 0; index < mshInfo.elementTags[mshInfo.triangleIndex].size(); index++) {
            vector<int> nodeTags = mshInfo.getPointTags(index);
            int i = mshInfo.tagMap[nodeTags[0]];
            int j = mshInfo.tagMap[nodeTags[1]];
            int k = mshInfo.tagMap[nodeTags[2]];
            unordered_map<int, double> a;
            unordered_map<int, double> b;
            unordered_map<int, double> c;
            calcABC(a, b, c, i, j, k);
            double square = calcSquare(i, j, k);
            double temp = (mshInfo.E * mshInfo.t) / (4 * (1 - mshInfo.v * mshInfo.v) * square);
            double tempMatrix[2][2] = { 0 };
            vector<int> indexes;
            indexes.push_back(i);
            indexes.push_back(j);
            indexes.push_back(k);
            for (int r : indexes) {
                for (int s : indexes) {
                    mshInfo.kMatrix(r * 2, s * 2) += temp * (b[r] * b[s] + (1 - mshInfo.v) / 2 * c[r] * c[s]);
                    mshInfo.kMatrix(r * 2, s * 2 + 1) += temp * (mshInfo.v * b[r] * c[s] + (1 - mshInfo.v) / 2 * c[r] * b[s]);
                    mshInfo.kMatrix(r * 2 + 1, s * 2) += temp * (mshInfo.v * c[r] * b[s] + (1 - mshInfo.v) / 2 * b[r] * c[s]);
                    mshInfo.kMatrix(r * 2 + 1, s * 2 + 1) += temp * (c[r] * c[s] + (1 - mshInfo.v) / 2 * b[r] * b[s]);
                }
            }
        }
        return true;
    }


    void calcABC(std::unordered_map<int, double>& a, std::unordered_map<int, double>& b, std::unordered_map<int, double>& c, int i, int j, int  k) {
        vector<int> list;
        list.push_back(i);
        list.push_back(j);
        list.push_back(k);
        for (int i = 0; i < 3; i++) {
            a[list[i]] = mshInfo.xList[list[(i + 1) % 3]] * mshInfo.yList[list[(i + 2) % 3]]
                - mshInfo.xList[list[(i + 2) % 3]] * mshInfo.yList[list[(i + 1) % 3]];
            b[list[i]] = mshInfo.yList[list[(i + 1) % 3]] - mshInfo.yList[list[(i + 2) % 3]];
            c[list[i]] = -mshInfo.xList[list[(i + 1) % 3]] + mshInfo.xList[list[(i + 2) % 3]];
        }
    }

    double calcSquare(int i, int j, int k) {
        double ret = (mshInfo.xList[i] * mshInfo.yList[j] + mshInfo.xList[j] * mshInfo.yList[k] + mshInfo.xList[k] * mshInfo.yList[i] - mshInfo.xList[j] * mshInfo.yList[i] - mshInfo.xList[k] * mshInfo.yList[j] - mshInfo.xList[i] * mshInfo.yList[k]);
        return ret > 0 ? ret : -ret;
    }



    void calcFMatrix() {
        for (int i = 0; i < mshInfo.forces.size(); i++) {
            Point o;
            o.x = mshInfo.forces[i].x;
            o.y = mshInfo.forces[i].y;
            for (int j = 0; j < mshInfo.elementTags[mshInfo.triangleIndex].size(); j++) {
                Point p1, p2, p3;
                int index1, index2, index3;
                index1 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 0]];
                index2 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 1]];
                index3 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 2]];
                p1.x = mshInfo.xList[index1];
                p1.y = mshInfo.yList[index1];
                p2.x = mshInfo.xList[index2];
                p2.y = mshInfo.yList[index2];
                p3.x = mshInfo.xList[index3];
                p3.y = mshInfo.yList[index3];
                if (!isInTriangle(p1, p2, p3, o)) {
                    continue;
                }
                unordered_map<int, double> a, b, c, N;
                vector<int> list;
                list.push_back(index1);
                list.push_back(index2);
                list.push_back(index3);
                calcABC(a, b, c, index1, index2, index3);
                double A = (mshInfo.yList[index3] * (mshInfo.xList[index2] - mshInfo.xList[index1]) +
                    mshInfo.yList[index2] * (mshInfo.xList[index1] - mshInfo.xList[index3]) +
                    mshInfo.yList[index1] * (mshInfo.xList[index3] - mshInfo.xList[index2])) / 2;
                for (int i = 0; i < 3; i++) {
                    N[list[i]] = (a[list[i]] + b[list[i]] * o.x + c[list[i]] * o.y) / 2 / A;
                }
                mshInfo.fMatrix(index1 * 2, 0) += N[index1] * mshInfo.forces[i].xForce;
                mshInfo.fMatrix(index1 * 2 + 1, 0) += N[index1] * mshInfo.forces[i].yForce;
                mshInfo.fMatrix(index2 * 2, 0) += N[index2] * mshInfo.forces[i].xForce;
                mshInfo.fMatrix(index2 * 2 + 1, 0) += N[index2] * mshInfo.forces[i].yForce;
                mshInfo.fMatrix(index3 * 2, 0) += N[index3] * mshInfo.forces[i].xForce;
                mshInfo.fMatrix(index3 * 2 + 1, 0) += N[index3] * mshInfo.forces[i].yForce;
                break;
            }
        }

    }


    bool isInTriangle(Point p1, Point p2, Point p3, Point o) {
        double s1 = getArea(p1, p2, o);
        double s2 = getArea(p2, p3, o);
        double s3 = getArea(p3, p1, o);
        double s = getArea(p1, p2, p3);
        return (s1 + s2 + s3 - s) < 1e-5;
    }

    double getArea(Point p1, Point p2, Point p3) {
        double a = getDist(p1, p2);
        double b = getDist(p2, p3);
        double c = getDist(p1, p3);
        double p = (a + b + c) / 2;
        return sqrt(p * (p - a) * (p - b) * (p - c));
    }

    double getDist(Point p1, Point p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }


    void setEdge(double x, double y, bool xFixed, bool yFixed) {
        EdgeInfo edgeInfo;
        edgeInfo.x = x;
        edgeInfo.y = y;
        edgeInfo.xFixed = xFixed;
        edgeInfo.yFixed = yFixed;
        mshInfo.edgeInfos.push_back(edgeInfo);
    }

    void calcStressStrain()
    {
        for (int i = 0; i < mshInfo.elementTags[mshInfo.triangleIndex].size(); i++) {
            int index1 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][i * 3 + 0]];
            int index2 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][i * 3 + 1]];
            int index3 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][i * 3 + 2]];
            MechanicBehavior mechanicBehavoir;
            mechanicBehavoir.p1.x = mshInfo.xList[index1];
            mechanicBehavoir.p1.y = mshInfo.yList[index1];
            mechanicBehavoir.p2.x = mshInfo.xList[index2];
            mechanicBehavoir.p2.y = mshInfo.yList[index2];
            mechanicBehavoir.p3.x = mshInfo.xList[index3];
            mechanicBehavoir.p3.y = mshInfo.yList[index3];

            double _2A = (mshInfo.yList[index3] - mshInfo.yList[index1]) * (mshInfo.xList[index2] - mshInfo.xList[index1])
                - (mshInfo.yList[index1] - mshInfo.yList[index2]) * (mshInfo.xList[index1] - mshInfo.xList[index3]);
            unordered_map<int, double> a;
            unordered_map<int, double> b;
            unordered_map<int, double> c;
            calcABC(a, b, c, index1, index2, index3);
            Eigen::MatrixXd B(3, 6);
            B << b[index1], 0, b[index2], 0, b[index3], 0,
                0, c[index1], 0, c[index2], 0, c[index3],
                c[index1], b[index1], c[index2], b[index2], c[index3], b[index3];
            Eigen::MatrixXd UV(6, 1);
            UV << mshInfo.X(index1 * 2, 0), mshInfo.X(index1 * 2 + 1, 0),
                mshInfo.X(index2 * 2, 0), mshInfo.X(index2 * 2 + 1, 0),
                mshInfo.X(index3 * 2, 0), mshInfo.X(index3 * 2 + 1, 0);
            mechanicBehavoir.strain = (1 / _2A) * (B * UV);
            double epsilonX = mechanicBehavoir.strain(0, 0);
            double epsilonY = mechanicBehavoir.strain(1, 0);
            double tau = mechanicBehavoir.strain(2, 0);
            mechanicBehavoir.equalStrain = sqrt(2 * (pow(epsilonX - epsilonY, 2) + epsilonY * epsilonY + epsilonX * epsilonX
                + 6 * tau * tau) / 3);
            Eigen::MatrixXd D(3, 3);
            D << 1, mshInfo.v, 0,
                mshInfo.v, 1, 0,
                0, 0, (1 - mshInfo.v) / 2;
            D = ((mshInfo.E) / (1 - mshInfo.v * mshInfo.v)) * D;
            mechanicBehavoir.stress = D * B * UV;
            double sigmaX;
            double sigmaY;
            sigmaX = mechanicBehavoir.stress(0, 0);
            sigmaY = mechanicBehavoir.stress(1, 0);
            tau = mechanicBehavoir.stress(2, 0);
            mechanicBehavoir.equalStress = sqrt(sigmaX * sigmaX - sigmaX * sigmaY + sigmaY * sigmaY + 3 * tau * tau);
            mshInfo.mechanicBehaviors.push_back(mechanicBehavoir);
        }
    }

    void getExtreme(double& min, double& max)
    {
        for (MechanicBehavior factor : mshInfo.mechanicBehaviors) {
            if (factor.equalStress < min) {
                min = factor.equalStress;
            }
            if (factor.equalStress > max) {
                max = factor.equalStress;
            }
        }
    }

    void handleEdge()
    {
        for (EdgeInfo &edgeInfo : mshInfo.edgeInfos) {
            double x = edgeInfo.x;
            double y = edgeInfo.y;
            bool xFixed = edgeInfo.xFixed;
            bool yFixed = edgeInfo.yFixed;
            for (int j = 0; j < mshInfo.elementTags[mshInfo.triangleIndex].size(); j++) {
                Point p1, p2, p3, o;
                int index1, index2, index3;
                index1 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 0]];
                index2 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 1]];
                index3 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 2]];
                p1.x = mshInfo.xList[index1];
                p1.y = mshInfo.yList[index1];
                p2.x = mshInfo.xList[index2];
                p2.y = mshInfo.yList[index2];
                p3.x = mshInfo.xList[index3];
                p3.y = mshInfo.yList[index3];
                o.x = x;
                o.y = y;
                int closestIndex = -1;

                if (isInTriangle(p1, p2, p3, o)) {
                    closestIndex = pow(mshInfo.xList[index1] - x, 2) + pow(mshInfo.yList[index1] - y, 2) <
                        pow(mshInfo.xList[index1] - x, 2) + pow(mshInfo.yList[index1] - y, 2) ? index1 : index2;
                    closestIndex = pow(mshInfo.xList[closestIndex] - x, 2) + pow(mshInfo.yList[closestIndex] - y, 2) <
                        pow(mshInfo.xList[index3] - x, 2) + pow(mshInfo.yList[index3] - y, 2) ? closestIndex : index3;
                    if (xFixed) {
                        mshInfo.fMatrix(closestIndex * 2 + 0, 0) = 0;
                        mshInfo.kMatrix(closestIndex * 2, closestIndex * 2) = 1;
                        for (int i = 0; i < mshInfo.kMatrix.rows(); i++) {
                            if (i == closestIndex * 2) {
                                continue;
                            }
                            mshInfo.kMatrix(i, closestIndex * 2) = 0;
                            mshInfo.kMatrix(closestIndex * 2, i) = 0;
                        }
                    }
                    if (yFixed) {
                        mshInfo.fMatrix(closestIndex * 2 + 1, 0) = 0;
                        mshInfo.kMatrix(closestIndex * 2 + 1, closestIndex * 2 + 1) = 1;
                        for (int i = 0; i < mshInfo.kMatrix.rows(); i++) {
                            if (i == closestIndex * 2 + 1) {
                                continue;
                            }
                            mshInfo.kMatrix(i, closestIndex * 2 + 1) = 0;
                            mshInfo.kMatrix(closestIndex * 2 + 1, i) = 0;
                        }
                    }
                    break;
                }
                

            }
            /*for (const auto& pair : mshInfo.tagMap) {
                int index = pair.second;
                if (pow(mshInfo.xList[index] - x, 2) < 1e-4 && pow(mshInfo.yList[index] - y, 2) < 1e-4) {
                    if (xFixed) {
                        mshInfo.fMatrix(index * 2 + 0, 0) = 0;
                        mshInfo.kMatrix(index * 2, index * 2) = 1;
                        for (int i = 0; i < mshInfo.kMatrix.rows(); i++) {
                            if (i == index * 2) {
                                continue;
                            }
                            mshInfo.kMatrix(i, index * 2) = 0;
                            mshInfo.kMatrix(index * 2, i) = 0;
                        }
                    }
                    if (yFixed) {
                        mshInfo.fMatrix(index * 2 + 1, 0) = 0;
                        mshInfo.kMatrix(index * 2 + 1, index * 2 + 1) = 1;
                        for (int i = 0; i < mshInfo.kMatrix.rows(); i++) {
                            if (i == index * 2 + 1) {
                                continue;
                            }
                            mshInfo.kMatrix(i, index * 2 + 1) = 0;
                            mshInfo.kMatrix(index * 2 + 1, i) = 0;
                        }
                    }
                }
            }*/
        }

    }

    bool getPointInfos(double x, double y, MechanicBehavior& mechanicBehavior, double& uValue, double& vValue)
    {
        bool inTriangle = false;
        Point o;
        o.x = x;
        o.y = y;
        for (MechanicBehavior tempMechanicBehavoir : mshInfo.mechanicBehaviors) {
            if (isInTriangle(tempMechanicBehavoir.p1, tempMechanicBehavoir.p2, tempMechanicBehavoir.p3, o)) {//当这个点在已知的网格三角形中时
                inTriangle = true;
                mechanicBehavior.p1 = tempMechanicBehavoir.p1;
                mechanicBehavior.p2 = tempMechanicBehavoir.p2;
                mechanicBehavior.p3 = tempMechanicBehavoir.p3;
                mechanicBehavior.stress = tempMechanicBehavoir.stress;
                mechanicBehavior.strain = tempMechanicBehavoir.strain;
                mechanicBehavior.equalStress = tempMechanicBehavoir.equalStress;
                mechanicBehavior.equalStrain = tempMechanicBehavoir.equalStrain;
            }
        }
        if (!inTriangle) {
            return false;
        }
        for (int j = 0; j < mshInfo.elementTags[mshInfo.triangleIndex].size(); j++) {
            Point p1, p2, p3;
            int index1, index2, index3;
            index1 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 0]];
            index2 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 1]];
            index3 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][j * 3 + 2]];
            p1.x = mshInfo.xList[index1];
            p1.y = mshInfo.yList[index1];
            p2.x = mshInfo.xList[index2];
            p2.y = mshInfo.yList[index2];
            p3.x = mshInfo.xList[index3];
            p3.y = mshInfo.yList[index3];
            if (!isInTriangle(p1, p2, p3, o)) {
                continue;
            }
            unordered_map<int, double> a, b, c, N;
            vector<int> list;
            list.push_back(index1);
            list.push_back(index2);
            list.push_back(index3);
            calcABC(a, b, c, index1, index2, index3);
            double A = (mshInfo.yList[index3] * (mshInfo.xList[index2] - mshInfo.xList[index1]) +
                mshInfo.yList[index2] * (mshInfo.xList[index1] - mshInfo.xList[index3]) +
                mshInfo.yList[index1] * (mshInfo.xList[index3] - mshInfo.xList[index2])) / 2;
            for (int i = 0; i < 3; i++) {
                N[list[i]] = (a[list[i]] + b[list[i]] * o.x + c[list[i]] * o.y) / 2 / A;
            }

            Eigen::MatrixXd uviMatrix(6, 1);
            uviMatrix(0, 0) = mshInfo.X(2 * index1, 0);
            uviMatrix(1, 0) = mshInfo.X(2 * index1 + 1, 0);
            uviMatrix(2, 0) = mshInfo.X(2 * index2, 0);
            uviMatrix(3, 0) = mshInfo.X(2 * index2 + 1, 0);
            uviMatrix(4, 0) = mshInfo.X(2 * index3, 0);
            uviMatrix(5, 0) = mshInfo.X(2 * index3 + 1, 0);
            Eigen::MatrixXd nMatrix = Eigen::MatrixXd::Zero(2, 6);
            nMatrix(0, 0) = N[index1];
            nMatrix(0, 2) = N[index2];
            nMatrix(0, 4) = N[index3];
            nMatrix(1, 1) = N[index1];
            nMatrix(1, 3) = N[index2];
            nMatrix(1, 5) = N[index3];
            Eigen::MatrixXd uvMatrix = nMatrix * uviMatrix;
            uValue = uvMatrix.coeff(0, 0);
            vValue = uvMatrix.coeff(1, 0);

        }
        return true;
    }

    double distance(double x1, double y1, double x2, double y2) {
        return std::hypot(x2 - x1, y2 - y1);
    }
    double pointToSegmentDistance(double ax, double ay, double bx, double by, double cx, double cy) {
        double abx = bx - ax;
        double aby = by - ay;
        double acx = cx - ax;
        double acy = cy - ay;
        double bcx = cx - bx;
        double bcy = cy - by;

        // 计算投影长度t
        double t = (acx * abx + acy * aby) / (abx * abx + aby * aby);

        // 如果垂足落在线段AB上
        if (t >= 0 && t <= 1) {
            // 计算垂足坐标
            double px = ax + t * abx;
            double py = ay + t * aby;
            // 返回点C到垂足的距离
            return distance(cx, cy, px, py);
        }

        // 否则返回点C到A或B的距离中较小的一个
        return std::fmin(distance(ax, ay, cx, cy), distance(bx, by, cx, cy));
    }
    extern "C" void solveMatrix(Eigen::MatrixXd & kMatrix, Eigen::MatrixXd & fMatrix, Eigen::MatrixXd & uvMatrix);
    void generateMatrixes(double E, double v, double t)
    {
        mshInfo.E = E;
        mshInfo.v = v;  
        mshInfo.t = t;
        calcKMatrix();
        calcFMatrix();
        handleEdge();
        //mshInfo.X = mshInfo.kMatrix.colPivHouseholderQr().solve(mshInfo.fMatrix);
        solveMatrix(mshInfo.kMatrix, mshInfo.fMatrix, mshInfo.X);
        mshInfo.saveMatrixes();
    }

}
