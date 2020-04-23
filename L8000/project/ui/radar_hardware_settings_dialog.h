#ifndef RADAR_HARDWARE_SETTINGSDIALOG_H
#define RADAR_HARDWARE_SETTINGSDIALOG_H

#include <QDialog>
#include "global_settings.h"

namespace Ui
{
class RadarHardwareSettingsDialog;
}

class RadarHardwareSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RadarHardwareSettingsDialog(QWidget* parent = nullptr);
    ~RadarHardwareSettingsDialog();

private:
    Ui::RadarHardwareSettingsDialog* ui;
    HardwareSettings local_hardware_settings;
    int current_set_spr_mode_index;

    void changeEvent(QEvent* e);

    void updateLocalHardwareSettingsBasedOnParametersOnDialog();

public:
    void updateParametersOnDialogBasedOnLocalHardwareSettings();

private slots:
    void sltComboBoxTurntableRotatingModeChanged();
    void sltOnLineEditSPRChanged();
    void sltOnClickPushButtonApply();
    void sltOnClickPushButtonCancel();
    void sltOnClickPushButtonOK();

signals:
    void sigUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);

public slots:
    void sltUpdateLocalHardwareSettings(const HardwareSettings& new_hardware_settings);
    void sltUpdateCurrentSetSprModeIndex(const int& new_spr_mode_index);
};

#endif // RADAR_HARDWARE_SETTINGSDIALOG_H
