#include "radar_data_processing_module.h"
#include "global_functions.h"
#include <QDebug>

RadarSystemState::RadarSystemState()
{
    current_data_processing_mode = RADAR_SCAN_MODE;
    radar_connected_flag = 0;
    pkt_num = 0;
    current_radar_mission_mode_index = 0;
    current_radar_mode_index = NEAR_SCAN_MODE_INDEX;
    current_scan_or_track_mode = RADAR_SCAN_MODE;
    current_azim_deg = 0;
    current_azim_deg_to_north = 0;
    current_transmit_azim_deg = 0;
    last_transmit_azim_deg = -1;
    current_transmit_azim_center = 0;
    current_lost_azim_sectors_num = 0;
    total_lost_azim_sectors_num = 0;
    error_lost_azim_sectors_num = 0;
    receiver_weight_set = 0;
    dbf_coef_receive_raising_angle_index = 0;
    next_elev_round_flag = 0;
    last_elev_index = ELEV_BEAM_NUM_SCAN_MODE;
    current_elev_index = 0;
    current_elev_deg = 0;
    radar_state_register = 0;
    target_num = 0;
    radar_mission_mode = 0;
    radar_set_mode = 0;
    biss_angle_deg = 0;
    turntable_rotating_speed = 0;
    track_azim_set = 0;
    antenna_command_mode = 0;
    antenna_temperature_warning = 0;
    antenna_work_mode = 0;
    bank_12v_current = 0;
    launch_28v_current = 0;
    output_power_1 = 0;
    output_power_2 = 0;
    fre_code = 0;
    in_current_28v = 0;
    recv_12v_current = 0;
    fre_12v_current = 0;
    beam_ctrl_12v_current = 0;
    battery_T = 0;
    T_transmitter1 = 0;
    T_transmitter2 = 0;
    T_recv1 = 0;
    T_recv2 = 0;
    T_recv3 = 0;
    T_recv4 = 0;
    bank_mode_report = 0;
    T_AD_FPGA1 = 0;
    T_AD_FPGA2 = 0;
    T_QDR_FPGA1 = 0;
    T_QDR_FPGA2 = 0;
    T_QDR_FPGA3 = 0;
    T_QDR_FPGA4 = 0;
    last_GMAC_count = -1;
    current_GMAC_count = -1;
    current_lost_packets_count = 0;
    total_lost_packets_count = 0;
    error_lost_packets_count = 0;
    current_free_time = 0;
    last_free_time = 0;
    cumulative_free_time = 0;
    antenna_data_parity = 0;
    ATP_azim_position = 0;
    ATP_azim_speed = 0;
    ATP_azim_acc = 0;
    ATP_cyclic_code = 0;
    ATP_parity = 0;
    FPGA_version = 0;
    memset(&this->error_info_flag_list, 0x00, sizeof(error_info_flag_list));
}

void initializeRadarSystemState(RadarSystemState& radar_system_state)
{
    radar_system_state.current_data_processing_mode = RADAR_SCAN_MODE;
    radar_system_state.radar_connected_flag = 0;
    radar_system_state.pkt_num = 0;
    radar_system_state.current_radar_mission_mode_index = 0;
    radar_system_state.current_radar_mode_index = NEAR_SCAN_MODE_INDEX;
    radar_system_state.current_scan_or_track_mode = RADAR_SCAN_MODE;
    radar_system_state.current_azim_deg = 0;
    radar_system_state.current_azim_deg_to_north = 0;
    radar_system_state.current_transmit_azim_deg = 0;
    radar_system_state.last_transmit_azim_deg = -1;
    radar_system_state.current_transmit_azim_center = 0;
    radar_system_state.current_lost_azim_sectors_num = 0;
    radar_system_state.total_lost_azim_sectors_num = 0;
    radar_system_state.error_lost_azim_sectors_num = 0;
    radar_system_state.next_elev_round_flag = 0;
    radar_system_state.last_elev_index = ELEV_BEAM_NUM_SCAN_MODE;
    radar_system_state.current_elev_index = 0;
    radar_system_state.current_elev_deg = 0;
    radar_system_state.radar_state_register = 0;
    radar_system_state.target_num = 0;
    radar_system_state.radar_mission_mode = 0;
    radar_system_state.radar_set_mode = 0;
    radar_system_state.biss_angle_deg = 0;
    radar_system_state.turntable_rotating_speed = 0;
    radar_system_state.track_azim_set = 0;
    radar_system_state.antenna_command_mode = 0;
    radar_system_state.antenna_temperature_warning = 0;
    radar_system_state.antenna_work_mode = 0;
    radar_system_state.bank_12v_current = 0;
    radar_system_state.launch_28v_current = 0;
    radar_system_state.output_power_1 = 0;
    radar_system_state.output_power_2 = 0;
    radar_system_state.fre_code = 0;
    radar_system_state.in_current_28v = 0;
    radar_system_state.recv_12v_current = 0;
    radar_system_state.fre_12v_current = 0;
    radar_system_state.beam_ctrl_12v_current = 0;
    radar_system_state.battery_T = 0;
    radar_system_state.T_transmitter1 = 0;
    radar_system_state.T_transmitter2 = 0;
    radar_system_state.T_recv1 = 0;
    radar_system_state.T_recv2 = 0;
    radar_system_state.T_recv3 = 0;
    radar_system_state.T_recv4 = 0;
    radar_system_state.bank_mode_report = 0;
    radar_system_state.T_AD_FPGA1 = 0;
    radar_system_state.T_AD_FPGA2 = 0;
    radar_system_state.T_QDR_FPGA1 = 0;
    radar_system_state.T_QDR_FPGA2 = 0;
    radar_system_state.T_QDR_FPGA3 = 0;
    radar_system_state.T_QDR_FPGA4 = 0;
    radar_system_state.last_GMAC_count = -1;
    radar_system_state.current_GMAC_count = -1;
    radar_system_state.current_lost_packets_count = 0;
    radar_system_state.total_lost_packets_count = 0;
    radar_system_state.error_lost_packets_count = 0;
    radar_system_state.current_free_time = 0;
    radar_system_state.last_free_time = 0;
    radar_system_state.cumulative_free_time = 0;
    radar_system_state.antenna_data_parity = 0;
    radar_system_state.ATP_azim_position = 0;
    radar_system_state.ATP_azim_speed = 0;
    radar_system_state.ATP_azim_acc = 0;
    radar_system_state.ATP_cyclic_code = 0;
    radar_system_state.ATP_parity = 0;
    radar_system_state.FPGA_version = 0;
    memset(&radar_system_state.error_info_flag_list, 0x00, sizeof(radar_system_state.error_info_flag_list));
}

TargetOriginalPoint::TargetOriginalPoint()
{
    date_time.currentDateTime();
    radar_mode = 0;
    range_index = 0;
    azim_index = 0;
    elev_index = 0;
    doppler_index = 0;
    range = 0;
    azim_deg = 0;
    elev_deg = 0;
    vr = 0;
    x = 0;
    y = 0;
    z = 0;
    vx = 0;
    vy = 0;
    vz = 0;
    amp_sigma = 0;
    amp_delta_azim = 0;
    amp_delta_elev1 = 0;
    amp_delta_elev2 = 0;
    sigma_I = 0;
    sigma_Q = 0;
    delta_azim_I = 0;
    delta_azim_Q = 0;
    delta_elev1_I = 0;
    delta_elev1_Q = 0;
    delta_elev2_I = 0;
    delta_elev2_Q = 0;
    free_time = 0;
    snr = 0;
    cfar_neighbour_num = 0;
    rcs = 0;
    is_filtered = false;
    is_combined = false;
    is_correlated = false;
    best_correlate_distance = 1e9;
    best_correlate_distance_track_established_flag = false;
}

TargetOriginalPoint::TargetOriginalPoint(QDateTime _date_time,
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
                                         double _rcs,
                                         bool _is_filtered,
                                         bool _is_combined,
                                         bool _is_correlated,
                                         double _best_correlate_distance,
                                         bool _best_correlate_distance_track_established_flag)
{
    date_time = _date_time;
    radar_mode = _radar_mode;
    range_index = _range_index;
    azim_index = _azim_index;
    elev_index = _elev_index;
    doppler_index = _doppler_index;
    range = _range;
    azim_deg = _azim_deg;
    elev_deg = _elev_deg;
    vr = _vr;
    x = _x;
    y = _y;
    z = _z;
    vx = _vx;
    vy = _vy;
    vz = _vz;
    amp_sigma = _amp_sigma;
    amp_delta_azim = _amp_delta_azim;
    amp_delta_elev1 = _amp_delta_elev1;
    amp_delta_elev2 = _amp_delta_elev2;
    sigma_I = _sigma_I;
    sigma_Q = _sigma_Q;
    delta_azim_I = _delta_azim_I;
    delta_azim_Q = _delta_azim_Q;
    delta_elev1_I = _delta_elev1_I;
    delta_elev1_Q = _delta_elev1_Q;
    delta_elev2_I = _delta_elev2_I;
    delta_elev2_Q = _delta_elev2_Q;
    free_time = _free_time;
    snr = _snr;
    cfar_neighbour_num = _cfar_neighbour_num;
    rcs = _rcs;
    is_filtered = _is_filtered;
    is_combined = _is_combined;
    is_correlated = _is_correlated;
    best_correlate_distance = _best_correlate_distance;
    best_correlate_distance_track_established_flag = _best_correlate_distance_track_established_flag;
}

TargetTrack::TargetTrack(int new_track_id)
{
    track_id = new_track_id;
    effective_track_size = 0;
    is_correlated = false;
    track_times = 0;
    continuous_lost_times = 0;
    total_lost_times = 0;
    track_size_track_mode = 0;
    track_times_track_mode = 0;
    lost_times_track_mode = 0;
    last_measured_point_index = 0;
    target_track_points_list.clear();
    distance_to_point_index_map.clear();

    // α-β 滤波
    alpha_beta_k = 1;
    coef_alpha = 1;
    coef_beta = 1;
}

bool TargetTrack::precorrelateWithOriginalPoint(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    //航迹与点迹预相关
    if (target_track_points_list.size() == 0)
    {
        return false;
    }

    TargetTrackPoint* tp = &(target_track_points_list[last_measured_point_index]); //取航迹的上一个实测点（即非外推点）

    //    double delta_x = tp->x - target_original_point.x;
    //    double delta_y = tp->y - target_original_point.y;
    //    double delta_z = tp->z - target_original_point.z;
    //    double delta_distance_square = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
    //    if (delta_distance_square > 40000)
    //    {
    //        return false;
    //    }

    double predicted_azim = tp->azim_deg;
    double predicted_elev = tp->elev_deg;
    double delta_azim = circularlyAddDouble(target_original_point.azim_deg, -predicted_azim, -180, 360);
    double delta_elev = circularlyAddDouble(target_original_point.elev_deg, -predicted_elev, -180, 360);
    double predicted_vr = tp->vr;
    double delta_free_time = fabs(target_original_point.free_time - tp->free_time);

    // TODO 设置两个门限, 起批之后的门限和丢失后的大波门

    double delta_range = fabs(tp->range - 0.5 * (predicted_vr + target_original_point.vr) * delta_free_time - target_original_point.range);
    double predicted_delta_range = fabs(0.5 * (predicted_vr + target_original_point.vr) * delta_free_time); // 注意, 门限必须为正, 此处一定要取绝对值!
    double range_threshold = predicted_delta_range;
    if (predicted_delta_range > data_processing_settings.correlate_thresholds[0].correlate_range_threshold_ceil)
    {
        range_threshold = data_processing_settings.correlate_thresholds[0].correlate_range_threshold_ceil;
    }
    else if (predicted_delta_range < data_processing_settings.correlate_thresholds[0].correlate_range_threshold_floor)
    {
        range_threshold = data_processing_settings.correlate_thresholds[0].correlate_range_threshold_floor;
    }

    double delta_range_stop_go = fabs(tp->range - 0.25 * (predicted_vr + target_original_point.vr) * delta_free_time - target_original_point.range);

    // 根据速度方向设定速度门限
    double velocity_threshold = data_processing_settings.correlate_thresholds[0].correlate_velocity_threshold_same_direction;
    if (predicted_vr * target_original_point.vr < 0)
    {
        velocity_threshold = data_processing_settings.correlate_thresholds[0].correlate_velocity_threshold_opposite_direction;
    }

    double azim_threshold = 0;
    double elev_threshold = 0;

    if (tp->range <= data_processing_settings.correlate_thresholds[0].correlate_range_division_01)
    {
        azim_threshold = data_processing_settings.correlate_thresholds[0].correlate_azim_threshold_deg_0;
        elev_threshold = data_processing_settings.correlate_thresholds[0].correlate_elev_threshold_deg_0;
    }
    else if (tp->range <= data_processing_settings.correlate_thresholds[0].correlate_range_division_12)
    {
        azim_threshold = data_processing_settings.correlate_thresholds[0].correlate_azim_threshold_deg_1;
        elev_threshold = data_processing_settings.correlate_thresholds[0].correlate_elev_threshold_deg_1;
    }
    else
    {
        azim_threshold = data_processing_settings.correlate_thresholds[0].correlate_azim_threshold_deg_2;
        elev_threshold = data_processing_settings.correlate_thresholds[0].correlate_elev_threshold_deg_2;
    }

    if (fabs(delta_azim) < azim_threshold && fabs(delta_elev) < elev_threshold && fabs(predicted_vr - target_original_point.vr) < velocity_threshold)
    {

        // 判断最后两个点的向量夹角
        //        double x_k_x_kp1 = target_original_point.x - tp->x;
        //        double y_k_y_kp1 = target_original_point.y - tp->y;
        //        double x_km1_x_k = last_two_point_vector.x_km1_x_k;
        //        double y_km1_y_k = last_two_point_vector.y_km1_y_k;
        //        double theta = acos((x_km1_x_k * x_k_x_kp1 + y_km1_y_k * y_k_y_kp1) / sqrt((x_km1_x_k * x_km1_x_k + y_km1_y_k * y_km1_y_k) *
        //        (x_k_x_kp1 * x_k_x_kp1 + y_k_y_kp1 * y_k_y_kp1))); if (theta > DOT9_PI)
        //        {
        //            return false;
        //        }

        double real_delta_range = 0;
        if (delta_range < range_threshold)
        {
            real_delta_range = delta_range;
        }
        else if (delta_range_stop_go < range_threshold)
        {
            real_delta_range = delta_range_stop_go;
        }
        else
        {
            return false;
        }

        double distance = real_delta_range / data_processing_settings.radar_wave_paremeters[target_original_point.radar_mode].range_unit +
                          fabs(delta_azim) / data_processing_settings.sigma_delta_parameters[0].beam_width +
                          fabs(delta_elev) / data_processing_settings.sigma_delta_parameters[1].beam_width;
        if (tp->established_mode > 0)
        {
            // 已经起批的航迹的优先级高于未起批的航迹
            if (target_original_point.best_correlate_distance_track_established_flag)
            {
                if (distance < target_original_point.best_correlate_distance)
                {
                    target_original_point.best_correlate_distance = distance;
                    target_original_point.best_correlate_distance_track_established_flag = true;
                    distance_to_point_index_map.insert(distance, &target_original_point);
                    return true;
                }
            }
            else
            {
                target_original_point.best_correlate_distance = distance;
                target_original_point.best_correlate_distance_track_established_flag = true;
                distance_to_point_index_map.insert(distance, &target_original_point);
                return true;
            }
        }
        else
        {
            // 如果航迹未起批, 只有当原始点迹的最优匹配航迹也是未起批时, 才更新
            if (!target_original_point.best_correlate_distance_track_established_flag)
            {
                if (distance < target_original_point.best_correlate_distance)
                {
                    target_original_point.best_correlate_distance = distance;
                    target_original_point.best_correlate_distance_track_established_flag = false;
                    distance_to_point_index_map.insert(distance, &target_original_point);
                    return true;
                }
            }
        }
    }
    return false;
}

void TargetTrack::addMeasuredTrackPoint(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    TargetTrackPoint tp;
    tp.track_id = track_id;
    tp.radar_mode = target_original_point.radar_mode;
    tp.track_times = ++track_times;
    continuous_lost_times = 0;
    tp.lost_times = continuous_lost_times;
    tp.date_time = target_original_point.date_time;
    tp.range = target_original_point.range;
    tp.azim_index = target_original_point.azim_index;
    tp.azim_deg = target_original_point.azim_deg;
    tp.elev_index = target_original_point.elev_index;
    tp.elev_deg = target_original_point.elev_deg;
    tp.vr = target_original_point.vr;
    tp.amp_sigma = target_original_point.amp_sigma;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_elev1 = target_original_point.amp_delta_elev1;
    tp.amp_delta_elev2 = target_original_point.amp_delta_elev2;
    tp.snr = target_original_point.snr;
    tp.cfar_neighbour_num = target_original_point.cfar_neighbour_num;
    tp.rcs = target_original_point.rcs;
    tp.x = target_original_point.x;
    tp.y = target_original_point.y;
    tp.z = target_original_point.z;
    // 如果航迹原始点数大于1, 根据当前原始点和航迹点队列的 最后一个实测点来对正在加入的点的 vx vy vz 做外推
    if (target_track_points_list.size() >= 1)
    {
        double delta_free_time = fabs(target_original_point.free_time - target_track_points_list.at(last_measured_point_index).free_time);
        if (delta_free_time < EPS)
        {
            qDebug() << QString("[Warning]: 航迹更新, 但是自由时间不变!!!");
        }
        else
        {
            tp.vx = (target_original_point.x - target_track_points_list.at(last_measured_point_index).x) / delta_free_time;
            tp.vy = (target_original_point.y - target_track_points_list.at(last_measured_point_index).y) / delta_free_time;
            tp.vz = (target_original_point.z - target_track_points_list.at(last_measured_point_index).z) / delta_free_time;
        }
    }
    else
    {
        tp.vx = target_original_point.vx;
        tp.vy = target_original_point.vy;
        tp.vz = target_original_point.vz;
    }
    tp.azim_index = target_original_point.azim_index;
    tp.free_time = target_original_point.free_time;

    convertLonLatAlt(tp, data_processing_settings);

    target_original_point.is_correlated = true;
    is_correlated = true;
    tp.is_correlated = is_correlated;
    target_track_points_list.append(tp);
    ++effective_track_size;
    last_measured_point_index = target_track_points_list.size() - 1;

    // 注意, 为了滤波可以正常工作, 在没有打开滤波开关的时候, 需要把当前原始点实时设置为滤波初始值
    alpha_beta_k = 1;
    track_filter_point.range_p = target_original_point.range;
    track_filter_point.azim_p = target_original_point.azim_deg;
    track_filter_point.elev_p = target_original_point.elev_deg;
    track_filter_point.x_p = target_original_point.x;
    track_filter_point.y_p = target_original_point.y;
    track_filter_point.z_p = target_original_point.z;
    track_filter_point.vx_p = target_original_point.vx;
    track_filter_point.vy_p = target_original_point.vy;
    track_filter_point.vz_p = target_original_point.vz;
}

void TargetTrack::addMeasuredTrackPointAlphaBeta(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    TargetTrackPoint tp;
    tp.track_id = track_id;
    tp.radar_mode = target_original_point.radar_mode;
    tp.track_times = ++track_times;
    continuous_lost_times = 0;
    tp.lost_times = continuous_lost_times;
    tp.date_time = target_original_point.date_time;
    tp.azim_index = target_original_point.azim_index;
    tp.elev_index = target_original_point.elev_index;
    tp.vr = target_original_point.vr;
    tp.amp_sigma = target_original_point.amp_sigma;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_elev1 = target_original_point.amp_delta_elev1;
    tp.amp_delta_elev2 = target_original_point.amp_delta_elev2;
    tp.snr = target_original_point.snr;
    tp.cfar_neighbour_num = target_original_point.cfar_neighbour_num;
    tp.rcs = target_original_point.rcs;

    // 如果航迹原始点数大于1, 根据当前原始点和航迹点队列的 最后一个实测点来对正在加入的点的 vx vy vz 做外推
    if (target_track_points_list.size() >= 1)
    {
        double delta_free_time = fabs(target_original_point.free_time - target_track_points_list.at(last_measured_point_index).free_time);
        if (delta_free_time < EPS)
        {
            qDebug() << QString("[Warning]: 航迹更新, 但是自由时间不变!!!");
        }
        else
        {
            tp.vx = (target_original_point.x - target_track_points_list.at(last_measured_point_index).x) / delta_free_time;
            tp.vy = (target_original_point.y - target_track_points_list.at(last_measured_point_index).y) / delta_free_time;
            tp.vz = (target_original_point.z - target_track_points_list.at(last_measured_point_index).z) / delta_free_time;

            // alpha_beta 迭代次数不能大于 7
            ++alpha_beta_k;
            if (alpha_beta_k > VARIABLE_ALPHA_BETA_MAX_ITERATION_NUM)
            {
                alpha_beta_k = VARIABLE_ALPHA_BETA_MAX_ITERATION_NUM;
            }
            coef_alpha = ALPHA_BETA_FILER_ALPHA_ARRAY[alpha_beta_k];
            coef_beta = ALPHA_BETA_FILER_BETA_ARRAY[alpha_beta_k];

            // 预测下一步
            track_predict_point.x_p = track_filter_point.x_p + track_filter_point.vx_p * delta_free_time;
            track_predict_point.y_p = track_filter_point.y_p + track_filter_point.vy_p * delta_free_time;
            track_predict_point.z_p = track_filter_point.z_p + track_filter_point.vz_p * delta_free_time;

            // 滤波下一步
            track_filter_point.x_p = track_predict_point.x_p + coef_alpha * (target_original_point.x - track_predict_point.x_p);
            track_filter_point.y_p = track_predict_point.y_p + coef_alpha * (target_original_point.y - track_predict_point.y_p);
            track_filter_point.z_p = track_predict_point.z_p + coef_alpha * (target_original_point.z - track_predict_point.z_p);
            if (track_filter_point.z_p < 0)
            {
                track_filter_point.z_p = 0;
            }
            track_filter_point.vx_p = track_filter_point.vx_p + coef_beta * (target_original_point.x - track_predict_point.x_p) / delta_free_time;
            track_filter_point.vy_p = track_filter_point.vy_p + coef_beta * (target_original_point.y - track_predict_point.y_p) / delta_free_time;
            track_filter_point.vz_p = track_filter_point.vz_p + coef_beta * (target_original_point.z - track_predict_point.z_p) / delta_free_time;
            track_filter_point.range_p = sqrt(pow(track_filter_point.x_p, 2) + pow(track_filter_point.y_p, 2) + pow(track_filter_point.z_p, 2));
            track_filter_point.azim_p = fmod(atan2(track_filter_point.y_p, track_filter_point.x_p) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
            track_filter_point.elev_p = asin(track_filter_point.z_p / track_filter_point.range_p) * RAD2DEG;

            tp.range = track_filter_point.range_p;
            tp.azim_deg = track_filter_point.azim_p;
            tp.elev_deg = track_filter_point.elev_p;
            tp.x = track_filter_point.x_p;
            tp.y = track_filter_point.y_p;
            tp.z = track_filter_point.z_p;
            tp.vx = track_filter_point.vx_p;
            tp.vy = track_filter_point.vy_p;
            tp.vz = track_filter_point.vz_p;
        }
    }
    else
    {
        tp.range = target_original_point.range;
        tp.azim_deg = target_original_point.azim_deg;
        tp.elev_deg = target_original_point.elev_deg;
        tp.x = target_original_point.x;
        tp.y = target_original_point.y;
        tp.z = target_original_point.z;
        tp.vx = target_original_point.vx;
        tp.vy = target_original_point.vy;
        tp.vz = target_original_point.vz;

        track_filter_point.range_p = target_original_point.range;
        track_filter_point.azim_p = target_original_point.azim_deg;
        track_filter_point.elev_p = target_original_point.elev_deg;
        track_filter_point.x_p = target_original_point.x;
        track_filter_point.y_p = target_original_point.y;
        track_filter_point.z_p = target_original_point.z;
        track_filter_point.vx_p = target_original_point.vx;
        track_filter_point.vy_p = target_original_point.vy;
        track_filter_point.vz_p = target_original_point.vz;
    }
    tp.azim_index = target_original_point.azim_index;
    tp.free_time = target_original_point.free_time;
    convertLonLatAlt(tp, data_processing_settings);
    target_original_point.is_correlated = true;
    is_correlated = true;
    tp.is_correlated = is_correlated;
    target_track_points_list.append(tp);
    ++effective_track_size;
    last_measured_point_index = target_track_points_list.size() - 1;
}

void TargetTrack::addMeasuredTrackPointAlphaBetaConstant(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    TargetTrackPoint tp;
    tp.track_id = track_id;
    tp.radar_mode = target_original_point.radar_mode;
    tp.track_times = ++track_times;
    continuous_lost_times = 0;
    tp.lost_times = continuous_lost_times;
    tp.date_time = target_original_point.date_time;
    tp.azim_index = target_original_point.azim_index;
    tp.elev_index = target_original_point.elev_index;
    tp.vr = target_original_point.vr;
    tp.amp_sigma = target_original_point.amp_sigma;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_elev1 = target_original_point.amp_delta_elev1;
    tp.amp_delta_elev2 = target_original_point.amp_delta_elev2;
    tp.snr = target_original_point.snr;
    tp.cfar_neighbour_num = target_original_point.cfar_neighbour_num;
    tp.rcs = target_original_point.rcs;

    // 如果航迹原始点数大于1, 根据当前原始点和航迹点队列的 最后一个实测点来对正在加入的点的 vx vy vz 做外推
    if (target_track_points_list.size() >= 1)
    {
        double delta_free_time = fabs(target_original_point.free_time - target_track_points_list.at(last_measured_point_index).free_time);
        if (delta_free_time < EPS)
        {
            qDebug() << QString("[Warning]: 航迹更新, 但是自由时间不变!!!");
        }
        else
        {
            tp.vx = (target_original_point.x - target_track_points_list.at(last_measured_point_index).x) / delta_free_time;
            tp.vy = (target_original_point.y - target_track_points_list.at(last_measured_point_index).y) / delta_free_time;
            tp.vz = (target_original_point.z - target_track_points_list.at(last_measured_point_index).z) / delta_free_time;

            alpha_beta_k = 1;
            coef_alpha = data_processing_settings.smooth_track_settings[0].filter_track_coef_alpha;
            coef_beta = data_processing_settings.smooth_track_settings[0].filter_track_coef_beta;

            // 预测下一步
            track_predict_point.x_p = track_filter_point.x_p + track_filter_point.vx_p * delta_free_time;
            track_predict_point.y_p = track_filter_point.y_p + track_filter_point.vy_p * delta_free_time;
            track_predict_point.z_p = track_filter_point.z_p + track_filter_point.vz_p * delta_free_time;

            // 滤波下一步
            track_filter_point.x_p = track_predict_point.x_p + coef_alpha * (target_original_point.x - track_predict_point.x_p);
            track_filter_point.y_p = track_predict_point.y_p + coef_alpha * (target_original_point.y - track_predict_point.y_p);
            track_filter_point.z_p = track_predict_point.z_p + coef_alpha * (target_original_point.z - track_predict_point.z_p);
            if (track_filter_point.z_p < 0)
            {
                track_filter_point.z_p = 0;
            }
            track_filter_point.vx_p = track_filter_point.vx_p + coef_beta * (target_original_point.x - track_predict_point.x_p) / delta_free_time;
            track_filter_point.vy_p = track_filter_point.vy_p + coef_beta * (target_original_point.y - track_predict_point.y_p) / delta_free_time;
            track_filter_point.vz_p = track_filter_point.vz_p + coef_beta * (target_original_point.z - track_predict_point.z_p) / delta_free_time;
            track_filter_point.range_p = sqrt(pow(track_filter_point.x_p, 2) + pow(track_filter_point.y_p, 2) + pow(track_filter_point.z_p, 2));
            track_filter_point.azim_p = fmod(atan2(track_filter_point.y_p, track_filter_point.x_p) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
            track_filter_point.elev_p = asin(track_filter_point.z_p / track_filter_point.range_p) * RAD2DEG;

            tp.range = track_filter_point.range_p;
            tp.azim_deg = track_filter_point.azim_p;
            tp.elev_deg = track_filter_point.elev_p;
            tp.x = track_filter_point.x_p;
            tp.y = track_filter_point.y_p;
            tp.z = track_filter_point.z_p;
            tp.vx = track_filter_point.vx_p;
            tp.vy = track_filter_point.vy_p;
            tp.vz = track_filter_point.vz_p;
        }
    }
    else
    {
        tp.range = target_original_point.range;
        tp.azim_deg = target_original_point.azim_deg;
        tp.elev_deg = target_original_point.elev_deg;
        tp.x = target_original_point.x;
        tp.y = target_original_point.y;
        tp.z = target_original_point.z;
        tp.vx = target_original_point.vx;
        tp.vy = target_original_point.vy;
        tp.vz = target_original_point.vz;

        track_filter_point.range_p = target_original_point.range;
        track_filter_point.azim_p = target_original_point.azim_deg;
        track_filter_point.elev_p = target_original_point.elev_deg;
        track_filter_point.x_p = target_original_point.x;
        track_filter_point.y_p = target_original_point.y;
        track_filter_point.z_p = target_original_point.z;
        track_filter_point.vx_p = target_original_point.vx;
        track_filter_point.vy_p = target_original_point.vy;
        track_filter_point.vz_p = target_original_point.vz;
    }
    tp.azim_index = target_original_point.azim_index;
    tp.free_time = target_original_point.free_time;

    convertLonLatAlt(tp, data_processing_settings);

    target_original_point.is_correlated = true;
    is_correlated = true;
    tp.is_correlated = is_correlated;
    target_track_points_list.append(tp);
    ++effective_track_size;
    last_measured_point_index = target_track_points_list.size() - 1;
}

void TargetTrack::predictOneTrackPoint(double current_free_time, const DataProcessingSettings& m_data_processing_settings)
{
    ++continuous_lost_times;
    ++total_lost_times;
    TargetTrackPoint tp = target_track_points_list.last();
    if (fabs(tp.vr) >= 1)
    {
        double delta_free_time = fabs(current_free_time - tp.free_time);
        tp.range -= tp.vr * delta_free_time;
    }
    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    {
        tp.range = MIN_PREDICT_TARGET_RANGE;
    }
    tp.snr = 0;
    tp.cfar_neighbour_num = 0;
    // 根据外推后的距离、方位、俯仰外推 x y z
    tp.x = tp.range * cos(tp.elev_deg * DEG2RAD) * cos(tp.azim_deg * DEG2RAD);
    tp.y = tp.range * cos(tp.elev_deg * DEG2RAD) * sin(tp.azim_deg * DEG2RAD);
    tp.z = tp.range * sin(tp.elev_deg * DEG2RAD);
    tp.date_time = QDateTime::currentDateTime();
    tp.track_id = track_id;
    tp.track_times = track_times;
    tp.lost_times = continuous_lost_times;
    tp.free_time = current_free_time;
    tp.is_correlated = false;
    convertLonLatAlt(tp, m_data_processing_settings);
    is_correlated = true; // 如果已经外推了, 也改变标志位, 防止重复外推
    target_track_points_list.append(tp);
    ++effective_track_size;
}

void TargetTrack::predictOneTrackPointNotLost(double current_free_time, const DataProcessingSettings& m_data_processing_settings)
{
    TargetTrackPoint tp = target_track_points_list.last();
    if (fabs(tp.vr) >= 1)
    {
        double delta_free_time = fabs(current_free_time - tp.free_time);
        tp.range -= tp.vr * delta_free_time;
    }
    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    {
        tp.range = MIN_PREDICT_TARGET_RANGE;
    }
    tp.snr = 0;
    tp.cfar_neighbour_num = 0;
    // 根据外推后的距离、方位、俯仰外推 x y z
    tp.x = tp.range * cos(tp.elev_deg * DEG2RAD) * cos(tp.azim_deg * DEG2RAD);
    tp.y = tp.range * cos(tp.elev_deg * DEG2RAD) * sin(tp.azim_deg * DEG2RAD);
    tp.z = tp.range * sin(tp.elev_deg * DEG2RAD);
    tp.date_time = QDateTime::currentDateTime();
    tp.track_id = track_id;
    tp.track_times = track_times;
    tp.lost_times = continuous_lost_times;
    tp.free_time = current_free_time;
    tp.is_correlated = false;
    convertLonLatAlt(tp, m_data_processing_settings);
    is_correlated = true; // 如果已经外推了, 也改变标志位, 防止重复外推
    target_track_points_list.append(tp);
}

void TargetTrack::predictOneTrackPointAlphaBeta(double current_free_time, const DataProcessingSettings& m_data_processing_settings)
{
    ++continuous_lost_times;
    ++total_lost_times;
    alpha_beta_k -= 2;
    if (alpha_beta_k < 1)
    {
        alpha_beta_k = 1;
    }
    TargetTrackPoint tp = target_track_points_list.last();
    double delta_free_time = fabs(current_free_time - tp.free_time);
    tp.snr = 0;
    tp.cfar_neighbour_num = 0;
    // 根据外推后的距离、方位、俯仰外推 x y z
    tp.x += tp.vx * delta_free_time;
    tp.y += tp.vy * delta_free_time;
    tp.z += tp.vz * delta_free_time;
    if (tp.z < 0)
    {
        tp.z = 0;
    }

    tp.range = sqrt(pow(tp.x, 2) + pow(tp.y, 2) + pow(tp.z, 2));
    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    {
        tp.range = MIN_PREDICT_TARGET_RANGE;
    }
    tp.azim_deg = fmod(atan2(tp.y, tp.x) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
    tp.elev_deg = asin(tp.z / tp.range) * RAD2DEG;
    tp.date_time = QDateTime::currentDateTime();
    tp.track_id = track_id;
    tp.track_times = track_times;
    tp.lost_times = continuous_lost_times;
    tp.free_time = current_free_time;
    tp.is_correlated = false;
    convertLonLatAlt(tp, m_data_processing_settings);
    is_correlated = true; // 如果已经外推了, 也改变标志位, 防止重复外推
    target_track_points_list.append(tp);
    ++effective_track_size;
}

void TargetTrack::predictOneTrackPointNotLostAlphaBeta(double current_free_time, const DataProcessingSettings& m_data_processing_settings)
{
    TargetTrackPoint tp = target_track_points_list.last();
    double delta_free_time = fabs(current_free_time - tp.free_time);
    // 根据外推后的距离、方位、俯仰外推 x y z
    tp.x += tp.vx * delta_free_time;
    tp.y += tp.vy * delta_free_time;
    tp.z += tp.vz * delta_free_time;
    if (tp.z < 0)
    {
        tp.z = 0;
    }

    tp.range = sqrt(pow(tp.x, 2) + pow(tp.y, 2) + pow(tp.z, 2));
    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    {
        tp.range = MIN_PREDICT_TARGET_RANGE;
    }
    tp.snr = 0;
    tp.cfar_neighbour_num = 0;
    tp.azim_deg = fmod(atan2(tp.y, tp.x) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
    tp.elev_deg = asin(tp.z / tp.range) * RAD2DEG;
    tp.date_time = QDateTime::currentDateTime();
    tp.track_id = track_id;
    tp.track_times = track_times;
    tp.lost_times = continuous_lost_times;
    tp.free_time = current_free_time;
    tp.is_correlated = false;
    convertLonLatAlt(tp, m_data_processing_settings);

    is_correlated = true; // 如果已经外推了, 也改变标志位, 防止重复外推
    target_track_points_list.append(tp);

    alpha_beta_k = alpha_beta_k - 1;
    if (alpha_beta_k < 1)
    {
        alpha_beta_k = 1;
    }
}

void TargetTrack::updateTrackWithMeasureOrPredict(double current_free_time, const DataProcessingSettings& data_processing_settings)
{
    if (distance_to_point_index_map.isEmpty())
    {
        // 如果没有原始点迹与航迹关联, 则外推
        switch (data_processing_settings.smooth_track_settings[0].filter_track_method)
        {
        default:
            predictOneTrackPoint(current_free_time, data_processing_settings);
            break;
        case 1:
            predictOneTrackPointAlphaBeta(current_free_time, data_processing_settings);
            break;
        case 2:
            predictOneTrackPointAlphaBeta(current_free_time, data_processing_settings);
            break;
        }
    }
    else
    {
        // 否则用实测点更新
        for (QMap<double, TargetOriginalPoint*>::iterator it = distance_to_point_index_map.begin(); it != distance_to_point_index_map.end();)
        {
            if (it.value()->is_correlated) //如果这个点已经被别的航迹相关了, 则跳过
            {
                ++it;
                continue;
            }
            if (fabs(it.key() - it.value()->best_correlate_distance) < EPS)
            {
                switch (data_processing_settings.smooth_track_settings[0].filter_track_method)
                {
                default:
                    addMeasuredTrackPoint(*(it.value()), data_processing_settings);
                    break;
                case 1:
                    addMeasuredTrackPointAlphaBeta(*(it.value()), data_processing_settings);
                    break;
                case 2:
                    addMeasuredTrackPointAlphaBetaConstant(*(it.value()), data_processing_settings);
                    break;
                }
                ++it;
                break;
            }
            ++it;
        }
        distance_to_point_index_map.clear();
        if (!is_correlated)
        {
            // 如果没有原始点迹与航迹关联, 则外推
            switch (data_processing_settings.smooth_track_settings[0].filter_track_method)
            {
            default:
                predictOneTrackPoint(current_free_time, data_processing_settings);
                break;
            case 1:
                predictOneTrackPointAlphaBeta(current_free_time, data_processing_settings);
                break;
            case 2:
                predictOneTrackPointAlphaBeta(current_free_time, data_processing_settings);
                break;
            }
        }
    }
    updateEstablishedMode();
}

void TargetTrack::updateTrackWithMeasureOrPredictNotLost(double current_free_time, const DataProcessingSettings& data_processing_settings)
{
    if (distance_to_point_index_map.isEmpty())
    {
        // 如果没有原始点迹与航迹关联, 则外推
        switch (data_processing_settings.smooth_track_settings[0].filter_track_method)
        {
        default:
            predictOneTrackPointNotLost(current_free_time, data_processing_settings);
            break;
        case 1:
            predictOneTrackPointNotLostAlphaBeta(current_free_time, data_processing_settings);
            break;
        case 2:
            predictOneTrackPointNotLostAlphaBeta(current_free_time, data_processing_settings);
            break;
        }
    }
    else
    {
        // 否则用实测点更新
        for (QMap<double, TargetOriginalPoint*>::iterator it = distance_to_point_index_map.begin(); it != distance_to_point_index_map.end();)
        {
            if (it.value()->is_correlated) //如果这个点已经被别的航迹相关了, 则跳过
            {
                ++it;
                continue;
            }
            if (fabs(it.key() - it.value()->best_correlate_distance) < EPS)
            {
                switch (data_processing_settings.smooth_track_settings[0].filter_track_method)
                {
                default:
                    addMeasuredTrackPoint(*(it.value()), data_processing_settings);
                    break;
                case 1:
                    addMeasuredTrackPointAlphaBeta(*(it.value()), data_processing_settings);
                    break;
                case 2:
                    addMeasuredTrackPointAlphaBetaConstant(*(it.value()), data_processing_settings);
                    break;
                }
                ++it;
                break;
            }
            ++it;
        }
        distance_to_point_index_map.clear();
        if (!is_correlated)
        {
            // 如果没有原始点迹与航迹关联, 则外推
            switch (data_processing_settings.smooth_track_settings[0].filter_track_method)
            {
            default:
                predictOneTrackPointNotLost(current_free_time, data_processing_settings);
                break;
            case 1:
                predictOneTrackPointNotLostAlphaBeta(current_free_time, data_processing_settings);
                break;
            case 2:
                predictOneTrackPointNotLostAlphaBeta(current_free_time, data_processing_settings);
                break;
            }
        }
    }
    updateEstablishedMode();
}

void TargetTrack::convertLonLatAlt(TargetTrackPoint& track_point, const DataProcessingSettings& m_data_processing_settings)
{
    if (m_data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag)
    {
        // 水平距离, 北向夹角
        double range_target_to_radar = track_point.range * cos(track_point.elev_deg * DEG2RAD);
        double azim_target_to_radar_north = circularlyAddDouble(450, -track_point.azim_deg, 0, 360);
        convertRangeAzimToLonLat(m_data_processing_settings.radar_lon_lat_alt.radar_longitude,
                                 m_data_processing_settings.radar_lon_lat_alt.radar_latitude,
                                 range_target_to_radar,
                                 azim_target_to_radar_north,
                                 track_point.longitude,
                                 track_point.latitude);
        track_point.altitude = m_data_processing_settings.radar_lon_lat_alt.radar_altitude + track_point.z;
    }
}

bool TargetTrack::precorrelateWithOriginalPointTrackMode(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    //航迹与点迹预相关
    if (target_track_points_list.size() == 0)
    {
        return false;
    }

    TargetTrackPoint* tp = &(target_track_points_list[last_measured_point_index]); //取航迹的上一个实测点（即非外推点）
    double predicted_azim = tp->azim_deg;
    double predicted_elev = tp->elev_deg;
    double delta_azim = circularlyAddDouble(predicted_azim, -target_original_point.azim_deg, -180, 360);
    double delta_elev = circularlyAddDouble(predicted_elev, -target_original_point.elev_deg, -180, 360);
    double predicted_vr = tp->vr;
    double delta_free_time = fabs(target_original_point.free_time - tp->free_time);
    double delta_range = tp->range - 0.5 * (predicted_vr + target_original_point.vr) * delta_free_time - target_original_point.range;
    double predicted_delta_range = fabs(0.5 * (predicted_vr + target_original_point.vr) * delta_free_time);
    double range_threshold = predicted_delta_range;
    if (predicted_delta_range > data_processing_settings.correlate_thresholds[1].correlate_range_threshold_ceil)
    {
        range_threshold = data_processing_settings.correlate_thresholds[1].correlate_range_threshold_ceil;
    }
    else if (predicted_delta_range < data_processing_settings.correlate_thresholds[1].correlate_range_threshold_floor)
    {
        range_threshold = data_processing_settings.correlate_thresholds[1].correlate_range_threshold_floor;
    }

    // 根据速度方向设定速度门限
    double velocity_threshold = data_processing_settings.correlate_thresholds[1].correlate_velocity_threshold_same_direction;
    if (predicted_vr * target_original_point.vr < 0)
    {
        velocity_threshold = data_processing_settings.correlate_thresholds[1].correlate_velocity_threshold_opposite_direction;
    }
    double azim_threshold = 0;
    double elev_threshold = 0;

    if (continuous_lost_times <= 0)
    {
        if (tp->range <= data_processing_settings.correlate_thresholds[1].correlate_range_division_01)
        {
            azim_threshold = data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_0;
            elev_threshold = data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_0;
        }
        else if (tp->range <= data_processing_settings.correlate_thresholds[1].correlate_range_division_12)
        {
            azim_threshold = data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_1;
            elev_threshold = data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_1;
        }
        else
        {
            azim_threshold = data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_2;
            elev_threshold = data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_2;
        }
    }
    else
    {
        if (tp->range <= data_processing_settings.correlate_thresholds[1].correlate_range_division_12)
        {
            azim_threshold = data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_0;
            elev_threshold = data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_0;
        }
        else
        {
            azim_threshold = data_processing_settings.correlate_thresholds[1].correlate_azim_threshold_deg_1;
            elev_threshold = data_processing_settings.correlate_thresholds[1].correlate_elev_threshold_deg_1;
        }
    }

    if (fabs(delta_azim) < azim_threshold && fabs(delta_elev) < elev_threshold && fabs(delta_range) < range_threshold &&
        fabs(predicted_vr - target_original_point.vr) < velocity_threshold)
    {
        double distance = fabs(delta_range) / data_processing_settings.radar_wave_paremeters[target_original_point.radar_mode].range_unit +
                          fabs(delta_azim) / data_processing_settings.sigma_delta_parameters[0].beam_width +
                          fabs(delta_elev) / data_processing_settings.sigma_delta_parameters[1].beam_width;
        if (distance < target_original_point.best_correlate_distance)
        {
            target_original_point.best_correlate_distance = distance;
        }
        distance_to_point_index_map.insert(distance, &target_original_point);
        return true;
    }
    return false;
}

void TargetTrack::precorrelateWithOriginalPointListTrackMode(QList<TargetOriginalPoint>& original_points_batch, const DataProcessingSettings& data_processing_settings)
{
    if (target_track_points_list.size() == 0)
    {
        return;
    }

    distance_to_point_index_map.clear();
    for (int ii = 0; ii < original_points_batch.size(); ++ii)
    {
        if (original_points_batch[ii].is_combined || original_points_batch[ii].is_filtered || original_points_batch[ii].is_correlated)
        {
            continue;
        }

        // 第一个点按照搜索模式, 后面按照跟踪模式
        if (track_times_track_mode <= 0)
        {
            precorrelateWithOriginalPoint(original_points_batch[ii], data_processing_settings);
        }
        else
        {
            precorrelateWithOriginalPointTrackMode(original_points_batch[ii], data_processing_settings);
        }
    }
}

void TargetTrack::addMeasuredTrackPointTrackMode(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    ++track_times_track_mode;
    lost_times_track_mode = 0;
    continuous_lost_times = 0;
    TargetTrackPoint tp;
    tp.track_id = track_id;
    tp.radar_mode = target_original_point.radar_mode;
    tp.track_times = ++track_times;
    tp.lost_times = continuous_lost_times;
    tp.date_time = target_original_point.date_time;
    tp.range = target_original_point.range;
    tp.azim_index = target_original_point.azim_index;
    tp.azim_deg = target_original_point.azim_deg;
    tp.elev_index = target_original_point.elev_index;
    tp.elev_deg = target_original_point.elev_deg;
    tp.vr = target_original_point.vr;
    tp.amp_sigma = target_original_point.amp_sigma;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_elev1 = target_original_point.amp_delta_elev1;
    tp.amp_delta_elev2 = target_original_point.amp_delta_elev2;
    tp.snr = target_original_point.snr;
    tp.cfar_neighbour_num = target_original_point.cfar_neighbour_num;
    tp.rcs = target_original_point.rcs;
    tp.azim_index = target_original_point.azim_index;
    tp.free_time = target_original_point.free_time;

    tp.x = target_original_point.x;
    tp.y = target_original_point.y;
    tp.z = target_original_point.z;
    // 如果航迹原始点数大于1, 根据当前原始点和航迹点队列的 最后一个实测点来对正在加入的点的 vx vy vz 做外推
    if (target_track_points_list.size() >= 1)
    {
        double delta_free_time = fabs(target_original_point.free_time - target_track_points_list.at(last_measured_point_index).free_time);
        if (delta_free_time < EPS)
        {
            qDebug() << QString("[Warning]: 航迹更新, 但是自由时间不变!!!");
        }
        else
        {
            // 跟踪模式下，每 ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE 更新一次 vx vy vz
            if (track_times_track_mode % ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE == 1)
            {
                tp.vx = (target_original_point.x - target_track_points_list.at(last_measured_point_index).x) / delta_free_time;
                tp.vy = (target_original_point.y - target_track_points_list.at(last_measured_point_index).y) / delta_free_time;
                tp.vz = (target_original_point.z - target_track_points_list.at(last_measured_point_index).z) / delta_free_time;

                // 注意, 为了滤波可以正常工作, 在没有打开滤波开关的时候, 需要把当前原始点实时设置为滤波初始值
                track_filter_point.range_p = target_original_point.range;
                track_filter_point.azim_p = target_original_point.azim_deg;
                track_filter_point.elev_p = target_original_point.elev_deg;
                track_filter_point.x_p = target_original_point.x;
                track_filter_point.y_p = target_original_point.y;
                track_filter_point.z_p = target_original_point.z;
                track_filter_point.vx_p = target_original_point.vx;
                track_filter_point.vy_p = target_original_point.vy;
                track_filter_point.vz_p = target_original_point.vz;

                convertLonLatAlt(tp, data_processing_settings);
                target_original_point.is_correlated = true;
                is_correlated = true;
                tp.is_correlated = is_correlated;
                target_track_points_list.append(tp);
                ++effective_track_size;

                // 跟踪模式下，每 ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE 更新一次 last_measured_point_index
                last_measured_point_index = target_track_points_list.size() - 1;
                return;
            }
            else
            {
                tp.vx = target_track_points_list.at(last_measured_point_index).vx;
                tp.vy = target_track_points_list.at(last_measured_point_index).vy;
                tp.vz = target_track_points_list.at(last_measured_point_index).vz;
            }
        }
    }
    else
    {
        tp.vx = target_original_point.vx;
        tp.vy = target_original_point.vy;
        tp.vz = target_original_point.vz;
    }

    convertLonLatAlt(tp, data_processing_settings);
    target_original_point.is_correlated = true;
    is_correlated = true;
    tp.is_correlated = is_correlated;
    target_track_points_list.append(tp);
    ++effective_track_size;
    ++track_size_track_mode;
}

void TargetTrack::addMeasuredTrackPointTrackModeAlphaBeta(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    ++track_times_track_mode;
    lost_times_track_mode = 0;
    continuous_lost_times = 0;
    TargetTrackPoint tp;
    tp.track_id = track_id;
    tp.radar_mode = target_original_point.radar_mode;
    tp.track_times = ++track_times;
    tp.lost_times = continuous_lost_times;
    tp.date_time = target_original_point.date_time;
    tp.azim_index = target_original_point.azim_index;
    tp.elev_index = target_original_point.elev_index;
    tp.vr = target_original_point.vr;
    tp.amp_sigma = target_original_point.amp_sigma;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_elev1 = target_original_point.amp_delta_elev1;
    tp.amp_delta_elev2 = target_original_point.amp_delta_elev2;
    tp.snr = target_original_point.snr;
    tp.cfar_neighbour_num = target_original_point.cfar_neighbour_num;
    tp.rcs = target_original_point.rcs;
    tp.azim_index = target_original_point.azim_index;
    tp.free_time = target_original_point.free_time;

    tp.x = target_original_point.x;
    tp.y = target_original_point.y;
    tp.z = target_original_point.z;
    // 如果跟踪模式的航迹原始点数大于1, 根据当前原始点和航迹点队列的 最后一个实测点来对正在加入的点的 vx vy vz 做外推
    if (track_times_track_mode >= 1)
    {
        double delta_free_time = fabs(target_original_point.free_time - target_track_points_list.at(last_measured_point_index).free_time);
        if (delta_free_time < EPS)
        {
            qDebug() << QString("[Warning]: 航迹更新, 但是自由时间不变!!!");
        }
        else
        {
            // 跟踪模式下，每 ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE 更新一次 vx vy vz
            if (track_times_track_mode % ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE == 1)
            {
                tp.vx = (target_original_point.x - target_track_points_list.at(last_measured_point_index).x) / delta_free_time;
                tp.vy = (target_original_point.y - target_track_points_list.at(last_measured_point_index).y) / delta_free_time;
                tp.vz = (target_original_point.z - target_track_points_list.at(last_measured_point_index).z) / delta_free_time;

                ++alpha_beta_k;
                if (alpha_beta_k > VARIABLE_ALPHA_BETA_MAX_ITERATION_NUM)
                {
                    alpha_beta_k = VARIABLE_ALPHA_BETA_MAX_ITERATION_NUM;
                }
                coef_alpha = ALPHA_BETA_FILER_ALPHA_ARRAY[alpha_beta_k];
                coef_beta = ALPHA_BETA_FILER_BETA_ARRAY[alpha_beta_k];

                // 预测下一步
                track_predict_point.x_p = track_filter_point.x_p + track_filter_point.vx_p * delta_free_time;
                track_predict_point.y_p = track_filter_point.y_p + track_filter_point.vy_p * delta_free_time;
                track_predict_point.z_p = track_filter_point.z_p + track_filter_point.vz_p * delta_free_time;

                // 滤波下一步
                track_filter_point.x_p = track_predict_point.x_p + coef_alpha * (target_original_point.x - track_predict_point.x_p);
                track_filter_point.y_p = track_predict_point.y_p + coef_alpha * (target_original_point.y - track_predict_point.y_p);
                track_filter_point.z_p = track_predict_point.z_p + coef_alpha * (target_original_point.z - track_predict_point.z_p);
                track_filter_point.vx_p = track_filter_point.vx_p + coef_beta * (target_original_point.x - track_predict_point.x_p) / delta_free_time;
                track_filter_point.vy_p = track_filter_point.vy_p + coef_beta * (target_original_point.y - track_predict_point.y_p) / delta_free_time;
                track_filter_point.vz_p = track_filter_point.vz_p + coef_beta * (target_original_point.z - track_predict_point.z_p) / delta_free_time;
                track_filter_point.range_p = sqrt(pow(track_filter_point.x_p, 2) + pow(track_filter_point.y_p, 2) + pow(track_filter_point.z_p, 2));
                track_filter_point.azim_p = fmod(atan2(track_filter_point.y_p, track_filter_point.x_p) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
                track_filter_point.elev_p = asin(track_filter_point.z_p / track_filter_point.range_p) * RAD2DEG;

                tp.range = track_filter_point.range_p;
                tp.azim_deg = track_filter_point.azim_p;
                tp.elev_deg = track_filter_point.elev_p;
                tp.x = track_filter_point.x_p;
                tp.y = track_filter_point.y_p;
                tp.z = track_filter_point.z_p;
                tp.vx = track_filter_point.vx_p;
                tp.vy = track_filter_point.vy_p;
                tp.vz = track_filter_point.vz_p;

                convertLonLatAlt(tp, data_processing_settings);
                target_original_point.is_correlated = true;
                is_correlated = true;
                tp.is_correlated = is_correlated;
                target_track_points_list.append(tp);
                ++effective_track_size;

                // 跟踪模式下，每 ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE 更新一次 last_measured_point_index
                last_measured_point_index = target_track_points_list.size() - 1;
                return;
            }
            else
            {
                tp.vx = target_track_points_list.at(last_measured_point_index).vx;
                tp.vy = target_track_points_list.at(last_measured_point_index).vy;
                tp.vz = target_track_points_list.at(last_measured_point_index).vz;
            }
        }
    }
    else
    {
        tp.range = target_original_point.range;
        tp.azim_deg = target_original_point.azim_deg;
        tp.elev_deg = target_original_point.elev_deg;
        tp.x = target_original_point.x;
        tp.y = target_original_point.y;
        tp.z = target_original_point.z;
        tp.vx = target_original_point.vx;
        tp.vy = target_original_point.vy;
        tp.vz = target_original_point.vz;

        // 跟踪模式初始化跟踪参数
        alpha_beta_k = 1;
        track_filter_point.range_p = target_original_point.range;
        track_filter_point.azim_p = target_original_point.azim_deg;
        track_filter_point.elev_p = target_original_point.elev_deg;
        track_filter_point.x_p = target_original_point.x;
        track_filter_point.y_p = target_original_point.y;
        track_filter_point.z_p = target_original_point.z;
        track_filter_point.vx_p = target_original_point.vx;
        track_filter_point.vy_p = target_original_point.vy;
        track_filter_point.vz_p = target_original_point.vz;
    }

    convertLonLatAlt(tp, data_processing_settings);

    target_original_point.is_correlated = true;
    is_correlated = true;
    tp.is_correlated = is_correlated;
    target_track_points_list.append(tp);
    ++track_size_track_mode;
    ++effective_track_size;
}

void TargetTrack::addMeasuredTrackPointTrackModeAlphaBetaConstant(TargetOriginalPoint& target_original_point, const DataProcessingSettings& data_processing_settings)
{
    ++track_times_track_mode;
    lost_times_track_mode = 0;
    continuous_lost_times = 0;
    TargetTrackPoint tp;
    tp.track_id = track_id;
    tp.radar_mode = target_original_point.radar_mode;
    tp.track_times = ++track_times;
    tp.lost_times = continuous_lost_times;
    tp.date_time = target_original_point.date_time;
    tp.azim_index = target_original_point.azim_index;
    tp.elev_index = target_original_point.elev_index;
    tp.vr = target_original_point.vr;
    tp.amp_sigma = target_original_point.amp_sigma;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_azim = target_original_point.amp_delta_azim;
    tp.amp_delta_elev1 = target_original_point.amp_delta_elev1;
    tp.amp_delta_elev2 = target_original_point.amp_delta_elev2;
    tp.snr = target_original_point.snr;
    tp.cfar_neighbour_num = target_original_point.cfar_neighbour_num;
    tp.rcs = target_original_point.rcs;

    tp.x = target_original_point.x;
    tp.y = target_original_point.y;
    tp.z = target_original_point.z;
    // 如果跟踪模式的航迹原始点数大于1, 根据当前原始点和航迹点队列的 最后一个实测点来对正在加入的点的 vx vy vz 做外推
    if (track_times_track_mode >= 1)
    {
        double delta_free_time = fabs(target_original_point.free_time - target_track_points_list.at(last_measured_point_index).free_time);
        if (delta_free_time < EPS)
        {
            qDebug() << QString("[Warning]: 航迹更新, 但是自由时间不变!!!");
        }
        else
        {
            // 跟踪模式下，每 ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE 更新一次 vx vy vz
            if (track_times_track_mode % ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE == 1)
            {
                tp.vx = (target_original_point.x - target_track_points_list.at(last_measured_point_index).x) / delta_free_time;
                tp.vy = (target_original_point.y - target_track_points_list.at(last_measured_point_index).y) / delta_free_time;
                tp.vz = (target_original_point.z - target_track_points_list.at(last_measured_point_index).z) / delta_free_time;

                alpha_beta_k = 1;
                coef_alpha = data_processing_settings.smooth_track_settings[1].filter_track_coef_alpha;
                coef_beta = data_processing_settings.smooth_track_settings[1].filter_track_coef_beta;

                // 预测下一步
                track_predict_point.x_p = track_filter_point.x_p + track_filter_point.vx_p * delta_free_time;
                track_predict_point.y_p = track_filter_point.y_p + track_filter_point.vy_p * delta_free_time;
                track_predict_point.z_p = track_filter_point.z_p + track_filter_point.vz_p * delta_free_time;

                // 滤波下一步
                track_filter_point.x_p = track_predict_point.x_p + coef_alpha * (target_original_point.x - track_predict_point.x_p);
                track_filter_point.y_p = track_predict_point.y_p + coef_alpha * (target_original_point.y - track_predict_point.y_p);
                track_filter_point.z_p = track_predict_point.z_p + coef_alpha * (target_original_point.z - track_predict_point.z_p);
                track_filter_point.vx_p = track_filter_point.vx_p + coef_beta * (target_original_point.x - track_predict_point.x_p) / delta_free_time;
                track_filter_point.vy_p = track_filter_point.vy_p + coef_beta * (target_original_point.y - track_predict_point.y_p) / delta_free_time;
                track_filter_point.vz_p = track_filter_point.vz_p + coef_beta * (target_original_point.z - track_predict_point.z_p) / delta_free_time;
                track_filter_point.range_p = sqrt(pow(track_filter_point.x_p, 2) + pow(track_filter_point.y_p, 2) + pow(track_filter_point.z_p, 2));
                track_filter_point.azim_p = fmod(atan2(track_filter_point.y_p, track_filter_point.x_p) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
                track_filter_point.elev_p = asin(track_filter_point.z_p / track_filter_point.range_p) * RAD2DEG;

                tp.range = track_filter_point.range_p;
                tp.azim_deg = track_filter_point.azim_p;
                tp.elev_deg = track_filter_point.elev_p;
                tp.x = track_filter_point.x_p;
                tp.y = track_filter_point.y_p;
                tp.z = track_filter_point.z_p;
                tp.vx = track_filter_point.vx_p;
                tp.vy = track_filter_point.vy_p;
                tp.vz = track_filter_point.vz_p;

                convertLonLatAlt(tp, data_processing_settings);
                target_original_point.is_correlated = true;
                is_correlated = true;
                tp.is_correlated = is_correlated;
                target_track_points_list.append(tp);
                ++effective_track_size;

                // 跟踪模式下，每 ALPHA_BETA_ITERATION_INTERVAL_TRACK_MODE 更新一次 last_measured_point_index
                last_measured_point_index = target_track_points_list.size() - 1;
                return;
            }
            else
            {
                tp.vx = target_track_points_list.at(last_measured_point_index).vx;
                tp.vy = target_track_points_list.at(last_measured_point_index).vy;
                tp.vz = target_track_points_list.at(last_measured_point_index).vz;
            }
        }
    }
    else
    {
        tp.range = target_original_point.range;
        tp.azim_deg = target_original_point.azim_deg;
        tp.elev_deg = target_original_point.elev_deg;
        tp.x = target_original_point.x;
        tp.y = target_original_point.y;
        tp.z = target_original_point.z;
        tp.vx = target_original_point.vx;
        tp.vy = target_original_point.vy;
        tp.vz = target_original_point.vz;

        alpha_beta_k = 1;
        track_filter_point.range_p = target_original_point.range;
        track_filter_point.azim_p = target_original_point.azim_deg;
        track_filter_point.elev_p = target_original_point.elev_deg;
        track_filter_point.x_p = target_original_point.x;
        track_filter_point.y_p = target_original_point.y;
        track_filter_point.z_p = target_original_point.z;
        track_filter_point.vx_p = target_original_point.vx;
        track_filter_point.vy_p = target_original_point.vy;
        track_filter_point.vz_p = target_original_point.vz;
    }
    tp.azim_index = target_original_point.azim_index;
    tp.free_time = target_original_point.free_time;
    convertLonLatAlt(tp, data_processing_settings);
    target_original_point.is_correlated = true;
    is_correlated = true;
    tp.is_correlated = is_correlated;
    target_track_points_list.append(tp);
    ++track_size_track_mode;
    ++effective_track_size;
}

void TargetTrack::predictOneTrackPointTrackMode(double current_free_time, const DataProcessingSettings& m_data_processing_settings)
{
    ++lost_times_track_mode;
    continuous_lost_times = lost_times_track_mode / CONTINUOUS_LOST_TIMES_TRACK_MODE;

    // 通过这里控制跟踪模式什么时候算丢失
    if (lost_times_track_mode % CONTINUOUS_LOST_TIMES_TRACK_MODE == 0)
    {
        ++total_lost_times;
        alpha_beta_k = alpha_beta_k - 2;
        if (alpha_beta_k < 1)
        {
            alpha_beta_k = 1;
        }
    }

    TargetTrackPoint tp = target_track_points_list.last();
    //    if (fabs(tp.vr) >= 1)
    //    {
    //        double delta_free_time = circularlyAddDouble(current_free_time, -tp.free_time, 0, CUMULATIVE_FREE_TIME_MAX);
    //        tp.range -= tp.vr * delta_free_time;
    //    }
    //    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    //    {
    //        tp.range = MIN_PREDICT_TARGET_RANGE;
    //    }
    //    // 根据外推后的距离、方位、俯仰外推 x y z
    //    tp.x = tp.range * cos(tp.elev_deg * DEG2RAD) * cos(tp.azim_deg * DEG2RAD);
    //    tp.y = tp.range * cos(tp.elev_deg * DEG2RAD) * sin(tp.azim_deg * DEG2RAD);
    //    tp.z = tp.range * sin(tp.elev_deg * DEG2RAD);

    double delta_free_time = fabs(current_free_time - tp.free_time);
    // 根据外推后的距离、方位、俯仰外推 x y z
    tp.x += tp.vx * delta_free_time;
    tp.y += tp.vy * delta_free_time;
    tp.z += tp.vz * delta_free_time;
    if (tp.z < 0)
    {
        tp.z = 0;
    }

    tp.range = sqrt(pow(tp.x, 2) + pow(tp.y, 2) + pow(tp.z, 2));
    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    {
        tp.range = MIN_PREDICT_TARGET_RANGE;
    }
    tp.azim_deg = fmod(atan2(tp.y, tp.x) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
    tp.elev_deg = asin(tp.z / tp.range) * RAD2DEG;
    tp.snr = 0;
    tp.cfar_neighbour_num = 0;
    tp.date_time = QDateTime::currentDateTime();
    tp.track_id = track_id;
    tp.track_times = track_times;
    tp.lost_times = continuous_lost_times;
    tp.free_time = current_free_time;
    tp.is_correlated = false;
    convertLonLatAlt(tp, m_data_processing_settings);
    is_correlated = true; // 如果已经外推了, 也改变标志位, 防止重复外推
    target_track_points_list.append(tp);
    ++track_size_track_mode;
    ++effective_track_size;
}

void TargetTrack::predictOneTrackPointTrackModeAlphaBeta(double current_free_time, const DataProcessingSettings& m_data_processing_settings)
{
    ++lost_times_track_mode;
    continuous_lost_times = lost_times_track_mode / CONTINUOUS_LOST_TIMES_TRACK_MODE;
    // 通过这里控制跟踪模式什么时候算丢失
    if (lost_times_track_mode % CONTINUOUS_LOST_TIMES_TRACK_MODE == 0)
    {
        ++total_lost_times;
        alpha_beta_k = alpha_beta_k - 2;
        if (alpha_beta_k < 1)
        {
            alpha_beta_k = 1;
        }
    }
    TargetTrackPoint tp = target_track_points_list.last();
    double delta_free_time = fabs(current_free_time - tp.free_time);
    // 根据外推后的距离、方位、俯仰外推 x y z
    tp.x += tp.vx * delta_free_time;
    tp.y += tp.vy * delta_free_time;
    tp.z += tp.vz * delta_free_time;
    if (tp.z < 0)
    {
        tp.z = 0;
    }

    tp.range = sqrt(pow(tp.x, 2) + pow(tp.y, 2) + pow(tp.z, 2));
    if (tp.range < MIN_PREDICT_TARGET_RANGE)
    {
        tp.range = MIN_PREDICT_TARGET_RANGE;
    }
    tp.azim_deg = fmod(atan2(tp.y, tp.x) * RAD2DEG + 360, 360); // 注意: atan2返回-pi ~ pi
    tp.elev_deg = asin(tp.z / tp.range) * RAD2DEG;
    tp.snr = 0;
    tp.cfar_neighbour_num = 0;
    tp.date_time = QDateTime::currentDateTime();
    tp.track_id = track_id;
    tp.track_times = track_times;
    tp.lost_times = continuous_lost_times;
    tp.free_time = current_free_time;
    tp.is_correlated = false;
    convertLonLatAlt(tp, m_data_processing_settings);
    is_correlated = true; // 如果已经外推了, 也改变标志位, 防止重复外推
    target_track_points_list.append(tp);
    ++track_size_track_mode;
    ++effective_track_size;
}

void TargetTrack::updateTrackWithMeasureOrPredictTrackMode(double current_free_time, const DataProcessingSettings& data_processing_settings)
{
    if (distance_to_point_index_map.size() > 0)
    {
        switch (data_processing_settings.smooth_track_settings[1].filter_track_method)
        {
        default:
            addMeasuredTrackPointTrackMode(*(distance_to_point_index_map.first()), data_processing_settings);
            break;
        case 1:
            addMeasuredTrackPointTrackModeAlphaBeta(*(distance_to_point_index_map.first()), data_processing_settings);
            break;
        case 2:
            addMeasuredTrackPointTrackModeAlphaBetaConstant(*(distance_to_point_index_map.first()), data_processing_settings);
            break;
        }
        distance_to_point_index_map.clear();
    }
    else
    {
        switch (data_processing_settings.smooth_track_settings[1].filter_track_method)
        {
        default:
            predictOneTrackPointTrackMode(current_free_time, data_processing_settings);
            break;
        case 1:
            predictOneTrackPointTrackModeAlphaBeta(current_free_time, data_processing_settings);
            break;
        case 2:
            predictOneTrackPointTrackModeAlphaBeta(current_free_time, data_processing_settings);
            break;
        }
    }
    updateEstablishedMode();
}

void TargetTrack::updateEstablishedMode()
{
    // 判断是否终止
    if (effective_track_size <= MAX_START_TRACK_SIZE)
    {
        if (total_lost_times >= MAX_START_TRACK_LOST_TIMES)
        {
            target_track_points_list.last().established_mode = -1;
            return;
        }
    }
    else if (continuous_lost_times >= MAX_LOST_TIMES)
    {
        target_track_points_list.last().established_mode = -1;
        return;
    }

    if (track_times >= COMPLETELY_START_TRACK_TIMES)
    {
        target_track_points_list.last().established_mode = 2;
    }
    else if (track_times >= PREPARE_START_TRACK_TIMES)
    {
        target_track_points_list.last().established_mode = 1;
    }
    else
    {
        target_track_points_list.last().established_mode = 0;
    }
}

AzimSector::AzimSector(int index)
{
    azim_sector_index = index;
    target_original_points_list.clear();
    target_tracks_list.clear();
    last_radar_mission_index = 0;
    current_radar_mission_index = 0;
    current_free_time = 0;
}

void AzimSector::addTargetOriginalPoint(TargetOriginalPoint target_original_point)
{
    target_original_points_list.append(target_original_point);
}

void AzimSector::updateTracks(const DataProcessingSettings& data_processing_settings)
{
    for (QList<TargetTrack*>::iterator tt_it = target_tracks_list.begin(); tt_it != target_tracks_list.end();)
    {
        if (current_radar_mission_index == last_radar_mission_index)
        {
            // 如果本航迹未相关上, 则将其更新（因为航迹是从其他扇区转移过来的, 可能已经相关过了）
            if (!(*tt_it)->is_correlated)
            {
                (*tt_it)->updateTrackWithMeasureOrPredict(current_free_time, data_processing_settings);
            }
        }
        else
        {
            if ((*tt_it)->target_track_points_list.last().radar_mode == current_radar_mission_index)
            {
                if (!(*tt_it)->is_correlated)
                {
                    (*tt_it)->updateTrackWithMeasureOrPredict(current_free_time, data_processing_settings);
                }
            }
            else
            {
                if (!(*tt_it)->is_correlated)
                {
                    (*tt_it)->updateTrackWithMeasureOrPredictNotLost(current_free_time, data_processing_settings);
                }
            }
        }
        ++tt_it;
    }
}

AzimSectorListScanMode::AzimSectorListScanMode()
{
    azim_sector_list.clear();
    if (AZIM_SECTOR_NUM > 0)
    {
        azim_sector_list.reserve(AZIM_SECTOR_NUM);
        for (int ii = 0; ii < AZIM_SECTOR_NUM; ++ii)
        {
            azim_sector_list.append(AzimSector(ii));
        }
    }
    initializeDataProcessingAzimSectorIndexScamMode();
    unused_track_id_list.clear();
    unused_track_id_list.reserve(MAX_TRACK_ID_NUM);
    for (int ii = 0; ii < MAX_TRACK_ID_NUM; ++ii)
    {
        unused_track_id_list.append(ii + 1);
    }
}

void AzimSectorListScanMode::combineTwoOriginalPoints(TargetOriginalPoint& op1, TargetOriginalPoint& op2, const DataProcessingSettings& data_processing_settings)
{
    if (op1.radar_mode == op2.radar_mode)
    {
        // [Warning:] 注意计算多普勒的差值需要循环相减, 比如积累次数是256时, 1和255可以融合。
        if ((fabs(op1.range_index - op2.range_index) <= data_processing_settings.combine_thresholds.combine_range_unit_threshold) &&
            (fabs(circularlyAddInt(op1.doppler_index,
                                   -op2.doppler_index,
                                   -data_processing_settings.radar_wave_paremeters[op1.radar_mode].mtd_num / 2,
                                   data_processing_settings.radar_wave_paremeters[op1.radar_mode].mtd_num)) <=
             data_processing_settings.combine_thresholds.combine_doppler_unit_threshold) &&
            (fabs(circularlyAddDouble(op1.azim_deg, -op2.azim_deg, -180, 360)) <= data_processing_settings.combine_thresholds.combine_azim_threshold_deg) &&
            (fabs(op1.elev_deg - op2.elev_deg) <= data_processing_settings.combine_thresholds.combine_elev_threshold_deg))

        {
            if (op1.amp_sigma >= op2.amp_sigma)
            {
                op2.is_combined = true;
                // [Warning]: 在融合的时候, 用能量强的目标的能量替换弱目标的能量, 确保后续把能融合的点都融合掉！
                //                    op2.amp_sigma = op1.amp_sigma;
            }
            else
            {
                op1.is_combined = true;
                //                    op1.amp_sigma = op2.amp_sigma;
            }
        }
    }
    else
    {
        if ((fabs(op1.range - op2.range) <= data_processing_settings.combine_thresholds.combine_range_threshold) &&
            (fabs(op1.vr - op2.vr) <= data_processing_settings.combine_thresholds.combine_velocity_threshold) &&
            (fabs(circularlyAddDouble(op1.azim_deg, -op2.azim_deg, -180, 360)) <= data_processing_settings.combine_thresholds.combine_azim_threshold_deg) &&
            (fabs(op1.elev_deg - op2.elev_deg) <= data_processing_settings.combine_thresholds.combine_elev_threshold_deg))
        {
            if (op1.amp_sigma >= op2.amp_sigma)
            {
                op2.is_combined = true;
                //                    op2.amp_sigma = op1.amp_sigma;
            }
            else
            {
                op1.is_combined = true;
                //                    op1.amp_sigma = op2.amp_sigma;
            }
        }
    }
}

void AzimSectorListScanMode::combineTwoOriginalPointsWeighted(TargetOriginalPoint& op1, TargetOriginalPoint& op2, const DataProcessingSettings& data_processing_settings)
{
    double delta_azim = circularlyAddDouble(op1.azim_deg, -op2.azim_deg, -180, 360);
    // 方位需要注意, 在0度附近的加权问题
    if (op1.radar_mode == op2.radar_mode)
    {
        // [Warning:] 注意计算多普勒的差值需要循环相减, 比如积累次数是256时, 1和255可以融合。
        if ((fabs(op1.range_index - op2.range_index) <= data_processing_settings.combine_thresholds.combine_range_unit_threshold) &&
            (fabs(circularlyAddInt(op1.doppler_index,
                                   -op2.doppler_index,
                                   -data_processing_settings.radar_wave_paremeters[op1.radar_mode].mtd_num / 2,
                                   data_processing_settings.radar_wave_paremeters[op1.radar_mode].mtd_num)) <=
             data_processing_settings.combine_thresholds.combine_doppler_unit_threshold) &&
            (fabs(delta_azim) <= data_processing_settings.combine_thresholds.combine_azim_threshold_deg) &&
            (fabs(op1.elev_deg - op2.elev_deg) <= data_processing_settings.combine_thresholds.combine_elev_threshold_deg))

        {
            if (op1.amp_sigma >= op2.amp_sigma)
            {
                double total_amp = fabs(op1.amp_sigma) + fabs(op2.amp_sigma);
                if (total_amp > EPS)
                {
                    double alpha_op1 = fabs(op1.amp_sigma) / total_amp;
                    double alpha_op2 = fabs(op2.amp_sigma) / total_amp;
                    if (data_processing_settings.sigma_delta_parameters[0].weight_flag)
                    {
                        op1.azim_deg = circularlyAddDouble(alpha_op1 * delta_azim, op2.azim_deg, 0, 360);
                    }
                    if (data_processing_settings.sigma_delta_parameters[1].weight_flag)
                    {
                        op1.elev_deg = alpha_op1 * op1.elev_deg + alpha_op2 * op2.elev_deg;
                    }
                }

                op2.is_combined = true;
                // [Warning]: 在融合的时候, 用能量强的目标的能量替换弱目标的能量, 确保后续把能融合的点都融合掉！
                //                    op2.amp_sigma = op1.amp_sigma;    // 不能更新 amp_sigma, 因为要根据 amp_sigma 计算加权方位
            }
            else
            {
                double total_amp = fabs(op1.amp_sigma) + fabs(op2.amp_sigma);
                if (total_amp > EPS)
                {
                    double alpha_op1 = fabs(op1.amp_sigma) / total_amp;
                    double alpha_op2 = fabs(op2.amp_sigma) / total_amp;
                    if (data_processing_settings.sigma_delta_parameters[0].weight_flag)
                    {
                        op2.azim_deg = circularlyAddDouble(alpha_op1 * delta_azim, op2.azim_deg, 0, 360);
                    }
                    if (data_processing_settings.sigma_delta_parameters[1].weight_flag)
                    {
                        op2.elev_deg = alpha_op1 * op1.elev_deg + alpha_op2 * op2.elev_deg;
                    }
                }

                op1.is_combined = true;
                //                    op1.amp_sigma = op2.amp_sigma;
            }
        }
    }
    else
    {
        if ((fabs(op1.range - op2.range) <= data_processing_settings.combine_thresholds.combine_range_threshold) &&
            (fabs(op1.vr - op2.vr) <= data_processing_settings.combine_thresholds.combine_velocity_threshold) &&
            (fabs(circularlyAddDouble(op1.azim_deg, -op2.azim_deg, -180, 360)) <= data_processing_settings.combine_thresholds.combine_azim_threshold_deg) &&
            (fabs(op1.elev_deg - op2.elev_deg) <= data_processing_settings.combine_thresholds.combine_elev_threshold_deg))
        {
            if (op1.amp_sigma >= op2.amp_sigma)
            {
                double total_amp = fabs(op1.amp_sigma) + fabs(op2.amp_sigma);
                if (total_amp > EPS)
                {
                    double alpha_op1 = fabs(op1.amp_sigma) / total_amp;
                    double alpha_op2 = fabs(op2.amp_sigma) / total_amp;
                    if (data_processing_settings.sigma_delta_parameters[0].weight_flag)
                    {
                        op1.azim_deg = circularlyAddDouble(alpha_op1 * delta_azim, op2.azim_deg, 0, 360);
                    }
                    if (data_processing_settings.sigma_delta_parameters[1].weight_flag)
                    {
                        op1.elev_deg = alpha_op1 * op1.elev_deg + alpha_op2 * op2.elev_deg;
                    }
                }
                op2.is_combined = true;
                //                    op2.amp_sigma = op1.amp_sigma;
            }
            else
            {
                double total_amp = fabs(op1.amp_sigma) + fabs(op2.amp_sigma);
                if (total_amp > EPS)
                {
                    double alpha_op1 = fabs(op1.amp_sigma) / total_amp;
                    double alpha_op2 = fabs(op2.amp_sigma) / total_amp;
                    if (data_processing_settings.sigma_delta_parameters[0].weight_flag)
                    {
                        op2.azim_deg = circularlyAddDouble(alpha_op1 * delta_azim, op2.azim_deg, 0, 360);
                    }
                    if (data_processing_settings.sigma_delta_parameters[1].weight_flag)
                    {
                        op2.elev_deg = alpha_op1 * op1.elev_deg + alpha_op2 * op2.elev_deg;
                    }
                }
                op1.is_combined = true;
                //                    op1.amp_sigma = op2.amp_sigma;
            }
        }
    }
}

void AzimSectorListScanMode::initializeDataProcessingAzimSectorIndexScamMode()
{
    scan_current_input_index = 0;
    scan_prepare_combine_index = scan_current_input_index;                                                                                    // 0
    scan_current_combine_index = circularlyAddInt(scan_prepare_combine_index, 1, 0, AZIM_SECTOR_NUM);                                         // 1
    scan_current_precorrelate_index = circularlyAddInt(scan_current_combine_index, COMBINE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM);        // 3
    scan_current_track_update_index = circularlyAddInt(scan_current_precorrelate_index, CORRELATE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM); // 5
    scan_current_track_start_index = circularlyAddInt(scan_current_track_update_index, CORRELATE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM);  // 7
}

void AzimSectorListScanMode::combineCurrentSectorWithPreviousSectors(const DataProcessingSettings& data_processing_settings)
{
    //本扇区内部融合
    int op_size = azim_sector_list[scan_current_combine_index].target_original_points_list.size();
    for (int ii = 0; ii < op_size - 1; ++ii)
    {
        TargetOriginalPoint& op1 = azim_sector_list[scan_current_combine_index].target_original_points_list[ii];
        if (op1.is_correlated)
        {
            continue;
        }
        for (int jj = ii + 1; jj < op_size; ++jj)
        {
            TargetOriginalPoint& op2 = azim_sector_list[scan_current_combine_index].target_original_points_list[jj];
            if (op2.is_correlated)
            {
                continue;
            }
            combineTwoOriginalPoints(op1, op2, data_processing_settings);
        }
    }
    //本扇区与前 COMBINE_NEIGHBOUR_SECTORS_NUM 个扇区之间进行融合
    for (int ii = 0; ii < op_size; ++ii)
    {
        TargetOriginalPoint& op1 = azim_sector_list[scan_current_combine_index].target_original_points_list[ii];
        if (op1.is_correlated)
        {
            continue;
        }
        for (int jj = 1; jj <= COMBINE_NEIGHBOUR_SECTORS_NUM; ++jj)
        {
            AzimSector& azim_sector_before =
                azim_sector_list[circularlyAddInt(scan_current_combine_index, jj, 0, AZIM_SECTOR_NUM)]; //考虑当前扇区0, 前两个扇区是99、98-- 当前扇区-1之后要在[0,100)之间
            int s = azim_sector_before.target_original_points_list.size();
            for (int k = 0; k < s; ++k)
            {
                TargetOriginalPoint& op2 = azim_sector_before.target_original_points_list[k];
                if (op2.is_correlated)
                {
                    continue;
                }
                combineTwoOriginalPoints(op1, op2, data_processing_settings);
            }
        }
    }
}

void AzimSectorListScanMode::precorrelateTracksWithOriginalPoints(const DataProcessingSettings& data_processing_settings)
{
    // 当前扇区的原始点迹与当前扇区及其左右 CORRELATE_NEIGHBOUR_SECTORS_NUM 个扇区做相关处理
    if (azim_sector_list[scan_current_precorrelate_index].target_original_points_list.isEmpty())
    {
        return;
    }

    for (int ii = 0; ii < 2 * CORRELATE_NEIGHBOUR_SECTORS_NUM + 1; ++ii)
    {
        int precorrelate_index = circularlyAddInt(scan_current_precorrelate_index, ii - CORRELATE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM);
        for (int jj = 0; jj < azim_sector_list[precorrelate_index].target_tracks_list.size(); ++jj)
        {
            if (!azim_sector_list[precorrelate_index].target_tracks_list[jj]->is_correlated)
            {
                for (int kk = 0; kk < azim_sector_list[scan_current_precorrelate_index].target_original_points_list.size(); ++kk)
                {
                    if (azim_sector_list[scan_current_precorrelate_index].target_original_points_list[kk].is_combined ||
                        azim_sector_list[scan_current_precorrelate_index].target_original_points_list[kk].is_filtered ||
                        azim_sector_list[scan_current_precorrelate_index].target_original_points_list[kk].is_correlated)
                    {
                        continue;
                    }
                    azim_sector_list[precorrelate_index].target_tracks_list[jj]->precorrelateWithOriginalPoint(
                        azim_sector_list[scan_current_precorrelate_index].target_original_points_list[kk],
                        data_processing_settings);
                }
            }
        }
    }
}

void AzimSectorListScanMode::updateTracks(const DataProcessingSettings& data_processing_settings)
{
    // [Warning]: S1: 航迹更新(滤波和外推)和航迹终止; S2: 更新航迹所在的扇区、删除终止的航迹、航迹关联的标志位, 此步骤在线程中进行, 配合航迹发送和航迹输出!
    azim_sector_list[scan_current_track_update_index].updateTracks(data_processing_settings);
}

void AzimSectorListScanMode::updateCombineAndPrecorrelateAndTrackAndUpdateAndTrackStartIndex(int delta_index)
{
    scan_prepare_combine_index = circularlyAddInt(scan_prepare_combine_index, delta_index, 0, AZIM_SECTOR_NUM);
    scan_current_combine_index = circularlyAddInt(scan_prepare_combine_index, 1, 0, AZIM_SECTOR_NUM);
    scan_current_precorrelate_index = circularlyAddInt(scan_current_combine_index, COMBINE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM);
    scan_current_track_update_index = circularlyAddInt(scan_current_precorrelate_index, CORRELATE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM);
    scan_current_track_start_index = circularlyAddInt(scan_current_track_update_index, CORRELATE_NEIGHBOUR_SECTORS_NUM, 0, AZIM_SECTOR_NUM);
}

OriginalPointsBatchTrackMode::OriginalPointsBatchTrackMode()
{
    target_original_points_list.clear();
    current_free_time = 0;
}

void OriginalPointsBatchTrackMode::combineTwoOriginalPointsTrackMode(TargetOriginalPoint& op1, TargetOriginalPoint& op2, const DataProcessingSettings& data_processing_settings)
{
    if (fabs(op1.range_index - op2.range_index) <= data_processing_settings.combine_thresholds.combine_range_unit_threshold &&
        fabs(circularlyAddInt(op1.doppler_index,
                              -op2.doppler_index,
                              -data_processing_settings.radar_wave_paremeters[op1.radar_mode].mtd_num / 2,
                              data_processing_settings.radar_wave_paremeters[op1.radar_mode].mtd_num)) <=
            4 * data_processing_settings.combine_thresholds.combine_doppler_unit_threshold)
    {
        if (op1.amp_sigma >= op2.amp_sigma)
        {
            op2.is_combined = true;
        }
        else
        {
            op1.is_combined = true;
        }
    }
}

void OriginalPointsBatchTrackMode::combineCurrentOriginalPointsBatch(const DataProcessingSettings& data_processing_settings)
{
    int op_size = target_original_points_list.size();
    for (int i = 0; i < op_size - 1; ++i)
    {
        TargetOriginalPoint& op1 = target_original_points_list[i];
        if (op1.is_correlated)
        {
            continue;
        }
        for (int j = i + 1; j < op_size; ++j)
        {
            TargetOriginalPoint& op2 = target_original_points_list[j];
            if (op2.is_correlated)
            {
                continue;
            }
            combineTwoOriginalPointsTrackMode(op1, op2, data_processing_settings);
        }
    }
}

void OriginalPointsBatchTrackMode::addTargetOriginalPoint(TargetOriginalPoint target_original_point)
{
    target_original_points_list.append(target_original_point);
}

// 显示的航迹信息
void initializeTrackInfoDisplayed(TrackInfoDisplayed& track_info_displayed)
{
    track_info_displayed.track_no = -1;
    track_info_displayed.range = 0;
    track_info_displayed.azim_deg = 0;
    track_info_displayed.elev_deg = 0;
    track_info_displayed.velocity = 0;
    track_info_displayed.amp = 0;
    track_info_displayed.snr = 0;
    track_info_displayed.track_times = 0;
    track_info_displayed.lost_times = 0;
    track_info_displayed.x = 0;
    track_info_displayed.y = 0;
    track_info_displayed.z = 0;
    track_info_displayed.ve = 0;
    track_info_displayed.vn = 0;
    track_info_displayed.vz = 0;
    track_info_displayed.longitude = 0;
    track_info_displayed.latitude = 0;
    track_info_displayed.altitude = 0;
    track_info_displayed.is_visible = false;
}

void updateTrackInfoDisplayed(TrackInfoDisplayed& track_info_displayed, const TargetTrack* target_track)
{
    track_info_displayed.track_no = target_track->target_track_points_list.last().track_id;
    track_info_displayed.range = target_track->target_track_points_list.last().range;
    track_info_displayed.azim_deg = circularlyAddDouble(450, -target_track->target_track_points_list.last().azim_deg, 0, 360);
    track_info_displayed.elev_deg = target_track->target_track_points_list.last().elev_deg;
    track_info_displayed.velocity = target_track->target_track_points_list.last().vr;
    track_info_displayed.amp = target_track->target_track_points_list.last().amp_sigma;
    track_info_displayed.snr = target_track->target_track_points_list.last().snr;
    track_info_displayed.track_times = target_track->target_track_points_list.last().track_times;
    track_info_displayed.lost_times = target_track->target_track_points_list.last().lost_times;
    track_info_displayed.x = target_track->target_track_points_list.last().x;
    track_info_displayed.y = target_track->target_track_points_list.last().y;
    track_info_displayed.z = target_track->target_track_points_list.last().z;
    track_info_displayed.ve = target_track->target_track_points_list.last().vx;
    track_info_displayed.vn = target_track->target_track_points_list.last().vy;
    track_info_displayed.vz = target_track->target_track_points_list.last().vz;
    track_info_displayed.longitude = target_track->target_track_points_list.last().longitude;
    track_info_displayed.latitude = target_track->target_track_points_list.last().latitude;
    track_info_displayed.altitude = target_track->target_track_points_list.last().altitude;
}

void initializeTrackingTargetPositionWithTrackConsiderCurrentAzim(TrackingTargetPosition& tracking_target_position,
                                                                  TargetTrack* target_track,
                                                                  const RadarSystemState& m_radar_system_state)
{
    initializeTrackingTargetPosition(tracking_target_position);
    if (target_track) //当前跟踪的航线不为空
    {
        tracking_target_position.range = target_track->target_track_points_list.last().range;

        for (int ii = 0; ii < USING_TRACK_MODE_NUM; ii++)
        {
            int current_track_mode_index = USING_TRACK_MODE_INDEX_MAP[ii];
            if (tracking_target_position.range >= RANGE_START[current_track_mode_index] && tracking_target_position.range < RANGE_END[current_track_mode_index])
            {
                tracking_target_position.last_radar_track_mode = tracking_target_position.current_radar_track_mode;
                tracking_target_position.current_radar_track_mode = current_track_mode_index;
                break;
            }
        }

        // 根据目标方位角和当前的角度指向, 计算从当前方位角转向到目标角度需要的时间
        double delta_time_for_pointing_to_target = 0.7;                                       // 0.7 是停止、指向需要的时间 (communicate_with_fpga_thread 的函数)
        double target_last_azim_deg = target_track->target_track_points_list.last().azim_deg; // 目标角度

        // 统一控制旋转一圈后停止到目标位置附近, 计算从当前角度顺时针旋转到目标位置的角度差
        double delta_azim_deg_pointing_to_target = circularlyAddDouble(m_radar_system_state.biss_angle_deg, -target_last_azim_deg, 0, 360);
        qDebug() << "【跟踪模式】: 当前指向角度到目标航迹的角度差为: " << delta_azim_deg_pointing_to_target << "当前码盘角度为: " << m_radar_system_state.biss_angle_deg;
        delta_time_for_pointing_to_target += (delta_azim_deg_pointing_to_target / (fabs(m_radar_system_state.turntable_rotating_speed) + EPS));
        qDebug() << "【跟踪模式】: 当前指向角度到目标航迹的需要的时间为: " << delta_time_for_pointing_to_target;

        // 外推1个点的方位角
        double predict_x = target_track->target_track_points_list.last().x + target_track->target_track_points_list.last().vx * delta_time_for_pointing_to_target;
        double predict_y = target_track->target_track_points_list.last().y + target_track->target_track_points_list.last().vy * delta_time_for_pointing_to_target;
        double predict_z = target_track->target_track_points_list.last().z + target_track->target_track_points_list.last().vz * delta_time_for_pointing_to_target;
        if (predict_z < 0)
        {
            predict_z = 0;
        }
        double predict_azim = circularlyAddDouble(atan2(predict_y, predict_x) * 180 / PI, 360, 0, 360);
        double predict_elev = circularlyAddDouble(atan2(predict_z, sqrt(predict_x * predict_x + predict_y * predict_y)) * 180 / PI, 360, 0, 360);

        qDebug() << "外推前的方位角为:" << target_track->target_track_points_list.last().azim_deg << "\n外推后的方位角为:" << predict_azim;
        qDebug() << "外推前的俯仰角为:" << target_track->target_track_points_list.last().elev_deg << "\n外推后的俯仰角为:" << predict_elev;

        tracking_target_position.current_pointing_azim_deg = predict_azim;
        tracking_target_position.last_but_one_track_point_azim_deg = tracking_target_position.current_pointing_azim_deg;
        tracking_target_position.last_track_point_azim_deg = tracking_target_position.current_pointing_azim_deg;
        tracking_target_position.azim_deg_deviate_times = 0;

        tracking_target_position.last_track_point_elev_index = target_track->target_track_points_list.last().elev_index;
        tracking_target_position.last_but_one_track_point_elev_index = target_track->target_track_points_list.last().elev_index;
        switch (target_track->target_track_points_list.last().elev_index)
        {
        case 0:
            tracking_target_position.elev_index_list[0] = 0;
            tracking_target_position.elev_index_list[1] = 1;
            tracking_target_position.elev_index_list[2] = 2;
            tracking_target_position.elev_index_list[3] = 3;
            break;
        case MAX_VALID_ELEV_BEAM_INDEX:
            tracking_target_position.elev_index_list[0] = MAX_VALID_ELEV_BEAM_INDEX - 2;
            tracking_target_position.elev_index_list[1] = MAX_VALID_ELEV_BEAM_INDEX - 1;
            tracking_target_position.elev_index_list[2] = MAX_VALID_ELEV_BEAM_INDEX;
            tracking_target_position.elev_index_list[3] = MAX_VALID_ELEV_BEAM_INDEX + 1;
            break;
        default:
            tracking_target_position.elev_index_list[0] = target_track->target_track_points_list.last().elev_index - 1;
            tracking_target_position.elev_index_list[1] = target_track->target_track_points_list.last().elev_index;
            tracking_target_position.elev_index_list[2] = target_track->target_track_points_list.last().elev_index + 1;
            tracking_target_position.elev_index_list[3] = target_track->target_track_points_list.last().elev_index + 2;
            break;
        }
        tracking_target_position.elev_index_deviate_times = 0;

        tracking_target_position.hardware_settings_needed_to_update_flag = false;

        qDebug() << "【跟踪模式】: 初始化目标位置："
                 << "range" << tracking_target_position.range << "\ntrack_mode" << tracking_target_position.current_radar_track_mode << "\nazim_deg"
                 << tracking_target_position.current_pointing_azim_deg << "target_elev_deg: " << target_track->target_track_points_list.last().elev_deg << '\n'
                 << "target_elev_index: " << target_track->target_track_points_list.last().elev_index << '\n'
                 << "elev_index[0]: " << tracking_target_position.elev_index_list[0] << '\n'
                 << "elev_index[1]: " << tracking_target_position.elev_index_list[1] << '\n'
                 << "elev_index[2]: " << tracking_target_position.elev_index_list[2] << '\n'
                 << "elev_index[3]: " << tracking_target_position.elev_index_list[3] << '\n';
    }
}

void updateTrackingTargetPosition(TrackingTargetPosition& tracking_target_position, TargetTrack* target_track)
{
    if (target_track)
    {
        tracking_target_position.range = target_track->target_track_points_list.last().range;
        for (int ii = 0; ii < USING_TRACK_MODE_NUM; ii++)
        {
            int current_track_mode_index = USING_TRACK_MODE_INDEX_MAP[ii];
            if (tracking_target_position.range > RANGE_START[current_track_mode_index] && tracking_target_position.range <= RANGE_END[current_track_mode_index])
            {
                tracking_target_position.current_radar_track_mode = current_track_mode_index;
                break;
            }
        }
        if (tracking_target_position.current_radar_track_mode != tracking_target_position.last_radar_track_mode)
        {
            tracking_target_position.hardware_settings_needed_to_update_flag = true;
            tracking_target_position.last_radar_track_mode = tracking_target_position.current_radar_track_mode;
        }

        if (tracking_target_position.azim_deg_deviate_times >= 2)
        {
            tracking_target_position.hardware_settings_needed_to_update_flag = true;
            // 取两个方位的平均值
            double delta_azim_deg = circularlyAddDouble(tracking_target_position.last_but_one_track_point_azim_deg, -tracking_target_position.last_track_point_azim_deg, -180, 360);
            tracking_target_position.current_pointing_azim_deg = circularlyAddDouble(tracking_target_position.last_track_point_azim_deg, delta_azim_deg / 2.0, 0, 360);
            tracking_target_position.last_track_point_azim_deg = tracking_target_position.current_pointing_azim_deg;
            tracking_target_position.last_but_one_track_point_azim_deg = tracking_target_position.current_pointing_azim_deg;
            tracking_target_position.azim_deg_deviate_times = 0;
        }

        if (tracking_target_position.elev_index_deviate_times >= 2)
        {
            double mean_elev_deg_index = (tracking_target_position.last_but_one_track_point_elev_index + tracking_target_position.last_track_point_elev_index) / 2;
            // 为了防止突变, 俯仰一个一个变动
            if (mean_elev_deg_index < tracking_target_position.elev_index_list[1])
            {
                if (tracking_target_position.elev_index_list[0] > 0)
                {
                    tracking_target_position.hardware_settings_needed_to_update_flag = true;
                    --tracking_target_position.elev_index_list[0];
                    --tracking_target_position.elev_index_list[1];
                    --tracking_target_position.elev_index_list[2];
                    --tracking_target_position.elev_index_list[3];
                }
            }
            else if (mean_elev_deg_index > tracking_target_position.elev_index_list[2])
            {
                if (tracking_target_position.elev_index_list[3] <= MAX_VALID_ELEV_BEAM_INDEX)
                {
                    tracking_target_position.hardware_settings_needed_to_update_flag = true;
                    ++tracking_target_position.elev_index_list[0];
                    ++tracking_target_position.elev_index_list[1];
                    ++tracking_target_position.elev_index_list[2];
                    ++tracking_target_position.elev_index_list[3];
                }
            }

            tracking_target_position.elev_index_deviate_times = 0;
        }

        qDebug() << "【跟踪模式】: 根据航迹最新点更新跟踪配置!";
        qDebug() << "range" << tracking_target_position.range << "\ntrack_mode" << tracking_target_position.current_radar_track_mode << "\nazim_deg"
                 << tracking_target_position.current_pointing_azim_deg << "target_elev_index: " << target_track->target_track_points_list.last().elev_index << '\n'
                 << "elev_index[0]: " << tracking_target_position.elev_index_list[0] << '\n'
                 << "elev_index[1]: " << tracking_target_position.elev_index_list[1] << '\n'
                 << "elev_index[2]: " << tracking_target_position.elev_index_list[2] << '\n'
                 << "elev_index[3]: " << tracking_target_position.elev_index_list[3] << '\n';
    }
}

bool needingUpdateTrackingTargetPosition(TrackingTargetPosition& tracking_target_position, TargetTrack* target_track)
{
    bool needing_update_flag = false;
    if (fabs(circularlyAddDouble(tracking_target_position.current_pointing_azim_deg, -target_track->target_track_points_list.last().azim_deg, -180, 360)) >
        MAX_CONTINUOUS_DEVIATE_AZIM_DEGREE)
    {
        tracking_target_position.last_but_one_track_point_azim_deg = tracking_target_position.last_track_point_azim_deg;
        tracking_target_position.last_track_point_azim_deg = target_track->target_track_points_list.last().azim_deg;
        ++tracking_target_position.azim_deg_deviate_times;
        needing_update_flag = true;
        qDebug() << "【跟踪模式】: 方位变化超标!!!" << tracking_target_position.azim_deg_deviate_times << "次!";
    }

    if (target_track->target_track_points_list.last().elev_index < tracking_target_position.elev_index_list[1] ||
        target_track->target_track_points_list.last().elev_index > tracking_target_position.elev_index_list[2])
    {
        tracking_target_position.last_but_one_track_point_elev_index = tracking_target_position.last_track_point_elev_index;
        tracking_target_position.last_track_point_elev_index = target_track->target_track_points_list.last().elev_index;
        ++tracking_target_position.elev_index_deviate_times;
        needing_update_flag = true;
        qDebug() << "【跟踪模式】: 俯仰变化超标!!!" << tracking_target_position.elev_index_deviate_times << "次!";
    }

    if (target_track->target_track_points_list.last().range < RANGE_START[tracking_target_position.current_radar_track_mode] ||
        target_track->target_track_points_list.last().range > RANGE_END[tracking_target_position.current_radar_track_mode])
    {
        needing_update_flag = true;
        qDebug() << "【跟踪模式】: 距离模式需要更新配置!!!";
    }
    return needing_update_flag;
}

void initializeTrackingTargetPosition(TrackingTargetPosition& tracking_target_position)
{
    tracking_target_position.range = 0;
    tracking_target_position.current_radar_track_mode = NEAR_TRACK_MODE_INDEX;
    tracking_target_position.last_radar_track_mode = 0;
    tracking_target_position.last_but_one_track_point_azim_deg = 0;
    tracking_target_position.last_track_point_azim_deg = 0;
    tracking_target_position.current_pointing_azim_deg = 0;
    tracking_target_position.azim_deg_deviate_times = 0;
    tracking_target_position.last_track_point_elev_index = 0;
    tracking_target_position.last_but_one_track_point_elev_index = 0;
    tracking_target_position.elev_index_deviate_times = 0;
    tracking_target_position.elev_index_list[0] = 6;
    tracking_target_position.elev_index_list[1] = 7;
    tracking_target_position.elev_index_list[2] = 8;
    tracking_target_position.elev_index_list[3] = 9;
    tracking_target_position.hardware_settings_needed_to_update_flag = false;
}

void updateHardwareSettingsTrackModeBasedOnTrackingTargetPosition(const TrackingTargetPosition& new_tracking_target_position,
                                                                  HardwareSettingsTrackMode& hardware_settings_track_mode,
                                                                  const DataProcessingSettings& m_data_processing_settings)
{
    hardware_settings_track_mode.next_radar_mode = new_tracking_target_position.current_radar_track_mode;
    hardware_settings_track_mode.next_pointing_azim_deg =
        circularlyAddDouble(new_tracking_target_position.current_pointing_azim_deg, -m_data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle, 0, 360);
    memcpy(hardware_settings_track_mode.next_elev_index_list, new_tracking_target_position.elev_index_list, sizeof(new_tracking_target_position.elev_index_list));
}

void updateHardwareSettingsWhenQuitTrackModeBasedOnTrackingTargetPosition(const TrackingTargetPosition& new_tracking_target_position,
                                                                          HardwareSettingsTrackMode& hardware_settings_track_mode)
{
    if (new_tracking_target_position.range < RANGE_DIVISION_SHORT_LONG1)
    {
        hardware_settings_track_mode.next_radar_mode = NEAR_SCAN_MODE_INDEX;
    }
    else
    {
        hardware_settings_track_mode.next_radar_mode = FAR_SCAN_MODE_INDEX;
    }
    memset(hardware_settings_track_mode.next_elev_index_list, 0x00, sizeof(hardware_settings_track_mode.next_elev_index_list));
}
