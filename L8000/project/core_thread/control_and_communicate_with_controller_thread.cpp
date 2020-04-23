#include "control_and_communicate_with_controller_thread.h"
#include "global_functions.h"

ControlAndCommunicateWithControllerObject::ControlAndCommunicateWithControllerObject(QObject* parent) : QObject(parent)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: 进入构造函数!");

    initializeRadarSystemState(local_radar_system_state);
    radar_system_info_timer = new QTimer(this);
    radar_system_info_timer->setInterval(RADAR_SYSTEM_INFO_TIMER_INTERVAL);

    sending_target_set.clear();

    udp_socket = new QUdpSocket(this);
    settings_in_config = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat, this);
    initializeHardwareAndSoftwareAndDataProcessingSettingsBasedOnConfigFile(local_hardware_settings,
                                                                            local_software_settings,
                                                                            local_dbf_correct_coef_file_dir_settings,
                                                                            local_data_processing_settings,
                                                                            settings_in_config);
    if (udp_socket->bind(QHostAddress::AnyIPv4, local_software_settings.communication_settings.input_port))
    {
        qDebug() << tr("[ControlAndCommunicateWithControllerObject]: udp_socket bind sucessfully! port = ")
                 << QString::number(local_software_settings.communication_settings.input_port);
    }
    else
    {
        qDebug() << tr("[ControlAndCommunicateWithControllerObject]: udp_socket bind failed! port = ")
                 << QString::number(local_software_settings.communication_settings.input_port);
    }

    connect(radar_system_info_timer, &QTimer::timeout, this, &ControlAndCommunicateWithControllerObject::sltSendSystemInfoPacketToController);
    radar_system_info_timer->start();
}

ControlAndCommunicateWithControllerObject::~ControlAndCommunicateWithControllerObject()
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: 进入析构函数!");
}

void ControlAndCommunicateWithControllerObject::initializeHardwareAndSoftwareAndDataProcessingSettingsInOtherThreads()
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: initializeHardwareAndSoftwareAndDataProcessingSettingsInOtherModules!");
    emit sigInitializeHardwareSettingsInOtherThreads(local_hardware_settings);
    emit sigInitializeSoftwareSettingsInOtherThreads(local_software_settings);
    emit sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads(local_dbf_correct_coef_file_dir_settings);
    emit sigInitializeDataProcessingSettingsInOtherThreads(local_data_processing_settings);
}

void ControlAndCommunicateWithControllerObject::sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: sltUpdateLocalHardwareSettings!");
    memcpy(&local_hardware_settings, &new_hardware_settings, sizeof(HardwareSettings));
    updateHardwareParametersInConfigFileBasedOnHardwareSettings(local_hardware_settings, settings_in_config);
}

void ControlAndCommunicateWithControllerObject::sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: sltUpdateLocalSoftwareSettings!");
    if (local_software_settings.communication_settings.input_port != new_software_settings.communication_settings.input_port)
    {
        udp_socket->close();
        if (udp_socket->bind(QHostAddress::AnyIPv4, new_software_settings.communication_settings.input_port))
        {
            qDebug() << tr("[ControlAndCommunicateWithControllerObject]: udp_socket bind another port sucessfully! port = ")
                     << QString::number(new_software_settings.communication_settings.input_port);
        }
        else
        {
            qDebug() << tr("[ControlAndCommunicateWithControllerObject]: udp_socket bind another port failed! port = ")
                     << QString::number(new_software_settings.communication_settings.input_port);
        }
    }
    memcpy(&local_software_settings, &new_software_settings, sizeof(SoftwareSettings));
    updateSoftwareParametersInConfigFileBasedOnSoftwareSettings(local_software_settings, settings_in_config);
}

void ControlAndCommunicateWithControllerObject::sltUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: sltUpdateLocalDbfCorrectCoefFileDirSettings!");
    if (local_dbf_correct_coef_file_dir_settings.master_external_file_dir != new_dbf_correct_coef_file_dir_settings.master_external_file_dir ||
        local_dbf_correct_coef_file_dir_settings.slave_external_file_dir != new_dbf_correct_coef_file_dir_settings.slave_external_file_dir ||
        local_dbf_correct_coef_file_dir_settings.master_internal_file_dir != new_dbf_correct_coef_file_dir_settings.master_internal_file_dir ||
        local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir != new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir)
    {
        local_dbf_correct_coef_file_dir_settings.master_external_file_dir = new_dbf_correct_coef_file_dir_settings.master_external_file_dir;
        local_dbf_correct_coef_file_dir_settings.slave_external_file_dir = new_dbf_correct_coef_file_dir_settings.slave_external_file_dir;
        local_dbf_correct_coef_file_dir_settings.master_internal_file_dir = new_dbf_correct_coef_file_dir_settings.master_internal_file_dir;
        local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir = new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir;
        updateDbfCorrectCoefFileDirParametersInConfigFileBasedOnDbfCorrectCoefFileDirSettings(local_dbf_correct_coef_file_dir_settings, settings_in_config);
    }
}

void ControlAndCommunicateWithControllerObject::sltUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: sltUpdateLocalDataProcessingSettings!");
    memcpy(&local_data_processing_settings, &new_data_processing_settings, sizeof(DataProcessingSettings));
    updateDataProcessingParametersInConfigFileBasedOnDataProcessingSetting(local_data_processing_settings, settings_in_config);
}

void ControlAndCommunicateWithControllerObject::sltAddTrackIdToSendingTargetSet(const int& new_track_id)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: sltAddTrackIdToSendingTargetSet! new_track_id = ") << new_track_id;
    switch (local_software_settings.communication_settings.output_selected_or_all_method)
    {
    default:
        sending_target_set.insert(new_track_id);
        break;
    case 1:
        sending_target_set.clear();
        sending_target_set.insert(new_track_id);
        break;
    }
}

void ControlAndCommunicateWithControllerObject::sltRemoveTrackIdFromSendingTargetSet(const int& new_track_id)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerObject]: sltRemoveTrackIdFromSendingTargetSet! new_track_id = ") << new_track_id;
    sending_target_set.remove(new_track_id);
}

void ControlAndCommunicateWithControllerObject::sltSendTrackLastPointListToController(const int& m_sector_index, QList<TargetTrackPoint>* m_track_to_send_list)
{
    int track_size = m_track_to_send_list->size();
    if (track_size == 0)
    {
        delete m_track_to_send_list;
        return;
    }
    ZardTrackInfoUdpPacket udp_packet;
    udp_packet.track_size = 0;
    udp_packet.sector_index = m_sector_index;
    memset(udp_packet.targets, 0x00, TARGET_INFO_NUM_PER_PACKET * sizeof(ZardTrackInfo));

    // 指控1
    if (local_software_settings.communication_settings.output_tracks_through_adapter_flag)
    {
        if (local_software_settings.communication_settings.output_selected_or_all_method)
        {
            int num = 0;
            for (int ii = 0; ii < track_size; ii++)
            {
                TargetTrackPoint& tp = (*m_track_to_send_list)[ii];
                if (sending_target_set.contains(tp.track_id))
                {
                    ++udp_packet.track_size;
                    udp_packet.targets[num].trackNo = tp.track_id;
                    udp_packet.targets[num].datetime = tp.date_time.toMSecsSinceEpoch();
                    udp_packet.targets[num].range = tp.range;
                    if (local_software_settings.communication_settings.output_azim_north_or_east_flag)
                    {
                        udp_packet.targets[num].azimu = circularlyAddDouble(450, -tp.azim_deg, 0, 360);
                    }
                    else
                    {
                        udp_packet.targets[num].azimu = tp.azim_deg;
                    }
                    udp_packet.targets[num].elev = tp.elev_deg;
                    udp_packet.targets[num].vr = tp.vr;
                    udp_packet.targets[num].amp_sigma = tp.amp_sigma;
                    udp_packet.targets[num].longitude = int(tp.longitude * 1e6);
                    udp_packet.targets[num].latitude = int(tp.latitude * 1e6);
                    udp_packet.targets[num].altitude = tp.altitude;
                    udp_packet.targets[num].ve = tp.vx;
                    udp_packet.targets[num].vn = tp.vy;
                    udp_packet.targets[num].vz = tp.vz;
                    udp_packet.targets[num].x = tp.x;
                    udp_packet.targets[num].y = tp.y;
                    udp_packet.targets[num].z = tp.z;
                    udp_packet.targets[num].correlated = tp.is_correlated;
                    udp_packet.targets[num].track_times = tp.track_times;
                    udp_packet.targets[num].losted_times = tp.lost_times;
                    udp_packet.targets[num].device_id = local_software_settings.communication_settings.output_device_id;
                    num++;

                    if (num == TARGET_INFO_NUM_PER_PACKET)
                    {
                        udp_socket->writeDatagram((char*)(&udp_packet),
                                                  sizeof(ZardTrackInfoUdpPacket),
                                                  QHostAddress(local_software_settings.communication_settings.output_IP_address),
                                                  local_software_settings.communication_settings.output_port);
                        udp_packet.track_size = 0;
                        udp_packet.sector_index = m_sector_index;
                        memset(udp_packet.targets, 0x00, TARGET_INFO_NUM_PER_PACKET * sizeof(ZardTrackInfo));
                        num = 0;
                    }
                }
            }
            if (num > 0)
            {
                udp_socket->writeDatagram((char*)(&udp_packet),
                                          sizeof(ZardTrackInfoUdpPacket),
                                          QHostAddress(local_software_settings.communication_settings.output_IP_address),
                                          local_software_settings.communication_settings.output_port);
            }
        }
        else
        {
            int num = 0;
            for (int jj = 0; jj < track_size; jj++)
            {
                TargetTrackPoint& tp = (*m_track_to_send_list)[jj];
                udp_packet.targets[num].trackNo = tp.track_id;
                udp_packet.targets[num].datetime = tp.date_time.toMSecsSinceEpoch();
                udp_packet.targets[num].range = tp.range;
                if (local_software_settings.communication_settings.output_azim_north_or_east_flag)
                {
                    udp_packet.targets[num].azimu = circularlyAddDouble(450, -tp.azim_deg, 0, 360);
                }
                else
                {
                    udp_packet.targets[num].azimu = tp.azim_deg;
                }
                udp_packet.targets[num].elev = tp.elev_deg;
                udp_packet.targets[num].vr = tp.vr;
                udp_packet.targets[num].amp_sigma = tp.amp_sigma;
                udp_packet.targets[num].longitude = int(tp.longitude * 1e6);
                udp_packet.targets[num].latitude = int(tp.latitude * 1e6);
                udp_packet.targets[num].altitude = tp.altitude;
                udp_packet.targets[num].ve = tp.vx;
                udp_packet.targets[num].vn = tp.vy;
                udp_packet.targets[num].vz = tp.vz;
                udp_packet.targets[num].x = tp.x;
                udp_packet.targets[num].y = tp.y;
                udp_packet.targets[num].z = tp.z;
                udp_packet.targets[num].correlated = tp.is_correlated;
                udp_packet.targets[num].track_times = tp.track_times;
                udp_packet.targets[num].losted_times = tp.lost_times;
                udp_packet.targets[num].device_id = local_software_settings.communication_settings.output_device_id;
                num++;
                if (num == TARGET_INFO_NUM_PER_PACKET)
                {
                    udp_socket->writeDatagram((char*)(&udp_packet),
                                              sizeof(ZardTrackInfoUdpPacket),
                                              QHostAddress(local_software_settings.communication_settings.output_IP_address),
                                              local_software_settings.communication_settings.output_port);
                    udp_packet.track_size = 0;
                    udp_packet.sector_index = m_sector_index;
                    memset(udp_packet.targets, 0x00, TARGET_INFO_NUM_PER_PACKET * sizeof(ZardTrackInfo));
                    num = 0;
                }
            }
            if (num > 0)
            {
                udp_socket->writeDatagram((char*)(&udp_packet),
                                          sizeof(ZardTrackInfoUdpPacket),
                                          QHostAddress(local_software_settings.communication_settings.output_IP_address),
                                          local_software_settings.communication_settings.output_port);
            }
        }
    }

    delete m_track_to_send_list;
}

void ControlAndCommunicateWithControllerObject::sltUpdateLocalRadarSystemState(const RadarSystemState& new_radar_system_state)
{
    memcpy(&local_radar_system_state, &new_radar_system_state, sizeof(RadarSystemState));
}

void ControlAndCommunicateWithControllerObject::sltSendSystemInfoPacketToController()
{
    radar_system_info_udp_packet.radar_id = local_software_settings.communication_settings.output_device_id;
    radar_system_info_udp_packet.timestamp = QDateTime::currentMSecsSinceEpoch();
    radar_system_info_udp_packet.radar_lon = int(local_data_processing_settings.radar_lon_lat_alt.radar_longitude * 1e6);
    radar_system_info_udp_packet.radar_lat = int(local_data_processing_settings.radar_lon_lat_alt.radar_latitude * 1e6);
    radar_system_info_udp_packet.radar_alt = local_data_processing_settings.radar_lon_lat_alt.radar_altitude;
    uint8_t turntable_state = 0;
    if (local_radar_system_state.turntable_rotating_speed > 1)
    {
        turntable_state = 0x02 << 4;
    }
    else if (local_radar_system_state.turntable_rotating_speed < -1)
    {

        turntable_state = 0x03 << 4;
    }
    uint8_t antenna_power_state = local_radar_system_state.antenna_work_mode & 0x03;
    radar_system_info_udp_packet.radar_work_state = turntable_state | antenna_power_state;
    radar_system_info_udp_packet.radar_azim_deg = int(circularlyAddDouble(450, -local_radar_system_state.biss_angle_deg, 0, 360) * 100);
    radar_system_info_udp_packet.radar_elev_deg = int(local_radar_system_state.current_elev_deg * 100);
    radar_system_info_udp_packet.is_working_abnormally_flag = 0;
    int tmp_check_sum = 0;
    uint8_t* packet_start = (uint8_t*)(&radar_system_info_udp_packet);
    for (int ii = 0; ii < sizeof(ZardRadarSystemInfoUdpPacket) - 1; ++ii)
    {
        tmp_check_sum += packet_start[ii];
    }
    radar_system_info_udp_packet.check_byte = tmp_check_sum;

    udp_socket->writeDatagram((char*)(&radar_system_info_udp_packet),
                              sizeof(ZardRadarSystemInfoUdpPacket),
                              QHostAddress(local_software_settings.communication_settings.output_IP_address),
                              local_software_settings.communication_settings.output_port);
}

ControlAndCommunicateWithControllerThread::ControlAndCommunicateWithControllerThread(QObject* parent) : QObject(parent)
{
    qDebug() << tr("[ControlAndCommunicateWithControllerThread]: 进入构造函数!");
    control_and_communicate_object = new ControlAndCommunicateWithControllerObject;
    control_and_communicate_object->moveToThread(&working_thread);
    connect(&working_thread, &QThread::finished, control_and_communicate_object, &QObject::deleteLater);
    connect(&working_thread, &QThread::finished, &working_thread, &QObject::deleteLater);

    connect(control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sigInitializeHardwareSettingsInOtherThreads,
            this,
            &ControlAndCommunicateWithControllerThread::sigInitializeHardwareSettingsInOtherThreads);
    connect(control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sigInitializeSoftwareSettingsInOtherThreads,
            this,
            &ControlAndCommunicateWithControllerThread::sigInitializeSoftwareSettingsInOtherThreads);
    connect(control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads,
            this,
            &ControlAndCommunicateWithControllerThread::sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads);
    connect(control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sigInitializeDataProcessingSettingsInOtherThreads,
            this,
            &ControlAndCommunicateWithControllerThread::sigInitializeDataProcessingSettingsInOtherThreads);

    connect(this,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalHardwareSettings,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltUpdateLocalHardwareSettings);
    connect(this,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalSoftwareSettings,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltUpdateLocalSoftwareSettings);
    connect(this,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalDbfCorrectCoefFileDirSettings,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltUpdateLocalDbfCorrectCoefFileDirSettings);
    connect(this,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalDataProcessingSettings,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltUpdateLocalDataProcessingSettings);

    connect(this,
            &ControlAndCommunicateWithControllerThread::sigAddTrackIdToSendingTargetSet,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltAddTrackIdToSendingTargetSet);
    connect(this,
            &ControlAndCommunicateWithControllerThread::sigRemoveTrackIdFromSendingTargetSet,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltRemoveTrackIdFromSendingTargetSet);
    connect(this,
            &ControlAndCommunicateWithControllerThread::sigSendTrackLastPointListToController,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltSendTrackLastPointListToController);
    connect(this,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalRadarSystemState,
            control_and_communicate_object,
            &ControlAndCommunicateWithControllerObject::sltUpdateLocalRadarSystemState);

    working_thread.start();
}

ControlAndCommunicateWithControllerThread::~ControlAndCommunicateWithControllerThread()
{
    qDebug() << tr("[ControlAndCommunicateWithControllerThread]: 进入析构函数!");
}

void ControlAndCommunicateWithControllerThread::startWorking()
{
    qDebug() << tr("[ControlAndCommunicateWithControllerThread]: startWorking!");
    control_and_communicate_object->initializeHardwareAndSoftwareAndDataProcessingSettingsInOtherThreads();
}

void ControlAndCommunicateWithControllerThread::stopWorking()
{
    qDebug() << tr("[ControlAndCommunicateWithControllerThread]: stopWorking!");
    working_thread.quit();
    working_thread.wait();
}
