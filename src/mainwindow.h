#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dsm_gui_lib.h"
#include "dsm_menu.h"
#include "fov_menu.h"
#include "grh_menu.h"
#include "ipc_menu.h"
#include "nos_menu.h"
#include "orb_menu.h"
#include "rgn_menu.h"
#include "sim_menu.h"
#include "spc_menu.h"
#include "tdr_menu.h"
#include <QMainWindow>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
   Q_OBJECT

   public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

   signals:
   void send_data(QString);

   public slots:
   void enable_sub_menus();
   void disable_sub_menus();

   private slots:

   void on_new_mission_clicked();

   void on_GRH_Menu_clicked();
   void on_TDR_Menu_clicked();
   void on_FOV_Menu_clicked();
   void on_NOS_Menu_clicked();
   void on_RGN_Menu_clicked();
   void on_IPC_Menu_clicked();
   void on_SPC_Menu_clicked();
   void on_ORB_Menu_clicked();
   void on_SIM_Menu_clicked();

   void on_DSM_Menu_clicked();
   void on_load_mission_clicked();
   void set_menu_buttons(bool enabled);

   private:
   Ui::MainWindow *ui;

   GRH_Menu *grh_menu;
   TDR_Menu *tdr_menu;
   FOV_Menu *fov_menu;
   NOS_Menu *nos_menu;
   RGN_Menu *rgn_menu;
   IPC_Menu *ipc_menu;
   SPC_Menu *spc_menu;
   ORB_Menu *orb_menu;
   SIM_Menu *sim_menu;
   DSM_Menu *dsm_menu;
   QString path;
   QString appPath;
   QString pythonCmd;

   const QStringList neededFiles = {
       "Inp_Cmd.txt",  "Inp_FOV.yaml",  "Inp_Graphics.yaml",
       "Inp_IPC.yaml", "Inp_NOS3.yaml", "Inp_Region.yaml",
       "Inp_Sim.yaml", "Inp_TDRS.yaml", "Inp_DSM.yaml"};
};
#endif // MAINWINDOW_H
