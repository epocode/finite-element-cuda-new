#pragma once
#include <QObject>
#include <vector>
#include <publicElement.h>
class Controller  : public QObject
{
	Q_OBJECT

public:
	Controller(QObject *parent);
	~Controller();
public:
	//定义静态方法用于处理请求
	static void addRectToMsh(double x, double y, double width, double height);
	static void addCircleToMsh(double x, double y, double radius);
	static void addPolygonToMsh(vector<Coordinate> points);
	static void generateMsh();
	static void saveMsh(bool& success, QString& filePath);
	static bool loadMsh(QString &filePath);
	static void addUniformLoad(double startX, double startY, double endX, double endY, double xDirection, double yDirection);
	static void addConcentratedForce(double x, double y, double xForce, double yForce);
	static void addEdges(double x, double y, bool xFixed, bool yFixed);
	static void saveConstraint(bool& success, QString& filePath);
	static void loadConstraint(bool& success, QString& filePath);
	static void generateMatrixes(double E, double v, double t);
};
