#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QEventLoop>
#include "global_functions.h"
#include "communicate_with_fpga_thread.h"

CommunicateWithFpgaObject::CommunicateWithFpgaObject(QObject* parent) : QObject(parent)
{
    qDebug() << tr("[CommunicateWithFpgaObject]: 进入构造函数!");
    stop_flag = false;

    initializeSoftwareSettings(local_software_settings);

    local_packet_cap = new PacketCap;

    current_capture_adapter_index = 0;
    current_capture_adapter_opened_flag = false;
    res = -1;
    header = nullptr;
    packet_data = nullptr;

    // 读取FPGA数据报文文件
    initializeReadFpgaPacketFileSettings(local_read_fpga_packet_file_settings);
    current_fpga_packet_file_opened_flag = false;
    start_read_fpga_packet_file_flag = false;
    finish_read_fpga_packet_file_flag = false;
    current_line.clear();
    packet_to_read_char.clear();
    packet_to_read_hex.clear();
    add_line_to_packet_flag = false;

    // 输出FPGA报文到文件
    fpga_packet_txt = nullptr;
    fpga_packet_text_stream = nullptr;
    is_fpga_packet_txt_opened = false;
    createNewFpgaPacketFile();
}

CommunicateWithFpgaObject::~CommunicateWithFpgaObject()
{
    qDebug() << tr("[CommunicateWithFpgaObject]: 进入析构函数!");
    delete local_packet_cap;
    if (fpga_packet_text_stream)
    {
        fpga_packet_text_stream->flush();
        fpga_packet_text_stream->reset();
        delete fpga_packet_text_stream;
        fpga_packet_text_stream = nullptr;
    }
    if (fpga_packet_txt)
    {
        fpga_packet_txt->close();
        delete fpga_packet_txt;
        fpga_packet_txt = nullptr;
    }
}

void CommunicateWithFpgaObject::stopGetFpgaPacket()
{
    qDebug() << tr("[CommunicateWithFpgaObject]: stopGetFpgaPacket()!");
    QMutexLocker locker(&local_mutex);
    stop_flag = true;
}

bool CommunicateWithFpgaObject::getNextFpgaPacketFromAdapter()
{
    if (local_packet_cap->selected_adapter_opened_flag)
    {
        if ((res = pcap_next_ex(local_packet_cap->ad_handle, &header, &packet_data)) > 0)
        {
            if (header->len == FPGA_PACKET_SIZE)
            {
                if (packet_data[0] == 0xff && packet_data[1] == 0xff && packet_data[2] == 0xff && packet_data[3] == 0xff && packet_data[4] == 0xff && packet_data[5] == 0xff &&
                    packet_data[6] == 0x5a && packet_data[7] == 0x5a && packet_data[8] == 0x00 && packet_data[9] == 0x08 && packet_data[10] == 0x06 && packet_data[11] == 0x06)
                {
                    // 添加一包
                    FpgaPacket* fpga_packet = new FpgaPacket;
                    memcpy(fpga_packet, packet_data, sizeof(FpgaPacket));
                    if (local_software_settings.output_file_settings.output_fpga_packet_flag)
                    {
                        outputNewFpgaPacketToText((unsigned char*)fpga_packet);
                    }

                    emit sigProcessingNextFpgaPacket(fpga_packet);

                    return true;
                }
            }
        }
        else
        {
            if (res < 0)
            {
                qDebug() << tr("[CommunicateWithFpgaObject]: 收取报文失败, 网口停止工作!");
                emit sigCaptureAdapterStopWorking();
                local_packet_cap->selected_adapter_opened_flag = false;
            }
        }
    }
    else
    {
        local_packet_cap->openAdapter(current_capture_adapter_index);
    }
    return false;
}

bool CommunicateWithFpgaObject::getNextFpgaPacketFromFile()
{
    if (current_fpga_packet_file_opened_flag)
    {
        current_line.clear();
        packet_to_read_char.clear();
        packet_to_read_hex.clear();
        add_line_to_packet_flag = false;

        while (!current_fpga_packet_file.atEnd())
        {
            current_line.clear();
            current_line = current_fpga_packet_file.readLine();
            current_line.replace("\n", "");
            if (current_line.size() > 48) // 如果没有截取
            {
                current_line.remove(0, 6);
                current_line.remove(47, 19);
            }
            current_line.replace(" ", "");
            if (add_line_to_packet_flag)
            {
                packet_to_read_char.append(current_line);
                if (packet_to_read_char.size() == FPGA_PACKET_SIZE * 2)
                {
                    packet_to_read_hex = QByteArray::fromHex(packet_to_read_char);

                    // 添加一包
                    FpgaPacket* fpga_packet = new FpgaPacket;
                    memcpy(fpga_packet, packet_to_read_hex.data(), sizeof(FpgaPacket));
                    emit sigProcessingNextFpgaPacket(fpga_packet);
                    return true;
                }
            }
            else
            {
                if (current_line.left(24) == FPGA_PACKET_HEAD.left(24))
                {
                    add_line_to_packet_flag = true;
                    packet_to_read_char.append(current_line);
                }
            }
        }
        emit sigFinishReadingFpgaPacketFile(QDateTime::currentDateTime().time().toString() + QFileInfo(current_fpga_packet_file).fileName() + tr("读取完成!"));
        current_fpga_packet_file.seek(0);
        qDebug() << tr("[CommunicateWithFpgaThread] : 文件读取完成, 从头再来!");
    }

    return false;
}

void CommunicateWithFpgaObject::createNewFpgaPacketFile()
{
    // S1: 如果目录不存在, 则创建这个目录
    QDir fpga_packet_dir;
    if (!fpga_packet_dir.exists(FPGA_PACKET_FOLDER_DIR))
    {
        fpga_packet_dir.mkpath(FPGA_PACKET_FOLDER_DIR);
    }
    fpga_packet_dir.setPath(FPGA_PACKET_FOLDER_DIR);
    // S2: 读取所有的txt
    QStringList fpga_packet_file_filter;
    fpga_packet_file_filter << "*.txt";
    QFileInfoList fpga_packet_file_list = fpga_packet_dir.entryInfoList(fpga_packet_file_filter, QDir::Files, QDir::Name);
    // S3: 如果文件数量超过MAX_FPGA_PACKET_FILE_NUM, 删除最老的
    while (fpga_packet_file_list.size() >= MAX_FPGA_PACKET_FILE_NUM)
    {
        fpga_packet_dir.remove(fpga_packet_file_list.first().fileName());
        fpga_packet_file_list.pop_front();
    }
    // S4: 新建文件并打开
    // 关闭之前的文件
    if (fpga_packet_txt)
    {
        fpga_packet_txt->close();
        delete fpga_packet_txt;
        is_fpga_packet_txt_opened = false;
    }
    // 新建一个文件
    QString new_fpga_packet_file_name = QString(FPGA_PACKET_FOLDER_DIR + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_T_hh_mm_ss_") + FPGA_PACKET_POSTFIX);
    fpga_packet_txt = new QFile(new_fpga_packet_file_name);

    if (fpga_packet_txt->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        is_fpga_packet_txt_opened = true;
        if (fpga_packet_text_stream)
        {
            fpga_packet_text_stream->setDevice(fpga_packet_txt);
            fpga_packet_text_stream->setIntegerBase(16);
        }
        else
        {
            fpga_packet_text_stream = new QTextStream(fpga_packet_txt);
            fpga_packet_text_stream->setIntegerBase(16);
        }
    }
}

void CommunicateWithFpgaObject::outputNewFpgaPacketToText(const unsigned char* packet_data)
{
    if (is_fpga_packet_txt_opened)
    {
        int bytes_num_per_line = 16;
        int byte_index_line_end = bytes_num_per_line - 1;
        for (int ii = 0; ii < FPGA_PACKET_SIZE; ++ii)
        {
            if (packet_data[ii] <= 0x0f)
            {
                (*fpga_packet_text_stream) << '0'; // 给小于等于 0x0f的前面补零
            }
            if (ii % bytes_num_per_line == byte_index_line_end)
            {
                (*fpga_packet_text_stream) << packet_data[ii] << " \n";
            }
            else
            {
                (*fpga_packet_text_stream) << packet_data[ii] << ' ';
            }
        }
        (*fpga_packet_text_stream) << "\n\n";
        ++current_fpga_packet_num;

        if (current_fpga_packet_num % MAX_ITEM_NUM_FLUSH == 0)
        {
            fpga_packet_text_stream->flush();
        }
        if (current_fpga_packet_num >= MAX_FPGA_PACKET_NUM_PER_FILE)
        {
            createNewFpgaPacketFile();
            current_fpga_packet_num = 0;
        }
    }
}

void CommunicateWithFpgaObject::sltGetNextFpgaPacket()
{
    qDebug() << tr("[CommunicateWithFpgaObject]: sltGetNextFpgaPacket()!");

    {
        QMutexLocker locker(&local_mutex);
        stop_flag = false;
    }

    while (true)
    {
        QMutexLocker locker(&local_mutex);
        if (stop_flag)
        {
            break;
        }

        if (local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag)
        {
            if (start_read_fpga_packet_file_flag)
            {
                getNextFpgaPacketFromFile();
                Sleep(5);
            }
        }
        else
        {
            getNextFpgaPacketFromAdapter();
        }
    }
}

CommunicateWithFpgaThread::CommunicateWithFpgaThread(QObject* parent) : QObject(parent)
{
    qDebug() << tr("[CommunicateWithFpgaThread]: 进入构造函数!");
    communicate_with_fpga_object = new CommunicateWithFpgaObject;
    communicate_with_fpga_object->moveToThread(&working_thread);
    connect(&working_thread, &QThread::finished, communicate_with_fpga_object, &QObject::deleteLater);
    connect(&working_thread, &QThread::finished, &working_thread, &QObject::deleteLater);

    initializeRadarMissionModeSettings(local_radar_mission_mode_settings);
    initializeHardwareSettings(local_hardware_settings);
    initializeDbfCorrectCoefFileDirSettings(local_dbf_correct_coef_file_dir_settings);

    local_master_external_file.setFileName(local_dbf_correct_coef_file_dir_settings.master_external_file_dir);
    local_slave_external_file.setFileName(local_dbf_correct_coef_file_dir_settings.slave_external_file_dir);
    local_master_internal_file.setFileName(local_dbf_correct_coef_file_dir_settings.master_internal_file_dir);
    local_slave_internal_file.setFileName(local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir);
    readDbfCorrectCoefFromFile(local_master_external_packet, local_master_external_file);
    readDbfCorrectCoefFromFile(local_slave_external_packet, local_slave_external_file);
    readDbfCorrectCoefFromFile(local_master_internal_packet, local_master_internal_file);
    readDbfCorrectCoefFromFile(local_slave_internal_packet, local_slave_internal_file);

    to_fpga_packet = new ToFpgaPacket;
    initializeToFpgaPacket(to_fpga_packet);
    radar_mode_index_list_to_update_threshold[0] = NEAR_SCAN_MODE_INDEX;  // 默认近搜
    radar_mode_index_list_to_update_threshold[1] = FAR_SCAN_MODE_INDEX;   // 默认远搜
    radar_mode_index_list_to_update_threshold[2] = NEAR_TRACK_MODE_INDEX; // 默认近跟
    radar_mode_index_list_to_update_threshold[3] = FAR_TRACK_MODE_INDEX;  // 默认远跟

    connect(this, &CommunicateWithFpgaThread::sigGetNextFpgaPacket, communicate_with_fpga_object, &CommunicateWithFpgaObject::sltGetNextFpgaPacket);
    connect(communicate_with_fpga_object, &CommunicateWithFpgaObject::sigProcessingNextFpgaPacket, this, &CommunicateWithFpgaThread::sigProcessingNextFpgaPacket);
    connect(communicate_with_fpga_object, &CommunicateWithFpgaObject::sigCaptureAdapterStopWorking, this, &CommunicateWithFpgaThread::sigCaptureAdapterStopWorking);
    connect(communicate_with_fpga_object, &CommunicateWithFpgaObject::sigFinishReadingFpgaPacketFile, this, &CommunicateWithFpgaThread::sigUpdateStatusBarOnMainWindow);

    working_thread.start();
}

CommunicateWithFpgaThread::~CommunicateWithFpgaThread()
{
    qDebug() << tr("[CommunicateWithFpgaThread]: 进入析构函数!");
    delete to_fpga_packet;
}

void CommunicateWithFpgaThread::startWorking()
{
    qDebug() << tr("[CommunicateWithFpgaThread]: startWorking!");
    emit sigGetNextFpgaPacket();
}

void CommunicateWithFpgaThread::stopWorking()
{
    qDebug() << tr("[CommunicateWithFpgaThread]: stopWorking!");
    communicate_with_fpga_object->stopGetFpgaPacket();
    working_thread.quit();
    working_thread.wait();
}

void CommunicateWithFpgaThread::delay(int msecs)
{
    QEventLoop loop;
    QTimer::singleShot(msecs, &loop, SLOT(quit()));
    loop.exec();
}

void CommunicateWithFpgaThread::readDbfCorrectCoefFromFile(QByteArray& coef_packet, QFile& coef_file)
{
    if (coef_file.exists())
    {
        if (coef_file.open(QIODevice::ReadOnly))
        {
            coef_packet.clear();
            while (!coef_file.atEnd())
            {
                QByteArray current_line;
                current_line.clear();
                current_line = coef_file.readLine();
                current_line.replace("\n", "");
                current_line.replace(" ", "");
                coef_packet.append(current_line);
            }
            coef_packet = QByteArray::fromHex(coef_packet);
            qDebug() << QString("[sltSendDbfCorrectCoefFileToFpga]: 读取 DBF 系数成功!");
            coef_file.close();
        }
    }
}

void CommunicateWithFpgaThread::updateToFpgaPacket(const RadarMissionModeSettings& m_radar_mission_mode_settings,
                                                   const HardwareSettings& m_hardware_settings,
                                                   ToFpgaPacket* m_to_fpga_packet)
{
    initializeToFpgaPacket(m_to_fpga_packet);

    int tmp_mission = m_radar_mission_mode_settings.radar_mission_mode;
    switch (tmp_mission)
    {
    default:
        m_to_fpga_packet->radar_mission_mode = 0x33; //近搜模式
        break;
    case 1:
        m_to_fpga_packet->radar_mission_mode = 0x44; //远搜模式
        break;
    case 2:
        m_to_fpga_packet->radar_mission_mode = 0x55; //近远搜模式
        break;
    case 3:
        m_to_fpga_packet->radar_mission_mode = 0x66; //超远搜模式
        radar_mode_index_list_to_update_threshold[1] = SUPER_FAR_SCAN_MODE_INDEX;
        break;
    case 4:
        m_to_fpga_packet->radar_mission_mode = 0x69; //近跟模式
        radar_mode_index_list_to_update_threshold[2] = NEAR_TRACK_MODE_INDEX;
        break;
    case 5:
        m_to_fpga_packet->radar_mission_mode = 0x96; //远跟模式
        radar_mode_index_list_to_update_threshold[3] = FAR_TRACK_MODE_INDEX;
        break;
    }

    m_to_fpga_packet->sigma_decay_1 = (m_hardware_settings.sigma_delta_decay_1 & 0x0f) | 0x40;

    int decay_tmp = m_hardware_settings.sigma_delta_decay_2;
    if (decay_tmp < 32)
    {
        m_to_fpga_packet->sigma_decay_2_3 = (decay_tmp & 0x3f) | 0x40;
    }
    else if (decay_tmp >= 32)
    {
        decay_tmp = decay_tmp - 32;
        m_to_fpga_packet->sigma_decay_2_3 = (decay_tmp & 0x0f) | 0x70;
    }

    m_to_fpga_packet->protect_decay_1 = (m_hardware_settings.protect_decay_1 & 0x0f) | 0x80;

    decay_tmp = m_hardware_settings.protect_decay_2;
    if (decay_tmp < 32)
    {
        m_to_fpga_packet->protect_decay_2_3 = (decay_tmp & 0x3f) | 0x80;
    }
    else if (decay_tmp >= 32)
    {
        decay_tmp = decay_tmp - 32;
        m_to_fpga_packet->protect_decay_2_3 = (decay_tmp & 0x0f) | 0xB0;
    }

    m_to_fpga_packet->antenna_raising_angle_and_fre_code = m_hardware_settings.antennna_freq_code & 0x0f;
    m_to_fpga_packet->antenna_raising_angle_and_fre_code |= ((m_hardware_settings.antenna_raising_angle & 0x0f) << 4);

    int tmp_antenna_power_mode = 0;
    tmp_antenna_power_mode = m_radar_mission_mode_settings.antenna_power_mode;
    switch (tmp_antenna_power_mode)
    {
    default: // 待机
        m_to_fpga_packet->antenna_power_mode = 0x00;
        break;
    case 1: //只接收加电
        m_to_fpga_packet->antenna_power_mode = 0x01;
        break;
    case 2: //只发射加电
        m_to_fpga_packet->antenna_power_mode = 0x02;
        break;
    case 3: //收发均加电
        m_to_fpga_packet->antenna_power_mode = 0x03;
        break;
    }
    m_to_fpga_packet->antenna_power_mode |= (m_hardware_settings.antennna_fre_or_amp_config & 0x01) << 2;
    m_to_fpga_packet->antenna_power_mode |= (m_hardware_settings.work_mode_or_correct_mode & 0x01) << 3;
    m_to_fpga_packet->antenna_power_mode |= (m_hardware_settings.antenna_weight_method & 0x0f) << 4;

    switch (m_hardware_settings.antenna_enable)
    {
    default:
        m_to_fpga_packet->antenna_enable = 0x00; // 非使能
        break;
    case 0:
        m_to_fpga_packet->antenna_enable = 0x5a; // 使能
        break;
    case 2:
        m_to_fpga_packet->antenna_enable = 0x01; // 测试模式 ( 64通道各100点下变频数据 )
        break;
    case 3:
        m_to_fpga_packet->antenna_enable = 0x02; // 测试模式 ( 上位机送和差波束能量值 )
        break;
    case 4:
        m_to_fpga_packet->antenna_enable = 0x05; // 发射通道校准模式
        break;
    }

    double tmp_turntable_rotating_speed = 0;
    if (fabs(m_hardware_settings.spr_map[local_radar_mission_mode_settings.current_spr_mode_index]) > EPS)
    {
        tmp_turntable_rotating_speed = 360.0 / m_hardware_settings.spr_map[local_radar_mission_mode_settings.current_spr_mode_index];
    }
    else
    {
        tmp_turntable_rotating_speed = m_hardware_settings.turntable_rotating_speed;
    }

    uint16_t tmp_rpm = int(tmp_turntable_rotating_speed * 100);
    uint16_t tmp_pointing_angle = int(m_hardware_settings.turntable_pointing_angle * 100);
    switch (m_hardware_settings.turntable_rotating_mode)
    {
    case 0:
        m_to_fpga_packet->turntable_rotating_mode = 0x01;
        m_to_fpga_packet->turntable_enable = 0x5a;
        break;
    case 1:
        m_to_fpga_packet->turntable_rotating_mode = 0x02;
        m_to_fpga_packet->turntable_set[0] = tmp_pointing_angle >> 8;
        m_to_fpga_packet->turntable_set[1] = tmp_pointing_angle;
        m_to_fpga_packet->turntable_enable = 0x5a;
        break;
    case 2:
        m_to_fpga_packet->turntable_rotating_mode = 0x03;
        m_to_fpga_packet->turntable_set[0] = tmp_rpm >> 8;
        m_to_fpga_packet->turntable_set[1] = tmp_rpm;
        m_to_fpga_packet->turntable_enable = 0x5a;
        break;
    case 3:
        m_to_fpga_packet->turntable_rotating_mode = 0x04;
        m_to_fpga_packet->turntable_set[0] = tmp_rpm >> 8;
        m_to_fpga_packet->turntable_set[1] = tmp_rpm;
        m_to_fpga_packet->turntable_enable = 0x5a;
        break;
    case 4:
        m_to_fpga_packet->turntable_rotating_mode = 0x05;
        m_to_fpga_packet->turntable_set[0] = tmp_pointing_angle >> 8;
        m_to_fpga_packet->turntable_set[1] = tmp_pointing_angle;
        m_to_fpga_packet->turntable_enable = 0x5a;
        break;
    default:
        m_to_fpga_packet->turntable_rotating_mode = 0x00;
        m_to_fpga_packet->turntable_enable = 0x00;
        break;
    }

    m_to_fpga_packet->track_elev_angle[0] = 6;
    m_to_fpga_packet->track_elev_angle[1] = 6;
    m_to_fpga_packet->track_elev_angle[2] = 7;
    m_to_fpga_packet->track_elev_angle[3] = 7;
    m_to_fpga_packet->track_elev_angle[4] = 8;
    m_to_fpga_packet->track_elev_angle[5] = 8;
    m_to_fpga_packet->track_elev_angle[6] = 9;
    m_to_fpga_packet->track_elev_angle[7] = 9;

    uint16_t channel_amp_phase_control_code = m_hardware_settings.channel_amp_phase_control_code / 2;
    m_to_fpga_packet->channel_amp_phase_control_code[0] = channel_amp_phase_control_code >> 8;
    m_to_fpga_packet->channel_amp_phase_control_code[1] = channel_amp_phase_control_code;

    m_to_fpga_packet->dbf_weight_method[0] = (m_hardware_settings.dbf_coe_choose & 0x0f);
    m_to_fpga_packet->dbf_weight_method[0] |= ((m_hardware_settings.dbf_coef_receive_raising_angle & 0x03) << 4);
    m_to_fpga_packet->dbf_weight_method[1] = 0x00;

    if (m_hardware_settings.stc_select)
    {
        m_to_fpga_packet->stc_select = 0x00 << 7;
    }
    else
    {
        m_to_fpga_packet->stc_select = 0x01 << 7;
    }
    m_to_fpga_packet->stc_select |= ((m_hardware_settings.elev_support_angle & 0x01) << 6);

    m_to_fpga_packet->channel_amp_phase_control_mode = m_hardware_settings.channel_amp_phase_control_mode;
    m_to_fpga_packet->channel_num = m_hardware_settings.channel_num;
    m_to_fpga_packet->amp_code = m_hardware_settings.amp_code;

    switch (m_hardware_settings.work_or_emulate)
    {
    default:
        to_fpga_packet->work_or_emulate = 0x00;
        break;
    case 1:
        to_fpga_packet->work_or_emulate = 0x01;
        break;
    case 2:
        to_fpga_packet->work_or_emulate = 0x02;
        break;
    }

    if (m_hardware_settings.remove_dc_flag)
    {
        to_fpga_packet->work_or_emulate |= 0x08;
    }

    switch (m_hardware_settings.cfar_method)
    {
    default:
        to_fpga_packet->work_or_emulate |= 0x00;
        break;
    case 1:
        to_fpga_packet->work_or_emulate |= 0x10;
        break;
    case 2:
        to_fpga_packet->work_or_emulate |= 0x20;
        break;
    case 3:
        to_fpga_packet->work_or_emulate |= 0x30;
        break;
    }

    switch (m_hardware_settings.work_or_serdes)
    {
    default: //正常工作
        to_fpga_packet->work_or_serdes = 0;
        break;
    case 1: // serdes检测
        to_fpga_packet->work_or_serdes = 1;
        break;
    }

    for (int ii = 0; ii < 4; ii++)
    {
        int tmp_radar_mode_index = radar_mode_index_list_to_update_threshold[ii];
        uint16_t tmp_damp_cut_division_index = m_hardware_settings.thresholds[tmp_radar_mode_index].damp_cut_division_range_index;
        m_to_fpga_packet->cut_and_exclude_settings[ii].damp_cut_division_index[0] = tmp_damp_cut_division_index >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].damp_cut_division_index[1] = tmp_damp_cut_division_index;

        m_to_fpga_packet->cut_and_exclude_settings[ii].damp_cut_near = m_hardware_settings.thresholds[tmp_radar_mode_index].damp_cut_near;
        m_to_fpga_packet->cut_and_exclude_settings[ii].damp_cut_far = m_hardware_settings.thresholds[tmp_radar_mode_index].damp_cut_far;

        uint16_t tmp_doppler_exclude = m_hardware_settings.thresholds[tmp_radar_mode_index].doppler_exclude[0];
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_1[0] = tmp_doppler_exclude >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_1[1] = tmp_doppler_exclude;

        tmp_doppler_exclude = m_hardware_settings.thresholds[tmp_radar_mode_index].doppler_exclude[1];
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_2[0] = tmp_doppler_exclude >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_2[1] = tmp_doppler_exclude;

        tmp_doppler_exclude = m_hardware_settings.thresholds[tmp_radar_mode_index].doppler_exclude[2];
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_3[0] = tmp_doppler_exclude >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_3[1] = tmp_doppler_exclude;

        tmp_doppler_exclude = m_hardware_settings.thresholds[tmp_radar_mode_index].doppler_exclude[3];
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_4[0] = tmp_doppler_exclude >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_exclude_4[1] = tmp_doppler_exclude;

        uint16_t tmp_doppler_threshold = m_hardware_settings.thresholds[tmp_radar_mode_index].doppler_threshold;
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_threshold[0] = tmp_doppler_threshold >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].doppler_threshold[1] = tmp_doppler_threshold;

        uint16_t tmp_range_threshold_division_index_n_f = m_hardware_settings.thresholds[tmp_radar_mode_index].range_threshold_division_range_index_n_f;
        m_to_fpga_packet->cut_and_exclude_settings[ii].range_threshold_division_index_n_f[0] = tmp_range_threshold_division_index_n_f >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].range_threshold_division_index_n_f[1] = tmp_range_threshold_division_index_n_f;

        uint16_t tmp_range_threshold_near = m_hardware_settings.thresholds[tmp_radar_mode_index].range_threshold_near;
        m_to_fpga_packet->cut_and_exclude_settings[ii].range_threshold_near[0] = tmp_range_threshold_near >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].range_threshold_near[1] = tmp_range_threshold_near;

        uint16_t tmp_range_threshold_far = m_hardware_settings.thresholds[tmp_radar_mode_index].range_threshold_far;
        m_to_fpga_packet->cut_and_exclude_settings[ii].range_threshold_far[0] = tmp_range_threshold_far >> 8;
        m_to_fpga_packet->cut_and_exclude_settings[ii].range_threshold_far[1] = tmp_range_threshold_far;
    }

    m_to_fpga_packet->scan_fft_cut = (m_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[0]].fft_cut << 4) |
                                     (m_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[1]].fft_cut);
    m_to_fpga_packet->track_fft_cut = (m_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[2]].fft_cut << 4) |
                                      (m_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[3]].fft_cut);

    int tmp_test_mode = 0;
    tmp_test_mode = m_radar_mission_mode_settings.radar_test_mode;
    switch (tmp_test_mode)
    {
    case 0:
        m_to_fpga_packet->radar_test_mode = 0x00;
        break;
    case 1:
        // 先发停止命令, 再复位
        sltSendStopOrderToFpga();
        m_to_fpga_packet->radar_test_mode = 0x55;
        break;
    case 2:
        m_to_fpga_packet->radar_test_mode = 0x66;
        break;
    case 3:
        // 模拟转台, 需要把转台控制字修正为“停止模式”, 0x01
        m_to_fpga_packet->radar_test_mode = 0x99;
        m_to_fpga_packet->turntable_rotating_mode = 0x01;
        break;
    case 4:
        m_to_fpga_packet->radar_test_mode = 0x96;
        m_to_fpga_packet->turntable_rotating_mode = 0x01;
        break;
    default:
        m_to_fpga_packet->radar_test_mode = 0x00;
        break;
    }
}

void CommunicateWithFpgaThread::sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings)
{
    qDebug() << tr("[CommunicateWithFpgaThread]: sltUpdateLocalHardwareSettings! ");
    memcpy(&local_hardware_settings, &new_hardware_settings, sizeof(HardwareSettings));
}

void CommunicateWithFpgaThread::sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings)
{
    qDebug() << tr("[CommunicateWithFpgaThread]: sltUpdateLocalSoftwareSettings! ");

    QMutexLocker locker(&communicate_with_fpga_object->local_mutex);
    memcpy(&communicate_with_fpga_object->local_software_settings, &new_software_settings, sizeof(SoftwareSettings));
}

void CommunicateWithFpgaThread::sltUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings)
{
    qDebug() << tr("[CommunicateWithFpgaThread]: sltUpdateLocalDbfCorrectCoefFileDirSettings! ");
    if (local_dbf_correct_coef_file_dir_settings.master_external_file_dir != new_dbf_correct_coef_file_dir_settings.master_external_file_dir &&
        new_dbf_correct_coef_file_dir_settings.master_external_file_dir.size() > 0 && QFile(new_dbf_correct_coef_file_dir_settings.master_external_file_dir).exists())
    {
        local_dbf_correct_coef_file_dir_settings.master_external_file_dir = new_dbf_correct_coef_file_dir_settings.master_external_file_dir;
        local_master_external_file.close();
        local_master_external_file.setFileName(local_dbf_correct_coef_file_dir_settings.master_external_file_dir);
        readDbfCorrectCoefFromFile(local_master_external_packet, local_master_external_file);
    }
    if (local_dbf_correct_coef_file_dir_settings.slave_external_file_dir != new_dbf_correct_coef_file_dir_settings.slave_external_file_dir &&
        new_dbf_correct_coef_file_dir_settings.slave_external_file_dir.size() > 0 && QFile(new_dbf_correct_coef_file_dir_settings.slave_external_file_dir).exists())
    {
        local_dbf_correct_coef_file_dir_settings.slave_external_file_dir = new_dbf_correct_coef_file_dir_settings.slave_external_file_dir;
        local_slave_external_file.close();
        local_slave_external_file.setFileName(local_dbf_correct_coef_file_dir_settings.slave_external_file_dir);
        readDbfCorrectCoefFromFile(local_slave_external_packet, local_slave_external_file);
    }
    if (local_dbf_correct_coef_file_dir_settings.master_internal_file_dir != new_dbf_correct_coef_file_dir_settings.master_internal_file_dir &&
        new_dbf_correct_coef_file_dir_settings.master_internal_file_dir.size() > 0 && QFile(new_dbf_correct_coef_file_dir_settings.master_internal_file_dir).exists())
    {
        local_dbf_correct_coef_file_dir_settings.master_internal_file_dir = new_dbf_correct_coef_file_dir_settings.master_internal_file_dir;
        local_master_internal_file.close();
        local_master_internal_file.setFileName(local_dbf_correct_coef_file_dir_settings.master_internal_file_dir);
        readDbfCorrectCoefFromFile(local_master_internal_packet, local_master_internal_file);
    }
    if (local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir != new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir &&
        new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir.size() > 0 && QFile(new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir).exists())
    {
        local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir = new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir;
        local_slave_internal_file.close();
        local_slave_internal_file.setFileName(local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir);
        readDbfCorrectCoefFromFile(local_slave_internal_packet, local_slave_internal_file);
    }
}

void CommunicateWithFpgaThread::sltChangeCaptureAdapter(const int& new_adapter_index)
{
    qDebug() << tr("[CommunicateWithFpgaThread]: sltChangeCaptureAdapter! new_adapter_index = ") << new_adapter_index;

    QMutexLocker locker(&communicate_with_fpga_object->local_mutex);
    communicate_with_fpga_object->current_capture_adapter_index = new_adapter_index;
    if (communicate_with_fpga_object->local_packet_cap->openAdapter(new_adapter_index))
    {
        qDebug() << tr("[CommunicateWithFpgaThread]: sltChangeCaptureAdapter 成功!");
    }
    else
    {
        qDebug() << tr("[CommunicateWithFpgaThread]: sltChangeCaptureAdapter 失败!");
    }
}

void CommunicateWithFpgaThread::sltSendNewDbfCorrectCoefFileToFpga()
{
    if (local_master_external_packet.size() > 0 && local_slave_external_packet.size() > 0)
    {
        communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)local_master_external_packet.data(), local_master_external_packet.size());
        communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)local_slave_external_packet.data(), local_slave_external_packet.size());
        emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + tr(" DBF correction file sended successfully! "));
    }
}

void CommunicateWithFpgaThread::sltStartReadingFpgaPacketFile(const ReadFpgaPacketFileSettings& new_read_fpga_file_settings)
{
    QMutexLocker locker(&communicate_with_fpga_object->local_mutex);
    communicate_with_fpga_object->local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag = new_read_fpga_file_settings.read_fpga_packet_from_file_flag;
    if (communicate_with_fpga_object->local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag)
    {
        communicate_with_fpga_object->start_read_fpga_packet_file_flag = true;
        if ((new_read_fpga_file_settings.fpga_packet_file_dir != communicate_with_fpga_object->local_read_fpga_packet_file_settings.fpga_packet_file_dir) &&
            (!new_read_fpga_file_settings.fpga_packet_file_dir.isEmpty()))
        {
            qDebug() << "[CommunicateWithFpgaThread][Warning]: 之前可能已经打开一个文件, 关闭之!";
            communicate_with_fpga_object->current_fpga_packet_file.close();
            communicate_with_fpga_object->current_fpga_packet_file_opened_flag = false;
            // 打开新选择的文件
            communicate_with_fpga_object->current_fpga_packet_file.setFileName(new_read_fpga_file_settings.fpga_packet_file_dir);
            communicate_with_fpga_object->local_read_fpga_packet_file_settings.fpga_packet_file_dir = new_read_fpga_file_settings.fpga_packet_file_dir;
            if (communicate_with_fpga_object->current_fpga_packet_file.exists())
            {
                if (communicate_with_fpga_object->current_fpga_packet_file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    qDebug() << "[CommunicateWithFpgaThread]: 打开了选择的新FPGA数据报文文件!";
                    communicate_with_fpga_object->current_fpga_packet_file_opened_flag = true;
                    emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + tr(" Start reading new FPGA packet file!"));
                }
                else
                {
                    qDebug() << "[CommunicateWithFpgaThread][Error]: 当前选择的数据报文文件无法打开!";
                    communicate_with_fpga_object->current_fpga_packet_file.setFileName("");
                }
            }
            else
            {
                qDebug() << "[CommunicateWithFpgaThread][Error]: 当前选择的数据报文文件不存在!";
                communicate_with_fpga_object->current_fpga_packet_file.setFileName("");
            }
        }
        else
        {
            qDebug() << "[CommunicateWithFpgaThread]: 新文件名称与之前一致, 继续回读!";
        }
    }
    else
    {
        qDebug() << "[CommunicateWithFpgaThread]: 回读勾选取消, 停止回读!";
        communicate_with_fpga_object->local_read_fpga_packet_file_settings.fpga_packet_file_dir.clear();
        communicate_with_fpga_object->current_fpga_packet_file.close();
        communicate_with_fpga_object->current_fpga_packet_file.setFileName("");
        communicate_with_fpga_object->current_fpga_packet_file_opened_flag = false;
        communicate_with_fpga_object->start_read_fpga_packet_file_flag = false;
    }
}

void CommunicateWithFpgaThread::sltStopReadingFpgaPacketFile()
{
    qDebug() << "[CommunicateWithFpgaThread]: 停止回读!";
    QMutexLocker locker(&communicate_with_fpga_object->local_mutex);
    communicate_with_fpga_object->start_read_fpga_packet_file_flag = false;
    emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + tr(" Stop reading new FPGA packet file!"));
}

void CommunicateWithFpgaThread::sltSendWorkOrderToFpga(const RadarMissionModeSettings& new_radar_mission_mode_settings)
{
    memcpy(&local_radar_mission_mode_settings, &new_radar_mission_mode_settings, sizeof(RadarMissionModeSettings));
    updateToFpgaPacket(local_radar_mission_mode_settings, local_hardware_settings, to_fpga_packet);
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
    emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + tr(" New work order sended successfully!"));
}

void CommunicateWithFpgaThread::sltSendStopOrderToFpga()
{
    // Step 1: 先让天线和转台停止工作
    to_fpga_packet->antenna_power_mode &= 0xfc;
    to_fpga_packet->turntable_rotating_mode = 0x01;
    to_fpga_packet->turntable_set[0] = 0x00;
    to_fpga_packet->turntable_set[1] = 0x00;
    to_fpga_packet->antenna_enable = 0x5a;
    to_fpga_packet->turntable_enable = 0x5a;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
    // Step 2: 再改变天线和转台的使能
    to_fpga_packet->antenna_enable = 0x00;
    to_fpga_packet->turntable_enable = 0x00;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);

    emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + tr(" Stop order sended successfully!"));
}

void CommunicateWithFpgaThread::sltSendResetOrderToFpga()
{
    sltSendStopOrderToFpga();
    to_fpga_packet->radar_test_mode = 0x55;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);

    emit sigUpdateStatusBarOnMainWindow(QDateTime::currentDateTime().time().toString() + tr(" Reset order sended successfully!"));
}

void CommunicateWithFpgaThread::sltSendPowerOffOrderToFpga()
{
    to_fpga_packet->antenna_enable = 0x5a;
    to_fpga_packet->antenna_power_mode &= 0xfc;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
}

void CommunicateWithFpgaThread::sltSendPowerOnOrderToFpga()
{
    switch (local_radar_mission_mode_settings.antenna_power_mode)
    {
    default:
        to_fpga_packet->antenna_power_mode &= 0xfc;
        break;
    case 1:
        to_fpga_packet->antenna_power_mode &= 0xfc;
        to_fpga_packet->antenna_power_mode |= 0x01;
        break;
    case 2:
        to_fpga_packet->antenna_power_mode &= 0xfc;
        to_fpga_packet->antenna_power_mode |= 0x02;
        break;
    case 3:
        to_fpga_packet->antenna_power_mode &= 0xfc;
        to_fpga_packet->antenna_power_mode |= 0x03;
        break;
    }
    to_fpga_packet->antenna_enable = 0x5a;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
}

void CommunicateWithFpgaThread::sltSendStopPointingAndTrackOrderToEnterTrackMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode)
{
    // S1: 发送转台停止命令
    to_fpga_packet->antenna_power_mode &= 0xfc;
    to_fpga_packet->turntable_rotating_mode = 0x01;
    to_fpga_packet->turntable_set[0] = 0x00;
    to_fpga_packet->turntable_set[1] = 0x00;
    to_fpga_packet->antenna_enable = 0x5a;
    to_fpga_packet->turntable_enable = 0x5a;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);

    delay(200);

    // S2: 发送转台指向命令
    int tmp_pointing_angle = int(new_hardware_settings_track_mode.next_pointing_azim_deg * 100);
    to_fpga_packet->turntable_rotating_mode = 0x02;
    to_fpga_packet->turntable_set[0] = tmp_pointing_angle >> 8;
    to_fpga_packet->turntable_set[1] = tmp_pointing_angle;
    to_fpga_packet->turntable_enable = 0x5a;
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);

    delay(500);

    // S3: 发送跟踪命令
    to_fpga_packet->antenna_enable = 0x5a;
    to_fpga_packet->antenna_power_mode &= 0xfc;
    to_fpga_packet->antenna_power_mode |= 0x03;
    to_fpga_packet->radar_mission_mode = RADAR_MODE_CONFIG_BYTE[new_hardware_settings_track_mode.next_radar_mode];

    // 更新门限和截位等
    // 根据下一个模式在报文中的位置, 更新对应的报文
    int tmp_threshold_index = 2;
    if (new_hardware_settings_track_mode.next_radar_mode >= NEAR_FAR_TRACK_MODE_INDEX_DIVISION)
    {
        // 远跟
        tmp_threshold_index = 3;
        radar_mode_index_list_to_update_threshold[2] = NEAR_TRACK_MODE_INDEX;
        radar_mode_index_list_to_update_threshold[3] = new_hardware_settings_track_mode.next_radar_mode;
    }
    else
    {
        // 近跟
        tmp_threshold_index = 2;
        radar_mode_index_list_to_update_threshold[2] = new_hardware_settings_track_mode.next_radar_mode;
        radar_mode_index_list_to_update_threshold[3] = FAR_TRACK_MODE_INDEX;
    }
    uint16_t tmp_doppler_threshold = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_threshold;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_threshold[0] = tmp_doppler_threshold >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_threshold[1] = tmp_doppler_threshold;

    uint16_t tmp_range_threshold_division_index_n_f = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].range_threshold_division_range_index_n_f;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_division_index_n_f[0] = tmp_range_threshold_division_index_n_f >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_division_index_n_f[1] = tmp_range_threshold_division_index_n_f;

    uint16_t tmp_range_threshold_near = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].range_threshold_near;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_near[0] = tmp_range_threshold_near >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_near[1] = tmp_range_threshold_near;

    uint16_t tmp_range_threshold_far = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].range_threshold_far;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_far[0] = tmp_range_threshold_far >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_far[1] = tmp_range_threshold_far;

    to_fpga_packet->track_fft_cut = (local_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[2]].fft_cut << 4) |
                                    (local_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[3]].fft_cut);

    uint16_t tmp_damp_cut_division_index = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].damp_cut_division_range_index;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_division_index[0] = tmp_damp_cut_division_index >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_division_index[1] = tmp_damp_cut_division_index;

    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_near =
        local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].damp_cut_near;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_far = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].damp_cut_far;

    uint16_t tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[0];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_1[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_1[1] = tmp_doppler_exclude;
    tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[1];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_2[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_2[1] = tmp_doppler_exclude;
    tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[2];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_3[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_3[1] = tmp_doppler_exclude;
    tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[3];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_4[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_4[1] = tmp_doppler_exclude;

    to_fpga_packet->track_elev_angle[0] = new_hardware_settings_track_mode.next_elev_index_list[0];
    to_fpga_packet->track_elev_angle[1] = new_hardware_settings_track_mode.next_elev_index_list[0];
    to_fpga_packet->track_elev_angle[2] = new_hardware_settings_track_mode.next_elev_index_list[1];
    to_fpga_packet->track_elev_angle[3] = new_hardware_settings_track_mode.next_elev_index_list[1];
    to_fpga_packet->track_elev_angle[4] = new_hardware_settings_track_mode.next_elev_index_list[2];
    to_fpga_packet->track_elev_angle[5] = new_hardware_settings_track_mode.next_elev_index_list[2];
    to_fpga_packet->track_elev_angle[6] = new_hardware_settings_track_mode.next_elev_index_list[3];
    to_fpga_packet->track_elev_angle[7] = new_hardware_settings_track_mode.next_elev_index_list[3];

    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
}

void CommunicateWithFpgaThread::sltQuitTrackModeToScanMode(const HardwareSettingsTrackMode& new_hardware_settings_track_mode)
{
    switch (new_hardware_settings_track_mode.next_radar_mode)
    {
    default:
        local_radar_mission_mode_settings.radar_mission_mode = 0;
        break;
    case 1:
        local_radar_mission_mode_settings.radar_mission_mode = 1;
        break;
    }
    radar_mode_index_list_to_update_threshold[0] = NEAR_SCAN_MODE_INDEX;  // 默认近搜
    radar_mode_index_list_to_update_threshold[1] = FAR_SCAN_MODE_INDEX;   // 默认远搜
    radar_mode_index_list_to_update_threshold[2] = NEAR_TRACK_MODE_INDEX; // 默认近跟
    radar_mode_index_list_to_update_threshold[3] = FAR_TRACK_MODE_INDEX;  // 默认远跟
    updateToFpgaPacket(local_radar_mission_mode_settings, local_hardware_settings, to_fpga_packet);
    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
}

void CommunicateWithFpgaThread::sltSendNewHardwareSettingsTrackModeToFpga(const HardwareSettingsTrackMode& new_hardware_settings_track_mode)
{
    int tmp_pointing_angle = int(new_hardware_settings_track_mode.next_pointing_azim_deg * 100);
    to_fpga_packet->turntable_rotating_mode = 0x02;
    to_fpga_packet->turntable_set[0] = tmp_pointing_angle >> 8;
    to_fpga_packet->turntable_set[1] = tmp_pointing_angle;
    to_fpga_packet->turntable_enable = 0x5a;
    to_fpga_packet->antenna_enable = 0x5a;
    to_fpga_packet->antenna_power_mode &= 0xfc;
    to_fpga_packet->antenna_power_mode |= 0x03;
    to_fpga_packet->radar_mission_mode = RADAR_MODE_CONFIG_BYTE[new_hardware_settings_track_mode.next_radar_mode];

    // 更新门限和截位等
    // 根据下一个模式在报文中的位置, 更新对应的报文
    int tmp_threshold_index = 2;
    if (new_hardware_settings_track_mode.next_radar_mode >= NEAR_FAR_TRACK_MODE_INDEX_DIVISION)
    {
        // 远跟
        tmp_threshold_index = 3;
        radar_mode_index_list_to_update_threshold[2] = NEAR_TRACK_MODE_INDEX;
        radar_mode_index_list_to_update_threshold[3] = new_hardware_settings_track_mode.next_radar_mode;
    }
    else
    {
        // 近跟
        tmp_threshold_index = 2;
        radar_mode_index_list_to_update_threshold[2] = new_hardware_settings_track_mode.next_radar_mode;
        radar_mode_index_list_to_update_threshold[3] = FAR_TRACK_MODE_INDEX;
    }
    uint16_t tmp_doppler_threshold = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_threshold;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_threshold[0] = tmp_doppler_threshold >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_threshold[1] = tmp_doppler_threshold;

    uint16_t tmp_range_threshold_division_index_n_f = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].range_threshold_division_range_index_n_f;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_division_index_n_f[0] = tmp_range_threshold_division_index_n_f >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_division_index_n_f[1] = tmp_range_threshold_division_index_n_f;

    uint16_t tmp_range_threshold_near = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].range_threshold_near;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_near[0] = tmp_range_threshold_near >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_near[1] = tmp_range_threshold_near;

    uint16_t tmp_range_threshold_far = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].range_threshold_far;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_far[0] = tmp_range_threshold_far >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].range_threshold_far[1] = tmp_range_threshold_far;

    to_fpga_packet->track_fft_cut = (local_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[2]].fft_cut << 4) |
                                    (local_hardware_settings.thresholds[radar_mode_index_list_to_update_threshold[3]].fft_cut);

    uint16_t tmp_damp_cut_division_index = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].damp_cut_division_range_index;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_division_index[0] = tmp_damp_cut_division_index >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_division_index[1] = tmp_damp_cut_division_index;

    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_near =
        local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].damp_cut_near;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].damp_cut_far = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].damp_cut_far;

    uint16_t tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[0];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_1[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_1[1] = tmp_doppler_exclude;
    tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[1];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_2[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_2[1] = tmp_doppler_exclude;
    tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[2];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_3[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_3[1] = tmp_doppler_exclude;
    tmp_doppler_exclude = local_hardware_settings.thresholds[new_hardware_settings_track_mode.next_radar_mode].doppler_exclude[3];
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_4[0] = tmp_doppler_exclude >> 8;
    to_fpga_packet->cut_and_exclude_settings[tmp_threshold_index].doppler_exclude_4[1] = tmp_doppler_exclude;

    to_fpga_packet->track_elev_angle[0] = new_hardware_settings_track_mode.next_elev_index_list[0];
    to_fpga_packet->track_elev_angle[1] = new_hardware_settings_track_mode.next_elev_index_list[0];
    to_fpga_packet->track_elev_angle[2] = new_hardware_settings_track_mode.next_elev_index_list[1];
    to_fpga_packet->track_elev_angle[3] = new_hardware_settings_track_mode.next_elev_index_list[1];
    to_fpga_packet->track_elev_angle[4] = new_hardware_settings_track_mode.next_elev_index_list[2];
    to_fpga_packet->track_elev_angle[5] = new_hardware_settings_track_mode.next_elev_index_list[2];
    to_fpga_packet->track_elev_angle[6] = new_hardware_settings_track_mode.next_elev_index_list[3];
    to_fpga_packet->track_elev_angle[7] = new_hardware_settings_track_mode.next_elev_index_list[3];

    communicate_with_fpga_object->local_packet_cap->sendPacket((unsigned char*)to_fpga_packet, TO_FPGA_PACKET_SIZE);
}
