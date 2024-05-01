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

    //形状信息
    Circle circle;
    vector<Coordinate> coordinates;
    //文件信息
    QString filePath;

    //网格点的信息
    vector<int> pointIndexs;
    vector<int> lineIndexs;
    vector<int> circlePointIndexs;
    vector<int>circleArcIndexs;
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
};
