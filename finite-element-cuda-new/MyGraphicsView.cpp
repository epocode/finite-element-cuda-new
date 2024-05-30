#include "MyGraphicsView.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QString>
#include "MyGradientLabel.h"
#include <qgraphicsproxywidget.h>
#include <QGroupBox>
#include <QVBoxLayout>
MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    setMode(COMMON);
    // 初始化代码
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setMouseTracking(true);
    setupDragMode();
    this->scale(40, 40);
    this->pen.setWidthF(pen.widthF() / this->transform().m11());
    myScene = new QGraphicsScene;
    this->setScene(myScene);
    this->scale(1, -1);
    //显示坐标值
    coordinateLabel = new QLabel(this);
    coordinateLabel->move(0, 0); // 放置在左上角
    coordinateLabel->setText("X: 0, Y: 0");
    coordinateLabel->setStyleSheet("QLabel { color : black; }");
    coordinateLabel->setFixedSize(100, 30);

    isDrawing = false;
    //颜色条添加（旧）
   /* MyColorBar* colorBar = new MyColorBar(this);
    this->myScene->addItem(colorBar);*/
    //添加颜色条
    gradientBox = new QGroupBox(this);
    QVBoxLayout* layout= new QVBoxLayout();
    maxGradientLabel = new QLabel(gradientBox);
    colorBar = new MyGradientLabel(gradientBox);
    minGradientLabel = new QLabel(gradientBox);
    maxGradientLabel->setStyleSheet("color: black;");
    minGradientLabel->setStyleSheet("color: black;");
    layout->addWidget(maxGradientLabel);
    layout->addWidget(colorBar);
    layout->addWidget(minGradientLabel);
    gradientBox->setLayout(layout);
    gradientBox->move(0, 30);
    gradientBox->hide();
}

void MyGraphicsView::wheelEvent(QWheelEvent* event) {
    const double scaleFactor = 1.15; // 缩放的比例因子
    QPoint scrollAmount = event->angleDelta();
    if (scrollAmount.y() > 0) {
        // 向上滚动鼠标滚轮，放大
        scale(scaleFactor, scaleFactor);
    }
    else {
        // 向下滚动鼠标滚轮，缩小
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}



void MyGraphicsView::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    double x = list.at(0).toDouble();
    double y = list.at(1).toDouble();
    QPointF point(x, y);
    switch (curMode) {
    case CREATELINE: 
        if (points.size() <= 2) {
            points.append(point);
            myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen);
            emit setTipsSignal(QString("设置下一个点的坐标 x y:"));
            if (points.size() > 1) {//绘制线段
                myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
            }
        }
        else {
            if (isCloseToFirstPoint(point)) {
                // 绘制最后一条线段闭合多边形
                myScene->addLine(QLineF(points.last(), points.first()), this->pen);
                setMode(COMMON);
                emit createPolygonSignal(points);
                points.clear();
                tempLine = nullptr;
            }
            else {
                points.append(point);
                myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen);
                myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
            }
        }
    
        break;
    case CREATERECT:
        list = text.split(" ");
        x = list.at(0).toDouble();
        y = list.at(1).toDouble();
        point = QPointF(x, y);
        if (tempRect) {//完成了创建
            myScene->removeItem((QGraphicsItem*)(tempRect));
            delete tempRect;
            tempRect = myScene->addRect(QRectF(rectStartPoint, point), this->pen);
            tempRect = nullptr;
            isDrawing = false;
            setMode(COMMON);
            emit createRectSignal(rectStartPoint, point);
            emit resetInputAreaSignal();
        }
        else {
            isDrawing = true;
            rectStartPoint = point;
            emit setTipsSignal(QString("设置对顶角顶点坐标 x y:"));
        }
        break;
    case CREATECIRCLE: 
        list = text.split(" ");
        x = list.at(0).toDouble();
        y = list.at(1).toDouble();
        point = QPointF(x, y);
        if (!isDrawing) {
            startPoint = point;
            isDrawing = true;
            emit setTipsSignal(QString("设置终点坐标 x y:"));
        }
        else {
            QPointF endPoint = point;
            qreal radius = QLineF(startPoint, endPoint).length();
            if (tempEllipse) {
                myScene->removeItem((QGraphicsItem*)tempEllipse);
                delete tempEllipse;
                myScene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, 2 * radius, 2 * radius, this->pen);
            }
            else {
                tempEllipse = myScene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, 2 * radius, 2 * radius, this->pen);
            }
            isDrawing = false;
            tempEllipse = nullptr;
            emit createCircleSignal(startPoint.x(), startPoint.y(), radius);
            emit resetInputAreaSignal();
        }
        break;
    }
}


void MyGraphicsView::mousePressEvent(QMouseEvent* event)
{
    QPointF point = mapToScene(event->pos());
    switch (curMode) {
    case COMMON:
        if (event->button() == Qt::LeftButton) {
            lastMousePos = event->pos();
        }
        QGraphicsView::mousePressEvent(event);
        break;
    case CREATELINE:
        if (event->button() == Qt::MouseButton::LeftButton) {
            if (points.size() <= 2) {
                points.append(point);
                ellipseItemList.push_back(myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen));
                if (points.size() > 1) {//绘制线段
                    lineItems.push_back(myScene->addLine(QLineF(points[points.size() - 2], point), this->pen));
                }
                QString msg = QString("设置下一个点的坐标 x y : ");
                emit setTipsSignal(msg);
            }
            else {
                if (isCloseToFirstPoint(point)) {
                    // 绘制最后一条线段闭合多边形
                    lineItems.push_back(myScene->addLine(QLineF(points.last(), points.first()), this->pen));
                    setMode(COMMON);
                    emit createPolygonSignal(points);
                    emit resetInputAreaSignal();
                    points.clear();
                    lineItems.clear();
                    ellipseItemList.clear();
                }
                else {
                    points.append(point);
                    ellipseItemList.push_back(myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen));
                    lineItems.push_back(myScene->addLine(QLineF(points[points.size() - 2], point), this->pen));
                }
            }
        }
        break;
    case CREATERECT:
        if (event->button() == Qt::LeftButton) {
            if (tempRect) {//完成了创建
                myScene->removeItem((QGraphicsItem*)(tempRect));
                delete tempRect;
                tempRect = myScene->addRect(QRectF(rectStartPoint, point), this->pen);
                tempRect = nullptr;
                isDrawing = false;
                setMode(COMMON);
                emit createRectSignal(rectStartPoint, point);
                emit resetInputAreaSignal();
            }
            else {
                isDrawing = true;
                rectStartPoint = point;
                // 创建一个临时的矩形来显示预创建的形状
                tempRect = myScene->addRect(QRectF(rectStartPoint, point), this->pen);
                emit setTipsSignal(QString("设置对顶角顶点坐标 x y:"));
            }
        }
        break;
    case CREATECIRCLE:
        if (event->button() == Qt::LeftButton) {
            if (!isDrawing) {
                startPoint = mapToScene(event->pos());
                isDrawing = true;
                emit setTipsSignal(QString("设置终点坐标 x y:"));
            }
            else {
                QPointF endPoint = mapToScene(event->pos());
                qreal radius = QLineF(startPoint, endPoint).length();
                if (tempEllipse) {
                    myScene->removeItem((QGraphicsItem*)tempEllipse);
                    delete tempEllipse;
                    myScene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, 2 * radius, 2 * radius, this->pen);
                }
                else {
                    tempEllipse = myScene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, 2 * radius, 2 * radius, this->pen);
                }
                isDrawing = false;
                tempEllipse = nullptr;
                emit createCircleSignal(startPoint.x(), startPoint.y(), radius);
                emit resetInputAreaSignal();
                setMode(COMMON);
            }
        }
        break;
    }
}



void MyGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = mapToScene(event->pos());
    // 更新坐标显示标签
    coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    //QGraphicsView::mouseMoveEvent(event); // 调用基类的方法
    switch (curMode) {
    case COMMON:
        if (event->buttons() & Qt::LeftButton) {
            QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePos);
            translate(delta.x(), delta.y());
            lastMousePos = event->pos();
        }
        QGraphicsView::mouseMoveEvent(event);
        break;
    case CREATELINE:
        if (!points.isEmpty()) {
            QPointF point = mapToScene(event->pos());
            bool found = isCloseToFirstPoint(point);
            if (found) {
                // 与首个点连接上后
                if (tempLine) {
                    myScene->removeItem((QGraphicsItem*)(tempLine));
                    delete tempLine;
                }
                tempLine = myScene->addLine(QLineF(points.last(), points.first()), this->pen);
            }
            else {
                if (tempLine) {
                    myScene->removeItem((QGraphicsItem*)(tempLine));
                    delete tempLine;
                }
                tempLine = myScene->addLine(QLineF(points.last(), point), this->pen);
            }

        }
        break;
    case CREATERECT: 
        if (isDrawing) {
            if (tempRect) {//完成了创建
                myScene->removeItem((QGraphicsItem*)(tempRect));
                delete tempRect;
                tempRect = myScene->addRect(QRectF(rectStartPoint, point), this->pen);

            }
            else {
                tempRect = new QGraphicsRectItem(QRectF(rectStartPoint, point));
                tempRect = myScene->addRect(QRectF(rectStartPoint, point), this->pen);
            }
        }
        break;
    case CREATECIRCLE:
        if (isDrawing) {
            QPointF endPoint = mapToScene(event->pos());
            qreal radius = QLineF(startPoint, endPoint).length();
            if (tempEllipse) {
                myScene->removeItem((QGraphicsItem*)tempEllipse);
                delete tempEllipse;
                tempEllipse = myScene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, 2 * radius, 2 * radius, this->pen);
            }
            else {
                tempEllipse = myScene->addEllipse(startPoint.x() - radius, startPoint.y() - radius, 2 * radius, 2 * radius, this->pen);
            }
        }
        break;
    }
}


    void MyGraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void MyGraphicsView::setupDragMode()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
}
void MyGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF scenePoint = mapToScene(event->pos());
    emit doubleClicked(scenePoint); // 发送信号
}





void MyGraphicsView::setMode(Mode mode) {
    this->curMode = mode;
}

void MyGraphicsView::setMode(QString mode)
{
    if (mode == QString("COMMON")) {
        setMode(COMMON);
    }
    else if (mode == QString("CREATELINE")) {
        setMode(CREATELINE);
    }
    else if (mode == QString("CREATERECT")) {
        setMode(CREATERECT);
    }
    else if (mode == QString("CREATECIRCLE")) {
        setMode(CREATECIRCLE);
    }
}



bool MyGraphicsView::isCloseToFirstPoint(const QPointF& mousePos)
{
    const double snapDistance = 1; // 吸附距离阈值
    QPointF point = points.first();
    double distance = std::hypot(mousePos.x() - point.x(), mousePos.y() - point.y());
    if (distance < snapDistance) {
        return true;
    }

    return false;
}

void MyGraphicsView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        switch (curMode) {
        case CREATELINE:
            if (points.size() == 0) {
                setMode(COMMON);
                emit resetInputAreaSignal();
                tempLine = nullptr;
            }
            if (points.size() == 1) {
                if (tempLine) {
                    myScene->removeItem((QGraphicsItem*)tempLine);
                    delete tempLine;
                }
                points.removeLast();
                QGraphicsEllipseItem* ellipseItem = ellipseItemList.last();
                ellipseItemList.removeLast();
                myScene->removeItem((QGraphicsItem*)ellipseItem);
                delete ellipseItem;
                setMode(COMMON);
                emit resetInputAreaSignal();
                tempLine = nullptr;
            }
            else {
                if (tempLine) {
                    myScene->removeItem((QGraphicsItem*)tempLine);
                    delete tempLine;
                }
                tempLine = lineItems.last();
                lineItems.removeLast();
                myScene->removeItem((QGraphicsItem*)tempLine);
                delete tempLine;
                QGraphicsEllipseItem* ellipseItem = ellipseItemList.last();
                ellipseItemList.removeLast();
                myScene->removeItem((QGraphicsItem*)ellipseItem);
                delete ellipseItem;
                points.removeLast();
                tempLine = nullptr;
            }
            break;
        case CREATERECT:
            if (tempRect) {//完成了创建
                myScene->removeItem((QGraphicsItem*)(tempRect));
                delete tempRect;
            }
            tempRect = nullptr;
            isDrawing = false;
            setMode(COMMON);
            emit resetInputAreaSignal();
            break;
        case CREATECIRCLE:
            if (tempEllipse) {
                myScene->removeItem((QGraphicsItem*)tempEllipse);
                delete tempEllipse;
            }
            tempEllipse = nullptr;
            isDrawing = false;
            emit resetInputAreaSignal();
            setMode(COMMON);
            break;
        }
    }
    else {
        // 如果按下的不是 ESC 键，则调用基类的方法处理该事件
        QGraphicsView::keyPressEvent(event);
    }
}

void MyGraphicsView::showRenderInfo(double max, double min)
{
    this->maxGradientLabel->setText(QString::number(max, 'e', 2));
    this->minGradientLabel->setText(QString::number(min, 'e', 2));
    this->gradientBox->show();
}

void MyGraphicsView::hideREnderInf()
{
    this->gradientBox->hide();
}

void MyGraphicsView::drawForeground(QPainter* painter, const QRectF& rect)
{
    //Q_UNUSED(rect);

    //// 计算colorbar的位置和尺寸
    //qreal colorBarWidth = 20; // colorbar的宽度
    //qreal colorBarHeight = this->height() - 20; // colorbar的高度，留出一些边距
    //qreal colorBarX = this->viewport()->width() - colorBarWidth - 10; // colorbar的x坐标，靠近视图的右侧
    //qreal colorBarY = 10; // colorbar的y坐标，从视图顶部向下10个单位

    //// 创建渐变
    //QLinearGradient gradient(colorBarX, colorBarY, colorBarX, colorBarY + colorBarHeight);
    //gradient.setColorAt(0.0, Qt::blue); // 渐变起始颜色
    //gradient.setColorAt(1.0, Qt::red); // 渐变结束颜色

    //// 设置画笔和画刷
    //painter->setPen(Qt::NoPen); // 不绘制边框
    //painter->setBrush(gradient); // 设置渐变画刷

    //// 绘制colorbar
    //painter->drawRect(QRectF(colorBarX, colorBarY, colorBarWidth, colorBarHeight));

}
