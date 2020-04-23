#ifndef PROTOCOL_WITH_FPGA_H
#define PROTOCOL_WITH_FPGA_H

#include <stdint.h>
#include "global_definitions.h"

// 0-近搜 1_远搜 2_超远搜 3_近跟 4_近跟B 5_远跟 6_远跟B
const int RADAR_MODE_CONFIG_BYTE[RADAR_MODE_NUM] = {0x33, 0x44, 0x66, 0x69, 0x77, 0x96, 0x88};
const int NEAR_FAR_TRACK_MODE_INDEX_DIVISION = FAR_TRACK_MODE_INDEX; // 大于等于 NEAR_FAR_TRACK_MODE_INDEX_DIVISION 为远跟

// 发送给 FPGA 的报文
struct CutAndExcludeSettings
{
    uint8_t damp_cut_division_index[2];
    uint8_t damp_cut_near;
    uint8_t damp_cut_far;
    uint8_t doppler_exclude_1[2];
    uint8_t doppler_exclude_2[2];
    uint8_t doppler_exclude_3[2];
    uint8_t doppler_exclude_4[2];
    uint8_t doppler_threshold[2];
    uint8_t range_threshold_division_index_n_f[2];
    uint8_t range_threshold_near[2];
    uint8_t range_threshold_far[2];
};

struct ToFpgaPacket
{
    uint8_t dest_addr[6];                              // 0-5
    uint8_t src_addr[6];                               // 6-11
    uint8_t packet_type[2];                            // 12-13
    uint8_t head1;                                     // 14
    uint8_t track_elev_angle[8];                       // 15-22 俯仰四个跟踪角度
    uint8_t track_azim_angle[2];                       // 23-24
    uint8_t radar_mission_mode;                        // 25   0x55：搜索模式 0x69：近距离跟踪模式 0x96：远距离跟踪模式
    uint8_t sigma_decay_1;                             // 26
    uint8_t sigma_decay_2_3;                           // 27
    uint8_t protect_decay_1;                           // 28
    uint8_t protect_decay_2_3;                         // 29
    uint8_t antenna_raising_angle_and_fre_code;        // 30
    uint8_t antenna_power_mode;                        // 31
    uint8_t antenna_enable;                            // 32
    uint8_t radar_test_mode;                           // 33
    uint8_t head2;                                     // 34
    uint8_t turntable_rotating_mode;                   // 35
    uint8_t turntable_set[2];                          // 36-37
    uint8_t channel_amp_phase_control_code[2];         // 38-39
    uint8_t dbf_weight_method[2];                      // 40-41
    uint8_t turntable_enable;                          // 42
    uint8_t stc_select;                                // 43
    uint8_t channel_amp_phase_control_mode;            // 44
    uint8_t channel_num;                               // 45
    uint8_t amp_code;                                  // 46
    uint8_t reserve1[8];                               // 47-54
    CutAndExcludeSettings cut_and_exclude_settings[4]; // 56-134
    uint8_t scan_fft_cut;                              // 135
    uint8_t track_fft_cut;                             // 136
    uint8_t work_or_serdes;                            // 137
    uint8_t work_or_emulate;                           // 138
    uint8_t reserve3;                                  // 139
};

// 目标信息
struct TargetInfo
{
    uint8_t range_index[2];
    uint8_t amp_exp_doppler_high; // bit[7:3]: 目标能量指数  [1:0]目标速度信息高两位                                   bit[1:0]目标速度信息高2位
    uint8_t doppler_low_index;    // 目标速度信息低8位
    uint8_t amp_sigma[2];
    uint8_t amp_delta_azim[2];
    uint8_t amp_delta_elev1[2];
    uint8_t amp_delta_elev2[2];
    uint8_t sigma_I[2];
    uint8_t sigma_Q[2];
    uint8_t delta_azim_I[2];
    uint8_t delta_azim_Q[2];
    uint8_t delta_elev1_I[2];
    uint8_t delta_elev1_Q[2];
    uint8_t delta_elev2_I[2];
    uint8_t delta_elev2_Q[2];
    uint8_t neighbour_target_num;
};

struct FpgaPacket
{
    uint8_t dest_addr[6];                            // 0-5
    uint8_t src_addr[6];                             // 6-11
    uint8_t packet_type[2];                          // 12-13
    uint8_t azim_angle[2];                           // 14-15  (hex->dec)/(2^16-1)*360
    uint8_t elev_index;                              // 16  搜索模式下为1-10（对应0-30的10个角度）跟踪模式传角度的下标（0-9）
    uint8_t dbf_coef_receive_raising_angle_index;    // 17
    uint8_t radar_system_status;                     // 18
    uint8_t target_num;                              // 19
    uint8_t radar_work_mode;                         // 20
    uint8_t radar_set_work_mode;                     // 21
    uint8_t BISS_angle[2];                           // 22-23
    uint8_t turntable_rotating_speed[2];             // 24-25
    uint8_t track_azim_set[2];                       // 26-27
    uint8_t antenna_temperature_warning_or_fre_code; // 28
    uint8_t antenna_command_code;                    // 29
    uint8_t antenna_work_mode_or_in_current_28v;     // 30
    uint8_t bank_report_mode;                        // 31
    uint8_t T_AD_FPGA1[2];                           // 32-33
    uint8_t T_AD_FPGA2[2];                           // 34-35
    uint8_t T_QDR_FPGA1[2];                          // 36-37
    uint8_t T_QDR_FPGA2[2];                          // 38-39
    uint8_t T_QDR_FPGA3[2];                          // 40-41
    uint8_t T_QDR_FPGA4[2];                          // 42-43
    uint8_t GMAC_packet_num[2];                      // 44-45
    uint8_t free_time[2];                            // 46-47
    uint8_t current_12v;                             // 48
    uint8_t current_28v_or_12v;                      // 49
    uint8_t out_put_power_1_or_T;                    // 50
    uint8_t out_put_power_2_or_T;                    // 51
    uint8_t antenna_data_parity;                     // 52
    uint8_t reserve;                                 // 53
    uint8_t ATP_azim_position[4];                    // 54-57
    uint8_t ATP_azim_speed[4];                       // 58-61
    uint8_t ATP_azim_acceleration[4];                // 62-65
    uint8_t ATP_cyclic_code;                         // 66
    uint8_t ATP_check_code;                          // 67
    TargetInfo target_info[40];                      // 68-1227
    uint8_t AD_FPGA_serial_number[8];                // 1228-1235
    uint8_t reserve2[93];                            // 1236
    uint8_t FCS[4];                                  // 1329
};

const int TO_FPGA_PACKET_SIZE = sizeof(ToFpgaPacket);
const int FPGA_PACKET_SIZE = sizeof(FpgaPacket);

void generateHead(ToFpgaPacket* to_FPGA_packet);
void initializeToFpgaPacket(ToFpgaPacket* to_FPGA_packet);

#endif // PROTOCOL_WITH_FPGA_H
