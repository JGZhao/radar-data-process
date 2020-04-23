#ifndef RADAR_SOFTWARE_SETTINGS_DIALOG_H
#define RADAR_SOFTWARE_SETTINGS_DIALOG_H

#include <QDialog>
#include "global_settings.h"

namespace Ui
{
class RadarSoftwareSettingsDialog;
}

class RadarSoftwareSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RadarSoftwareSettingsDialog(QWidget* parent = nullptr);
    ~RadarSoftwareSettingsDialog();

private:
    Ui::RadarSoftwareSettingsDialog* ui;
    SoftwareSettings local_software_settings;
    DbfCorrectCoefFileDirSettings local_dbf_correct_coef_file_dir_settings;

    void changeEvent(QEvent* e);
    void updateLocalSoftwareSettingsBasedOnParametersOnDialog();

public:
    void updateParametersOnDialogBasedOnLocalSoftwareSettings();

private slots:
    void sltOnClickPushButtonApply();
    void sltOnClickPushButtonCancel();
    void sltOnClickPushButtonOK();

    void sltOnClickPushButtonSelectMasterExternalFile();
    void sltOnClickPushButtonSelectSlaveExternalFile();
    void sltOnClickPushButtonSelectMasterInternalFile();
    void sltOnClickPushButtonSelectSlaveInternalFile();

signals:
    void sigUpdateSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sigUpdateDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& local_dbf_correct_coef_file_dir_settings);

public slots:
    void sltUpdateLocalSoftwareSettings(const SoftwareSettings& new_software_settings);
    void sltUpdateLocalDbfCorrectCoefFileDirSettings(const DbfCorrectCoefFileDirSettings& new_dbf_correct_coef_file_dir_settings);
};

#endif // RADAR_SOFTWARE_SETTINGS_DIALOG_H
