#include "Controller.h"
#include "MshInformation.h"
#include "CalcTools.h"
extern MshInformation mshInfo;

Controller::Controller(QObject *parent)
	: QObject(parent)
{}

Controller::~Controller()
{}

void Controller::addRectToMsh(double x, double y, double width, double height)
{
	mshInfo.addRect(x, y, width, height);
}



void Controller::addCircleToMsh(double x, double y, double radius)
{
	mshInfo.addCircle(x, y, radius);
}

void Controller::addPolygonToMsh(vector<Coordinate> points)
{
	mshInfo.addPolygon(points);
}

void Controller::generateMsh()
{
	mshInfo.createMsh();
}

void Controller::saveMsh(bool& success, QString& filePath)
{
	mshInfo.saveMsh(success, filePath);
}

bool Controller::loadMsh(QString &filePath)
{
	return mshInfo.loadMsh(filePath);
}

void Controller::addUniformLoad(double startX, double startY, double endX, double endY, double xDirection, double yDirection, vector<Force> &forces)
{
	mshInfo.addUniformLoad(startX, startY, endX, endY, xDirection, yDirection, forces);
}

void Controller::addConcentratedForce(double x, double y, double xForce, double yForce)
{
	mshInfo.addPointForce(x, y, xForce, yForce);
}

void Controller::addEdges(double x, double y, bool xFixed, bool yFixed)
{
	CalcTools::setEdge(x, y, xFixed, yFixed);
}

void Controller::saveConstraint(bool &success, QString &filePath)
{
	mshInfo.saveConstraint(success, filePath);
}

void Controller::loadConstraint(bool& success, QString& filePath)
{
	mshInfo.loadConstraint(success, filePath);
}

void Controller::generateMatrixes(double E, double v, double t)
{
	CalcTools::generateMatrixes(E, v, t);
}

void Controller::generateColorMap()
{
	mshInfo.generateColorMap();
}

double Controller::getColorValue(double normalizedStressValue)
{
	return mshInfo.getColorValue(normalizedStressValue);
}

