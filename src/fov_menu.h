#ifndef FOV_MENU_H
#define FOV_MENU_H

#include "dsm_gui_lib.h"

#include <QColorDialog>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

class Color {

   private:
   QVector3D _rgb = {0, 0, 0};
   double _alpha  = 1;

   public:
   Color(const QVector3D rgb = {0, 0, 0}, const double alpha = 1) {
      setRGB(rgb);
      setAlpha(alpha);
   }
   void setRGB(const QVector3D rgb) {
      for (int i = 0; i < 3; i++)
         _rgb[i] = dsm_gui_lib::limit(rgb[i], 0, 1);
   }
   void setAlpha(const double alpha) {
      _alpha = dsm_gui_lib::limit(alpha, 0, 1);
   }
   QVector3D rgb() const {
      return _rgb;
   }
   double alpha() const {
      return _alpha;
   }
   double red() const {
      return _rgb[0];
   }
   double green() const {
      return _rgb[1];
   }
   double blue() const {
      return _rgb[2];
   }
};

class Sides {

   private:
   int _n         = 0;
   double _length = 1.0;

   public:
   Sides(const int n = 0, const double len = 0) {
      setN(n);
      setLength(len);
   }
   void setN(const int n) {
      _n = std::max(n, 0);
   }
   void setLength(const double len) {
      _length = std::max(len, 0.0);
   }
   int n() const {
      return _n;
   }
   double length() const {
      return _length;
   }
};

class FOV {
   public:
   enum types { WIREFRAME = 0, SOLID, VECTOR, PLANE };
   enum boresights { X_AXIS = 0, Y_AXIS, Z_AXIS };

   protected:
   inline static const QMap<QString, enum types> type_strs = {
       {"WIREFRAME", WIREFRAME},
       {"SOLID", SOLID},
       {"VECTOR", VECTOR},
       {"PLANE", PLANE}};
   inline static const QMap<QString, enum boresights> boresight_strs = {
       {"X_AXIS", X_AXIS}, {"Y_AXIS", Y_AXIS}, {"Z_AXIS", Z_AXIS}};

   private:
   QString _label             = "";
   Sides _sides               = Sides();
   double _width              = 0;
   double _height             = 0;
   Color _color               = Color();
   enum types _type           = WIREFRAME;
   bool _near_field           = true;
   bool _far_field            = true;
   int _sc                    = 0;
   int _body                  = 0;
   QVector3D _position        = {0, 0, 0};
   EulerAngles _euler_angles  = EulerAngles();
   enum boresights _boresight = X_AXIS;

   public:
   FOV(const QString label = "", const Sides sides = Sides(),
       const double width = 0, const double height = 0,
       const Color color = Color(), const QString type = "WIREFRAME",
       const bool near = true, const bool far = true, const int sc = 0,
       const int body = 0, const QVector3D pos = {0, 0, 0},
       const EulerAngles euler = EulerAngles(),
       const QString boresight = "X_AXIS") {
      setLabel(label);
      setSides(sides);
      setWidth(width);
      setHeight(height);
      setColor(color);
      setType(type);
      setNearField(near);
      setFarField(far);
      setSC(sc);
      setBody(body);
      setPosition(pos);
      setEulerAngles(euler);
      setBoresight(boresight);
   }

   void setLabel(const QString label) {
      _label = label;
   }
   void setSides(const Sides sides) {
      _sides = sides;
   }
   void setWidth(const double width) {
      _width = width;
   }
   void setHeight(const double height) {
      _height = height;
   }
   void setColor(const Color color) {
      _color = color;
   }
   void setType(const QString type) {
      _type = type_strs.value(type, WIREFRAME);
   }
   void setNearField(const bool near) {
      _near_field = near;
   }
   void setFarField(const bool far) {
      _far_field = far;
   }
   void setSC(const int sc) {
      _sc = dsm_gui_lib::limit(sc, 0, INT32_MAX);
   }
   void setBody(const int body) {
      _body = dsm_gui_lib::limit(body, 0, INT32_MAX);
   }
   void setPosition(const QVector3D pos) {
      _position = pos;
   }
   void setEulerAngles(const EulerAngles euler) {
      _euler_angles = euler;
   }
   void setBoresight(const QString boresight) {
      _boresight = boresight_strs.value(boresight, X_AXIS);
   }

   QString label() const {
      return _label;
   }
   Sides sides() const {
      return _sides;
   }
   double width() const {
      return _width;
   }
   double height() const {
      return _height;
   }
   Color color() const {
      return _color;
   }
   QString type() const {
      return type_strs.key(_type);
   }
   bool near_field() const {
      return _near_field;
   }
   bool far_field() const {
      return _far_field;
   }
   int sc() const {
      return _sc;
   }
   int body() const {
      return _body;
   }
   QVector3D position() const {
      return _position;
   }
   EulerAngles euler_angles() const {
      return _euler_angles;
   }
   QString boresight() const {
      return boresight_strs.key(_boresight);
   }
};

namespace Ui {
class FOV_Menu;
}

class FOV_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit FOV_Menu(QWidget *parent = nullptr);
   ~FOV_Menu();

   signals:
   void send_rgbavalues(QStringList);

   private slots:
   void set_validators();
   void receive_fovpath(QString);
   void receive_apppath(QString path);
   void receive_pythoncmd(QString cmd);
   void receive_data();
   void apply_data();
   void write_data(YAML::Node);

   void on_fov_remove_clicked();
   void on_fov_add_clicked();
   void on_fovlist_itemClicked(QListWidgetItem *item);
   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void on_pickcolor_clicked();

   void sides_changed();
   void dims_changed();
   void color_changed();
   void on_fov_name_textChanged(const QString &arg1);
   void field_changed();
   void scbody_changed();
   void pos_changed();
   void euler_changed();

   void on_boresightaxis_currentTextChanged(const QString &arg1);

   void on_fov_type_currentTextChanged(const QString &arg1);
   void clear_fields();

   void on_fov_duplicate_clicked();

   void on_sc_name_currentTextChanged(const QString &arg1);

   private:
   Ui::FOV_Menu *ui;

   QString appPath;
   QString pythonCmd;

   QHash<QListWidgetItem *, FOV> fov_list_hash = {};

   const int fovNLines = 11;
   QHash<QString, int> scNums;

   QString inout_path;
   QString file_path;

   const QHash<QString, QString> axis_inputs = {
       {"X_AXIS", "x-Axis"}, {"Y_AXIS", "y-Axis"}, {"Z_AXIS", "z-Axis"}};
   const QHash<QString, QString> fovtype_inputs = {{"WIREFRAME", "Wireframe"},
                                                   {"SOLID", "Solid"},
                                                   {"VECTOR", "Vector"},
                                                   {"PLANE", "Plane"}};
};

// Configure YAML conversions
namespace YAML {
// Color
template <> struct convert<Color> {
   static Node encode(const Color &rhs) {
      Node node(NodeType::Map);
      node["RGB"]   = rhs.rgb();
      node["Alpha"] = rhs.alpha();
      return node;
   }
   static bool decode(const Node &node, Color &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setRGB(node["RGB"].as<QVector3D>());
      rhs.setAlpha(node["Alpha"].as<double>());
      return true;
   }
};

// Sides
template <> struct convert<Sides> {
   static Node encode(const Sides &rhs) {
      Node node(NodeType::Map);
      node["Number"] = rhs.n();
      node["Length"] = rhs.length();
      return node;
   }
   static bool decode(const Node &node, Sides &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setN(node["Number"].as<int>());
      rhs.setLength(node["Length"].as<double>());
      return true;
   }
};

// FOV
template <> struct convert<FOV> {
   static Node encode(const FOV &rhs) {
      Node out_node(NodeType::Map);
      Node node(NodeType::Map);
      node["Label"]        = rhs.label();
      node["Sides"]        = rhs.sides();
      node["Width"]        = rhs.width();
      node["Height"]       = rhs.height();
      node["Color"]        = rhs.color();
      node["Type"]         = rhs.type();
      node["Near Field"]   = rhs.near_field();
      node["Far Field"]    = rhs.far_field();
      node["SC"]           = rhs.sc();
      node["Body"]         = rhs.body();
      node["Position"]     = rhs.position();
      node["Euler Angles"] = rhs.euler_angles();
      node["Boresight"]    = rhs.boresight();
      out_node["FOV"]      = node;
      return out_node;
   }
   static bool decode(const Node &in_node, FOV &rhs) {
      if (!in_node.IsMap())
         return false;

      const Node node = in_node["FOV"];
      if (!node.IsMap())
         return false;

      rhs.setLabel(node["Label"].as<QString>());
      rhs.setSides(node["Sides"].as<Sides>());
      rhs.setWidth(node["Width"].as<double>());
      rhs.setHeight(node["Height"].as<double>());
      rhs.setColor(node["Color"].as<Color>());
      rhs.setType(node["Type"].as<QString>());
      rhs.setNearField(node["Near Field"].as<bool>());
      rhs.setFarField(node["Far Field"].as<bool>());
      rhs.setSC(node["SC"].as<int>());
      rhs.setBody(node["Body"].as<int>());
      rhs.setPosition(node["Position"].as<QVector3D>());
      rhs.setEulerAngles(node["Euler Angles"].as<EulerAngles>());
      rhs.setBoresight(node["Boresight"].as<QString>());
      return true;
   }
};
} // namespace YAML

#endif // FOV_MENU_H
