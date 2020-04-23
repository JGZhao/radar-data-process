#include "my_track_info_item.h"
#include <QPainter>
#include <QBrush>
#include <QFont>

MyTrackInfoItem::MyTrackInfoItem(QGraphicsItem *parent) : QGraphicsSimpleTextItem(parent)
{
    QBrush track_info_item_brush = QBrush(QColor(255, 255, 255, 255));
    QFont track_info_item_font("Times", 20, QFont::Bold);
    setFont(track_info_item_font);
    setBrush(track_info_item_brush);
}

MyTrackInfoItem::~MyTrackInfoItem()
{
}

void MyTrackInfoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    double scale_factor = 1.0 / painter->matrix().m11();
    painter->scale(scale_factor, scale_factor);
    QGraphicsSimpleTextItem::paint(painter, option, widget);
}
