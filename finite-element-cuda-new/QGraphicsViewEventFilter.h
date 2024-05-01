#include <QObject>
#include <QEvent>
#include <QGraphicsView>
#include <QMouseEvent>

class GraphicsViewEventFilter : public QObject {
    Q_OBJECT
public:
    GraphicsViewEventFilter(QObject *parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (watched->inherits("QGraphicsView")) {
                auto *view = static_cast<QGraphicsView *>(watched);
                QPointF scenePoint = view->mapToScene(mouseEvent->pos());
                emit clicked(scenePoint);
                return true; // 返回true表示事件已被处理
            }
        }
        return QObject::eventFilter(watched, event); // 其他事件交给基类处理
    }

signals:
    void clicked(QPointF point);
};
