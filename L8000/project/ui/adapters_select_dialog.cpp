#include "adapters_select_dialog.h"
#include "ui_adapters_select_dialog.h"
#include <QDebug>
#include "packet_cap.h"

AdaptersSelectDialog::AdaptersSelectDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AdaptersSelectDialog)
{
    ui->setupUi(this);
    m_adapters_list.clear();
    current_selected_adapter_index = -1;
    ui->listWidget_local_adapters->clear();
    updateAdaptersList();
    // 连接信号与槽
    connect(ui->listWidget_local_adapters, SIGNAL(currentRowChanged(int)), this, SLOT(sltChangeCurrentSelectedAdapterIndex()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(sltOnClickButtonOK()));
}

AdaptersSelectDialog::~AdaptersSelectDialog()
{
    delete ui;
}

bool AdaptersSelectDialog::updateAdaptersList()
{
    PacketCap packet_cap;
    m_adapters_list.clear();
    m_adapters_list = packet_cap.getFriendlyNameListOfAdapters();
    if (m_adapters_list.size() > 0)
    {
        ui->listWidget_local_adapters->clear();
        ui->listWidget_local_adapters->addItems(m_adapters_list);
        return true;
    }
    return false;
}

void AdaptersSelectDialog::changeEvent(QEvent* e)
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

void AdaptersSelectDialog::sltChangeCurrentSelectedAdapterIndex()
{
    int current_row = ui->listWidget_local_adapters->currentRow();
    if (current_row >= 0)
    {
        current_selected_adapter_index = current_row;
        qDebug() << "current_selected_adapter_index: " << current_selected_adapter_index;
    }
}

void AdaptersSelectDialog::sltOnClickButtonOK()
{
    emit sigChangeCaptureAdapter(current_selected_adapter_index);
}
