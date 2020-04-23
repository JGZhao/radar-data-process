#ifndef RADAR_DATA_PROCESSING_THREAD_H
#define RADAR_DATA_PROCESSING_THREAD_H

#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QSet>
#include <QTimer>
#include "radar_data_processing_module.h"
#include "protocol_with_fpga.h"
#include "original_point_and_track_item.h"

const int UPDATE_RADAR_SYSTEM_STATE_TIMER_INTERVAL = 50;   // 更新其他线程中的雷达系统状态的计时器 (ms)
const int MAX_UPDATE_RADAR_SYSTEM_STATE_TIMER_COUNT = 200; // 更新雷达连接状态、自动配置校准系数、发送复位命令等

class RadarDataProcessingObject : public QObject
{
    Q_OBJECT
public:
    explicit RadarDataProcessingObject(QObject* parent = nullptr);
    ~RadarDataProcessingObject();

private:
    // 软硬件设置
    HardwareSettings local_hardware_settings;
    SoftwareSettings local_software_settings;
    DataProcessingSettings local_data_processing_settings;

    // 方位毛刺平滑相关
    FpgaPacket* current_packet; // 当前报文
    FpgaPacket* next_packet;    // 上一个报文
    int last_packet_azim_dec;
    int current_packet_azim_dec;
    int next_packet_azim_dec;
    int delta_last_current_azim_dec;
    int delta_current_next_azim_dec;

    // 系统状态
    int current_input_azim_sector_index; // 当前方位下标
    int last_azim_input_sector_index;    // 上一个方位下标
    RadarSystemState local_radar_system_state;
    QTimer* update_radar_system_state_in_other_thread_timer;
    bool need_to_update_radar_system_state_in_other_thread_flag;
    unsigned int update_radar_system_state_timer_count;
    bool serdes_error_flag;
    bool none_dbf_coef_in_fpga_flag;

    // 静默控制
    bool enter_power_off_area_flag;

    // 点迹及航迹处理
    bool can_add_original_point_to_input_azim_sector_flag; // 原始点迹能否加入输入扇区（当输入扇区位于融合扇区到航迹起始扇区区间内时不能加入）
    bool original_points_and_tracks_scan_mode_cleared_up_flag;  // 搜索模式的原始点迹和航迹是否清空
    bool original_points_and_tracks_track_mode_cleared_up_flag; // 跟踪模式的原始点迹和航迹是否清空

    // 动态杂波图
    double clutter_map[MAX_RADAR_MODE_NUM][MAX_RANGE_UNIT_NUM][MAX_AZIM_UNIT_NUM][MAX_ELEV_UNIT_NUM] = {{{{0}}}};

    // 增删点航迹
    QList<QList<OriginalPointItem*>> added_original_point_item_list_scan_mode; // 已经添加的原始点迹 item 搜索模式
    QList<OriginalPointItem*>* adding_original_point_item_list_track_mode;     // 正在添加的原始点迹 item 跟踪模式
    QList<OriginalPointItem*>* added_original_point_item_list_track_mode;      // 已经添加的原始点迹 item 跟踪模式
    QHash<TargetTrack*, TargetTrackItem*> target_track_to_item_hashmap;

    // 发送航迹
    QSet<int> sending_target_set;
    QList<TargetTrackPoint>* track_to_send_list;

    // 跟踪模式
    FpgaPacket* current_packet_track_mode;
    bool original_points_batch_track_mode_ready_flag;             // 跟踪模式一轮有多个俯仰波束，一轮结束才进行后续点迹融合、相关等处理
    unsigned int continuous_packet_num_track_mode;                // 连续的跟踪模式数据报文的数量
    TargetTrack* tracking_target_track;                           // 当前跟踪的航迹
    int tracking_target_track_id;                                 // 当前跟踪的航迹id
    bool prepare_tracking_flag;                                   // 准备跟踪标志位，搜索模式基于这个标志位，进入跟踪模式
    bool start_tracking_flag;                                     // 开始跟踪模式标志位，表示已经进入跟踪模式
    bool prepare_to_quit_tracking_flag;                           // 准备退出跟踪标志位
    TrackingTargetPosition last_tracking_target_position;         // 当前跟踪模式进行模式、转台及俯仰控制时的目标位置基准
    HardwareSettingsTrackMode local_hardware_settings_track_mode; // 当前跟踪模式的模式、转台及俯仰控制设置

    // 输出各种信息到文件
    QFile* radar_system_info_txt;
    QTextStream* radar_system_info_text_stream;
    bool is_radar_system_info_txt_opened;
    int current_radar_system_info_row_num;

    QFile* radar_original_points_txt;
    QTextStream* radar_original_points_text_stream;
    bool is_radar_original_points_txt_opened;
    int current_radar_original_points_row_num;

    QFile* radar_track_points_txt;
    QTextStream* radar_track_points_text_stream;
    bool is_radar_track_points_txt_opened;
    int current_radar_track_points_row_num;

public:
    AzimSectorListScanMode azim_sector_list_scan_mode;
    OriginalPointsBatchTrackMode original_points_batch_track_mode;

private:
    // 杂波图
    void updateClutterMapBasedOnNewOriginalPoint(const TargetOriginalPoint& original_point);
    bool cluttermapFilter(TargetOriginalPoint& original_point);
    void updateClutterMap(const int& m_azim_sector_index);

    // 提取目标
    TargetOriginalPoint extractTargetInfoFromFpgaPacket(const TargetInfo& target_info,
                                                        const QDateTime& date_time,
                                                        const int& radar_mode,
                                                        const double& free_time,
                                                        const double& beamcenter_azim_deg,
                                                        const int& azim_index,
                                                        const double& breamcenter_elev_deg,
                                                        const int& elev_index,
                                                        const int& dbf_weight_flag);

    // 搜索模式报文解析
    void initializeDataProcessingAzimSectorIndexScamMode(); // 初始化搜索模式的预相关、搜索、跟踪等各方位扇区
    void initializeParametersForSmoothingAzimScanMode();    // 初始化方位毛刺平滑相关参数
    void smoothAzimInCurrentFpgaPacket();
    bool packetHandlerScanMode(const FpgaPacket* fpga_packet);

    void extractTargetInfoFromFpgaPacketScanMode(const TargetInfo& target_info,
                                                 const QDateTime& date_time,
                                                 const int& azim_sector_index,
                                                 const int& radar_mode,
                                                 const double& free_time,
                                                 const double& beamcenter_azim_deg,
                                                 const int& azim_index,
                                                 const double& breamcenter_elev_deg,
                                                 const int& elev_index,
                                                 const int& dbf_weight_flag);

    void updateRadarSystemState(const FpgaPacket* fpga_packet, RadarSystemState& radar_system_state);

    // 原始点迹及航迹处理
    void clearUpOriginalPointsTrackModeInOriginalPointsBatch();
    void clearUpTracksTrackMode();
    void updateTrackAzimSectorIndexAndTrackItemsAndTrackToSendListAfterTracksUpdated();
    void startTrackAndGenerateNewTrackItemAndUpdateTrackToSendList();

    // 跟踪模式
    void clearUpOriginalPointsScanModeInAzimSectors();
    void clearUpTracksScanModeInAzimSectors();
    bool packetHandlerTrackMode(const FpgaPacket* fpga_packet);
    void extractTargetInfoFromFpgaPacketTrackMode(const TargetInfo& target_info,
                                                  const QDateTime& date_time,
                                                  const int& azim_sector_index,
                                                  const int& radar_mode,
                                                  const double& free_time,
                                                  const double& beamcenter_azim_deg,
                                                  const int& azim_index,
                                                  const double& breamcenter_elev_deg,
                                                  const int& elev_index,
                                                  const int& dbf_weight_flag);

    void initializeRadarDataProcessingTrackModeParameters();

    // 输出各种信息到文件
    void createNewRadarSystemInfoFile();
    void createNewRadarOriginalPointsFile();
    void createNewRadarTrackPointsFile();
    void outputRadarSystemStateToText(const RadarSystemState& radar_system_state);
    void outputRadarOriginalPointToText(const TargetOriginalPoint& target_original_point, const RadarSystemState& radar_system_state);
    void outputLastTrackPointToText(const TargetTrack* target_track);

signals:
    void sigUpdateRadarSystemStateInOtherThreads(const RadarSystemState& new_radar_system_state);
    void sigSendPowerOffOrderToFpga();
    void sigSendPowerOnOrderToFpga();
    void sigSendResetOrderToFpgaSerdesError();
    void sigSendDbfCorrectCoefFileToFpgaWhenNoneCoef();

    void sigUpdateStatusBarOnMainWindow(const QString& new_message);
    void sigUpdateOriginalPointItems();
    void sigUpdateTargetTrackItems();

    void sigRemoveTrackIdFromSendingTargetSet(const int& new_track_id);
    void sigSendTrackLastPointListToController(const int& m_sector_index, QList<TargetTrackPoint>* m_track_to_send_list);

    // 跟踪模式
    void sigUpdateDataProcessingMode(const int& new_data_processing_mode); // 0-表示搜索模式, 1-表示prepare_track_flag 或者 start_track_flag 为 true

    void sigSendStopPointingAndTrackOrderToEnterTrackMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
    void sigQuitTrackModeToScanMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
    void sigSendNewHardwareSettingsTrackModeToFpga(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);

public slots:
    void sltUpdateRadarSystemStateInOtherThreads();
    void sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);
    void sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sltUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings);

    void sltProcessingNextFpgaPacket(FpgaPacket* received_fpga_packet);

    void sltAddTrackIdToSendingTargetSet(const int& new_track_id);
    void sltPrepareToEnterTrackMode(const int& new_track_id);
    void sltClickStopToQuitTrackMode();
};

class RadarDataProcessingThread : public QObject
{
    Q_OBJECT
public:
    explicit RadarDataProcessingThread(QObject* parent = nullptr);
    ~RadarDataProcessingThread() override;

    void startWorking();
    void stopWorking();

private:
    RadarDataProcessingObject* radar_data_processing_object;
    QThread working_thread;

signals:
    void sigUpdateRadarSystemState(const RadarSystemState& new_radar_system_state);
    void sigSendPowerOffOrderToFpga();
    void sigSendPowerOnOrderToFpga();
    void sigSendResetOrderToFpgaSerdesError();
    void sigSendDbfCorrectCoefFileToFpgaWhenNoneCoef();

    void sigRemoveTrackIdFromSendingTargetSet(const int& new_track_id);
    void sigSendTrackLastPointListToController(const int& m_sector_index, QList<TargetTrackPoint>* m_track_to_send_list);

    void sigUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);
    void sigUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sigUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings);

    void sigAddTrackIdToSendingTargetSet(const int& new_track_id);
    void sigPrepareToEnterTrackMode(const int& new_track_id);
    void sigClickStopToQuitTrackMode();

    void sigProcessingNextFpgaPacket(FpgaPacket* received_fpga_packet);

    void sigUpdateStatusBarOnMainWindow(const QString& new_message);
    void sigUpdateOriginalPointItems();
    void sigUpdateTargetTrackItems();
    void sigUpdateDataProcessingMode(const int& new_data_processing_mode);
    void sigSendStopPointingAndTrackOrderToEnterTrackMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
    void sigQuitTrackModeToScanMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
    void sigSendNewHardwareSettingsTrackModeToFpga(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
};

#endif // RADAR_DATA_PROCESSING_THREAD_H
