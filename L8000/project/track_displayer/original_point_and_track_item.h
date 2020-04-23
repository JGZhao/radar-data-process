#ifndef ORIGINAL_POINT_AND_TRACK_ITEM_H
#define ORIGINAL_POINT_AND_TRACK_ITEM_H

#include <QGraphicsPathItem>
#include <QPainter>
#include <QPen>
#include <QReadWriteLock>
#include <QMutex>
#include "radar_data_processing_module.h"

// 界面显示相关
const double ORIGINAL_POINT_ITEM_RADIUS = 10;   // 原始点半径
const double TRACK_ITEM_LAST_POINT_RADIUS = 50; // 航迹最新点的半径

// 绘图相关
const int ORIGINAL_POINT_UNSELECTED_PEN_WIDTH = 2;
const int ORIGINAL_POINT_SELECTED_PEN_WIDTH = 2;

const int TRACK_LINE_UNSELECTED_LINE_WIDTH = 6;
const int TRACK_LINE_5_UNSELECTED_LINE_WIDTH = 6;
const int TRACK_LINE_SELECTED_LINE_WIDTH = 8;
const int TRACK_LAST_POINT_UNSELECTED_WIDTH = 16;
const int TRACK_LAST_POINT_SELECTED_WIDTH = 16;

extern TrackInfoDisplayed g_track_info_displayed;
extern QReadWriteLock g_read_write_lock_for_g_track_info_displayed;

class OriginalPointItem : public QGraphicsItem
{
public:
    explicit OriginalPointItem(const TargetOriginalPoint& _original_point, QGraphicsItem* parent = nullptr);
    ~OriginalPointItem() override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void handleSelectedStateChanged(bool is_selected);
    void updateTargetInfoOnView(bool _is_selected);

private:
    bool is_selected;
    QPen unselected_pen; // 未选中时的画笔
    QPen selected_pen;   // 选中时的画笔
    QPen* current_pen;
    QPointF origin_point_pos;
    TargetOriginalPoint original_point;
};

class TargetTrackItem : public QGraphicsItem
{
public:
    explicit TargetTrackItem(TargetTrack* m_target_track, QGraphicsItem* parent = nullptr);
    ~TargetTrackItem() override;
    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void updateTargetTrackItemAsTrackUpdated();
    void updateTargetTrackLastPoint(TargetTrack* m_target_track);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void updateTargetInfoOnView(bool _is_selected);
    void handleSelectedStateChanged(bool _is_selected);

private:
    bool is_selected;
    QPen* current_pen_track_line;             // 当前航迹的画笔
    QPen* current_pen_last_point;             // 当前航迹最新点的画笔
    QPen unselected_pen_track_line;           // 未选中时的未起批的画笔
    QPen unselected_pen_last_point;           // 未选中时的未起批的航迹最新点迹的画笔
    QPen unselected_pen_track_line_5;         // 未选中时的起批的画笔
    QPen unselected_pen_last_point_5;         // 未选中时的起批的航迹最新点迹的画笔
    QPen selected_pen_track_line;             // 选中时的画笔
    QPen selected_pen_last_point;             // 选中时的航迹最新点迹的画笔
    QPainterPath track_line_path;             // 航迹线条
    TargetTrackPoint target_track_last_point; // 航迹最新点
    QPointF last_point;                       // 上一个航迹最新点的位置
};

extern QList<OriginalPointItem*> g_original_point_item_to_add_list;
extern QList<OriginalPointItem*> g_original_point_item_to_delete_list;
extern QMutex g_mutex_for_g_original_point_item_list_and_g_target_track_item_list;
extern QList<TargetTrackItem*> g_target_track_item_to_add_list;
extern QList<TargetTrackItem*> g_target_track_item_to_delete_list;
extern QList<TargetTrackItem*> g_target_track_item_to_update_list;

#endif // ORIGINAL_POINT_AND_TRACK_ITEM_H
