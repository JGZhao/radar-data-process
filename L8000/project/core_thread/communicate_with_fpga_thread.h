#ifndef COMMUNICATE_WITH_FPGA_THREAD_H
#define COMMUNICATE_WITH_FPGA_THREAD_H

/*
 * 功能: 跟FPGA通信线程, 包括
 *    打开选择的网口
 *    发送指控命令
 *    接收 FPGA 报文
 */

#include <QThread>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include "global_settings.h"
#include "protocol_with_fpga.h"
#include "packet_cap.h" // 在包含头文件时, 这个一定要在最后面

const QByteArray FPGA_PACKET_HEAD = "ffffffffffff5a5a00080606";

class CommunicateWithFpgaObject : public QObject
{
    Q_OBJECT
public:
    explicit CommunicateWithFpgaObject(QObject* parent = nullptr);
    ~CommunicateWithFpgaObject();

public:
    // 线程外用于切换网口、更改文件等操作时的互斥锁
    QMutex local_mutex;

    // 本地的软件设置
    SoftwareSettings local_software_settings;

    // 收发包工具
    PacketCap* local_packet_cap;
    int current_capture_adapter_index;

    // 读取FPGA数据报文文件
    ReadFpgaPacketFileSettings local_read_fpga_packet_file_settings;
    QFile current_fpga_packet_file;
    bool current_fpga_packet_file_opened_flag;
    bool start_read_fpga_packet_file_flag;

private:
    // 停止标志位
    bool stop_flag;

    // 接收FPGA数据报文
    bool current_capture_adapter_opened_flag;
    int res;
    struct pcap_pkthdr* header;
    const unsigned char* packet_data;

    // 读取FPGA数据报文文件
    bool finish_read_fpga_packet_file_flag;
    QByteArray current_line;
    QByteArray packet_to_read_char;
    QByteArray packet_to_read_hex;
    bool add_line_to_packet_flag; // 是否已经是有效FPGA报文

    // 输出FPGA数据报文到文件
    QFile* fpga_packet_txt;
    QTextStream* fpga_packet_text_stream;
    bool is_fpga_packet_txt_opened;
    int current_fpga_packet_num;

public:
    void stopGetFpgaPacket();

private:
    // 获取报文
    bool getNextFpgaPacketFromAdapter(); // 从网口读取下一个FPGA数据报文
    bool getNextFpgaPacketFromFile();    // 从文件读取下一个FPGA数据报文

    // 输出报文到文件
    void createNewFpgaPacketFile();
    void outputNewFpgaPacketToText(const unsigned char* packet_data);

signals:
    void sigProcessingNextFpgaPacket(FpgaPacket* new_received_fpga_packet);
    void sigCaptureAdapterStopWorking();
    void sigFinishReadingFpgaPacketFile(const QString& finish_read_fpga_file);

public slots:
    void sltGetNextFpgaPacket();
};

class CommunicateWithFpgaThread : public QObject
{
    Q_OBJECT
public:
    explicit CommunicateWithFpgaThread(QObject* parent = nullptr);
    ~CommunicateWithFpgaThread() override;

    void startWorking();
    void stopWorking();

private:
    CommunicateWithFpgaObject* communicate_with_fpga_object;
    QThread working_thread;

    // 本地的硬件设置, 用于更新FPGA命令报文
    RadarMissionModeSettings local_radar_mission_mode_settings;
    HardwareSettings local_hardware_settings;
    DbfCorrectCoefFileDirSettings local_dbf_correct_coef_file_dir_settings;

    // 命令报文
    ToFpgaPacket* to_fpga_packet;
    int radar_mode_index_list_to_update_threshold[4];

    // 校准系数
    QFile local_master_external_file;        // 主板外校准系数文件
    QFile local_slave_external_file;         // 从板外校准系数文件
    QFile local_master_internal_file;        // 主板内校准系数文件
    QFile local_slave_internal_file;         // 从板内校准系数文件
    QByteArray local_master_external_packet; // 主板外校准系数报文
    QByteArray local_slave_external_packet;  // 从板外校准系数报文
    QByteArray local_master_internal_packet; // 主板内校准系数报文
    QByteArray local_slave_internal_packet;  // 从板内校准系数报文

private:
    void delay(int msecs); // 不阻塞延时

    void readDbfCorrectCoefFromFile(QByteArray& coef_packet, QFile& coef_file);

    // 更新命令报文
    void updateToFpgaPacket(const RadarMissionModeSettings& m_radar_mission_mode_settings, const HardwareSettings& m_hardware_settings, ToFpgaPacket* m_to_fpga_packet);

signals:
    void sigGetNextFpgaPacket();
    void sigCaptureAdapterStopWorking();
    void sigUpdateStatusBarOnMainWindow(const QString& new_message);
    void sigProcessingNextFpgaPacket(FpgaPacket* new_received_fpga_packet);

public slots:
    void sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);
    void sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sltUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings);
    void sltChangeCaptureAdapter(const int& new_adapter_index);

    void sltSendNewDbfCorrectCoefFileToFpga();

    void sltStartReadingFpgaPacketFile(const ReadFpgaPacketFileSettings& new_read_fpga_packet_file_settings);
    void sltStopReadingFpgaPacketFile();

    void sltSendWorkOrderToFpga(const RadarMissionModeSettings& new_radar_mission_mode_settings);
    void sltSendStopOrderToFpga();
    void sltSendResetOrderToFpga();
    void sltSendPowerOffOrderToFpga();
    void sltSendPowerOnOrderToFpga();

    void sltSendStopPointingAndTrackOrderToEnterTrackMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
    void sltQuitTrackModeToScanMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
    void sltSendNewHardwareSettingsTrackModeToFpga(const HardwareSettingsTrackMode& new_hardware_settings_track_mode);
};

#endif // COMMUNICATE_WITH_FPGA_THREAD_H
