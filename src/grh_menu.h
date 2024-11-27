#ifndef GRH_MENU_H
#define GRH_MENU_H

#include "dsm_gui_lib.h"

#include <QDebug>
#include <QDialog>
#include <QFileDialog>

class HostTarget {
   protected:
   enum types { WORLD = 0, REFORB, FRM, SC, BODY };
   enum frames { N = 0, L, F, S, B };

   private:
   const static inline QMap<QString, enum types> type_strs = {
       {"WORLD", WORLD},
       {"REFORB", REFORB},
       {"FRM", FRM},
       {"SC", SC},
       {"BODY", BODY}};
   const static inline QMap<QString, enum frames> frame_strs = {
       {"N", N}, {"L", L}, {"F", F}, {"S", S}, {"B", B}};

   enum types _type   = SC;
   long _sc           = 0;
   long _body         = 0;
   enum frames _frame = N;

   public:
   HostTarget(const QString type = type_strs.key(SC), const long sc = 0,
              const long body = 0, const QString frame = frame_strs.key(N)) {
      setType(type);
      setSC(sc, body);
      setFrame(frame);
   }
   void setType(const QString type) {
      _type = type_strs.value(type, SC);
   }
   void setSC(const long sc, const long body) {
      _sc   = std::max(sc, 0l);
      _body = std::max(body, 0l);
   }
   void setFrame(const QString frame) {
      _frame = frame_strs.value(frame, N);
   }
   QString type() const {
      return type_strs.key(_type);
   }
   long sc() const {
      return _sc;
   }
   long body() const {
      return _body;
   }
   QString frame() const {
      return frame_strs.key(_frame);
   }
};

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
   void write_data(YAML::Node);

   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void on_boresightaxis_currentTextChanged(const QString &arg1);

   void on_hostSC_currentTextChanged(const QString &arg1);

   void on_targetSC_currentTextChanged(const QString &arg1);

   private:
   Ui::GRH_Menu *ui;

   YAML::Node grh_file_yaml;
   QHash<QString, int> scNums;
   QString inout_path;
   QString graphics_path;
   QHash<QString, QString> upaxis_inputs;

   QList<QLineEdit *> ui_pov;

   QList<QString> cam_show_fields;
   QHash<QString, QCheckBox *> cam_show_checkboxes;
   QHash<QString, QLineEdit *> cam_show_names;

   QList<QString> map_show_fields;
   QHash<QString, QCheckBox *> map_show_checkboxes;
   QHash<QString, QLineEdit *> map_show_names;

   QList<QString> const_show_fields;
   QHash<QString, QCheckBox *> const_show_checkboxes;

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

// Configure YAML conversions
namespace YAML {
// HostTarget
template <> struct convert<HostTarget> {
   static Node encode(const HostTarget &rhs) {
      Node node(NodeType::Map);
      node["Type"]  = rhs.type();
      node["SC"]    = rhs.sc();
      node["Body"]  = rhs.body();
      node["Frame"] = rhs.frame();
      return node;
   }
   static bool decode(const Node &node, HostTarget &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setType(node["Type"].as<QString>());
      rhs.setSC(node["SC"].as<long>(), node["Body"].as<long>());
      rhs.setFrame(node["Frame"].as<QString>());
      return true;
   }
};

} // namespace YAML

#endif // GRH_MENU_H
