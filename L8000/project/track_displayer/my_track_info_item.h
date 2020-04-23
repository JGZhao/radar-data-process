#ifndef MY_TRACK_INFO_ITEM_H
#define MY_TRACK_INFO_ITEM_H

// 航迹信息 Item

#include <QGraphicsSimpleTextItem>

class MyTrackInfoItem : public QGraphicsSimpleTextItem
{
public:
    explicit MyTrackInfoItem(QGraphicsItem *parent = nullptr);
    ~MyTrackInfoItem() override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

#endif // MY_TRACK_INFO_ITEM_H
