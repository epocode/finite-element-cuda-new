#include "UniformForceGraphicsItem.h"
#include <QPainter>
UniformForceGraphicsItem::UniformForceGraphicsItem(QGraphicsItem* parent, double startX, double startY, double endX, double endY, double xForce, double yForce, QPen pen)
{
    this->startX = startX;
    this->startY = startY;
    this->endX = endX;
    this->endY = endY;
    this->xForce = xForce;
    this->yForce = yForce;
    this->myPen = pen;
    setScale(0.5);
    myPen.setWidth(0.5);
    myPen.setColor(Qt::red);
    setTransformOriginPoint(QPointF(0, 2));
    setPos(startX, startY - 2);

    double EntityRadians = std::atan2(endY - startY, endX - startX);
    double theta_degrees = EntityRadians * (180.0 / M_PI) + 360;
    entityDegree = theta_degrees -  (int)(theta_degrees / 360) * 360;
    setRotation(theta_degrees);

    this->length = sqrt(pow(endX - startX, 2) + pow(endY - startY, 2));
    setZValue(1);
}

UniformForceGraphicsItem::~UniformForceGraphicsItem()
{
 
}

QRectF UniformForceGraphicsItem::boundingRect() const
{
    return QRectF(0, 0, this->length, 2);
}

void rotatePoint(double &x, double &y, double a, double b, double degree) {
    // 将角度转换为弧度
    double radian = degree * M_PI / 180.0;

    // 计算旋转后的坐标
    double new_x = cos(radian) * (x - a) - sin(radian) * (y - b) + a;
    double new_y = sin(radian) * (x - a) + cos(radian) * (y - b) + b;
    x = new_x;
    y = new_y;
}

struct Point {
    double x;
    double y;
    Point(double x, double y) {
        this->x = x;
        this->y = y;
    }
};

void UniformForceGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(myPen);
    int count = length / 2;
    QVector<Point> pointList;
    Point pTop = Point(1, 2);
    pointList.push_back(pTop);
    Point pLeft = Point(0, 1);
    pointList.push_back(pLeft);
    Point pRight = Point(2, 1);
    pointList.push_back(pRight);
    Point pButtom = Point(1, 0);
    pointList.push_back(pButtom);
    Point pCenter = Point(1, 1);
    double theta_radians = std::atan2(yForce, xForce);
    double theta_degrees = theta_radians * (180.0 / M_PI);
    theta_degrees = theta_degrees + 270 - entityDegree;
    for (Point &point : pointList) {
        rotatePoint(point.x, point.y, pCenter.x, pCenter.y, theta_degrees);
    }
    //pTop = pointList[0];
    //pLeft = pointList[1];
    //pRight = pointList[2];
    //pButtom = pointList[3];

    for (int i = 0; i < count; i++) {
        painter->drawLine(pTop.x + i * 2, pTop.y, pButtom.x + i * 2, pButtom.y); // 箭身
        painter->drawLine(pTop.x + i * 2, pTop.y, pLeft.x + i * 2, pLeft.y); // 箭头左半部
        painter->drawLine(pTop.x + i * 2, pTop.y, pRight.x + i * 2, pRight.y);
    }
}
