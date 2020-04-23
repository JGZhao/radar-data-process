#ifndef ADAPTERS_SELECT_DIALOG_H
#define ADAPTERS_SELECT_DIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui
{
class AdaptersSelectDialog;
}

class AdaptersSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdaptersSelectDialog(QWidget* parent = nullptr);
    ~AdaptersSelectDialog();
    bool updateAdaptersList();

private:
    Ui::AdaptersSelectDialog* ui;
    QStringList m_adapters_list;
    int current_selected_adapter_index;

    void changeEvent(QEvent* e);

private slots:
    void sltChangeCurrentSelectedAdapterIndex();
    void sltOnClickButtonOK();

signals:
    void sigChangeCaptureAdapter(const int& new_adapter_index);
};

#endif // ADAPTERS_SELECT_DIALOG_H
