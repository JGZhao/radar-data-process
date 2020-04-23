#include "my_graphics_view.h"
#include <QPoint>
#include <QWheelEvent>
#include <QScrollBar>
#include "global_definitions.h"
#include "radar_data_processing_module.h"
//#ifndef QT_NO_OPENGL
//#include <QGLWidget>
//#else
//#include <QtWidgets>
//#endif

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(ScrollHandDrag); // 允许鼠标拖动
    setCacheMode(QGraphicsView::CacheBackground);
//    #ifndef QT_NO_OPENGL
//    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
//    #endif
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    // 参考 https://blog.csdn.net/GoForwardToStep/article/details/77035287
    // 获取当前鼠标相对于 view 的位置
    QPointF cursor_point = event->pos();
    // 获取当前鼠标相对于 scene 的位置
    QPointF scene_pos = this->mapToScene(QPoint(cursor_point.x(), cursor_point.y()));

    // 获取当前鼠标位置相当于 view 大小的横纵比例
    qreal h_scale = cursor_point.x() / view_width;
    qreal v_scale = cursor_point.y() / view_height;

    // 当前放缩倍数
    qreal scale_factor_x = this->matrix().m11();
    qreal scale_factor_y = this->matrix().m22();
    int wheel_delta_value = event->delta();

    // 向上滚动, 放大, 需要注意, 缩放过大会出错, 所以, 控制缩放的上限和下限为 0.02 ~ 5
    if (wheel_delta_value > 0)
    {
        if (scale_factor_x > 5 || scale_factor_y > 5)
        {
            return;
        }
        else
        {
            this->scale(1.2, 1.2);
        }
    }
    // 向下滚动, 缩小
    else
    {
        if (scale_factor_x < 0.02 || scale_factor_y < 0.02)
        {
            return;
        }
        else
        {
            this->scale(1.0 / 1.2, 1.0 / 1.2);
        }
    }

    // 将 scene 坐标转换为放大缩小后的坐标
    QPointF view_point = this->matrix().map(scene_pos);
    // 通过滚动条控制
    horizontalScrollBar()->setValue(int(view_point.x() - view_width * h_scale));
    verticalScrollBar()->setValue(int(view_point.y() - view_height * v_scale));
}

void MyGraphicsView::resizeEvent(QResizeEvent *event)
{
    // 更新场景的宽和高
    view_width = this->viewport()->width();
    view_height = this->viewport()->height();
    QGraphicsView::resizeEvent(event); // 只有加上这一句, setResizeAnchor(QGraphicsView::AnchorViewCenter) 才有用
}

void MyGraphicsView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
        centerOn(0, 0);
        break;
    default:
        event->ignore();
        break;
    }
}
