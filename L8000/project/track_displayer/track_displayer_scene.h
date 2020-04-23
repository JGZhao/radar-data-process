#ifndef TRACK_DISPLAYER_SCENE_H
#define TRACK_DISPLAYER_SCENE_H

#include <QGraphicsScene>
#include "global_settings.h"
#include "my_track_info_item.h"

// 界面显示相关
const double MAX_TRACK_DISPLAYER_RADIUS = 20e3; // 场景最大半径
const int SCAN_LINE_UPDATE_INTERVAL = 50;       // 扫描线更新时间 (ms)

class TrackDisplayerScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit TrackDisplayerScene(QObject* parent = nullptr);

public:
    void addOrUpdateCircles();            // 增加同心圆
    void addOrUpdateAngleDialLines();     // 增加角刻度线
    void addScanLine();                   // 增加扫描线
    void addOrUpdatePowerOffSectorItem(); // 添加静默扇形
    void addMap();                        // 增加地图
    void addTrackInfoItem();              // 增加航迹信息

public slots:
    void sltUpdateCirclesAndAngleDialLinesAndPowerOffSectorItem(const SoftwareSettings& new_software_settings);
    void sltUpdatePowerOffSectorItem(const HardwareSettings& new_hardware_settings);
    void sltUpdateScanLineBasedOnCurrentRadarAzim(const double& current_azim);
    void sltUpdateTrackInfoItem();

private:
    double display_area_radius;
    double display_circles_gap;
    double true_radius;
    int circles_num;
    double beam_width;
    bool enable_power_off_area_flag;
    double power_off_azim_deg_floor;
    double power_off_azim_deg_ceil;
    QGraphicsItemGroup* circles_group;
    QGraphicsItemGroup* angle_dial_line_group;
    QGraphicsPolygonItem* scan_line;
    MyTrackInfoItem* track_info_item;
    QGraphicsEllipseItem* power_off_sector_item;
};

#endif // TRACK_DISPLAYER_SCENE_H
