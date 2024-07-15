#ifndef SPC_Menu_H
#define SPC_Menu_H

#include <QComboBox>
#include <QDialog>
#include <QListWidgetItem>
#include <QStringList>

#include "dsm_gui_lib.h"
#include "spc_submenu.h"

namespace Ui {
class SPC_Menu;
}

class SPC_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit SPC_Menu(QWidget *parent = nullptr);
   ~SPC_Menu();

   signals:
   void send_data(QString, QString);
   void name_changed();

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
   void write_data(YAML::Node inp_spc);

   void receive_spcpath(QString path);
   void receive_apppath(QString path);
   void receive_pythoncmd(QString cmd);

   void on_spc_list_itemActivated(QListWidgetItem *item);

   void set_validators();

   void setQComboBox(QComboBox *comboBox, QString string);

   void on_spc_cur_att_param_currentTextChanged(const QString &arg1);

   void on_spc_list_currentTextChanged(const QString &currentText);

   void on_SPC_Menu_rejected();

   void on_sc3u_clicked();
   void on_sc6u_clicked();
   void on_sc12u_clicked();

   void proc_add_template(QString sc_template_name, long counter);
   void load_specific_file(QString load_sc_name, long counter);
   void load_1SC_default(QString sc_string);

   private:
   Ui::SPC_Menu *ui;

   int global_spc_index  = -1;
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
   int new_item;
   long counter3u;
   long counter6u;
   long counter12u;

   QStringList spc_file_headers; // section headers in the file
   QStringList spc_file_descrip; // data descriptors in the file

   SPC_submenu *spc_submenu;

   const QStringList fswid_types = {"PASSIVE_FSW", "PROTOTYPE_FSW",
                                    "AD_HOC_FSW",  "SPINNER_FSW",
                                    "MOMBIAS_FSW", "THREE_AXIS_FSW",
                                    "ISS_FSW",     "CMG_FSW",
                                    "THR_FSW",     "DSM_FSW",
                                    "CFS_FSW"}; //"PASSIVE_FSW",
                                                //"PROTOTYPE_FSW", "AD_HOC_FSW",
                                                //"SPINNER_FSW", "MOMBIAS_FSW",
                                                //"THREE_AXIS_FSW", "ISS_FSW",
                                                //"CMG_FSW", "THR_FSW",
                                                //"DSM_FSW", "CFS_FSW"
   const QStringList orbit_type = {"FIXED", "EULER_HILL", "ENCKE", "COWELL"};
   const QStringList pos_wrt_F  = {"CM", "ORIGIN"};
   const QStringList angvel_wrt = {"N", "L"};
   const QStringList att_params = {"Q", "A"};
   const QStringList att_wrt    = {"N", "L", "F"};

   YAML::Node cur_spc_yaml;
   QString appPath;
   QString pythonCmd;
};

#endif // SPC_Menu_H
