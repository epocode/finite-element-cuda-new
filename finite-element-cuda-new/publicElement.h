#pragma once

#include<Eigen/Dense>
#include <QColor>
using namespace std;

struct Circle{
    double x;
    double y;
    double radius;
};

struct Coordinate{
    double x;
    double y;
};
struct ColorPoint {
    double position;
    QColor color;
};
struct Force {
    double x;
    double y;
    double xForce;
    double yForce;
};

class Point {
public:
    double x;
    double y;
    Point() {
        this->x = 0;
        this -> y = 0;
    }
    Point(double x, double y) {
        this->x = x;
        this->y = y;
    }
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};
namespace std {
    template <>
    struct hash<Point> {
        size_t operator()(const Point& p) const {
            return hash<double>()(p.x) ^ (hash<double>()(p.y) << 1); // 调用Point类的hash成员函数
        }
    };
}
struct MechanicBehavior{
    int index1;
    int index2;
    int index3;
    Point p1;
    Point p2;
    Point p3;
    Eigen::MatrixXd stress;//应力
    Eigen::MatrixXd strain;//应变
    double equalStress;
    double equalStrain;
};

struct EdgeInfo{
    double x;
    double y;
    bool xFixed;
    bool yFixed;
};
struct Rect{
    double x;
    double y;
    double width;
    double height;
};
