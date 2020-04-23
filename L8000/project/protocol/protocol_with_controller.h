#ifndef JOINT_PROTOCOL_H
#define JOINT_PROTOCOL_H

#include <stdint.h>

const int TARGET_INFO_NUM_PER_PACKET = 10;

#pragma pack(push)
#pragma pack(1)
struct ZardTrackInfo
{
    int32_t trackNo;
    int64_t datetime;
    float range;
    float azimu;
    float elev;
    float vr;
    int32_t amp_sigma;
    int32_t longitude;
    int32_t latitude;
    float altitude;
    float ve;
    float vn;
    float vz;
    float x;
    float y;
    float z;
    int32_t correlated;
    int32_t track_times;
    int32_t losted_times;
    uint8_t device_id;
    uint8_t reserve[7];
};

struct ZardTrackInfoUdpPacket
{
    ZardTrackInfoUdpPacket()
    {
        header[0] = 0x77;
        header[1] = 0xFF;
        header[2] = 0xFF;
        header[3] = 0xFF;
        header[4] = 0xFF;
        header[5] = 0xFF;

        header[6] = 0x77;
        header[7] = 0xCC;
        header[8] = 0xCC;
        header[9] = 0xCC;
        header[10] = 0xCC;
        header[11] = 0xCC;
    }
    uint8_t header[12];
    int32_t sector_index;
    int32_t track_size;
    ZardTrackInfo targets[TARGET_INFO_NUM_PER_PACKET];
};

// 系统状态报文
struct ZardRadarSystemInfoUdpPacket
{
    uint8_t header[4];
    uint8_t radar_id;
    int64_t timestamp;
    int32_t radar_lon;
    int32_t radar_lat;
    float radar_alt;
    uint8_t radar_work_state;
    uint16_t radar_azim_deg;
    int16_t radar_elev_deg;
    uint8_t is_working_abnormally_flag;
    uint8_t abnormal_index;
    uint8_t preserve[35];
    uint8_t check_byte;
    ZardRadarSystemInfoUdpPacket()
    {
        header[0] = 0x5a;
        header[1] = 0x5a;
        header[2] = 0x08;
        header[3] = 0x02;
        radar_id = 0x01;
        timestamp = 0;
        radar_lon = 0;
        radar_lat = 0;
        radar_alt = 0;
        radar_work_state = 0;
        radar_azim_deg = 0;
        radar_elev_deg = 0;
        is_working_abnormally_flag = 0;
        abnormal_index = 0;
        for (int ii = 0; ii < 35; ++ii)
        {
            preserve[ii] = 0;
        }
        check_byte = 0;
    }
};

// 指控协议
struct OrderPacketFromController
{
    uint8_t src;
    uint8_t dst;
    uint8_t packet_type;
    uint16_t packet_length;
    uint8_t radar_id;
    uint8_t radar_mission_mode;
    uint8_t radar_work_mode;
    uint8_t elev_beams;
    uint8_t reset_to_default_flag;
    uint8_t enable_control_turntable_flag;
    uint8_t turntable_rotating_mode;
    uint16_t turntable_rotating_settings;
    uint8_t enable_control_antenna_flag;
    uint8_t antenna_power_mode;
    uint8_t antenna_fre_code;
    uint8_t enter_or_quit_track_mode;
    uint32_t track_id;
    uint8_t reserve1[2];
    uint8_t enable_control_power_off_area_1;
    uint16_t power_off_azim_deg_floor_1;
    uint16_t power_off_azim_deg_ceil_1;
    uint8_t reserve2[10];
    uint8_t send_track_method;
    uint32_t selected_track_id_to_send;
    uint8_t enable_set_ip_and_port;
    uint8_t reserve3[2];
    uint32_t dst_ip_address;
    uint16_t dst_port;
    uint8_t enable_set_radar_lon_lat_alt_flag;
    int32_t radar_lon; // 经度*1e6, 如123.456789发送123456789
    int32_t radar_lat; // 纬度*1e6, 如12.345678发送12345678
    int32_t radar_alt; // 海拔*10, 如12345.6发送123456
    uint8_t enable_set_azim_bias_flag;
    int16_t azim_bias;
    uint8_t reserve4[10];
    uint8_t check_byte;

    OrderPacketFromController();
};

#pragma pack(pop)

const int PACKET_LENGTH_OF_ORDER_PACKET = sizeof(OrderPacketFromController);

struct OrderFromController
{
    int src;
    int dst;
    int packet_type;
    int packet_length;
    int radar_id;
    int radar_mission_mode;
    int radar_work_mode;
    int elev_beams;
    int reset_to_default_flag;
    int enable_control_turntable_flag;
    int turntable_rotating_mode;
    int turntable_rotating_settings;
    int enable_control_antenna_flag;
    int antenna_power_mode;
    int antenna_fre_code;
    int enter_or_quit_track_mode;
    int track_id;
    int enable_control_power_off_area_1;
    double power_off_azim_deg_floor_1;
    double power_off_azim_deg_ceil_1;
    int send_track_method;
    int selected_track_id_to_send;
    int enable_set_ip_and_port;
    unsigned int dst_ip_address;
    int dst_port;
    int enable_set_radar_lon_lat_alt_flag;
    double radar_lon;
    double radar_lat;
    double radar_alt;
    int enable_set_azim_bias_flag;
    double azim_bias;

    OrderFromController();
};

#endif // JOINT_PROTOCOL_H
