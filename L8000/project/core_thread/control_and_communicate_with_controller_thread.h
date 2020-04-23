#ifndef CONTROL_AND_COMMUNICATE_WITH_CONTROLLER_THREAD_H
#define CONTROL_AND_COMMUNICATE_WITH_CONTROLLER_THREAD_H

/*
 * 功能: 控制和通信线程, 包括
 *    控制: 跟设置文件交互（读取并初始化软硬件及算法设置、更新设置文件、更新最新设置等）
 *         跟界面及其他线程交互, 传递最新设置
 *    通信: 给指控发送航迹报文、状态报文
 *         接收指控的命令, 响应相应的信息
 */

#include <QThread>
#include <QUdpSocket>
#include <QSettings>
#include <QTimer>
#include "global_settings.h"
#include "protocol_with_controller.h"
#include "radar_data_processing_module.h"

const int RADAR_SYSTEM_INFO_TIMER_INTERVAL = 500;

class ControlAndCommunicateWithControllerObject : public QObject
{
    Q_OBJECT
public:
    explicit ControlAndCommunicateWithControllerObject(QObject* parent = nullptr);
    ~ControlAndCommunicateWithControllerObject();

    void initializeHardwareAndSoftwareAndDataProcessingSettingsInOtherThreads();

private:
    QUdpSocket* udp_socket;
    QSettings* settings_in_config;
    HardwareSettings local_hardware_settings;
    SoftwareSettings local_software_settings;
    DbfCorrectCoefFileDirSettings local_dbf_correct_coef_file_dir_settings;
    DataProcessingSettings local_data_processing_settings;
    RadarSystemState local_radar_system_state;
    ZardRadarSystemInfoUdpPacket radar_system_info_udp_packet;
    QTimer* radar_system_info_timer;

    QSet<int> sending_target_set; // 给指控平台1发航迹

signals:
    void sigInitializeHardwareSettingsInOtherThreads(const HardwareSettings& new_hardware_settings);
    void sigInitializeSoftwareSettingsInOtherThreads(const SoftwareSettings& new_software_settings);
    void sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings);
    void sigInitializeDataProcessingSettingsInOtherThreads(const DataProcessingSettings& new_data_processing_settings);

public slots:
    void sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);
    void sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sltUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings);
    void sltUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings);

    void sltAddTrackIdToSendingTargetSet(const int& new_track_id);
    void sltRemoveTrackIdFromSendingTargetSet(const int& new_track_id);
    void sltSendTrackLastPointListToController(const int& m_sector_index, QList<TargetTrackPoint>* m_track_to_send_list);

    void sltUpdateLocalRadarSystemState(const RadarSystemState& new_radar_system_state);
    void sltSendSystemInfoPacketToController();
};

class ControlAndCommunicateWithControllerThread : public QObject
{
    Q_OBJECT
public:
    explicit ControlAndCommunicateWithControllerThread(QObject* parent = nullptr);
    ~ControlAndCommunicateWithControllerThread() override;

    void startWorking();
    void stopWorking();

private:
    ControlAndCommunicateWithControllerObject* control_and_communicate_object;
    QThread working_thread;

signals:
    void sigUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);
    void sigUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sigUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings);
    void sigUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings);
    void sigInitializeHardwareSettingsInOtherThreads(const HardwareSettings& new_hardware_settings);
    void sigInitializeSoftwareSettingsInOtherThreads(const SoftwareSettings& new_software_settings);
    void sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings);
    void sigInitializeDataProcessingSettingsInOtherThreads(const DataProcessingSettings& new_data_processing_settings);

    void sigAddTrackIdToSendingTargetSet(const int& new_track_id);
    void sigRemoveTrackIdFromSendingTargetSet(const int& new_track_id);
    void sigSendTrackLastPointListToController(const int& m_sector_index, QList<TargetTrackPoint>* m_track_to_send_list);
    void sigUpdateLocalRadarSystemState(const RadarSystemState& new_radar_system_state);
};

#endif // CONTROL_AND_COMMUNICATE_WITH_CONTROLLER_THREAD_H
