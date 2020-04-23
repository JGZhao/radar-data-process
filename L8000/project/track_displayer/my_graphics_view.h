#ifndef MY_GRAPHICS_VIEW_H
#define MY_GRAPHICS_VIEW_H

#include <QGraphicsView>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyGraphicsView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    double view_width;
    double view_height;
};

#endif // MY_GRAPHICS_VIEW_H
