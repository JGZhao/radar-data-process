#ifndef RADAR_DATA_PROCESSING_MODULE_H
#define RADAR_DATA_PROCESSING_MODULE_H

#include <QDateTime>
#include <QStringList>
#include "global_settings.h"

// 常量
// 数据解析
const int RADAR_SCAN_MODE = 0;
const int RADAR_TRACK_MODE = 1;
const double DBF_COEF_RECEIVE_RAISING_ANGLE_MAP[4] = {0, 0.5, 1, 3};
const double UNWEIGHTED_ELEV_INDEX_TO_BEAM_CENTER_DEG_MAP[ELEV_BEAM_NUM_SCAN_MODE] = {25.5, 22.5, 19.5, 16.5, 13.5, 10.5, 7.5, 4.5, 1.5, 1.5};
const double WEIGHTED_ELEV_INDEX_TO_BEAM_CENTER_DEG_MAP[ELEV_BEAM_NUM_SCAN_MODE] = {29.25, 25.75, 22.25, 18.75, 15.25, 11.75, 8.25, 4.75, 1.5, 1.5};
// 0-近搜 1_远搜 2_超远搜 3_近跟 4_近跟B 5_远跟 6_远跟B
const int USING_TRACK_MODE_NUM = 3;                                     // 实际使用的跟踪模式的数量
const int USING_TRACK_MODE_INDEX_MAP[USING_TRACK_MODE_NUM] = {3, 5, 6}; // 实际使用的跟踪模式对应的下标
const double RANGE_DIVISION_SHORT_LONG1 = 3000;
const double RANGE_DIVISION_LONG1_LONG2 = 6000;
const double RANGE_START[RADAR_MODE_NUM] = {180, 1800, 11000, 180, 4000, RANGE_DIVISION_SHORT_LONG1, RANGE_DIVISION_LONG1_LONG2};
const double RANGE_END[RADAR_MODE_NUM] = {6000, 11000, 20000, RANGE_DIVISION_SHORT_LONG1, 10000, RANGE_DIVISION_LONG1_LONG2, 11000};

// 信号处理相关
const int TURNTABLE_ROTATING_SPEED_THRESHOLD_TO_CORRECT_AZIM = 1; // 转速大于这个值才修正发射方位角

// 数据处理
const int MAX_ORIGINAL_POINTS_NUM_IN_EACH_AZIM_SETCOR = 800; // 每一个方位扇区的原始点迹的最大数量
const int COMBINE_NEIGHBOUR_SECTORS_NUM = 2;                 // 融合的相邻扇区数
const int CORRELATE_NEIGHBOUR_SECTORS_NUM = 2;               // 相关时的相邻扇区数
const int MAX_LOST_TIMES = 4;                                // 丢失次数达到 MAX_LOST_TIMES 就删除
const int MAX_LOST_TIMES_TRACK_MODE = 2;                     // 丢失次数达到 MAX_LOST_TIMES_TRACK_MODE 就删除
const int CONTINUOUS_LOST_TIMES_TRACK_MODE = 20;             // 跟踪模式连续丢失CONTINUOUS_LOST_TIMES_TRACK_MODE算丢失次数+1
const int PREPARE_START_TRACK_TIMES = 3;                     // 未完全起批跟踪次数
const int COMPLETELY_START_TRACK_TIMES = 5;                  // 完全起批跟踪次数
const int MAX_START_TRACK_LOST_TIMES = 3;                    // 起批时最大丢失次数, 3/5原则
const int MAX_START_TRACK_SIZE = 5;                          // 起批时航迹最大点数, 3/5原则
const double MIN_PREDICT_TARGET_RANGE = 100;                 // 外推的最小距离

// 杂波图相关
const int MAX_RADAR_MODE_NUM = RADAR_SCAN_MODE_NUM;
const int MAX_RANGE_UNIT_NUM = 4096;
const int MAX_AZIM_UNIT_NUM = AZIM_SECTOR_NUM;
const int MAX_ELEV_UNIT_NUM = ELEV_BEAM_NUM_SCAN_MODE;
const int MAX_DOPPLER_UNIT_NUM = 512;

// 航迹关联算法相关
const int MAX_TRACK_ID_NUM = 2000;                   // 航迹ID的最大数量
const int VARIABLE_ALPHA_BETA_MAX_ITERATION_NUM = 7; // alpha-beta 滤波最多迭代 7 次, 对应 k 在 2 ~ 7 ( beta在 k = 1 是无效的 )
const double ALPHA_BETA_FILER_ALPHA_ARRAY[11] = {1, 1, 5.0 / 6.0, 7.0 / 10.0, 3.0 / 5.0, 11.0 / 21.0, 13.0 / 28.0, 5.0 / 12.0, 17.0 / 45.0, 19.0 / 55.0, 7.0 / 22.0};
const double ALPHA_BETA_FILER_BETA_ARRAY[11] = {1, 1, 1.0 / 2.0, 3.0 / 10.0, 1.0 / 5.0, 1.0 / 7.0, 3.0 / 28.0, 1.0 / 12.0, 1.0 / 15.0, 3.0 / 55.0, 1.0 / 22.0};
const int ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE = 20;  // 跟踪模式的 alpha-beta 滤波的滤波时间间隔控制与搜索模式基本一样 (20 * 0.1s = 2s)

// 系统状态相关
const int RADAR_SYSTEM_STATE_ERROR_INFO_NUM = 10;
const double WARNING_ANTENNA_TEMPERATURE = 85;

// 跟踪模式
const double AZIM_DEG_RESERVED_STOP_TO_TRACK_MODE = 25;
const double MAX_CONTINUOUS_DEVIATE_AZIM_DEGREE = 0.8; // 方位角两次连续偏离的阈值

struct RadarSystemState
{
    int current_data_processing_mode;     // 当前软件的数据处理模式: 0-搜索 1-跟踪
    int radar_connected_flag;             // 雷达连通标志
    unsigned int pkt_num;                 // 自己统计包数
    int current_radar_mission_mode_index; // 当前雷达任务模式
    int current_radar_mode_index;         // 当前雷达模式
    int current_scan_or_track_mode;       // 当前处于搜索模式还是跟踪模式
    double current_azim_deg;              // 考虑方位偏移角之后的方位角
    double current_azim_deg_to_north;     // 当前码盘角度对应的北向角
    double current_transmit_azim_deg;     // 当前发射方位角
    double last_transmit_azim_deg;        // 上一轮的方位角
    double current_transmit_azim_center;  // 当前波束方位角度中心
    int current_lost_azim_sectors_num;
    unsigned int total_lost_azim_sectors_num;
    int error_lost_azim_sectors_num;
    // DBF是否加权
    int receiver_weight_set;
    int dbf_coef_receive_raising_angle_index;
    int next_elev_round_flag; // 是否俯仰已经到下一轮了
    int last_elev_index;
    int current_elev_index;
    double current_elev_deg;
    int radar_state_register;
    int target_num;
    int radar_mission_mode;
    int radar_set_mode;
    double biss_angle_deg;
    double turntable_rotating_speed;
    int track_azim_set;

    int antenna_command_mode;
    // 0x81
    int antenna_temperature_warning;
    int antenna_work_mode;
    double bank_12v_current;
    double launch_28v_current;
    int output_power_1;
    int output_power_2;
    // 0x82
    int fre_code;
    double in_current_28v;
    double recv_12v_current;
    double fre_12v_current;
    int beam_ctrl_12v_current;
    int battery_T;
    // 0x83
    int T_transmitter1;
    int T_transmitter2;
    int T_recv1;
    int T_recv2;
    int T_recv3;
    int T_recv4;

    int bank_mode_report;
    double T_AD_FPGA1;
    double T_AD_FPGA2;
    double T_QDR_FPGA1;
    double T_QDR_FPGA2;
    double T_QDR_FPGA3;
    double T_QDR_FPGA4;
    int last_GMAC_count; // 用于判断是否丢包
    int current_GMAC_count;
    int current_lost_packets_count;
    int total_lost_packets_count;
    int error_lost_packets_count; // 用于显示的丢包次数
    double current_free_time;
    double last_free_time;
    double cumulative_free_time;
    int antenna_data_parity;
    float ATP_azim_position;
    float ATP_azim_speed;
    float ATP_azim_acc;
    int ATP_cyclic_code;
    int ATP_parity;
    uint64_t FPGA_version;
    int error_info_flag_list[RADAR_SYSTEM_STATE_ERROR_INFO_NUM];

    // 构造函数
    RadarSystemState();
};

void initializeRadarSystemState(RadarSystemState& radar_system_state);

// 原始点迹类
struct TargetOriginalPoint
{
    TargetOriginalPoint();
    TargetOriginalPoint(QDateTime _date_time,
                        int _radar_mode,
                        int _range_index,
                        int _azim_index,
                        int _elev_index,
                        int _doppler_index,
                        double _range,
                        double _azim_deg,
                        double _elev_deg,
                        double _vr,
                        double _x,
                        double _y,
                        double _z,
                        double _vx,
                        double _vy,
                        double _vz,
                        int _amp_sigma,
                        int _amp_delta_azim,
                        int _amp_delta_elev1,
                        int _amp_delta_elev2,
                        int _sigma_I,
                        int _sigma_Q,
                        int _delta_azim_I,
                        int _delta_azim_Q,
                        int _delta_elev1_I,
                        int _delta_elev1_Q,
                        int _delta_elev2_I,
                        int _delta_elev2_Q,
                        double _free_time,
                        int _snr,
                        int _cfar_neighbour_num,
                        double rcs,
                        bool _is_filtered,
                        bool _is_combined,
                        bool _is_correlated,
                        double _best_correlate_distance,
                        bool _best_correlate_distance_track_established_flag);

    QDateTime date_time;
    int radar_mode;
    int range_index;
    int azim_index;
    int elev_index;
    int doppler_index;
    double range;
    double azim_deg;
    double elev_deg;
    double vr;
    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz;
    int amp_sigma;
    int amp_delta_azim;
    int amp_delta_elev1;
    int amp_delta_elev2;
    int sigma_I;
    int sigma_Q;
    int delta_azim_I;
    int delta_azim_Q;
    int delta_elev1_I;
    int delta_elev1_Q;
    int delta_elev2_I;
    int delta_elev2_Q;
    double free_time;
    int snr;
    int cfar_neighbour_num;
    double rcs;
    bool is_filtered;
    bool is_combined;
    bool is_correlated;
    double best_correlate_distance;
    bool best_correlate_distance_track_established_flag; // 优先与已经起批的航迹相关
};

// TODO 航向
struct TargetTrackPoint
{
    QDateTime date_time;
    int track_id;
    int radar_mode;
    int track_times;
    int lost_times;
    int azim_index;
    int elev_index;
    double range;
    double azim_deg;
    double elev_deg;
    double vr;
    int amp_sigma;
    int amp_delta_azim;
    int amp_delta_elev1;
    int amp_delta_elev2;
    int snr;
    int cfar_neighbour_num;
    double rcs;
    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz;
    double longitude;
    double latitude;
    double altitude;
    double free_time;
    bool is_correlated;   // 是否相关, 用于判断是否是外推点
    bool is_shown;        // 是否显示
    int established_mode; // 航迹起始状态
    TargetTrackPoint()
    {
        date_time = QDateTime::currentDateTime();
        track_id = -1;
        radar_mode = 0;
        track_times = 0;
        lost_times = 0;
        azim_index = 0;
        elev_index = 0;
        range = 0;
        azim_deg = 0;
        elev_deg = 0;
        vr = 0;
        amp_sigma = 0;
        amp_delta_azim = 0;
        amp_delta_elev1 = 0;
        amp_delta_elev2 = 0;
        snr = 0;
        cfar_neighbour_num = 0;
        rcs = 0;
        x = 0;
        y = 0;
        z = 0;
        vx = 0;
        vy = 0;
        vz = 0;
        longitude = 0;
        latitude = 0;
        altitude = 0;
        free_time = 0;
        is_correlated = false;
        is_shown = true;
        established_mode = 0;
    }
};

struct TrackPredictOrFilteredPoint
{
    double range_p;
    double azim_p;
    double elev_p;
    double x_p;
    double y_p;
    double z_p;
    double vx_p;
    double vy_p;
    double vz_p;

    TrackPredictOrFilteredPoint()
    {
        range_p = 0;
        azim_p = 0;
        elev_p = 0;
        x_p = 0;
        y_p = 0;
        z_p = 0;
        vx_p = 0;
        vy_p = 0;
        vz_p = 0;
    }
};

// 航迹类
class TargetTrack
{
public:
    TargetTrack(int new_track_id);
    ~TargetTrack()
    {
    }

public:
    // 搜索模式
    bool precorrelateWithOriginalPoint(TargetOriginalPoint& targetoriginal_point,
                                       const DataProcessingSettings& data_processing_settings); // 跟原始点迹预相关（搜索模式）
    void addMeasuredTrackPoint(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings);
    void addMeasuredTrackPointAlphaBeta(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings);
    void addMeasuredTrackPointAlphaBetaConstant(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings);
    void updateTrackWithMeasureOrPredict(double current_free_time, const DataProcessingSettings& data_processing_settings);
    void updateTrackWithMeasureOrPredictNotLost(double current_free_time, const DataProcessingSettings& data_processing_settings);

    // 跟踪模式
    bool precorrelateWithOriginalPointTrackMode(TargetOriginalPoint& target_original_point,
                                                const DataProcessingSettings& data_processing_settings); // 跟原始点迹预相关（跟踪模式）
    void precorrelateWithOriginalPointListTrackMode(QList<TargetOriginalPoint>& original_points_batch, const DataProcessingSettings& data_processing_settings);
    void addMeasuredTrackPointTrackMode(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings);
    void addMeasuredTrackPointTrackModeAlphaBeta(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings);
    void addMeasuredTrackPointTrackModeAlphaBetaConstant(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings);
    void updateTrackWithMeasureOrPredictTrackMode(double current_free_time, const DataProcessingSettings& data_processing_settings);

    // 更新起批状态
    void updateEstablishedMode();

private:
    void convertLonLatAlt(TargetTrackPoint& track_point, const DataProcessingSettings& m_data_processing_settings);

    // 搜索模式
    void predictOneTrackPoint(double current_free_time, const DataProcessingSettings& m_data_processing_settings);
    void predictOneTrackPointNotLost(double current_free_time, const DataProcessingSettings& m_data_processing_settings);
    void predictOneTrackPointAlphaBeta(double current_free_time, const DataProcessingSettings& m_data_processing_settings);
    void predictOneTrackPointNotLostAlphaBeta(double current_free_time, const DataProcessingSettings& m_data_processing_settings);

    // 跟踪模式
    void predictOneTrackPointTrackMode(double current_free_time, const DataProcessingSettings& m_data_processing_settings);
    void predictOneTrackPointTrackModeAlphaBeta(double current_free_time, const DataProcessingSettings& m_data_processing_settings);

public:
    int track_id;                  // 航迹编号
    int effective_track_size;      // 航迹的有效size (在搜索模式下前后两圈不一致的航迹外推不算有效航迹)
    int track_times;               // 跟踪次数
    int continuous_lost_times;     // 连续丢失次数
    int total_lost_times;          // 累计丢失次数
    int track_size_track_mode;     // 跟踪模式下的航迹数量
    int track_times_track_mode;    // 跟踪模式下的跟踪次数
    int lost_times_track_mode;     // 跟踪模式下的丢失次数
    bool is_correlated;            // 本次是否相关上
    int last_measured_point_index; // 最新的一个实测点的下标

    QMap<double, TargetOriginalPoint*> distance_to_point_index_map; // Map插入时自动升序
    QList<TargetTrackPoint> target_track_points_list;               //航迹

    // α-β 滤波
    int alpha_beta_k;
    double coef_alpha;
    double coef_beta;
    TrackPredictOrFilteredPoint track_predict_point;
    TrackPredictOrFilteredPoint track_filter_point;
};

// 扇区类
class AzimSector
{
public:
    AzimSector(int index);
    ~AzimSector()
    {
    }

public:
    void addTargetOriginalPoint(TargetOriginalPoint target_original_point);
    void updateTracks(const DataProcessingSettings& data_processing_settings); // 更新航迹

public:
    int azim_sector_index;
    QList<TargetOriginalPoint> target_original_points_list;
    QList<TargetTrack*> target_tracks_list;
    int last_radar_mission_index;
    int current_radar_mission_index;
    double current_free_time;
};

class AzimSectorListScanMode
{
public:
    AzimSectorListScanMode();
    ~AzimSectorListScanMode()
    {
    }

private:
    void combineTwoOriginalPoints(TargetOriginalPoint& op1, TargetOriginalPoint& op2, const DataProcessingSettings& data_processing_settings);
    void combineTwoOriginalPointsWeighted(TargetOriginalPoint& op1, TargetOriginalPoint& op2, const DataProcessingSettings& data_processing_settings);

public:
    void initializeDataProcessingAzimSectorIndexScamMode();                                               // 初始化搜索模式的预相关、搜索、跟踪等各方位扇区
    void combineCurrentSectorWithPreviousSectors(const DataProcessingSettings& data_processing_settings); // 融合
    void precorrelateTracksWithOriginalPoints(const DataProcessingSettings& data_processing_settings);    // 扇区的每个航迹与左中右的每个点迹做预相关
    void updateTracks(const DataProcessingSettings& data_processing_settings);                            // 更新航迹
    void updateCombineAndPrecorrelateAndTrackAndUpdateAndTrackStartIndex(int delta_index);                // 更新融合扇区到航迹起始扇区区间的下标

public:
    QList<AzimSector> azim_sector_list;
    int scan_current_input_index;
    int scan_prepare_combine_index;
    int scan_current_combine_index;
    int scan_current_precorrelate_index;
    int scan_current_track_update_index;
    int scan_current_track_start_index;
    QList<int> unused_track_id_list;
};

// 跟踪模式
class OriginalPointsBatchTrackMode
{
public:
    OriginalPointsBatchTrackMode();
    ~OriginalPointsBatchTrackMode()
    {
    }

    void combineCurrentOriginalPointsBatch(const DataProcessingSettings& data_processing_settings);
    void addTargetOriginalPoint(TargetOriginalPoint target_original_point);

private:
    void combineTwoOriginalPointsTrackMode(TargetOriginalPoint& op1, TargetOriginalPoint& op2, const DataProcessingSettings& data_processing_settings);

public:
    QList<TargetOriginalPoint> target_original_points_list;
    double current_free_time;
};

struct TrackInfoDisplayed
{
    int track_no;
    int radar_mode;
    double range;
    double azim_deg;
    double elev_deg;
    double velocity;
    int amp;
    int snr;
    double rcs;
    int track_times;
    int lost_times;
    double x;
    double y;
    double z;
    double ve;
    double vn;
    double vz;
    double longitude;
    double latitude;
    double altitude;
    bool is_visible;

    TrackInfoDisplayed()
    {
        track_no = -1;
        radar_mode = 0;
        range = 0;
        azim_deg = 0;
        elev_deg = 0;
        velocity = 0;
        amp = 0;
        snr = 0;
        rcs = 0;
        track_times = 0;
        lost_times = 0;
        x = 0;
        y = 0;
        z = 0;
        ve = 0;
        vn = 0;
        vz = 0;
        longitude = 0;
        latitude = 0;
        altitude = 0;
        is_visible = false;
    }
};
void initializeTrackInfoDisplayed(TrackInfoDisplayed& track_info_displayed);
void updateTrackInfoDisplayed(TrackInfoDisplayed& track_info_displayed, const TargetTrack* target_track);

struct TrackingTargetPosition
{
    double range;
    int current_radar_track_mode;
    int last_radar_track_mode;
    double last_but_one_track_point_azim_deg;
    double last_track_point_azim_deg;
    double current_pointing_azim_deg;
    int azim_deg_deviate_times;
    int last_track_point_elev_index;
    int last_but_one_track_point_elev_index;
    int elev_index_deviate_times;
    int elev_index_list[4];
    bool hardware_settings_needed_to_update_flag;

    TrackingTargetPosition()
    {
        range = 0;
        current_radar_track_mode = NEAR_TRACK_MODE_INDEX;
        last_radar_track_mode = 0;
        last_but_one_track_point_azim_deg = 0;
        last_track_point_azim_deg = 0;
        current_pointing_azim_deg = 0;
        azim_deg_deviate_times = 0;
        last_track_point_elev_index = 0;
        last_but_one_track_point_elev_index = 0;
        elev_index_deviate_times = 0;
        elev_index_list[0] = 6;
        elev_index_list[1] = 7;
        elev_index_list[2] = 8;
        elev_index_list[3] = 9;
        hardware_settings_needed_to_update_flag = false;
    }
};
void initializeTrackingTargetPosition(TrackingTargetPosition& tracking_target_position);
void initializeTrackingTargetPositionWithTrackConsiderCurrentAzim(TrackingTargetPosition& tracking_target_position,
                                                                  TargetTrack* target_track,
                                                                  const RadarSystemState& m_radar_system_state);
void updateTrackingTargetPosition(TrackingTargetPosition& tracking_target_position, TargetTrack* target_track);
bool needingUpdateTrackingTargetPosition(TrackingTargetPosition& tracking_target_position, TargetTrack* target_track);
void updateHardwareSettingsTrackModeBasedOnTrackingTargetPosition(const TrackingTargetPosition& new_tracking_target_position,
                                                                  HardwareSettingsTrackMode& hardware_settings_track_mode,
                                                                  const DataProcessingSettings& m_data_processing_settings);
void updateHardwareSettingsWhenQuitTrackModeBasedOnTrackingTargetPosition(const TrackingTargetPosition& new_tracking_target_position,
                                                                          HardwareSettingsTrackMode& hardware_settings_track_mode);

#endif // RADAR_DATA_PROCESSING_MODULE_H
