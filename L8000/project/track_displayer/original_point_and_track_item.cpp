#include "original_point_and_track_item.h"
#include <QCursor>
#include <QDebug>
#include "global_functions.h"

TrackInfoDisplayed g_track_info_displayed;
QReadWriteLock g_read_write_lock_for_g_track_info_displayed;

QMutex g_mutex_for_g_original_point_item_list_and_g_target_track_item_list;
QList<OriginalPointItem*> g_original_point_item_to_add_list;
QList<OriginalPointItem*> g_original_point_item_to_delete_list;
QList<TargetTrackItem*> g_target_track_item_to_add_list;
QList<TargetTrackItem*> g_target_track_item_to_delete_list;
QList<TargetTrackItem*> g_target_track_item_to_update_list;

OriginalPointItem::OriginalPointItem(const TargetOriginalPoint& _original_point, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    original_point = _original_point;
    origin_point_pos = QPointF(_original_point.x, -_original_point.y);
    unselected_pen = QPen(QColor(255, 255, 255, 100), ORIGINAL_POINT_UNSELECTED_PEN_WIDTH);
    unselected_pen.setCapStyle(Qt::RoundCap);
    unselected_pen.setCosmetic(true);
    selected_pen = QPen(Qt::yellow, ORIGINAL_POINT_SELECTED_PEN_WIDTH);
    selected_pen.setCapStyle(Qt::RoundCap);
    selected_pen.setCosmetic(true);
    is_selected = false;
    current_pen = &unselected_pen;

    setZValue(1);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setCursor(Qt::PointingHandCursor);
}

OriginalPointItem::~OriginalPointItem()
{
    if (is_selected)
    {
        QWriteLocker locker(&g_read_write_lock_for_g_track_info_displayed);
        initializeTrackInfoDisplayed(g_track_info_displayed);
    }
}

QPainterPath OriginalPointItem::shape() const
{
    QPainterPath path;
    path.addEllipse(QRectF(
        origin_point_pos.x() - ORIGINAL_POINT_ITEM_RADIUS, origin_point_pos.y() - ORIGINAL_POINT_ITEM_RADIUS, ORIGINAL_POINT_ITEM_RADIUS * 2, ORIGINAL_POINT_ITEM_RADIUS * 2));
    return path;
}

QRectF OriginalPointItem::boundingRect() const
{
    return shape().boundingRect().adjusted(
        -ORIGINAL_POINT_SELECTED_PEN_WIDTH, -ORIGINAL_POINT_SELECTED_PEN_WIDTH, ORIGINAL_POINT_SELECTED_PEN_WIDTH, ORIGINAL_POINT_SELECTED_PEN_WIDTH);
}

void OriginalPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(*current_pen);
    painter->drawPoint(origin_point_pos);
}

QVariant OriginalPointItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        is_selected = value.toBool();
        handleSelectedStateChanged(is_selected);
    }
    return value;
}

void OriginalPointItem::handleSelectedStateChanged(bool _is_selected)
{
    if (_is_selected)
    {
        current_pen = &selected_pen;
    }
    else
    {
        current_pen = &unselected_pen;
    }
    updateTargetInfoOnView(_is_selected);
}

void OriginalPointItem::updateTargetInfoOnView(bool _is_selected)
{
    QWriteLocker locker(&g_read_write_lock_for_g_track_info_displayed);
    initializeTrackInfoDisplayed(g_track_info_displayed);
    if (_is_selected)
    {
        g_track_info_displayed.radar_mode = original_point.radar_mode;
        g_track_info_displayed.range = original_point.range;
        g_track_info_displayed.azim_deg = original_point.azim_deg;
        g_track_info_displayed.elev_deg = original_point.elev_deg;
        g_track_info_displayed.velocity = original_point.vr;
        g_track_info_displayed.amp = original_point.amp_sigma;
        g_track_info_displayed.snr = original_point.snr;
        g_track_info_displayed.rcs = original_point.rcs;
        g_track_info_displayed.x = original_point.x;
        g_track_info_displayed.y = original_point.y;
        g_track_info_displayed.z = original_point.z;
        g_track_info_displayed.ve = original_point.vx;
        g_track_info_displayed.vn = original_point.vy;
        g_track_info_displayed.vz = original_point.vz;
    }
    g_track_info_displayed.is_visible = _is_selected;
}

TargetTrackItem::TargetTrackItem(TargetTrack* _target_track, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    target_track_last_point = _target_track->target_track_points_list.last();
    is_selected = false;
    unselected_pen_track_line = QPen(Qt::gray, TRACK_LINE_UNSELECTED_LINE_WIDTH);
    unselected_pen_track_line.setCosmetic(true);
    unselected_pen_last_point = QPen(Qt::gray, TRACK_LAST_POINT_UNSELECTED_WIDTH);
    unselected_pen_last_point.setCapStyle(Qt::RoundCap);
    unselected_pen_last_point.setCosmetic(true);
    unselected_pen_track_line_5 = QPen(Qt::yellow, TRACK_LINE_5_UNSELECTED_LINE_WIDTH);
    unselected_pen_track_line_5.setCosmetic(true);
    unselected_pen_last_point_5 = QPen(Qt::yellow, TRACK_LAST_POINT_UNSELECTED_WIDTH);
    unselected_pen_last_point_5.setCapStyle(Qt::RoundCap);
    unselected_pen_last_point_5.setCosmetic(true);
    selected_pen_track_line = QPen(Qt::red, TRACK_LINE_SELECTED_LINE_WIDTH);
    selected_pen_track_line.setCosmetic(true);
    selected_pen_last_point = QPen(Qt::red, TRACK_LAST_POINT_SELECTED_WIDTH);
    selected_pen_last_point.setCapStyle(Qt::RoundCap);
    selected_pen_last_point.setCosmetic(true);

    // 初始化航迹线
    switch (_target_track->target_track_points_list.last().established_mode)
    {
    case 2:
        current_pen_track_line = &unselected_pen_track_line_5;
        current_pen_last_point = &unselected_pen_track_line_5;
        break;

    default:
        current_pen_track_line = &unselected_pen_last_point;
        current_pen_last_point = &unselected_pen_last_point;
        break;
    }

    last_point = QPointF(_target_track->target_track_points_list.first().x, -(_target_track->target_track_points_list.first().y));
    track_line_path = QPainterPath(last_point);
    for (int ii = 1; ii < _target_track->target_track_points_list.size(); ++ii)
    {
        last_point = QPointF(_target_track->target_track_points_list.at(ii).x, -(_target_track->target_track_points_list.at(ii).y));
        track_line_path.lineTo(last_point);
    }

    setZValue(2); // 将航迹放在点迹上一层
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setCursor(Qt::PointingHandCursor);
    setVisible(false);
}

TargetTrackItem::~TargetTrackItem()
{
    if (is_selected)
    {
        QWriteLocker locker(&g_read_write_lock_for_g_track_info_displayed);
        initializeTrackInfoDisplayed(g_track_info_displayed);
    }
}

void TargetTrackItem::updateTargetInfoOnView(bool _is_selected)
{
    QWriteLocker locker(&g_read_write_lock_for_g_track_info_displayed);
    initializeTrackInfoDisplayed(g_track_info_displayed);
    if (_is_selected)
    {
        g_track_info_displayed.track_no = target_track_last_point.track_id;
        g_track_info_displayed.radar_mode = target_track_last_point.radar_mode;
        g_track_info_displayed.range = target_track_last_point.range;
        g_track_info_displayed.azim_deg = target_track_last_point.azim_deg;
        g_track_info_displayed.elev_deg = target_track_last_point.elev_deg;
        g_track_info_displayed.velocity = target_track_last_point.vr;
        g_track_info_displayed.amp = target_track_last_point.amp_sigma;
        g_track_info_displayed.snr = target_track_last_point.snr;
        g_track_info_displayed.rcs = target_track_last_point.rcs;
        g_track_info_displayed.track_times = target_track_last_point.track_times;
        g_track_info_displayed.lost_times = target_track_last_point.lost_times;
        g_track_info_displayed.x = target_track_last_point.x;
        g_track_info_displayed.y = target_track_last_point.y;
        g_track_info_displayed.z = target_track_last_point.z;
        g_track_info_displayed.ve = target_track_last_point.vx;
        g_track_info_displayed.vn = target_track_last_point.vy;
        g_track_info_displayed.vz = target_track_last_point.vz;
        g_track_info_displayed.longitude = target_track_last_point.longitude;
        g_track_info_displayed.latitude = target_track_last_point.latitude;
        g_track_info_displayed.altitude = target_track_last_point.altitude;
    }
    g_track_info_displayed.is_visible = _is_selected;
}

void TargetTrackItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(*current_pen_track_line);
    painter->drawPath(track_line_path);
    painter->setPen(*current_pen_last_point);
    painter->drawPoint(last_point);
}

QPainterPath TargetTrackItem::shape() const
{
    QPainterPath path = track_line_path;
    path.addEllipse(
        QRectF(last_point.x() - TRACK_ITEM_LAST_POINT_RADIUS, last_point.y() - TRACK_ITEM_LAST_POINT_RADIUS, TRACK_ITEM_LAST_POINT_RADIUS * 2, TRACK_ITEM_LAST_POINT_RADIUS * 2));
    return path;
}

QRectF TargetTrackItem::boundingRect() const
{
    return shape().boundingRect().adjusted(-TRACK_LAST_POINT_SELECTED_WIDTH, -TRACK_LAST_POINT_SELECTED_WIDTH, TRACK_LAST_POINT_SELECTED_WIDTH, TRACK_LAST_POINT_SELECTED_WIDTH);
}

void TargetTrackItem::handleSelectedStateChanged(bool _is_selected)
{
    if (_is_selected)
    {
        current_pen_track_line = &selected_pen_track_line;
        current_pen_last_point = &selected_pen_last_point;
    }
    else
    {
        switch (target_track_last_point.established_mode)
        {
        case 2:
            current_pen_track_line = &unselected_pen_track_line_5;
            current_pen_last_point = &unselected_pen_last_point_5;
            break;

        default:
            current_pen_track_line = &unselected_pen_track_line;
            current_pen_last_point = &unselected_pen_last_point;
            break;
        }
    }
    updateTargetInfoOnView(_is_selected);
}

QVariant TargetTrackItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        is_selected = value.toBool();
        handleSelectedStateChanged(is_selected);
    }
    return value;
}

void TargetTrackItem::updateTargetTrackItemAsTrackUpdated()
{
    prepareGeometryChange(); // [Important]: 由于形状可能改变, 一定要加这个函数！

    // 控制外推点的连接数量
    if (target_track_last_point.lost_times < 1)
    {
        last_point = QPointF(target_track_last_point.x, -(target_track_last_point.y));
        track_line_path.lineTo(last_point);
    }

    if (target_track_last_point.is_shown)
    {
        switch (target_track_last_point.established_mode)
        {
        case 1:
            setVisible(true);
            if (is_selected)
            {
                updateTargetInfoOnView(is_selected);
                current_pen_track_line = &selected_pen_track_line;
                current_pen_last_point = &selected_pen_last_point;
            }
            else
            {
                current_pen_track_line = &unselected_pen_track_line;
                current_pen_last_point = &unselected_pen_last_point;
            }
            break;

        case 2:
            setVisible(true);
            if (is_selected)
            {
                updateTargetInfoOnView(is_selected);
                current_pen_track_line = &selected_pen_track_line;
                current_pen_last_point = &selected_pen_last_point;
            }
            else
            {
                current_pen_track_line = &unselected_pen_track_line_5;
                current_pen_last_point = &unselected_pen_last_point_5;
            }
            break;
        default:
            setVisible(false);
            current_pen_track_line = &unselected_pen_track_line;
            current_pen_last_point = &unselected_pen_last_point;
            break;
        }
    }
    else
    {
        setVisible(false);
        current_pen_track_line = &unselected_pen_track_line;
        current_pen_last_point = &unselected_pen_last_point;
    }
}

void TargetTrackItem::updateTargetTrackLastPoint(TargetTrack* m_target_track)
{
    target_track_last_point = m_target_track->target_track_points_list.last();
}
