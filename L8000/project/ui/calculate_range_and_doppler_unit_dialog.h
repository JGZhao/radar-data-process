#ifndef CALCULATE_RANGE_AND_DOPPLER_UNIT_DIALOG_H
#define CALCULATE_RANGE_AND_DOPPLER_UNIT_DIALOG_H

#include <QDialog>

const double LIGHT_SPEED = 3e8; // 光速

namespace Ui
{
class CalculateRangeAndDopplerUnitDialog;
}

class CalculateRangeAndDopplerUnitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalculateRangeAndDopplerUnitDialog(QWidget *parent = nullptr);
    ~CalculateRangeAndDopplerUnitDialog();

private:
    Ui::CalculateRangeAndDopplerUnitDialog *ui;

private:
    void CalculateRangeUnitDopplerUnit(double _fc,
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
                                       double &_velocity_resolution);
    void changeEvent(QEvent *e);

private slots:
    void sltCalculateRangeUnitDopplerUnit();
};

#endif // CALCULATE_RANGE_AND_DOPPLER_UNIT_DIALOG_H
