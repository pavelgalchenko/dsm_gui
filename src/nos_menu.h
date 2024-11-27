#ifndef NOS_MENU_H
#define NOS_MENU_H

#include "dsm_gui_lib.h"
#include <QDebug>
#include <QDialog>

namespace Ui {
class NOS_Menu;
}

class NOS_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit NOS_Menu(QWidget *parent = nullptr);
   ~NOS_Menu();

   private slots:
   void receive_nospath(QString);
   void apply_data();
   void write_data(YAML::Node);

   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   private:
   Ui::NOS_Menu *ui;

   QString inout_path;
   QString file_path;
   YAML::Node nos3_yaml;
};

#endif // NOS_MENU_H
