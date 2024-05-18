#pragma once

#include<Eigen/Dense>

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

struct Force {
    double x;
    double y;
    double xForce;
    double yForce;
};

struct Point {
    double x;
    double y;
};

struct MechanicBehavior{
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
