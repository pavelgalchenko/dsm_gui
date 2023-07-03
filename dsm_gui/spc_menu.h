#ifndef SPC_Menu_H
#define SPC_Menu_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
#include <QComboBox>
#include <QStringList>

#include "spc_submenu.h"

namespace Ui {
class SPC_Menu;
}

class SPC_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit SPC_Menu(QWidget *parent = nullptr);
    ~SPC_Menu();

signals:
    void send_data(QString, QString);

private slots:

    void on_spc_add_clicked();
    void on_spc_remove_clicked();
    void on_spc_duplicate_clicked();

    void on_spc_load_clicked();
    void on_spc_save_clicked();
    void on_spc_close_clicked();
    void on_spc_apply_clicked();

    void on_spc_conf_clicked();

    void on_spc_list_itemClicked(QListWidgetItem *item);

    void receive_data();
    void apply_data();
    void write_data();

    void receive_spcpath(QString);

    void on_spc_list_itemActivated(QListWidgetItem *item);

    void set_validators();

    void setQComboBox(QComboBox *comboBox, QString string);

private:
    Ui::SPC_Menu *ui;

    int global_spc_index = -1;
    int global_spc_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList file_paths;

    QStringList file_paths_default;
    QStringList spc_names_default;

    QStringList spc_names;
    QStringList spc_string;
    QStringList spc_data;
    QStringList spc_update;
    int spc_name_index;


    QStringList spc_file_headers; // section headers in the file
    QStringList spc_file_descrip; // data descriptors in the file

    SPC_submenu *spc_submenu;

    const QStringList fswid_types = {"PASSIVE_FSW", "PROTOTYPE_FSW", "AD_HOC_FSW", "SPINNER_FSW", "MOMBIAS_FSW", "THREE_AXIS_FSW", "ISS_FSW", "CMG_FSW", "THR_FSW", "DSM_FSW", "CFS_FSW"}; //"PASSIVE_FSW", "PROTOTYPE_FSW", "AD_HOC_FSW", "SPINNER_FSW", "MOMBIAS_FSW", "THREE_AXIS_FSW", "ISS_FSW", "CMG_FSW", "THR_FSW", "DSM_FSW", "CFS_FSW"
    const QStringList orbit_type = {"FIXED", "EULER_HILL", "ENCKE", "COWELL"};
    const QStringList pos_wrt_F = {"CM", "ORIGIN"};
    const QStringList angvel_wrt = {"N", "L"};
    const QStringList att_params = {"Q", "A"};
    const QStringList att_wrt = {"N", "L", "F"};
    const QStringList euler_seq = {"123", "231", "312", "132", "213", "321", "121", "131", "212", "232", "313","323"};
};

#endif // SPC_Menu_H
