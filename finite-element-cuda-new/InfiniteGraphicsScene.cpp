#include "InfiniteGraphicsScene.h"
#include <QVarLengthArray>
#include <QPainter>
InfiniteGraphicsScene::InfiniteGraphicsScene(QObject* parent, QPen pen): QGraphicsScene(parent)
{
    this->myPen = pen;
}
void InfiniteGraphicsScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    const int gridInterval = 1;

    // 计算绘制网格的起始和结束点
    qreal left = int(rect.left()) - (int(rect.left()) % gridInterval);
    qreal top = int(rect.top()) - (int(rect.top()) % gridInterval);

    // 绘制网格线
    QVarLengthArray<QLineF, 100> lines;
    for (qreal x = left; x < rect.right(); x += gridInterval)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += gridInterval)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    // 设置网格颜色和绘制
    myPen.setColor(Qt::gray);
    painter->setPen(myPen);
    painter->drawLines(lines.data(), lines.size());
    myPen.setColor(Qt::black);
    painter->setPen(myPen);
    painter->drawLine(QPointF(rect.left(), 0), QPointF(rect.right(), 0)); // X轴
    painter->drawLine(QPointF(0, rect.top()), QPointF(0, rect.bottom())); // Y轴
}
