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
class atmoConfig {
   private:
   QComboBox *methodBox;
   QLineEdit *f107Box;
   QLineEdit *apBox;

   public:
   atmoConfig(QLineEdit *f107 = nullptr, QLineEdit *ap = nullptr,
              QComboBox *method = nullptr) {
      f107Box   = f107;
      apBox     = ap;
      methodBox = method;
   }
   void setMethod(QString method) {
      dsm_gui_lib::setQComboBox(methodBox, method);
   }
   void setF107AP(QString f107, QString ap) {
      f107Box->setText(f107);
      apBox->setText(ap);
   }
   QString getMethod() {
      return methodBox->currentText();
   }
   QString getF107() {
      return f107Box->text();
   }
   QString getAp() {
      return apBox->text();
   }
};
class gravConfig {
   private:
   QSpinBox *degreeBox;
   QSpinBox *orderBox;

   public:
   gravConfig(QSpinBox *deg = nullptr, QSpinBox *ord = nullptr) {
      degreeBox = deg;
      orderBox  = ord;
   }
   void setDegreeOrder(int degree, int order) {
      degreeBox->setValue(degree);
      orderBox->setValue(order);
   }
   int getDegree() {
      return degreeBox->value();
   }
   int getOrder() {
      return orderBox->value();
   }
};
class magConfig : public gravConfig {
   private:
   QComboBox *methodBox;

   public:
   magConfig(QSpinBox *deg = nullptr, QSpinBox *ord = nullptr,
             QComboBox *meth = nullptr)
       : gravConfig(deg, ord) {
      methodBox = meth;
   }
   void setMethod(QString method) {
      dsm_gui_lib::setQComboBox(methodBox, method);
   }
   QString getMethod() {
      return methodBox->currentText();
   }
};
class worldConfig {
   private:
   atmoConfig *atmoConf;
   gravConfig *gravConf;
   magConfig *magConf;
   QCheckBox *enabConf;
   dsm_gui_lib::WorldID id;
   bool hasChildren;

   public:
   worldConfig(atmoConfig *atmo = nullptr, gravConfig *grav = nullptr,
               magConfig *mag = nullptr, QCheckBox *enabl = nullptr,
               dsm_gui_lib::WorldID i = dsm_gui_lib::WorldID::SOL,
               bool isPlan            = false) {
      atmoConf    = atmo;
      gravConf    = grav;
      magConf     = mag;
      enabConf    = enabl;
      id          = i;
      hasChildren = isPlan;
   }
   void setEnabled(bool enabled) {
      enabConf->setChecked(enabled);
   }
   void setAtmo(QString method, QString f107, QString ap) {
      atmoConf->setMethod(method);
      atmoConf->setF107AP(f107, ap);
   }
   void setGrav(int degree, int order) {
      gravConf->setDegreeOrder(degree, order);
   }
   void setMag(QString method, int degree, int order) {
      magConf->setMethod(method);
      magConf->setDegreeOrder(degree, order);
   }
   bool getEnabled() {
      return enabConf->isChecked();
   }
   bool hasAtmo() {
      return atmoConf != nullptr;
   }
   bool hasGrav() {
      return gravConf != nullptr;
   }
   bool hasMag() {
      return magConf != nullptr;
   }
   dsm_gui_lib::WorldID getID() {
      return id;
   }
   QString getAtmoMethod() {
      return atmoConf->getMethod();
   }
   QString getAtmoF107() {
      return atmoConf->getF107();
   }
   QString getAtmoAp() {
      return atmoConf->getAp();
   }
   QString getMagMethod() {
      return magConf->getMethod();
   }
   int getMagDegree() {
      return magConf->getDegree();
   }
   int getMagOrder() {
      return magConf->getOrder();
   }
   int getGravDegree() {
      return gravConf->getDegree();
   }
   int getGravOrder() {
      return gravConf->getOrder();
   }
};
class SIM_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit SIM_Menu(QWidget *parent = nullptr);
   ~SIM_Menu();

   private slots:
   void set_validators();
   void receive_simpath(QString path);
   void receive_data();
   void clear_data();
   void write_data(YAML::Node inp_sim);

   void on_loadDefaultButton_clicked();
   void on_saveDefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void setQComboBox(QComboBox *comboBox, QString string);

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

   const QHash<QString, QString> f107Inputs = {
       {"USER", "User"}, {"NOMINAL", "Nominal"}, {"TWOSIGMA", "2σ"}};

   const QHash<QString, QString> magfieldInputs = {
       {"NONE", "None"}, {"DIPOLE", "Dipole"}, {"IGRF", "IGRF"}};

   const QHash<QString, QString> ephemInputs = {
       {"MEAN", "Mean"}, {"DE430", "DE430"}, {"DE440", "DE440"}};

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

   QHash<QString, worldConfig> worldConfigHash;
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

#endif // SIM_MENU_H
