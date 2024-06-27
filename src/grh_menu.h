#ifndef GRH_MENU_H
#define GRH_MENU_H

#include "dsm_gui_lib.h"

#include <QDebug>
#include <QDialog>
#include <QFileDialog>

namespace Ui {
class GRH_Menu;
}

class GRH_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit GRH_Menu(QWidget *parent = nullptr);
   ~GRH_Menu();

   private slots:
   void set_validators();
   void receive_grhpath(QString);
   void receive_data();
   void apply_data();
   void write_data();

   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void on_boresightaxis_currentTextChanged(const QString &arg1);

   void on_hostSC_currentTextChanged(const QString &arg1);

   void on_targetSC_currentTextChanged(const QString &arg1);

   private:
   Ui::GRH_Menu *ui;

   QHash<QString, int> scNums;
   QString inout_path;
   QString graphics_path;
   QStringList grh_data;
   QStringList grh_update;
   QStringList grh_string;
   QHash<QString, QString> upaxis_inputs;

   const QHash<QString, QString> povMode_inputs = {
       {"TRACK_HOST", "Track Host"},
       {"TRACK_TARGET", "Track Target"},
       {"FIXED_IN_HOST", "Fixed in Host"}};
   const QHash<QString, QString> hosttarget_inputs = {{"WORLD", "World"},
                                                      {"REFORB", "Ref Orbit"},
                                                      {"FRM", "Frame"},
                                                      {"SC", "Spacecraft"},
                                                      {"BODY", "Body"}};
   const QHash<QString, QString> frame_inputs      = {{"N", "Inertial"},
                                                      {"L", "Local Vert"},
                                                      {"F", "Formation"},
                                                      {"S", "Spacecraft"},
                                                      {"B", "Body"}};
   const QHash<QString, QString> axis_inputs       = {
       {"POS_X", "+x"}, {"POS_Y", "+y"}, {"POS_Z", "+z"},
       {"NEG_X", "-x"}, {"NEG_Y", "-y"}, {"NEG_Z", "-z"}};
   const QHash<QString, QString> view_inputs = {{"FRONT", "Front"},
                                                {"FRONT_RIGHT", "Front Right"},
                                                {"FRONT_LEFT", "Front Left"},
                                                {"REAR", "Rear"},
                                                {"REAR_LEFT", "Rear Left"},
                                                {"REAR_RIGHT", "Rear Right"},
                                                {"UP", "Up"},
                                                {"DOWN", "Down"},
                                                {"LEFT", "Left"},
                                                {"RIGHT", "Right"}};
};

#endif // GRH_MENU_H
