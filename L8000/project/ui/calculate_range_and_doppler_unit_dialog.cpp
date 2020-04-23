#include "calculate_range_and_doppler_unit_dialog.h"
#include "ui_calculate_range_and_doppler_unit_dialog.h"
#include "global_definitions.h"
#include <QPushButton>

CalculateRangeAndDopplerUnitDialog::CalculateRangeAndDopplerUnitDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CalculateRangeAndDopplerUnitDialog)
{
    ui->setupUi(this);
    ui->lineEdit_fc->setText(QString::number(16));
    ui->lineEdit_fs->setText(QString::number(50));
    ui->lineEdit_pw->setText(QString::number(1));
    ui->lineEdit_bw->setText(QString::number(40));
    ui->lineEdit_prt->setText(QString::number(41.4));
    ui->lineEdit_mtd_pulse_num->setText(QString::number(256));
    ui->lineEdit_mtd_fft_num->setText(QString::number(256));
    ui->lineEdit_range_units_num->setText(QString::number(1024));
    ui->lineEdit_real_blind_len->setText(QString::number(0));
    ui->lineEdit_range_unit->setText(QString::number(0));
    ui->lineEdit_velocity_unit->setText(QString::number(0));
    ui->lineEdit_theoretical_blind_len->setText(QString::number(0));
    ui->lineEdit_range_resolution->setText(QString::number(0));
    ui->lineEdit_velocity_resolution->setText(QString::number(0));
    connect(ui->pushButton_calculate, &QAbstractButton::clicked, this, &CalculateRangeAndDopplerUnitDialog::sltCalculateRangeUnitDopplerUnit);
}

CalculateRangeAndDopplerUnitDialog::~CalculateRangeAndDopplerUnitDialog()
{
    delete ui;
}

void CalculateRangeAndDopplerUnitDialog::CalculateRangeUnitDopplerUnit(double _fc,
                                                                       double _fs,
                                                                       double _pw,
                                                                       double _bw,
                                                                       double _prt,
                                                                       int _mtd_pulse_num,
                                                                       int _mtd_fft_num,
                                                                       int _range_units_num,
                                                                       double _real_blind_len,
                                                                       double &_range_unit,
                                                                       double &_velocity_unit,
                                                                       double &_theoretical_blind_len,
                                                                       double &_range_resolution,
                                                                       double &_velocity_resolution)
{
    double prf = 1 / (_prt + EPS);
    _range_unit = LIGHT_SPEED / _fs / 2;
    _velocity_unit = prf / _mtd_fft_num * LIGHT_SPEED / _fc / 2;
    _theoretical_blind_len = _pw * LIGHT_SPEED / 2;
    _range_resolution = LIGHT_SPEED / 2 / _bw;
    _velocity_resolution = prf / _mtd_pulse_num * LIGHT_SPEED / _fc / 2;
}

void CalculateRangeAndDopplerUnitDialog::changeEvent(QEvent *e)
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

void CalculateRangeAndDopplerUnitDialog::sltCalculateRangeUnitDopplerUnit()
{
    double fc = ui->lineEdit_fc->text().toDouble() * 1e9;
    double fs = ui->lineEdit_fs->text().toDouble() * 1e6;
    double pw = ui->lineEdit_pw->text().toDouble() * 1e-6;
    double bw = ui->lineEdit_bw->text().toDouble() * 1e6;
    double prt = ui->lineEdit_prt->text().toDouble() * 1e-6;
    int mtd_pulse_num = ui->lineEdit_mtd_pulse_num->text().toInt();
    int mtd_fft_num = ui->lineEdit_mtd_fft_num->text().toInt();
    int range_units_num = ui->lineEdit_range_units_num->text().toInt();
    double real_blind_len = ui->lineEdit_real_blind_len->text().toDouble();
    double range_unit = 0;
    double velocity_unit = 0;
    double theoretical_blind_len = 0;
    double range_resolution = 0;
    double velocity_resolution = 0;
    CalculateRangeUnitDopplerUnit(
        fc, fs, pw, bw, prt, mtd_pulse_num, mtd_fft_num, range_units_num, real_blind_len, range_unit, velocity_unit, theoretical_blind_len, range_resolution, velocity_resolution);

    ui->lineEdit_range_unit->setText(QString::number(range_unit, 'f', 2));
    ui->lineEdit_velocity_unit->setText(QString::number(velocity_unit, 'f', 5));
    ui->lineEdit_theoretical_blind_len->setText(QString::number(theoretical_blind_len, 'f', 2));
    ui->lineEdit_range_resolution->setText(QString::number(range_resolution, 'f', 2));
    ui->lineEdit_velocity_resolution->setText(QString::number(velocity_resolution, 'f', 5));
}
