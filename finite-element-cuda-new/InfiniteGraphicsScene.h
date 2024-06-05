#pragma once
#include <QGraphicsScene>
class InfiniteGraphicsScene :
    public QGraphicsScene
{
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
    InfiniteGraphicsScene(QObject* parent = nullptr, QPen pen = QPen());
    QPen myPen;
protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
};

