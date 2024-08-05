#ifndef SPC_SUBMENU_H
#define SPC_SUBMENU_H

#include "dsm_gui_lib.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHash>
#include <QList>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

namespace Ui {
class SPC_submenu;
}

class SPC_submenu : public QDialog {
   Q_OBJECT

   public:
   explicit SPC_submenu(QWidget *parent = nullptr);
   ~SPC_submenu();

   private slots:
   void receive_spc_sm_path(QString, QString);

   void receive_data();
   void apply_data();
   void write_data(YAML::Node inp_spc);

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

   void on_spc_cur_fss_remove_clicked();
   void on_spc_cur_fss_add_clicked();
   void on_spc_cur_fss_duplicate_clicked();
   void on_spc_cur_fss_list_itemClicked(QListWidgetItem *item);

   void on_spc_cur_strack_remove_clicked();
   void on_spc_cur_strack_add_clicked();
   void on_spc_cur_strack_duplicate_clicked();
   void on_spc_cur_strack_list_itemClicked(QListWidgetItem *item);

   void on_spc_cur_gps_remove_clicked();
   void on_spc_cur_gps_add_clicked();
   void on_spc_cur_gps_duplicate_clicked();
   void on_spc_cur_gps_list_itemClicked(QListWidgetItem *item);

   void on_spc_cur_accel_remove_clicked();
   void on_spc_cur_accel_add_clicked();
   void on_spc_cur_accel_duplicate_clicked();
   void on_spc_cur_accel_list_itemClicked(QListWidgetItem *item);

   void on_spc_cur_joint_remove_clicked();
   void on_spc_cur_joint_add_clicked();
   void on_spc_cur_joint_duplicate_clicked();
   void on_spc_cur_shaker_select_clicked();

   void on_spc_cur_node_select_clicked();
   void on_spc_cur_flex_select_clicked();
   void on_spc_cur_joint_param_select_clicked();

   void on_spc_cur_shaker_clear_clicked();
   void on_spc_cur_node_clear_clicked();
   void on_spc_cur_flex_clear_clicked();
   void on_spc_cur_joint_param_clear_clicked();
   void on_spc_cur_wheel_drjit_select_clicked();
   void on_spc_cur_wheel_drjit_clear_clicked();

   void on_actuator_sections_tabBarClicked(int index);
   void on_sensor_sections_tabBarClicked(int index);

   void proc_duplicates(QListWidget *cur_list);

   void proc_add(QListWidget *cur_list, QStringList tmp_data);
   void destroy_submenu(QString command, QString junk);

   void receive_apppath(QString path, QString junk);
   void receive_pythoncmd(QString cmd, QString junk);

   private:
   Ui::SPC_submenu *ui;
   QString spc_cur_name;
   QString spc_cur_file;

   QString appPath;
   QString pythonCmd;
   YAML::Node cur_spc_yaml;
   // All ComboBox options
   const QStringList orbit_type = {"FIXED", "EULER_HILL", "ENCKE", "COWELL"};
   const QStringList pos_wrt_F  = {"CM", "ORIGIN"};
   const QStringList angvel_wrt = {"N", "L"};
   const QStringList att_params = {"Q", "A"};
   const QStringList att_wrt    = {"N", "L", "F"};
   const QStringList solve_dynamics = {"GAUSS_ELIM", "ORDER_N"};
   const QStringList mass_prop_ref  = {"REFPT_CM", "REFPT_JOINT"};
   const QStringList joint_type     = {
       "PASSIVE",         "ACTUATED",           "STEPPER_MOTOR",
       "TVC_JOINT",       "VIBRATION_ISOLATOR", "SLOSH",
       "STEERING_MIRROR", "AD_HOC_JOINT"};
   const QStringList rot_joint_seqtype = {"GIMBAL", "SPHERICAL"};
   const QStringList thruster_mode     = {"PULSED", "PROPORTIONAL"};
   const QStringList axis              = {"X_AXIS", "Y_AXIS", "Z_AXIS"};
   int come_from_bodyjoint;

   int cur_item_row;

   QString inout_path;
   QString file_path;

   QStringList spc_names;
   QStringList spc_string;
   QStringList spc_data;
   QStringList spc_update;
   QStringList spc_item_names;
   int spc_name_index;

   QStringList spc_file_headers; // section headers in the file
   QStringList spc_file_descrip; // data descriptors in the file

   QString wheel_drag;
   QString wheel_jitter;

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

   int joints_valid    = 1;
   int recursion_guard = 0;

   YAML::Node spc_comments;

   public slots:

   signals:
   void refresh_spc_menu(QString path);
};

#endif // SPC_SUBMENU_H
