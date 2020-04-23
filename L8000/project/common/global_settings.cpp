#include <QFile>
#include <QHostAddress>
#include "global_settings.h"

void initializeRadarMissionModeSettings(RadarMissionModeSettings& radar_mission_mode_settings)
{
    radar_mission_mode_settings.radar_mission_mode = 0;
    radar_mission_mode_settings.antenna_power_mode = 0;
    radar_mission_mode_settings.radar_test_mode = 0;
    radar_mission_mode_settings.current_spr_mode_index = 0;
}

void initializeHardwareSettings(HardwareSettings& hardware_settings)
{
    // 工作模式
    hardware_settings.antenna_enable = 0;
    hardware_settings.work_or_serdes = 0;
    hardware_settings.work_or_emulate = 0;

    // 天线相关
    hardware_settings.work_mode_or_correct_mode = 0;
    hardware_settings.antennna_fre_or_amp_config = 0;
    hardware_settings.antenna_weight_method = 0;
    hardware_settings.antennna_freq_code = 5;
    hardware_settings.elev_support_angle = 0;
    hardware_settings.antenna_raising_angle = 0;
    hardware_settings.sigma_delta_decay_1 = 0;
    hardware_settings.protect_decay_1 = 0;
    hardware_settings.sigma_delta_decay_2 = 0;
    hardware_settings.protect_decay_2 = 0;

    // DBF 相关
    hardware_settings.amp_code = 0;
    hardware_settings.channel_amp_phase_control_mode = 0;
    hardware_settings.channel_amp_phase_control_code = 0;
    hardware_settings.channel_num = 0;
    hardware_settings.dbf_coe_choose = 0;
    hardware_settings.dbf_coef_receive_raising_angle = 0;

    // 转台相关
    hardware_settings.turntable_rotating_mode = 2;
    for (int ii = 0; ii < RADAR_MISSION_NUM; ++ii)
    {
        if (ii == SUPER_FAR_SCAN_MODE_INDEX) // 超远搜
        {
            hardware_settings.spr_map[ii] = 4;
        }
        else
        {
            hardware_settings.spr_map[ii] = 2;
        }
    }
    if (fabs(hardware_settings.spr_map[0] - 0.0) < EPS)
    {
        hardware_settings.turntable_rotating_speed = 180;
    }
    else
    {
        hardware_settings.turntable_rotating_speed = 360.0 / hardware_settings.spr_map[0];
    }
    hardware_settings.turntable_pointing_angle = 0;

    // 静默相关
    hardware_settings.power_off_area_settings.enable_power_off_area_flag = 0;
    hardware_settings.power_off_area_settings.power_off_azim_deg_floor = 0;
    hardware_settings.power_off_area_settings.power_off_azim_deg_ceil = 0;

    // 信号处理相关
    hardware_settings.stc_select = 1;
    hardware_settings.remove_dc_flag = 1;
    hardware_settings.cfar_method = 0;

    hardware_settings.thresholds[0].doppler_threshold = 600;
    hardware_settings.thresholds[0].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[0].range_threshold_near = 4000;
    hardware_settings.thresholds[0].range_threshold_far = 2000;
    hardware_settings.thresholds[0].fft_cut = 5;
    hardware_settings.thresholds[0].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[0].damp_cut_near = 3;
    hardware_settings.thresholds[0].damp_cut_far = 3;
    hardware_settings.thresholds[0].doppler_exclude[0] = 1;
    hardware_settings.thresholds[0].doppler_exclude[1] = 127;
    hardware_settings.thresholds[0].doppler_exclude[2] = 129;
    hardware_settings.thresholds[0].doppler_exclude[3] = 255;

    hardware_settings.thresholds[1].doppler_threshold = 600;
    hardware_settings.thresholds[1].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[1].range_threshold_near = 3000;
    hardware_settings.thresholds[1].range_threshold_far = 2000;
    hardware_settings.thresholds[1].fft_cut = 5;
    hardware_settings.thresholds[1].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[1].damp_cut_near = 3;
    hardware_settings.thresholds[1].damp_cut_far = 3;
    hardware_settings.thresholds[1].doppler_exclude[0] = 6;
    hardware_settings.thresholds[1].doppler_exclude[1] = 127;
    hardware_settings.thresholds[1].doppler_exclude[2] = 129;
    hardware_settings.thresholds[1].doppler_exclude[3] = 250;

    hardware_settings.thresholds[2].doppler_threshold = 600;
    hardware_settings.thresholds[2].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[2].range_threshold_near = 2500;
    hardware_settings.thresholds[2].range_threshold_far = 1500;
    hardware_settings.thresholds[2].fft_cut = 5;
    hardware_settings.thresholds[2].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[2].damp_cut_near = 3;
    hardware_settings.thresholds[2].damp_cut_far = 3;
    hardware_settings.thresholds[2].doppler_exclude[0] = 6;
    hardware_settings.thresholds[2].doppler_exclude[1] = 127;
    hardware_settings.thresholds[2].doppler_exclude[2] = 129;
    hardware_settings.thresholds[2].doppler_exclude[3] = 250;

    hardware_settings.thresholds[3].doppler_threshold = 600;
    hardware_settings.thresholds[3].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[3].range_threshold_near = 4000;
    hardware_settings.thresholds[3].range_threshold_far = 2000;
    hardware_settings.thresholds[3].fft_cut = 5;
    hardware_settings.thresholds[3].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[3].damp_cut_near = 3;
    hardware_settings.thresholds[3].damp_cut_far = 3;
    hardware_settings.thresholds[3].doppler_exclude[0] = 6;
    hardware_settings.thresholds[3].doppler_exclude[1] = 511;
    hardware_settings.thresholds[3].doppler_exclude[2] = 513;
    hardware_settings.thresholds[3].doppler_exclude[3] = 1018;

    hardware_settings.thresholds[4].doppler_threshold = 600;
    hardware_settings.thresholds[4].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[4].range_threshold_near = 3000;
    hardware_settings.thresholds[4].range_threshold_far = 2000;
    hardware_settings.thresholds[4].fft_cut = 5;
    hardware_settings.thresholds[4].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[4].damp_cut_near = 3;
    hardware_settings.thresholds[4].damp_cut_far = 3;
    hardware_settings.thresholds[4].doppler_exclude[0] = 6;
    hardware_settings.thresholds[4].doppler_exclude[1] = 511;
    hardware_settings.thresholds[4].doppler_exclude[2] = 513;
    hardware_settings.thresholds[4].doppler_exclude[3] = 1018;

    hardware_settings.thresholds[5].doppler_threshold = 600;
    hardware_settings.thresholds[5].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[5].range_threshold_near = 3000;
    hardware_settings.thresholds[5].range_threshold_far = 2000;
    hardware_settings.thresholds[5].fft_cut = 5;
    hardware_settings.thresholds[5].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[5].damp_cut_near = 3;
    hardware_settings.thresholds[5].damp_cut_far = 3;
    hardware_settings.thresholds[5].doppler_exclude[0] = 6;
    hardware_settings.thresholds[5].doppler_exclude[1] = 511;
    hardware_settings.thresholds[5].doppler_exclude[2] = 513;
    hardware_settings.thresholds[5].doppler_exclude[3] = 1018;

    hardware_settings.thresholds[6].doppler_threshold = 600;
    hardware_settings.thresholds[6].range_threshold_division_range_index_n_f = 300;
    hardware_settings.thresholds[6].range_threshold_near = 3000;
    hardware_settings.thresholds[6].range_threshold_far = 2000;
    hardware_settings.thresholds[6].fft_cut = 5;
    hardware_settings.thresholds[6].damp_cut_division_range_index = 300;
    hardware_settings.thresholds[6].damp_cut_near = 3;
    hardware_settings.thresholds[6].damp_cut_far = 3;
    hardware_settings.thresholds[6].doppler_exclude[0] = 6;
    hardware_settings.thresholds[6].doppler_exclude[1] = 511;
    hardware_settings.thresholds[6].doppler_exclude[2] = 513;
    hardware_settings.thresholds[6].doppler_exclude[3] = 1018;
}

void initializeSoftwareSettings(SoftwareSettings& software_settings)
{
    software_settings.communication_settings.adapter_index_communicating_with_fpga = 0;
    software_settings.communication_settings.output_device_id = 1;
    software_settings.communication_settings.output_min_track_times = 3;
    software_settings.communication_settings.output_tracks_through_adapter_flag = 1;
    software_settings.communication_settings.output_selected_or_all_method = 1;
    software_settings.communication_settings.output_azim_north_or_east_flag = 1;
    software_settings.communication_settings.output_IP_address = QHostAddress("127.0.0.1").toIPv4Address();
    software_settings.communication_settings.output_port = 10010;
    software_settings.communication_settings.input_packets_from_other_device_flag = 0;
    software_settings.communication_settings.input_port = 10086;

    software_settings.display_settings.display_system_error_info_flag = 1;
    software_settings.display_settings.display_original_points_flag = 0;
    software_settings.display_settings.display_filtered_points_flag = 0;
    software_settings.display_settings.display_tracks_flag = 1;
    software_settings.display_settings.display_area_radius = 10000;
    software_settings.display_settings.display_circles_gap = 1000;
    software_settings.display_settings.display_key_targets_track_times = 3;

    software_settings.output_file_settings.output_fpga_packet_flag = 0;
    software_settings.output_file_settings.output_radar_system_info_flag = 1;
    software_settings.output_file_settings.output_radar_original_points_flag = 1;
    software_settings.output_file_settings.output_radar_track_points_flag = 1;

    software_settings.system_adjust_settings.enter_debug_mode_flag = 0;
    software_settings.system_adjust_settings.auto_adjust_signal_processer_flag = 1;
}

void initializeDataProcessingSettings(DataProcessingSettings& data_processing_settings)
{
    data_processing_settings.radar_wave_paremeters[0].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[0].blind_area = 90;
    data_processing_settings.radar_wave_paremeters[0].doppler_unit = 0.44121;
    data_processing_settings.radar_wave_paremeters[0].mtd_num = 256;
    data_processing_settings.radar_wave_paremeters[0].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[0].decay0 = 48;
    data_processing_settings.radar_wave_paremeters[0].range_division_01 = 1000;
    data_processing_settings.radar_wave_paremeters[0].range_division_12 = 2000;
    data_processing_settings.radar_wave_paremeters[0].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[0].time_to_correct_azim = 83e-6 * 128 + 3500 * 271 * 20e-9;

    data_processing_settings.radar_wave_paremeters[1].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[1].blind_area = 1710;
    data_processing_settings.radar_wave_paremeters[1].doppler_unit = 0.44121;
    data_processing_settings.radar_wave_paremeters[1].mtd_num = 256;
    data_processing_settings.radar_wave_paremeters[1].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[1].decay0 = 15;
    data_processing_settings.radar_wave_paremeters[1].range_division_01 = 1710;
    data_processing_settings.radar_wave_paremeters[1].range_division_12 = 1710;
    data_processing_settings.radar_wave_paremeters[1].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[1].time_to_correct_azim = 83e-6 * 128 + 3500 * 271 * 20e-9;

    data_processing_settings.radar_wave_paremeters[2].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[2].blind_area = 7910;
    data_processing_settings.radar_wave_paremeters[2].doppler_unit = 0.25835;
    data_processing_settings.radar_wave_paremeters[2].mtd_num = 256;
    data_processing_settings.radar_wave_paremeters[2].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[2].decay0 = 15;
    data_processing_settings.radar_wave_paremeters[2].range_division_01 = 7910;
    data_processing_settings.radar_wave_paremeters[2].range_division_12 = 7910;
    data_processing_settings.radar_wave_paremeters[2].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[2].time_to_correct_azim = 140e-6 * 128 + 3500 * 271 * 20e-9;

    data_processing_settings.radar_wave_paremeters[3].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[3].blind_area = 210;
    data_processing_settings.radar_wave_paremeters[3].doppler_unit = 0.09155;
    data_processing_settings.radar_wave_paremeters[3].mtd_num = 1024;
    data_processing_settings.radar_wave_paremeters[3].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[3].decay0 = 48;
    data_processing_settings.radar_wave_paremeters[3].range_division_01 = 1000;
    data_processing_settings.radar_wave_paremeters[3].range_division_12 = 2000;
    data_processing_settings.radar_wave_paremeters[3].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[3].time_to_correct_azim = 100e-6 * 512 + 2048 * 1101 * 20e-9;

    data_processing_settings.radar_wave_paremeters[4].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[4].blind_area = 1710;
    data_processing_settings.radar_wave_paremeters[4].doppler_unit = 0.09155;
    data_processing_settings.radar_wave_paremeters[4].mtd_num = 1024;
    data_processing_settings.radar_wave_paremeters[4].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[4].decay0 = 15;
    data_processing_settings.radar_wave_paremeters[4].range_division_01 = 1710;
    data_processing_settings.radar_wave_paremeters[4].range_division_12 = 1710;
    data_processing_settings.radar_wave_paremeters[4].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[4].time_to_correct_azim = 100e-6 * 512 + 2048 * 1101 * 20e-9;

    data_processing_settings.radar_wave_paremeters[5].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[5].blind_area = 4104;
    data_processing_settings.radar_wave_paremeters[5].doppler_unit = 0.09155;
    data_processing_settings.radar_wave_paremeters[5].mtd_num = 1024;
    data_processing_settings.radar_wave_paremeters[5].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[5].decay0 = 15;
    data_processing_settings.radar_wave_paremeters[5].range_division_01 = 4104;
    data_processing_settings.radar_wave_paremeters[5].range_division_12 = 4104;
    data_processing_settings.radar_wave_paremeters[5].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[5].time_to_correct_azim = 100e-6 * 512 + 2048 * 1101 * 20e-9;

    data_processing_settings.radar_wave_paremeters[6].range_unit = 3;
    data_processing_settings.radar_wave_paremeters[6].blind_area = 5604;
    data_processing_settings.radar_wave_paremeters[6].doppler_unit = 0.09155;
    data_processing_settings.radar_wave_paremeters[6].mtd_num = 1024;
    data_processing_settings.radar_wave_paremeters[6].rcs_correction_coef = 1e-25;
    data_processing_settings.radar_wave_paremeters[6].decay0 = 15;
    data_processing_settings.radar_wave_paremeters[6].range_division_01 = 5604;
    data_processing_settings.radar_wave_paremeters[6].range_division_12 = 5604;
    data_processing_settings.radar_wave_paremeters[6].decay2 = 15;
    data_processing_settings.radar_wave_paremeters[6].time_to_correct_azim = 100e-6 * 512 + 2048 * 1101 * 20e-9;

    for (int ii = 0; ii < ELEV_BEAM_NUM_SCAN_MODE; ++ii)
    {
        data_processing_settings.sigma_delta_elev_slope[0].sigma_delta_slope[ii] = 1.9;
        data_processing_settings.sigma_delta_elev_slope[1].sigma_delta_slope[ii] = 3.4;
    }

    data_processing_settings.sigma_delta_parameters[0].sigma_delta_flag = 1;
    data_processing_settings.sigma_delta_parameters[0].weight_flag = 0;
    data_processing_settings.sigma_delta_parameters[0].sigma_delta_slope = 2.6;
    data_processing_settings.sigma_delta_parameters[0].negative_bias_phase_floor = 0;
    data_processing_settings.sigma_delta_parameters[0].negative_bias_phase_ceil = 180;
    data_processing_settings.sigma_delta_parameters[0].beam_width = 4;
    data_processing_settings.sigma_delta_parameters[0].max_bias_angle = 2.2;
    data_processing_settings.sigma_delta_parameters[0].sigma_delta_filter_flag = 0;
    data_processing_settings.sigma_delta_parameters[0].sigma_delta_filter_ratio = 1.2;
    data_processing_settings.sigma_delta_parameters[1].sigma_delta_flag = 1;
    data_processing_settings.sigma_delta_parameters[1].weight_flag = 0;
    data_processing_settings.sigma_delta_parameters[1].sigma_delta_slope = 1.9;
    data_processing_settings.sigma_delta_parameters[1].negative_bias_phase_floor = -180;
    data_processing_settings.sigma_delta_parameters[1].negative_bias_phase_ceil = 0;
    data_processing_settings.sigma_delta_parameters[1].beam_width = 3;
    data_processing_settings.sigma_delta_parameters[1].max_bias_angle = 1.7;
    data_processing_settings.sigma_delta_parameters[1].sigma_delta_filter_flag = 0;
    data_processing_settings.sigma_delta_parameters[1].sigma_delta_filter_ratio = 1.2;
    data_processing_settings.sigma_delta_parameters[2].sigma_delta_flag = 1;
    data_processing_settings.sigma_delta_parameters[2].weight_flag = 0;
    data_processing_settings.sigma_delta_parameters[2].sigma_delta_slope = 3.4;
    data_processing_settings.sigma_delta_parameters[2].negative_bias_phase_floor = -180;
    data_processing_settings.sigma_delta_parameters[2].negative_bias_phase_ceil = 0;
    data_processing_settings.sigma_delta_parameters[2].beam_width = 3.5;
    data_processing_settings.sigma_delta_parameters[2].max_bias_angle = 1.95;
    data_processing_settings.sigma_delta_parameters[2].sigma_delta_filter_flag = 0;
    data_processing_settings.sigma_delta_parameters[2].sigma_delta_filter_ratio = 1.2;

    data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle = 0;
    data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle = 0; // 最低俯仰波束中心默认是1度
    data_processing_settings.azim_elev_height_bias_settings.height_bias = 0;
    data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag = 0;
    data_processing_settings.radar_lon_lat_alt.radar_longitude = 0;
    data_processing_settings.radar_lon_lat_alt.radar_latitude = 0;
    data_processing_settings.radar_lon_lat_alt.radar_altitude = 0;
    data_processing_settings.selected_area_settings.select_radar_mode_flag = 0;
    data_processing_settings.selected_area_settings.selected_radar_mode_floor = 0;
    data_processing_settings.selected_area_settings.selected_radar_mode_ceil = 4;
    data_processing_settings.selected_area_settings.select_range_flag = 0;
    data_processing_settings.selected_area_settings.selected_range_floor = 50;
    data_processing_settings.selected_area_settings.selected_range_ceil = 15000;
    data_processing_settings.selected_area_settings.select_azim_flag = 0;
    data_processing_settings.selected_area_settings.selected_azim_deg_floor = 0;
    data_processing_settings.selected_area_settings.selected_azim_deg_ceil = 360;
    data_processing_settings.selected_area_settings.select_elev_flag = 0;
    data_processing_settings.selected_area_settings.selected_elev_deg_floor = 0;
    data_processing_settings.selected_area_settings.selected_elev_deg_ceil = 90;
    data_processing_settings.selected_area_settings.select_velocity_flag = 0;
    data_processing_settings.selected_area_settings.selected_velocity_floor = 0;
    data_processing_settings.selected_area_settings.selected_velocity_ceil = 100;
    data_processing_settings.selected_area_settings.select_height_flag = 0;
    data_processing_settings.selected_area_settings.selected_height_floor = 0;
    data_processing_settings.selected_area_settings.selected_height_ceil = 1000;
    data_processing_settings.selected_area_settings.select_amp_flag = 0;
    data_processing_settings.selected_area_settings.selected_amp_floor = 0;
    data_processing_settings.selected_area_settings.selected_amp_ceil = 100000000;

    for (int ii = 0; ii < EXCLUDE_SETTINGS_NUM; ++ii)
    {
        data_processing_settings.exclude_settings[ii].is_selected_flag = 0;
        data_processing_settings.exclude_settings[ii].radar_mode_min = 0;
        data_processing_settings.exclude_settings[ii].radar_mode_max = 4;
        data_processing_settings.exclude_settings[ii].range_min = 0;
        data_processing_settings.exclude_settings[ii].range_max = 20e3;
        data_processing_settings.exclude_settings[ii].azim_deg_min = 0;
        data_processing_settings.exclude_settings[ii].azim_deg_max = 360;
        data_processing_settings.exclude_settings[ii].elev_deg_min = 0;
        data_processing_settings.exclude_settings[ii].elev_deg_max = 90;
        data_processing_settings.exclude_settings[ii].velocity_min = -1000;
        data_processing_settings.exclude_settings[ii].velocity_max = 1000;
        data_processing_settings.exclude_settings[ii].amp_min = 0;
        data_processing_settings.exclude_settings[ii].amp_max = 10000000;
        data_processing_settings.exclude_settings[ii].height_min = 0;
        data_processing_settings.exclude_settings[ii].height_max = 1000;
    }

    data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag = 0;
    data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio = 1.2;
    data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag = 0;
    data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio = 1.2;

    data_processing_settings.clutter_map_settings.clutter_map_filter_flag = 1;
    data_processing_settings.clutter_map_settings.clutter_map_generate_weight = 0.125;
    data_processing_settings.clutter_map_settings.clutter_map_filter_threshold = 10;

    data_processing_settings.filter_cloud_settings.enable_filter_cloud = 0;
    data_processing_settings.filter_cloud_settings.filter_cloud_threshold = 20;

    data_processing_settings.combine_thresholds.combine_range_unit_threshold = 3;
    data_processing_settings.combine_thresholds.combine_doppler_unit_threshold = 4;
    data_processing_settings.combine_thresholds.combine_range_threshold = 9;
    data_processing_settings.combine_thresholds.combine_velocity_threshold = 2;
    data_processing_settings.combine_thresholds.combine_azim_threshold_deg = 3 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.combine_thresholds.combine_elev_threshold_deg = 3 * data_processing_settings.sigma_delta_parameters[1].beam_width;

    data_processing_settings.correlate_thresholds[0].correlate_range_threshold_floor = 6;
    data_processing_settings.correlate_thresholds[0].correlate_range_threshold_ceil = 15;
    data_processing_settings.correlate_thresholds[0].correlate_velocity_threshold_same_direction = 6;
    data_processing_settings.correlate_thresholds[0].correlate_velocity_threshold_opposite_direction = 15;
    data_processing_settings.correlate_thresholds[0].correlate_azim_threshold_deg_0 = 3 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.correlate_thresholds[0].correlate_elev_threshold_deg_0 = 3 * data_processing_settings.sigma_delta_parameters[1].beam_width;
    data_processing_settings.correlate_thresholds[0].correlate_range_division_01 = 1000;
    data_processing_settings.correlate_thresholds[0].correlate_azim_threshold_deg_1 = 3 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.correlate_thresholds[0].correlate_elev_threshold_deg_1 = 3 * data_processing_settings.sigma_delta_parameters[1].beam_width;
    data_processing_settings.correlate_thresholds[0].correlate_range_division_12 = 2000;
    data_processing_settings.correlate_thresholds[0].correlate_azim_threshold_deg_2 = 3 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.correlate_thresholds[0].correlate_elev_threshold_deg_2 = 3 * data_processing_settings.sigma_delta_parameters[1].beam_width;
    data_processing_settings.correlate_thresholds[0].correlate_x_threshold = 25;
    data_processing_settings.correlate_thresholds[0].correlate_y_threshold = 25;
    data_processing_settings.correlate_thresholds[0].correlate_z_threshold = 25;
    data_processing_settings.correlate_thresholds[0].correlate_vx_threshold = 6;
    data_processing_settings.correlate_thresholds[0].correlate_vy_threshold = 6;
    data_processing_settings.correlate_thresholds[0].correlate_vz_threshold = 6;
    data_processing_settings.correlate_thresholds[1].correlate_range_threshold_floor = 3;
    data_processing_settings.correlate_thresholds[1].correlate_range_threshold_ceil = 10;
    data_processing_settings.correlate_thresholds[1].correlate_velocity_threshold_same_direction = 3;
    data_processing_settings.correlate_thresholds[1].correlate_velocity_threshold_opposite_direction = 6;
    data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_0 = 2.5 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_0 = 2.5 * data_processing_settings.sigma_delta_parameters[1].beam_width;
    data_processing_settings.correlate_thresholds[1].correlate_range_division_01 = 1000;
    data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_1 = 2.5 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_1 = 2.5 * data_processing_settings.sigma_delta_parameters[1].beam_width;
    data_processing_settings.correlate_thresholds[1].correlate_range_division_12 = 2000;
    data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_2 = 1.5 * data_processing_settings.sigma_delta_parameters[0].beam_width;
    data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_2 = 2.5 * data_processing_settings.sigma_delta_parameters[1].beam_width;
    data_processing_settings.correlate_thresholds[1].correlate_x_threshold = 10;
    data_processing_settings.correlate_thresholds[1].correlate_y_threshold = 10;
    data_processing_settings.correlate_thresholds[1].correlate_z_threshold = 10;
    data_processing_settings.correlate_thresholds[1].correlate_vx_threshold = 0.5;
    data_processing_settings.correlate_thresholds[1].correlate_vy_threshold = 0.5;
    data_processing_settings.correlate_thresholds[1].correlate_vz_threshold = 0.5;

    data_processing_settings.smooth_track_settings[0].filter_track_method = 0;
    data_processing_settings.smooth_track_settings[0].filter_track_coef_alpha = 0.5;
    data_processing_settings.smooth_track_settings[0].filter_track_coef_beta = 0.3;
    data_processing_settings.smooth_track_settings[0].filter_track_coef_gama = 0;
    data_processing_settings.smooth_track_settings[0].smooth_track_flag = 0;

    data_processing_settings.smooth_track_settings[1].filter_track_method = 0;
    data_processing_settings.smooth_track_settings[1].filter_track_coef_alpha = 0.5;
    data_processing_settings.smooth_track_settings[1].filter_track_coef_beta = 0.3;
    data_processing_settings.smooth_track_settings[1].filter_track_coef_gama = 0;
    data_processing_settings.smooth_track_settings[1].smooth_track_flag = 0;

    data_processing_settings.filter_tracks_settings.filter_tracks_flag = 0;
    data_processing_settings.filter_tracks_settings.filter_tracks_range_threshold = 10;
    data_processing_settings.filter_tracks_settings.filter_tracks_velocity_threshold = 1;
    data_processing_settings.filter_tracks_settings.filter_tracks_mean_delta_range_threshold = 5;
}

void initializeDbfCorrectCoefFileDirSettings(DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings)
{
    dbf_correct_coef_dir_settings.master_external_file_dir = DEFAULT_MASTER_EXTERNAL_FILE_DIR;
    dbf_correct_coef_dir_settings.slave_external_file_dir = DEFAULT_SLAVE_EXTERNAL_FILE_DIR;
    dbf_correct_coef_dir_settings.master_internal_file_dir = DEFAULT_MASTER_INTERNAL_FILE_DIR;
    dbf_correct_coef_dir_settings.slave_internal_file_dir = DEFAULT_SLAVE_INTERNAL_FILE_DIR;
}

void initializeHardwareAndSoftwareAndDataProcessingSettingsBasedOnConfigFile(HardwareSettings& hardware_settings,
                                                                             SoftwareSettings& software_settings,
                                                                             DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings,
                                                                             DataProcessingSettings& data_processing_settings,
                                                                             QSettings* settings_in_config_file)
{
    initializeHardwareSettings(hardware_settings);
    initializeSoftwareSettings(software_settings);
    initializeDataProcessingSettings(data_processing_settings);
    initializeDbfCorrectCoefFileDirSettings(dbf_correct_coef_dir_settings);
    QFile file(CONFIG_FILE_NAME);
    if (file.exists())
    {
        // 如果文件存在, 只将其赋给 settings_in_config_file, 并且更新 hardware_settings 和 software_settings
        initializeHardwareSettingsBasedOnConfigFile(hardware_settings, settings_in_config_file);
        initializeSoftwareSettingsBasedOnConfigFile(software_settings, settings_in_config_file);
        initializeDbfCorrectCoefFileDirSettingsBasedOnConfigFile(dbf_correct_coef_dir_settings, settings_in_config_file);
        initializeDataProcessingSettingsBasedOnInConfigFile(data_processing_settings, settings_in_config_file);
    }
    else
    {
        // 如果文件不存在, 则初始化这个文件
        updateHardwareParametersInConfigFileBasedOnHardwareSettings(hardware_settings, settings_in_config_file);
        updateSoftwareParametersInConfigFileBasedOnSoftwareSettings(software_settings, settings_in_config_file);
        updateDbfCorrectCoefFileDirParametersInConfigFileBasedOnDbfCorrectCoefFileDirSettings(dbf_correct_coef_dir_settings, settings_in_config_file);
        updateDataProcessingParametersInConfigFileBasedOnDataProcessingSetting(data_processing_settings, settings_in_config_file);
    }
}

void initializeHardwareSettingsBasedOnConfigFile(HardwareSettings& hardware_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_hardware_settings = QString("hardware_settings/");

        // 工作模式
        hardware_settings.antenna_enable = settings_in_config_file->value(string_hardware_settings + QString("antenna_enable"), hardware_settings.antenna_enable).toInt();
        hardware_settings.work_or_serdes = settings_in_config_file->value(string_hardware_settings + QString("work_or_serdes"), hardware_settings.work_or_serdes).toInt();
        hardware_settings.work_or_emulate = settings_in_config_file->value(string_hardware_settings + QString("work_or_emulate"), hardware_settings.work_or_emulate).toInt();

        // 天线相关
        hardware_settings.work_mode_or_correct_mode =
            settings_in_config_file->value(string_hardware_settings + QString("work_mode_or_correct_mode"), hardware_settings.work_mode_or_correct_mode).toInt();
        hardware_settings.antennna_fre_or_amp_config =
            settings_in_config_file->value(string_hardware_settings + QString("antennna_fre_or_amp_config"), hardware_settings.antennna_fre_or_amp_config).toInt();
        hardware_settings.antenna_weight_method =
            settings_in_config_file->value(string_hardware_settings + QString("antenna_weight_method"), hardware_settings.antenna_weight_method).toInt();
        hardware_settings.antennna_freq_code =
            settings_in_config_file->value(string_hardware_settings + QString("antennna_freq_code"), hardware_settings.antennna_freq_code).toInt();
        hardware_settings.elev_support_angle =
            settings_in_config_file->value(string_hardware_settings + QString("elev_support_angle"), hardware_settings.elev_support_angle).toInt();
        hardware_settings.antenna_raising_angle =
            settings_in_config_file->value(string_hardware_settings + QString("antenna_raising_angle"), hardware_settings.antenna_raising_angle).toInt();
        hardware_settings.sigma_delta_decay_1 =
            settings_in_config_file->value(string_hardware_settings + QString("sigma_delta_decay_1"), hardware_settings.sigma_delta_decay_1).toInt();
        hardware_settings.protect_decay_1 = settings_in_config_file->value(string_hardware_settings + QString("protect_decay_1"), hardware_settings.protect_decay_1).toInt();
        hardware_settings.sigma_delta_decay_2 =
            settings_in_config_file->value(string_hardware_settings + QString("sigma_delta_decay_2"), hardware_settings.sigma_delta_decay_2).toInt();
        hardware_settings.protect_decay_2 = settings_in_config_file->value(string_hardware_settings + QString("protect_decay_2"), hardware_settings.protect_decay_2).toInt();

        // DBF 相关
        hardware_settings.amp_code = settings_in_config_file->value(string_hardware_settings + QString("amp_code"), hardware_settings.amp_code).toInt();
        hardware_settings.channel_amp_phase_control_mode =
            settings_in_config_file->value(string_hardware_settings + QString("channel_amp_phase_control_mode"), hardware_settings.channel_amp_phase_control_mode).toInt();
        hardware_settings.channel_amp_phase_control_code =
            settings_in_config_file->value(string_hardware_settings + QString("channel_amp_phase_control_code"), hardware_settings.channel_amp_phase_control_code).toInt();
        hardware_settings.channel_num = settings_in_config_file->value(string_hardware_settings + QString("channel_num"), hardware_settings.channel_num).toInt();
        hardware_settings.dbf_coe_choose = settings_in_config_file->value(string_hardware_settings + QString("dbf_coe_choose"), hardware_settings.dbf_coe_choose).toInt();
        hardware_settings.dbf_coef_receive_raising_angle =
            settings_in_config_file->value(string_hardware_settings + QString("dbf_coef_receive_raising_angle"), hardware_settings.dbf_coef_receive_raising_angle).toInt();

        // 转台相关
        hardware_settings.turntable_rotating_mode =
            settings_in_config_file->value(string_hardware_settings + QString("turntable_rotating_mode"), hardware_settings.turntable_rotating_mode).toInt();
        for (int ii = 0; ii < RADAR_MISSION_NUM; ++ii)
        {
            hardware_settings.spr_map[ii] =
                settings_in_config_file->value(string_hardware_settings + QString("spr_map" + QString::number(ii)), hardware_settings.spr_map[ii]).toDouble();
        }
        hardware_settings.turntable_rotating_speed =
            settings_in_config_file->value(string_hardware_settings + QString("turntable_rotating_speed"), hardware_settings.turntable_rotating_speed).toDouble();
        hardware_settings.turntable_pointing_angle =
            settings_in_config_file->value(string_hardware_settings + QString("turntable_pointing_angle"), hardware_settings.turntable_pointing_angle).toDouble();

        hardware_settings.stc_select = settings_in_config_file->value(string_hardware_settings + QString("stc_select"), hardware_settings.stc_select).toInt();
        hardware_settings.remove_dc_flag = settings_in_config_file->value(string_hardware_settings + QString("remove_dc_flag"), hardware_settings.remove_dc_flag).toInt();
        hardware_settings.cfar_method = settings_in_config_file->value(string_hardware_settings + QString("cfar_method"), hardware_settings.cfar_method).toInt();
        for (int ii = 0; ii < THRESHOLDS_NUM; ++ii)
        {
            hardware_settings.thresholds[ii].doppler_threshold =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".doppler_threshold"), hardware_settings.thresholds[ii].doppler_threshold)
                    .toInt();
            hardware_settings.thresholds[ii].range_threshold_division_range_index_n_f =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".range_threshold_division_range_index_n_f"),
                            hardware_settings.thresholds[ii].range_threshold_division_range_index_n_f)
                    .toInt();
            hardware_settings.thresholds[ii].range_threshold_near =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".range_threshold_near"), hardware_settings.thresholds[ii].range_threshold_near)
                    .toInt();
            hardware_settings.thresholds[ii].range_threshold_far =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".range_threshold_far"), hardware_settings.thresholds[ii].range_threshold_far)
                    .toInt();
            hardware_settings.thresholds[ii].fft_cut =
                settings_in_config_file->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".fft_cut"), hardware_settings.thresholds[ii].fft_cut)
                    .toInt();
            hardware_settings.thresholds[ii].damp_cut_division_range_index =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".damp_cut_division_range_index"),
                            hardware_settings.thresholds[ii].damp_cut_division_range_index)
                    .toInt();
            hardware_settings.thresholds[ii].damp_cut_near =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".damp_cut_near"), hardware_settings.thresholds[ii].damp_cut_near)
                    .toInt();
            hardware_settings.thresholds[ii].damp_cut_far =
                settings_in_config_file
                    ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".damp_cut_far"), hardware_settings.thresholds[ii].damp_cut_far)
                    .toInt();

            for (int jj = 0; jj < DOPPLER_EXCLUDE_NUM; ++jj)
            {
                hardware_settings.thresholds[ii].doppler_exclude[jj] =
                    settings_in_config_file
                        ->value(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".doppler_exclude" + QString::number(jj)),
                                hardware_settings.thresholds[ii].doppler_exclude[jj])
                        .toInt();
            }
        }

        hardware_settings.power_off_area_settings.enable_power_off_area_flag = settings_in_config_file
                                                                                   ->value(string_hardware_settings + QString("power_off_area_settings.enable_power_off_area_flag"),
                                                                                           hardware_settings.power_off_area_settings.enable_power_off_area_flag)
                                                                                   .toInt();
        hardware_settings.power_off_area_settings.power_off_azim_deg_floor =
            settings_in_config_file
                ->value(string_hardware_settings + QString("power_off_area_settings.power_off_azim_deg_floor"), hardware_settings.power_off_area_settings.power_off_azim_deg_floor)
                .toInt();
        hardware_settings.power_off_area_settings.power_off_azim_deg_ceil =
            settings_in_config_file
                ->value(string_hardware_settings + QString("power_off_area_settings.power_off_azim_deg_ceil"), hardware_settings.power_off_area_settings.power_off_azim_deg_ceil)
                .toInt();
    }
}

void initializeSoftwareSettingsBasedOnConfigFile(SoftwareSettings& software_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_software_settings = QString("software_settings/");

        software_settings.communication_settings.adapter_index_communicating_with_fpga =
            settings_in_config_file
                ->value(string_software_settings + QString("communication_settings.adapter_index_communicating_with_fpga"),
                        software_settings.communication_settings.adapter_index_communicating_with_fpga)
                .toInt();
        software_settings.communication_settings.output_device_id =
            settings_in_config_file->value(string_software_settings + QString("communication_settings.output_device_id"), software_settings.communication_settings.output_device_id)
                .toInt();
        software_settings.communication_settings.output_min_track_times =
            settings_in_config_file
                ->value(string_software_settings + QString("communication_settings.output_min_track_times"), software_settings.communication_settings.output_min_track_times)
                .toInt();
        software_settings.communication_settings.output_tracks_through_adapter_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("communication_settings.output_tracks_through_adapter_flag"),
                        software_settings.communication_settings.output_tracks_through_adapter_flag)
                .toInt();
        software_settings.communication_settings.output_selected_or_all_method =
            settings_in_config_file
                ->value(string_software_settings + QString("communication_settings.output_selected_or_all_method"),
                        software_settings.communication_settings.output_selected_or_all_method)
                .toInt();
        software_settings.communication_settings.output_azim_north_or_east_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("communication_settings.output_azim_north_or_east_flag"),
                        software_settings.communication_settings.output_azim_north_or_east_flag)
                .toInt();
        software_settings.communication_settings.output_IP_address =
            QHostAddress(settings_in_config_file
                             ->value(string_software_settings + QString("communication_settings.output_IP_address"), software_settings.communication_settings.output_IP_address)
                             .toString())
                .toIPv4Address();
        software_settings.communication_settings.output_port =
            settings_in_config_file->value(string_software_settings + QString("communication_settings.output_port"), software_settings.communication_settings.output_port).toInt();

        software_settings.communication_settings.input_packets_from_other_device_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("communication_settings.input_packets_from_other_device_flag"),
                        software_settings.communication_settings.input_packets_from_other_device_flag)
                .toInt();
        software_settings.communication_settings.input_port =
            settings_in_config_file->value(string_software_settings + QString("communication_settings.input_port"), software_settings.communication_settings.input_port).toInt();

        software_settings.display_settings.display_system_error_info_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("display_settings.display_system_error_info_flag"), software_settings.display_settings.display_system_error_info_flag)
                .toInt();
        software_settings.display_settings.display_original_points_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("display_settings.display_original_points_flag"), software_settings.display_settings.display_original_points_flag)
                .toInt();
        software_settings.display_settings.display_filtered_points_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("display_settings.display_filtered_points_flag"), software_settings.display_settings.display_filtered_points_flag)
                .toInt();
        software_settings.display_settings.display_tracks_flag =
            settings_in_config_file->value(string_software_settings + QString("display_settings.display_tracks_flag"), software_settings.display_settings.display_tracks_flag)
                .toInt();
        software_settings.display_settings.display_area_radius =
            settings_in_config_file->value(string_software_settings + QString("display_settings.display_area_radius"), software_settings.display_settings.display_area_radius)
                .toDouble();
        software_settings.display_settings.display_circles_gap =
            settings_in_config_file->value(string_software_settings + QString("display_settings.display_circles_gap"), software_settings.display_settings.display_circles_gap)
                .toDouble();
        software_settings.display_settings.display_key_targets_track_times =
            settings_in_config_file
                ->value(string_software_settings + QString("display_settings.display_key_targets_track_times"), software_settings.display_settings.display_key_targets_track_times)
                .toInt();

        software_settings.output_file_settings.output_fpga_packet_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("output_file_settings.output_fpga_packet_flag"), software_settings.output_file_settings.output_fpga_packet_flag)
                .toInt();
        software_settings.output_file_settings.output_radar_system_info_flag = settings_in_config_file
                                                                                   ->value(string_software_settings + QString("output_file_settings.output_radar_system_info_flag"),
                                                                                           software_settings.output_file_settings.output_radar_system_info_flag)
                                                                                   .toInt();
        software_settings.output_file_settings.output_radar_original_points_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("output_file_settings.output_radar_original_points_flag"),
                        software_settings.output_file_settings.output_radar_original_points_flag)
                .toInt();
        software_settings.output_file_settings.output_radar_track_points_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("output_file_settings.output_radar_track_points_flag"),
                        software_settings.output_file_settings.output_radar_track_points_flag)
                .toInt();

        software_settings.system_adjust_settings.enter_debug_mode_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("system_adjust_settings.enter_debug_mode_flag"), software_settings.system_adjust_settings.enter_debug_mode_flag)
                .toInt();
        software_settings.system_adjust_settings.auto_adjust_signal_processer_flag =
            settings_in_config_file
                ->value(string_software_settings + QString("system_adjust_settings.auto_adjust_signal_processer_flag"),
                        software_settings.system_adjust_settings.auto_adjust_signal_processer_flag)
                .toInt();
    }
}

void initializeDbfCorrectCoefFileDirSettingsBasedOnConfigFile(DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_dbf_correct_coef_dir_settings = QString("dbf_correct_coef_dir_settings/");

        dbf_correct_coef_dir_settings.master_external_file_dir =
            settings_in_config_file->value(string_dbf_correct_coef_dir_settings + QString("master_external_file_dir"), dbf_correct_coef_dir_settings.master_external_file_dir)
                .toString();
        dbf_correct_coef_dir_settings.slave_external_file_dir =
            settings_in_config_file->value(string_dbf_correct_coef_dir_settings + QString("slave_external_file_dir"), dbf_correct_coef_dir_settings.slave_external_file_dir)
                .toString();
        dbf_correct_coef_dir_settings.master_internal_file_dir =
            settings_in_config_file->value(string_dbf_correct_coef_dir_settings + QString("master_internal_file_dir"), dbf_correct_coef_dir_settings.master_internal_file_dir)
                .toString();
        dbf_correct_coef_dir_settings.slave_internal_file_dir =
            settings_in_config_file->value(string_dbf_correct_coef_dir_settings + QString("slave_internal_file_dir"), dbf_correct_coef_dir_settings.slave_internal_file_dir)
                .toString();
    }
}

void initializeDataProcessingSettingsBasedOnInConfigFile(DataProcessingSettings& data_processing_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {

        QString string_data_processing_settings = QString("data_processing_settings/");

        for (int ii = 0; ii < RADAR_MODE_NUM; ++ii)
        {
            data_processing_settings.radar_wave_paremeters[ii].range_unit =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".range_unit"),
                            data_processing_settings.radar_wave_paremeters[ii].range_unit)
                    .toDouble();
            data_processing_settings.radar_wave_paremeters[ii].blind_area =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".blind_area"),
                            data_processing_settings.radar_wave_paremeters[ii].blind_area)
                    .toDouble();
            data_processing_settings.radar_wave_paremeters[ii].doppler_unit =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".doppler_unit"),
                            data_processing_settings.radar_wave_paremeters[ii].doppler_unit)
                    .toDouble();
            data_processing_settings.radar_wave_paremeters[ii].mtd_num =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".mtd_num"),
                            data_processing_settings.radar_wave_paremeters[ii].mtd_num)
                    .toInt();
            data_processing_settings.radar_wave_paremeters[ii].rcs_correction_coef =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".rcs_correction_coef"),
                            data_processing_settings.radar_wave_paremeters[ii].rcs_correction_coef)
                    .toDouble();
            data_processing_settings.radar_wave_paremeters[ii].decay0 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".decay0"),
                            data_processing_settings.radar_wave_paremeters[ii].decay0)
                    .toInt();
            data_processing_settings.radar_wave_paremeters[ii].range_division_01 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".range_division_01"),
                            data_processing_settings.radar_wave_paremeters[ii].range_division_01)
                    .toDouble();
            data_processing_settings.radar_wave_paremeters[ii].range_division_12 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".range_division_12"),
                            data_processing_settings.radar_wave_paremeters[ii].range_division_12)
                    .toDouble();
            data_processing_settings.radar_wave_paremeters[ii].decay2 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".decay2"),
                            data_processing_settings.radar_wave_paremeters[ii].decay2)
                    .toInt();
            data_processing_settings.radar_wave_paremeters[ii].time_to_correct_azim =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".time_to_correct_azim"),
                            data_processing_settings.radar_wave_paremeters[ii].time_to_correct_azim)
                    .toDouble();
        }

        for (int ii = 0; ii < 2; ++ii)
        {
            for (int jj = 0; jj < ELEV_BEAM_NUM_SCAN_MODE; ++jj)
            {
                data_processing_settings.sigma_delta_elev_slope[ii].sigma_delta_slope[jj] =
                    settings_in_config_file
                        ->value(string_data_processing_settings + QString("sigma_delta_elev_slope" + QString::number(ii) + ".sigma_delta_slope" + QString::number(jj)),
                                data_processing_settings.sigma_delta_elev_slope[ii].sigma_delta_slope[jj])
                        .toDouble();
            }
        }

        for (int ii = 0; ii < SIGMA_DELTA_SETTINGS_NUM; ++ii)
        {
            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_flag"),
                            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag)
                    .toInt();
            data_processing_settings.sigma_delta_parameters[ii].weight_flag =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".weight_flag"),
                            data_processing_settings.sigma_delta_parameters[ii].weight_flag)
                    .toInt();
            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_slope =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_slope"),
                            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_slope)
                    .toDouble();
            data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_floor =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".negative_bias_phase_floor"),
                            data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_floor)
                    .toDouble();
            data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_ceil =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".negative_bias_phase_ceil"),
                            data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_ceil)
                    .toDouble();
            data_processing_settings.sigma_delta_parameters[ii].beam_width =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".beam_width"),
                            data_processing_settings.sigma_delta_parameters[ii].beam_width)
                    .toDouble();
            data_processing_settings.sigma_delta_parameters[ii].max_bias_angle =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".max_bias_angle"),
                            data_processing_settings.sigma_delta_parameters[ii].max_bias_angle)
                    .toDouble();
            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_filter_flag"),
                            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag)
                    .toInt();
            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_ratio =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_filter_ratio"),
                            data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_ratio)
                    .toDouble();
        }

        data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag = settings_in_config_file
                                                                                  ->value(string_data_processing_settings + QString("radar_lon_lat_alt.convert_lon_lat_alt_flag"),
                                                                                          data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag)
                                                                                  .toInt();
        data_processing_settings.radar_lon_lat_alt.radar_longitude =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("radar_lon_lat_alt.radar_longitude"), data_processing_settings.radar_lon_lat_alt.radar_longitude)
                .toDouble();
        data_processing_settings.radar_lon_lat_alt.radar_latitude =
            settings_in_config_file->value(string_data_processing_settings + QString("radar_lon_lat_alt.radar_latitude"), data_processing_settings.radar_lon_lat_alt.radar_latitude)
                .toDouble();
        data_processing_settings.radar_lon_lat_alt.radar_altitude =
            settings_in_config_file->value(string_data_processing_settings + QString("radar_lon_lat_alt.radar_altitude"), data_processing_settings.radar_lon_lat_alt.radar_altitude)
                .toDouble();

        data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("azim_elev_height_bias_settings.azim_bias_angle"),
                        data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle)
                .toDouble();
        data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("azim_elev_height_bias_settings.elev_bias_angle"),
                        data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle)
                .toDouble();
        data_processing_settings.azim_elev_height_bias_settings.height_bias = settings_in_config_file
                                                                                  ->value(string_data_processing_settings + QString("azim_elev_height_bias_settings.height_bias"),
                                                                                          data_processing_settings.azim_elev_height_bias_settings.height_bias)
                                                                                  .toDouble();

        data_processing_settings.selected_area_settings.select_radar_mode_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.select_radar_mode_flag"),
                        data_processing_settings.selected_area_settings.select_radar_mode_flag)
                .toInt();
        data_processing_settings.selected_area_settings.selected_radar_mode_floor =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_radar_mode_floor"),
                        data_processing_settings.selected_area_settings.selected_radar_mode_floor)
                .toInt();
        data_processing_settings.selected_area_settings.selected_radar_mode_ceil =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_radar_mode_ceil"),
                        data_processing_settings.selected_area_settings.selected_radar_mode_ceil)
                .toInt();
        data_processing_settings.selected_area_settings.select_range_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.select_range_flag"), data_processing_settings.selected_area_settings.select_range_flag)
                .toInt();
        data_processing_settings.selected_area_settings.selected_range_floor = settings_in_config_file
                                                                                   ->value(string_data_processing_settings + QString("selected_area_settings.selected_range_floor"),
                                                                                           data_processing_settings.selected_area_settings.selected_range_floor)
                                                                                   .toDouble();
        data_processing_settings.selected_area_settings.selected_range_ceil = settings_in_config_file
                                                                                  ->value(string_data_processing_settings + QString("selected_area_settings.selected_range_ceil"),
                                                                                          data_processing_settings.selected_area_settings.selected_range_ceil)
                                                                                  .toDouble();
        data_processing_settings.selected_area_settings.select_azim_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.select_azim_flag"), data_processing_settings.selected_area_settings.select_azim_flag)
                .toInt();
        data_processing_settings.selected_area_settings.selected_azim_deg_floor =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_azim_deg_floor"),
                        data_processing_settings.selected_area_settings.selected_azim_deg_floor)
                .toDouble();
        data_processing_settings.selected_area_settings.selected_azim_deg_ceil =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_azim_deg_ceil"),
                        data_processing_settings.selected_area_settings.selected_azim_deg_ceil)
                .toDouble();
        data_processing_settings.selected_area_settings.select_elev_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.select_elev_flag"), data_processing_settings.selected_area_settings.select_elev_flag)
                .toInt();
        data_processing_settings.selected_area_settings.selected_elev_deg_floor =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_elev_deg_floor"),
                        data_processing_settings.selected_area_settings.selected_elev_deg_floor)
                .toDouble();
        data_processing_settings.selected_area_settings.selected_elev_deg_ceil =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_elev_deg_ceil"),
                        data_processing_settings.selected_area_settings.selected_elev_deg_ceil)
                .toDouble();
        data_processing_settings.selected_area_settings.select_velocity_flag = settings_in_config_file
                                                                                   ->value(string_data_processing_settings + QString("selected_area_settings.select_velocity_flag"),
                                                                                           data_processing_settings.selected_area_settings.select_velocity_flag)
                                                                                   .toInt();
        data_processing_settings.selected_area_settings.selected_velocity_floor =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_velocity_floor"),
                        data_processing_settings.selected_area_settings.selected_velocity_floor)
                .toDouble();
        data_processing_settings.selected_area_settings.selected_velocity_ceil =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_velocity_ceil"),
                        data_processing_settings.selected_area_settings.selected_velocity_ceil)
                .toDouble();
        data_processing_settings.selected_area_settings.select_height_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.select_height_flag"), data_processing_settings.selected_area_settings.select_height_flag)
                .toInt();
        data_processing_settings.selected_area_settings.selected_height_floor =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_height_floor"),
                        data_processing_settings.selected_area_settings.selected_height_floor)
                .toDouble();
        data_processing_settings.selected_area_settings.selected_height_ceil = settings_in_config_file
                                                                                   ->value(string_data_processing_settings + QString("selected_area_settings.selected_height_ceil"),
                                                                                           data_processing_settings.selected_area_settings.selected_height_ceil)
                                                                                   .toDouble();
        data_processing_settings.selected_area_settings.select_amp_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.select_amp_flag"), data_processing_settings.selected_area_settings.select_amp_flag)
                .toInt();
        data_processing_settings.selected_area_settings.selected_amp_floor =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_amp_floor"), data_processing_settings.selected_area_settings.selected_amp_floor)
                .toInt();
        data_processing_settings.selected_area_settings.selected_amp_ceil =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("selected_area_settings.selected_amp_ceil"), data_processing_settings.selected_area_settings.selected_amp_ceil)
                .toInt();

        data_processing_settings.clutter_map_settings.clutter_map_filter_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("clutter_map_settings.clutter_map_filter_flag"),
                        data_processing_settings.clutter_map_settings.clutter_map_filter_flag)
                .toInt();
        data_processing_settings.clutter_map_settings.clutter_map_generate_weight =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("clutter_map_settings.clutter_map_generate_weight"),
                        data_processing_settings.clutter_map_settings.clutter_map_generate_weight)
                .toDouble();
        data_processing_settings.clutter_map_settings.clutter_map_filter_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("clutter_map_settings.clutter_map_filter_threshold"),
                        data_processing_settings.clutter_map_settings.clutter_map_filter_threshold)
                .toDouble();

        data_processing_settings.filter_cloud_settings.enable_filter_cloud =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("filter_cloud_settings.enable_filter_cloud"), data_processing_settings.filter_cloud_settings.enable_filter_cloud)
                .toInt();
        data_processing_settings.filter_cloud_settings.filter_cloud_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("filter_cloud_settings.filter_cloud_threshold"),
                        data_processing_settings.filter_cloud_settings.filter_cloud_threshold)
                .toInt();

        data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("sidelobe_filter_settings.delta_azim_filter_flag"),
                        data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag)
                .toInt();
        data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("sidelobe_filter_settings.delta_azim_filter_ratio"),
                        data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio)
                .toDouble();
        data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("sidelobe_filter_settings.delta_elev_filter_flag"),
                        data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag)
                .toInt();
        data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("sidelobe_filter_settings.delta_elev_filter_ratio"),
                        data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio)
                .toDouble();

        for (int ii = 0; ii < EXCLUDE_SETTINGS_NUM; ++ii)
        {
            data_processing_settings.exclude_settings[ii].is_selected_flag =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".is_selected_flag"),
                            data_processing_settings.exclude_settings[ii].is_selected_flag)
                    .toInt();
            data_processing_settings.exclude_settings[ii].radar_mode_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".radar_mode_min"),
                            data_processing_settings.exclude_settings[ii].radar_mode_min)
                    .toInt();
            data_processing_settings.exclude_settings[ii].radar_mode_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".radar_mode_max"),
                            data_processing_settings.exclude_settings[ii].radar_mode_max)
                    .toInt();
            data_processing_settings.exclude_settings[ii].range_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".range_min"),
                            data_processing_settings.exclude_settings[ii].range_min)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].range_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".range_max"),
                            data_processing_settings.exclude_settings[ii].range_max)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].azim_deg_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".azim_deg_min"),
                            data_processing_settings.exclude_settings[ii].azim_deg_min)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].azim_deg_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".azim_deg_max"),
                            data_processing_settings.exclude_settings[ii].azim_deg_max)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].elev_deg_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".elev_deg_min"),
                            data_processing_settings.exclude_settings[ii].elev_deg_min)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].elev_deg_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".elev_deg_max"),
                            data_processing_settings.exclude_settings[ii].elev_deg_max)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].velocity_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".velocity_min"),
                            data_processing_settings.exclude_settings[ii].velocity_min)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].velocity_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".velocity_max"),
                            data_processing_settings.exclude_settings[ii].velocity_max)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].amp_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".amp_min"), data_processing_settings.exclude_settings[ii].amp_min)
                    .toInt();
            data_processing_settings.exclude_settings[ii].amp_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".amp_max"), data_processing_settings.exclude_settings[ii].amp_max)
                    .toInt();
            data_processing_settings.exclude_settings[ii].height_min =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".height_min"),
                            data_processing_settings.exclude_settings[ii].height_min)
                    .toDouble();
            data_processing_settings.exclude_settings[ii].height_max =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".height_max"),
                            data_processing_settings.exclude_settings[ii].height_max)
                    .toDouble();
        }

        data_processing_settings.combine_thresholds.combine_range_unit_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("combine_thresholds.combine_range_unit_threshold"),
                        data_processing_settings.combine_thresholds.combine_range_unit_threshold)
                .toInt();
        data_processing_settings.combine_thresholds.combine_doppler_unit_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("combine_thresholds.combine_doppler_unit_threshold"),
                        data_processing_settings.combine_thresholds.combine_doppler_unit_threshold)
                .toInt();
        data_processing_settings.combine_thresholds.combine_range_threshold = settings_in_config_file
                                                                                  ->value(string_data_processing_settings + QString("combine_thresholds.combine_range_threshold"),
                                                                                          data_processing_settings.combine_thresholds.combine_range_threshold)
                                                                                  .toDouble();
        data_processing_settings.combine_thresholds.combine_velocity_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("combine_thresholds.combine_velocity_threshold"),
                        data_processing_settings.combine_thresholds.combine_velocity_threshold)
                .toDouble();
        data_processing_settings.combine_thresholds.combine_azim_threshold_deg =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("combine_thresholds.combine_azim_threshold_deg"),
                        data_processing_settings.combine_thresholds.combine_azim_threshold_deg)
                .toDouble();
        data_processing_settings.combine_thresholds.combine_elev_threshold_deg =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("combine_thresholds.combine_elev_threshold_deg"),
                        data_processing_settings.combine_thresholds.combine_elev_threshold_deg)
                .toDouble();

        for (int ii = 0; ii < SMOOTH_TRACK_SETTINGS_NUM; ++ii)
        {
            data_processing_settings.smooth_track_settings[ii].filter_track_method =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_method"),
                            data_processing_settings.smooth_track_settings[ii].filter_track_method)
                    .toInt();
            data_processing_settings.smooth_track_settings[ii].filter_track_coef_alpha =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_coef_alpha"),
                            data_processing_settings.smooth_track_settings[ii].filter_track_coef_alpha)
                    .toDouble();
            data_processing_settings.smooth_track_settings[ii].filter_track_coef_beta =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_coef_beta"),
                            data_processing_settings.smooth_track_settings[ii].filter_track_coef_beta)
                    .toDouble();
            data_processing_settings.smooth_track_settings[ii].filter_track_coef_gama =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_coef_gama"),
                            data_processing_settings.smooth_track_settings[ii].filter_track_coef_gama)
                    .toDouble();
            data_processing_settings.smooth_track_settings[ii].smooth_track_flag =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".smooth_track_flag"),
                            data_processing_settings.smooth_track_settings[ii].smooth_track_flag)
                    .toInt();
        }

        data_processing_settings.filter_tracks_settings.filter_tracks_flag =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_flag"), data_processing_settings.filter_tracks_settings.filter_tracks_flag)
                .toInt();
        data_processing_settings.filter_tracks_settings.filter_tracks_range_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_range_threshold"),
                        data_processing_settings.filter_tracks_settings.filter_tracks_range_threshold)
                .toDouble();
        data_processing_settings.filter_tracks_settings.filter_tracks_velocity_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_velocity_threshold"),
                        data_processing_settings.filter_tracks_settings.filter_tracks_velocity_threshold)
                .toDouble();
        data_processing_settings.filter_tracks_settings.filter_tracks_mean_delta_range_threshold =
            settings_in_config_file
                ->value(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_mean_delta_range_threshold"),
                        data_processing_settings.filter_tracks_settings.filter_tracks_mean_delta_range_threshold)
                .toDouble();

        for (int ii = 0; ii < 2; ++ii)
        {
            data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_floor =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_threshold_floor"),
                            data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_floor)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_ceil =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_threshold_ceil"),
                            data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_ceil)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_same_direction =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_velocity_threshold_same_direction"),
                            data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_same_direction)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_opposite_direction =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_velocity_threshold_opposite_direction"),
                            data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_opposite_direction)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_0 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_azim_threshold_deg_0"),
                            data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_0)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_0 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_elev_threshold_deg_0"),
                            data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_0)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_range_division_01 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_division_01"),
                            data_processing_settings.correlate_thresholds[ii].correlate_range_division_01)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_1 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_azim_threshold_deg_1"),
                            data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_1)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_1 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_elev_threshold_deg_1"),
                            data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_1)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_range_division_12 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_division_12"),
                            data_processing_settings.correlate_thresholds[ii].correlate_range_division_12)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_2 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_azim_threshold_deg_2"),
                            data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_2)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_2 =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_elev_threshold_deg_2"),
                            data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_2)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_x_threshold =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_x_threshold"),
                            data_processing_settings.correlate_thresholds[ii].correlate_x_threshold)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_y_threshold =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_y_threshold"),
                            data_processing_settings.correlate_thresholds[ii].correlate_y_threshold)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_z_threshold =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_z_threshold"),
                            data_processing_settings.correlate_thresholds[ii].correlate_z_threshold)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_vx_threshold =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_vx_threshold"),
                            data_processing_settings.correlate_thresholds[ii].correlate_vx_threshold)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_vy_threshold"),
                            data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold)
                    .toDouble();
            data_processing_settings.correlate_thresholds[ii].correlate_vz_threshold =
                settings_in_config_file
                    ->value(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_vz_threshold"),
                            data_processing_settings.correlate_thresholds[ii].correlate_vz_threshold)
                    .toDouble();
        }
    }
}

void updateHardwareParametersInConfigFileBasedOnHardwareSettings(const HardwareSettings& hardware_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_hardware_settings = QString("hardware_settings/");

        // 工作模式
        settings_in_config_file->setValue(string_hardware_settings + QString("antenna_enable"), hardware_settings.antenna_enable);
        settings_in_config_file->setValue(string_hardware_settings + QString("work_or_serdes"), hardware_settings.work_or_serdes);
        settings_in_config_file->setValue(string_hardware_settings + QString("work_or_emulate"), hardware_settings.work_or_emulate);

        // 天线相关
        settings_in_config_file->setValue(string_hardware_settings + QString("work_mode_or_correct_mode"), hardware_settings.work_mode_or_correct_mode);
        settings_in_config_file->setValue(string_hardware_settings + QString("antennna_fre_or_amp_config"), hardware_settings.antennna_fre_or_amp_config);
        settings_in_config_file->setValue(string_hardware_settings + QString("antenna_weight_method"), hardware_settings.antenna_weight_method);
        settings_in_config_file->setValue(string_hardware_settings + QString("antennna_freq_code"), hardware_settings.antennna_freq_code);
        settings_in_config_file->setValue(string_hardware_settings + QString("elev_support_angle"), hardware_settings.elev_support_angle);
        settings_in_config_file->setValue(string_hardware_settings + QString("antenna_raising_angle"), hardware_settings.antenna_raising_angle);
        settings_in_config_file->setValue(string_hardware_settings + QString("sigma_delta_decay_1"), hardware_settings.sigma_delta_decay_1);
        settings_in_config_file->setValue(string_hardware_settings + QString("protect_decay_1"), hardware_settings.protect_decay_1);
        settings_in_config_file->setValue(string_hardware_settings + QString("sigma_delta_decay_2"), hardware_settings.sigma_delta_decay_2);
        settings_in_config_file->setValue(string_hardware_settings + QString("protect_decay_2"), hardware_settings.protect_decay_2);

        // DBF 相关
        settings_in_config_file->setValue(string_hardware_settings + QString("amp_code"), hardware_settings.amp_code);
        settings_in_config_file->setValue(string_hardware_settings + QString("channel_amp_phase_control_mode"), hardware_settings.channel_amp_phase_control_mode);
        settings_in_config_file->setValue(string_hardware_settings + QString("channel_amp_phase_control_code"), hardware_settings.channel_amp_phase_control_code);
        settings_in_config_file->setValue(string_hardware_settings + QString("channel_num"), hardware_settings.channel_num);
        settings_in_config_file->setValue(string_hardware_settings + QString("dbf_coe_choose"), hardware_settings.dbf_coe_choose);
        settings_in_config_file->setValue(string_hardware_settings + QString("dbf_coef_receive_raising_angle"), hardware_settings.dbf_coef_receive_raising_angle);

        // 转台相关
        settings_in_config_file->setValue(string_hardware_settings + QString("turntable_rotating_mode"), hardware_settings.turntable_rotating_mode);
        for (int ii = 0; ii < RADAR_MISSION_NUM; ++ii)
        {
            settings_in_config_file->setValue(string_hardware_settings + QString("spr_map") + QString::number(ii), hardware_settings.spr_map[ii]);
        }
        settings_in_config_file->setValue(string_hardware_settings + QString("turntable_rotating_speed"), hardware_settings.turntable_rotating_speed);
        settings_in_config_file->setValue(string_hardware_settings + QString("turntable_pointing_angle"), hardware_settings.turntable_pointing_angle);

        // 信号处理相关
        settings_in_config_file->setValue(string_hardware_settings + QString("stc_select"), hardware_settings.stc_select);
        settings_in_config_file->setValue(string_hardware_settings + QString("remove_dc_flag"), hardware_settings.remove_dc_flag);
        settings_in_config_file->setValue(string_hardware_settings + QString("cfar_method"), hardware_settings.cfar_method);
        for (int ii = 0; ii < THRESHOLDS_NUM; ++ii)
        {
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".doppler_threshold"),
                                              hardware_settings.thresholds[ii].doppler_threshold);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".range_threshold_division_range_index_n_f"),
                                              hardware_settings.thresholds[ii].range_threshold_division_range_index_n_f);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".range_threshold_near"),
                                              hardware_settings.thresholds[ii].range_threshold_near);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".range_threshold_far"),
                                              hardware_settings.thresholds[ii].range_threshold_far);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".fft_cut"),
                                              hardware_settings.thresholds[ii].fft_cut);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".damp_cut_division_range_index"),
                                              hardware_settings.thresholds[ii].damp_cut_division_range_index);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".damp_cut_near"),
                                              hardware_settings.thresholds[ii].damp_cut_near);
            settings_in_config_file->setValue(string_hardware_settings + QString("thresholds") + QString::number(ii) + QString(".damp_cut_far"),
                                              hardware_settings.thresholds[ii].damp_cut_far);
            for (int jj = 0; jj < DOPPLER_EXCLUDE_NUM; ++jj)
            {
                settings_in_config_file->setValue(string_hardware_settings + QString("thresholds" + QString::number(ii) + ".doppler_exclude" + QString::number(jj)),
                                                  hardware_settings.thresholds[ii].doppler_exclude[jj]);
            }
        }

        settings_in_config_file->setValue(string_hardware_settings + QString("power_off_area_settings.enable_power_off_area_flag"),
                                          hardware_settings.power_off_area_settings.enable_power_off_area_flag);
        settings_in_config_file->setValue(string_hardware_settings + QString("power_off_area_settings.power_off_azim_deg_floor"),
                                          hardware_settings.power_off_area_settings.power_off_azim_deg_floor);
        settings_in_config_file->setValue(string_hardware_settings + QString("power_off_area_settings.power_off_azim_deg_ceil"),
                                          hardware_settings.power_off_area_settings.power_off_azim_deg_ceil);
    }
}

void updateSoftwareParametersInConfigFileBasedOnSoftwareSettings(const SoftwareSettings& software_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_software_settings = QString("software_settings/");
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.adapter_index_communicating_with_fpga"),
                                          software_settings.communication_settings.adapter_index_communicating_with_fpga);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_device_id"), software_settings.communication_settings.output_device_id);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_min_track_times"),
                                          software_settings.communication_settings.output_min_track_times);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_tracks_through_adapter_flag"),
                                          software_settings.communication_settings.output_tracks_through_adapter_flag);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_selected_or_all_method"),
                                          software_settings.communication_settings.output_selected_or_all_method);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_azim_north_or_east_flag"),
                                          software_settings.communication_settings.output_azim_north_or_east_flag);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_IP_address"),
                                          QHostAddress(software_settings.communication_settings.output_IP_address).toString());
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.output_port"), software_settings.communication_settings.output_port);

        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.input_packets_from_other_device_flag"),
                                          software_settings.communication_settings.input_packets_from_other_device_flag);
        settings_in_config_file->setValue(string_software_settings + QString("communication_settings.input_port"), software_settings.communication_settings.input_port);

        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_system_error_info_flag"),
                                          software_settings.display_settings.display_system_error_info_flag);
        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_original_points_flag"),
                                          software_settings.display_settings.display_original_points_flag);
        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_filtered_points_flag"),
                                          software_settings.display_settings.display_filtered_points_flag);
        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_tracks_flag"), software_settings.display_settings.display_tracks_flag);
        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_area_radius"), software_settings.display_settings.display_area_radius);
        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_circles_gap"), software_settings.display_settings.display_circles_gap);
        settings_in_config_file->setValue(string_software_settings + QString("display_settings.display_key_targets_track_times"),
                                          software_settings.display_settings.display_key_targets_track_times);

        settings_in_config_file->setValue(string_software_settings + QString("output_file_settings.output_fpga_packet_flag"),
                                          software_settings.output_file_settings.output_fpga_packet_flag);
        settings_in_config_file->setValue(string_software_settings + QString("output_file_settings.output_radar_system_info_flag"),
                                          software_settings.output_file_settings.output_radar_system_info_flag);
        settings_in_config_file->setValue(string_software_settings + QString("output_file_settings.output_radar_original_points_flag"),
                                          software_settings.output_file_settings.output_radar_original_points_flag);
        settings_in_config_file->setValue(string_software_settings + QString("output_file_settings.output_radar_track_points_flag"),
                                          software_settings.output_file_settings.output_radar_track_points_flag);

        settings_in_config_file->setValue(string_software_settings + QString("system_adjust_settings.enter_debug_mode_flag"),
                                          software_settings.system_adjust_settings.enter_debug_mode_flag);
        settings_in_config_file->setValue(string_software_settings + QString("system_adjust_settings.auto_adjust_signal_processer_flag"),
                                          software_settings.system_adjust_settings.auto_adjust_signal_processer_flag);
    }
}

void updateDbfCorrectCoefFileDirParametersInConfigFileBasedOnDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& dbf_correct_coef_dir_settings,
                                                                                           QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_dbf_correct_coef_dir_settings = QString("dbf_correct_coef_dir_settings/");
        settings_in_config_file->setValue(string_dbf_correct_coef_dir_settings + QString("master_external_file_dir"), dbf_correct_coef_dir_settings.master_external_file_dir);
        settings_in_config_file->setValue(string_dbf_correct_coef_dir_settings + QString("slave_external_file_dir"), dbf_correct_coef_dir_settings.slave_external_file_dir);
        settings_in_config_file->setValue(string_dbf_correct_coef_dir_settings + QString("master_internal_file_dir"), dbf_correct_coef_dir_settings.master_internal_file_dir);
        settings_in_config_file->setValue(string_dbf_correct_coef_dir_settings + QString("slave_internal_file_dir"), dbf_correct_coef_dir_settings.slave_internal_file_dir);
    }
}

void updateDataProcessingParametersInConfigFileBasedOnDataProcessingSetting(const DataProcessingSettings& data_processing_settings, QSettings* settings_in_config_file)
{
    if (settings_in_config_file)
    {
        QString string_data_processing_settings = QString("data_processing_settings/");
        for (int ii = 0; ii < RADAR_MODE_NUM; ++ii)
        {
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".range_unit"),
                                              data_processing_settings.radar_wave_paremeters[ii].range_unit);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".blind_area"),
                                              data_processing_settings.radar_wave_paremeters[ii].blind_area);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".doppler_unit"),
                                              data_processing_settings.radar_wave_paremeters[ii].doppler_unit);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".mtd_num"),
                                              data_processing_settings.radar_wave_paremeters[ii].mtd_num);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".rcs_correction_coef"),
                                              data_processing_settings.radar_wave_paremeters[ii].rcs_correction_coef);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".decay0"),
                                              data_processing_settings.radar_wave_paremeters[ii].decay0);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".range_division_01"),
                                              data_processing_settings.radar_wave_paremeters[ii].range_division_01);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".range_division_12"),
                                              data_processing_settings.radar_wave_paremeters[ii].range_division_12);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".decay2"),
                                              data_processing_settings.radar_wave_paremeters[ii].decay2);
            settings_in_config_file->setValue(string_data_processing_settings + QString("radar_wave_paremeters" + QString::number(ii) + ".time_to_correct_azim"),
                                              data_processing_settings.radar_wave_paremeters[ii].time_to_correct_azim);
        }

        for (int ii = 0; ii < SIGMA_DELTA_SETTINGS_NUM; ++ii)
        {
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_flag"),
                                              data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".weight_flag"),
                                              data_processing_settings.sigma_delta_parameters[ii].weight_flag);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_slope"),
                                              data_processing_settings.sigma_delta_parameters[ii].sigma_delta_slope);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".negative_bias_phase_floor"),
                                              data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_floor);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".negative_bias_phase_ceil"),
                                              data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_ceil);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".beam_width"),
                                              data_processing_settings.sigma_delta_parameters[ii].beam_width);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".max_bias_angle"),
                                              data_processing_settings.sigma_delta_parameters[ii].max_bias_angle);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_filter_flag"),
                                              data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag);
            settings_in_config_file->setValue(string_data_processing_settings + QString("sigma_delta_parameters" + QString::number(ii) + ".sigma_delta_filter_ratio"),
                                              data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_ratio);
        }

        settings_in_config_file->setValue(string_data_processing_settings + QString("radar_lon_lat_alt.convert_lon_lat_alt_flag"),
                                          data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("radar_lon_lat_alt.radar_longitude"),
                                          data_processing_settings.radar_lon_lat_alt.radar_longitude);
        settings_in_config_file->setValue(string_data_processing_settings + QString("radar_lon_lat_alt.radar_latitude"), data_processing_settings.radar_lon_lat_alt.radar_latitude);
        settings_in_config_file->setValue(string_data_processing_settings + QString("radar_lon_lat_alt.radar_altitude"), data_processing_settings.radar_lon_lat_alt.radar_altitude);

        settings_in_config_file->setValue(string_data_processing_settings + QString("azim_elev_height_bias_settings.azim_bias_angle"),
                                          data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle);
        settings_in_config_file->setValue(string_data_processing_settings + QString("azim_elev_height_bias_settings.elev_bias_angle"),
                                          data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle);
        settings_in_config_file->setValue(string_data_processing_settings + QString("azim_elev_height_bias_settings.height_bias"),
                                          data_processing_settings.azim_elev_height_bias_settings.height_bias);

        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_radar_mode_flag"),
                                          data_processing_settings.selected_area_settings.select_radar_mode_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_radar_mode_floor"),
                                          data_processing_settings.selected_area_settings.selected_radar_mode_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_radar_mode_ceil"),
                                          data_processing_settings.selected_area_settings.selected_radar_mode_ceil);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_range_flag"),
                                          data_processing_settings.selected_area_settings.select_range_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_range_floor"),
                                          data_processing_settings.selected_area_settings.selected_range_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_range_ceil"),
                                          data_processing_settings.selected_area_settings.selected_range_ceil);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_azim_flag"),
                                          data_processing_settings.selected_area_settings.select_azim_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_azim_deg_floor"),
                                          data_processing_settings.selected_area_settings.selected_azim_deg_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_azim_deg_ceil"),
                                          data_processing_settings.selected_area_settings.selected_azim_deg_ceil);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_elev_flag"),
                                          data_processing_settings.selected_area_settings.select_elev_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_elev_deg_floor"),
                                          data_processing_settings.selected_area_settings.selected_elev_deg_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_elev_deg_ceil"),
                                          data_processing_settings.selected_area_settings.selected_elev_deg_ceil);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_velocity_flag"),
                                          data_processing_settings.selected_area_settings.select_velocity_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_velocity_floor"),
                                          data_processing_settings.selected_area_settings.selected_velocity_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_velocity_ceil"),
                                          data_processing_settings.selected_area_settings.selected_velocity_ceil);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_height_flag"),
                                          data_processing_settings.selected_area_settings.select_height_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_height_floor"),
                                          data_processing_settings.selected_area_settings.selected_height_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_height_ceil"),
                                          data_processing_settings.selected_area_settings.selected_height_ceil);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.select_amp_flag"),
                                          data_processing_settings.selected_area_settings.select_amp_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_amp_floor"),
                                          data_processing_settings.selected_area_settings.selected_amp_floor);
        settings_in_config_file->setValue(string_data_processing_settings + QString("selected_area_settings.selected_amp_ceil"),
                                          data_processing_settings.selected_area_settings.selected_amp_ceil);

        settings_in_config_file->setValue(string_data_processing_settings + QString("clutter_map_settings.clutter_map_filter_flag"),
                                          data_processing_settings.clutter_map_settings.clutter_map_filter_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("clutter_map_settings.clutter_map_generate_weight"),
                                          data_processing_settings.clutter_map_settings.clutter_map_generate_weight);
        settings_in_config_file->setValue(string_data_processing_settings + QString("clutter_map_settings.clutter_map_filter_threshold"),
                                          data_processing_settings.clutter_map_settings.clutter_map_filter_threshold);

        settings_in_config_file->setValue(string_data_processing_settings + QString("filter_cloud_settings.enable_filter_cloud"),
                                          data_processing_settings.filter_cloud_settings.enable_filter_cloud);
        settings_in_config_file->setValue(string_data_processing_settings + QString("filter_cloud_settings.filter_cloud_threshold"),
                                          data_processing_settings.filter_cloud_settings.filter_cloud_threshold);

        settings_in_config_file->setValue(string_data_processing_settings + QString("sidelobe_filter_settings.delta_azim_filter_flag"),
                                          data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("sidelobe_filter_settings.delta_azim_filter_ratio"),
                                          data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio);
        settings_in_config_file->setValue(string_data_processing_settings + QString("sidelobe_filter_settings.delta_elev_filter_flag"),
                                          data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("sidelobe_filter_settings.delta_elev_filter_ratio"),
                                          data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio);

        for (int ii = 0; ii < EXCLUDE_SETTINGS_NUM; ++ii)
        {
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".is_selected_flag"),
                                              data_processing_settings.exclude_settings[ii].is_selected_flag);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".radar_mode_min"),
                                              data_processing_settings.exclude_settings[ii].radar_mode_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".radar_mode_max"),
                                              data_processing_settings.exclude_settings[ii].radar_mode_max);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".range_min"),
                                              data_processing_settings.exclude_settings[ii].range_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".range_max"),
                                              data_processing_settings.exclude_settings[ii].range_max);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".azim_deg_min"),
                                              data_processing_settings.exclude_settings[ii].azim_deg_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".azim_deg_max"),
                                              data_processing_settings.exclude_settings[ii].azim_deg_max);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".elev_deg_min"),
                                              data_processing_settings.exclude_settings[ii].elev_deg_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".elev_deg_max"),
                                              data_processing_settings.exclude_settings[ii].elev_deg_max);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".velocity_min"),
                                              data_processing_settings.exclude_settings[ii].velocity_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".velocity_max"),
                                              data_processing_settings.exclude_settings[ii].velocity_max);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".amp_min"),
                                              data_processing_settings.exclude_settings[ii].amp_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".amp_max"),
                                              data_processing_settings.exclude_settings[ii].amp_max);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".height_min"),
                                              data_processing_settings.exclude_settings[ii].height_min);
            settings_in_config_file->setValue(string_data_processing_settings + QString("exclude_settings" + QString::number(ii) + ".height_max"),
                                              data_processing_settings.exclude_settings[ii].height_max);
        }

        settings_in_config_file->setValue(string_data_processing_settings + QString("combine_thresholds.combine_range_unit_threshold"),
                                          data_processing_settings.combine_thresholds.combine_range_unit_threshold);
        settings_in_config_file->setValue(string_data_processing_settings + QString("combine_thresholds.combine_doppler_unit_threshold"),
                                          data_processing_settings.combine_thresholds.combine_doppler_unit_threshold);
        settings_in_config_file->setValue(string_data_processing_settings + QString("combine_thresholds.combine_range_threshold"),
                                          data_processing_settings.combine_thresholds.combine_range_threshold);
        settings_in_config_file->setValue(string_data_processing_settings + QString("combine_thresholds.combine_velocity_threshold"),
                                          data_processing_settings.combine_thresholds.combine_velocity_threshold);
        settings_in_config_file->setValue(string_data_processing_settings + QString("combine_thresholds.combine_azim_threshold_deg"),
                                          data_processing_settings.combine_thresholds.combine_azim_threshold_deg);
        settings_in_config_file->setValue(string_data_processing_settings + QString("combine_thresholds.combine_elev_threshold_deg"),
                                          data_processing_settings.combine_thresholds.combine_elev_threshold_deg);

        for (int ii = 0; ii < SMOOTH_TRACK_SETTINGS_NUM; ++ii)
        {
            settings_in_config_file->setValue(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_method"),
                                              data_processing_settings.smooth_track_settings[ii].filter_track_method);
            settings_in_config_file->setValue(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_coef_alpha"),
                                              data_processing_settings.smooth_track_settings[ii].filter_track_coef_alpha);
            settings_in_config_file->setValue(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_coef_beta"),
                                              data_processing_settings.smooth_track_settings[ii].filter_track_coef_beta);
            settings_in_config_file->setValue(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".filter_track_coef_gama"),
                                              data_processing_settings.smooth_track_settings[ii].filter_track_coef_gama);
            settings_in_config_file->setValue(string_data_processing_settings + QString("smooth_track_settings" + QString::number(ii) + ".smooth_track_flag"),
                                              data_processing_settings.smooth_track_settings[ii].smooth_track_flag);
        }

        settings_in_config_file->setValue(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_flag"),
                                          data_processing_settings.filter_tracks_settings.filter_tracks_flag);
        settings_in_config_file->setValue(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_range_threshold"),
                                          data_processing_settings.filter_tracks_settings.filter_tracks_range_threshold);
        settings_in_config_file->setValue(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_velocity_threshold"),
                                          data_processing_settings.filter_tracks_settings.filter_tracks_velocity_threshold);
        settings_in_config_file->setValue(string_data_processing_settings + QString("filter_tracks_settings.filter_tracks_mean_delta_range_threshold"),
                                          data_processing_settings.filter_tracks_settings.filter_tracks_mean_delta_range_threshold);

        for (int ii = 0; ii < 2; ++ii)
        {
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_threshold_floor"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_floor);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_threshold_ceil"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_ceil);
            settings_in_config_file->setValue(string_data_processing_settings +
                                                  QString("correlate_thresholds" + QString::number(ii) + ".correlate_velocity_threshold_same_direction"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_same_direction);
            settings_in_config_file->setValue(string_data_processing_settings +
                                                  QString("correlate_thresholds" + QString::number(ii) + ".correlate_velocity_threshold_opposite_direction"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_opposite_direction);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_azim_threshold_deg_0"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_0);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_elev_threshold_deg_0"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_0);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_division_01"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_range_division_01);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_azim_threshold_deg_1"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_1);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_elev_threshold_deg_1"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_1);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_range_division_12"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_range_division_12);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_azim_threshold_deg_2"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_2);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_elev_threshold_deg_2"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_2);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_x_threshold"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_x_threshold);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_y_threshold"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_y_threshold);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_z_threshold"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_z_threshold);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_vx_threshold"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_vx_threshold);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_vy_threshold"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold);
            settings_in_config_file->setValue(string_data_processing_settings + QString("correlate_thresholds" + QString::number(ii) + ".correlate_vz_threshold"),
                                              data_processing_settings.correlate_thresholds[ii].correlate_vz_threshold);
        }
    }
}

void initializeReadFpgaPacketFileSettings(ReadFpgaPacketFileSettings& read_fpga_packet_file_settings)
{
    read_fpga_packet_file_settings.read_fpga_packet_from_file_flag = false;
    read_fpga_packet_file_settings.fpga_packet_file_dir.clear();
}

void initializeHardwareSettingsTrackMode(HardwareSettingsTrackMode& hardware_settings_track_mode)
{
    hardware_settings_track_mode.next_radar_mode = 0;
    hardware_settings_track_mode.next_pointing_azim_deg = 0;
    hardware_settings_track_mode.next_elev_index_list[0] = 6;
    hardware_settings_track_mode.next_elev_index_list[1] = 7;
    hardware_settings_track_mode.next_elev_index_list[2] = 8;
    hardware_settings_track_mode.next_elev_index_list[3] = 9;
}
