#ifndef SPC_SUBMENU_H
#define SPC_SUBMENU_H

#include <QDialog>
#include <QComboBox>
#include <QButtonGroup>

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
    const QStringList solve_dynamics = {"GAUSS_ELIM", "ORDER_M"};
    const QStringList mass_prop_ref = {"REFPT_CM", "REFPT_JOINT"};
    const QStringList joint_type = {"PASSIVE", "ACTUATED"};
    const QStringList rot_joint_seqtype = {"GIMBAL", "SPHERICAL"};
    const QStringList thruster_mode = {"PULSED", "PROPORTIONAL"};
    const QStringList axis = {"X_AXIS", "Y_AXIS", "Z_AXIS"};

    int global_spc_index = -1;
    int global_spc_ignore = 0;

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

    long body_entries = 10;
    long joint_entries = 16;
    long wheel_entries = 8;
    long mtb_entries = 4;
    long thr_entries = 6;
    long gyro_entries = 11;
    long mag_entries = 8;
    long css_entries = 11;
    long fss_entries = 11;
    long strack_entries = 11;
    long gps_entries = 6;
    long acc_entries = 11;
};

#endif // SPC_SUBMENU_H
