#include "radar_software_settings_dialog.h"
#include "ui_radar_software_settings_dialog.h"
#include "global_settings.h"
#include <QPushButton>
#include <QHostAddress>
#include <QFileDialog>

RadarSoftwareSettingsDialog::RadarSoftwareSettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RadarSoftwareSettingsDialog)
{
    ui->setupUi(this);
    initializeSoftwareSettings(local_software_settings);
    initializeDbfCorrectCoefFileDirSettings(local_dbf_correct_coef_file_dir_settings);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonApply);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonOK);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonCancel);

    connect(ui->pushButton_select_master_external_file, &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectMasterExternalFile);
    connect(ui->pushButton_select_slave_external_file, &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectSlaveExternalFile);
    connect(ui->pushButton_select_master_internal_file, &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectMasterInternalFile);
    connect(ui->pushButton_select_slave_internal_file, &QPushButton::clicked, this, &RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectSlaveInternalFile);
}

RadarSoftwareSettingsDialog::~RadarSoftwareSettingsDialog()
{
    delete ui;
}

void RadarSoftwareSettingsDialog::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void RadarSoftwareSettingsDialog::updateLocalSoftwareSettingsBasedOnParametersOnDialog()
{
    // 通信
    local_software_settings.communication_settings.adapter_index_communicating_with_fpga = ui->lineEdit_adapter_index_communicating_with_fpga->text().toInt();
    local_software_settings.communication_settings.output_device_id = ui->lineEdit_output_device_id->text().toInt();
    local_software_settings.communication_settings.output_min_track_times = ui->lineEdit_output_min_track_times->text().toInt();
    if (ui->checkBox_output_tracks_through_adapter->isChecked())
    {
        local_software_settings.communication_settings.output_tracks_through_adapter_flag = 1;
    }
    else
    {
        local_software_settings.communication_settings.output_tracks_through_adapter_flag = 0;
    }

    local_software_settings.communication_settings.output_selected_or_all_method = ui->comboBox_output_selected_or_all->currentIndex();

    if (ui->checkBox_output_azim_north_or_east->isChecked())
    {
        local_software_settings.communication_settings.output_azim_north_or_east_flag = 1;
    }
    else
    {
        local_software_settings.communication_settings.output_azim_north_or_east_flag = 0;
    }

    local_software_settings.communication_settings.output_IP_address = QHostAddress(ui->lineEdit_output_IP_address->text()).toIPv4Address();
    local_software_settings.communication_settings.output_port = ui->lineEdit_output_port->text().toInt();

    if (ui->checkBox_input_packets_from_other_device->isChecked())
    {
        local_software_settings.communication_settings.input_packets_from_other_device_flag = 1;
    }
    else
    {
        local_software_settings.communication_settings.input_packets_from_other_device_flag = 0;
    }
    local_software_settings.communication_settings.input_port = ui->lineEdit_input_port->text().toInt();

    // 显示内容
    if (ui->checkBox_display_system_error_info->isChecked())
    {
        local_software_settings.display_settings.display_system_error_info_flag = 1;
    }
    else
    {
        local_software_settings.display_settings.display_system_error_info_flag = 0;
    }
    if (ui->checkBox_display_original_points->isChecked())
    {
        local_software_settings.display_settings.display_original_points_flag = 1;
    }
    else
    {
        local_software_settings.display_settings.display_original_points_flag = 0;
    }

    if (ui->checkBox_display_filtered_points->isChecked())
    {
        local_software_settings.display_settings.display_filtered_points_flag = 1;
    }
    else
    {
        local_software_settings.display_settings.display_filtered_points_flag = 0;
    }

    if (ui->checkBox_display_tracks->isChecked())
    {
        local_software_settings.display_settings.display_tracks_flag = 1;
    }
    else
    {
        local_software_settings.display_settings.display_tracks_flag = 0;
    }

    // 显示参数
    local_software_settings.display_settings.display_area_radius = ui->lineEdit_display_area_radius->text().toDouble();
    local_software_settings.display_settings.display_circles_gap = ui->lineEdit_display_circles_gap->text().toDouble();
    local_software_settings.display_settings.display_key_targets_track_times = ui->lineEdit_display_key_targets_track_times->text().toInt();

    // 输出文件
    if (ui->checkBox_output_fpga_packet->isChecked())
    {
        local_software_settings.output_file_settings.output_fpga_packet_flag = 1;
    }
    else
    {
        local_software_settings.output_file_settings.output_fpga_packet_flag = 0;
    }
    if (ui->checkBox_output_radar_system_info->isChecked())
    {
        local_software_settings.output_file_settings.output_radar_system_info_flag = 1;
    }
    else
    {
        local_software_settings.output_file_settings.output_radar_system_info_flag = 0;
    }
    if (ui->checkBox_output_radar_original_points->isChecked())
    {
        local_software_settings.output_file_settings.output_radar_original_points_flag = 1;
    }
    else
    {
        local_software_settings.output_file_settings.output_radar_original_points_flag = 0;
    }
    if (ui->checkBox_output_radar_track_points->isChecked())
    {
        local_software_settings.output_file_settings.output_radar_track_points_flag = 1;
    }
    else
    {
        local_software_settings.output_file_settings.output_radar_track_points_flag = 0;
    }

    // 系统调整
    if (ui->checkBox_enter_debug_mode->isChecked())
    {
        local_software_settings.system_adjust_settings.enter_debug_mode_flag = 1;
    }
    else
    {
        local_software_settings.system_adjust_settings.enter_debug_mode_flag = 0;
    }
    if (ui->checkBox_auto_adjust_signal_processer->isChecked())
    {
        local_software_settings.system_adjust_settings.auto_adjust_signal_processer_flag = 1;
    }
    else
    {
        local_software_settings.system_adjust_settings.auto_adjust_signal_processer_flag = 0;
    }

    local_dbf_correct_coef_file_dir_settings.master_external_file_dir = ui->lineEdit_master_external_file_dir->text();
    local_dbf_correct_coef_file_dir_settings.slave_external_file_dir = ui->lineEdit_slave_external_file_dir->text();
    local_dbf_correct_coef_file_dir_settings.master_internal_file_dir = ui->lineEdit_master_internal_file_dir->text();
    local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir = ui->lineEdit_master_internal_file_dir->text();
}

void RadarSoftwareSettingsDialog::updateParametersOnDialogBasedOnLocalSoftwareSettings()
{
    // 通信
    ui->lineEdit_adapter_index_communicating_with_fpga->setText(QString::number(local_software_settings.communication_settings.adapter_index_communicating_with_fpga));
    ui->lineEdit_output_device_id->setText(QString::number(local_software_settings.communication_settings.output_device_id));
    ui->lineEdit_output_min_track_times->setText(QString::number(local_software_settings.communication_settings.output_min_track_times));
    if (local_software_settings.communication_settings.output_tracks_through_adapter_flag)
    {
        ui->checkBox_output_tracks_through_adapter->setChecked(true);
    }
    else
    {
        ui->checkBox_output_tracks_through_adapter->setChecked(false);
    }

    ui->comboBox_output_selected_or_all->setCurrentIndex(local_software_settings.communication_settings.output_selected_or_all_method);

    if (local_software_settings.communication_settings.output_azim_north_or_east_flag)
    {
        ui->checkBox_output_azim_north_or_east->setChecked(true);
    }
    else
    {
        ui->checkBox_output_azim_north_or_east->setChecked(false);
    }

    ui->lineEdit_output_IP_address->setText(QHostAddress(local_software_settings.communication_settings.output_IP_address).toString());
    ui->lineEdit_output_port->setText(QString::number(local_software_settings.communication_settings.output_port));

    if (local_software_settings.communication_settings.input_packets_from_other_device_flag)
    {
        ui->checkBox_input_packets_from_other_device->setChecked(true);
    }
    else
    {
        ui->checkBox_input_packets_from_other_device->setChecked(false);
    }
    ui->lineEdit_input_port->setText(QString::number(local_software_settings.communication_settings.input_port));

    // 显示内容
    if (local_software_settings.display_settings.display_system_error_info_flag)
    {
        ui->checkBox_display_system_error_info->setChecked(true);
    }
    else
    {
        ui->checkBox_display_system_error_info->setChecked(false);
    }
    if (local_software_settings.display_settings.display_original_points_flag)
    {
        ui->checkBox_display_original_points->setChecked(true);
    }
    else
    {
        ui->checkBox_display_original_points->setChecked(false);
    }
    if (local_software_settings.display_settings.display_filtered_points_flag)
    {
        ui->checkBox_display_filtered_points->setChecked(true);
    }
    else
    {
        ui->checkBox_display_filtered_points->setChecked(false);
    }
    if (local_software_settings.display_settings.display_tracks_flag)
    {
        ui->checkBox_display_tracks->setChecked(true);
    }
    else
    {
        ui->checkBox_display_tracks->setChecked(false);
    }

    // 显示参数
    ui->lineEdit_display_area_radius->setText(QString::number(local_software_settings.display_settings.display_area_radius));
    ui->lineEdit_display_circles_gap->setText(QString::number(local_software_settings.display_settings.display_circles_gap));
    ui->lineEdit_display_key_targets_track_times->setText(QString::number(local_software_settings.display_settings.display_key_targets_track_times));

    // 输出文件
    if (local_software_settings.output_file_settings.output_fpga_packet_flag)
    {
        ui->checkBox_output_fpga_packet->setChecked(true);
    }
    else
    {
        ui->checkBox_output_fpga_packet->setChecked(false);
    }
    if (local_software_settings.output_file_settings.output_radar_system_info_flag)
    {
        ui->checkBox_output_radar_system_info->setChecked(true);
    }
    else
    {
        ui->checkBox_output_radar_system_info->setChecked(false);
    }
    if (local_software_settings.output_file_settings.output_radar_original_points_flag)
    {
        ui->checkBox_output_radar_original_points->setChecked(true);
    }
    else
    {
        ui->checkBox_output_radar_original_points->setChecked(false);
    }
    if (local_software_settings.output_file_settings.output_radar_track_points_flag)
    {
        ui->checkBox_output_radar_track_points->setChecked(true);
    }
    else
    {
        ui->checkBox_output_radar_track_points->setChecked(false);
    }

    // 系统调整
    if (local_software_settings.system_adjust_settings.enter_debug_mode_flag)
    {
        ui->checkBox_enter_debug_mode->setChecked(true);
    }
    else
    {
        ui->checkBox_enter_debug_mode->setChecked(false);
    }
    if (local_software_settings.system_adjust_settings.auto_adjust_signal_processer_flag)
    {
        ui->checkBox_auto_adjust_signal_processer->setChecked(true);
    }
    else
    {
        ui->checkBox_auto_adjust_signal_processer->setChecked(false);
    }

    ui->lineEdit_master_external_file_dir->setText(local_dbf_correct_coef_file_dir_settings.master_external_file_dir);
    ui->lineEdit_slave_external_file_dir->setText(local_dbf_correct_coef_file_dir_settings.slave_external_file_dir);
    ui->lineEdit_master_internal_file_dir->setText(local_dbf_correct_coef_file_dir_settings.master_internal_file_dir);
    ui->lineEdit_slave_internal_file_dir->setText(local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir);
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonApply()
{
    qDebug() << tr("[RadarSoftwareSettingsDialog]: sltOnClickPushButtonApply");
    RadarSoftwareSettingsDialog::updateLocalSoftwareSettingsBasedOnParametersOnDialog();
    emit sigUpdateSoftwareSettings(local_software_settings);
    emit sigUpdateDbfCorrectCoefFileDirSettings(local_dbf_correct_coef_file_dir_settings);
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonCancel()
{
    RadarSoftwareSettingsDialog::close();
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonOK()
{
    RadarSoftwareSettingsDialog::sltOnClickPushButtonApply();
    RadarSoftwareSettingsDialog::close();
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectMasterExternalFile()
{
    QString tmp_dir = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("文件选取"), "C:", tr("文本文件(*txt)")));
    ui->lineEdit_master_external_file_dir->setText(tmp_dir);
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectSlaveExternalFile()
{
    QString tmp_dir = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("文件选取"), "C:", tr("文本文件(*txt)")));
    ui->lineEdit_slave_external_file_dir->setText(tmp_dir);
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectMasterInternalFile()
{
    QString tmp_dir = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("文件选取"), "C:", tr("文本文件(*txt)")));
    ui->lineEdit_master_internal_file_dir->setText(tmp_dir);
}

void RadarSoftwareSettingsDialog::sltOnClickPushButtonSelectSlaveInternalFile()
{
    QString tmp_dir = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("文件选取"), "C:", tr("文本文件(*txt)")));
    ui->lineEdit_slave_internal_file_dir->setText(tmp_dir);
}

void RadarSoftwareSettingsDialog::sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings)
{
    memcpy(&local_software_settings, &new_software_settings, sizeof(SoftwareSettings));
}

void RadarSoftwareSettingsDialog::sltUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings)
{
    local_dbf_correct_coef_file_dir_settings.master_external_file_dir = new_dbf_correct_coef_file_dir_settings.master_external_file_dir;
    local_dbf_correct_coef_file_dir_settings.slave_external_file_dir = new_dbf_correct_coef_file_dir_settings.slave_external_file_dir;
    local_dbf_correct_coef_file_dir_settings.master_internal_file_dir = new_dbf_correct_coef_file_dir_settings.master_internal_file_dir;
    local_dbf_correct_coef_file_dir_settings.slave_internal_file_dir = new_dbf_correct_coef_file_dir_settings.slave_internal_file_dir;
}
