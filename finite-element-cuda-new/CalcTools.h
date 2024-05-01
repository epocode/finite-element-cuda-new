#pragma once
#include <unordered_map>
#include "publicElement.h"

bool calcKMatrix();
void calcABC(std::unordered_map<int, double> & a, std::unordered_map<int, double> & b, std::unordered_map<int, double> & c, int i, int j, int k);
double calcSquare(int i, int j, int k);
void calcFMatrix();
bool isInTriangle(Point p1,Point p2,Point p3,Point o);
double getArea(Point p1, Point p2, Point p3);
double getDist(Point p1,Point p2);
void setEdge(double x, double y, bool xFixed, bool yFixed);
void calcStressStrain();
void getExtreme(double &min, double &max);
void handleEdge();
bool getPointInfos(double x, double y, MechanicBehavior &mechanicBehavior, double &u, double &v);
