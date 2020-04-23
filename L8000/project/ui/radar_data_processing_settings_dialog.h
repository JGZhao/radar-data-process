#ifndef RADAR_DATA_PROCESSING_SETTINGS_DIALOG_H
#define RADAR_DATA_PROCESSING_SETTINGS_DIALOG_H

#include <QDialog>
#include "global_settings.h"

namespace Ui
{
class RadarDataProcessingSettingsDialog;
}

class RadarDataProcessingSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RadarDataProcessingSettingsDialog(QWidget* parent = nullptr);
    ~RadarDataProcessingSettingsDialog();

private:
    Ui::RadarDataProcessingSettingsDialog* ui;
    DataProcessingSettings local_data_processing_settings;

    void changeEvent(QEvent* e);
    void updateLocalDataProcessingSettingsBasedOnParametersOnDialog();

public:
    void updateParametersOnDialogBasedOnLocalDataProcessingSettings();

private slots:
    void sltOnClickPushButtonApply();
    void sltOnClickPushButtonCancel();
    void sltOnClickPushButtonOK();

signals:
    void sigUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings);

public slots:
    void sltUpdateLocalDataProcessingSettings(const DataProcessingSettings& new_data_processing_settings);
};

#endif // RADAR_DATA_PROCESSING_SETTINGS_DIALOG_H
