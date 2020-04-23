#include "convert_lon_lat_to_range_azim_dialog.h"
#include "ui_convert_lon_lat_to_range_azim_dialog.h"
#include "global_functions.h"

ConvertLonLatToRangeAzimDialog::ConvertLonLatToRangeAzimDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ConvertLonLatToRangeAzimDialog)
{
    ui->setupUi(this);
    ui->lineEdit_lon1->setText(QString::number(0));
    ui->lineEdit_lat1->setText(QString::number(0));
    ui->lineEdit_lon2->setText(QString::number(0));
    ui->lineEdit_lat2->setText(QString::number(0));
    ui->lineEdit_range2to1->setText(QString::number(0));
    ui->lineEdit_azim2to1->setText(QString::number(0));
    connect(ui->pushButton_LonLatToRangeAzim, &QAbstractButton::clicked, this, &ConvertLonLatToRangeAzimDialog::sltConvertLonLatToRangeAzim);
    connect(ui->pushButton_RangeAzimToLonLat, &QAbstractButton::clicked, this, &ConvertLonLatToRangeAzimDialog::sltConvertRangeAzimToLonLat);
}

ConvertLonLatToRangeAzimDialog::~ConvertLonLatToRangeAzimDialog()
{
    delete ui;
}

void ConvertLonLatToRangeAzimDialog::changeEvent(QEvent *e)
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

void ConvertLonLatToRangeAzimDialog::sltConvertLonLatToRangeAzim()
{
    double lon1 = ui->lineEdit_lon1->text().toDouble();
    double lat1 = ui->lineEdit_lat1->text().toDouble();
    double lon2 = ui->lineEdit_lon2->text().toDouble();
    double lat2 = ui->lineEdit_lat2->text().toDouble();
    double range_2to1 = 0;
    double azim_2to1 = 0;

    convertLonLatToRangeAzim(lon1, lat1, lon2, lat2, range_2to1, azim_2to1);

    ui->lineEdit_range2to1->setText(QString::number(range_2to1, 'f', 6));
    ui->lineEdit_azim2to1->setText(QString::number(azim_2to1, 'f', 6));
}

void ConvertLonLatToRangeAzimDialog::sltConvertRangeAzimToLonLat()
{
    double lon1 = ui->lineEdit_lon1->text().toDouble();
    double lat1 = ui->lineEdit_lat1->text().toDouble();
    double range_2to1 = ui->lineEdit_range2to1->text().toDouble();
    double azim_2to1 = ui->lineEdit_azim2to1->text().toDouble();
    double lon2 = 0;
    double lat2 = 0;

    convertRangeAzimToLonLat(lon1, lat1, range_2to1, azim_2to1, lon2, lat2);

    ui->lineEdit_lon2->setText(QString::number(lon2, 'f', 8));
    ui->lineEdit_lat2->setText(QString::number(lat2, 'f', 8));
}
