#include <QGraphicsPolygonItem>
#include "track_displayer_scene.h"
#include "global_definitions.h"
#include "global_functions.h"
#include "original_point_and_track_item.h"

/*
 * Note: QGraphicsScene 的纵轴是正方向朝下, 笛卡尔坐标系的所有 (x, y) 需要取反
 */

TrackDisplayerScene::TrackDisplayerScene(QObject* parent) : QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setBackgroundBrush(QColor(0, 0, 0, 255));
    double scene_height = 2 * MAX_TRACK_DISPLAYER_RADIUS;
    double width_height_ratio = 1;
    double scene_width = scene_height * width_height_ratio;
    setSceneRect(-scene_width / 2, -scene_height / 2, scene_width, scene_height);
    display_area_radius = 10e3;
    display_circles_gap = 1e3;
    circles_num = int(display_area_radius / display_circles_gap) + 1;
    true_radius = circles_num * display_circles_gap;
    circles_group = nullptr;
    angle_dial_line_group = nullptr;

    addOrUpdateCircles();
    addOrUpdateAngleDialLines();

    beam_width = 4;
    addScanLine();
    addTrackInfoItem();

    enable_power_off_area_flag = false;
    power_off_azim_deg_floor = 0;
    power_off_azim_deg_ceil = 0;
    power_off_sector_item = nullptr;
    addOrUpdatePowerOffSectorItem();
}

void TrackDisplayerScene::addOrUpdateCircles()
{
    if (circles_group != nullptr)
    {
        removeItem(circles_group);
        delete circles_group;
        circles_group = nullptr;
    }

    QList<QGraphicsItem*> circle_items_list;
    QPen circles_pen_1km = QPen(Qt::green, 0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QPen circles_pen_3km = QPen(Qt::green, 3, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    circles_pen_3km.setCosmetic(true);

    for (int ii = 1; ii <= circles_num; ++ii)
    {
        if (ii % 3 == 0)
        {
            circle_items_list.append(addEllipse(QRectF(-display_circles_gap * ii, -display_circles_gap * ii, display_circles_gap * ii * 2, display_circles_gap * ii * 2),
                                                circles_pen_3km,
                                                QBrush(QColor(255, 255, 255, 0))));
        }
        else
        {
            circle_items_list.append(addEllipse(QRectF(-display_circles_gap * ii, -display_circles_gap * ii, display_circles_gap * ii * 2, display_circles_gap * ii * 2),
                                                circles_pen_1km,
                                                QBrush(QColor(255, 255, 255, 0))));
        }
    }

    circles_group = createItemGroup(circle_items_list);
}

void TrackDisplayerScene::addOrUpdateAngleDialLines()
{
    if (angle_dial_line_group != nullptr)
    {
        removeItem(angle_dial_line_group);
        delete angle_dial_line_group;
        angle_dial_line_group = nullptr;
    }

    QList<QGraphicsItem*> angle_dial_line_items_list;

    double angle_gap = 30;
    int angle_dial_lines_num = int(360 / angle_gap);
    QPointF dial_line_end = QPointF(0, 0);
    QPen dial_line = QPen(Qt::green, 0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    QPen north_line = QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    north_line.setCosmetic(true);

    for (int ii = 0; ii < angle_dial_lines_num; ++ii)
    {
        dial_line_end.setX(true_radius * cos(angle_gap * ii * DEG2RAD));
        dial_line_end.setY(-true_radius * sin(angle_gap * ii * DEG2RAD));
        if (ii == 3)
        {
            angle_dial_line_items_list.append(addLine(QLineF(QPointF(0, 0), dial_line_end), north_line));
        }
        else
        {
            angle_dial_line_items_list.append(addLine(QLineF(QPointF(0, 0), dial_line_end), dial_line));
        }
    }
    angle_dial_line_group = createItemGroup(angle_dial_line_items_list);
}

void TrackDisplayerScene::addScanLine()
{
    QVector<QPointF> scan_line_vertexes;
    scan_line_vertexes.append(QPointF(0, 0));
    scan_line_vertexes.append(QPointF(true_radius * cos(-beam_width / 2 * DEG2RAD), true_radius * sin(-beam_width / 2 * DEG2RAD)));
    scan_line_vertexes.append(QPointF(true_radius * cos(beam_width / 2 * DEG2RAD), true_radius * sin(beam_width / 2 * DEG2RAD)));
    scan_line_vertexes.append(QPointF(0, 0));
    QPen scan_line_pen = QPen(Qt::green, 0, Qt::NoPen, Qt::RoundCap, Qt::RoundJoin);
    QBrush scan_line_brush = QBrush(QColor(0, 255, 0, 100));
    scan_line = addPolygon(QPolygonF(scan_line_vertexes), scan_line_pen, scan_line_brush);
}

void TrackDisplayerScene::addOrUpdatePowerOffSectorItem()
{
    if (power_off_sector_item)
    {
        power_off_sector_item->setRect(QRectF(-true_radius, -true_radius, true_radius * 2, true_radius * 2));
    }
    else
    {
        QBrush power_off_sector_item_brush = QBrush(QColor(255, 0, 0, 100));
        power_off_sector_item = addEllipse(QRectF(-true_radius, -true_radius, true_radius * 2, true_radius * 2));
        power_off_sector_item->setBrush(power_off_sector_item_brush);
        power_off_sector_item->setStartAngle(power_off_azim_deg_floor * 16);
        int span_angle = fmod(power_off_azim_deg_ceil + 360 - power_off_azim_deg_floor, 360) * 16;
        power_off_sector_item->setSpanAngle(span_angle);
        power_off_sector_item->setVisible(enable_power_off_area_flag);
    }
}

void TrackDisplayerScene::addMap()
{
    // TODO
}

void TrackDisplayerScene::addTrackInfoItem()
{
    QReadLocker locker(&g_read_write_lock_for_g_track_info_displayed);

    QString track_info_string =
        QString(" No: " + QString::number(g_track_info_displayed.track_no) + "\n Mode: " + QString::number(g_track_info_displayed.radar_mode) +
                "\n Range: " + QString::number(g_track_info_displayed.range, 'f', 1) + "\n Azim: " + QString::number(g_track_info_displayed.azim_deg, 'f', 1) +
                "\n Elev: " + QString::number(g_track_info_displayed.elev_deg, 'f', 1) + "\n Vr: " + QString::number(g_track_info_displayed.velocity, 'f', 1) +
                "\n Height: " + QString::number(g_track_info_displayed.z, 'f', 1)) +
        "\n Amp: " + QString::number(g_track_info_displayed.amp) + "\n Scr: " + QString::number(g_track_info_displayed.snr) +
        "\n Rcs: " + QString::number(g_track_info_displayed.rcs, 'f', 5);
    track_info_item = new MyTrackInfoItem();
    track_info_item->setText(track_info_string);
    addItem(track_info_item);
    track_info_item->setPos(g_track_info_displayed.x, -g_track_info_displayed.y);
    track_info_item->setZValue(3);
    track_info_item->setVisible(g_track_info_displayed.is_visible);
}

void TrackDisplayerScene::sltUpdateCirclesAndAngleDialLinesAndPowerOffSectorItem(const SoftwareSettings& new_software_settings)
{
    if (fabs(new_software_settings.display_settings.display_area_radius - display_area_radius) > EPS ||
        fabs(new_software_settings.display_settings.display_circles_gap - display_circles_gap) > EPS)
    {
        display_area_radius = new_software_settings.display_settings.display_area_radius;
        display_circles_gap = new_software_settings.display_settings.display_circles_gap;
        circles_num = int(display_area_radius / display_circles_gap) + 1;
        true_radius = circles_num * display_circles_gap;
        addOrUpdateCircles();
        addOrUpdateAngleDialLines();
        addOrUpdatePowerOffSectorItem();
    }
}

void TrackDisplayerScene::sltUpdatePowerOffSectorItem(const HardwareSettings& new_hardware_settings)
{
    if (fabs(circularlyAddDouble(new_hardware_settings.power_off_area_settings.power_off_azim_deg_floor, -power_off_azim_deg_floor, -180, 360)) > EPS ||
        fabs(circularlyAddDouble(new_hardware_settings.power_off_area_settings.power_off_azim_deg_ceil, -power_off_azim_deg_ceil, -180, 360)) > EPS)
    {
        power_off_azim_deg_floor = new_hardware_settings.power_off_area_settings.power_off_azim_deg_floor;
        power_off_azim_deg_ceil = new_hardware_settings.power_off_area_settings.power_off_azim_deg_ceil;
        power_off_sector_item->setStartAngle(new_hardware_settings.power_off_area_settings.power_off_azim_deg_floor * 16);
        int span_angle =
            fmod(new_hardware_settings.power_off_area_settings.power_off_azim_deg_ceil + 360 - new_hardware_settings.power_off_area_settings.power_off_azim_deg_floor, 360) * 16;
        power_off_sector_item->setSpanAngle(span_angle);
    }

    if (new_hardware_settings.power_off_area_settings.enable_power_off_area_flag)
    {
        enable_power_off_area_flag = true;
    }
    else
    {
        enable_power_off_area_flag = false;
    }
    power_off_sector_item->setVisible(enable_power_off_area_flag);
}

void TrackDisplayerScene::sltUpdateScanLineBasedOnCurrentRadarAzim(const double& current_azim)
{
    scan_line->setRotation(current_azim);
}

void TrackDisplayerScene::sltUpdateTrackInfoItem()
{
    QReadLocker locker(&g_read_write_lock_for_g_track_info_displayed);
    QString track_info_string =
        QString(" No: " + QString::number(g_track_info_displayed.track_no) + "\n Mode: " + QString::number(g_track_info_displayed.radar_mode) +
                "\n Range: " + QString::number(g_track_info_displayed.range, 'f', 1) + "\n Azim: " + QString::number(g_track_info_displayed.azim_deg, 'f', 1) +
                "\n Elev: " + QString::number(g_track_info_displayed.elev_deg, 'f', 1) + "\n Vr: " + QString::number(g_track_info_displayed.velocity, 'f', 1) +
                "\n Height: " + QString::number(g_track_info_displayed.z, 'f', 1)) +
        "\n Amp: " + QString::number(g_track_info_displayed.amp) + "\n Scr: " + QString::number(g_track_info_displayed.snr) +
        "\n Rcs: " + QString::number(g_track_info_displayed.rcs, 'f', 5);
    track_info_item->setText(track_info_string);
    track_info_item->setPos(QPointF(g_track_info_displayed.x, -g_track_info_displayed.y));
    track_info_item->setVisible(g_track_info_displayed.is_visible);
}
