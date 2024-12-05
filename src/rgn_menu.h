#ifndef RGN_MENU_H
#define RGN_MENU_H

#include "dsm_gui_lib.h"

#include <QDebug>
#include <QDialog>
#include <QListWidgetItem>

class Location {
   protected:
   enum types { LLA = 0, POSW };

   private:
   enum types _type    = LLA;
   QVector3D _position = {0, 0, 0};

   const static inline QMap<QString, enum types> type_strs = {{"LLA", LLA},
                                                              {"POSW", POSW}};

   public:
   Location(const QString type       = type_strs.key(LLA),
            const QVector3D position = {0, 0, 0}) {
      setType(type);
      setPosition(position);
   }

   void setType(const QString type) {
      _type = type_strs.value(type, LLA);
   }
   void setPosition(const QVector3D position) {
      _position = position;
   }

   QString type() const {
      return type_strs.key(_type);
   }
   QVector3D position() const {
      return _position;
   }
};
class Coefficients {
   private:
   double _elasticity = 0;
   double _damping    = 0;
   double _friction   = 0;

   public:
   Coefficients(const double elas = 0, const double damp = 0,
                const double fric = 0) {
      setElasticity(elas);
      setDamping(damp);
      setFriction(fric);
   }
   void setElasticity(const double elas) {
      _elasticity = std::max(elas, 0.0);
   }
   void setDamping(const double damp) {
      _damping = std::max(damp, 0.0);
   }
   void setFriction(const double fric) {
      _friction = std::max(fric, 0.0);
   }
   double elasticity() const {
      return _elasticity;
   }
   double damping() const {
      return _damping;
   }
   double friction() const {
      return _friction;
   }
};

class Region {
   private:
   bool _exists               = true;
   QString _name              = "REGION";
   QString _world             = "EARTH";
   Location _location         = Location();
   Coefficients _coefficients = Coefficients();
   QString _file_name         = "Rgn_REGION.obj";

   public:
   Region(QString name = "REGION", bool exists = true, QString world = "EARTH",
          Location location         = Location(),
          Coefficients coefficients = Coefficients(),
          QString file_name         = "Rgn_REGION.obj"

   ) {
      setExists(exists);
      setName(name);
      setWorld(world);
      setLocation(location);
      setCoefficients(coefficients);
      setFileName(file_name);
   }
   void setExists(const bool exists) {
      _exists = exists;
   }
   void setName(const QString name) {
      _name = name;
   }
   void setWorld(const QString world) {
      QString set_world = world;
      if (!(set_world.contains("MINORBODY") ||
            dsm_gui_lib::worldInputs.contains(set_world)))
         set_world = dsm_gui_lib::worldInputs[0];

      _world = set_world;
   }
   void setLocation(const Location location) {
      _location = location;
   }
   void setLocation(const QString loc_type) {
      _location.setType(loc_type);
   }
   void setLocation(const QVector3D pos) {
      _location.setPosition(pos);
   }
   void setCoefficients(const Coefficients coefficients) {
      _coefficients = coefficients;
   }
   void setFileName(const QString file_name) {
      _file_name = file_name;
   }
   bool exists() const {
      return _exists;
   }
   QString name() const {
      return _name;
   }
   QString world() const {
      return _world;
   }
   Location location() const {
      return _location;
   }
   Coefficients coefficients() const {
      return _coefficients;
   }
   QString file_name() const {
      return _file_name;
   }
};

namespace Ui {
class RGN_Menu;
}

class RGN_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit RGN_Menu(QWidget *parent = nullptr);
   ~RGN_Menu();

   private slots:
   void set_validators();
   void receive_rgnpath(QString);
   void apply_data();

   void on_rgn_remove_clicked();
   void on_rgn_add_clicked();
   void on_rgnlist_itemClicked(QListWidgetItem *item);
   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void clear_fields();

   void world_changed();
   void location_changed();
   void posw_changed();
   void lla_changed();
   void coeffs_changed();
   void geometry_changed();

   void on_exists_toggled(bool checked);

   void on_regionname_textChanged(const QString &arg1);

   void on_rgn_duplicate_clicked();

   void on_location_currentTextChanged(const QString &arg1);

   private:
   Ui::RGN_Menu *ui;

   QList<QLineEdit *> ui_posw;
   QList<QLineEdit *> ui_lla;

   const int rgnNLines = 9;
   QString inout_path;
   QString file_path;

   QHash<QListWidgetItem *, Region> rgn_list_hash = {};

   const QHash<QString, QString> location_inputs = {
       {"POSW", "Position in World"}, {"LLA", "Lat, Long, Alt"}};
};

// Configure YAML conversions
namespace YAML {
// Location
template <> struct convert<Location> {
   static Node encode(const Location &rhs) {
      Node node(NodeType::Map);
      node["Type"]     = rhs.type();
      node["Position"] = rhs.position();
      return node;
   }
   static bool decode(const Node &node, Location &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setType(node["Type"].as<QString>());
      rhs.setPosition(node["Position"].as<QVector3D>());
      return true;
   }
};

// Coefficients
template <> struct convert<Coefficients> {
   static Node encode(const Coefficients &rhs) {
      Node node(NodeType::Map);
      node["Elasticity"] = rhs.elasticity();
      node["Damping"]    = rhs.damping();
      node["Friction"]   = rhs.friction();
      return node;
   }
   static bool decode(const Node &node, Coefficients &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setElasticity(node["Elasticity"].as<double>());
      rhs.setDamping(node["Damping"].as<double>());
      rhs.setFriction(node["Friction"].as<double>());
      return true;
   }
};

// Region
template <> struct convert<Region> {
   static Node encode(const Region &rhs) {
      Node node(NodeType::Map);
      Node dat_node(NodeType::Map);

      node["Region"]                 = dat_node;
      dat_node["Exists"]             = rhs.exists();
      dat_node["Name"]               = rhs.name();
      dat_node["World"]              = rhs.world();
      dat_node["Location"]           = rhs.location();
      dat_node["Coefficients"]       = rhs.coefficients();
      dat_node["Geometry File Name"] = rhs.file_name();
      return node;
   }
   static bool decode(const Node &node, Region &rhs) {
      if (!node.IsMap())
         return false;
      const Node &dat_node = node["Region"];
      rhs.setExists(dat_node["Exists"].as<bool>());
      rhs.setName(dat_node["Name"].as<QString>());
      rhs.setWorld(dat_node["World"].as<QString>());
      rhs.setLocation(dat_node["Location"].as<Location>());
      rhs.setCoefficients(dat_node["Coefficients"].as<Coefficients>());
      rhs.setFileName(dat_node["Geometry File Name"].as<QString>());
      return true;
   }
};
} // namespace YAML

#endif // RGN_MENU_H
