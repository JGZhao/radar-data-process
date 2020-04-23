#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include "global_definitions.h"
#include <QSettings>

// 配置文件名称
const QString CONFIG_FILE_NAME = "config.ini";

// 硬件设置相关
const int DOPPLER_EXCLUDE_NUM = 4;
const int THRESHOLDS_NUM = RADAR_MODE_NUM;

// 软件设置相关
const QString DEFAULT_MASTER_EXTERNAL_FILE_DIR = "./dbf_correct_coef_files/COE_External_Master.txt";
const QString DEFAULT_SLAVE_EXTERNAL_FILE_DIR = "./dbf_correct_coef_files/COE_External_Slave.txt";
const QString DEFAULT_MASTER_INTERNAL_FILE_DIR = "./dbf_correct_coef_files/COE_Internal_Master.txt";
const QString DEFAULT_SLAVE_INTERNAL_FILE_DIR = "./dbf_correct_coef_files/COE_Internal_Slave.txt";

// 数据处理参数设置相关
const int EXCLUDE_SETTINGS_NUM = 10;
const int SIGMA_DELTA_SETTINGS_NUM = 3;
const int SMOOTH_TRACK_SETTINGS_NUM = 2;

/********************硬件设置 start ********************/
struct RadarMissionModeSettings
{
    int radar_mission_mode;
    int antenna_power_mode;
    int radar_test_mode;
    int current_spr_mode_index;
    RadarMissionModeSettings()
    {
        radar_mission_mode = 0;
        antenna_power_mode = 0;
        radar_test_mode = 0;
        current_spr_mode_index = 0;
    }
};

struct Thresholds
{
    int doppler_threshold;
    int range_threshold_division_range_index_n_f;
    int range_threshold_near;
    int range_threshold_far;
    int fft_cut;
    int damp_cut_division_range_index;
    int damp_cut_near;
    int damp_cut_far;
    int doppler_exclude[DOPPLER_EXCLUDE_NUM];
};

struct PowerOffAreaSettings
{
    int enable_power_off_area_flag;
    double power_off_azim_deg_floor;
    double power_off_azim_deg_ceil;
};

struct HardwareSettings
{
    // 工作模式
    int antenna_enable;
    int work_or_serdes;
    int work_or_emulate;

    // 天线相关
    int work_mode_or_correct_mode;
    int antennna_fre_or_amp_config;
    int antenna_weight_method;
    int antennna_freq_code;
    int elev_support_angle;
    int antenna_raising_angle;
    int sigma_delta_decay_1;
    int protect_decay_1;
    int sigma_delta_decay_2;
    int protect_decay_2;

    // DBF 相关
    int amp_code;
    int channel_amp_phase_control_mode;
    int channel_amp_phase_control_code;
    int channel_num;
    int dbf_coe_choose;
    int dbf_coef_receive_raising_angle;

    // 转台相关
    int turntable_rotating_mode;
    double spr_map[RADAR_MISSION_NUM];
    double turntable_rotating_speed;
    double turntable_pointing_angle;

    // 信号处理相关
    int stc_select;
    int remove_dc_flag;
    int cfar_method;

    // 门限、截位、保留直流等
    Thresholds thresholds[THRESHOLDS_NUM];

    // 静默设置
    PowerOffAreaSettings power_off_area_settings;
};
/********************硬件设置 end ********************/

/********************软件设置 start ********************/
struct CommunicationSettings
{
    unsigned int adapter_index_communicating_with_fpga;
    int output_device_id;
    int output_min_track_times;
    int output_tracks_through_adapter_flag;
    int output_selected_or_all_method;
    int output_azim_north_or_east_flag;
    unsigned int output_IP_address; // 为了方便更新设置时使用memcpy, 所有IP地址采用unsigned int 来存放IP地址, 防止可能出现 sizeof(SoftwareSettings) 变化的情况
    int output_port;
    int input_packets_from_other_device_flag;
    int input_port;
};

struct DisplaySettings
{
    int display_system_error_info_flag;
    int display_original_points_flag;
    int display_filtered_points_flag;
    int display_tracks_flag;
    double display_area_radius;
    double display_circles_gap;
    int display_key_targets_track_times;
};

struct OutputFileSettings
{
    int output_fpga_packet_flag;
    int output_radar_system_info_flag;
    int output_radar_original_points_flag;
    int output_radar_track_points_flag;
    OutputFileSettings()
    {
        output_fpga_packet_flag = 0;
        output_radar_system_info_flag = 0;
        output_radar_original_points_flag = 0;
        output_radar_track_points_flag = 0;
    }
};

struct SystemAdjustSettings
{
    int enter_debug_mode_flag;
    int auto_adjust_signal_processer_flag;

    SystemAdjustSettings()
    {
        enter_debug_mode_flag = 0;
        auto_adjust_signal_processer_flag = 0;
    }
};

struct SoftwareSettings
{
    // 通信部分
    CommunicationSettings communication_settings;

    // 显示设置
    DisplaySettings display_settings;

    // 输出文件设置
    OutputFileSettings output_file_settings;

    // 系统自动调整设置
    SystemAdjustSettings system_adjust_settings;
};
/********************软件设置 end ********************/

/********************数据处理参数设置 start ********************/
struct RadarModeParameters
{
    double range_unit;
    double blind_area;
    double doppler_unit;
    int mtd_num;
    double rcs_correction_coef;
    int decay0;
    double range_division_01;
    double range_division_12; // 假定1和2之间是STC区域, 这个区域假定衰减补偿了距离衰减
    int decay2;
    double time_to_correct_azim;
};

struct SigmaDeltaElevSlope
{
    double sigma_delta_slope[ELEV_BEAM_NUM_SCAN_MODE];
};

struct SigmaDeltaParameters
{
    int sigma_delta_flag;
    int weight_flag;
    double sigma_delta_slope;
    double negative_bias_phase_floor;
    double negative_bias_phase_ceil;
    double beam_width;
    double max_bias_angle;
    int sigma_delta_filter_flag;
    double sigma_delta_filter_ratio;
};

struct AzimElevHeightBiasSettings
{
    double azim_bias_angle;
    double elev_bias_angle;
    double height_bias;
};

struct RadarLonLatAlt
{
    int convert_lon_lat_alt_flag;
    double radar_longitude;
    double radar_latitude;
    double radar_altitude;
};

struct SelectedAreaSettings
{
    int select_radar_mode_flag;
    int selected_radar_mode_floor;
    int selected_radar_mode_ceil;
    int select_range_flag;
    double selected_range_floor;
    double selected_range_ceil;
    int select_azim_flag;
    double selected_azim_deg_floor;
    double selected_azim_deg_ceil;
    int select_elev_flag;
    double selected_elev_deg_floor;
    double selected_elev_deg_ceil;
    int select_velocity_flag;
    double selected_velocity_floor;
    double selected_velocity_ceil;
    int select_height_flag;
    double selected_height_floor;
    double selected_height_ceil;
    int select_amp_flag;
    int selected_amp_floor;
    int selected_amp_ceil;
};

struct ExcludeSettings
{
    int is_selected_flag;
    int radar_mode_min;
    int radar_mode_max;
    double range_min;
    double range_max;
    double azim_deg_min;
    double azim_deg_max;
    double elev_deg_min;
    double elev_deg_max;
    double velocity_min;
    double velocity_max;
    int amp_min;
    int amp_max;
    double height_min;
    double height_max;
};

struct SidelobeFilterSettings
{
    int delta_azim_filter_flag;
    double delta_azim_filter_ratio;
    int delta_elev_filter_flag;
    double delta_elev_filter_ratio;
};

struct ClutterMapSettings
{
    int clutter_map_filter_flag;
    double clutter_map_generate_weight;
    double clutter_map_filter_threshold;
};

// TODO (L5000 程序里面也需要添加)
struct FilterCloudSettings
{
    int enable_filter_cloud;
    int filter_cloud_threshold;
    FilterCloudSettings()
    {
        enable_filter_cloud = 0;
        filter_cloud_threshold = 20;
    }
};

struct CombineThresholds
{
    int combine_range_unit_threshold;
    int combine_doppler_unit_threshold;
    double combine_range_threshold;
    double combine_velocity_threshold;
    double combine_azim_threshold_deg;
    double combine_elev_threshold_deg;
};

struct CorrelateThresholds
{
    double correlate_range_threshold_floor;
    double correlate_range_threshold_ceil;
    double correlate_velocity_threshold_same_direction;
    double correlate_velocity_threshold_opposite_direction;
    double correlate_azim_threshold_deg_0;
    double correlate_elev_threshold_deg_0;
    double correlate_range_division_01;
    double correlate_azim_threshold_deg_1;
    double correlate_elev_threshold_deg_1;
    double correlate_range_division_12;
    double correlate_azim_threshold_deg_2;
    double correlate_elev_threshold_deg_2;
    double correlate_x_threshold;
    double correlate_y_threshold;
    double correlate_z_threshold;
    double correlate_vx_threshold;
    double correlate_vy_threshold;
    double correlate_vz_threshold;
};

struct SmoothTrackSettings
{
    int filter_track_method;
    double filter_track_coef_alpha;
    double filter_track_coef_beta;
    double filter_track_coef_gama;
    int smooth_track_flag;
};

struct FilterTracksSettings
{
    int filter_tracks_flag;
    double filter_tracks_range_threshold;
    double filter_tracks_velocity_threshold;
    double filter_tracks_mean_delta_range_threshold;
};

struct DataProcessingSettings
{
    // 数据解析部分
    RadarModeParameters radar_wave_paremeters[RADAR_MODE_NUM];
    SigmaDeltaElevSlope sigma_delta_elev_slope[2];                         // DBF 各波束俯仰加权、不加权的斜率
    SigmaDeltaParameters sigma_delta_parameters[SIGMA_DELTA_SETTINGS_NUM]; // 方位差, 俯仰差
    AzimElevHeightBiasSettings azim_elev_height_bias_settings;
    RadarLonLatAlt radar_lon_lat_alt;

    // 数据过滤
    SelectedAreaSettings selected_area_settings;
    ExcludeSettings exclude_settings[EXCLUDE_SETTINGS_NUM];
    SidelobeFilterSettings sidelobe_filter_settings;
    ClutterMapSettings clutter_map_settings;
    FilterCloudSettings filter_cloud_settings;

    // 数据处理
    CombineThresholds combine_thresholds;
    CorrelateThresholds correlate_thresholds[2];
    SmoothTrackSettings smooth_track_settings[2];
    FilterTracksSettings filter_tracks_settings;
};
/********************数据处理参数设置 end ********************/

/********************校准系数设置 start ********************/
struct DbfCorrectCoefFileDirSettings
{
    QString master_external_file_dir;
    QString slave_external_file_dir;
    QString master_internal_file_dir;
    QString slave_internal_file_dir;
};

/********************校准系数设置 end ********************/

extern void initializeRadarMissionModeSettings(RadarMissionModeSettings& radar_mission_settings);
extern void initializeHardwareSettings(HardwareSettings& hardware_settings);
extern void initializeSoftwareSettings(SoftwareSettings& software_settings);
extern void initializeDataProcessingSettings(DataProcessingSettings& data_processing_settings);
extern void initializeDbfCorrectCoefFileDirSettings(DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings);
extern void initializeHardwareAndSoftwareAndDataProcessingSettingsBasedOnConfigFile(HardwareSettings& hardware_settings,
                                                                                    SoftwareSettings& software_settings,
                                                                                    DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings,
                                                                                    DataProcessingSettings& data_processing_settings,
                                                                                    QSettings* settings_in_config_file);
extern void initializeHardwareSettingsBasedOnConfigFile(HardwareSettings& hardware_settings, QSettings* settings_in_config_file);
extern void initializeSoftwareSettingsBasedOnConfigFile(SoftwareSettings& software_settings, QSettings* settings_in_config_file);
extern void initializeDbfCorrectCoefFileDirSettingsBasedOnConfigFile(DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings, QSettings* settings_in_config_file);
extern void initializeDataProcessingSettingsBasedOnInConfigFile(DataProcessingSettings& data_processing_settings, QSettings* settings_in_config_file);

extern void updateHardwareParametersInConfigFileBasedOnHardwareSettings(const HardwareSettings& hardware_settings, QSettings* settings_in_config_file);
extern void updateSoftwareParametersInConfigFileBasedOnSoftwareSettings(const SoftwareSettings& software_settings, QSettings* settings_in_config_file);
extern void updateDbfCorrectCoefFileDirParametersInConfigFileBasedOnDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings,
                                                                                                  QSettings* settings_in_config_file);
extern void updateDataProcessingParametersInConfigFileBasedOnDataProcessingSetting(const DataProcessingSettings& data_processing_settings, QSettings* settings_in_config_file);

/********************回读设置 start ********************/
struct ReadFpgaPacketFileSettings
{
    bool read_fpga_packet_from_file_flag;
    QString fpga_packet_file_dir;
    ReadFpgaPacketFileSettings()
    {
        read_fpga_packet_from_file_flag = false;
        fpga_packet_file_dir.clear();
    }
};
void initializeReadFpgaPacketFileSettings(ReadFpgaPacketFileSettings& read_fpga_packet_file_settings);
/********************回读设置 end ********************/

/********************跟踪模式配置 start ********************/
struct HardwareSettingsTrackMode
{

    int next_radar_mode;
    double next_pointing_azim_deg;
    int next_elev_index_list[4];
    HardwareSettingsTrackMode()
    {
        next_radar_mode = 0;
        next_pointing_azim_deg = 0;
        next_elev_index_list[0] = 6;
        next_elev_index_list[1] = 7;
        next_elev_index_list[2] = 8;
        next_elev_index_list[3] = 9;
    }
};
void initializeHardwareSettingsTrackMode(HardwareSettingsTrackMode& hardware_settings_track_mode);
/********************跟踪模式配置 end ********************/

#endif // GLOBAL_SETTINGS_H
