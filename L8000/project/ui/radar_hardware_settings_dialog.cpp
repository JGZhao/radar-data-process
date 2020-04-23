#include "radar_hardware_settings_dialog.h"
#include "ui_radar_hardware_settings_dialog.h"
#include "global_settings.h"
#include <QPushButton>
#include <QDebug>

RadarHardwareSettingsDialog::RadarHardwareSettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RadarHardwareSettingsDialog)
{
    ui->setupUi(this);
    initializeHardwareSettings(local_hardware_settings);
    current_set_spr_mode_index = 0;
    connect(ui->comboBox_turntable_rotating_mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RadarHardwareSettingsDialog::sltComboBoxTurntableRotatingModeChanged);
    connect(ui->lineEdit_SPR, &QLineEdit::textChanged, this, &RadarHardwareSettingsDialog::sltOnLineEditSPRChanged);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &RadarHardwareSettingsDialog::sltOnClickPushButtonApply);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &RadarHardwareSettingsDialog::sltOnClickPushButtonCancel);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &RadarHardwareSettingsDialog::sltOnClickPushButtonOK);
}

RadarHardwareSettingsDialog::~RadarHardwareSettingsDialog()
{
    delete ui;
}

void RadarHardwareSettingsDialog::changeEvent(QEvent* e)
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

void RadarHardwareSettingsDialog::updateLocalHardwareSettingsBasedOnParametersOnDialog()
{
    // 工作模式
    local_hardware_settings.antenna_enable = ui->comboBox_antenna_enable->currentIndex();
    local_hardware_settings.work_or_serdes = ui->comboBox_work_or_serdes->currentIndex();
    local_hardware_settings.work_or_emulate = ui->comboBox_work_or_emulate->currentIndex();

    // 天线相关
    local_hardware_settings.work_mode_or_correct_mode = ui->comboBox_work_mode_or_correct_mode->currentIndex();
    local_hardware_settings.antennna_fre_or_amp_config = ui->comboBox_antennna_fre_or_amp_config->currentIndex();
    local_hardware_settings.antenna_weight_method = ui->comboBox_antenna_weight_method->currentIndex();
    local_hardware_settings.antennna_freq_code = ui->comboBox_antennna_freq_code->currentIndex();
    local_hardware_settings.elev_support_angle = ui->comboBox_elev_support_angle->currentIndex();
    local_hardware_settings.antenna_raising_angle = ui->lineEdit_antenna_raising_angle->text().toInt();
    local_hardware_settings.sigma_delta_decay_1 = ui->lineEdit_sigma_delta_decay_1->text().toInt();
    local_hardware_settings.sigma_delta_decay_2 = ui->lineEdit_sigma_delta_decay_2->text().toInt();
    local_hardware_settings.protect_decay_1 = ui->lineEdit_protected_decay_1->text().toInt();
    local_hardware_settings.protect_decay_2 = ui->lineEdit_protected_decay_2->text().toInt();

    // DBF 相关
    local_hardware_settings.amp_code = ui->lineEdit_amp_code->text().toInt();
    local_hardware_settings.channel_amp_phase_control_mode = ui->lineEdit_amp_phase_control_mode->text().toInt();
    local_hardware_settings.channel_amp_phase_control_code = ui->lineEdit_channel_amp_phase_control_code->text().toInt();
    local_hardware_settings.channel_num = ui->lineEdit_channel_num->text().toInt();
    local_hardware_settings.dbf_coe_choose = ui->comboBox_DBF_coe_choose->currentIndex();
    local_hardware_settings.dbf_coef_receive_raising_angle = ui->comboBox_dbf_coef_receive_raise_angle->currentIndex();

    // 转台相关
    local_hardware_settings.turntable_rotating_mode = ui->comboBox_turntable_rotating_mode->currentIndex();
    local_hardware_settings.spr_map[current_set_spr_mode_index] = ui->lineEdit_SPR->text().toDouble();
    if (fabs(local_hardware_settings.spr_map[current_set_spr_mode_index]) < EPS)
    {
        local_hardware_settings.turntable_rotating_speed = 180;
    }
    else
    {
        local_hardware_settings.turntable_rotating_speed = 360.0 / local_hardware_settings.spr_map[current_set_spr_mode_index];
    }
    local_hardware_settings.turntable_pointing_angle = ui->lineEdit_turntable_pointing_angle->text().toDouble();

    // 静默
    if (ui->checkBox_enble_power_off_area->isChecked())
    {
        local_hardware_settings.power_off_area_settings.enable_power_off_area_flag = 1;
    }
    else
    {
        local_hardware_settings.power_off_area_settings.enable_power_off_area_flag = 0;
    }
    local_hardware_settings.power_off_area_settings.power_off_azim_deg_floor = ui->lineEdit_power_off_azim_deg_floor->text().toDouble();
    local_hardware_settings.power_off_area_settings.power_off_azim_deg_ceil = ui->lineEdit_power_off_azim_deg_ceil->text().toDouble();

    // 信号处理相关
    if (ui->checkBox_STC->isChecked())
    {
        local_hardware_settings.stc_select = 1;
    }
    else
    {
        local_hardware_settings.stc_select = 0;
    }
    if (ui->checkBox_remove_dc_flag->isChecked())
    {
        local_hardware_settings.remove_dc_flag = 1;
    }
    else
    {
        local_hardware_settings.remove_dc_flag = 0;
    }
    local_hardware_settings.cfar_method = ui->comboBox_cfar_method->currentIndex();

    // thresholds
    for (int ii = 0; ii < THRESHOLDS_NUM; ii++)
    {
        local_hardware_settings.thresholds[ii].doppler_threshold = ui->tableWidget_thresholds->item(0, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].range_threshold_division_range_index_n_f = ui->tableWidget_thresholds->item(1, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].range_threshold_near = ui->tableWidget_thresholds->item(2, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].range_threshold_far = ui->tableWidget_thresholds->item(3, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].fft_cut = ui->tableWidget_thresholds->item(4, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].damp_cut_division_range_index = ui->tableWidget_thresholds->item(5, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].damp_cut_near = ui->tableWidget_thresholds->item(6, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].damp_cut_far = ui->tableWidget_thresholds->item(7, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].doppler_exclude[0] = ui->tableWidget_thresholds->item(8, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].doppler_exclude[1] = ui->tableWidget_thresholds->item(9, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].doppler_exclude[2] = ui->tableWidget_thresholds->item(10, ii)->text().toInt();
        local_hardware_settings.thresholds[ii].doppler_exclude[3] = ui->tableWidget_thresholds->item(11, ii)->text().toInt();

        // 搜索模式
        if (ii < RADAR_SCAN_TRACK_MODE_DIVISION_INDEX)
        {
            if (local_hardware_settings.thresholds[ii].doppler_exclude[0] > local_hardware_settings.thresholds[ii].doppler_exclude[1] ||
                local_hardware_settings.thresholds[ii].doppler_exclude[1] > local_hardware_settings.thresholds[ii].doppler_exclude[2] ||
                local_hardware_settings.thresholds[ii].doppler_exclude[2] > local_hardware_settings.thresholds[ii].doppler_exclude[3])
            {
                local_hardware_settings.thresholds[ii].doppler_exclude[0] = 1;
                local_hardware_settings.thresholds[ii].doppler_exclude[1] = 127;
                local_hardware_settings.thresholds[ii].doppler_exclude[2] = 129;
                local_hardware_settings.thresholds[ii].doppler_exclude[3] = 255;
            }
        }
        // 跟踪模式
        else
        {
            if (local_hardware_settings.thresholds[ii].doppler_exclude[0] > local_hardware_settings.thresholds[ii].doppler_exclude[1] ||
                local_hardware_settings.thresholds[ii].doppler_exclude[1] > local_hardware_settings.thresholds[ii].doppler_exclude[2] ||
                local_hardware_settings.thresholds[ii].doppler_exclude[2] > local_hardware_settings.thresholds[ii].doppler_exclude[3])
            {
                local_hardware_settings.thresholds[ii].doppler_exclude[0] = 1;
                local_hardware_settings.thresholds[ii].doppler_exclude[1] = 511;
                local_hardware_settings.thresholds[ii].doppler_exclude[2] = 513;
                local_hardware_settings.thresholds[ii].doppler_exclude[3] = 1023;
            }
        }
    }
}

void RadarHardwareSettingsDialog::updateParametersOnDialogBasedOnLocalHardwareSettings()
{
    // 工作模式
    ui->comboBox_antenna_enable->setCurrentIndex(local_hardware_settings.antenna_enable);
    ui->comboBox_work_or_serdes->setCurrentIndex(local_hardware_settings.work_or_serdes);
    ui->comboBox_work_or_emulate->setCurrentIndex(local_hardware_settings.work_or_emulate);

    // 天线相关
    ui->comboBox_work_mode_or_correct_mode->setCurrentIndex(local_hardware_settings.work_mode_or_correct_mode);
    ui->comboBox_antennna_fre_or_amp_config->setCurrentIndex(local_hardware_settings.antennna_fre_or_amp_config);
    ui->comboBox_antenna_weight_method->setCurrentIndex(local_hardware_settings.antenna_weight_method);
    ui->comboBox_antennna_freq_code->setCurrentIndex(local_hardware_settings.antennna_freq_code);
    ui->comboBox_elev_support_angle->setCurrentIndex(local_hardware_settings.elev_support_angle);
    ui->lineEdit_antenna_raising_angle->setText(QString::number(local_hardware_settings.antenna_raising_angle));
    ui->lineEdit_sigma_delta_decay_1->setText(QString::number(local_hardware_settings.sigma_delta_decay_1));
    ui->lineEdit_sigma_delta_decay_2->setText(QString::number(local_hardware_settings.sigma_delta_decay_2));
    ui->lineEdit_protected_decay_1->setText(QString::number(local_hardware_settings.protect_decay_1));
    ui->lineEdit_protected_decay_2->setText(QString::number(local_hardware_settings.protect_decay_2));

    // DBF 相关
    ui->lineEdit_amp_code->setText(QString::number(local_hardware_settings.amp_code));
    ui->lineEdit_amp_phase_control_mode->setText(QString::number(local_hardware_settings.channel_amp_phase_control_mode));
    ui->lineEdit_channel_amp_phase_control_code->setText(QString::number(local_hardware_settings.channel_amp_phase_control_code));
    ui->lineEdit_channel_num->setText(QString::number(local_hardware_settings.channel_num));
    ui->comboBox_DBF_coe_choose->setCurrentIndex(local_hardware_settings.dbf_coe_choose);
    ui->comboBox_dbf_coef_receive_raise_angle->setCurrentIndex(local_hardware_settings.dbf_coef_receive_raising_angle);

    // 转台相关
    ui->comboBox_turntable_rotating_mode->setCurrentIndex(local_hardware_settings.turntable_rotating_mode);
    ui->lineEdit_SPR->setText(QString::number(local_hardware_settings.spr_map[current_set_spr_mode_index], 'f', 3));
    ui->lineEdit_turntabel_rotating_speed->setText(QString::number(local_hardware_settings.turntable_rotating_speed, 'f', 3));
    ui->lineEdit_turntable_pointing_angle->setText(QString::number(local_hardware_settings.turntable_pointing_angle, 'f', 3));

    // 静默
    if (local_hardware_settings.power_off_area_settings.enable_power_off_area_flag)
    {
        ui->checkBox_enble_power_off_area->setCheckState(Qt::Checked);
    }
    else
    {
        ui->checkBox_enble_power_off_area->setCheckState(Qt::Unchecked);
    }
    ui->lineEdit_power_off_azim_deg_floor->setText(QString::number(local_hardware_settings.power_off_area_settings.power_off_azim_deg_floor));
    ui->lineEdit_power_off_azim_deg_ceil->setText(QString::number(local_hardware_settings.power_off_area_settings.power_off_azim_deg_ceil));

    // 信号处理
    if (local_hardware_settings.stc_select)
    {
        ui->checkBox_STC->setCheckState(Qt::Checked);
    }
    else
    {
        ui->checkBox_STC->setCheckState(Qt::Unchecked);
    }
    if (local_hardware_settings.remove_dc_flag)
    {
        ui->checkBox_remove_dc_flag->setCheckState(Qt::Checked);
    }
    else
    {
        ui->checkBox_remove_dc_flag->setCheckState(Qt::Unchecked);
    }
    ui->comboBox_cfar_method->setCurrentIndex(local_hardware_settings.cfar_method);
    // thresholds
    for (int ii = 0; ii < THRESHOLDS_NUM; ii++)
    {
        //-----------------第 0 行速度门限-----------------
        if (ui->tableWidget_thresholds->item(0, ii))
        {
            ui->tableWidget_thresholds->item(0, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].doppler_threshold));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(0, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].doppler_threshold)));
        }
        //-----------------第 1 行2d门限切换距离点-----------------
        if (ui->tableWidget_thresholds->item(1, ii))
        {
            ui->tableWidget_thresholds->item(1, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].range_threshold_division_range_index_n_f));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(1, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].range_threshold_division_range_index_n_f)));
        }
        //-----------------第 2 行近距离门限-----------------
        if (ui->tableWidget_thresholds->item(2, ii))
        {
            ui->tableWidget_thresholds->item(2, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].range_threshold_near));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(2, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].range_threshold_near)));
        }
        //-----------------第 3 行远距离门限-----------------
        if (ui->tableWidget_thresholds->item(3, ii))
        {
            ui->tableWidget_thresholds->item(3, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].range_threshold_far));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(3, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].range_threshold_far)));
        }
        //-----------------第 4 行fft截位数-----------------
        if (ui->tableWidget_thresholds->item(4, ii))
        {
            ui->tableWidget_thresholds->item(4, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].fft_cut));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(4, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].fft_cut)));
        }

        //-----------------第 5 行截位距离-----------------
        if (ui->tableWidget_thresholds->item(5, ii))
        {
            ui->tableWidget_thresholds->item(5, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].damp_cut_division_range_index));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(5, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].damp_cut_division_range_index)));
        }
        //-----------------第 6 行近距离截位数-----------------
        if (ui->tableWidget_thresholds->item(6, ii))
        {
            ui->tableWidget_thresholds->item(6, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].damp_cut_near));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(6, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].damp_cut_near)));
        }
        //-----------------第 7 行远距离截位数-----------------
        if (ui->tableWidget_thresholds->item(7, ii))
        {
            ui->tableWidget_thresholds->item(7, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].damp_cut_far));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(7, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].damp_cut_far)));
        }
        //-----------------第 8 行删除点0-----------------
        if (ui->tableWidget_thresholds->item(8, ii))
        {
            ui->tableWidget_thresholds->item(8, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[0]));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(8, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[0])));
        }
        //-----------------第 9 行删除点1-----------------
        if (ui->tableWidget_thresholds->item(9, ii))
        {
            ui->tableWidget_thresholds->item(9, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[1]));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(9, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[1])));
        }
        //-----------------第 10 行删除点2-----------------
        if (ui->tableWidget_thresholds->item(10, ii))
        {
            ui->tableWidget_thresholds->item(10, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[2]));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(10, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[2])));
        }
        //-----------------第 11 行删除点3-----------------
        if (ui->tableWidget_thresholds->item(11, ii))
        {
            ui->tableWidget_thresholds->item(11, ii)->setText(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[3]));
        }
        else
        {
            ui->tableWidget_thresholds->setItem(11, ii, new QTableWidgetItem(QString::number(local_hardware_settings.thresholds[ii].doppler_exclude[3])));
        }
    }

    // 显示内容居中
    for (int ii = 0; ii < ui->tableWidget_thresholds->rowCount(); ++ii)
    {
        for (int jj = 0; jj < ui->tableWidget_thresholds->columnCount(); ++jj)
        {
            ui->tableWidget_thresholds->item(ii, jj)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void RadarHardwareSettingsDialog::sltComboBoxTurntableRotatingModeChanged()
{
    switch (ui->comboBox_turntable_rotating_mode->currentIndex())
    {
    case 0:
        ui->lineEdit_turntabel_rotating_speed->setEnabled(false);
        ui->lineEdit_turntable_pointing_angle->setEnabled(false);
        break;
    case 1:
        ui->lineEdit_turntabel_rotating_speed->setEnabled(false);
        ui->lineEdit_turntable_pointing_angle->setEnabled(true);
        break;
    case 4:
        ui->lineEdit_turntabel_rotating_speed->setEnabled(false);
        ui->lineEdit_turntable_pointing_angle->setEnabled(true);
        break;
    default:
        ui->lineEdit_turntabel_rotating_speed->setEnabled(true);
        ui->lineEdit_turntable_pointing_angle->setEnabled(false);
        break;
    }
}

// 如果修改 SPR, 同时修改转动速度
void RadarHardwareSettingsDialog::sltOnLineEditSPRChanged()
{
    double tmp_SPR = ui->lineEdit_SPR->text().toDouble();
    if (tmp_SPR > 0)
    {
        double tmp_rotating_speed = 360 / tmp_SPR;
        ui->lineEdit_turntabel_rotating_speed->setText(QString::number(tmp_rotating_speed, 'f', 3));
    }
}

void RadarHardwareSettingsDialog::sltOnClickPushButtonApply()
{
    qDebug() << tr("[RadarHardwareSettingsDialog]: sltOnClickPushButtonApply");
    RadarHardwareSettingsDialog::updateLocalHardwareSettingsBasedOnParametersOnDialog();
    emit sigUpdateLocalHardwareSettings(local_hardware_settings);
}

void RadarHardwareSettingsDialog::sltOnClickPushButtonCancel()
{
    RadarHardwareSettingsDialog::close();
}

void RadarHardwareSettingsDialog::sltOnClickPushButtonOK()
{
    RadarHardwareSettingsDialog::sltOnClickPushButtonApply();
    RadarHardwareSettingsDialog::close();
}

void RadarHardwareSettingsDialog::sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings)
{
    memcpy(&local_hardware_settings, &new_hardware_settings, sizeof(HardwareSettings));
}

void RadarHardwareSettingsDialog::sltUpdateCurrentSetSprModeIndex(const int& new_spr_mode_index)
{
    qDebug() << tr("[RadarHardwareSettingsDialog]: sltUpdateCurrentSetSprModeIndex! ");
    current_set_spr_mode_index = new_spr_mode_index;
}
