#ifndef CONVERT_LON_LAT_TO_RANGE_AZIM_DIALOG_H
#define CONVERT_LON_LAT_TO_RANGE_AZIM_DIALOG_H

#include <QDialog>

namespace Ui
{
class ConvertLonLatToRangeAzimDialog;
}

class ConvertLonLatToRangeAzimDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvertLonLatToRangeAzimDialog(QWidget *parent = nullptr);
    ~ConvertLonLatToRangeAzimDialog();

private:
    Ui::ConvertLonLatToRangeAzimDialog *ui;
    void changeEvent(QEvent *e);

private slots:
    void sltConvertLonLatToRangeAzim();
    void sltConvertRangeAzimToLonLat();
};

#endif // CONVERT_LON_LAT_TO_RANGE_AZIM_DIALOG_H
