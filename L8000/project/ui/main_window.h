#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTranslator>
#include "global_settings.h"
#include "adapters_select_dialog.h"
#include "calculate_range_and_doppler_unit_dialog.h"
#include "convert_lon_lat_to_range_azim_dialog.h"
#include "radar_data_processing_settings_dialog.h"
#include "radar_hardware_settings_dialog.h"
#include "radar_software_settings_dialog.h"
#include "my_graphics_view.h"
#include "track_displayer_scene.h"
#include "communicate_with_fpga_thread.h"
#include "control_and_communicate_with_controller_thread.h"
#include "radar_data_processing_thread.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void startWorking();

public:
    AdaptersSelectDialog* adapters_select_dialog;

private:
    Ui::MainWindow* ui;

    CalculateRangeAndDopplerUnitDialog* calculate_range_and_doppler_unit_dialog;
    ConvertLonLatToRangeAzimDialog* convert_lon_Lat_to_range_azim_dialog;
    RadarDataProcessingSettingsDialog* radar_data_processing_settings_dialog;
    RadarHardwareSettingsDialog* radar_hardware_settings_dialog;
    RadarSoftwareSettingsDialog* radar_software_settings_dialog;

    RadarSystemState local_radar_system_state;
    int current_data_processing_mode;

    TrackDisplayerScene* track_displayer_scene;
    MyGraphicsView* my_graphics_view;
    QTimer* track_displayer_view_timer;

    CommunicateWithFpgaThread* communicate_with_fpga_thread;
    ControlAndCommunicateWithControllerThread* control_and_communicate_with_controller_thread;
    RadarDataProcessingThread* radar_data_processing_thread;

    RadarMissionModeSettings local_radar_mission_mode_settings;
    SoftwareSettings local_software_settings;
    ReadFpgaPacketFileSettings local_read_fpga_packet_file_settings;

    QTranslator* translator;
    bool is_chinese_flag;

private:
    void updateRadarSystemInfoOnMainWindow();

signals:
    void sigSendWorkOrderToFpga(const RadarMissionModeSettings& new_radar_mission_mode_settings);
    void sigSendStopOrderToFpga();
    void sigSendResetOrderToFpga();
    void sigUpdateCurrentSetSprModeIndex(const int& new_spr_mode_index);

    void sigSendNewDbfCorrectCoefFileToFpga(const QString& new_master_correct_coef_file_dir, const QString& new_slave_correct_coef_file_dir);

    void sigStartReadingFpgaPacketFile(const ReadFpgaPacketFileSettings& new_read_fpga_packet_file_settings);
    void sigStopReadingFpgaPacketFile();
    void sigAddTrackIdToSendingTargetSet(const int& new_track_id);
    void sigPrepareToEnterTrackMode(const int& new_track_id);
    void sigClickStopToQuitTrackMode();

private slots:
    // 菜单栏
    void sltOnTriggerActionChangeAdapter();
    void sltOnTriggerActionHardwareSettings();
    void sltOnTriggerActionSoftwareSettings();
    void sltOnTriggerActionDataProcessingSettings();
    void sltOnTriggerActionChangeLanguage();

    // 雷达控制
    void sltOnClickPushButtonWork();
    void sltOnClickPushButtonStop();
    void sltOnClickPushButtonReset();
    void sltOnClickPushButtonOtherRadarSettings();
    // 修改当前模式
    void sltComboBoxRadarMissionModeChanged();

    // 数据回读
    void sltOnCheckBoxReadFpgaPacketFileChanged(const int& state);
    void sltOnClickPushButtonSelectFile();

    // 航迹交互
    void sltOnClickPushButtonSendThisTrack();
    void sltOnClickPushButtonTrackThisTarget();

    // 更新系统状态
    void sltUpdateRadarSystemInfoOnMainWindow(const RadarSystemState& new_radar_system_state);

    // 更新航迹显示界面
    void sltUpdateTrackDisplayerView();
    void sltUpdateTableWidgetTrackInfo();
    void sltUpdateOriginalPointItems();
    void sltUpdateTargetTrackItems();

    // 跟其他线程交互
    void sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sltCaptureAdapterStopWorking();
    void sltUpdateDataProcessingMode(const int& new_data_processing_mode);

    // 更新 statusBar, 所有的信息统一用这个槽函数来更新
    void sltUpdateStatusBar(const QString& new_message);
};

#endif // MAIN_WINDOW_H
