#include "main_window.h"
#include "ui_main_window.h"
#include <QMessageBox>
#include <QDebug>
#include <QMetaType>
#include <QFileDialog>
#include "global_functions.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit_system_info->document()->setMaximumBlockCount(100);
    connect(ui->pushButton_clear_system_info, &QAbstractButton::clicked, ui->textEdit_system_info, &QTextEdit::clear);

    // 设置航迹显示及跟踪
    ui->tableWidget_track_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 先自适应宽度
    //    ui->tableWidget_track_info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); //然后设置要根据内容使用宽度的列
    ui->tableWidget_track_info->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置不可输入
    // 显示内容居中
    for (int ii = 0; ii < ui->tableWidget_track_info->rowCount(); ++ii)
    {
        for (int jj = 0; jj < ui->tableWidget_track_info->columnCount(); ++jj)
        {
            if (ui->tableWidget_track_info->item(ii, jj))
            {
                ui->tableWidget_track_info->item(ii, jj)->setText(QString::number(0));
            }
            else
            {
                ui->tableWidget_track_info->setItem(ii, jj, new QTableWidgetItem(QString::number(0)));
            }

            ui->tableWidget_track_info->item(ii, jj)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // 注册自定义类型
    qRegisterMetaType<RadarMissionModeSettings>("RadarMissionModeSettings");
    qRegisterMetaType<HardwareSettings>("HardwareSettings");
    qRegisterMetaType<SoftwareSettings>("SoftwareSettings");
    qRegisterMetaType<DbfCorrectCoefFileDirSettings>("DbfCorrectCoefFileDirSettings");
    qRegisterMetaType<DataProcessingSettings>("DataProcessingSettings");
    qRegisterMetaType<ReadFpgaPacketFileSettings>("ReadFpgaPacketFileSettings");
    qRegisterMetaType<FpgaPacket*>("FpgaPacket*");
    qRegisterMetaType<RadarSystemState>("RadarSystemState");
    qRegisterMetaType<QList<TargetTrackPoint>*>("QList<TargetTrackPoint>*");
    qRegisterMetaType<HardwareSettingsTrackMode>("HardwareSettingsTrackMode");

    adapters_select_dialog = new AdaptersSelectDialog(this);
    calculate_range_and_doppler_unit_dialog = new CalculateRangeAndDopplerUnitDialog(this);
    convert_lon_Lat_to_range_azim_dialog = new ConvertLonLatToRangeAzimDialog(this);
    radar_data_processing_settings_dialog = new RadarDataProcessingSettingsDialog(this);
    radar_hardware_settings_dialog = new RadarHardwareSettingsDialog(this);
    radar_software_settings_dialog = new RadarSoftwareSettingsDialog(this);

    initializeRadarSystemState(local_radar_system_state);
    current_data_processing_mode = RADAR_SCAN_MODE;

    track_displayer_scene = new TrackDisplayerScene(this);
    my_graphics_view = new MyGraphicsView(ui->frame_track_displayer);
    QGridLayout* grid_layout_on_frame = new QGridLayout(ui->frame_track_displayer);
    grid_layout_on_frame->addWidget(my_graphics_view);
    my_graphics_view->setScene(track_displayer_scene);
    my_graphics_view->scale(0.1, 0.1);
    my_graphics_view->show();

    track_displayer_view_timer = new QTimer(this);
    track_displayer_view_timer->setInterval(SCAN_LINE_UPDATE_INTERVAL);
    connect(track_displayer_view_timer, &QTimer::timeout, this, &MainWindow::sltUpdateTrackDisplayerView);

    communicate_with_fpga_thread = new CommunicateWithFpgaThread(this);
    control_and_communicate_with_controller_thread = new ControlAndCommunicateWithControllerThread(this);
    radar_data_processing_thread = new RadarDataProcessingThread(this);

    initializeRadarMissionModeSettings(local_radar_mission_mode_settings);
    initializeReadFpgaPacketFileSettings(local_read_fpga_packet_file_settings);

    translator = new QTranslator(this);
    is_chinese_flag = true;

    connect(adapters_select_dialog, &AdaptersSelectDialog::sigChangeCaptureAdapter, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltChangeCaptureAdapter);
    connect(ui->action_change_adapter, &QAction::triggered, this, &MainWindow::sltOnTriggerActionChangeAdapter);
    connect(ui->action_hardware_settings, &QAction::triggered, this, &MainWindow::sltOnTriggerActionHardwareSettings);
    connect(ui->action_software_settings, &QAction::triggered, this, &MainWindow::sltOnTriggerActionSoftwareSettings);
    connect(ui->action_data_processing_settings, &QAction::triggered, this, &MainWindow::sltOnTriggerActionDataProcessingSettings);
    connect(ui->action_display_radar_system_state_dock, &QAction::triggered, ui->dockWidget_radar_system_state, &QDockWidget::show);
    connect(ui->action_change_language, &QAction::triggered, this, &MainWindow::sltOnTriggerActionChangeLanguage);

    connect(ui->action_calculate_range_unit_and_doppler_unit, &QAction::triggered, calculate_range_and_doppler_unit_dialog, &CalculateRangeAndDopplerUnitDialog::show);
    connect(ui->action_convert_lon_lat_range_azim, &QAction::triggered, convert_lon_Lat_to_range_azim_dialog, &ConvertLonLatToRangeAzimDialog::show);

    connect(ui->pushButton_work, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonWork);
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonStop);
    connect(ui->pushButton_reset, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonReset);
    connect(ui->pushButton_other_radar_settings, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonOtherRadarSettings);
    connect(ui->comboBox_radar_mission_mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::sltComboBoxRadarMissionModeChanged);
    connect(this, &MainWindow::sigUpdateCurrentSetSprModeIndex, radar_hardware_settings_dialog, &RadarHardwareSettingsDialog::sltUpdateCurrentSetSprModeIndex);

    ui->checkBox_read_fpga_packet_file->setChecked(false);
    ui->pushButton_select_fpga_packet_file->setEnabled(false);
    ui->lineEdit_selected_fpga_packet_file_directory->setEnabled(false);
    connect(ui->checkBox_read_fpga_packet_file, &QCheckBox::stateChanged, this, &MainWindow::sltOnCheckBoxReadFpgaPacketFileChanged);
    connect(ui->pushButton_select_fpga_packet_file, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonSelectFile);
    connect(this, &MainWindow::sigStartReadingFpgaPacketFile, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltStartReadingFpgaPacketFile);
    connect(this, &MainWindow::sigStopReadingFpgaPacketFile, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltStopReadingFpgaPacketFile);

    connect(ui->pushButton_send_this_track, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonSendThisTrack);
    connect(this, &MainWindow::sigAddTrackIdToSendingTargetSet, radar_data_processing_thread, &RadarDataProcessingThread::sigAddTrackIdToSendingTargetSet);
    connect(this,
            &MainWindow::sigAddTrackIdToSendingTargetSet,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigAddTrackIdToSendingTargetSet);
    connect(ui->pushButton_track_this_target, &QPushButton::clicked, this, &MainWindow::sltOnClickPushButtonTrackThisTarget);
    connect(this, &MainWindow::sigPrepareToEnterTrackMode, radar_data_processing_thread, &RadarDataProcessingThread::sigPrepareToEnterTrackMode);

    connect(radar_data_processing_thread, &RadarDataProcessingThread::sigUpdateRadarSystemState, this, &MainWindow::sltUpdateRadarSystemInfoOnMainWindow);
    connect(radar_data_processing_thread, &RadarDataProcessingThread::sigUpdateOriginalPointItems, this, &MainWindow::sltUpdateOriginalPointItems);
    connect(radar_data_processing_thread, &RadarDataProcessingThread::sigUpdateTargetTrackItems, this, &MainWindow::sltUpdateTargetTrackItems);

    connect(communicate_with_fpga_thread, &CommunicateWithFpgaThread::sigCaptureAdapterStopWorking, this, &MainWindow::sltCaptureAdapterStopWorking);

    // 初始化各线程中的硬件设置
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeHardwareSettingsInOtherThreads,
            radar_hardware_settings_dialog,
            &RadarHardwareSettingsDialog::sltUpdateLocalHardwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeHardwareSettingsInOtherThreads,
            track_displayer_scene,
            &TrackDisplayerScene::sltUpdatePowerOffSectorItem);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeHardwareSettingsInOtherThreads,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltUpdateLocalHardwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeHardwareSettingsInOtherThreads,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateLocalHardwareSettings);

    // 初始化各线程中的软件设置
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeSoftwareSettingsInOtherThreads,
            this,
            &MainWindow::sltUpdateLocalSoftwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeSoftwareSettingsInOtherThreads,
            track_displayer_scene,
            &TrackDisplayerScene::sltUpdateCirclesAndAngleDialLinesAndPowerOffSectorItem);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeSoftwareSettingsInOtherThreads,
            radar_software_settings_dialog,
            &RadarSoftwareSettingsDialog::sltUpdateLocalSoftwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeSoftwareSettingsInOtherThreads,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltUpdateLocalSoftwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeSoftwareSettingsInOtherThreads,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateLocalSoftwareSettings);

    // 初始化各线程中的 DBF 校准系数设置
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads,
            radar_software_settings_dialog,
            &RadarSoftwareSettingsDialog::sltUpdateLocalDbfCorrectCoefFileDirSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeDbfCorrectCoefFileDirSettingsInOtherThreads,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltUpdateLocalDbfCorrectCoefFileDirSettings);

    // 初始化各线程中的数据处理参数设置
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeDataProcessingSettingsInOtherThreads,
            radar_data_processing_settings_dialog,
            &RadarDataProcessingSettingsDialog::sltUpdateLocalDataProcessingSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigInitializeDataProcessingSettingsInOtherThreads,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateLocalDataProcessingSettings);

    // 更新各线程中的硬件设置
    connect(radar_hardware_settings_dialog,
            &RadarHardwareSettingsDialog::sigUpdateLocalHardwareSettings,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalHardwareSettings);
    connect(radar_hardware_settings_dialog, &RadarHardwareSettingsDialog::sigUpdateLocalHardwareSettings, track_displayer_scene, &TrackDisplayerScene::sltUpdatePowerOffSectorItem);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalHardwareSettings,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltUpdateLocalHardwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalHardwareSettings,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateLocalHardwareSettings);

    // 更新各线程中的软件设置
    connect(radar_software_settings_dialog, &RadarSoftwareSettingsDialog::sigUpdateSoftwareSettings, this, &MainWindow::sltUpdateLocalSoftwareSettings);
    connect(radar_software_settings_dialog,
            &RadarSoftwareSettingsDialog::sigUpdateSoftwareSettings,
            track_displayer_scene,
            &TrackDisplayerScene::sltUpdateCirclesAndAngleDialLinesAndPowerOffSectorItem);
    connect(radar_software_settings_dialog,
            &RadarSoftwareSettingsDialog::sigUpdateSoftwareSettings,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalSoftwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalSoftwareSettings,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltUpdateLocalSoftwareSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalSoftwareSettings,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateLocalSoftwareSettings);

    // 更新各线程中的 DBF 校准系数设置
    connect(radar_software_settings_dialog,
            &RadarSoftwareSettingsDialog::sigUpdateDbfCorrectCoefFileDirSettings,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalDbfCorrectCoefFileDirSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalDbfCorrectCoefFileDirSettings,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltUpdateLocalDbfCorrectCoefFileDirSettings);

    // 更新各线程中的数据处理参数设置
    connect(radar_data_processing_settings_dialog,
            &RadarDataProcessingSettingsDialog::sigUpdateLocalDataProcessingSettings,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalDataProcessingSettings);
    connect(control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalDataProcessingSettings,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateLocalDataProcessingSettings);

    // 开始、停止、复位
    connect(this, &MainWindow::sigSendWorkOrderToFpga, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltSendWorkOrderToFpga);
    connect(this, &MainWindow::sigSendStopOrderToFpga, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltSendStopOrderToFpga);
    connect(this, &MainWindow::sigSendResetOrderToFpga, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltSendResetOrderToFpga);

    // 静默
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendPowerOffOrderToFpga,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltSendPowerOffOrderToFpga);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendPowerOnOrderToFpga,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltSendPowerOnOrderToFpga);

    // 跟踪模式
    connect(this, &MainWindow::sigClickStopToQuitTrackMode, radar_data_processing_thread, &RadarDataProcessingThread::sigClickStopToQuitTrackMode);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendStopPointingAndTrackOrderToEnterTrackMode,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltSendStopPointingAndTrackOrderToEnterTrackMode);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendNewHardwareSettingsTrackModeToFpga,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltSendNewHardwareSettingsTrackModeToFpga);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigQuitTrackModeToScanMode,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltQuitTrackModeToScanMode);
    connect(radar_data_processing_thread, &RadarDataProcessingThread::sigUpdateDataProcessingMode, this, &MainWindow::sltUpdateDataProcessingMode);

    // 收取FPGA报文和数据处理报文
    connect(communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sigProcessingNextFpgaPacket,
            radar_data_processing_thread,
            &RadarDataProcessingThread::sigProcessingNextFpgaPacket);

    // 发送报文给指控
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendTrackLastPointListToController,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigSendTrackLastPointListToController);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigRemoveTrackIdFromSendingTargetSet,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigRemoveTrackIdFromSendingTargetSet);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigUpdateRadarSystemState,
            control_and_communicate_with_controller_thread,
            &ControlAndCommunicateWithControllerThread::sigUpdateLocalRadarSystemState);

    // 发送配置校准系数
    connect(this, &MainWindow::sigSendNewDbfCorrectCoefFileToFpga, communicate_with_fpga_thread, &CommunicateWithFpgaThread::sltSendNewDbfCorrectCoefFileToFpga);
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendDbfCorrectCoefFileToFpgaWhenNoneCoef,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltSendNewDbfCorrectCoefFileToFpga);

    // Serdes 异常时发送复位报文
    connect(radar_data_processing_thread,
            &RadarDataProcessingThread::sigSendResetOrderToFpgaSerdesError,
            communicate_with_fpga_thread,
            &CommunicateWithFpgaThread::sltSendResetOrderToFpga);

    // 更新系统界面 statusBar
    connect(communicate_with_fpga_thread, &CommunicateWithFpgaThread::sigUpdateStatusBarOnMainWindow, this, &MainWindow::sltUpdateStatusBar);
    connect(radar_data_processing_thread, &RadarDataProcessingThread::sigUpdateStatusBarOnMainWindow, this, &MainWindow::sltUpdateStatusBar);

    // 初始化系统状态界面
    updateRadarSystemInfoOnMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
    communicate_with_fpga_thread->stopWorking();
    radar_data_processing_thread->stopWorking();
    control_and_communicate_with_controller_thread->stopWorking();
}

void MainWindow::startWorking()
{
    qDebug() << tr("[MainWindow]: startWorking!");
    control_and_communicate_with_controller_thread->startWorking();
    radar_data_processing_thread->startWorking();
    communicate_with_fpga_thread->startWorking();
    track_displayer_view_timer->start();
}

void MainWindow::updateRadarSystemInfoOnMainWindow()
{
    if (local_radar_system_state.radar_connected_flag)
    {
        ui->lineEdit_mainwindow_radar_connecting_state->setText(tr(" 连通 "));
    }
    else
    {
        ui->lineEdit_mainwindow_radar_connecting_state->setText(tr(" 断开 "));
    }
    ui->lineEdit_mainwindow_GMAC->setText(QString::number(local_radar_system_state.current_GMAC_count));
    ui->lineEdit_mainwindow_azim_deg_to_north->setText(QString::number(local_radar_system_state.current_azim_deg_to_north));
    ui->lineEdit_mainwindow_elev_deg->setText(QString::number(local_radar_system_state.current_elev_deg));
    ui->lineEdit_mainwindow_freq_code->setText(QString::number(local_radar_system_state.fre_code));
    ui->lineEdit_mainwindow_rotating_speed->setText(QString::number(local_radar_system_state.turntable_rotating_speed, 'f', 2));

    ui->lineEdit_current_azim_deg_to_north->setText(QString::number(local_radar_system_state.current_azim_deg_to_north, 'f', 2));
    ui->lineEdit_current_azim_deg->setText(QString::number(local_radar_system_state.current_azim_deg, 'f', 2));
    ui->lineEdit_transmit_azim_deg->setText(QString::number(local_radar_system_state.current_transmit_azim_deg, 'f', 2));
    ui->lineEdit_transmit_azim_center->setText(QString::number(local_radar_system_state.current_transmit_azim_center, 'f', 2));
    ui->lineEdit_antenna_elev_deg->setText(QString::number(local_radar_system_state.current_elev_deg));
    ui->lineEdit_radar_state_register->setText(QString::number(local_radar_system_state.radar_state_register));
    ui->lineEdit_target_num->setText(QString::number(local_radar_system_state.target_num));
    switch (local_radar_system_state.radar_mission_mode)
    {
    case 0x00:
        ui->lineEdit_radar_work_mode->setText(tr(" 近搜 "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 近搜 "));
        break;
    case 0x01:
        ui->lineEdit_radar_work_mode->setText(tr(" 远搜 "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 远搜 "));
        break;
    case 0x02:
        ui->lineEdit_radar_work_mode->setText(tr(" 近跟 "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 近跟 "));
        break;
    case 0x03:
        ui->lineEdit_radar_work_mode->setText(tr(" 远跟 "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 远跟 "));
        break;
    case 0x04:
        ui->lineEdit_radar_work_mode->setText(tr(" 近远搜 "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 近远搜 "));
        break;
    case 0x05:
        ui->lineEdit_radar_work_mode->setText(tr(" 超远搜 "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 超远搜 "));
        break;
    case 0x06:
        ui->lineEdit_radar_work_mode->setText(tr(" 近跟B "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 近跟B "));
        break;
    case 0x07:
        ui->lineEdit_radar_work_mode->setText(tr(" 远跟B "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" 远跟B "));
        break;
    default:
        ui->lineEdit_radar_work_mode->setText(tr(" InValid "));
        ui->lineEdit_mainwindow_radar_mode->setText(tr(" InValid "));
        break;
    }

    switch (local_radar_system_state.radar_set_mode)
    {
    case 0x33:
        ui->lineEdit_radar_set_mode->setText(tr(" 近搜 "));
        break;
    case 0x44:
        ui->lineEdit_radar_set_mode->setText(tr(" 远搜 "));
        break;
    case 0x55:
        ui->lineEdit_radar_set_mode->setText(tr(" 近远搜 "));
        break;
    case 0x66:
        ui->lineEdit_radar_set_mode->setText(tr(" 超远搜 "));
        break;
    case 0x69:
        ui->lineEdit_radar_set_mode->setText(tr(" 近跟 "));
        break;
    case 0x96:
        ui->lineEdit_radar_set_mode->setText(tr(" 远跟 "));
        break;
    case 0x77:
        ui->lineEdit_radar_set_mode->setText(tr(" 近跟B "));
        break;
    case 0x88:
        ui->lineEdit_radar_set_mode->setText(tr(" 远跟B "));
        break;
    default:
        ui->lineEdit_radar_set_mode->setText(tr(" 未配置 "));
        break;
    }

    ui->lineEdit_turntable_rotating_speed->setText(QString::number(local_radar_system_state.turntable_rotating_speed, 'f', 2));
    ui->lineEdit_biss_angle_deg->setText(QString::number(local_radar_system_state.biss_angle_deg, 'f', 2));
    ui->lineEdit_GMAC_count->setText(QString::number(local_radar_system_state.current_GMAC_count));
    ui->lineEdit_free_time->setText(QString::number(local_radar_system_state.current_free_time, 'f', 2));
    ui->lineEdit_FPGA_version->setText(QString::number(local_radar_system_state.FPGA_version % 100000));

    // 0x81
    //天线温度告警
    switch (local_radar_system_state.antenna_temperature_warning)
    {
    case 0x01:
        ui->lineEdit_antenna_temperature_warning->setText(tr(" 异常 "));
        break;
    case 0x00:
        ui->lineEdit_antenna_temperature_warning->setText(tr(" 正常 "));
        break;
    default:
        ui->lineEdit_antenna_temperature_warning->setText(tr(" InValid "));
        break;
    }

    //天线工作模式
    switch (local_radar_system_state.antenna_work_mode)
    {
    case 0x00:
        ui->lineEdit_antenna_work_mode->setText(tr(" 待机 "));
        ui->lineEdit_mainwindow_antenna_power_mode->setText(tr(" 待机 "));
        break;
    case 0x01:
        ui->lineEdit_antenna_work_mode->setText(tr(" 只接收加电 "));
        ui->lineEdit_mainwindow_antenna_power_mode->setText(tr(" 只接收加电 "));
        break;
    case 0x02:
        ui->lineEdit_antenna_work_mode->setText(tr(" 只发射加电 "));
        ui->lineEdit_mainwindow_antenna_power_mode->setText(tr(" 只发射加电 "));

        break;
    case 0x03:
        ui->lineEdit_antenna_work_mode->setText(tr(" 收发均加电 "));
        ui->lineEdit_mainwindow_antenna_power_mode->setText(tr(" 收发均加电 "));
        break;
    default:
        ui->lineEdit_antenna_work_mode->setText(tr(" InValid "));
        ui->lineEdit_mainwindow_antenna_power_mode->setText(tr(" InValid "));
        break;
    }

    //信号处理12v电流
    ui->lineEdit_bank_12v_current->setText(QString::number(local_radar_system_state.bank_12v_current, 'f', 1));
    //发射28v电流
    ui->lineEdit_launch_28v_current->setText(QString::number(local_radar_system_state.launch_28v_current, 'f', 1));
    //模块1输出功率
    switch (local_radar_system_state.output_power_1)
    {
    case 0x01:
        ui->lineEdit_output_power_1->setText(tr(" 发射 "));
        break;
    case 0x00:
        ui->lineEdit_output_power_1->setText(tr(" 不发射 "));
        break;
    default:
        ui->lineEdit_output_power_1->setText(tr(" InValid "));
        break;
    }

    //模块2输出功率
    switch (local_radar_system_state.output_power_2)
    {
    case 0x01:
        ui->lineEdit_output_power_2->setText(tr(" 发射 "));
        break;
    case 0x00:
        ui->lineEdit_output_power_2->setText(tr(" 不发射 "));
        break;
    default:
        ui->lineEdit_output_power_2->setText(tr(" InValid "));
        break;
    }
    // 0x82
    ui->lineEdit_fre_code->setText(QString::number(local_radar_system_state.fre_code));
    ui->lineEdit_in_28v_current->setText(QString::number(local_radar_system_state.in_current_28v, 'f', 1));
    ui->lineEdit_recv_12v_current->setText(QString::number(local_radar_system_state.recv_12v_current, 'f', 1));
    ui->lineEdit_fre_12v_current->setText(QString::number(local_radar_system_state.fre_12v_current, 'f', 1));
    ui->lineEdit_beam_ctrl_12v_current->setText(QString::number(local_radar_system_state.beam_ctrl_12v_current));
    ui->lineEdit_battery_T->setText(QString::number(local_radar_system_state.battery_T));

    // 0x83
    ui->lineEdit_T_transmitter1->setText(QString::number(local_radar_system_state.T_transmitter1));
    ui->lineEdit_T_transmitter2->setText(QString::number(local_radar_system_state.T_transmitter2));
    ui->lineEdit_T_recv1->setText(QString::number(local_radar_system_state.T_recv1));
    ui->lineEdit_T_recv2->setText(QString::number(local_radar_system_state.T_recv2));
    ui->lineEdit_T_recv3->setText(QString::number(local_radar_system_state.T_recv3));
    ui->lineEdit_T_recv4->setText(QString::number(local_radar_system_state.T_recv4));

    ui->lineEdit_ATP_azim_position->setText(QString::number(local_radar_system_state.ATP_azim_position));
    ui->lineEdit_ATP_azim_acc->setText(QString::number(local_radar_system_state.ATP_azim_acc));
    ui->lineEdit_ATP_cyclic_code->setText(QString::number(local_radar_system_state.ATP_cyclic_code));
    ui->lineEdit_ATP_parity->setText(QString::number(local_radar_system_state.ATP_parity));
    ui->lineEdit_track_azim_set->setText(QString::number(local_radar_system_state.track_azim_set));
    ui->lineEdit_ATP_azim_speed->setText(QString::number(local_radar_system_state.ATP_azim_speed));
    ui->lineEdit_bank_mode_report->setText(QString::number(local_radar_system_state.bank_mode_report));
    ui->lineEdit_T_AD_FPGA1->setText(QString::number(local_radar_system_state.T_AD_FPGA1, 'f', 1));
    ui->lineEdit_T_AD_FPGA2->setText(QString::number(local_radar_system_state.T_AD_FPGA2, 'f', 1));
    ui->lineEdit_T_QDR_FPGA1->setText(QString::number(local_radar_system_state.T_QDR_FPGA1, 'f', 1));
    ui->lineEdit_T_QDR_FPGA2->setText(QString::number(local_radar_system_state.T_QDR_FPGA2, 'f', 1));
    ui->lineEdit_T_QDR_FPGA3->setText(QString::number(local_radar_system_state.T_QDR_FPGA3, 'f', 1));
    ui->lineEdit_T_QDR_FPGA4->setText(QString::number(local_radar_system_state.T_QDR_FPGA4, 'f', 1));

    // 加权设置
    if (local_radar_system_state.receiver_weight_set)
    {
        ui->lineEdit_receiver_weight_set->setText(tr(" 加权 "));
    }
    else
    {
        ui->lineEdit_receiver_weight_set->setText(tr(" 不加权 "));
    }

    // 系统信息
    if (local_software_settings.display_settings.display_system_error_info_flag)
    {
        if (local_radar_system_state.error_info_flag_list[0])
        {
            ui->textEdit_system_info->append(
                QDateTime::currentDateTime().time().toString() +
                QString("丢扇区: %1 个, 累积 %2 个").arg(local_radar_system_state.error_lost_azim_sectors_num).arg(local_radar_system_state.total_lost_azim_sectors_num));
        }

        if (local_radar_system_state.error_info_flag_list[1])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("阵列失锁2"));
        }
        if (local_radar_system_state.error_info_flag_list[2])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("阵列失锁1"));
        }
        if (local_radar_system_state.error_info_flag_list[3])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("BISS码盘错误"));
        }
        //        if (local_radar_system_state.error_info_flag_list[4])
        //        {
        //            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("转台状态异常!"));
        //        }
        if (local_radar_system_state.error_info_flag_list[5])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("FPGA未收到校准系数!"));
        }

        if (local_radar_system_state.error_info_flag_list[6])
        {
            ui->textEdit_system_info->append(
                QDateTime::currentDateTime().time().toString() +
                QString("丢包: %1 个, 累积 %2 个").arg(local_radar_system_state.error_lost_packets_count).arg(local_radar_system_state.total_lost_packets_count));
        }
        if (local_radar_system_state.error_info_flag_list[7])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("天线温度警告!"));
        }
        if (local_radar_system_state.error_info_flag_list[8])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("Serdes!"));
        }
        if (local_radar_system_state.error_info_flag_list[9])
        {
            ui->textEdit_system_info->append(QDateTime::currentDateTime().time().toString() + tr("FPGA温度过高!"));
        }
    }
}

void MainWindow::sltOnTriggerActionChangeAdapter()
{
    adapters_select_dialog->updateAdaptersList();
    adapters_select_dialog->show();
}

void MainWindow::sltOnTriggerActionHardwareSettings()
{
    // 先依据设置更新界面, 再显示界面, 防止界面上残留未 Apply 的改动!
    radar_hardware_settings_dialog->updateParametersOnDialogBasedOnLocalHardwareSettings();
    radar_hardware_settings_dialog->show();
}

void MainWindow::sltOnTriggerActionSoftwareSettings()
{
    // 先依据设置更新界面, 再显示界面, 防止界面上残留未 Apply 的改动!
    radar_software_settings_dialog->updateParametersOnDialogBasedOnLocalSoftwareSettings();
    radar_software_settings_dialog->show();
}

void MainWindow::sltOnTriggerActionDataProcessingSettings()
{
    // 先依据设置更新界面, 再显示界面, 防止界面上残留未 Apply 的改动!
    radar_data_processing_settings_dialog->updateParametersOnDialogBasedOnLocalDataProcessingSettings();
    radar_data_processing_settings_dialog->show();
}

void MainWindow::sltOnTriggerActionChangeLanguage()
{
    if (is_chinese_flag)
    {
        translator->load("translations_to_EN.qm");
        qApp->installTranslator(translator);
    }
    else
    {
        qApp->removeTranslator(translator);
    }
    ui->retranslateUi(this);
    is_chinese_flag = !is_chinese_flag;
}

void MainWindow::sltOnClickPushButtonWork()
{
    if (local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag)
    {
        emit sigStartReadingFpgaPacketFile(local_read_fpga_packet_file_settings);
        ui->statusBar->showMessage(QDateTime::currentDateTime().time().toString() + tr("开始回读!"));
    }
    else
    {
        // 搜索模式下的工作才会配置“雷达任务模式”、“天线上电模式”等参数
        if (current_data_processing_mode == RADAR_SCAN_MODE)
        {
            local_radar_mission_mode_settings.antenna_power_mode = ui->comboBox_antenna_power_mode->currentIndex();
            local_radar_mission_mode_settings.radar_mission_mode = ui->comboBox_radar_mission_mode->currentIndex();
            local_radar_mission_mode_settings.radar_test_mode = ui->comboBox_radar_test_mode->currentIndex();
            local_radar_mission_mode_settings.current_spr_mode_index = local_radar_mission_mode_settings.radar_mission_mode;
            emit sigSendWorkOrderToFpga(local_radar_mission_mode_settings);
        }
    }
}

void MainWindow::sltOnClickPushButtonStop()
{
    if (local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag)
    {
        if (current_data_processing_mode == RADAR_SCAN_MODE)
        {
            emit sigStopReadingFpgaPacketFile();
        }
        else
        {
            emit sigClickStopToQuitTrackMode();
        }
    }
    else
    {
        if (current_data_processing_mode == RADAR_SCAN_MODE)
        {
            emit sigSendStopOrderToFpga();
            ui->comboBox_antenna_power_mode->setCurrentIndex(0);
        }
        else
        {
            emit sigClickStopToQuitTrackMode();
        }
    }
}

void MainWindow::sltOnClickPushButtonReset()
{
    sltOnClickPushButtonStop();
    emit sigSendResetOrderToFpga();
}

void MainWindow::sltOnClickPushButtonOtherRadarSettings()
{
    // 先依据设置更新界面, 再显示界面, 防止界面上残留未 Apply 的改动!
    radar_hardware_settings_dialog->updateParametersOnDialogBasedOnLocalHardwareSettings();
    radar_hardware_settings_dialog->show();
}

void MainWindow::sltComboBoxRadarMissionModeChanged()
{
    qDebug() << tr(" [MainWindow]: sltComboBoxRadarMissionModeOrRadarMissionBeamsNumChanged ");
    local_radar_mission_mode_settings.current_spr_mode_index = ui->comboBox_radar_mission_mode->currentIndex();
    emit sigUpdateCurrentSetSprModeIndex(local_radar_mission_mode_settings.current_spr_mode_index);
}

void MainWindow::sltOnCheckBoxReadFpgaPacketFileChanged(const int& state)
{
    switch (state)
    {
    case Qt::Unchecked:
        local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag = false;
        local_read_fpga_packet_file_settings.fpga_packet_file_dir.clear();
        emit sigStartReadingFpgaPacketFile(local_read_fpga_packet_file_settings);
        ui->statusBar->showMessage(QDateTime::currentDateTime().time().toString() + tr("回读勾选取消, 停止回读!"));
        ui->lineEdit_selected_fpga_packet_file_directory->clear();
        ui->pushButton_select_fpga_packet_file->setEnabled(false);
        ui->lineEdit_selected_fpga_packet_file_directory->setEnabled(false);
        break;
    default:
        local_read_fpga_packet_file_settings.read_fpga_packet_from_file_flag = true;
        ui->pushButton_select_fpga_packet_file->setEnabled(true);
        ui->lineEdit_selected_fpga_packet_file_directory->setEnabled(true);
        break;
    }
}

void MainWindow::sltOnClickPushButtonSelectFile()
{
    QString selected_file_directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("文件选取"), "C:", tr("文本文件(*txt)")));
    if (!selected_file_directory.isEmpty())
    {
        ui->lineEdit_selected_fpga_packet_file_directory->setText(selected_file_directory);
        local_read_fpga_packet_file_settings.fpga_packet_file_dir = selected_file_directory;
    }
}

void MainWindow::sltOnClickPushButtonSendThisTrack()
{
    QReadLocker locker(&g_read_write_lock_for_g_track_info_displayed);
    if (g_track_info_displayed.track_no != -1)
    {
        emit sigAddTrackIdToSendingTargetSet(g_track_info_displayed.track_no);
    }
}

void MainWindow::sltOnClickPushButtonTrackThisTarget()
{
    QReadLocker locker(&g_read_write_lock_for_g_track_info_displayed);
    if (g_track_info_displayed.track_no != -1)
    {
        emit sigPrepareToEnterTrackMode(g_track_info_displayed.track_no);
        statusBar()->showMessage(QString(QDateTime::currentDateTime().time().toString() + "Prepare to track " + "No" + QString::number(g_track_info_displayed.track_no)));
        qDebug() << QString(QDateTime::currentDateTime().time().toString() + "Prepare to track" + "No" + QString::number(g_track_info_displayed.track_no));
    }
}

void MainWindow::sltUpdateRadarSystemInfoOnMainWindow(const RadarSystemState& new_radar_system_state)
{
    if (new_radar_system_state.pkt_num > 0)
    {
        memcpy(&local_radar_system_state, &new_radar_system_state, sizeof(RadarSystemState));
        updateRadarSystemInfoOnMainWindow();
    }
}

void MainWindow::sltUpdateTrackDisplayerView()
{
    track_displayer_scene->sltUpdateScanLineBasedOnCurrentRadarAzim(circularlyAddDouble(360, -local_radar_system_state.current_transmit_azim_deg, 0, 360));
    track_displayer_scene->sltUpdateTrackInfoItem();
    my_graphics_view->viewport()->update();
}

void MainWindow::sltUpdateTableWidgetTrackInfo()
{
    QReadLocker locker(&g_read_write_lock_for_g_track_info_displayed);
    if (ui->tableWidget_track_info->item(0, 0))
    {
        ui->tableWidget_track_info->item(0, 0)->setText(QString::number(g_track_info_displayed.track_no));
    }
    else
    {
        ui->tableWidget_track_info->setItem(0, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.track_no)));
    }
    if (ui->tableWidget_track_info->item(1, 0))
    {
        ui->tableWidget_track_info->item(1, 0)->setText(QString::number(g_track_info_displayed.range));
    }
    else
    {
        ui->tableWidget_track_info->setItem(1, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.range)));
    }
    if (ui->tableWidget_track_info->item(2, 0))
    {
        ui->tableWidget_track_info->item(2, 0)->setText(QString::number(g_track_info_displayed.azim_deg));
    }
    else
    {
        ui->tableWidget_track_info->setItem(2, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.azim_deg)));
    }
    if (ui->tableWidget_track_info->item(3, 0))
    {
        ui->tableWidget_track_info->item(3, 0)->setText(QString::number(g_track_info_displayed.elev_deg));
    }
    else
    {
        ui->tableWidget_track_info->setItem(3, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.elev_deg)));
    }
    if (ui->tableWidget_track_info->item(4, 0))
    {
        ui->tableWidget_track_info->item(4, 0)->setText(QString::number(g_track_info_displayed.velocity));
    }
    else
    {
        ui->tableWidget_track_info->setItem(4, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.velocity)));
    }
    if (ui->tableWidget_track_info->item(5, 0))
    {
        ui->tableWidget_track_info->item(5, 0)->setText(QString::number(g_track_info_displayed.z));
    }
    else
    {
        ui->tableWidget_track_info->setItem(5, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.z)));
    }
    if (ui->tableWidget_track_info->item(6, 0))
    {
        ui->tableWidget_track_info->item(6, 0)->setText(QString::number(g_track_info_displayed.amp));
    }
    else
    {
        ui->tableWidget_track_info->setItem(6, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.amp)));
    }
    if (ui->tableWidget_track_info->item(7, 0))
    {
        ui->tableWidget_track_info->item(7, 0)->setText(QString::number(g_track_info_displayed.snr));
    }
    else
    {
        ui->tableWidget_track_info->setItem(7, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.snr)));
    }
    if (ui->tableWidget_track_info->item(8, 0))
    {
        ui->tableWidget_track_info->item(8, 0)->setText(QString::number(g_track_info_displayed.track_times));
    }
    else
    {
        ui->tableWidget_track_info->setItem(8, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.track_times)));
    }
    if (ui->tableWidget_track_info->item(9, 0))
    {
        ui->tableWidget_track_info->item(9, 0)->setText(QString::number(g_track_info_displayed.lost_times));
    }
    else
    {
        ui->tableWidget_track_info->setItem(9, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.lost_times)));
    }
    if (ui->tableWidget_track_info->item(10, 0))
    {
        ui->tableWidget_track_info->item(10, 0)->setText(QString::number(g_track_info_displayed.ve));
    }
    else
    {
        ui->tableWidget_track_info->setItem(10, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.ve)));
    }
    if (ui->tableWidget_track_info->item(11, 0))
    {
        ui->tableWidget_track_info->item(11, 0)->setText(QString::number(g_track_info_displayed.vn));
    }
    else
    {
        ui->tableWidget_track_info->setItem(11, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.vn)));
    }
    if (ui->tableWidget_track_info->item(12, 0))
    {
        ui->tableWidget_track_info->item(12, 0)->setText(QString::number(g_track_info_displayed.vz));
    }
    else
    {
        ui->tableWidget_track_info->setItem(12, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.vz)));
    }
    if (ui->tableWidget_track_info->item(13, 0))
    {
        ui->tableWidget_track_info->item(13, 0)->setText(QString::number(g_track_info_displayed.longitude, 'f', 6));
    }
    else
    {
        ui->tableWidget_track_info->setItem(13, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.longitude, 'f', 6)));
    }
    if (ui->tableWidget_track_info->item(14, 0))
    {
        ui->tableWidget_track_info->item(14, 0)->setText(QString::number(g_track_info_displayed.latitude, 'f', 6));
    }
    else
    {
        ui->tableWidget_track_info->setItem(14, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.latitude, 'f', 6)));
    }
    if (ui->tableWidget_track_info->item(15, 0))
    {
        ui->tableWidget_track_info->item(15, 0)->setText(QString::number(g_track_info_displayed.altitude, 'f', 6));
    }
    else
    {
        ui->tableWidget_track_info->setItem(15, 0, new QTableWidgetItem(QString::number(g_track_info_displayed.altitude, 'f', 6)));
    }
}

void MainWindow::sltUpdateOriginalPointItems()
{
    QMutexLocker locker(&g_mutex_for_g_original_point_item_list_and_g_target_track_item_list);
    while (g_original_point_item_to_add_list.size() > 0)
    {
        track_displayer_scene->addItem(g_original_point_item_to_add_list.takeFirst());
    }

    while (g_original_point_item_to_delete_list.size() > 0)
    {
        track_displayer_scene->removeItem(g_original_point_item_to_delete_list.first());
        delete (g_original_point_item_to_delete_list.takeFirst());
    }
}

void MainWindow::sltUpdateTargetTrackItems()
{
    QMutexLocker locker(&g_mutex_for_g_original_point_item_list_and_g_target_track_item_list);
    while (g_target_track_item_to_add_list.size() > 0)
    {
        track_displayer_scene->addItem(g_target_track_item_to_add_list.takeFirst());
    }

    while (g_target_track_item_to_update_list.size() > 0)
    {
        g_target_track_item_to_update_list.takeFirst()->updateTargetTrackItemAsTrackUpdated();
    }

    while (g_target_track_item_to_delete_list.size() > 0)
    {
        track_displayer_scene->removeItem(g_target_track_item_to_delete_list.first());
        delete (g_target_track_item_to_delete_list.takeFirst());
    }
}

void MainWindow::sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings)
{
    memcpy(&local_software_settings, &new_software_settings, sizeof(new_software_settings));
}

void MainWindow::sltCaptureAdapterStopWorking()
{
    QMessageBox::warning(this, tr("Warning"), tr("当前选中的网口停止工作, 请重新选择网口!"), QMessageBox::Ok);
}

void MainWindow::sltUpdateDataProcessingMode(const int& new_data_processing_mode)
{
    current_data_processing_mode = new_data_processing_mode;
}

void MainWindow::sltUpdateStatusBar(const QString& new_message)
{
    ui->statusBar->showMessage(new_message);
}
