#ifndef IPC_MENU_H
#define IPC_MENU_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QInputDialog>
#include <dsm_gui_lib.h>

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
    void write_data();
    void clear_fields();

    void on_ipc_remove_clicked();
    void on_ipc_add_clicked();
    void on_ipclist_itemClicked(QListWidgetItem *item);
    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void on_prefix_remove_clicked();
    void on_prefix_add_clicked();

    void on_prefix_rename_clicked();

    void on_ipc_duplicate_clicked();

    void server_changed();

    void on_ipcmode_currentTextChanged(const QString &arg1);

    void on_acs_id_textChanged(const QString &arg1);

    void on_filename_textChanged(const QString &arg1);

    void on_socketrole_currentTextChanged(const QString &arg1);

    void on_blocking_toggled(bool checked);

    void on_echo_toggled(bool checked);

    void on_prefixlist_currentRowChanged(int currentRow);
    void update_prefixes(QListWidgetItem* item);

private:
    Ui::IPC_Menu *ui;

    QString inout_path;
    QString file_path;
    QVector<int> ipc_name_index;
    QVector<int> ipc_name_size;
    QVector<int> ipc_name_prefixes;
    QStringList ipc_data;
    QStringList ipc_update;
    QStringList ipc_string;

    enum ipcDataRoles {
        Name = Qt::DisplayRole,
        Mode = Qt::UserRole,
        ACID,
        FileName,
        Role,
        Server,
        Blocking,
        Echo,
        nTX,
        Prefixes
    };

    const int ipcNLines = 9;

    QHash<QString,QString> ipcmodeinputs = { {"OFF","Off"},
                                             {"TX","TX"},
                                             {"RX","RX"},
                                             {"TXRX","TXRX"},
                                             {"ACS","ACS"},
                                             {"WRITEFILE","Write to File"},
                                             {"READFILE","Read from File"}};
    QHash<QString,QString> socketrole_inputs = { {"SERVER","Server"},
                                                 {"CLIENT","Client"},
                                                 {"GMSEC_CLIENT","GMSEC Client"}};

};

#endif // IPC_MENU_H
