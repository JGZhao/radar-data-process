#include <QDebug>
#include <QDir>
#include <complex>
#include "global_functions.h"
#include "radar_data_processing_thread.h"

RadarDataProcessingObject::RadarDataProcessingObject(QObject* parent) : QObject(parent)
{
    qDebug() << tr("[RadarDataProcessingObject]: 进入构造函数!");
    initializeHardwareSettings(local_hardware_settings);
    initializeSoftwareSettings(local_software_settings);
    initializeDataProcessingSettings(local_data_processing_settings);

    // 方位毛刺平滑相关
    initializeParametersForSmoothingAzimScanMode();

    // 系统状态
    current_input_azim_sector_index = AZIM_SECTOR_NUM - 1;
    last_azim_input_sector_index = AZIM_SECTOR_NUM - 1;
    initializeRadarSystemState(local_radar_system_state);
    update_radar_system_state_in_other_thread_timer = new QTimer(this);
    update_radar_system_state_in_other_thread_timer->setInterval(UPDATE_RADAR_SYSTEM_STATE_TIMER_INTERVAL);
    need_to_update_radar_system_state_in_other_thread_flag = false;
    update_radar_system_state_timer_count = 0;
    serdes_error_flag = false;
    none_dbf_coef_in_fpga_flag = false;

    // 静默控制
    enter_power_off_area_flag = false;

    // 点迹及航迹处理
    can_add_original_point_to_input_azim_sector_flag = false;
    original_points_and_tracks_scan_mode_cleared_up_flag = false;
    original_points_and_tracks_track_mode_cleared_up_flag = false;

    // 动态杂波图
    memset(clutter_map, 0x00, sizeof(clutter_map));

    // 增删点航迹
    added_original_point_item_list_scan_mode.clear();
    for (int ii = 0; ii < AZIM_SECTOR_NUM; ++ii)
    {
        QList<OriginalPointItem*> original_point_item_list;
        added_original_point_item_list_scan_mode.append(original_point_item_list);
        added_original_point_item_list_scan_mode[ii].clear();
    }
    adding_original_point_item_list_track_mode = new QList<OriginalPointItem*>;
    adding_original_point_item_list_track_mode->clear();
    added_original_point_item_list_track_mode = new QList<OriginalPointItem*>;
    added_original_point_item_list_track_mode->clear();
    target_track_to_item_hashmap.clear();

    g_original_point_item_to_add_list.clear();
    g_original_point_item_to_delete_list.clear();
    g_target_track_item_to_add_list.clear();
    g_target_track_item_to_update_list.clear();
    g_target_track_item_to_delete_list.clear();

    // 发送航迹
    sending_target_set.clear();

    // 跟踪模式
    current_packet_track_mode = nullptr;
    original_points_batch_track_mode_ready_flag = false;
    continuous_packet_num_track_mode = 0;
    tracking_target_track = nullptr;
    tracking_target_track_id = -1;
    prepare_tracking_flag = false;
    start_tracking_flag = false;
    prepare_to_quit_tracking_flag = false;
    initializeHardwareSettingsTrackMode(local_hardware_settings_track_mode);

    // 输出各种信息到文件
    radar_system_info_txt = nullptr;
    is_radar_system_info_txt_opened = false;
    radar_system_info_text_stream = nullptr;
    createNewRadarSystemInfoFile();
    current_radar_system_info_row_num = 0;

    radar_original_points_txt = nullptr;
    is_radar_original_points_txt_opened = false;
    radar_original_points_text_stream = nullptr;
    createNewRadarOriginalPointsFile();
    current_radar_original_points_row_num = 0;

    radar_track_points_txt = nullptr;
    is_radar_track_points_txt_opened = false;
    radar_track_points_text_stream = nullptr;
    createNewRadarTrackPointsFile();
    current_radar_track_points_row_num = 0;

    connect(update_radar_system_state_in_other_thread_timer, &QTimer::timeout, this, &RadarDataProcessingObject::sltUpdateRadarSystemStateInOtherThreads);
    update_radar_system_state_in_other_thread_timer->start();
}

RadarDataProcessingObject::~RadarDataProcessingObject()
{
    qDebug() << tr("[RadarDataProcessingObject]: 进入析构函数!");
    QMutexLocker locker(&g_mutex_for_g_original_point_item_list_and_g_target_track_item_list);

    if (current_packet)
    {
        delete current_packet;
        current_packet = nullptr;
    }
    if (next_packet)
    {
        delete next_packet;
        next_packet = nullptr;
    }
    if (current_packet_track_mode)
    {
        delete current_packet_track_mode;
        current_packet_track_mode = nullptr;
    }

    //  删除没有发送的航迹列表
    if (track_to_send_list)
    {
        track_to_send_list->clear();
        delete track_to_send_list;
        track_to_send_list = nullptr;
    }

    // 删除搜索模式的原始点迹
    while (added_original_point_item_list_scan_mode[current_input_azim_sector_index].size() > 0)
    {
        // 当前输入扇区的原始点迹尚未加入主界面，需要delete
        delete (added_original_point_item_list_scan_mode[current_input_azim_sector_index].takeFirst());
    }

    for (int ii = 0; ii < AZIM_SECTOR_NUM; ++ii)
    {
        // 已经添加到主界面的原始点迹，需要由主界面删除
        if (added_original_point_item_list_scan_mode[ii].size() > 0)
        {
            g_original_point_item_to_delete_list.append(added_original_point_item_list_scan_mode[ii]);
            added_original_point_item_list_scan_mode[ii].clear();
        }
    }

    // 删除跟踪模式的原始点迹
    // 如果跟踪模式当前原始点迹批已经准备完毕，表示原始点迹已经添加至主界面，需要由主界面删除这些点迹
    if (added_original_point_item_list_track_mode->size() > 0)
    {
        g_original_point_item_to_delete_list.append(*added_original_point_item_list_track_mode);
    }
    delete added_original_point_item_list_track_mode;
    added_original_point_item_list_track_mode = nullptr;

    // 如果跟踪模式当前原始点迹批尚未准备完毕，表示原始点迹尚未添加至主界面，需要delete
    while (adding_original_point_item_list_track_mode->size() > 0)
    {
        delete (adding_original_point_item_list_track_mode->takeFirst());
    }
    delete adding_original_point_item_list_track_mode;
    adding_original_point_item_list_track_mode = nullptr;

    if (g_original_point_item_to_delete_list.size() > 0)
    {
        emit sigUpdateOriginalPointItems();
    }

    if (radar_system_info_text_stream)
    {
        radar_system_info_text_stream->flush();
        radar_system_info_text_stream->reset();
        delete radar_system_info_text_stream;
        radar_system_info_text_stream = nullptr;
    }
    if (radar_system_info_txt)
    {
        radar_system_info_txt->close();
        delete radar_system_info_txt;
        radar_system_info_txt = nullptr;
    }

    if (radar_original_points_text_stream)
    {
        radar_original_points_text_stream->flush();
        radar_original_points_text_stream->reset();
        delete radar_original_points_text_stream;
        radar_original_points_text_stream = nullptr;
    }
    if (radar_original_points_txt)
    {
        radar_original_points_txt->close();
        delete radar_original_points_txt;
        radar_original_points_txt = nullptr;
    }

    if (radar_track_points_text_stream)
    {
        radar_track_points_text_stream->flush();
        radar_track_points_text_stream->reset();
        delete radar_track_points_text_stream;
        radar_track_points_text_stream = nullptr;
    }
    if (radar_track_points_txt)
    {
        radar_track_points_txt->close();
        delete radar_track_points_txt;
        radar_track_points_txt = nullptr;
    }
}

void RadarDataProcessingObject::initializeDataProcessingAzimSectorIndexScamMode()
{
    azim_sector_list_scan_mode.initializeDataProcessingAzimSectorIndexScamMode();
}

void RadarDataProcessingObject::initializeParametersForSmoothingAzimScanMode()
{
    current_packet = nullptr;
    next_packet = nullptr;
    last_packet_azim_dec = 0xffff;
    current_packet_azim_dec = 0xffff;
    next_packet_azim_dec = 0xffff;
    delta_last_current_azim_dec = 1;
    delta_current_next_azim_dec = 0;
}

void RadarDataProcessingObject::smoothAzimInCurrentFpgaPacket()
{
    if (current_packet)
    {
        last_packet_azim_dec = current_packet_azim_dec;
        current_packet_azim_dec = next_packet_azim_dec;
        next_packet_azim_dec = ((uint16_t)next_packet->azim_angle[0] << 8) | next_packet->azim_angle[1];
        delta_last_current_azim_dec = delta_current_next_azim_dec;
        delta_current_next_azim_dec = current_packet_azim_dec - next_packet_azim_dec;
        // 如果有毛刺, delta_azim12 * delta_azim23 < 0, 当前角为前一个角与后一个角的平均值
        if (delta_last_current_azim_dec * delta_current_next_azim_dec < 0)
        {
            // 下毛刺
            if (delta_last_current_azim_dec > 0 && delta_current_next_azim_dec < 0)
            {
                if (abs(delta_last_current_azim_dec) > abs(delta_current_next_azim_dec))
                {
                    qDebug() << QString("有毛刺!") << "last" << last_packet_azim_dec << "current" << current_packet_azim_dec << "next" << next_packet_azim_dec;
                    int smoothed_packet_azim_dec = (last_packet_azim_dec + next_packet_azim_dec) >> 1;
                    current_packet->azim_angle[0] = (smoothed_packet_azim_dec >> 8) & 0xff;
                    current_packet->azim_angle[1] = smoothed_packet_azim_dec & 0xff;
                    current_packet_azim_dec = smoothed_packet_azim_dec;
                    delta_current_next_azim_dec = current_packet_azim_dec - next_packet_azim_dec;
                    qDebug() << QString("校正毛刺!") << "last" << last_packet_azim_dec << "current" << current_packet_azim_dec << "next" << next_packet_azim_dec;
                }
            }
            // 上毛刺
            else if (delta_last_current_azim_dec < 0 && delta_current_next_azim_dec > 0)
            {
                if (abs(delta_last_current_azim_dec) < abs(delta_current_next_azim_dec))
                {
                    qDebug() << QString("有毛刺!") << "last" << last_packet_azim_dec << "current" << current_packet_azim_dec << "next" << next_packet_azim_dec;
                    int smoothed_packet_azim_dec = (last_packet_azim_dec + next_packet_azim_dec) >> 1;
                    current_packet->azim_angle[0] = (smoothed_packet_azim_dec >> 8) & 0xff;
                    current_packet->azim_angle[1] = smoothed_packet_azim_dec & 0xff;
                    current_packet_azim_dec = smoothed_packet_azim_dec;
                    delta_current_next_azim_dec = current_packet_azim_dec - next_packet_azim_dec;
                    qDebug() << QString("校正毛刺!") << "last" << last_packet_azim_dec << "current" << current_packet_azim_dec << "next" << next_packet_azim_dec;
                }
            }
        }
    }
    else
    {
        last_packet_azim_dec = current_packet_azim_dec;
        current_packet_azim_dec = next_packet_azim_dec;
        next_packet_azim_dec = ((uint16_t)next_packet->azim_angle[0] << 8) | next_packet->azim_angle[1];
    }
}

bool RadarDataProcessingObject::packetHandlerScanMode(const FpgaPacket* fpga_packet)
{
    QDateTime date_time = QDateTime::currentDateTime();
    updateRadarSystemState(fpga_packet, local_radar_system_state);
    if (local_radar_system_state.current_radar_mode_index >= RADAR_SCAN_TRACK_MODE_DIVISION_INDEX)
    {
        //        qDebug() << QString("[Warning]: scan mode, ignore packets track mode!");
        return false;
    }

    // 静默控制
    if (local_hardware_settings.power_off_area_settings.enable_power_off_area_flag)
    {
        if (isBetweenFloorAndCeilDouble(local_radar_system_state.biss_angle_deg,
                                        local_hardware_settings.power_off_area_settings.power_off_azim_deg_floor,
                                        local_hardware_settings.power_off_area_settings.power_off_azim_deg_ceil,
                                        0,
                                        360))
        {
            if (!enter_power_off_area_flag)
            {
                enter_power_off_area_flag = true;
                emit sigSendPowerOffOrderToFpga();
            }
        }
        else
        {
            if (enter_power_off_area_flag)
            {
                enter_power_off_area_flag = false;
                emit sigSendPowerOnOrderToFpga();
            }
        }
    }

    if (prepare_tracking_flag)
    {
        // 统一控制旋转一圈后停止到目标位置附近
        double delta_azim_deg_current_azim_to_tracking_target =
            circularlyAddDouble(local_radar_system_state.biss_angle_deg, -last_tracking_target_position.current_pointing_azim_deg, 0, 360);
        if (fabs(delta_azim_deg_current_azim_to_tracking_target) < AZIM_DEG_RESERVED_STOP_TO_TRACK_MODE)
        {
            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << QString("start开始跟踪!");
            emit sigSendStopPointingAndTrackOrderToEnterTrackMode(local_hardware_settings_track_mode);
            prepare_tracking_flag = false;
            start_tracking_flag = true;
            tracking_target_track->distance_to_point_index_map.clear(); // 清空可以关联上的原始点队列
            tracking_target_track->continuous_lost_times = 0;           // 进入跟踪模式时，丢失次数重新置零
            emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + QString(" Start tracking %1!").arg(tracking_target_track_id));

            return true;
        }
    }

    // 根据发射角度确认在哪个扇区
    double transmit_azim_deg_beam_center = local_radar_system_state.current_transmit_azim_center;
    int azim_index = (int(transmit_azim_deg_beam_center / AZIM_SECTOR_BEAM_WIDTH)) % AZIM_SECTOR_NUM;

    current_input_azim_sector_index = azim_index;
    azim_sector_list_scan_mode.scan_current_input_index = current_input_azim_sector_index;

    can_add_original_point_to_input_azim_sector_flag = (!isBetweenFloorAndCeilInt(azim_sector_list_scan_mode.scan_current_input_index,
                                                                                  azim_sector_list_scan_mode.scan_current_combine_index,
                                                                                  azim_sector_list_scan_mode.scan_current_track_start_index,
                                                                                  0,
                                                                                  AZIM_SECTOR_NUM - 1));

    // 如果当前扇区与上一个扇区不一样
    if (current_input_azim_sector_index != last_azim_input_sector_index)
    {
        // 更新上一个扇区的杂波图
        updateClutterMap(last_azim_input_sector_index);

        // 添加上一个扇区的原始点 original_item_list (原始点迹的发送也以扇区为单位)
        if (added_original_point_item_list_scan_mode[last_azim_input_sector_index].size() > 0)
        {
            g_original_point_item_to_add_list.append(added_original_point_item_list_scan_mode[last_azim_input_sector_index]);
        }

        // 删除上一个扇区到当前扇区内的原始点 original_item_list （上一个扇区不删, 当前扇区删]
        last_azim_input_sector_index = circularlyAddInt(last_azim_input_sector_index, -1, 0, AZIM_SECTOR_NUM);
        while (last_azim_input_sector_index != current_input_azim_sector_index)
        {
            // 更新上一个扇区到当前扇区的杂波图
            updateClutterMap(last_azim_input_sector_index);

            if (!isBetweenFloorAndCeilInt(last_azim_input_sector_index,
                                          azim_sector_list_scan_mode.scan_current_combine_index,
                                          azim_sector_list_scan_mode.scan_current_track_start_index,
                                          0,
                                          AZIM_SECTOR_NUM - 1))
            {
                if (added_original_point_item_list_scan_mode[last_azim_input_sector_index].size() > 0)
                {
                    g_original_point_item_to_delete_list.append(added_original_point_item_list_scan_mode[last_azim_input_sector_index]);
                    added_original_point_item_list_scan_mode[last_azim_input_sector_index].clear();
                }
            }
            last_azim_input_sector_index = circularlyAddInt(last_azim_input_sector_index, -1, 0, AZIM_SECTOR_NUM);
        }

        if (can_add_original_point_to_input_azim_sector_flag)
        {
            // 当俯仰已经到下一轮了，则更新当前扇区的自由时间以及雷达模式
            if (local_radar_system_state.next_elev_round_flag)
            {
                azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_input_index].current_free_time = local_radar_system_state.cumulative_free_time;
                azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_input_index].last_radar_mission_index =
                    azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_input_index].current_radar_mission_index;
                azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_input_index].current_radar_mission_index =
                    local_radar_system_state.current_radar_mission_mode_index;
            }

            // 删除当前扇区的原始点迹 original_item_list
            if (added_original_point_item_list_scan_mode[current_input_azim_sector_index].size() > 0)
            {
                g_original_point_item_to_delete_list.append(added_original_point_item_list_scan_mode[current_input_azim_sector_index]);
                added_original_point_item_list_scan_mode[current_input_azim_sector_index].clear();
            }

            // 提取目标信息
            for (int ii = 0; ii < local_radar_system_state.target_num; ++ii)
            {
                extractTargetInfoFromFpgaPacketScanMode(fpga_packet->target_info[ii],
                                                        date_time,
                                                        current_input_azim_sector_index,
                                                        local_radar_system_state.current_radar_mode_index,
                                                        local_radar_system_state.cumulative_free_time,
                                                        transmit_azim_deg_beam_center,
                                                        azim_index,
                                                        local_radar_system_state.current_elev_deg,
                                                        local_radar_system_state.current_elev_index,
                                                        local_radar_system_state.receiver_weight_set);
            }
        }

        if (g_original_point_item_to_add_list.size() > 0 || g_original_point_item_to_delete_list.size() > 0)
        {
            emit sigUpdateOriginalPointItems();
        }
    }
    // 如果当前扇区与上一个扇区一样, 则提取目标并新建 original_point_item 加入 本扇区的 original_item_list
    else
    {
        if (can_add_original_point_to_input_azim_sector_flag)
        {
            // 提取目标信息
            for (int ii = 0; ii < local_radar_system_state.target_num; ++ii)
            {
                extractTargetInfoFromFpgaPacketScanMode(fpga_packet->target_info[ii],
                                                        date_time,
                                                        current_input_azim_sector_index,
                                                        local_radar_system_state.current_radar_mode_index,
                                                        local_radar_system_state.cumulative_free_time,
                                                        transmit_azim_deg_beam_center,
                                                        azim_index,
                                                        local_radar_system_state.current_elev_deg,
                                                        local_radar_system_state.current_elev_index,
                                                        local_radar_system_state.receiver_weight_set);
            }
        }
    }

    return true;
}

void RadarDataProcessingObject::updateClutterMapBasedOnNewOriginalPoint(const TargetOriginalPoint& original_point)
{
    clutter_map[original_point.radar_mode][original_point.range_index][original_point.azim_index][original_point.elev_index] +=
        local_data_processing_settings.clutter_map_settings.clutter_map_generate_weight * original_point.amp_sigma;

    // 同时更新相邻距离门的能量
    if (original_point.range_index > 0)
    {
        clutter_map[original_point.radar_mode][original_point.range_index - 1][original_point.azim_index][original_point.elev_index] +=
            local_data_processing_settings.clutter_map_settings.clutter_map_generate_weight * original_point.amp_sigma;
    }
    if (original_point.range_index < MAX_RANGE_UNIT_NUM - 1)
    {
        clutter_map[original_point.radar_mode][original_point.range_index + 1][original_point.azim_index][original_point.elev_index] +=
            local_data_processing_settings.clutter_map_settings.clutter_map_generate_weight * original_point.amp_sigma;
    }
}

bool RadarDataProcessingObject::cluttermapFilter(TargetOriginalPoint& original_point)
{
    if (original_point.amp_sigma < local_data_processing_settings.clutter_map_settings.clutter_map_filter_threshold *
                                       clutter_map[original_point.radar_mode][original_point.range_index][original_point.azim_index][original_point.elev_index])
    {
        original_point.is_filtered = true;
        return true;
    }
    return false;
}

void RadarDataProcessingObject::updateClutterMap(const int& m_azim_sector_index)
{
    // 以扇区为单位更新杂波图
    double iteration_weight = 1 - local_data_processing_settings.clutter_map_settings.clutter_map_generate_weight;
    for (int ii = 0; ii < MAX_RADAR_MODE_NUM; ++ii)
    {
        for (int jj = 0; jj < MAX_RANGE_UNIT_NUM; ++jj)
        {
            for (int kk = 0; kk < MAX_ELEV_UNIT_NUM; ++kk)
            {
                clutter_map[ii][jj][m_azim_sector_index][kk] *= iteration_weight;
            }
        }
    }
}

TargetOriginalPoint RadarDataProcessingObject::extractTargetInfoFromFpgaPacket(const TargetInfo& target_info,
                                                                               const QDateTime& date_time,
                                                                               const int& radar_mode,
                                                                               const double& free_time,
                                                                               const double& beamcenter_azim_deg,
                                                                               const int& azim_index,
                                                                               const double& breamcenter_elev_deg,
                                                                               const int& elev_index,
                                                                               const int& dbf_weight_flag)
{
    using namespace std;
    int range_index = 0;
    int doppler_index = 0;
    double range = 0;
    double azim_deg = beamcenter_azim_deg;
    double elev_deg = breamcenter_elev_deg;
    double vr = 0;
    double x = 0;
    double y = 0;
    double z = 0;
    double vx = 0;
    double vy = 0;
    double vz = 0;
    int amp_sigma = 0;
    int amp_delta_azim = 0;
    int amp_delta_elev1 = 0;
    int amp_delta_elev2 = 0;
    int sigma_I = 0;
    int sigma_Q = 0;
    int delta_azim_I = 0;
    int delta_azim_Q = 0;
    int delta_elev1_I = 0;
    int delta_elev1_Q = 0;
    int delta_elev2_I = 0;
    int delta_elev2_Q = 0;
    int snr = 0;
    int cfar_neighbour_num = 0;
    double rcs = 0;
    bool is_filtered = false;
    bool is_combined = false;
    bool is_correlated = false;
    double best_correlate_distance = 1e9;

    range_index = ((uint16_t)target_info.range_index[0] << 8) | target_info.range_index[1];
    doppler_index = (uint16_t)(((target_info.amp_exp_doppler_high & 0x03) << 8) | target_info.doppler_low_index);

    double range_unit = local_data_processing_settings.radar_wave_paremeters[radar_mode].range_unit;
    double velocity_unit = local_data_processing_settings.radar_wave_paremeters[radar_mode].doppler_unit;
    double blind_area = local_data_processing_settings.radar_wave_paremeters[radar_mode].blind_area;
    int mtd_num = local_data_processing_settings.radar_wave_paremeters[radar_mode].mtd_num;
    double rcs_correction_coef = local_data_processing_settings.radar_wave_paremeters[radar_mode].rcs_correction_coef;
    double decay0 = pow(10, -local_data_processing_settings.radar_wave_paremeters[radar_mode].decay0 / 10.0);
    double range_division_01 = local_data_processing_settings.radar_wave_paremeters[radar_mode].range_division_01;
    double range_division_12 = local_data_processing_settings.radar_wave_paremeters[radar_mode].range_division_12;
    double decay2 = pow(10, -local_data_processing_settings.radar_wave_paremeters[radar_mode].decay2 / 10.0);

    // 转换距离
    range = range_index * range_unit + blind_area;
    if (range > RANGE_END[radar_mode])
    {
        is_filtered = true;
    }

    // 转换速度
    if (doppler_index < mtd_num / 2)
    {
        vr = doppler_index * velocity_unit;
    }
    else
    {
        vr = (doppler_index - mtd_num) * velocity_unit;
    }

    // 转换能量
    int amp_sigma_exp = ((target_info.amp_exp_doppler_high & 0xf8) >> 3) & 0x1f;
    //    amp_sigma = ((uint16_t)target_info.amp_sigma[0] << 8) | target_info.amp_sigma[1];
    //    amp_sigma = amp_sigma << (amp_sigma_exp);

    int amp_delta_azim_exp = target_info.amp_delta_azim[1] & 0x1f;
    //    amp_delta_azim = ((uint16_t)(target_info.amp_delta_azim[0] & 0x7f) << 3) | (target_info.amp_delta_azim[1] >> 5);
    //    amp_delta_azim = amp_delta_azim << (amp_delta_azim_exp + 6);

    int amp_delta_elev_exp1 = target_info.amp_delta_elev1[1] & 0x1f;
    //    amp_delta_elev1 = ((uint16_t)(target_info.amp_delta_elev1[0] & 0x7f) << 3) | (target_info.amp_delta_elev1[1] >> 5);
    //    amp_delta_elev1 = amp_delta_elev1 << (amp_delta_elev_exp1 + 6);

    int amp_delta_elev_exp2 = target_info.amp_delta_elev2[1] & 0x1f;
    //    amp_delta_elev2 = ((uint16_t)(target_info.amp_delta_elev2[0] & 0x7f) << 3) | (target_info.amp_delta_elev2[1] >> 5);
    //    amp_delta_elev2 = amp_delta_elev2 << (amp_delta_elev_exp2 + 6);

    // 消副瓣
    //    if (local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag)
    //    {
    //        if (amp_delta_azim > amp_sigma * local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio)
    //        {
    //            is_filtered = true;
    //        }
    //    }

    //    if (local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag)
    //    {
    //        if (amp_delta_elev1 > amp_sigma * local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio ||
    //            amp_delta_elev2 > amp_sigma * local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio)
    //        {
    //            is_filtered = true;
    //        }
    //    }

    // 提取各通道 IQ, IQ 需要左移对应能量的指数
    uint16_t mid_val = ((uint16_t)target_info.sigma_I[0] << 8) | target_info.sigma_I[1];
    sigma_I = int16_t(mid_val);
    sigma_I <<= amp_sigma_exp;
    mid_val = ((uint16_t)target_info.sigma_Q[0] << 8) | target_info.sigma_Q[1];
    sigma_Q = int16_t(mid_val);
    sigma_Q <<= amp_sigma_exp;
    mid_val = ((uint16_t)target_info.delta_azim_I[0] << 8) | target_info.delta_azim_I[1];
    delta_azim_I = int16_t(mid_val);
    delta_azim_I <<= amp_delta_azim_exp;
    mid_val = ((uint16_t)target_info.delta_azim_Q[0] << 8) | target_info.delta_azim_Q[1];
    delta_azim_Q = int16_t(mid_val);
    delta_azim_Q <<= amp_delta_azim_exp;
    mid_val = ((uint16_t)target_info.delta_elev1_I[0] << 8) | target_info.delta_elev1_I[1];
    delta_elev1_I = int16_t(mid_val);
    delta_elev1_I <<= amp_delta_elev_exp1;
    mid_val = ((uint16_t)target_info.delta_elev1_Q[0] << 8) | target_info.delta_elev1_Q[1];
    delta_elev1_Q = int16_t(mid_val);
    delta_elev1_Q <<= amp_delta_elev_exp1;
    mid_val = ((uint16_t)target_info.delta_elev2_I[0] << 8) | target_info.delta_elev2_I[1];
    delta_elev2_I = int16_t(mid_val);
    delta_elev2_I <<= amp_delta_elev_exp2;
    mid_val = ((uint16_t)target_info.delta_elev2_Q[0] << 8) | target_info.delta_elev2_Q[1];
    delta_elev2_Q = int16_t(mid_val);
    delta_elev2_Q <<= amp_delta_elev_exp2;

    complex<double> sigma_complex(sigma_I, sigma_Q);
    complex<double> delta_azim_complex(delta_azim_I, delta_azim_Q);
    complex<double> delta_elev1_complex(delta_elev1_I, delta_elev1_Q);
    complex<double> delta_elev2_complex(delta_elev2_I, delta_elev2_Q);

    double sigma_modulus = abs(sigma_complex);
    double delta_azim_modulus = abs(delta_azim_complex);
    double delta_elev1_modulus = abs(delta_elev1_complex);
    double delta_elev2_modulus = abs(delta_elev2_complex);

    amp_sigma = int(sigma_modulus);
    amp_delta_azim = int(delta_azim_modulus);
    amp_delta_elev1 = int(delta_elev1_modulus);
    amp_delta_elev2 = int(delta_elev2_modulus);

    // 方位和差
    // 通过和差过滤杂波
    if (local_data_processing_settings.sigma_delta_parameters[0].sigma_delta_filter_flag)
    {
        if (delta_azim_modulus > sigma_modulus * local_data_processing_settings.sigma_delta_parameters[0].sigma_delta_filter_ratio)
        {
            is_filtered = true;
        }
    }

    if (local_data_processing_settings.sigma_delta_parameters[0].sigma_delta_flag)
    {
        double azim_bias_angle_sd = 0;
        if (sigma_I != 0 && sigma_Q != 0)
        {
            complex<double> sigma_delta_azim_complex = delta_azim_complex / sigma_complex;
            double sigma_delta_azim_modulus = abs(sigma_delta_azim_complex);
            double sigma_delta_azim_phase = arg(sigma_delta_azim_complex) * RAD2DEG;
            azim_bias_angle_sd = sigma_delta_azim_modulus * local_data_processing_settings.sigma_delta_parameters[0].sigma_delta_slope;
            if (azim_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[0].max_bias_angle)
            {
                azim_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[0].max_bias_angle;
            }
            if (isBetweenFloorAndCeilDegPlusMinus180(sigma_delta_azim_phase,
                                                     local_data_processing_settings.sigma_delta_parameters[0].negative_bias_phase_floor,
                                                     local_data_processing_settings.sigma_delta_parameters[0].negative_bias_phase_ceil))
            {
                azim_bias_angle_sd = -azim_bias_angle_sd;
            }
        }
        azim_deg = circularlyAddDouble(azim_deg, azim_bias_angle_sd, 0.0, 360.0);
    }

    // 俯仰和差
    if (dbf_weight_flag)
    {
        // 通过和差过滤杂波
        if (local_data_processing_settings.sigma_delta_parameters[2].sigma_delta_filter_flag)
        {
            if (delta_elev1_modulus > sigma_modulus * local_data_processing_settings.sigma_delta_parameters[2].sigma_delta_filter_ratio ||
                delta_elev2_modulus > sigma_modulus * local_data_processing_settings.sigma_delta_parameters[2].sigma_delta_filter_ratio)
            {
                is_filtered = true;
            }
        }
        if (local_data_processing_settings.sigma_delta_parameters[2].sigma_delta_flag)
        {
            double elev_bias_angle_sd = 0;
            double sigma_delta_elev_slope = 0;
            if (elev_index < MAX_VALID_ELEV_BEAM_INDEX)
            {
                if (sigma_I != 0 && sigma_Q != 0)
                {
                    // 方法1：DBF 相邻和差
                    if (delta_elev1_modulus > delta_elev2_modulus)
                    {
                        // 需要判定目标实际在哪个相邻合成的和差波束中
                        int sigma_delta_elev_beam_index = elev_index - 1;
                        if (sigma_delta_elev_beam_index < 0)
                        {
                            sigma_delta_elev_beam_index = 0;
                        }
                        // 根据是否加权选用对应波束的斜率
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_elev_slope[1].sigma_delta_slope[sigma_delta_elev_beam_index];

                        double elev_high_plus_lower_amp = delta_elev1_modulus + sigma_modulus;
                        double elev_high_minus_lower_amp = delta_elev1_modulus - sigma_modulus;
                        if (fabs(elev_high_plus_lower_amp) > EPS)
                        {
                            double sigma_delta_elev_high_lower_modulus = elev_high_minus_lower_amp / elev_high_plus_lower_amp;
                            elev_bias_angle_sd = fabs(sigma_delta_elev_high_lower_modulus) * sigma_delta_elev_slope;

                            if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                            }

                            if (sigma_modulus >= delta_elev1_modulus)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0 - elev_bias_angle_sd;
                            }
                            else
                            {
                                elev_bias_angle_sd += local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                            }
                        }
                    }
                    else
                    {
                        int sigma_delta_elev_beam_index = elev_index;
                        if (sigma_delta_elev_beam_index < 0)
                        {
                            sigma_delta_elev_beam_index = 0;
                        }
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_elev_slope[1].sigma_delta_slope[sigma_delta_elev_beam_index];

                        double elev_high_plus_lower_amp = sigma_modulus + delta_elev2_modulus;
                        double elev_high_minus_lower_amp = sigma_modulus - delta_elev2_modulus;
                        if (fabs(elev_high_plus_lower_amp) > EPS)
                        {
                            double sigma_delta_elev_high_lower_modulus = elev_high_minus_lower_amp / elev_high_plus_lower_amp;
                            elev_bias_angle_sd = fabs(sigma_delta_elev_high_lower_modulus) * sigma_delta_elev_slope;
                            if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                            }

                            if (sigma_modulus >= delta_elev2_modulus)
                            {
                                elev_bias_angle_sd = elev_bias_angle_sd - local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                            }
                            else
                            {
                                elev_bias_angle_sd = -elev_bias_angle_sd - local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                            }
                        }
                    }

                    elev_deg += elev_bias_angle_sd;
                }
            }
            else if (elev_index == MAX_VALID_ELEV_BEAM_INDEX)
            {
                if (sigma_I != 0 && sigma_Q != 0)
                {
                    // 如果第7波束幅度大于第8波束幅度, 表示目标可能在第7波束, 则按照方法1做和差
                    if (sigma_modulus < delta_elev1_modulus)
                    {
                        int sigma_delta_elev_beam_index = elev_index - 1;
                        if (sigma_delta_elev_beam_index < 0)
                        {
                            sigma_delta_elev_beam_index = 0;
                        }
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_elev_slope[1].sigma_delta_slope[sigma_delta_elev_beam_index];

                        double elev_high_plus_lower_amp = delta_elev1_modulus + sigma_modulus;
                        double elev_high_minus_lower_amp = delta_elev1_modulus - sigma_modulus;
                        if (fabs(elev_high_plus_lower_amp) > EPS)
                        {
                            double sigma_delta_elev_high_lower_modulus = elev_high_minus_lower_amp / elev_high_plus_lower_amp;
                            elev_bias_angle_sd = fabs(sigma_delta_elev_high_lower_modulus) * sigma_delta_elev_slope;

                            if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                            }
                            elev_bias_angle_sd += local_data_processing_settings.sigma_delta_parameters[2].beam_width / 2.0;
                        }
                        elev_deg += elev_bias_angle_sd;
                    }
                    else
                    {
                        // 方法2：正常和差
                        complex<double> sigma_delta_elev2_complex = delta_elev2_complex / sigma_complex;
                        double sigma_delta_elev2_modulus = abs(sigma_delta_elev2_complex);
                        double sigma_delta_elev2_phase = arg(sigma_delta_elev2_complex) * RAD2DEG;
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_parameters[2].sigma_delta_slope;
                        elev_bias_angle_sd = sigma_delta_elev2_modulus * sigma_delta_elev_slope;
                        if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[2].max_bias_angle)
                        {
                            elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[2].max_bias_angle;
                        }
                        if (isBetweenFloorAndCeilDegPlusMinus180(sigma_delta_elev2_phase,
                                                                 local_data_processing_settings.sigma_delta_parameters[2].negative_bias_phase_floor,
                                                                 local_data_processing_settings.sigma_delta_parameters[2].negative_bias_phase_ceil))
                        {
                            elev_bias_angle_sd = -elev_bias_angle_sd;
                        }
                        elev_deg += elev_bias_angle_sd;
                    }
                }
            }
        }
    }
    else
    {
        // 通过和差过滤杂波
        if (local_data_processing_settings.sigma_delta_parameters[1].sigma_delta_filter_flag)
        {
            if (delta_elev1_modulus > sigma_modulus * local_data_processing_settings.sigma_delta_parameters[1].sigma_delta_filter_ratio ||
                delta_elev2_modulus > sigma_modulus * local_data_processing_settings.sigma_delta_parameters[1].sigma_delta_filter_ratio)
            {
                is_filtered = true;
            }
        }
        if (local_data_processing_settings.sigma_delta_parameters[1].sigma_delta_flag)
        {
            double elev_bias_angle_sd = 0;
            double sigma_delta_elev_slope = 0;
            if (elev_index < MAX_VALID_ELEV_BEAM_INDEX)
            {
                if (sigma_I != 0 && sigma_Q != 0)
                {
                    // 方法1：DBF 相邻和差
                    if (delta_elev1_modulus > delta_elev2_modulus)
                    {
                        // 需要判定目标实际在哪个相邻合成的和差波束中
                        int sigma_delta_elev_beam_index = elev_index - 1;
                        if (sigma_delta_elev_beam_index < 0)
                        {
                            sigma_delta_elev_beam_index = 0;
                        }
                        // 根据是否加权选用对应波束的斜率
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_elev_slope[0].sigma_delta_slope[sigma_delta_elev_beam_index];

                        double elev_high_plus_lower_amp = delta_elev1_modulus + sigma_modulus;
                        double elev_high_minus_lower_amp = delta_elev1_modulus - sigma_modulus;
                        if (fabs(elev_high_plus_lower_amp) > EPS)
                        {
                            double sigma_delta_elev_high_lower_modulus = elev_high_minus_lower_amp / elev_high_plus_lower_amp;
                            elev_bias_angle_sd = fabs(sigma_delta_elev_high_lower_modulus) * sigma_delta_elev_slope;

                            if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                            }

                            if (sigma_modulus >= delta_elev1_modulus)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0 - elev_bias_angle_sd;
                            }
                            else
                            {
                                elev_bias_angle_sd += local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                            }
                        }
                    }
                    else
                    {
                        int sigma_delta_elev_beam_index = elev_index;
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_elev_slope[0].sigma_delta_slope[sigma_delta_elev_beam_index];

                        double elev_high_plus_lower_amp = sigma_modulus + delta_elev2_modulus;
                        double elev_high_minus_lower_amp = sigma_modulus - delta_elev2_modulus;
                        if (fabs(elev_high_plus_lower_amp) > EPS)
                        {
                            double sigma_delta_elev_high_lower_modulus = elev_high_minus_lower_amp / elev_high_plus_lower_amp;
                            elev_bias_angle_sd = fabs(sigma_delta_elev_high_lower_modulus) * sigma_delta_elev_slope;
                            if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                            }

                            if (sigma_modulus >= delta_elev2_modulus)
                            {
                                elev_bias_angle_sd = elev_bias_angle_sd - local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                            }
                            else
                            {
                                elev_bias_angle_sd = -elev_bias_angle_sd - local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                            }
                        }
                    }

                    elev_deg += elev_bias_angle_sd;
                }
            }
            else if (elev_index == MAX_VALID_ELEV_BEAM_INDEX)
            {
                if (sigma_I != 0 && sigma_Q != 0)
                {
                    // 如果第7波束幅度大于第8波束幅度, 表示目标可能在第7波束, 则按照方法1做和差
                    if (sigma_modulus < delta_elev1_modulus)
                    {
                        int sigma_delta_elev_beam_index = elev_index - 1;
                        if (sigma_delta_elev_beam_index < 0)
                        {
                            sigma_delta_elev_beam_index = 0;
                        }
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_elev_slope[0].sigma_delta_slope[sigma_delta_elev_beam_index];

                        double elev_high_plus_lower_amp = delta_elev1_modulus + sigma_modulus;
                        double elev_high_minus_lower_amp = delta_elev1_modulus - sigma_modulus;
                        if (fabs(elev_high_plus_lower_amp) > EPS)
                        {
                            double sigma_delta_elev_high_lower_modulus = elev_high_minus_lower_amp / elev_high_plus_lower_amp;
                            elev_bias_angle_sd = fabs(sigma_delta_elev_high_lower_modulus) * sigma_delta_elev_slope;

                            if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0)
                            {
                                elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                            }
                            elev_bias_angle_sd += local_data_processing_settings.sigma_delta_parameters[1].beam_width / 2.0;
                        }
                        elev_deg += elev_bias_angle_sd;
                    }
                    else
                    {
                        // 方法2：正常和差
                        complex<double> sigma_delta_elev2_complex = delta_elev2_complex / sigma_complex;
                        double sigma_delta_elev2_modulus = abs(sigma_delta_elev2_complex);
                        double sigma_delta_elev2_phase = arg(sigma_delta_elev2_complex) * RAD2DEG;
                        sigma_delta_elev_slope = local_data_processing_settings.sigma_delta_parameters[1].sigma_delta_slope;
                        elev_bias_angle_sd = sigma_delta_elev2_modulus * sigma_delta_elev_slope;
                        if (elev_bias_angle_sd > local_data_processing_settings.sigma_delta_parameters[1].max_bias_angle)
                        {
                            elev_bias_angle_sd = local_data_processing_settings.sigma_delta_parameters[1].max_bias_angle;
                        }
                        if (isBetweenFloorAndCeilDegPlusMinus180(sigma_delta_elev2_phase,
                                                                 local_data_processing_settings.sigma_delta_parameters[1].negative_bias_phase_floor,
                                                                 local_data_processing_settings.sigma_delta_parameters[1].negative_bias_phase_ceil))
                        {
                            elev_bias_angle_sd = -elev_bias_angle_sd;
                        }
                        elev_deg += elev_bias_angle_sd;
                    }
                }
            }
        }
    }

    if (amp_sigma < 300)
    {
        is_filtered = true;
    }

    snr = (((uint16_t)target_info.amp_sigma[0] << 8) | target_info.amp_sigma[1]) / 16.0;
    cfar_neighbour_num = target_info.neighbour_target_num;

    // 转换 RCS
    if (range < range_division_01)
    {
        rcs = pow(range, 4) * pow(amp_sigma, 2) / decay0 * rcs_correction_coef;
    }
    else if (range >= range_division_12)
    {
        rcs = pow(range, 4) * pow(amp_sigma, 2) / decay2 * rcs_correction_coef;
    }
    else
    {
        rcs = pow(range_division_01, 4) * pow(amp_sigma, 2) / decay0 * rcs_correction_coef;
    }

    // 转换x y z, vx vy vz
    x = range * cos(elev_deg * DEG2RAD) * cos(azim_deg * DEG2RAD);
    y = range * cos(elev_deg * DEG2RAD) * sin(azim_deg * DEG2RAD);
    z = range * sin(elev_deg * DEG2RAD);

    // 注意: vr的符号跟多普勒一致, 靠近为正, 远离为负, 但是跟距离变化趋势相反, 而 vx vy vz 形容的是 x y z 的变化趋势, 要对 vr 取反之后计算
    vx = -vr * cos(elev_deg * DEG2RAD) * cos(azim_deg * DEG2RAD);
    vy = -vr * cos(elev_deg * DEG2RAD) * sin(azim_deg * DEG2RAD);
    vz = -vr * sin(elev_deg * DEG2RAD);

    // 区域过滤
    if (local_data_processing_settings.selected_area_settings.select_radar_mode_flag)
    {
        if (radar_mode < local_data_processing_settings.selected_area_settings.selected_radar_mode_floor ||
            radar_mode > local_data_processing_settings.selected_area_settings.selected_radar_mode_ceil)
        {
            is_filtered = true;
        }
    }
    if (local_data_processing_settings.selected_area_settings.select_range_flag)
    {
        if (range < local_data_processing_settings.selected_area_settings.selected_range_floor || range > local_data_processing_settings.selected_area_settings.selected_range_ceil)
        {
            is_filtered = true;
        }
    }
    if (local_data_processing_settings.selected_area_settings.select_azim_flag)
    {
        if (!isBetweenFloorAndCeilDouble(azim_deg,
                                         local_data_processing_settings.selected_area_settings.selected_azim_deg_floor,
                                         local_data_processing_settings.selected_area_settings.selected_azim_deg_ceil,
                                         0,
                                         360))
        {
            is_filtered = true;
        }
    }
    if (local_data_processing_settings.selected_area_settings.select_elev_flag)
    {
        if (elev_deg < local_data_processing_settings.selected_area_settings.selected_elev_deg_floor ||
            elev_deg > local_data_processing_settings.selected_area_settings.selected_elev_deg_ceil)
        {
            is_filtered = true;
        }
    }
    if (local_data_processing_settings.selected_area_settings.select_velocity_flag)
    {
        if (fabs(vr) < local_data_processing_settings.selected_area_settings.selected_velocity_floor ||
            fabs(vr) > local_data_processing_settings.selected_area_settings.selected_velocity_ceil)
        {
            is_filtered = true;
        }
    }
    if (local_data_processing_settings.selected_area_settings.select_height_flag)
    {
        if (z < local_data_processing_settings.selected_area_settings.selected_height_floor || z > local_data_processing_settings.selected_area_settings.selected_height_ceil)
        {
            is_filtered = true;
        }
    }
    if (local_data_processing_settings.selected_area_settings.select_amp_flag)
    {
        if (amp_sigma < local_data_processing_settings.selected_area_settings.selected_amp_floor ||
            amp_sigma > local_data_processing_settings.selected_area_settings.selected_amp_ceil)
        {
            is_filtered = true;
        }
    }

    // exclude
    for (int ii = 0; ii < EXCLUDE_SETTINGS_NUM; ii++)
    {
        if (local_data_processing_settings.exclude_settings[ii].is_selected_flag)
        {
            if ((radar_mode >= local_data_processing_settings.exclude_settings[ii].radar_mode_min) &&
                (radar_mode <= local_data_processing_settings.exclude_settings[ii].radar_mode_max) && (range >= local_data_processing_settings.exclude_settings[ii].range_min) &&
                (range <= local_data_processing_settings.exclude_settings[ii].range_max) &&
                (isBetweenFloorAndCeilDouble(azim_deg,
                                             local_data_processing_settings.exclude_settings[ii].azim_deg_min,
                                             local_data_processing_settings.exclude_settings[ii].azim_deg_max,
                                             0,
                                             360)) &&
                (elev_deg >= local_data_processing_settings.exclude_settings[ii].elev_deg_min) && (elev_deg <= local_data_processing_settings.exclude_settings[ii].elev_deg_max) &&
                (vr >= local_data_processing_settings.exclude_settings[ii].velocity_min) && (vr <= local_data_processing_settings.exclude_settings[ii].velocity_max) &&
                (amp_sigma >= local_data_processing_settings.exclude_settings[ii].amp_min) && (amp_sigma <= local_data_processing_settings.exclude_settings[ii].amp_max) &&
                (z >= local_data_processing_settings.exclude_settings[ii].height_min) && (z <= local_data_processing_settings.exclude_settings[ii].height_max))
            {
                is_filtered = true;
                break;
            }
        }
    }

    TargetOriginalPoint new_op = TargetOriginalPoint(date_time,
                                                     radar_mode,
                                                     range_index,
                                                     azim_index,
                                                     elev_index,
                                                     doppler_index,
                                                     range,
                                                     azim_deg,
                                                     elev_deg,
                                                     vr,
                                                     x,
                                                     y,
                                                     z,
                                                     vx,
                                                     vy,
                                                     vz,
                                                     amp_sigma,
                                                     amp_delta_azim,
                                                     amp_delta_elev1,
                                                     amp_delta_elev2,
                                                     sigma_I,
                                                     sigma_Q,
                                                     delta_azim_I,
                                                     delta_azim_Q,
                                                     delta_elev1_I,
                                                     delta_elev1_Q,
                                                     delta_elev2_I,
                                                     delta_elev2_Q,
                                                     free_time,
                                                     snr,
                                                     cfar_neighbour_num,
                                                     rcs,
                                                     is_filtered,
                                                     is_combined,
                                                     is_correlated,
                                                     best_correlate_distance,
                                                     false);

    // 输出原始点迹
    if (local_software_settings.output_file_settings.output_radar_original_points_flag)
    {
        outputRadarOriginalPointToText(new_op, local_radar_system_state);
    }
    return new_op;
}

void RadarDataProcessingObject::extractTargetInfoFromFpgaPacketScanMode(const TargetInfo& target_info,
                                                                        const QDateTime& date_time,
                                                                        const int& azim_sector_index,
                                                                        const int& radar_mode,
                                                                        const double& free_time,
                                                                        const double& beamcenter_azim_deg,
                                                                        const int& azim_index,
                                                                        const double& breamcenter_elev_deg,
                                                                        const int& elev_index,
                                                                        const int& dbf_weight_flag)
{
    // 保证每个扇区的原始点不会太多(模拟码盘时比较容易有这个问题)
    if (azim_sector_list_scan_mode.azim_sector_list[azim_sector_index].target_original_points_list.size() <= MAX_ORIGINAL_POINTS_NUM_IN_EACH_AZIM_SETCOR)
    {
        TargetOriginalPoint new_op =
            extractTargetInfoFromFpgaPacket(target_info, date_time, radar_mode, free_time, beamcenter_azim_deg, azim_index, breamcenter_elev_deg, elev_index, dbf_weight_flag);

        // 过滤云雨杂波 放在搜索模式
        // 思想：云雨杂波是一片目标, 云雨目标所在的数据报文的目标数量一般会, 甚至达到 40
        if (local_data_processing_settings.filter_cloud_settings.enable_filter_cloud)
        {
            if (local_radar_system_state.target_num > local_data_processing_settings.filter_cloud_settings.filter_cloud_threshold)
            {
                new_op.is_filtered = true;
            }
        }

        // 但是最低波束 (差波束)不参与后续处理
        if (elev_index <= MAX_VALID_ELEV_BEAM_INDEX)
        {
            // 动态杂波图
            if (local_data_processing_settings.clutter_map_settings.clutter_map_filter_flag)
            {
                cluttermapFilter(new_op);
            }
            updateClutterMapBasedOnNewOriginalPoint(new_op);

            // 过滤的点迹也进入后续处理用于融合, 但是根据是否显示过滤点迹, 判断是否需要新建原始点迹
            azim_sector_list_scan_mode.azim_sector_list[azim_sector_index].addTargetOriginalPoint(new_op);
        }

        if (new_op.is_filtered)
        {
            if (local_software_settings.display_settings.display_original_points_flag && local_software_settings.display_settings.display_filtered_points_flag)
            {
                OriginalPointItem* original_point_item = new OriginalPointItem(new_op);
                added_original_point_item_list_scan_mode[azim_sector_index].append(original_point_item);
            }
        }
        else
        {
            if (local_software_settings.display_settings.display_original_points_flag)
            {
                OriginalPointItem* original_point_item = new OriginalPointItem(azim_sector_list_scan_mode.azim_sector_list[azim_sector_index].target_original_points_list.last());
                added_original_point_item_list_scan_mode[azim_sector_index].append(original_point_item);
            }
        }
    }
}

void RadarDataProcessingObject::extractTargetInfoFromFpgaPacketTrackMode(const TargetInfo& target_info,
                                                                         const QDateTime& date_time,
                                                                         const int& azim_sector_index,
                                                                         const int& radar_mode,
                                                                         const double& free_time,
                                                                         const double& beamcenter_azim_deg,
                                                                         const int& azim_index,
                                                                         const double& breamcenter_elev_deg,
                                                                         const int& elev_index,
                                                                         const int& dbf_weight_flag)
{
    TargetOriginalPoint new_op =
        extractTargetInfoFromFpgaPacket(target_info, date_time, radar_mode, free_time, beamcenter_azim_deg, azim_index, breamcenter_elev_deg, elev_index, dbf_weight_flag);

    // 过滤的点迹也进入后续处理用于融合, 但是根据是否显示过滤点迹, 判断是否需要新建原始点迹
    // 但是最低波束 (差波束)不参与后续处理
    if (elev_index <= MAX_VALID_ELEV_BEAM_INDEX)
    {
        original_points_batch_track_mode.addTargetOriginalPoint(new_op);
    }

    if (new_op.is_filtered)
    {
        if (local_software_settings.display_settings.display_original_points_flag && local_software_settings.display_settings.display_filtered_points_flag)
        {
            OriginalPointItem* original_point_item = new OriginalPointItem(new_op);
            adding_original_point_item_list_track_mode->append(original_point_item);
        }
    }
    else
    {
        if (local_software_settings.display_settings.display_original_points_flag)
        {
            OriginalPointItem* original_point_item = new OriginalPointItem(original_points_batch_track_mode.target_original_points_list.last());
            adding_original_point_item_list_track_mode->append(original_point_item);
        }
    }
}

void RadarDataProcessingObject::updateRadarSystemState(const FpgaPacket* fpga_packet, RadarSystemState& radar_system_state)
{
    // 更新当前数据处理的模式
    if (prepare_tracking_flag || start_tracking_flag)
    {
        radar_system_state.current_data_processing_mode = RADAR_TRACK_MODE;
    }
    else
    {
        radar_system_state.current_data_processing_mode = RADAR_SCAN_MODE;
    }

    radar_system_state.radar_connected_flag = 1;
    ++radar_system_state.pkt_num;
    // 更新雷达模式
    radar_system_state.radar_mission_mode = fpga_packet->radar_work_mode & 0x07;
    switch (radar_system_state.radar_mission_mode)
    {
    default: // 近搜
        radar_system_state.current_radar_mission_mode_index = 0;
        radar_system_state.current_radar_mode_index = NEAR_SCAN_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_SCAN_MODE;
        break;
    case 0x01: // 远搜
        radar_system_state.current_radar_mission_mode_index = 1;
        radar_system_state.current_radar_mode_index = FAR_SCAN_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_SCAN_MODE;
        break;
    case 0x05: // 超远搜
        radar_system_state.current_radar_mission_mode_index = 2;
        radar_system_state.current_radar_mode_index = SUPER_FAR_SCAN_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_SCAN_MODE;
        break;
    case 0x02: // 近跟
        radar_system_state.current_radar_mission_mode_index = 3;
        radar_system_state.current_radar_mode_index = NEAR_TRACK_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_TRACK_MODE;
        break;
    case 0x03: // 近跟B
        radar_system_state.current_radar_mission_mode_index = 4;
        radar_system_state.current_radar_mode_index = NEAR_TRACK_B_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_TRACK_MODE;
        break;
    case 0x06: // 远跟
        radar_system_state.current_radar_mission_mode_index = 5;
        radar_system_state.current_radar_mode_index = FAR_TRACK_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_TRACK_MODE;
        break;
    case 0x0B: // 远跟B
        radar_system_state.current_radar_mission_mode_index = 6;
        radar_system_state.current_radar_mode_index = FAR_TRACK_B_MODE_INDEX;
        radar_system_state.current_scan_or_track_mode = RADAR_TRACK_MODE;
        break;
    }

    // 更新自由时间
    radar_system_state.current_free_time = (((uint16_t)fpga_packet->free_time[0] << 8) | fpga_packet->free_time[1]) * 0.2 / 1000.0;
    double delta_free_time = circularlyAddDouble(radar_system_state.current_free_time, -radar_system_state.last_free_time, 0, FREE_TIME_MAX);
    radar_system_state.last_free_time = radar_system_state.current_free_time;
    radar_system_state.cumulative_free_time = radar_system_state.cumulative_free_time + delta_free_time;

    // 更新方位角(需考虑方位偏移)
    radar_system_state.biss_angle_deg = (((uint16_t)fpga_packet->BISS_angle[0] << 8) | fpga_packet->BISS_angle[1]) / 65536.0 * 360.0;
    radar_system_state.current_azim_deg =
        circularlyAddDouble(radar_system_state.biss_angle_deg, local_data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle, 0, 360);
    radar_system_state.current_azim_deg_to_north = circularlyAddDouble(450, -radar_system_state.current_azim_deg, 0, 360);

    radar_system_state.current_transmit_azim_deg = (((uint16_t)fpga_packet->azim_angle[0] << 8) | fpga_packet->azim_angle[1]) / 65536.0 * 360.0;
    radar_system_state.current_transmit_azim_deg =
        circularlyAddDouble(radar_system_state.current_transmit_azim_deg, local_data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle, 0.0, 360.0);
    if (radar_system_state.last_transmit_azim_deg < 0)
    {
        radar_system_state.total_lost_azim_sectors_num = 0;
        radar_system_state.last_transmit_azim_deg = radar_system_state.current_transmit_azim_deg;
    }
    else
    {
        double delta_azim_deg = circularlyAddDouble(radar_system_state.current_transmit_azim_deg, -radar_system_state.last_transmit_azim_deg, -180, 360);
        radar_system_state.last_transmit_azim_deg = radar_system_state.current_transmit_azim_deg;

        if (delta_free_time < 1) // 状态报文应该是2s传一次, 如果传状态报文, 不统计丢失扇区
        {
            // 如果两包间隔小于1s, 即当前不是状态报文
            if (delta_azim_deg > 1.5 * local_data_processing_settings.sigma_delta_parameters[0].beam_width)
            {
                radar_system_state.current_lost_azim_sectors_num = int(fabs(delta_azim_deg) / local_data_processing_settings.sigma_delta_parameters[0].beam_width);
            }
        }
        else
        {
            radar_system_state.current_lost_azim_sectors_num = 0;
        }
        radar_system_state.total_lost_azim_sectors_num += radar_system_state.current_lost_azim_sectors_num;
        if (radar_system_state.current_lost_azim_sectors_num > 0)
        {
            radar_system_state.error_info_flag_list[0] = 1; // 丢扇区
            radar_system_state.error_lost_azim_sectors_num = radar_system_state.current_lost_azim_sectors_num;
        }
    }

    radar_system_state.radar_state_register = fpga_packet->radar_system_status;
    // 转换俯仰角
    if ((radar_system_state.radar_state_register & 0x40) == 0x40)
    {
        radar_system_state.receiver_weight_set = 1;
    }
    else
    {
        radar_system_state.receiver_weight_set = 0;
    }
    radar_system_state.dbf_coef_receive_raising_angle_index = fpga_packet->dbf_coef_receive_raising_angle_index & 0x03;
    radar_system_state.current_elev_index = fpga_packet->elev_index;
    // 搜索模式 1-10 跟踪模式 0-9
    if (radar_system_state.current_scan_or_track_mode == RADAR_SCAN_MODE)
    {
        radar_system_state.current_elev_index -= 1;
    }
    if (radar_system_state.current_elev_index < 0)
    {
        radar_system_state.current_elev_index = 0;
    }
    else if (radar_system_state.current_elev_index > MAX_ELEV_BEAM_INDEX)
    {
        radar_system_state.current_elev_index = MAX_ELEV_BEAM_INDEX;
    }
    double elev_last_beam_raising_deg =
        DBF_COEF_RECEIVE_RAISING_ANGLE_MAP[radar_system_state.dbf_coef_receive_raising_angle_index] + local_data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle;
    if (radar_system_state.receiver_weight_set)
    {
        radar_system_state.current_elev_deg = WEIGHTED_ELEV_INDEX_TO_BEAM_CENTER_DEG_MAP[radar_system_state.current_elev_index] + elev_last_beam_raising_deg;
    }
    else
    {
        radar_system_state.current_elev_deg = UNWEIGHTED_ELEV_INDEX_TO_BEAM_CENTER_DEG_MAP[radar_system_state.current_elev_index] + elev_last_beam_raising_deg;
    }

    // 状态信息
    if ((radar_system_state.radar_state_register & 0x02) == 0) // 阵列失锁2 (1-锁定 0-失锁)
    {
        radar_system_state.error_info_flag_list[1] = 1;
    }
    if ((radar_system_state.radar_state_register & 0x04) == 0) // 阵列失锁1 (1-锁定 0-失锁)
    {
        radar_system_state.error_info_flag_list[2] = 1;
    }
    if ((radar_system_state.radar_state_register & 0x08) == 0) // BISS角度错误  (0-不可信 1-正确)
    {
        radar_system_state.error_info_flag_list[3] = 1;
    }
    if ((radar_system_state.radar_state_register & 0x10) == 0x10) // 转台状态异常 (0-正常 1-异常)
    {
        radar_system_state.error_info_flag_list[4] = 1;
    }
    if ((radar_system_state.radar_state_register & 0x20) == 0) // FPGA收到校准系数标志位， 0-未收到 1-收到
    {
        radar_system_state.error_info_flag_list[5] = 1;
        none_dbf_coef_in_fpga_flag = true;
    }
    else
    {
        none_dbf_coef_in_fpga_flag = false;
    }

    radar_system_state.target_num = fpga_packet->target_num;
    radar_system_state.radar_set_mode = fpga_packet->radar_set_work_mode;
    radar_system_state.track_azim_set = fpga_packet->track_azim_set[0] << 8 | fpga_packet->track_azim_set[1];
    radar_system_state.turntable_rotating_speed = int16_t(((uint16_t)fpga_packet->turntable_rotating_speed[0] << 8) | fpga_packet->turntable_rotating_speed[1]) / 100.0;

    radar_system_state.current_GMAC_count = ((uint16_t)fpga_packet->GMAC_packet_num[0] << 8) | fpga_packet->GMAC_packet_num[1];
    if (radar_system_state.last_GMAC_count < 0)
    {
        radar_system_state.total_lost_packets_count = 0;
        radar_system_state.last_GMAC_count = radar_system_state.current_GMAC_count;
    }
    else
    {
        // 丢包次数等于前后两包差值减 1
        radar_system_state.current_lost_packets_count = circularlyAddInt(radar_system_state.current_GMAC_count, -radar_system_state.last_GMAC_count, 0, TWO_BYTE_UINT_MAX) - 1;
        radar_system_state.total_lost_packets_count += radar_system_state.current_lost_packets_count;
        radar_system_state.last_GMAC_count = radar_system_state.current_GMAC_count;

        if (radar_system_state.current_lost_packets_count > 0)
        {
            radar_system_state.error_info_flag_list[6] = 1; // 丢包
            radar_system_state.error_lost_packets_count = radar_system_state.current_lost_packets_count;
        }
    }
    // 天线状态
    radar_system_state.antenna_command_mode = fpga_packet->antenna_command_code;
    switch (radar_system_state.antenna_command_mode)
    {
    case 0x81:
        radar_system_state.antenna_work_mode = (fpga_packet->antenna_work_mode_or_in_current_28v & 0x0C) >> 2;
        radar_system_state.antenna_temperature_warning = fpga_packet->antenna_temperature_warning_or_fre_code & 0x01;
        if (radar_system_state.antenna_temperature_warning == 0x01)
        {
            radar_system_state.error_info_flag_list[7] = 1; // 天线温度警告
        }
        radar_system_state.bank_12v_current = fpga_packet->current_12v * 0.2;
        radar_system_state.launch_28v_current = fpga_packet->current_28v_or_12v * 0.2;
        radar_system_state.output_power_1 = fpga_packet->out_put_power_1_or_T & 0x01;
        radar_system_state.output_power_2 = fpga_packet->out_put_power_2_or_T & 0x01;
        break;
    case 0x82:
        radar_system_state.fre_code = fpga_packet->antenna_temperature_warning_or_fre_code;
        radar_system_state.in_current_28v = fpga_packet->antenna_work_mode_or_in_current_28v * 0.2;
        radar_system_state.recv_12v_current = fpga_packet->current_12v * 0.2;
        radar_system_state.fre_12v_current = fpga_packet->current_28v_or_12v * 0.2;
        radar_system_state.beam_ctrl_12v_current = fpga_packet->out_put_power_1_or_T;
        radar_system_state.battery_T = int8_t(fpga_packet->out_put_power_2_or_T);
        break;
    case 0x83:
        radar_system_state.T_transmitter1 = int8_t(fpga_packet->antenna_temperature_warning_or_fre_code);
        radar_system_state.T_transmitter2 = int8_t(fpga_packet->antenna_work_mode_or_in_current_28v);
        radar_system_state.T_recv1 = int8_t(fpga_packet->current_12v);
        radar_system_state.T_recv2 = int8_t(fpga_packet->current_28v_or_12v);
        radar_system_state.T_recv3 = int8_t(fpga_packet->out_put_power_1_or_T);
        radar_system_state.T_recv4 = int8_t(fpga_packet->out_put_power_2_or_T);
        break;
    }

    radar_system_state.bank_mode_report = fpga_packet->bank_report_mode;

    // TODO 温度错误
    uint16_t T_fpga_tmp1 = fpga_packet->T_AD_FPGA1[0] << 8 | fpga_packet->T_AD_FPGA1[1];
    radar_system_state.T_AD_FPGA1 = T_fpga_tmp1 * 503.975 / 65536.0 - 273.15;

    uint16_t T_fpga_tmp2 = fpga_packet->T_AD_FPGA2[0] << 8 | fpga_packet->T_AD_FPGA2[1];
    radar_system_state.T_AD_FPGA2 = T_fpga_tmp2 * 503.975 / 65536.0 - 273.15;

    uint16_t T_fpga_tmp3 = fpga_packet->T_QDR_FPGA1[0] << 8 | fpga_packet->T_QDR_FPGA1[1];
    radar_system_state.T_QDR_FPGA1 = (T_fpga_tmp3 & 0xFFE0) * 503.975 / 65536.0 - 273.15;

    uint16_t T_fpga_tmp4 = fpga_packet->T_QDR_FPGA2[0] << 8 | fpga_packet->T_QDR_FPGA2[1];
    radar_system_state.T_QDR_FPGA2 = (T_fpga_tmp4 & 0xFFE0) * 503.975 / 65536.0 - 273.15;

    uint16_t T_fpga_tmp5 = fpga_packet->T_QDR_FPGA3[0] << 8 | fpga_packet->T_QDR_FPGA3[1];
    radar_system_state.T_QDR_FPGA3 = (T_fpga_tmp5 & 0xFFE0) * 503.975 / 65536.0 - 273.15;

    uint16_t T_fpga_tmp6 = fpga_packet->T_QDR_FPGA4[0] << 8 | fpga_packet->T_QDR_FPGA4[1];
    radar_system_state.T_QDR_FPGA4 = (T_fpga_tmp6 & 0xFFE0) * 503.975 / 65536.0 - 273.15;

    if (T_fpga_tmp3 == 0 || T_fpga_tmp4 == 0 || T_fpga_tmp4 == 0 || T_fpga_tmp5 == 0 || T_fpga_tmp6 == 0)
    {
        radar_system_state.error_info_flag_list[8] = 1; // Serdes 连接失败
        serdes_error_flag = true;
    }
    else
    {
        serdes_error_flag = false;
    }

    if (radar_system_state.T_AD_FPGA1 >= WARNING_ANTENNA_TEMPERATURE || radar_system_state.T_AD_FPGA2 >= WARNING_ANTENNA_TEMPERATURE ||
        radar_system_state.T_QDR_FPGA1 >= WARNING_ANTENNA_TEMPERATURE || radar_system_state.T_QDR_FPGA2 >= WARNING_ANTENNA_TEMPERATURE ||
        radar_system_state.T_QDR_FPGA3 >= WARNING_ANTENNA_TEMPERATURE || radar_system_state.T_QDR_FPGA4 >= WARNING_ANTENNA_TEMPERATURE)
    {
        radar_system_state.error_info_flag_list[9] = 1; // FPGA温度过高
    }

    uint8_t tmp[4];
    tmp[3] = fpga_packet->ATP_azim_position[0];
    tmp[2] = fpga_packet->ATP_azim_position[1];
    tmp[1] = fpga_packet->ATP_azim_position[2];
    tmp[0] = fpga_packet->ATP_azim_position[3];
    memcpy(&radar_system_state.ATP_azim_position, tmp, sizeof(tmp));

    tmp[3] = fpga_packet->ATP_azim_speed[0];
    tmp[2] = fpga_packet->ATP_azim_speed[1];
    tmp[1] = fpga_packet->ATP_azim_speed[2];
    tmp[0] = fpga_packet->ATP_azim_speed[3];
    memcpy(&radar_system_state.ATP_azim_speed, tmp, sizeof(tmp));

    tmp[3] = fpga_packet->ATP_azim_acceleration[0];
    tmp[2] = fpga_packet->ATP_azim_acceleration[1];
    tmp[1] = fpga_packet->ATP_azim_acceleration[2];
    tmp[0] = fpga_packet->ATP_azim_acceleration[3];
    memcpy(&radar_system_state.ATP_azim_acc, tmp, sizeof(tmp));

    radar_system_state.ATP_cyclic_code = fpga_packet->ATP_cyclic_code;
    radar_system_state.ATP_parity = fpga_packet->ATP_check_code;

    radar_system_state.FPGA_version = (uint64_t)fpga_packet->AD_FPGA_serial_number[0] << 56 | (uint64_t)fpga_packet->AD_FPGA_serial_number[1] << 48 |
                                      (uint64_t)fpga_packet->AD_FPGA_serial_number[2] << 40 | (uint64_t)fpga_packet->AD_FPGA_serial_number[3] << 32 |
                                      (uint64_t)fpga_packet->AD_FPGA_serial_number[4] << 24 | (uint64_t)fpga_packet->AD_FPGA_serial_number[5] << 16 |
                                      (uint64_t)fpga_packet->AD_FPGA_serial_number[6] << 8 | (uint64_t)fpga_packet->AD_FPGA_serial_number[7];
    radar_system_state.FPGA_version = radar_system_state.FPGA_version >> 7;

    // 根据俯仰下标更新发射方位角, 如果当前俯仰下标小于之前的俯仰下标，表示已经到了下一轮
    if (radar_system_state.current_elev_index <= radar_system_state.last_elev_index)
    {
        radar_system_state.next_elev_round_flag = 1;
        // 如果转速的绝对值大于一定值，修正发射方位角
        if (fabs(radar_system_state.turntable_rotating_speed) >= TURNTABLE_ROTATING_SPEED_THRESHOLD_TO_CORRECT_AZIM)
        {
            radar_system_state.current_transmit_azim_center =
                circularlyAddDouble(radar_system_state.current_azim_deg,
                                    -radar_system_state.turntable_rotating_speed *
                                        local_data_processing_settings.radar_wave_paremeters[radar_system_state.current_radar_mode_index].time_to_correct_azim,
                                    0,
                                    360);
        }
        else
        {
            radar_system_state.current_transmit_azim_center = radar_system_state.current_azim_deg;
        }
    }
    else
    {
        radar_system_state.next_elev_round_flag = 0;
    }
    radar_system_state.last_elev_index = radar_system_state.current_elev_index;

    need_to_update_radar_system_state_in_other_thread_flag = true;
}

void RadarDataProcessingObject::clearUpOriginalPointsTrackModeInOriginalPointsBatch()
{
    original_points_batch_track_mode.target_original_points_list.clear();

    // 如果已经发送了, 就由航迹界面删除, 否则由本地删除
    if (added_original_point_item_list_track_mode->size() > 0)
    {
        g_original_point_item_to_delete_list.append(*added_original_point_item_list_track_mode);
        added_original_point_item_list_track_mode->clear();
    }

    while (adding_original_point_item_list_track_mode->size() > 0)
    {
        delete (adding_original_point_item_list_track_mode->takeFirst());
    }

    if (g_original_point_item_to_delete_list.size() > 0)
    {
        emit(sigUpdateOriginalPointItems());
    }
}

void RadarDataProcessingObject::clearUpTracksTrackMode()
{
    if (tracking_target_track)
    {
        // 如果航迹未达到终止条件, 则不删除航迹, 把它放到对应的扇区
        if (tracking_target_track->continuous_lost_times < MAX_LOST_TIMES)
        {
            qDebug() << "【跟踪模式】: 退出时航迹未终止, 放到对应的扇区中!";
            tracking_target_track->distance_to_point_index_map.clear();
            tracking_target_track->is_correlated = false; // 改变相关标志位, 方便进入搜索模式后直接参与后续相关
            tracking_target_track->track_times_track_mode = 0;
            tracking_target_track->track_size_track_mode = 0;
            azim_sector_list_scan_mode.azim_sector_list[tracking_target_track->target_track_points_list.last().azim_index].target_tracks_list.append(tracking_target_track);
        }
        else
        {
            // TODO 航迹列表 同时需要删除界面显示的信息
            if (target_track_to_item_hashmap.contains(tracking_target_track))
            {
                g_target_track_item_to_delete_list.append(target_track_to_item_hashmap.value(tracking_target_track));
                target_track_to_item_hashmap.remove(tracking_target_track);
            }

            if (sending_target_set.contains(tracking_target_track->track_id))
            {
                sending_target_set.remove(tracking_target_track->track_id);
                emit sigRemoveTrackIdFromSendingTargetSet(tracking_target_track->track_id);
            }
            // 删除跟踪点迹需要把跟踪的航迹 id 返回给航迹 id 队列
            azim_sector_list_scan_mode.unused_track_id_list.append(tracking_target_track->track_id);
            delete (tracking_target_track);
        }

        tracking_target_track = nullptr;
        tracking_target_track_id = -1;

        if (g_target_track_item_to_delete_list.size() > 0)
        {
            emit sigUpdateTargetTrackItems();
        }
    }
}

void RadarDataProcessingObject::updateTrackAzimSectorIndexAndTrackItemsAndTrackToSendListAfterTracksUpdated()
{
    /*
     * 功能: 更新航迹所在扇区、更新航迹 item、输出至文件
     */
    for (QList<TargetTrack*>::iterator tt_it = azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_update_index].target_tracks_list.begin();
         tt_it != azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_update_index].target_tracks_list.end();)
    {
        // 根据是否显示航迹, 修改航迹的标志位
        (*tt_it)->target_track_points_list.last().is_shown = local_software_settings.display_settings.display_tracks_flag;

        // S1: 如果航迹已经终止, 删除已经终止的航迹 item 和航迹 (先输出航迹和发送, 删除航迹 item 和航迹)
        if ((*tt_it)->target_track_points_list.last().established_mode == -1)
        {
            // 输出航迹
            if (local_software_settings.output_file_settings.output_radar_track_points_flag)
            {
                outputLastTrackPointToText(*tt_it);
            }

            // 发送该航迹
            if ((*tt_it)->track_times >= local_software_settings.communication_settings.output_min_track_times)
            {
                track_to_send_list->append((*tt_it)->target_track_points_list.last());
            }

            if (sending_target_set.contains((*tt_it)->track_id))
            {
                sending_target_set.remove((*tt_it)->track_id);
                emit sigRemoveTrackIdFromSendingTargetSet((*tt_it)->track_id);
            }

            if (target_track_to_item_hashmap.contains(*tt_it))
            {
                g_target_track_item_to_delete_list.append(target_track_to_item_hashmap.value(*tt_it));
                target_track_to_item_hashmap.remove(*tt_it);
            }

            // 删除航迹
            azim_sector_list_scan_mode.unused_track_id_list.append((*tt_it)->track_id);
            delete (*tt_it);
            tt_it = azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_update_index].target_tracks_list.erase(tt_it);
            continue;
        }
        else
        {
            // S2: 如果航迹未终止, 如果航迹已经不在当前扇区了, 需要更新对应的扇区
            if ((*tt_it)->target_track_points_list.last().azim_index != azim_sector_list_scan_mode.scan_current_track_update_index)
            {
                int delta_azim_sector_index = circularlyAddInt((*tt_it)->target_track_points_list.last().azim_index,
                                                               -azim_sector_list_scan_mode.scan_current_track_update_index,
                                                               -HALF_OF_AZIM_SECTOR_NUM,
                                                               AZIM_SECTOR_NUM);
                // 如果航迹的最新点迹的扇区号 > 本扇区（即本扇区及之前）, 更改相关标志位, 然后写入文件、发送、更新或新建 item
                if (delta_azim_sector_index > 0)
                {
                    (*tt_it)->is_correlated = false;
                    if (target_track_to_item_hashmap.contains(*tt_it))
                    {
                        target_track_to_item_hashmap.value(*tt_it)->updateTargetTrackLastPoint(*tt_it);
                        g_target_track_item_to_update_list.append(target_track_to_item_hashmap.value(*tt_it));
                    }
                    else
                    {
                        if ((*tt_it)->track_times >= local_software_settings.display_settings.display_key_targets_track_times)
                        {
                            TargetTrackItem* track_item = new TargetTrackItem(*tt_it);
                            target_track_to_item_hashmap.insert(*tt_it, track_item);
                            g_target_track_item_to_add_list.append(track_item);
                        }
                    }

                    // 输出航迹
                    if (local_software_settings.output_file_settings.output_radar_track_points_flag)
                    {
                        outputLastTrackPointToText(*tt_it);
                    }

                    // 发送该航迹
                    if ((*tt_it)->track_times >= local_software_settings.communication_settings.output_min_track_times)
                    {
                        track_to_send_list->append((*tt_it)->target_track_points_list.last());
                    }
                }

                // 航迹最新点编号不在本扇区的, 更新航迹所在扇区
                azim_sector_list_scan_mode.azim_sector_list[(*tt_it)->target_track_points_list.last().azim_index].target_tracks_list.append((*tt_it));
                tt_it = azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_update_index].target_tracks_list.erase(tt_it);
                continue;
            }
            else
            {
                // 如果航迹的最新点迹的扇区号 == 本扇区, 更改相关标志位, 然后写入文件、发送、更新或新建 item
                (*tt_it)->is_correlated = false;

                // 输出航迹
                if (local_software_settings.output_file_settings.output_radar_track_points_flag)
                {
                    outputLastTrackPointToText(*tt_it);
                }

                // 发送该航迹
                if ((*tt_it)->track_times >= local_software_settings.communication_settings.output_min_track_times)
                {
                    track_to_send_list->append((*tt_it)->target_track_points_list.last());
                }

                if (target_track_to_item_hashmap.contains(*tt_it))
                {
                    target_track_to_item_hashmap.value(*tt_it)->updateTargetTrackLastPoint(*tt_it);
                    g_target_track_item_to_update_list.append(target_track_to_item_hashmap.value(*tt_it));
                }
                else
                {
                    if ((*tt_it)->track_times >= local_software_settings.display_settings.display_key_targets_track_times)
                    {
                        TargetTrackItem* track_item = new TargetTrackItem(*tt_it);
                        target_track_to_item_hashmap.insert(*tt_it, track_item);
                        g_target_track_item_to_add_list.append(track_item);
                    }
                }
            }
            ++tt_it;
        }
    }
}

void RadarDataProcessingObject::startTrackAndGenerateNewTrackItemAndUpdateTrackToSendList()
{
    /*
     * 功能: 将未相关上的原始点起始为新的航迹, 并且生成航迹 item 然后更新 track_to_send_list 并且输出至文件
     */
    for (int ii = 0; ii < azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_original_points_list.size(); ++ii)
    {
        if (azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_original_points_list[ii].is_filtered ||
            azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_original_points_list[ii].is_combined ||
            azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_original_points_list[ii].is_correlated)
        {
            continue;
        }
        if (azim_sector_list_scan_mode.unused_track_id_list.size() > 0)
        {
            int new_track_id = azim_sector_list_scan_mode.unused_track_id_list.takeFirst();
            TargetTrack* tt = new TargetTrack(new_track_id);
            tt->addMeasuredTrackPoint(azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_original_points_list[ii],
                                      local_data_processing_settings);
            tt->is_correlated = false; // 方便后续的相关
            azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_tracks_list.append(tt);

            // 航迹输出
            if (local_software_settings.output_file_settings.output_radar_track_points_flag)
            {
                outputLastTrackPointToText(tt);
            }

            // 航迹发送
            if (tt->track_times >= local_software_settings.communication_settings.output_min_track_times)
            {
                track_to_send_list->append(tt->target_track_points_list.last());
            }

            // 新建航迹item
            if (tt->track_times >= local_software_settings.display_settings.display_key_targets_track_times)
            {
                TargetTrackItem* track_item = new TargetTrackItem(tt);
                target_track_to_item_hashmap.insert(tt, track_item);
                g_target_track_item_to_add_list.append(track_item);
            }
        }
        else
        {
            break;
        }
    }
    azim_sector_list_scan_mode.azim_sector_list[azim_sector_list_scan_mode.scan_current_track_start_index].target_original_points_list.clear();
}

void RadarDataProcessingObject::clearUpOriginalPointsScanModeInAzimSectors()
{
    // 当前扇区的原始点迹还未发送到主界面, 直接删除
    while (added_original_point_item_list_scan_mode[current_input_azim_sector_index].size() > 0)
    {
        delete (added_original_point_item_list_scan_mode[current_input_azim_sector_index].takeFirst());
    }

    // 其余扇区的原始点迹由主界面删除
    for (int ii = 0; ii < AZIM_SECTOR_NUM; ++ii)
    {
        azim_sector_list_scan_mode.azim_sector_list[ii].target_original_points_list.clear();
        if (added_original_point_item_list_scan_mode[ii].size() > 0)
        {
            g_original_point_item_to_delete_list.append(added_original_point_item_list_scan_mode[ii]);
            added_original_point_item_list_scan_mode[ii].clear();
        }
    }
    if (g_original_point_item_to_delete_list.size() > 0)
    {
        emit sigUpdateOriginalPointItems();
    }
}

void RadarDataProcessingObject::clearUpTracksScanModeInAzimSectors()
{
    for (int ii = 0; ii < azim_sector_list_scan_mode.azim_sector_list.size(); ++ii)
    {
        for (QList<TargetTrack*>::iterator tt_it = azim_sector_list_scan_mode.azim_sector_list[ii].target_tracks_list.begin();
             tt_it != azim_sector_list_scan_mode.azim_sector_list[ii].target_tracks_list.end();)
        {
            (*tt_it)->distance_to_point_index_map.clear();
            if ((*tt_it)->track_id != tracking_target_track_id)
            {
                if (target_track_to_item_hashmap.contains(*tt_it))
                {
                    g_target_track_item_to_delete_list.append(target_track_to_item_hashmap.value(*tt_it));
                }
                target_track_to_item_hashmap.remove(*tt_it);

                if (sending_target_set.contains((*tt_it)->track_id))
                {
                    sending_target_set.remove((*tt_it)->track_id);
                    emit sigRemoveTrackIdFromSendingTargetSet((*tt_it)->track_id);
                }

                // 删除航迹前，需要归还航迹id
                azim_sector_list_scan_mode.unused_track_id_list.append((*tt_it)->track_id);
                delete (*tt_it);
            }
            tt_it = azim_sector_list_scan_mode.azim_sector_list[ii].target_tracks_list.erase(tt_it);
        }
    }
    if (g_target_track_item_to_delete_list.size() > 0)
    {
        emit sigUpdateTargetTrackItems();
    }
}

bool RadarDataProcessingObject::packetHandlerTrackMode(const FpgaPacket* fpga_packet)
{
    QDateTime date_time = QDateTime::currentDateTime();
    updateRadarSystemState(fpga_packet, local_radar_system_state);
    if (local_radar_system_state.current_radar_mode_index < RADAR_SCAN_TRACK_MODE_DIVISION_INDEX)
    {
        //        qDebug() << QString("[Warning]: track mode, ignore packets scan mode!");
        return false;
    }

    // 根据发射角度确定在哪个扇区方位角
    double transmit_azim_deg_beam_center = local_radar_system_state.current_transmit_azim_center;
    int azim_index = (int(transmit_azim_deg_beam_center / AZIM_SECTOR_BEAM_WIDTH)) % AZIM_SECTOR_NUM;

    // 提取目标信息
    for (int ii = 0; ii < local_radar_system_state.target_num; ++ii)
    {
        extractTargetInfoFromFpgaPacketTrackMode(fpga_packet->target_info[ii],
                                                 date_time,
                                                 azim_index,
                                                 local_radar_system_state.current_radar_mode_index,
                                                 local_radar_system_state.cumulative_free_time,
                                                 transmit_azim_deg_beam_center,
                                                 azim_index,
                                                 local_radar_system_state.current_elev_deg,
                                                 local_radar_system_state.current_elev_index,
                                                 local_radar_system_state.receiver_weight_set);
    }

    // 判断是否满足跟踪条件
    ++continuous_packet_num_track_mode;
    if (continuous_packet_num_track_mode % ELEV_BEAM_NUM_TRACK_MODE == 0)
    {
        g_original_point_item_to_delete_list.append(*added_original_point_item_list_track_mode);
        added_original_point_item_list_track_mode->clear();
        g_original_point_item_to_add_list.append(*adding_original_point_item_list_track_mode);
        added_original_point_item_list_track_mode->swap(*adding_original_point_item_list_track_mode);

        original_points_batch_track_mode.current_free_time = local_radar_system_state.cumulative_free_time;
        original_points_batch_track_mode_ready_flag = true;
        continuous_packet_num_track_mode = 0;

        if (g_original_point_item_to_add_list.size() > 0 || g_original_point_item_to_delete_list.size() > 0)
        {
            emit sigUpdateOriginalPointItems();
        }
    }

    return true;
}

void RadarDataProcessingObject::initializeRadarDataProcessingTrackModeParameters()
{
    continuous_packet_num_track_mode = 0;
    original_points_batch_track_mode_ready_flag = false;
}

void RadarDataProcessingObject::createNewRadarSystemInfoFile()
{
    // S1: 如果目录不存在, 则创建这个目录
    QDir radar_system_info_dir;
    if (!radar_system_info_dir.exists(RADAR_SYSTEM_INFO_FOLDER_DIR))
    {
        radar_system_info_dir.mkpath(RADAR_SYSTEM_INFO_FOLDER_DIR);
    }
    radar_system_info_dir.setPath(RADAR_SYSTEM_INFO_FOLDER_DIR);
    // S2: 读取所有的txt
    QStringList radar_system_info_file_filter;
    radar_system_info_file_filter << "*.txt";
    QFileInfoList radar_system_info_file_list = radar_system_info_dir.entryInfoList(radar_system_info_file_filter, QDir::Files, QDir::Name);
    // S3: 如果文件数量超过MAX_RADAR_SYSTEM_INFO_FILE_NUM, 删除最老的
    while (radar_system_info_file_list.size() >= MAX_RADAR_SYSTEM_INFO_FILE_NUM)
    {
        radar_system_info_dir.remove(radar_system_info_file_list.first().fileName());
        radar_system_info_file_list.pop_front();
    }
    // S4: 新建文件并打开
    // 关闭之前的文件
    if (radar_system_info_txt)
    {
        radar_system_info_txt->close();
        delete radar_system_info_txt;
        is_radar_system_info_txt_opened = false;
    }
    // 新建一个文件
    QString new_radar_system_info_file_name =
        QString(RADAR_SYSTEM_INFO_FOLDER_DIR + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_T_hh_mm_ss_") + RADAR_SYSTEM_INFO_POSTFIX);
    radar_system_info_txt = new QFile(new_radar_system_info_file_name);

    if (radar_system_info_txt->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        is_radar_system_info_txt_opened = true;
        if (radar_system_info_text_stream)
        {
            radar_system_info_text_stream->setDevice(radar_system_info_txt);
        }
        else
        {
            radar_system_info_text_stream = new QTextStream(radar_system_info_txt);
        }
        // S5: 写入抬头
        *radar_system_info_text_stream << "currentDateTime" << '\t' << "pkt_num" << '\t' << "current_radar_mode_index" << '\t' << "fre_code" << '\t' << "free_time" << '\t'
                                       << "ATP_parity" << '\t' << "T_AD_FPGA1" << '\t' << "T_AD_FPGA2" << '\t' << "beam_ctrl_12v_current" << '\t' << "battery_T" << '\t'
                                       << "target_num" << '\t' << "T_QDR_FPGA1" << '\t' << "T_QDR_FPGA2" << '\t' << "T_QDR_FPGA3" << '\t' << "T_QDR_FPGA4" << '\t' << "ATP_azim_acc"
                                       << '\t' << "biss_angle_deg" << '\t' << "in_current_28v" << '\t' << "output_power_1" << '\t' << "output_power_2" << '\t' << "radar_set_mode"
                                       << '\t' << "track_azim_set" << '\t' << "ATP_cyclic_code" << '\t' << "fre_12v_current" << '\t' << "last_GMAC_count" << '\t'
                                       << "antenna_elev_deg" << '\t' << "bank_12v_current" << '\t' << "bank_mode_report" << '\t' << "recv_12v_current" << '\t'
                                       << "ATP_azim_position" << '\t' << '\t' << "antenna_work_mode" << '\t' << "current_GMAC_count" << '\t' << "launch_28v_current" << '\t'
                                       << "radar_mission_mode" << '\t' << "antenna_data_parity" << '\t' << "radar_state_register" << '\t' << "current_transmit_azim_deg" << '\t'
                                       << "current_azim_deg" << '\t' << "total_lost_packets_count" << '\t' << "turntable_rotating_speed" << '\t' << "antenna_temperature_warning"
                                       << '\t' << "total_lost_azim_sectors_num" << '\t' << "T_transmitter1" << '\t' << "T_transmitter2" << '\t' << "T_recv1" << '\t' << "T_recv2"
                                       << '\t' << "T_recv3" << '\t' << "T_recv4" << '\n';
    }
}

void RadarDataProcessingObject::createNewRadarOriginalPointsFile()
{
    // S1: 如果目录不存在, 则创建这个目录
    QDir radar_original_points_dir;
    if (!radar_original_points_dir.exists(RADAR_ORIGINAL_POINTS_FOLDER_DIR))
    {
        radar_original_points_dir.mkpath(RADAR_ORIGINAL_POINTS_FOLDER_DIR);
    }
    radar_original_points_dir.setPath(RADAR_ORIGINAL_POINTS_FOLDER_DIR);
    // S2: 读取所有的txt
    QStringList radar_original_points_file_filter;
    radar_original_points_file_filter << "*.txt";
    QFileInfoList radar_original_points_file_list = radar_original_points_dir.entryInfoList(radar_original_points_file_filter, QDir::Files, QDir::Name);
    // S3: 如果文件数量超过 MAX_RADAR_ORIGINAL_POINTS_FILE_NUM, 删除最老的
    while (radar_original_points_file_list.size() >= MAX_RADAR_ORIGINAL_POINTS_FILE_NUM)
    {
        radar_original_points_dir.remove(radar_original_points_file_list.first().fileName());
        radar_original_points_file_list.pop_front();
    }
    // S4: 新建文件并打开
    // 关闭之前的文件
    if (radar_original_points_txt)
    {
        if (radar_original_points_text_stream)
        {
            radar_original_points_text_stream->flush();
        }
        radar_original_points_txt->close();
        delete radar_original_points_txt;
        is_radar_original_points_txt_opened = false;
    }
    // 新建一个文件
    QString new_radar_original_points_file_name =
        QString(RADAR_ORIGINAL_POINTS_FOLDER_DIR + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_T_hh_mm_ss_") + RADAR_ORIGINAL_POINTS_POSTFIX);
    radar_original_points_txt = new QFile(new_radar_original_points_file_name);

    if (radar_original_points_txt->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        is_radar_original_points_txt_opened = true;
        if (radar_original_points_text_stream)
        {
            radar_original_points_text_stream->setDevice(radar_original_points_txt);
        }
        else
        {
            radar_original_points_text_stream = new QTextStream(radar_original_points_txt);
        }
        // S5: 写入抬头
        *radar_original_points_text_stream << "date_time" << '\t' << "pkt_num" << '\t' << "current_azim_deg" << '\t' << "radar_mode" << '\t' << "range_index" << '\t'
                                           << "azim_index" << '\t' << "elev_index" << '\t' << "doppler_index" << '\t' << "range" << '\t' << "azim_deg" << '\t' << "elev_deg" << '\t'
                                           << "vr" << '\t' << "amp_sigma" << '\t' << "amp_delta_azim" << '\t' << "amp_delta_elev" << '\t' << "amp_protect" << '\t' << "sigma_I"
                                           << '\t' << "sigma_Q" << '\t' << "delta_azim_I" << '\t' << "delta_azim_Q" << '\t' << "delta_elev1_I" << '\t' << "delta_elev1_Q" << '\t'
                                           << "delta_elev2_I" << '\t' << "delta_elev2_Q" << '\t' << "snr" << '\t' << "cfar_neighbour_num" << '\t' << "rcs" << '\t' << "free_time"
                                           << '\t' << "is_filtered" << '\n';
    }
}

void RadarDataProcessingObject::createNewRadarTrackPointsFile()
{
    // S1: 如果目录不存在, 则创建这个目录
    QDir radar_track_points_dir;
    if (!radar_track_points_dir.exists(RADAR_TRACK_POINTS_FOLDER_DIR))
    {
        radar_track_points_dir.mkpath(RADAR_TRACK_POINTS_FOLDER_DIR);
    }
    radar_track_points_dir.setPath(RADAR_TRACK_POINTS_FOLDER_DIR);
    // S2: 读取所有的txt
    QStringList radar_track_points_file_filter;
    radar_track_points_file_filter << "*.txt";
    QFileInfoList radar_track_points_file_list = radar_track_points_dir.entryInfoList(radar_track_points_file_filter, QDir::Files, QDir::Name);
    // S3: 如果文件数量超过MAX_RADAR_TRACK_POINTS_FILE_NUM, 删除最老的
    while (radar_track_points_file_list.size() >= MAX_RADAR_TRACK_POINTS_FILE_NUM)
    {
        radar_track_points_dir.remove(radar_track_points_file_list.first().fileName());
        radar_track_points_file_list.pop_front();
    }
    // S4: 新建文件并打开
    // 关闭之前的文件
    if (radar_track_points_txt)
    {
        if (radar_track_points_text_stream)
        {
            radar_track_points_text_stream->flush();
        }
        radar_track_points_txt->close();
        delete radar_track_points_txt;
        is_radar_track_points_txt_opened = false;
    }
    // 新建一个文件
    QString new_radar_track_points_file_name =
        QString(RADAR_TRACK_POINTS_FOLDER_DIR + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_T_hh_mm_ss_") + RADAR_TRACK_POINTS_POSTFIX);
    radar_track_points_txt = new QFile(new_radar_track_points_file_name);

    if (radar_track_points_txt->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        is_radar_track_points_txt_opened = true;
        if (radar_track_points_text_stream)
        {
            radar_track_points_text_stream->setDevice(radar_track_points_txt);
        }
        else
        {
            radar_track_points_text_stream = new QTextStream(radar_track_points_txt);
        }
        // S5: 写入抬头
        *radar_track_points_text_stream << "date_time" << '\t' << "track_id" << '\t' << "radar_mode" << '\t' << "track_times" << '\t' << "lost_times" << '\t' << "azim_index"
                                        << '\t' << "elev_index" << '\t' << "range" << '\t' << "azim_deg" << '\t' << "elev_deg" << '\t' << "vr" << '\t' << "amp_sigma" << '\t'
                                        << "amp_delta_azim" << '\t' << "amp_delta_elev1" << '\t' << "amp_delta_elev1" << '\t' << "snr" << '\t' << "cfar_neighbour_num" << '\t'
                                        << "rcs" << '\t' << "x" << '\t' << "y" << '\t' << "z" << '\t' << "vx" << '\t' << "vy" << '\t' << "vz" << '\t' << "longitude" << '\t'
                                        << "latitude" << '\t' << "altitude" << '\t' << "free_time" << '\t' << "is_correlated" << '\t' << "total_lost_times" << '\t'
                                        << "effective_track_size" << '\t' << "established_mode" << '\n';
    }
}

void RadarDataProcessingObject::outputRadarSystemStateToText(const RadarSystemState& radar_system_state)
{
    if (is_radar_system_info_txt_opened)
    {
        *radar_system_info_text_stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << '\t' << radar_system_state.pkt_num << '\t'
                                       << radar_system_state.current_radar_mode_index << '\t' << radar_system_state.fre_code << '\t' << radar_system_state.current_free_time << '\t'
                                       << radar_system_state.ATP_parity << '\t' << radar_system_state.T_AD_FPGA1 << '\t' << radar_system_state.T_AD_FPGA2 << '\t'
                                       << radar_system_state.beam_ctrl_12v_current << '\t' << radar_system_state.battery_T << '\t' << radar_system_state.target_num << '\t'
                                       << radar_system_state.T_QDR_FPGA1 << '\t' << radar_system_state.T_QDR_FPGA2 << '\t' << radar_system_state.T_QDR_FPGA3 << '\t'
                                       << radar_system_state.T_QDR_FPGA4 << '\t' << radar_system_state.ATP_azim_acc << '\t' << radar_system_state.biss_angle_deg << '\t'
                                       << radar_system_state.in_current_28v << '\t' << radar_system_state.output_power_1 << '\t' << radar_system_state.output_power_2 << '\t'
                                       << radar_system_state.radar_set_mode << '\t' << radar_system_state.track_azim_set << '\t' << radar_system_state.ATP_cyclic_code << '\t'
                                       << radar_system_state.fre_12v_current << '\t' << radar_system_state.last_GMAC_count << '\t' << radar_system_state.current_elev_deg << '\t'
                                       << radar_system_state.bank_12v_current << '\t' << radar_system_state.bank_mode_report << '\t' << radar_system_state.recv_12v_current << '\t'
                                       << radar_system_state.ATP_azim_position << '\t' << '\t' << radar_system_state.antenna_work_mode << '\t'
                                       << radar_system_state.current_GMAC_count << '\t' << radar_system_state.launch_28v_current << '\t' << radar_system_state.radar_mission_mode
                                       << '\t' << radar_system_state.antenna_data_parity << '\t' << radar_system_state.radar_state_register << '\t'
                                       << radar_system_state.current_transmit_azim_center << '\t' << radar_system_state.current_transmit_azim_deg << '\t'
                                       << radar_system_state.total_lost_packets_count << '\t' << radar_system_state.turntable_rotating_speed << '\t'
                                       << radar_system_state.antenna_temperature_warning << '\t' << radar_system_state.total_lost_azim_sectors_num << '\t'
                                       << radar_system_state.T_transmitter1 << '\t' << radar_system_state.T_transmitter2 << '\t' << radar_system_state.T_recv1 << '\t'
                                       << radar_system_state.T_recv2 << '\t' << radar_system_state.T_recv3 << '\t' << radar_system_state.T_recv4 << '\n';
        ++current_radar_system_info_row_num;
        if (current_radar_system_info_row_num % MAX_ITEM_NUM_FLUSH == 0)
        {
            radar_system_info_text_stream->flush();
        }
        if (current_radar_system_info_row_num >= MAX_RADAR_SYSTEM_INFO_NUM_PER_FILE)
        {
            createNewRadarSystemInfoFile();
            current_radar_system_info_row_num = 0;
        }
    }
}

void RadarDataProcessingObject::outputRadarOriginalPointToText(const TargetOriginalPoint& target_original_point, const RadarSystemState& radar_system_state)
{
    // TODO 添加其他方位角度输出
    if (is_radar_original_points_txt_opened)
    {
        *radar_original_points_text_stream << target_original_point.date_time.toString(Qt::ISODateWithMs) << '\t' << radar_system_state.pkt_num << '\t'
                                           << radar_system_state.current_transmit_azim_deg << '\t' << target_original_point.radar_mode << '\t' << target_original_point.range_index
                                           << '\t' << target_original_point.azim_index << '\t' << target_original_point.elev_index << '\t' << target_original_point.doppler_index
                                           << '\t' << target_original_point.range << '\t' << target_original_point.azim_deg << '\t' << target_original_point.elev_deg << '\t'
                                           << target_original_point.vr << '\t' << target_original_point.amp_sigma << '\t' << target_original_point.amp_delta_azim << '\t'
                                           << target_original_point.amp_delta_elev1 << '\t' << target_original_point.amp_delta_elev2 << '\t' << target_original_point.sigma_I
                                           << '\t' << target_original_point.sigma_Q << '\t' << target_original_point.delta_azim_I << '\t' << target_original_point.delta_azim_Q
                                           << '\t' << target_original_point.delta_elev1_I << '\t' << target_original_point.delta_elev1_Q << '\t'
                                           << target_original_point.delta_elev2_I << '\t' << target_original_point.delta_elev2_Q << '\t' << target_original_point.snr << '\t'
                                           << target_original_point.cfar_neighbour_num << '\t' << target_original_point.rcs << '\t' << target_original_point.free_time << '\t'
                                           << target_original_point.is_filtered << '\n';

        ++current_radar_original_points_row_num;
        if (current_radar_original_points_row_num % MAX_ITEM_NUM_FLUSH == 0)
        {
            radar_original_points_text_stream->flush();
        }
        if (current_radar_original_points_row_num >= MAX_RADAR_ORIGINAL_POINTS_NUM_PER_FILE)
        {
            createNewRadarOriginalPointsFile();
            current_radar_original_points_row_num = 0;
        }
    }
}

void RadarDataProcessingObject::outputLastTrackPointToText(const TargetTrack* target_track)
{
    if (is_radar_track_points_txt_opened)
    {
        *radar_track_points_text_stream << target_track->target_track_points_list.last().date_time.toString(Qt::ISODateWithMs) << '\t'
                                        << target_track->target_track_points_list.last().track_id << '\t' << target_track->target_track_points_list.last().radar_mode << '\t'
                                        << target_track->target_track_points_list.last().track_times << '\t' << target_track->target_track_points_list.last().lost_times << '\t'
                                        << target_track->target_track_points_list.last().azim_index << '\t' << target_track->target_track_points_list.last().elev_index << '\t'
                                        << target_track->target_track_points_list.last().range << '\t' << target_track->target_track_points_list.last().azim_deg << '\t'
                                        << target_track->target_track_points_list.last().elev_deg << '\t' << target_track->target_track_points_list.last().vr << '\t'
                                        << target_track->target_track_points_list.last().amp_sigma << '\t' << target_track->target_track_points_list.last().amp_delta_azim << '\t'
                                        << target_track->target_track_points_list.last().amp_delta_elev1 << '\t' << target_track->target_track_points_list.last().amp_delta_elev2
                                        << '\t' << target_track->target_track_points_list.last().snr << '\t' << target_track->target_track_points_list.last().cfar_neighbour_num
                                        << '\t' << target_track->target_track_points_list.last().rcs << '\t' << target_track->target_track_points_list.last().x << '\t'
                                        << target_track->target_track_points_list.last().y << '\t' << target_track->target_track_points_list.last().z << '\t'
                                        << target_track->target_track_points_list.last().vx << '\t' << target_track->target_track_points_list.last().vy << '\t'
                                        << target_track->target_track_points_list.last().vz << '\t'
                                        << QString::number(target_track->target_track_points_list.last().longitude, 'f', 6) << '\t'
                                        << QString::number(target_track->target_track_points_list.last().latitude, 'f', 6) << '\t'
                                        << target_track->target_track_points_list.last().altitude << '\t' << target_track->target_track_points_list.last().free_time << '\t'
                                        << target_track->target_track_points_list.last().is_correlated << '\t' << target_track->total_lost_times << '\t'
                                        << target_track->effective_track_size << '\t' << target_track->target_track_points_list.last().established_mode << '\n';
        ++current_radar_track_points_row_num;
        if (current_radar_track_points_row_num % MAX_ITEM_NUM_FLUSH == 0)
        {
            radar_track_points_text_stream->flush();
        }
        if (current_radar_track_points_row_num >= MAX_RADAR_TRACK_POINTS_NUM_PER_FILE)
        {
            createNewRadarTrackPointsFile();
            current_radar_track_points_row_num = 0;
        }
    }
}

void RadarDataProcessingObject::sltUpdateRadarSystemStateInOtherThreads()
{
    ++update_radar_system_state_timer_count;
    if (update_radar_system_state_timer_count >= MAX_UPDATE_RADAR_SYSTEM_STATE_TIMER_COUNT)
    {
        // 控制自动复位、自动发送校准系数
        if (local_software_settings.system_adjust_settings.auto_adjust_signal_processer_flag)
        {
            if (serdes_error_flag)
            {
                emit sigSendResetOrderToFpgaSerdesError();
                serdes_error_flag = false;
            }

            if (none_dbf_coef_in_fpga_flag)
            {
                emit sigSendDbfCorrectCoefFileToFpgaWhenNoneCoef();
                none_dbf_coef_in_fpga_flag = false;
            }
        }

        // 更新雷达连接状态
        if (!need_to_update_radar_system_state_in_other_thread_flag)
        {
            local_radar_system_state.radar_connected_flag = 0;
            emit(sigUpdateRadarSystemStateInOtherThreads(local_radar_system_state));
            local_radar_system_state.error_lost_packets_count = 0;
            local_radar_system_state.error_lost_azim_sectors_num = 0;
            memset(&local_radar_system_state.error_info_flag_list, 0x00, sizeof(local_radar_system_state.error_info_flag_list));
        }
        update_radar_system_state_timer_count = 0;
    }

    if (need_to_update_radar_system_state_in_other_thread_flag)
    {
        emit(sigUpdateRadarSystemStateInOtherThreads(local_radar_system_state));
        need_to_update_radar_system_state_in_other_thread_flag = false;
        local_radar_system_state.error_lost_packets_count = 0;
        local_radar_system_state.error_lost_azim_sectors_num = 0;
        memset(&local_radar_system_state.error_info_flag_list, 0x00, sizeof(local_radar_system_state.error_info_flag_list));
    }
}

void RadarDataProcessingObject::sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings)
{
    qDebug() << tr("[RadarDataProcessingObject]: sltUpdateLocalHardwareSettings! ");
    memcpy(&local_hardware_settings, &new_hardware_settings, sizeof(HardwareSettings));
}

void RadarDataProcessingObject::sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings)
{
    qDebug() << tr("[RadarDataProcessingObject]: sltUpdateLocalSoftwareSettings! ");
    memcpy(&local_software_settings, &new_software_settings, sizeof(SoftwareSettings));
}

void RadarDataProcessingObject::sltUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings)
{
    qDebug() << tr("[RadarDataProcessingObject]: sltUpdateLocalDataProcessingSettings! ");
    memcpy(&local_data_processing_settings, &new_data_processing_settings, sizeof(DataProcessingSettings));
}

void RadarDataProcessingObject::sltProcessingNextFpgaPacket(FpgaPacket* received_fpga_packet)
{
    QMutexLocker locker(&g_mutex_for_g_original_point_item_list_and_g_target_track_item_list);
    //    qDebug() << tr("[RadarDataProcessingObject]: sltProcessingNextFpgaPackets! size = ") << received_fpga_packet_list.size();
    if (received_fpga_packet)
    {
        if (start_tracking_flag)
        {
            // 跟踪模式
            // S1:先清空搜索模式的原始点迹和除了跟踪航迹之外的航迹
            if (!original_points_and_tracks_scan_mode_cleared_up_flag)
            {
                qDebug() << QString("[Track]: Change to track mode, clear up original points and tracks in scan mode!");
                clearUpTracksScanModeInAzimSectors();         // 先清理航迹
                clearUpOriginalPointsScanModeInAzimSectors(); // 再清理原始点迹
                original_points_and_tracks_scan_mode_cleared_up_flag = true;
                original_points_and_tracks_track_mode_cleared_up_flag = false;
                // 删除可能没有处理的报文
                if (current_packet)
                {
                    packetHandlerTrackMode(current_packet);
                    delete current_packet;
                    current_packet = nullptr;
                }
                if (next_packet)
                {
                    packetHandlerTrackMode(next_packet);
                    delete next_packet;
                    next_packet = nullptr;
                }
                initializeRadarDataProcessingTrackModeParameters();
            }

            // S2:解析数据报文，提取系统状态和原始点迹
            current_packet_track_mode = received_fpga_packet;

            packetHandlerTrackMode(current_packet_track_mode);
            delete current_packet_track_mode;
            current_packet_track_mode = nullptr;

            // S3:点迹融合, 航迹相关, 航迹更新
            if (original_points_batch_track_mode_ready_flag)
            {
                if (tracking_target_track)
                {
                    original_points_batch_track_mode.combineCurrentOriginalPointsBatch(local_data_processing_settings);
                    tracking_target_track->precorrelateWithOriginalPointListTrackMode(original_points_batch_track_mode.target_original_points_list, local_data_processing_settings);
                    tracking_target_track->updateTrackWithMeasureOrPredictTrackMode(original_points_batch_track_mode.current_free_time, local_data_processing_settings);

                    if (needingUpdateTrackingTargetPosition(last_tracking_target_position, tracking_target_track))
                    {
                        updateTrackingTargetPosition(last_tracking_target_position, tracking_target_track);
                        if (last_tracking_target_position.hardware_settings_needed_to_update_flag)
                        {
                            updateHardwareSettingsTrackModeBasedOnTrackingTargetPosition(last_tracking_target_position,
                                                                                         local_hardware_settings_track_mode,
                                                                                         local_data_processing_settings);
                            emit sigSendNewHardwareSettingsTrackModeToFpga(local_hardware_settings_track_mode);
                            last_tracking_target_position.hardware_settings_needed_to_update_flag = false;
                        }
                    }

                    // 界面航迹跟踪3次更新一次
                    if (tracking_target_track->effective_track_size % 3 == 0)
                    {
                        if (target_track_to_item_hashmap.contains(tracking_target_track))
                        {
                            target_track_to_item_hashmap.value(tracking_target_track)->updateTargetTrackLastPoint(tracking_target_track);
                            g_target_track_item_to_update_list.append(target_track_to_item_hashmap.value(tracking_target_track));
                        }
                        if (g_target_track_item_to_update_list.size() > 0)
                        {
                            emit(sigUpdateTargetTrackItems());
                        }
                    }

                    // 发送航迹
                    track_to_send_list = new QList<TargetTrackPoint>;
                    track_to_send_list->clear();
                    track_to_send_list->append(tracking_target_track->target_track_points_list.last());
                    emit sigSendTrackLastPointListToController(tracking_target_track->target_track_points_list.last().azim_index, track_to_send_list);
                    track_to_send_list = nullptr;

                    // 输出航迹
                    if (local_software_settings.output_file_settings.output_radar_track_points_flag)
                    {
                        outputLastTrackPointToText(tracking_target_track);
                    }
                }

                // 清空原始点
                original_points_batch_track_mode.target_original_points_list.clear();
                original_points_batch_track_mode_ready_flag = false;

                if (tracking_target_track->continuous_lost_times >= MAX_LOST_TIMES_TRACK_MODE)
                {
                    prepare_to_quit_tracking_flag = true;
                }
            }

            // S4: 如果需要退出跟踪，则进行跟踪转搜索的模式设置
            if (prepare_to_quit_tracking_flag)
            {
                qDebug() << QString(QDateTime::currentDateTime().toString() + " 退出跟踪转搜索!");
                updateTrackingTargetPosition(last_tracking_target_position, tracking_target_track);
                updateHardwareSettingsWhenQuitTrackModeBasedOnTrackingTargetPosition(last_tracking_target_position, local_hardware_settings_track_mode);
                start_tracking_flag = false;
                prepare_to_quit_tracking_flag = false;
                emit sigQuitTrackModeToScanMode(local_hardware_settings_track_mode);
                emit sigUpdateDataProcessingMode(RADAR_SCAN_MODE);
                emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() +
                                                    QString(" Quit tracking %1 and change to SCAN mode!").arg(tracking_target_track_id));
            }
        }
        else
        {
            if (!original_points_and_tracks_track_mode_cleared_up_flag)
            {
                // 这里表示刚由跟踪模式转入搜索模式，需要清空跟踪模式的原始点迹及航迹，并且初始化搜索模式的扇区号等参数
                qDebug() << QString("[Scan]: Change to scan mode, clear up original points and tracks in track mode");
                clearUpTracksTrackMode();                              // 先清理航迹
                clearUpOriginalPointsTrackModeInOriginalPointsBatch(); // 再清理原始点迹
                original_points_and_tracks_track_mode_cleared_up_flag = true;
                original_points_and_tracks_scan_mode_cleared_up_flag = false;

                initializeDataProcessingAzimSectorIndexScamMode();
                initializeParametersForSmoothingAzimScanMode();
            }

            // 搜索模式
            // S0: 读取报文, 并做方位平滑去除毛刺
            // 为了做方位平滑消除毛刺, 需要等待一包, 即下一包到了后, 根据前一包和后一包对当前包做平滑
            current_packet = next_packet;
            next_packet = received_fpga_packet;
            smoothAzimInCurrentFpgaPacket();

            // S1: 解析报文, 将原始点迹放入对应扇区
            if (current_packet)
            {
                packetHandlerScanMode(current_packet);
                delete current_packet;
                current_packet = nullptr;

                int delta_azim_index =
                    circularlyAddInt(azim_sector_list_scan_mode.scan_prepare_combine_index, -azim_sector_list_scan_mode.scan_current_input_index, 0, AZIM_SECTOR_NUM);

                //                        qDebug() << "delta_azim_index: " << delta_azim_index;
                while (delta_azim_index > 0)
                {
                    // S2: 更新融合扇区到航迹起始扇区的区间范围
                    azim_sector_list_scan_mode.updateCombineAndPrecorrelateAndTrackAndUpdateAndTrackStartIndex(-1);

                    // S3: 融合
                    // scan_true_combine_index 与之前 COMBINE_NEIGHBOUR_SECTORS_NUM 个扇区融合
                    azim_sector_list_scan_mode.combineCurrentSectorWithPreviousSectors(local_data_processing_settings);

                    // S4: 航迹预相关
                    // 此时 scan_true_precorrelate_index 扇区的点迹已经与左右 COMBINE_NEIGHBOUR_SECTORS_NUM 个扇区点迹做完融合了
                    // 该扇区的所有航迹与左中右扇区的点迹相关, 将可以相关上的点放入该航迹的map
                    azim_sector_list_scan_mode.precorrelateTracksWithOriginalPoints(local_data_processing_settings);

                    // S5: 航迹更新
                    // 1.对该扇区的每个航迹而言, 遍历map找到匹配的点, 如果没有找到, 则外推一个点；
                    // 2.对丢失的航迹进行删除, 并且删除对应的航迹 item；
                    // 3.判断这个航迹的最新点迹是不是还应该在这个扇区, 如果不是, 推进对应扇区, 并删除原先扇区的此航迹;
                    track_to_send_list = new QList<TargetTrackPoint>;
                    track_to_send_list->clear();
                    azim_sector_list_scan_mode.updateTracks(local_data_processing_settings);
                    updateTrackAzimSectorIndexAndTrackItemsAndTrackToSendListAfterTracksUpdated();

                    // S6: 航迹起始
                    startTrackAndGenerateNewTrackItemAndUpdateTrackToSendList();

                    // S7: 发送航迹最新点队列给与指控通信线程, 用于发送报文
                    if (track_to_send_list->size() > 0)
                    {
                        emit sigSendTrackLastPointListToController(azim_sector_list_scan_mode.scan_current_track_update_index, track_to_send_list);
                    }
                    else
                    {
                        delete track_to_send_list;
                        track_to_send_list = nullptr;
                    }

                    // S8: 发送更新航迹的信号给主界面, 发送航迹（发送完要记得清除队列）
                    emit sigUpdateTargetTrackItems();

                    --delta_azim_index;
                }
            }
        }
    }
}

void RadarDataProcessingObject::sltAddTrackIdToSendingTargetSet(const int& new_track_id)
{
    qDebug() << tr("[RadarDataProcessingObject]: sltAddTrackIdToSendingTargetSet! new_track_id = ") << new_track_id;
    sending_target_set.insert(new_track_id);
}

void RadarDataProcessingObject::sltPrepareToEnterTrackMode(const int& new_track_id)
{
    qDebug() << tr("[RadarDataProcessingObject]: sltPrepareToEnterTrackMode! new_track_id = ") << new_track_id;
    if ((!start_tracking_flag) && (!prepare_tracking_flag))
    {
        // 如果收到命令要进入跟踪模式：
        // S1: 遍历所有航迹确认跟踪的航迹是否存在
        // S2: 如果存在, 则初始化跟踪模式的初始位置信息, 并且修改准备进入跟踪标志位
        bool is_tracking_target_track_id_exist = false;
        for (int ii = 0; ii < azim_sector_list_scan_mode.azim_sector_list.size(); ++ii)
        {
            for (QList<TargetTrack*>::iterator tt_it = azim_sector_list_scan_mode.azim_sector_list[ii].target_tracks_list.begin();
                 tt_it != azim_sector_list_scan_mode.azim_sector_list[ii].target_tracks_list.end();)
            {
                if ((*tt_it)->track_id == new_track_id)
                {
                    is_tracking_target_track_id_exist = true;
                    tracking_target_track = *tt_it;
                    tracking_target_track_id = new_track_id;
                    initializeTrackingTargetPositionWithTrackConsiderCurrentAzim(last_tracking_target_position, tracking_target_track, local_radar_system_state);
                    updateHardwareSettingsTrackModeBasedOnTrackingTargetPosition(last_tracking_target_position, local_hardware_settings_track_mode, local_data_processing_settings);
                    prepare_tracking_flag = true;
                    emit sigUpdateDataProcessingMode(RADAR_TRACK_MODE);
                    emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + QString(" Prepare to track %1!").arg(tracking_target_track_id));
                    qDebug() << "【收到跟踪命令】: ID 为" << tracking_target_track->track_id << "! 命令中的航迹存在, 初始化跟踪信息完毕, 准备跟踪!";
                    break;
                }
                ++tt_it;
            }
            if (is_tracking_target_track_id_exist)
            {
                break;
            }
        }
    }
}

void RadarDataProcessingObject::sltClickStopToQuitTrackMode()
{
    qDebug() << "[RadarDataProcessingObject]: sltClickStopToQuitTrackMode!";
    if (prepare_tracking_flag)
    {
        emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + QString(" Quit tracking %1!").arg(tracking_target_track_id));
        // 如果进入准备跟踪状态，表示 tracking_target_track 和 tracking_target_track_id 已经初始化
        tracking_target_track = nullptr;
        tracking_target_track_id = -1;
        initializeTrackingTargetPosition(last_tracking_target_position);
        initializeHardwareSettingsTrackMode(local_hardware_settings_track_mode);
        prepare_tracking_flag = false;
    }
    else if (start_tracking_flag)
    {
        emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + QString(" Prepare to quit tracking %1!").arg(tracking_target_track_id));
        prepare_to_quit_tracking_flag = true;
    }
}

RadarDataProcessingThread::RadarDataProcessingThread(QObject* parent) : QObject(parent)
{
    qDebug() << tr("[RadarDataProcessingThread]: 进入构造函数!");
    radar_data_processing_object = new RadarDataProcessingObject;
    radar_data_processing_object->moveToThread(&working_thread);
    connect(&working_thread, &QThread::finished, radar_data_processing_object, &QObject::deleteLater);
    connect(&working_thread, &QThread::finished, &working_thread, &QObject::deleteLater);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigUpdateRadarSystemStateInOtherThreads, this, &RadarDataProcessingThread::sigUpdateRadarSystemState);

    connect(radar_data_processing_object, &RadarDataProcessingObject::sigSendPowerOffOrderToFpga, this, &RadarDataProcessingThread::sigSendPowerOffOrderToFpga);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigSendPowerOnOrderToFpga, this, &RadarDataProcessingThread::sigSendPowerOnOrderToFpga);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigSendResetOrderToFpgaSerdesError, this, &RadarDataProcessingThread::sigSendResetOrderToFpgaSerdesError);
    connect(radar_data_processing_object,
            &RadarDataProcessingObject::sigSendDbfCorrectCoefFileToFpgaWhenNoneCoef,
            this,
            &RadarDataProcessingThread::sigSendDbfCorrectCoefFileToFpgaWhenNoneCoef);

    connect(radar_data_processing_object, &RadarDataProcessingObject::sigRemoveTrackIdFromSendingTargetSet, this, &RadarDataProcessingThread::sigRemoveTrackIdFromSendingTargetSet);
    connect(radar_data_processing_object,
            &RadarDataProcessingObject::sigSendTrackLastPointListToController,
            this,
            &RadarDataProcessingThread::sigSendTrackLastPointListToController);

    connect(this, &RadarDataProcessingThread::sigUpdateLocalHardwareSettings, radar_data_processing_object, &RadarDataProcessingObject::sltUpdateLocalHardwareSettings);
    connect(this, &RadarDataProcessingThread::sigUpdateLocalSoftwareSettings, radar_data_processing_object, &RadarDataProcessingObject::sltUpdateLocalSoftwareSettings);
    connect(this, &RadarDataProcessingThread::sigUpdateLocalDataProcessingSettings, radar_data_processing_object, &RadarDataProcessingObject::sltUpdateLocalDataProcessingSettings);

    connect(this, &RadarDataProcessingThread::sigAddTrackIdToSendingTargetSet, radar_data_processing_object, &RadarDataProcessingObject::sltAddTrackIdToSendingTargetSet);
    connect(this, &RadarDataProcessingThread::sigPrepareToEnterTrackMode, radar_data_processing_object, &RadarDataProcessingObject::sltPrepareToEnterTrackMode);

    connect(this, &RadarDataProcessingThread::sigProcessingNextFpgaPacket, radar_data_processing_object, &RadarDataProcessingObject::sltProcessingNextFpgaPacket);

    connect(radar_data_processing_object, &RadarDataProcessingObject::sigUpdateStatusBarOnMainWindow, this, &RadarDataProcessingThread::sigUpdateStatusBarOnMainWindow);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigUpdateOriginalPointItems, this, &RadarDataProcessingThread::sigUpdateOriginalPointItems);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigUpdateTargetTrackItems, this, &RadarDataProcessingThread::sigUpdateTargetTrackItems);

    connect(this, &RadarDataProcessingThread::sigClickStopToQuitTrackMode, radar_data_processing_object, &RadarDataProcessingObject::sltClickStopToQuitTrackMode);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigUpdateDataProcessingMode, this, &RadarDataProcessingThread::sigUpdateDataProcessingMode);
    connect(radar_data_processing_object,
            &RadarDataProcessingObject::sigSendStopPointingAndTrackOrderToEnterTrackMode,
            this,
            &RadarDataProcessingThread::sigSendStopPointingAndTrackOrderToEnterTrackMode);
    connect(radar_data_processing_object,
            &RadarDataProcessingObject::sigSendNewHardwareSettingsTrackModeToFpga,
            this,
            &RadarDataProcessingThread::sigSendNewHardwareSettingsTrackModeToFpga);
    connect(radar_data_processing_object, &RadarDataProcessingObject::sigQuitTrackModeToScanMode, this, &RadarDataProcessingThread::sigQuitTrackModeToScanMode);

    working_thread.start();
}

RadarDataProcessingThread::~RadarDataProcessingThread()
{
    qDebug() << tr("[RadarDataProcessingThread]: 进入析构函数!");
}

void RadarDataProcessingThread::startWorking()
{
    qDebug() << tr("[RadarDataProcessingThread]: startWorking!");
}

void RadarDataProcessingThread::stopWorking()
{
    qDebug() << tr("[RadarDataProcessingThread]: stopWorking!");
    working_thread.quit();
    working_thread.wait();
}
