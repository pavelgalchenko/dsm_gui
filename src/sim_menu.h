#ifndef SIM_MENU_H
#define SIM_MENU_H

#include "dsm_gui_lib.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>

namespace Ui {
class SIM_Menu;
}

class atmoConfig;
class gravConfig;
class magConfig;
class worldConfig;
class GroundStation;
class orbitConfig;
class scConfig;

class SIM_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit SIM_Menu(QWidget *parent = nullptr);
   ~SIM_Menu();
   static const inline QHash<QString, QString> f107Inputs = {
       {"USER", "User"}, {"NOMINAL", "Nominal"}, {"TWOSIGMA", "2σ"}};
   static const inline QHash<QString, QString> magfieldInputs = {
       {"NONE", "None"}, {"DIPOLE", "Dipole"}, {"IGRF", "IGRF"}};
   static inline QHash<QListWidgetItem *, orbitConfig *> orbitHash = {};
   static inline QHash<QListWidgetItem *, scConfig *> scHash       = {};

   private slots:
   void set_validators();
   void receive_simpath(QString path);
   void receive_data();
   void clear_data();

   void on_loadDefaultButton_clicked();
   void on_saveDefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();
   GroundStation getGSData(const long i, const QListWidgetItem *const item);
   void setGSData(const GroundStation &, QListWidgetItem *const item);

   void UpdateModels();

   void on_simOrbList_itemClicked(QListWidgetItem *item);

   void on_simOrbitEn_toggled(bool checked);

   void on_simSCList_itemClicked(QListWidgetItem *item);

   void on_simSCEn_toggled(bool checked);

   void on_simSCOrbit_currentTextChanged(const QString &arg1);

   void on_simGSList_itemClicked(QListWidgetItem *item);

   void on_simGSEn_toggled(bool checked);

   void on_simGSWorld_currentTextChanged(const QString &arg1);

   void on_simGSLat_textEdited(const QString &arg1);

   void on_simGSLong_textEdited(const QString &arg1);

   void on_simGSLabel_textEdited(const QString &arg1);

   void on_simGSListRemove_clicked();

   void on_simGSListAdd_clicked();

   void on_simGSListDuplicate_clicked();

   void on_simF107Ap_currentTextChanged(const QString &arg1);

   void on_simMagfieldType_currentTextChanged(const QString &arg1);

   void on_simGSMinorBodyNum_textChanged(const QString &arg1);

   void on_simAeroPertShadow_toggled(bool checked);

   void on_simSRPPertShadow_toggled(bool checked);

   void on_simOrbList_currentItemChanged(QListWidgetItem *current,
                                         QListWidgetItem *previous);

   void on_simSCList_currentItemChanged(QListWidgetItem *current,
                                        QListWidgetItem *previous);

   private:
   Ui::SIM_Menu *ui;

   // do unique ptrs so they only last as long as the SIM_Menu object
   std::unique_ptr<QDoubleValidator> double_valid;

   QString inoutPath;
   QString filePath;

   QString orbDescription, scDescription, gsDescription;

   QString simFile;

   QList<QCheckBox *> celestialBodies;
   QList<QCheckBox *> lagrangeSystems;

   const QHash<QString, QString> timeModeInputs = {{"FAST", "Fast"},
                                                   {"REAL", "Real"},
                                                   {"EXTERNAL", "External"},
                                                   {"NOS3", "NOS3"}};

   const QHash<QString, QString> ephemInputs = {{"MEAN", "Mean"},
                                                {"DE430", "DE430"},
                                                {"DE440", "DE440"},
                                                {"SPICE", "Spice"}};

   enum class headerLineNames { // Doing this as a class so the values can be
                                // used as variables elsewhere
      HEADER,
      CONTROL,
      ORBITS,
      SPACECRAFT,
      ENVIRONMENT,
      BODIES,
      LAGRANGE,
      GROUND
   };

   // Here is the only place that we need to change text if inp sim headers
   // change
   static QString toString(const headerLineNames h) {
      switch (h) {
         case headerLineNames::HEADER:
            return "HEADER";
         case headerLineNames::CONTROL:
            return "CONTROL";
         case headerLineNames::ORBITS:
            return "ORBITS";
         case headerLineNames::SPACECRAFT:
            return "SPACECRAFT";
         case headerLineNames::ENVIRONMENT:
            return "ENVIRONMENT";
         case headerLineNames::BODIES:
            return "BODIES";
         case headerLineNames::LAGRANGE:
            return "LAGRANGE";
         case headerLineNames::GROUND:
            return "GROUND";
      }
   }

   enum orbListData { orbEnabledRole = Qt::UserRole, orbNumberRole };

   enum scListData { scEnabledRole = Qt::UserRole, scOrbNameRole };

   enum gsListData {
      gsEnabledRole = Qt::UserRole,
      gsWorldRole,
      gsLongRole,
      gsLatRole
   };

   QHash<QString, QString> orbFileHash;
   QHash<QString, QString> scFileHash;

   QHash<QString, worldConfig *> worldConfigHash;
   QHash<QString, QString> worldConfNames = {
       {"MERCURY", "Mercury"},
       {"VENUS", "Venus"},
       {"EARTH", "Earth and Luna"},
       {"MARS", "Mars and its moons"},
       {"JUPITER", "Jupiter and its moons"},
       {"SATURN", "Saturn and its moons"},
       {"URANUS", "Uranus and its moons"},
       {"NEPTUNE", "Neptune and its moons"},
       {"PLUTO", "Pluto and its moons"},
       {"MINORBODY", "Asteroids and Comets"},
   };
};

class atmoConfig {
   private:
   QComboBox *methodBox = nullptr;
   QLineEdit *f107Box   = nullptr;
   QLineEdit *apBox     = nullptr;
   QString _method;
   double _f107;
   double _ap;
   QString _world;
   bool _enabled = false;

   protected:
   void updateMethod() {
      if (methodBox != nullptr)
         _method = SIM_Menu::f107Inputs.key(methodBox->currentText());
   }
   void updateF107() {
      if (f107Box != nullptr)
         _f107 = f107Box->text().toDouble();
   }
   void updateAp() {
      if (apBox != nullptr)
         _ap = apBox->text().toDouble();
   }

   public:
   YAML::Node other_data = YAML::Node(YAML::NodeType::Null);
   atmoConfig(const QString world        = "DEFAULT",
              QLineEdit *const f107_it   = nullptr,
              QLineEdit *const ap_it     = nullptr,
              QComboBox *const method_it = nullptr) {
      _world  = world;
      f107Box = f107_it;
      if (f107_it != nullptr)
         _f107 = f107_it->text().toDouble();
      apBox = ap_it;
      if (ap_it != nullptr)
         _ap = ap_it->text().toDouble();
      methodBox = method_it;
      if (method_it != nullptr)
         _method = SIM_Menu::f107Inputs.key(method_it->currentText());
      if (f107Box != nullptr && apBox != nullptr && methodBox != nullptr)
         _enabled = true;
   }
   atmoConfig &operator=(const atmoConfig &other) {
      if (!_enabled && other.isEnabled()) {
         methodBox = other.methodBox;
         f107Box   = other.f107Box;
         apBox     = other.apBox;
         _enabled  = true;
      }
      _world = other._world;
      setMethod(other._method);
      setF107AP(other._f107, other._ap);
      if (!other.other_data.IsNull())
         other_data = other.other_data;
      return *this;
   }
   atmoConfig(const atmoConfig &other) {
      *this = other;
   }
   void Update() {
      updateMethod();
      updateF107();
      updateAp();
   }
   void setMethod(const QString method) {
      _method = method;
      if (methodBox != nullptr)
         dsm_gui_lib::setQComboBox(methodBox,
                                   SIM_Menu::f107Inputs.value(method));
   }
   void setF107AP(const double f107, const double ap) {
      _f107 = f107;
      _ap   = ap;
      if (f107Box != nullptr)
         f107Box->setText(QString::number(f107));
      if (apBox != nullptr)
         apBox->setText(QString::number(ap));
   }
   void setWorld(const QString world) {
      if (dsm_gui_lib::worldInputs.contains(world))
         _world = world;
   }
   QString getMethod() const {
      return _method;
   }
   double getF107() const {
      return _f107;
   }
   double getAp() const {
      return _ap;
   }
   bool isEnabled() const {
      return _enabled;
   }
   QString getWorld() const {
      return _world;
   }
};
class gravConfig {
   private:
   QString _world;
   QSpinBox *degreeBox = nullptr;
   QSpinBox *orderBox  = nullptr;
   int _degree;
   int _order;

   protected:
   bool _enabled = false;
   void updateDegree() {
      if (degreeBox != nullptr) {
         _degree = degreeBox->value();
         orderBox->setMaximum(_degree);
      }
   }
   void updateOrder() {
      if (orderBox != nullptr)
         _order = orderBox->value();
   }
   void setEnabled(const bool en) {
      _enabled = en;
   }

   public:
   YAML::Node other_data = YAML::Node(YAML::NodeType::Null);
   gravConfig(const QString world = "DEFAULT", QSpinBox *const deg_it = nullptr,
              QSpinBox *const ord_it = nullptr) {
      _world    = world;
      degreeBox = deg_it;
      if (deg_it != nullptr)
         _degree = deg_it->text().toInt();
      orderBox = ord_it;
      if (ord_it != nullptr)
         _order = ord_it->text().toInt();
      if (degreeBox != nullptr && orderBox != nullptr)
         _enabled = true;
   }
   gravConfig &operator=(const gravConfig &other) {
      if (!_enabled && other._enabled) {
         degreeBox = other.degreeBox;
         orderBox  = other.orderBox;
         _enabled  = true;
      }
      _world = other._world;
      setDegreeOrder(other._degree, other._order);
      if (!other.other_data.IsNull())
         other_data = other.other_data;
      return *this;
   }
   gravConfig(const gravConfig &other) {
      *this = other;
   }
   void Update() {
      updateDegree();
      updateOrder();
   }
   void setDegreeOrder(const int degree, const int order) {
      _degree = degree;
      _order  = order;
      if (degreeBox != nullptr)
         degreeBox->setValue(degree);
      if (orderBox != nullptr)
         orderBox->setValue(order);
   }
   int getDegree() const {
      return _degree;
   }
   int getOrder() const {
      return _order;
   }
   QString getWorld() const {
      return _world;
   }
   void setWorld(const QString world) {
      if (dsm_gui_lib::worldInputs.contains(world))
         _world = world;
   }
   bool isEnabled() const {
      return _enabled;
   }
};
class magConfig : public gravConfig {
   private:
   QString _method;
   QComboBox *methodBox = nullptr;

   protected:
   void updateMethod() {
      if (methodBox != nullptr)
         _method = SIM_Menu::magfieldInputs.key(methodBox->currentText());
   }

   public:
   YAML::Node other_data = YAML::Node(YAML::NodeType::Null);
   magConfig(const QString world = "DEFAULT", QSpinBox *const deg = nullptr,
             QSpinBox *const ord        = nullptr,
             QComboBox *const method_it = nullptr)
       : gravConfig(world, deg, ord) {
      methodBox = method_it;
      if (method_it != nullptr) {
         _method = SIM_Menu::magfieldInputs.key(methodBox->currentText());
      }
   }
   magConfig &operator=(const magConfig &other) {
      gravConfig::operator=(other);
      if (methodBox == nullptr) {
         methodBox = other.methodBox;
         if (other.methodBox == nullptr) {
            setEnabled(false);
         }
      }
      setMethod(other._method);
      if (!other.other_data.IsNull())
         other_data = other.other_data;
      return *this;
   }
   magConfig(const magConfig &other) {
      *this = other;
   }
   void Update() {
      gravConfig::Update();
      updateMethod();
   }
   void setMethod(const QString method) {
      _method = method;
      if (methodBox != nullptr)
         dsm_gui_lib::setQComboBox(methodBox,
                                   SIM_Menu::magfieldInputs.value(method));
   }
   QString getMethod() const {
      return _method;
   }
};
class worldConfig {
   private:
   atmoConfig atmoConf;
   gravConfig gravConf;
   magConfig magConf;
   QCheckBox *enabConf;
   dsm_gui_lib::WorldID id;
   bool _hasChildren = false; // TODO: does nothing

   public:
   worldConfig(const dsm_gui_lib::WorldID i = dsm_gui_lib::WorldID::SOL,
               QCheckBox *const enabl = nullptr, const bool isPlan = false,
               const gravConfig &grav = gravConfig(),
               const atmoConfig &atmo = atmoConfig(),
               const magConfig &mag   = magConfig()) {
      id                       = i;
      const QString world_name = dsm_gui_lib::ID2World(i);
      if (!grav.getWorld().compare("DEFAULT")) {
         gravConf = gravConfig(world_name);
      } else {
         gravConf = grav;
      }
      if (!atmo.getWorld().compare("DEFAULT")) {
         atmoConf = atmoConfig(world_name);
      } else {
         atmoConf = atmo;
      }
      if (!mag.getWorld().compare("DEFAULT")) {
         magConf = magConfig(world_name);
      } else {
         magConf = mag;
      }
      enabConf     = enabl;
      _hasChildren = isPlan;
   }
   void UpdateAll() {
      if (hasAtmo())
         atmoConf.Update();
      if (hasGrav())
         gravConf.Update();
      if (hasAtmo())
         magConf.Update();
   }
   void setHasChildren(const bool hasChildren) {
      _hasChildren = hasChildren;
   }
   void setEnable(QCheckBox *const enabl) {
      enabConf = enabl;
   }
   void setEnabled(const bool enabled) {
      enabConf->setChecked(enabled);
   }
   template <typename T> void set(const T item) {
      if constexpr (std::is_same_v<T, atmoConfig>) {
         atmoConf = item;
      } else if constexpr (std::is_same_v<T, gravConfig>) {
         gravConf = item;
      } else if constexpr (std::is_same_v<T, magConfig>) {
         magConf = item;
      }
   }
   void setAtmo(const QString method, const double f107, const double ap) {
      atmoConf.setMethod(method);
      atmoConf.setF107AP(f107, ap);
   }
   void setGrav(const int degree, const int order) {
      gravConf.setDegreeOrder(degree, order);
   }
   void setMag(const QString method, const int degree, const int order) {
      magConf.setMethod(method);
      if (!method.compare("IGRF"))
         magConf.setDegreeOrder(degree, order);
   }
   bool getEnabled() const {
      return enabConf->isChecked();
   }
   bool hasAtmo() const {
      return atmoConf.isEnabled();
   }
   bool hasGrav() const {
      return gravConf.isEnabled();
   }
   bool hasMag() const {
      return magConf.isEnabled();
   }
   dsm_gui_lib::WorldID getID() const {
      return id;
   }
   QString getAtmoMethod() const {
      return atmoConf.getMethod();
   }
   double getAtmoF107() const {
      return atmoConf.getF107();
   }
   double getAtmoAp() const {
      return atmoConf.getAp();
   }
   QString getMagMethod() const {
      return magConf.getMethod();
   }
   QString getName() const {
      return dsm_gui_lib::ID2World(id);
   }
   int getMagDegree() const {
      return magConf.getDegree();
   }
   int getMagOrder() const {
      return magConf.getOrder();
   }
   int getGravDegree() const {
      return gravConf.getDegree();
   }
   int getGravOrder() const {
      return gravConf.getOrder();
   }
   atmoConfig const &getAtmo() const {
      return atmoConf;
   }
   gravConfig const &getGrav() const {
      return gravConf;
   }
   magConfig const &getMag() const {
      return magConf;
   }
};

class GroundStation {
   private:
   long _index       = 0;
   bool _enabled     = false;
   QString _world    = "EARTH";
   double _longitude = 0.0;
   double _latitude  = 0.0;
   QString _label    = "";

   public:
   GroundStation(const long index = 0, const bool enabled = false,
                 const QString world = "EARTH", const double lng = 0.0,
                 const double lat = 0.0, const QString label = "") {
      setIndex(index);
      setEnabled(enabled);
      setWorld(world);
      setLongitude(lng);
      setLatitude(lat);
      setLabel(label);
   }
   void setIndex(const long index) {
      _index = std::max(index, 0l);
   }
   void setEnabled(const bool enabled) {
      _enabled = enabled;
   }
   void setWorld(const QString world) {
      if (dsm_gui_lib::worldInputs.contains(world))
         _world = world;
   }
   void setLongitude(const double lng) {
      _longitude = lng;
   }
   void setLatitude(const double lat) {
      _latitude = lat;
   }
   void setLabel(const QString label) {
      _label = label;
   }
   long index() const {
      return _index;
   }
   bool enabled() const {
      return _enabled;
   }
   QString world() const {
      return _world;
   }
   double longitude() const {
      return _longitude;
   }
   double latitude() const {
      return _latitude;
   }
   QString label() const {
      return _label;
   }
};

class orbitConfig {
   private:
   QString _name = "DEFAULT";
   bool _enabled = false;

   public:
   orbitConfig(const QString name = "DEFAULT", const bool enabled = false) {
      setName(name);
      setEnabled(enabled);
   }
   void setName(const QString name) {
      _name = name;
   }
   void setEnabled(const bool enabled) {
      _enabled = enabled;
   }
   QString name() const {
      return _name;
   }
   bool enabled() const {
      return _enabled;
   }
};
class scConfig {
   private:
   QString _name       = "DEFAULT";
   bool _enabled       = false;
   orbitConfig *_orbit = nullptr;

   public:
   scConfig(const QString name = "DEFAULT", const bool enabled = false,
            orbitConfig *orbit = nullptr) {
      setName(name);
      setEnabled(enabled);
      setOrbit(orbit);
   }
   void setName(const QString name) {
      _name = name;
   }
   void setEnabled(const bool enabled) {
      _enabled = enabled;
   }
   void setOrbit(orbitConfig &orbit) {
      _orbit = &orbit;
   }
   void setOrbit(orbitConfig *orbit) {
      _orbit = orbit;
   }
   void setOrbit(const QString orbit_name) {
      _orbit =
          dsm_gui_lib::getObjectFromItemName(orbit_name, SIM_Menu::orbitHash);
   }
   QString name() const {
      return _name;
   }
   bool enabled() const {
      return _enabled;
   }
   orbitConfig *orbit() const {
      return _orbit;
   }
};

// Configure YAML conversions
namespace YAML {
// atmoConfig
template <> struct convert<atmoConfig> {
   static Node encode(const atmoConfig &rhs) {
      Node node(NodeType::Map);
      const QString method = rhs.getMethod();
      node["World"]        = rhs.getWorld();
      node["Method"]       = method;
      if (!method.compare("USER")) {
         node["F10.7"] = rhs.getF107();
         node["Ap"]    = rhs.getAp();
      }
      for (auto it = rhs.other_data.begin(); it != rhs.other_data.end(); ++it) {
         node[it->first] = it->second;
      }
      return node;
   }
   static bool decode(const Node &node, atmoConfig &rhs) {
      if ((node.IsNull() ^ rhs.isEnabled()) || !node.IsMap())
         return false;
      rhs.setWorld(node["World"].as<QString>());
      const QString method = node["Method"].as<QString>();
      rhs.setMethod(method);
      if (!method.compare("USER"))
         rhs.setF107AP(node["F10.7"].as<double>(), node["Ap"].as<double>());

      Node other_data(NodeType::Map);
      const QStringList not_other_data = {"World", "Method", "USER", "F10.7",
                                          "Ap"};
      for (auto it = node.begin(); it != node.end(); ++it) {
         const QString node_name = it->first.as<QString>();
         if (!not_other_data.contains(node_name)) {
            other_data[node_name] = it->second;
         }
      }
      rhs.other_data = other_data;
      return true;
   }
};

// gravConfig
template <> struct convert<gravConfig> {
   static Node encode(const gravConfig &rhs) {
      Node node(NodeType::Map);
      node["World"]  = rhs.getWorld();
      node["Degree"] = rhs.getDegree();
      node["Order"]  = rhs.getOrder();
      for (auto it = rhs.other_data.begin(); it != rhs.other_data.end(); ++it) {
         node[it->first] = it->second;
      }
      return node;
   }
   static bool decode(const Node &node, gravConfig &rhs) {
      if ((node.IsNull() ^ rhs.isEnabled()) || !node.IsMap())
         return false;
      rhs.setWorld(node["World"].as<QString>());
      rhs.setDegreeOrder(node["Degree"].as<int>(), node["Order"].as<int>());

      Node other_data(NodeType::Map);
      const QStringList not_other_data = {"World", "Degree", "Order"};
      for (auto it = node.begin(); it != node.end(); ++it) {
         const QString node_name = it->first.as<QString>();
         if (!not_other_data.contains(node_name)) {
            other_data[node_name] = it->second;
         }
      }
      rhs.other_data = other_data;
      return true;
   }
};

// magConfig
template <> struct convert<magConfig> {
   static Node encode(const magConfig &rhs) {
      Node node(NodeType::Map);
      node["World"]        = rhs.getWorld();
      const QString method = rhs.getMethod();
      node["Method"]       = method;
      if (!method.compare("IGRF")) {
         node["Degree"] = rhs.getDegree();
         node["Order"]  = rhs.getOrder();
      }
      for (auto it = rhs.other_data.begin(); it != rhs.other_data.end(); ++it) {
         node[it->first] = it->second;
      }
      return node;
   }
   static bool decode(const Node &node, magConfig &rhs) {
      if ((node.IsNull() ^ rhs.isEnabled()) || !node.IsMap())
         return false;
      rhs.setWorld(node["World"].as<QString>());
      const QString method = node["Method"].as<QString>();
      rhs.setMethod(method);
      if (!method.compare("IGRF"))
         rhs.setDegreeOrder(node["Degree"].as<int>(), node["Order"].as<int>());

      Node other_data(NodeType::Map);
      const QStringList not_other_data = {"World", "Degree", "Order", "Method"};
      for (auto it = node.begin(); it != node.end(); ++it) {
         const QString node_name = it->first.as<QString>();
         if (!not_other_data.contains(node_name)) {
            other_data[node_name] = it->second;
         }
      }
      rhs.other_data = other_data;
      return true;
   }
};

// GroundStation
template <> struct convert<GroundStation> {
   static Node encode(const GroundStation &rhs) {
      Node node(NodeType::Map);
      Node dat_node(NodeType::Map);

      dat_node["Index"]      = rhs.index();
      dat_node["Enabled"]    = rhs.enabled();
      dat_node["World"]      = rhs.world();
      dat_node["Longitude"]  = rhs.longitude();
      dat_node["Latitude"]   = rhs.latitude();
      dat_node["Label"]      = rhs.label();
      node["Ground Station"] = dat_node;
      return node;
   }
   static bool decode(const Node &node, GroundStation &rhs) {
      if (!node.IsMap())
         return false;
      const Node &dat_node = node["Ground Station"];
      if (!node.IsMap())
         return false;
      rhs.setIndex(dat_node["Index"].as<long>());
      rhs.setEnabled(dat_node["Enabled"].as<bool>());
      rhs.setWorld(dat_node["World"].as<QString>());
      rhs.setLongitude(dat_node["Longitude"].as<double>());
      rhs.setLatitude(dat_node["Latitude"].as<double>());
      rhs.setLabel(dat_node["Label"].as<QString>());
      return true;
   }
};

// orbitConfig
template <> struct convert<orbitConfig> {
   static Node encode(const orbitConfig &rhs) {
      Node node(NodeType::Map);
      node["Name"]    = "Orb_" + rhs.name();
      node["Enabled"] = rhs.enabled();
      return node;
   }
   static bool decode(const Node &node, orbitConfig &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setName(node["Name"].as<QString>().mid(4));
      rhs.setEnabled(node["Enabled"].as<bool>());
      return true;
   }
};

// scConfig
template <> struct convert<scConfig> {
   static Node encode(const scConfig &rhs) {
      Node node(NodeType::Map);
      node["Name"]    = "SC_" + rhs.name();
      node["Orbit"]   = "Orb_" + rhs.orbit()->name();
      node["Enabled"] = rhs.enabled();
      return node;
   }
   static bool decode(const Node &node, scConfig &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setName(node["Name"].as<QString>().mid(3));
      rhs.setEnabled(node["Enabled"].as<bool>());
      rhs.setOrbit(node["Orbit"].as<QString>().mid(4));
      return true;
   }
};
} // namespace YAML
#endif // SIM_MENU_H
