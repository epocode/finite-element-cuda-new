#pragma once

#include <vector>
#include <unordered_map>
#include "publicElement.h"
#include "Eigen/Dense"
#include <QString>


using namespace std;

class MshInformation
{
public:
    double E;
    double  t;
    double v;
    double lc;
    //文件信息
    QString filePath;
    //用于存储curveLoop的信息
    vector<int> curveLoopList;
    //网格点的信息
    unordered_map<int, int> tagMap;//将点的index对应矩阵中的下标

    vector<size_t> nodeTags;//the tags of all nodes
    vector<double> coord; //the position of every nodes
    vector<double> parametricCoord;

    vector<int> elementTypes;
    vector<vector<size_t>> elementTags;
    vector<vector<size_t>> nodeTagsForTriangle;
    int triangleIndex;

    vector<double> xList;
    vector<double> yList;

    //计算时的K矩阵，计算时的F矩阵
    Eigen::MatrixXd kMatrix;
    Eigen::MatrixXd fMatrix;
    Eigen::MatrixXd X;
    //边界力
    vector<Force> forces;
    //边界条件
    vector<EdgeInfo> edgeInfos;

    //渲染的时候需要的信息
    vector<MechanicBehavior>  mechanicBehaviors;


    MshInformation();
    vector<int> getPointTags(int i);
    bool initTagMap();
    void initPointAndTriangleInfo();
    void clearAll();
    void addRect(double x, double y, double width, double height);
    void addCircle(double x, double y, double radius);
    void createMsh();
    void saveMsh();
    bool loadMsh();
    void addUniformLoad(double startX, double startY, double endX, double endY, double xDirection, double yDirection);
    void addPointForce(double x, double y, double xForce, double yForce);
    void saveConstraint();
    void loadConstraint();
    void saveMatrixes();
private:
    struct ComparePair {
        bool operator()(const std::pair<double, double>& a, const std::pair<double, double>& b) const {
            return a.first > b.first; // 按照pair的first值从小到大排序
        }
    };
};
