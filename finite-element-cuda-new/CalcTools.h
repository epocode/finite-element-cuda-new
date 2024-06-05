#pragma once
#include <unordered_map>
#include "publicElement.h"
#include <QColor>
#include <map>
namespace CalcTools{
	bool calcKMatrix();
	void calcABC(std::unordered_map<int, double>& a, std::unordered_map<int, double>& b, std::unordered_map<int, double>& c, int i, int j, int k);
	double calcSquare(int i, int j, int k);
	void calcFMatrix();
	bool isInTriangle(Point p1,Point p2,Point p3,Point o);
	bool isInTriangle(double px, double py, double ax, double ay, double bx, double by, double cx, double cy);
	double getArea(Point p1, Point p2, Point p3);
	double getDist(Point p1,Point p2);
	void setEdge(double x, double y, bool xFixed, bool yFixed);
	void calcStressStrain();
	void getExtreme(double& min, double& max);
	void handleEdge();
	bool getPointInfos(double x, double y, MechanicBehavior& mechanicBehavior, double& u, double& v);
	void getUV(double x, double y, MechanicBehavior mechanicBehavoir, double& uValue, double &vValue);
	void evenDistribute(vector<double> valueList, vector<double>& rangeList, map<double, double>& colorMap);
	double getMappedValue(double value, const vector<double>& rangeList, map<double, double>& colorMap);
	QColor getColor(double value);
	double distance(double x1, double y1, double x2, double y2);
	double pointToSegmentDistance(double ax, double ay, double bx, double by, double cx, double cy);
	void generateMatrixes(double E, double v, double t);
}

