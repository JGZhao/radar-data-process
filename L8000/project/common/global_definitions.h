#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

/*
 * 功能: 存放全局的常量
 */

#include <QString>
#include <cmath>
using namespace std;

// 常量
const double EARTH_RADIUS = 6378137.0;
const double PI = acos(-1);
const double TWO_PI = 2 * PI;
const double DEG2RAD = PI / 180;
const double RAD2DEG = 180 / PI;
const int TWO_BYTE_UINT_MAX = 65536;
const int TWO_BYTE_INT_MIN = -32768;
const int TWO_BYTE_INT_MAX = 32767;
const int UINT_INF = 0x7fffffff;
const double EPS = 1e-6;
const double FREE_TIME_MAX = TWO_BYTE_UINT_MAX * 0.2 / 1000; // 自由时间最大值
// 实际采用累积的自由时间, 因为丢失几次可能导致自由时间混叠, 造成航迹相关中自由时间存在误差

// 雷达相关
const int RADAR_MISSION_NUM = 8;    // 任务模式, 近搜、远搜、近远搜、超远搜、近跟、近跟B、远跟、远跟B
const int RADAR_MODE_NUM = 7;       // 工作模式, 近搜、远搜、超远搜、近跟、近跟B、远跟、远跟B
const int RADAR_SCAN_MODE_NUM = 3;  // 搜索模式的数量

const int NEAR_SCAN_MODE_INDEX = 0; // 0-近搜 1_远搜 2_超远搜 3_近跟 4_近跟B 5_远跟 6_远跟B
const int FAR_SCAN_MODE_INDEX = 1;
const int SUPER_FAR_SCAN_MODE_INDEX = 2;
const int NEAR_TRACK_MODE_INDEX = 3;
const int NEAR_TRACK_B_MODE_INDEX = 4;
const int FAR_TRACK_MODE_INDEX = 5;
const int FAR_TRACK_B_MODE_INDEX = 6;

const int RADAR_SCAN_TRACK_MODE_DIVISION_INDEX = NEAR_TRACK_MODE_INDEX;  // 大于等于 RADAR_SCAN_TRACK_MODE_DIVISION_INDEX 为跟踪模式
const int AZIM_SECTOR_NUM = 120;                                         // 方位扇区数量
const double AZIM_SECTOR_BEAM_WIDTH = 360.0 / AZIM_SECTOR_NUM;           // 方位扇区对应的波束宽度
const double HALF_AZIM_SECTOR_BEAM_WIDTH = AZIM_SECTOR_BEAM_WIDTH / 2.0; // 方位扇区对应的波束宽度的一半
const int HALF_OF_AZIM_SECTOR_NUM = AZIM_SECTOR_NUM / 2;                 // 方位扇区数量的一半
const int ELEV_BEAM_NUM_SCAN_MODE = 10;                                  // 俯仰波束数量（搜索模式）
const int ELEV_BEAM_NUM_TRACK_MODE = 4;                                  // 俯仰波束数量（跟踪模式）
const int MAX_ELEV_BEAM_INDEX = ELEV_BEAM_NUM_SCAN_MODE - 1;             // 俯仰下标的最大值
const int MAX_VALID_ELEV_BEAM_INDEX = MAX_ELEV_BEAM_INDEX - 1;

// 输出信息到文件
const int MAX_ITEM_NUM_FLUSH = 50000;                                                                                      // 多少条刷新一次缓存
const QString RECORD_FILES_ROOT_FOLDER_NAME = "record_files";                                                              // 数据存储根目录
const QString RADAR_SYSTEM_INFO_FOLDER_NAME = "radar_system_info";                                                         // 系统状态文件夹
const QString RADAR_SYSTEM_INFO_FOLDER_DIR = QString(RECORD_FILES_ROOT_FOLDER_NAME + "/" + RADAR_SYSTEM_INFO_FOLDER_NAME); // 系统状态目录
const QString RADAR_SYSTEM_INFO_POSTFIX = "radar_system_info.txt"; // 系统状态文件名称后缀（实际文件名为: 时间+后缀）
const int MAX_RADAR_SYSTEM_INFO_NUM_PER_FILE = 10000000;           // 每一包最多存放的雷达系统信息的数量
const int MAX_RADAR_SYSTEM_INFO_FILE_NUM = 5;                      // 最多存放的雷达系统信息的文件数

const QString RADAR_ORIGINAL_POINTS_FOLDER_NAME = "radar_original_points";                                                         // 原始点迹文件夹
const QString RADAR_ORIGINAL_POINTS_FOLDER_DIR = QString(RECORD_FILES_ROOT_FOLDER_NAME + "/" + RADAR_ORIGINAL_POINTS_FOLDER_NAME); // 原始点迹目录
const QString RADAR_ORIGINAL_POINTS_POSTFIX = "radar_original_points.txt"; // 原始点迹文件名称后缀（实际文件名为: 时间+后缀）
const int MAX_RADAR_ORIGINAL_POINTS_NUM_PER_FILE = 50000000;               // 每一包最多存放的原始点迹的数量
const int MAX_RADAR_ORIGINAL_POINTS_FILE_NUM = 5;                          // 最多存放的原始点迹的文件数

const QString RADAR_TRACK_POINTS_FOLDER_NAME = "radar_track_points";                                                         // 航迹点迹文件夹
const QString RADAR_TRACK_POINTS_FOLDER_DIR = QString(RECORD_FILES_ROOT_FOLDER_NAME + "/" + RADAR_TRACK_POINTS_FOLDER_NAME); // 航迹点迹目录
const QString RADAR_TRACK_POINTS_POSTFIX = "radar_track_points.txt"; // 航迹文件名称后缀（实际文件名为: 时间+后缀）
const int MAX_RADAR_TRACK_POINTS_NUM_PER_FILE = 50000000;            // 每一包最多存放的航迹点迹的数量
const int MAX_RADAR_TRACK_POINTS_FILE_NUM = 5;                       // 最多存放的航迹点迹的文件数

const QString FPGA_PACKET_FOLDER_NAME = "fpga_packet";                                                         // FPGA数据报文文件夹
const QString FPGA_PACKET_FOLDER_DIR = QString(RECORD_FILES_ROOT_FOLDER_NAME + "/" + FPGA_PACKET_FOLDER_NAME); // FPGA数据报文目录
const QString FPGA_PACKET_POSTFIX = "fpga_packet.txt"; // FPGA数据报文名称后缀（实际文件名为: 时间+后缀）
const int MAX_FPGA_PACKET_FILE_NUM = 5;                // 最多存放的FPGA报文文件的数量
const int MAX_FPGA_PACKET_NUM_PER_FILE = 120000;       // 每一包最多存放的FPGA报文的数量

#endif // GLOBAL_DEFINITIONS_H
