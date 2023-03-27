#ifndef IPC_MENU_H
#define IPC_MENU_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>

namespace Ui {
class IPC_Menu;
}

class IPC_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit IPC_Menu(QWidget *parent = nullptr);
    ~IPC_Menu();

private slots:
    void set_validators();
    void receive_ipcpath(QString);
    void receive_data();
    void apply_data();
    void populate_list();
    void write_data();
    int warning_message(QString);
    QString whitespace(QString);

    void on_ipc_remove_clicked();
    void on_ipc_add_clicked();
    void on_ipclist_itemClicked(QListWidgetItem *item);
    //void on_loaddefaultButton_clicked();
    //void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    //void on_applyButton_clicked();



private:
    Ui::IPC_Menu *ui;

    int global_ipc_index = -1;
    int global_ipc_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList ipc_names;
    QVector<int> ipc_name_index;
    QVector<int> ipc_name_size;
    QVector<int> ipc_name_prefixes;
    QStringList ipc_data;
    QStringList ipc_update;
    QStringList ipc_string;

    QStringList ipcmodeinputs = {"OFF","TX","RX","TXRX","ACS","WRITEFILE","READFILE"};
    QStringList socketrole_inputs = {"SERVER","CLIENT","GMSEC_CLIENT"};

    int test = 0;

};

#endif // IPC_MENU_H
