#ifndef SPC_SUBMENU_H
#define SPC_SUBMENU_H

#include <QDialog>
#include <QComboBox>
#include <QButtonGroup>
#include <QListWidgetItem>
#include <QHash>

namespace Ui {
class SPC_submenu;
}

class SPC_submenu : public QDialog
{
    Q_OBJECT

public:
    explicit SPC_submenu(QWidget *parent = nullptr);
    ~SPC_submenu();

private slots:
    void receive_spc_sm_path(QString, QString);

    void receive_data();
    void apply_data();
    void write_data();

    void set_validators();

    void on_spc_cur_load_clicked();
    void on_spc_cur_save_clicked();
    void on_spc_cur_close_clicked();
    void on_spc_cur_apply_clicked();

    void setQComboBox(QComboBox *comboBox, QString string);

    void on_spc_cur_body_remove_clicked();

    void on_spc_cur_body_add_clicked();

    void on_spc_cur_body_duplicate_clicked();

    void on_spc_cur_body_list_itemClicked(QListWidgetItem *item);


    void on_sections_tabBarClicked(int index);

    void on_spc_cur_joint_list_itemClicked(QListWidgetItem *item);

    void on_spc_cur_wheel_add_clicked();

    void on_spc_cur_wheel_remove_clicked();

    void on_spc_cur_wheel_duplicate_clicked();

    void on_spc_cur_wheel_list_itemClicked(QListWidgetItem *item);

    void on_spc_cur_mtb_remove_clicked();

    void on_spc_cur_mtb_add_clicked();

    void on_spc_cur_mtb_duplicate_clicked();

    void on_spc_cur_mtb_list_itemClicked(QListWidgetItem *item);

    void on_spc_cur_thruster_remove_clicked();

    void on_spc_cur_thruster_add_clicked();

    void on_spc_cur_thruster_duplicate_clicked();

    void on_spc_cur_thruster_list_itemClicked(QListWidgetItem *item);

    void on_spc_cur_gyro_remove_clicked();

    void on_spc_cur_gyro_add_clicked();

    void on_spc_cur_gyro_duplicate_clicked();

    void on_spc_cur_gyro_list_itemClicked(QListWidgetItem *item);

    void on_spc_cur_mag_remove_clicked();

    void on_spc_cur_mag_add_clicked();

    void on_spc_cur_mag_duplicate_clicked();

    void on_spc_cur_mag_list_itemClicked(QListWidgetItem *item);

    void on_spc_cur_css_remove_clicked();

    void on_spc_cur_css_add_clicked();

    void on_spc_cur_css_duplicate_clicked();

    void on_spc_cur_css_list_itemClicked(QListWidgetItem *item);

private:
    Ui::SPC_submenu *ui;
    QString spc_cur_name;
    QString spc_cur_file;

    // All ComboBox options
    const QStringList orbit_type = {"FIXED", "EULER_HILL", "ENCKE", "COWELL"};
    const QStringList pos_wrt_F = {"CM", "ORIGIN"};
    const QStringList angvel_wrt = {"N", "L"};
    const QStringList att_params = {"Q", "A"};
    const QStringList att_wrt = {"N", "L", "F"};
    const QStringList euler_seq = {"123", "231", "312", "132", "213", "321", "121", "131", "212", "232", "313","323"};
    const QStringList solve_dynamics = {"GAUSS_ELIM", "ORDER_N"};
    const QStringList mass_prop_ref = {"REFPT_CM", "REFPT_JOINT"};
    const QStringList joint_type = {"PASSIVE", "ACTUATED"};
    const QStringList rot_joint_seqtype = {"GIMBAL", "SPHERICAL"};
    const QStringList thruster_mode = {"PULSED", "PROPORTIONAL"};
    const QStringList axis = {"X_AXIS", "Y_AXIS", "Z_AXIS"};

    int global_spc_index = -1;
    int global_spc_ignore = 0;

    int global_body_index = -1;

    QString inout_path;
    QString file_path;

    QStringList spc_names;
    QStringList spc_string;
    QStringList spc_data;
    QStringList spc_update;
    int spc_name_index;

    QStringList spc_file_headers; // section headers in the file
    QStringList spc_file_descrip; // data descriptors in the file

    int bodies;
    int joints;
    int wheels;
    int mtbs;
    int thrusters;
    int gyros;
    int mags;
    int css_s;
    int fss_s;
    int stracks;
    int gps_s;
    int accels;

    long reset_ind_body = 29;

    long reset_ind_joint;
    long reset_ind_wheel;
    long reset_ind_mtb;
    long reset_ind_thr;
    long reset_ind_gyro;
    long reset_ind_mag;
    long reset_ind_css;
    long reset_ind_fss;
    long reset_ind_strack;
    long reset_ind_gps;
    long reset_ind_acc;
    long reset_ind_end;

    long body_headers = 4;
    long joint_headers = 4;
    long wheel_headers = 4;
    long mtb_headers = 2;
    long thr_headers = 2;
    long gyro_headers = 2;
    long mag_headers = 2;
    long css_headers = 2;
    long fss_headers = 2;
    long strack_headers = 2;
    long gps_headers = 2;
    long accel_headers = 2;

    long body_entries = 10;
    long joint_entries = 16;
    long wheel_entries = 8;
    long mtb_entries = 4;
    long thr_entries = 6;
    long gyro_entries = 11;
    long mag_entries = 8;
    long css_entries = 8;
    long fss_entries = 8;
    long strack_entries = 8;
    long gps_entries = 6;
    long acc_entries = 11;

};

#endif // SPC_SUBMENU_H
