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
    //�ļ���Ϣ
    QString filePath;
    //���ڴ洢curveLoop����Ϣ
    vector<int> curveLoopList;
    //��������Ϣ
    unordered_map<int, int> tagMap;//�����index��Ӧ�����е��±�

    vector<size_t> nodeTags;//the tags of all nodes
    vector<double> coord; //the position of every nodes
    vector<double> parametricCoord;

    vector<int> elementTypes;
    vector<vector<size_t>> elementTags;
    vector<vector<size_t>> nodeTagsForTriangle;
    int triangleIndex;

    vector<double> xList;
    vector<double> yList;

    //����ʱ��K���󣬼���ʱ��F����
    Eigen::MatrixXd kMatrix;
    Eigen::MatrixXd fMatrix;
    Eigen::MatrixXd X;
    //�߽���
    vector<Force> forces;
    //�߽�����
    vector<EdgeInfo> edgeInfos;

    //��Ⱦ��ʱ����Ҫ����Ϣ
    vector<MechanicBehavior>  mechanicBehaviors;


    MshInformation();
    vector<int> getPointTags(int i);
    bool initTagMap();
    void initPointAndTriangleInfo();
    void clearAll();
    void addRect(double x, double y, double width, double height);
    void addCircle(double x, double y, double radius);
    void addPolygon(vector<Coordinate> points);
    void createMsh();
    void saveMsh(bool& success, QString& filePath);
    bool loadMsh(QString& filePath);
    void addUniformLoad(double startX, double startY, double endX, double endY, double xDirection, double yDirection);
    void addPointForce(double x, double y, double xForce, double yForce);
    void saveConstraint(bool& success, QString& filePath);
    void loadConstraint(bool& success, QString& filePath);
    void saveMatrixes();
private:
    struct ComparePair {
        bool operator()(const std::pair<double, double>& a, const std::pair<double, double>& b) const {
            return a.first > b.first; // ����pair��firstֵ��С��������
        }
    };
    struct pair_hash {
        inline std::size_t operator()(const std::pair<int, int>& v) const {
            return v.first * 31 + v.second;
        }
    };
};