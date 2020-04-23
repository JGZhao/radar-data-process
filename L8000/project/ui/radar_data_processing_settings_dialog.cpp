#include "radar_data_processing_settings_dialog.h"
#include "ui_radar_data_processing_settings_dialog.h"
#include <QPushButton>
#include <QDebug>

RadarDataProcessingSettingsDialog::RadarDataProcessingSettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RadarDataProcessingSettingsDialog)
{
    ui->setupUi(this);
    initializeDataProcessingSettings(local_data_processing_settings);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &RadarDataProcessingSettingsDialog::sltOnClickPushButtonApply);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &RadarDataProcessingSettingsDialog::sltOnClickPushButtonCancel);
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &RadarDataProcessingSettingsDialog::sltOnClickPushButtonOK);
}

RadarDataProcessingSettingsDialog::~RadarDataProcessingSettingsDialog()
{
    delete ui;
}

void RadarDataProcessingSettingsDialog::changeEvent(QEvent* e)
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

void RadarDataProcessingSettingsDialog::updateParametersOnDialogBasedOnLocalDataProcessingSettings()
{
    // 雷达波形参数
    for (int ii = 0; ii < RADAR_MODE_NUM; ++ii)
    {
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 0))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 0)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].range_unit));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii, 0, new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].range_unit)));
        }

        if (ui->tableWidget_radar_waveform_parameters->item(ii, 1))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 1)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].blind_area));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].blind_area)));
        }

        if (ui->tableWidget_radar_waveform_parameters->item(ii, 2))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 2)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].doppler_unit));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].doppler_unit)));
        }

        if (ui->tableWidget_radar_waveform_parameters->item(ii, 3))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 3)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].mtd_num));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii, 3, new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].mtd_num)));
        }
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 4))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 4)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].rcs_correction_coef));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii,
                                                               4,
                                                               new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].rcs_correction_coef)));
        }
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 5))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 5)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].decay0));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii, 5, new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].decay0)));
        }
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 6))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 6)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].range_division_01));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii,
                                                               6,
                                                               new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].range_division_01)));
        }
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 7))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 7)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].range_division_12));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii,
                                                               7,
                                                               new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].range_division_12)));
        }
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 8))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 8)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].decay2));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(ii, 8, new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].decay2)));
        }
        if (ui->tableWidget_radar_waveform_parameters->item(ii, 9))
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, 9)->setText(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].time_to_correct_azim));
        }
        else
        {
            ui->tableWidget_radar_waveform_parameters->setItem(
                ii,
                9,
                new QTableWidgetItem(QString::number(local_data_processing_settings.radar_wave_paremeters[ii].time_to_correct_azim)));
        }
    }

    for (int ii = 0; ii < ui->tableWidget_radar_waveform_parameters->rowCount(); ++ii)
    {
        for (int jj = 0; jj < ui->tableWidget_radar_waveform_parameters->columnCount(); ++jj)
        {
            ui->tableWidget_radar_waveform_parameters->item(ii, jj)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // 和差参数
    for (int ii = 0; ii < SIGMA_DELTA_SETTINGS_NUM; ++ii)
    {
        if (ui->tableWidget_sigma_delta_parameters->item(ii, 0))
        {
            if (local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag)
            {
                ui->tableWidget_sigma_delta_parameters->item(ii, 0)->setCheckState(Qt::Checked);
            }
            else
            {
                ui->tableWidget_sigma_delta_parameters->item(ii, 0)->setCheckState(Qt::Unchecked);
            }
        }
        else
        {
            if (local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag)
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Checked);
                ui->tableWidget_sigma_delta_parameters->setItem(ii, 0, m_checkbox);
            }
            else
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Unchecked);
                ui->tableWidget_sigma_delta_parameters->setItem(ii, 0, m_checkbox);
            }
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 1))
        {
            if (local_data_processing_settings.sigma_delta_parameters[ii].weight_flag)
            {
                ui->tableWidget_sigma_delta_parameters->item(ii, 1)->setCheckState(Qt::Checked);
            }
            else
            {
                ui->tableWidget_sigma_delta_parameters->item(ii, 1)->setCheckState(Qt::Unchecked);
            }
        }
        else
        {
            if (local_data_processing_settings.sigma_delta_parameters[ii].weight_flag)
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Checked);
                ui->tableWidget_sigma_delta_parameters->setItem(ii, 1, m_checkbox);
            }
            else
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Unchecked);
                ui->tableWidget_sigma_delta_parameters->setItem(ii, 1, m_checkbox);
            }
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 2))
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, 2)->setText(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_slope));
        }
        else
        {
            ui->tableWidget_sigma_delta_parameters->setItem(ii,
                                                            2,
                                                            new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_slope)));
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 3))
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, 3)->setText(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_floor));
        }
        else
        {
            ui->tableWidget_sigma_delta_parameters->setItem(
                ii,
                3,
                new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_floor)));
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 4))
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, 4)->setText(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_ceil));
        }
        else
        {
            ui->tableWidget_sigma_delta_parameters->setItem(
                ii,
                4,
                new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_ceil)));
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 5))
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, 5)->setText(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].beam_width));
        }
        else
        {
            ui->tableWidget_sigma_delta_parameters->setItem(ii, 5, new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].beam_width)));
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 6))
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, 6)->setText(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].max_bias_angle));
        }
        else
        {
            ui->tableWidget_sigma_delta_parameters->setItem(ii, 6, new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].max_bias_angle)));
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 7))
        {
            if (local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag)
            {
                ui->tableWidget_sigma_delta_parameters->item(ii, 7)->setCheckState(Qt::Checked);
            }
            else
            {
                ui->tableWidget_sigma_delta_parameters->item(ii, 7)->setCheckState(Qt::Unchecked);
            }
        }
        else
        {
            if (local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag)
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Checked);
                ui->tableWidget_sigma_delta_parameters->setItem(ii, 7, m_checkbox);
            }
            else
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Unchecked);
                ui->tableWidget_sigma_delta_parameters->setItem(ii, 7, m_checkbox);
            }
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 8))
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, 8)->setText(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_ratio));
        }
        else
        {
            ui->tableWidget_sigma_delta_parameters->setItem(
                ii,
                8,
                new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_ratio)));
        }
    }

    for (int ii = 0; ii < ui->tableWidget_sigma_delta_parameters->rowCount(); ++ii)
    {
        for (int jj = 0; jj < ui->tableWidget_sigma_delta_parameters->columnCount(); ++jj)
        {
            ui->tableWidget_sigma_delta_parameters->item(ii, jj)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // 是否加权的俯仰和差参数
    for (int ii = 0; ii < 2; ++ii)
    {
        for (int jj = 0; jj < ELEV_BEAM_NUM_SCAN_MODE; ++jj)
        {
            if (ui->tableWidget_sigma_delta_elev_slope_each_beam->item(ii, jj))
            {
                ui->tableWidget_sigma_delta_elev_slope_each_beam->item(ii, jj)->setText(
                    QString::number(local_data_processing_settings.sigma_delta_elev_slope[ii].sigma_delta_slope[jj]));
            }
            else
            {
                ui->tableWidget_sigma_delta_elev_slope_each_beam->setItem(
                    ii,
                    jj,
                    new QTableWidgetItem(QString::number(local_data_processing_settings.sigma_delta_elev_slope[ii].sigma_delta_slope[jj])));
            }
        }
    }

    // 角度校正
    ui->lineEdit_azim_bias_deg->setText(QString::number(local_data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle));
    ui->lineEdit_elev_bias_deg->setText(QString::number(local_data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle));
    ui->lineEdit_height_bias->setText(QString::number(local_data_processing_settings.azim_elev_height_bias_settings.height_bias));

    // 经纬度转换
    if (local_data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag)
    {
        ui->checkBox_convert_lon_lat_alt->setChecked(true);
    }
    else
    {
        ui->checkBox_convert_lon_lat_alt->setChecked(false);
    }
    ui->lineEdit_longitude->setText(QString::number(local_data_processing_settings.radar_lon_lat_alt.radar_longitude, 'f', 6));
    ui->lineEdit_latitude->setText(QString::number(local_data_processing_settings.radar_lon_lat_alt.radar_latitude, 'f', 6));
    ui->lineEdit_altitude->setText(QString::number(local_data_processing_settings.radar_lon_lat_alt.radar_altitude, 'f', 6));

    // 选择区域
    if (ui->tableWidget_filter_area->item(0, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_radar_mode_flag)
        {
            ui->tableWidget_filter_area->item(0, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(0, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_radar_mode_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(0, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(0, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(0, 1))
    {
        ui->tableWidget_filter_area->item(0, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_radar_mode_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(0, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_radar_mode_floor)));
    }

    if (ui->tableWidget_filter_area->item(0, 2))
    {
        ui->tableWidget_filter_area->item(0, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_radar_mode_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(0, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_radar_mode_ceil)));
    }

    if (ui->tableWidget_filter_area->item(1, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_range_flag)
        {
            ui->tableWidget_filter_area->item(1, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(1, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_range_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(1, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(1, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(1, 1))
    {
        ui->tableWidget_filter_area->item(1, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_range_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(1, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_range_floor)));
    }

    if (ui->tableWidget_filter_area->item(1, 2))
    {
        ui->tableWidget_filter_area->item(1, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_range_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(1, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_range_ceil)));
    }

    if (ui->tableWidget_filter_area->item(2, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_azim_flag)
        {
            ui->tableWidget_filter_area->item(2, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(2, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_azim_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(2, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(2, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(2, 1))
    {
        ui->tableWidget_filter_area->item(2, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_azim_deg_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(2, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_azim_deg_floor)));
    }

    if (ui->tableWidget_filter_area->item(2, 2))
    {
        ui->tableWidget_filter_area->item(2, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_azim_deg_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(2, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_azim_deg_ceil)));
    }

    if (ui->tableWidget_filter_area->item(3, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_elev_flag)
        {
            ui->tableWidget_filter_area->item(3, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(3, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_elev_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(3, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(3, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(3, 1))
    {
        ui->tableWidget_filter_area->item(3, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_elev_deg_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(3, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_elev_deg_floor)));
    }

    if (ui->tableWidget_filter_area->item(3, 2))
    {
        ui->tableWidget_filter_area->item(3, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_elev_deg_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(3, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_elev_deg_ceil)));
    }

    if (ui->tableWidget_filter_area->item(4, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_velocity_flag)
        {
            ui->tableWidget_filter_area->item(4, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(4, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_velocity_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(4, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(4, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(4, 1))
    {
        ui->tableWidget_filter_area->item(4, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_velocity_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(4, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_velocity_floor)));
    }

    if (ui->tableWidget_filter_area->item(4, 2))
    {
        ui->tableWidget_filter_area->item(4, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_velocity_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(4, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_velocity_ceil)));
    }

    if (ui->tableWidget_filter_area->item(5, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_height_flag)
        {
            ui->tableWidget_filter_area->item(5, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(5, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_height_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(5, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(5, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(5, 1))
    {
        ui->tableWidget_filter_area->item(5, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_height_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(5, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_height_floor)));
    }

    if (ui->tableWidget_filter_area->item(5, 2))
    {
        ui->tableWidget_filter_area->item(5, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_height_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(5, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_height_ceil)));
    }

    if (ui->tableWidget_filter_area->item(6, 0))
    {
        if (local_data_processing_settings.selected_area_settings.select_amp_flag)
        {
            ui->tableWidget_filter_area->item(6, 0)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->tableWidget_filter_area->item(6, 0)->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        if (local_data_processing_settings.selected_area_settings.select_amp_flag)
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Checked);
            ui->tableWidget_filter_area->setItem(6, 0, m_checkbox);
        }
        else
        {
            QTableWidgetItem* m_checkbox = new QTableWidgetItem;
            m_checkbox->setCheckState(Qt::Unchecked);
            ui->tableWidget_filter_area->setItem(6, 0, m_checkbox);
        }
    }

    if (ui->tableWidget_filter_area->item(6, 1))
    {
        ui->tableWidget_filter_area->item(6, 1)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_amp_floor));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(6, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_amp_floor)));
    }

    if (ui->tableWidget_filter_area->item(6, 2))
    {
        ui->tableWidget_filter_area->item(6, 2)->setText(QString::number(local_data_processing_settings.selected_area_settings.selected_amp_ceil));
    }
    else
    {
        ui->tableWidget_filter_area->setItem(6, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.selected_area_settings.selected_amp_ceil)));
    }

    for (int ii = 0; ii < ui->tableWidget_filter_area->rowCount(); ++ii)
    {
        for (int jj = 0; jj < ui->tableWidget_filter_area->columnCount(); ++jj)
        {
            ui->tableWidget_filter_area->item(ii, jj)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // exclude
    for (int ii = 0; ii < EXCLUDE_SETTINGS_NUM; ++ii)
    {
        if (ui->tableWidget_exclude->item(ii, 0))
        {
            if (local_data_processing_settings.exclude_settings[ii].is_selected_flag)
            {
                ui->tableWidget_exclude->item(ii, 0)->setCheckState(Qt::Checked);
            }
            else
            {
                ui->tableWidget_exclude->item(ii, 0)->setCheckState(Qt::Unchecked);
            }
        }
        else
        {
            if (local_data_processing_settings.exclude_settings[ii].is_selected_flag)
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Checked);
                ui->tableWidget_exclude->setItem(ii, 0, m_checkbox);
            }
            else
            {
                QTableWidgetItem* m_checkbox = new QTableWidgetItem;
                m_checkbox->setCheckState(Qt::Unchecked);
                ui->tableWidget_exclude->setItem(ii, 0, m_checkbox);
            }
        }
        if (ui->tableWidget_exclude->item(ii, 1))
        {
            ui->tableWidget_exclude->item(ii, 1)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].radar_mode_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 1, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].radar_mode_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 2))
        {
            ui->tableWidget_exclude->item(ii, 2)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].radar_mode_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 2, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].radar_mode_max)));
        }
        if (ui->tableWidget_exclude->item(ii, 3))
        {
            ui->tableWidget_exclude->item(ii, 3)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].range_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 3, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].range_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 4))
        {
            ui->tableWidget_exclude->item(ii, 4)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].range_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 4, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].range_max)));
        }
        if (ui->tableWidget_exclude->item(ii, 5))
        {
            ui->tableWidget_exclude->item(ii, 5)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].azim_deg_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 5, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].azim_deg_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 6))
        {
            ui->tableWidget_exclude->item(ii, 6)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].azim_deg_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 6, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].azim_deg_max)));
        }
        if (ui->tableWidget_exclude->item(ii, 7))
        {
            ui->tableWidget_exclude->item(ii, 7)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].elev_deg_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 7, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].elev_deg_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 8))
        {
            ui->tableWidget_exclude->item(ii, 8)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].elev_deg_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 8, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].elev_deg_max)));
        }
        if (ui->tableWidget_exclude->item(ii, 9))
        {
            ui->tableWidget_exclude->item(ii, 9)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].velocity_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 9, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].velocity_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 10))
        {
            ui->tableWidget_exclude->item(ii, 10)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].velocity_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 10, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].velocity_max)));
        }
        if (ui->tableWidget_exclude->item(ii, 11))
        {
            ui->tableWidget_exclude->item(ii, 11)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].amp_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 11, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].amp_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 12))
        {
            ui->tableWidget_exclude->item(ii, 12)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].amp_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 12, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].amp_max)));
        }
        if (ui->tableWidget_exclude->item(ii, 13))
        {
            ui->tableWidget_exclude->item(ii, 13)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].height_min));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 13, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].height_min)));
        }
        if (ui->tableWidget_exclude->item(ii, 14))
        {
            ui->tableWidget_exclude->item(ii, 14)->setText(QString::number(local_data_processing_settings.exclude_settings[ii].height_max));
        }
        else
        {
            ui->tableWidget_exclude->setItem(ii, 14, new QTableWidgetItem(QString::number(local_data_processing_settings.exclude_settings[ii].height_max)));
        }
    }

    for (int jj = 0; jj < ui->tableWidget_exclude->rowCount(); ++jj)
    {
        for (int kk = 0; kk < ui->tableWidget_exclude->columnCount(); ++kk)
        {
            ui->tableWidget_exclude->item(jj, kk)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // 副瓣剔除
    if (local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag)
    {
        ui->checkBox_delta_azim_filter->setChecked(true);
    }
    else
    {
        ui->checkBox_delta_azim_filter->setChecked(false);
    }
    ui->lineEdit_delta_azim_filter_ratio->setText(QString::number(local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio));
    if (local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag)
    {
        ui->checkBox_delta_elev_filter->setChecked(true);
    }
    else
    {
        ui->checkBox_delta_elev_filter->setChecked(false);
    }
    ui->lineEdit_delta_elev_filter_ratio->setText(QString::number(local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio));

    // 杂波图
    if (local_data_processing_settings.clutter_map_settings.clutter_map_filter_flag)
    {
        ui->checkBox_clutter_map_filter->setChecked(true);
    }
    else
    {
        ui->checkBox_clutter_map_filter->setChecked(false);
    }
    ui->lineEdit_clutter_map_generate_weight->setText(QString::number(local_data_processing_settings.clutter_map_settings.clutter_map_generate_weight));
    ui->lineEdit_clutter_map_filter_threshold->setText(QString::number(local_data_processing_settings.clutter_map_settings.clutter_map_filter_threshold));

    // 过滤云雨杂波
    if (local_data_processing_settings.filter_cloud_settings.enable_filter_cloud)
    {
        ui->checkBox_enable_filer_cloud->setChecked(true);
    }
    else
    {
        ui->checkBox_enable_filer_cloud->setChecked(false);
    }
    ui->lineEdit_filter_cloud_threshold->setText(QString::number(local_data_processing_settings.filter_cloud_settings.filter_cloud_threshold));

    //  融合门限
    ui->lineEdit_combine_range_unit_threshold->setText(QString::number(local_data_processing_settings.combine_thresholds.combine_range_unit_threshold));
    ui->lineEdit_combine_doppler_unit_threshold->setText(QString::number(local_data_processing_settings.combine_thresholds.combine_doppler_unit_threshold));
    ui->lineEdit_combine_range_threshold->setText(QString::number(local_data_processing_settings.combine_thresholds.combine_range_threshold));
    ui->lineEdit_combine_velocity_threshold->setText(QString::number(local_data_processing_settings.combine_thresholds.combine_velocity_threshold));
    ui->lineEdit_combine_azim_threshold_deg->setText(QString::number(local_data_processing_settings.combine_thresholds.combine_azim_threshold_deg));
    ui->lineEdit_combine_elev_threshold_deg->setText(QString::number(local_data_processing_settings.combine_thresholds.combine_elev_threshold_deg));

    // 相关门限
    for (int ii = 0; ii < 2; ++ii)
    {
        if (ui->tableWidget_correlate_parameters->item(0, ii))
        {
            ui->tableWidget_correlate_parameters->item(0, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_floor));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                0,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_floor)));
        }
        if (ui->tableWidget_correlate_parameters->item(1, ii))
        {
            ui->tableWidget_correlate_parameters->item(1, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_ceil));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                1,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_ceil)));
        }
        if (ui->tableWidget_correlate_parameters->item(2, ii))
        {
            ui->tableWidget_correlate_parameters->item(2, ii)->setText(
                QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_same_direction));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                2,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_same_direction)));
        }
        if (ui->tableWidget_correlate_parameters->item(3, ii))
        {
            ui->tableWidget_correlate_parameters->item(3, ii)->setText(
                QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_opposite_direction));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                3,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_opposite_direction)));
        }
        if (ui->tableWidget_correlate_parameters->item(4, ii))
        {
            ui->tableWidget_correlate_parameters->item(4, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_0));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                4,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_0)));
        }
        if (ui->tableWidget_correlate_parameters->item(5, ii))
        {
            ui->tableWidget_correlate_parameters->item(5, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_0));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                5,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_0)));
        }
        if (ui->tableWidget_correlate_parameters->item(6, ii))
        {
            ui->tableWidget_correlate_parameters->item(6, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_division_01));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                6,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_division_01)));
        }
        if (ui->tableWidget_correlate_parameters->item(7, ii))
        {
            ui->tableWidget_correlate_parameters->item(7, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_1));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                7,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_1)));
        }
        if (ui->tableWidget_correlate_parameters->item(8, ii))
        {
            ui->tableWidget_correlate_parameters->item(8, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_1));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                8,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_1)));
        }
        if (ui->tableWidget_correlate_parameters->item(9, ii))
        {
            ui->tableWidget_correlate_parameters->item(9, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_division_12));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                9,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_range_division_12)));
        }
        if (ui->tableWidget_correlate_parameters->item(10, ii))
        {
            ui->tableWidget_correlate_parameters->item(10, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_2));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                10,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_2)));
        }
        if (ui->tableWidget_correlate_parameters->item(11, ii))
        {
            ui->tableWidget_correlate_parameters->item(11, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_2));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(
                11,
                ii,
                new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_2)));
        }
        if (ui->tableWidget_correlate_parameters->item(12, ii))
        {
            ui->tableWidget_correlate_parameters->item(12, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_x_threshold));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(12,
                                                          ii,
                                                          new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_x_threshold)));
        }
        if (ui->tableWidget_correlate_parameters->item(13, ii))
        {
            ui->tableWidget_correlate_parameters->item(13, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_y_threshold));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(13,
                                                          ii,
                                                          new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_y_threshold)));
        }
        if (ui->tableWidget_correlate_parameters->item(14, ii))
        {
            ui->tableWidget_correlate_parameters->item(14, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_z_threshold));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(14,
                                                          ii,
                                                          new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_z_threshold)));
        }
        if (ui->tableWidget_correlate_parameters->item(15, ii))
        {
            ui->tableWidget_correlate_parameters->item(15, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_vx_threshold));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(15,
                                                          ii,
                                                          new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_vx_threshold)));
        }
        if (ui->tableWidget_correlate_parameters->item(16, ii))
        {
            ui->tableWidget_correlate_parameters->item(16, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(16,
                                                          ii,
                                                          new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold)));
        }
        if (ui->tableWidget_correlate_parameters->item(17, ii))
        {
            ui->tableWidget_correlate_parameters->item(17, ii)->setText(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold));
        }
        else
        {
            ui->tableWidget_correlate_parameters->setItem(17,
                                                          ii,
                                                          new QTableWidgetItem(QString::number(local_data_processing_settings.correlate_thresholds[ii].correlate_vz_threshold)));
        }
    }

    // 航迹平滑
    ui->comboBox_filter_track_method->setCurrentIndex(local_data_processing_settings.smooth_track_settings[0].filter_track_method);
    ui->lineEdit_filter_track_coef_alpha->setText(QString::number(local_data_processing_settings.smooth_track_settings[0].filter_track_coef_alpha));
    ui->lineEdit_filter_track_coef_beta->setText(QString::number(local_data_processing_settings.smooth_track_settings[0].filter_track_coef_beta));
    ui->lineEdit_filter_track_coef_gama->setText(QString::number(local_data_processing_settings.smooth_track_settings[0].filter_track_coef_gama));
    if (local_data_processing_settings.smooth_track_settings[0].smooth_track_flag)
    {
        ui->checkBox_track_smooth->setChecked(true);
    }
    else
    {
        ui->checkBox_track_smooth->setChecked(false);
    }

    ui->comboBox_filter_track_method_1->setCurrentIndex(local_data_processing_settings.smooth_track_settings[1].filter_track_method);
    ui->lineEdit_filter_track_coef_alpha_1->setText(QString::number(local_data_processing_settings.smooth_track_settings[1].filter_track_coef_alpha));
    ui->lineEdit_filter_track_coef_beta_1->setText(QString::number(local_data_processing_settings.smooth_track_settings[1].filter_track_coef_beta));
    ui->lineEdit_filter_track_coef_gama_1->setText(QString::number(local_data_processing_settings.smooth_track_settings[1].filter_track_coef_gama));
    if (local_data_processing_settings.smooth_track_settings[1].smooth_track_flag)
    {
        ui->checkBox_track_smooth_1->setChecked(true);
    }
    else
    {
        ui->checkBox_track_smooth_1->setChecked(false);
    }

    // 航迹过滤
    if (local_data_processing_settings.filter_tracks_settings.filter_tracks_flag)
    {
        ui->checkBox_filter_tracks->setChecked(true);
    }
    else
    {
        ui->checkBox_filter_tracks->setChecked(false);
    }
    ui->lineEdit_filter_tracks_range_threshold->setText(QString::number(local_data_processing_settings.filter_tracks_settings.filter_tracks_range_threshold));
    ui->lineEdit_filter_tracks_velocity_threshold->setText(QString::number(local_data_processing_settings.filter_tracks_settings.filter_tracks_velocity_threshold));
    ui->lineEdit_filter_tracks_mean_delta_range_threshold->setText(QString::number(local_data_processing_settings.filter_tracks_settings.filter_tracks_mean_delta_range_threshold));
}

void RadarDataProcessingSettingsDialog::updateLocalDataProcessingSettingsBasedOnParametersOnDialog()
{
    // 雷达波形参数
    for (int ii = 0; ii < RADAR_MODE_NUM; ++ii)
    {
        local_data_processing_settings.radar_wave_paremeters[ii].range_unit = ui->tableWidget_radar_waveform_parameters->item(ii, 0)->text().toDouble();
        local_data_processing_settings.radar_wave_paremeters[ii].blind_area = ui->tableWidget_radar_waveform_parameters->item(ii, 1)->text().toDouble();
        local_data_processing_settings.radar_wave_paremeters[ii].doppler_unit = ui->tableWidget_radar_waveform_parameters->item(ii, 2)->text().toDouble();
        local_data_processing_settings.radar_wave_paremeters[ii].mtd_num = ui->tableWidget_radar_waveform_parameters->item(ii, 3)->text().toInt();
        local_data_processing_settings.radar_wave_paremeters[ii].rcs_correction_coef = ui->tableWidget_radar_waveform_parameters->item(ii, 4)->text().toDouble();
        local_data_processing_settings.radar_wave_paremeters[ii].decay0 = ui->tableWidget_radar_waveform_parameters->item(ii, 5)->text().toInt();
        local_data_processing_settings.radar_wave_paremeters[ii].range_division_01 = ui->tableWidget_radar_waveform_parameters->item(ii, 6)->text().toDouble();
        local_data_processing_settings.radar_wave_paremeters[ii].range_division_12 = ui->tableWidget_radar_waveform_parameters->item(ii, 7)->text().toDouble();
        local_data_processing_settings.radar_wave_paremeters[ii].decay2 = ui->tableWidget_radar_waveform_parameters->item(ii, 8)->text().toInt();
        local_data_processing_settings.radar_wave_paremeters[ii].time_to_correct_azim = ui->tableWidget_radar_waveform_parameters->item(ii, 9)->text().toDouble();
    }

    // 和差参数
    for (int ii = 0; ii < SIGMA_DELTA_SETTINGS_NUM; ++ii)
    {
        if (ui->tableWidget_sigma_delta_parameters->item(ii, 0)->checkState() == Qt::Checked)
        {
            local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag = 1;
        }
        else
        {
            local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_flag = 0;
        }

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 1)->checkState() == Qt::Checked)
        {
            local_data_processing_settings.sigma_delta_parameters[ii].weight_flag = 1;
        }
        else
        {
            local_data_processing_settings.sigma_delta_parameters[ii].weight_flag = 0;
        }

        local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_slope = ui->tableWidget_sigma_delta_parameters->item(ii, 2)->text().toDouble();
        local_data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_floor = ui->tableWidget_sigma_delta_parameters->item(ii, 3)->text().toDouble();
        local_data_processing_settings.sigma_delta_parameters[ii].negative_bias_phase_ceil = ui->tableWidget_sigma_delta_parameters->item(ii, 4)->text().toDouble();
        local_data_processing_settings.sigma_delta_parameters[ii].beam_width = ui->tableWidget_sigma_delta_parameters->item(ii, 5)->text().toDouble();
        local_data_processing_settings.sigma_delta_parameters[ii].max_bias_angle = ui->tableWidget_sigma_delta_parameters->item(ii, 6)->text().toDouble();

        if (ui->tableWidget_sigma_delta_parameters->item(ii, 7)->checkState() == Qt::Checked)
        {
            local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag = 1;
        }
        else
        {
            local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_flag = 0;
        }

        local_data_processing_settings.sigma_delta_parameters[ii].sigma_delta_filter_ratio = ui->tableWidget_sigma_delta_parameters->item(ii, 8)->text().toDouble();
    }

    // 加权和差
    for (int ii = 0; ii < 2; ++ii)
    {
        for (int jj = 0; jj < ELEV_BEAM_NUM_SCAN_MODE; ++jj)
        {
            local_data_processing_settings.sigma_delta_elev_slope[ii].sigma_delta_slope[jj] = ui->tableWidget_sigma_delta_elev_slope_each_beam->item(ii, jj)->text().toDouble();
        }
    }

    // 角度校正
    local_data_processing_settings.azim_elev_height_bias_settings.azim_bias_angle = ui->lineEdit_azim_bias_deg->text().toDouble();
    local_data_processing_settings.azim_elev_height_bias_settings.elev_bias_angle = ui->lineEdit_elev_bias_deg->text().toDouble();
    local_data_processing_settings.azim_elev_height_bias_settings.height_bias = ui->lineEdit_height_bias->text().toDouble();

    // 经纬度转换
    if (ui->checkBox_convert_lon_lat_alt->isChecked())
    {
        local_data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag = 1;
    }
    else
    {
        local_data_processing_settings.radar_lon_lat_alt.convert_lon_lat_alt_flag = 0;
    }

    local_data_processing_settings.radar_lon_lat_alt.radar_longitude = ui->lineEdit_longitude->text().toDouble();
    local_data_processing_settings.radar_lon_lat_alt.radar_latitude = ui->lineEdit_latitude->text().toDouble();
    local_data_processing_settings.radar_lon_lat_alt.radar_altitude = ui->lineEdit_altitude->text().toDouble();

    // 过滤区域
    if (ui->tableWidget_filter_area->item(0, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_radar_mode_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_radar_mode_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_radar_mode_floor = ui->tableWidget_filter_area->item(0, 1)->text().toInt();
    local_data_processing_settings.selected_area_settings.selected_radar_mode_ceil = ui->tableWidget_filter_area->item(0, 2)->text().toInt();

    if (ui->tableWidget_filter_area->item(1, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_range_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_range_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_range_floor = ui->tableWidget_filter_area->item(1, 1)->text().toDouble();
    local_data_processing_settings.selected_area_settings.selected_range_ceil = ui->tableWidget_filter_area->item(1, 2)->text().toDouble();

    if (ui->tableWidget_filter_area->item(2, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_azim_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_azim_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_azim_deg_floor = ui->tableWidget_filter_area->item(2, 1)->text().toDouble();
    local_data_processing_settings.selected_area_settings.selected_azim_deg_ceil = ui->tableWidget_filter_area->item(2, 2)->text().toDouble();

    if (ui->tableWidget_filter_area->item(3, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_elev_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_elev_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_elev_deg_floor = ui->tableWidget_filter_area->item(3, 1)->text().toDouble();
    local_data_processing_settings.selected_area_settings.selected_elev_deg_ceil = ui->tableWidget_filter_area->item(3, 2)->text().toDouble();

    if (ui->tableWidget_filter_area->item(4, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_velocity_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_velocity_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_velocity_floor = ui->tableWidget_filter_area->item(4, 1)->text().toDouble();
    local_data_processing_settings.selected_area_settings.selected_velocity_ceil = ui->tableWidget_filter_area->item(4, 2)->text().toDouble();

    if (ui->tableWidget_filter_area->item(5, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_height_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_height_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_height_floor = ui->tableWidget_filter_area->item(5, 1)->text().toDouble();
    local_data_processing_settings.selected_area_settings.selected_height_ceil = ui->tableWidget_filter_area->item(5, 2)->text().toDouble();

    if (ui->tableWidget_filter_area->item(6, 0)->checkState() == Qt::Checked)
    {
        local_data_processing_settings.selected_area_settings.select_amp_flag = 1;
    }
    else
    {
        local_data_processing_settings.selected_area_settings.select_amp_flag = 0;
    }

    local_data_processing_settings.selected_area_settings.selected_amp_floor = ui->tableWidget_filter_area->item(6, 1)->text().toInt();
    local_data_processing_settings.selected_area_settings.selected_amp_ceil = ui->tableWidget_filter_area->item(6, 2)->text().toInt();

    // exclude
    for (int ii = 0; ii < EXCLUDE_SETTINGS_NUM; ++ii)
    {
        if (ui->tableWidget_exclude->item(ii, 0)->checkState() == Qt::Checked)
        {
            local_data_processing_settings.exclude_settings[ii].is_selected_flag = 1;
        }
        else
        {
            local_data_processing_settings.exclude_settings[ii].is_selected_flag = 0;
        }
        local_data_processing_settings.exclude_settings[ii].radar_mode_min = ui->tableWidget_exclude->item(ii, 1)->text().toInt();
        local_data_processing_settings.exclude_settings[ii].radar_mode_max = ui->tableWidget_exclude->item(ii, 2)->text().toInt();
        local_data_processing_settings.exclude_settings[ii].range_min = ui->tableWidget_exclude->item(ii, 3)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].range_max = ui->tableWidget_exclude->item(ii, 4)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].azim_deg_min = ui->tableWidget_exclude->item(ii, 5)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].azim_deg_max = ui->tableWidget_exclude->item(ii, 6)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].elev_deg_min = ui->tableWidget_exclude->item(ii, 7)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].elev_deg_max = ui->tableWidget_exclude->item(ii, 8)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].velocity_min = ui->tableWidget_exclude->item(ii, 9)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].velocity_max = ui->tableWidget_exclude->item(ii, 10)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].amp_min = ui->tableWidget_exclude->item(ii, 11)->text().toInt();
        local_data_processing_settings.exclude_settings[ii].amp_max = ui->tableWidget_exclude->item(ii, 12)->text().toInt();
        local_data_processing_settings.exclude_settings[ii].height_min = ui->tableWidget_exclude->item(ii, 13)->text().toDouble();
        local_data_processing_settings.exclude_settings[ii].height_max = ui->tableWidget_exclude->item(ii, 14)->text().toDouble();
    }

    // 副瓣剔除
    if (ui->checkBox_delta_azim_filter->isChecked())
    {
        local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag = 1;
    }
    else
    {
        local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_flag = 0;
    }
    local_data_processing_settings.sidelobe_filter_settings.delta_azim_filter_ratio = ui->lineEdit_delta_azim_filter_ratio->text().toDouble();

    if (ui->checkBox_delta_elev_filter->isChecked())
    {
        local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag = 1;
    }
    else
    {
        local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_flag = 0;
    }
    local_data_processing_settings.sidelobe_filter_settings.delta_elev_filter_ratio = ui->lineEdit_delta_elev_filter_ratio->text().toDouble();

    // 杂波图
    if (ui->checkBox_clutter_map_filter->isChecked())
    {
        local_data_processing_settings.clutter_map_settings.clutter_map_filter_flag = 1;
    }
    else
    {
        local_data_processing_settings.clutter_map_settings.clutter_map_filter_flag = 0;
    }
    local_data_processing_settings.clutter_map_settings.clutter_map_generate_weight = ui->lineEdit_clutter_map_generate_weight->text().toDouble();
    local_data_processing_settings.clutter_map_settings.clutter_map_filter_threshold = ui->lineEdit_clutter_map_filter_threshold->text().toDouble();

    // 过滤云雨杂波
    if (ui->checkBox_enable_filer_cloud->isChecked())
    {
        local_data_processing_settings.filter_cloud_settings.enable_filter_cloud = 1;
    }
    else
    {
        local_data_processing_settings.filter_cloud_settings.enable_filter_cloud = 0;
    }
    local_data_processing_settings.filter_cloud_settings.filter_cloud_threshold = ui->lineEdit_filter_cloud_threshold->text().toInt();

    //  融合门限
    local_data_processing_settings.combine_thresholds.combine_range_unit_threshold = ui->lineEdit_combine_range_unit_threshold->text().toInt();
    local_data_processing_settings.combine_thresholds.combine_doppler_unit_threshold = ui->lineEdit_combine_doppler_unit_threshold->text().toInt();
    local_data_processing_settings.combine_thresholds.combine_range_threshold = ui->lineEdit_combine_range_threshold->text().toDouble();
    local_data_processing_settings.combine_thresholds.combine_velocity_threshold = ui->lineEdit_combine_velocity_threshold->text().toDouble();
    local_data_processing_settings.combine_thresholds.combine_azim_threshold_deg = ui->lineEdit_combine_azim_threshold_deg->text().toDouble();
    local_data_processing_settings.combine_thresholds.combine_elev_threshold_deg = ui->lineEdit_combine_elev_threshold_deg->text().toDouble();

    // 相关门限
    for (int ii = 0; ii < 2; ++ii)
    {
        local_data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_floor = ui->tableWidget_correlate_parameters->item(0, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_range_threshold_ceil = ui->tableWidget_correlate_parameters->item(1, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_same_direction = ui->tableWidget_correlate_parameters->item(2, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_velocity_threshold_opposite_direction =
            ui->tableWidget_correlate_parameters->item(3, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_0 = ui->tableWidget_correlate_parameters->item(4, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_0 = ui->tableWidget_correlate_parameters->item(5, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_range_division_01 = ui->tableWidget_correlate_parameters->item(6, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_1 = ui->tableWidget_correlate_parameters->item(7, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_1 = ui->tableWidget_correlate_parameters->item(8, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_range_division_12 = ui->tableWidget_correlate_parameters->item(9, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_azim_threshold_deg_2 = ui->tableWidget_correlate_parameters->item(10, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_elev_threshold_deg_2 = ui->tableWidget_correlate_parameters->item(11, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_x_threshold = ui->tableWidget_correlate_parameters->item(12, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_y_threshold = ui->tableWidget_correlate_parameters->item(13, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_z_threshold = ui->tableWidget_correlate_parameters->item(14, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_vx_threshold = ui->tableWidget_correlate_parameters->item(15, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_vy_threshold = ui->tableWidget_correlate_parameters->item(16, ii)->text().toDouble();
        local_data_processing_settings.correlate_thresholds[ii].correlate_vz_threshold = ui->tableWidget_correlate_parameters->item(17, ii)->text().toDouble();
    }

    // 航迹平滑
    local_data_processing_settings.smooth_track_settings[0].filter_track_method = ui->comboBox_filter_track_method->currentIndex();
    local_data_processing_settings.smooth_track_settings[0].filter_track_coef_alpha = ui->lineEdit_filter_track_coef_alpha->text().toDouble();
    local_data_processing_settings.smooth_track_settings[0].filter_track_coef_beta = ui->lineEdit_filter_track_coef_beta->text().toDouble();
    local_data_processing_settings.smooth_track_settings[0].filter_track_coef_gama = ui->lineEdit_filter_track_coef_gama->text().toDouble();
    if (ui->checkBox_track_smooth->isChecked())
    {
        local_data_processing_settings.smooth_track_settings[0].smooth_track_flag = 1;
    }
    else
    {
        local_data_processing_settings.smooth_track_settings[0].smooth_track_flag = 0;
    }

    local_data_processing_settings.smooth_track_settings[1].filter_track_method = ui->comboBox_filter_track_method_1->currentIndex();
    local_data_processing_settings.smooth_track_settings[1].filter_track_coef_alpha = ui->lineEdit_filter_track_coef_alpha_1->text().toDouble();
    local_data_processing_settings.smooth_track_settings[1].filter_track_coef_beta = ui->lineEdit_filter_track_coef_beta_1->text().toDouble();
    local_data_processing_settings.smooth_track_settings[1].filter_track_coef_gama = ui->lineEdit_filter_track_coef_gama_1->text().toDouble();
    if (ui->checkBox_track_smooth_1->isChecked())
    {
        local_data_processing_settings.smooth_track_settings[1].smooth_track_flag = 1;
    }
    else
    {
        local_data_processing_settings.smooth_track_settings[1].smooth_track_flag = 0;
    }

    // 航迹滤波
    if (ui->checkBox_filter_tracks->isChecked())
    {
        local_data_processing_settings.filter_tracks_settings.filter_tracks_flag = 1;
    }
    else
    {
        local_data_processing_settings.filter_tracks_settings.filter_tracks_flag = 0;
    }
    local_data_processing_settings.filter_tracks_settings.filter_tracks_range_threshold = ui->lineEdit_filter_tracks_range_threshold->text().toDouble();
    local_data_processing_settings.filter_tracks_settings.filter_tracks_velocity_threshold = ui->lineEdit_filter_tracks_velocity_threshold->text().toDouble();
    local_data_processing_settings.filter_tracks_settings.filter_tracks_mean_delta_range_threshold = ui->lineEdit_filter_tracks_mean_delta_range_threshold->text().toDouble();
}

void RadarDataProcessingSettingsDialog::sltOnClickPushButtonApply()
{
    qDebug() << tr("[RadarDataProcessingSettingsDialog]: sltOnClickPushButtonApply");
    RadarDataProcessingSettingsDialog::updateLocalDataProcessingSettingsBasedOnParametersOnDialog();
    emit sigUpdateLocalDataProcessingSettings(local_data_processing_settings);
}

void RadarDataProcessingSettingsDialog::sltOnClickPushButtonCancel()
{
    RadarDataProcessingSettingsDialog::close();
}

void RadarDataProcessingSettingsDialog::sltOnClickPushButtonOK()
{
    RadarDataProcessingSettingsDialog::sltOnClickPushButtonApply();
    RadarDataProcessingSettingsDialog::close();
}

void RadarDataProcessingSettingsDialog::sltUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings)
{
    memcpy(&local_data_processing_settings, &new_data_processing_settings, sizeof(DataProcessingSettings));
}
