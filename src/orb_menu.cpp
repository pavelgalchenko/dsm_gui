#include "orb_menu.h"
#include "ui_orb_menu.h"

ORB_Menu::ORB_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::ORB_Menu) {
   ui->setupUi(this);

   double_valid    = std::make_unique<QDoubleValidator>();
   ninf_pinf_valid = std::make_unique<QDoubleValidator>(-INFINITY, INFINITY, 5);
   zero_pinf_valid = std::make_unique<QDoubleValidator>(0, INFINITY, 5);
   set_validators();
}

ORB_Menu::~ORB_Menu() {
   delete ui;
}

void ORB_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");
   QRegularExpression rx1("[^\" ]*");

   no_quotes_valid        = std::make_unique<QRegularExpressionValidator>(rx);
   no_quotes_spaces_valid = std::make_unique<QRegularExpressionValidator>(rx1);

   // Numerical Validators
   // All validators that can be any double
   QList<QLineEdit *> double_line_edit = {
       ui->orbFormFrameEuler_1, ui->orbFormFrameEuler_2,
       ui->orbFormFrameEuler_3, ui->orbFormOriginPos_1,
       ui->orbFormOriginPos_2,  ui->orbFormOriginPos_3,
       ui->orbCentKepMinAlt,    ui->orbCentKepInc,
       ui->orbCentKepRAAN,      ui->orbCentKepTA,
       ui->orbCentPVPos_1,      ui->orbCentPVPos_2,
       ui->orbCentPVPos_3,      ui->orbCentPVVel_1,
       ui->orbCentPVVel_2,      ui->orbCentPVVel_3,
       ui->orbTBodyModeXYPhase, ui->orbTBodyModeXYPhase_2,
       ui->orbTBodyModeZPhase,  ui->orbTBodyCowellPos_1,
       ui->orbTBodyCowellPos_2, ui->orbTBodyCowellPos_3,
       ui->orbTBodyCowellVel_1, ui->orbTBodyCowellVel_2,
       ui->orbTBodyCowellVel_3};

   // All validators with (-INFINITY, INFINITY)
   QList<QLineEdit *> ninf_pinf = {ui->orbCentKepPeriAlt, ui->orbCentKepApoAlt};

   // All validators with [0, INFINITY)
   QList<QLineEdit *> zero_pinf = {
       ui->orbTBodyModeXYSMA,
       ui->orbTBodyModeXYSMA_2,
       ui->orbTBodyModeZSMA,
   };

   dsm_gui_lib::set_mult_validators(double_line_edit, double_valid.get());
   dsm_gui_lib::set_mult_validators(ninf_pinf, ninf_pinf_valid.get());
   dsm_gui_lib::set_mult_validators(zero_pinf, zero_pinf_valid.get());

   // Name Validators
   QList<QLineEdit *> item_names = {ui->orbDescription, ui->orbCentFileLabel,
                                    ui->orbTBodyFileLabel};

   dsm_gui_lib::set_mult_validators(item_names, no_quotes_valid.get());

   ui->orbLabel->setValidator(no_quotes_spaces_valid.get());

   ui->orbType->addItems(dsm_gui_lib::sortStringList(orbTypeInputs.values()));
   ui->orbFormFrame->addItems(orbFrameInputs);
   ui->orbFormOrigin->addItems(orbFrameInputs);
   ui->orbFormFrameEulerSeq->addItems(dsm_gui_lib::eulerInputs);

   ui->orbZeroWorld->addItems(dsm_gui_lib::worldInputs);
   ui->orbZeroWorld->setMaxVisibleItems(10);

   ui->orbCentWorld->addItems(dsm_gui_lib::worldInputs);
   ui->orbCentWorld->setMaxVisibleItems(10);
   ui->orbCentICParam->addItems(
       dsm_gui_lib::sortStringList(orbCentICTypeInputs.values()));
   ui->orbCentFileType->addItems(
       dsm_gui_lib::sortStringList(orbFileTypeInputs.values()));

   ui->orbTBodyLSystem->addItems(
       dsm_gui_lib::sortStringList(orbTBodyLSysInputs.values()));
   ui->orbTBodyProp->addItems(
       dsm_gui_lib::sortStringList(orbTBodyPropInputs.values()));
   ui->orbTBodyICParam->addItems(
       dsm_gui_lib::sortStringList(orbTBodyICTypeInputs.values()));
   ui->orbTBodyLPoint->addItems(lagrangePointInputs);
   ui->orbTBodyFileType->addItems(
       dsm_gui_lib::sortStringList(orbFileTypeInputs.values()));

   ui->orbZeroPolyGrav->setId(ui->orbZeroPolyGrav_on, 1);
   ui->orbZeroPolyGrav->setId(ui->orbZeroPolyGrav_off, 0);
   ui->orbFlightPolyGrav->setId(ui->orbFlightPolyGrav_on, 1);
   ui->orbFlightPolyGrav->setId(ui->orbFlightPolyGrav_off, 0);
   ui->orbCentJ2->setId(ui->orbCentJ2_on, 1);
   ui->orbCentJ2->setId(ui->orbCentJ2_off, 0);
   ui->orbCentPA->setId(ui->orbCentPA_on, 1);
   ui->orbCentPA->setId(ui->orbCentPA_off, 0);
   ui->orbTBodyModeSense->setId(ui->orbTBodyModeSense_CW, 1);
   ui->orbTBodyModeSense->setId(ui->orbTBodyModeSense_CCW, 0);
   ui->orbTBodyModeSense_2->setId(ui->orbTBodyModeSense_CW_2, 1);
   ui->orbTBodyModeSense_2->setId(ui->orbTBodyModeSense_CCW_2, 0);

   ui->orbList->setSortingEnabled(true);
   ui->orbList->sortItems(Qt::AscendingOrder);

   QList<QWidget *> init_disabled_items = {
       ui->orbCentKepPALabel, ui->orbCentKepPeriAlt, ui->orbCentKepApoAlt,
       ui->orbCentKepAELabel, ui->orbCentKepMinAlt,  ui->orbCentKepEcc};
   for (auto item : init_disabled_items) {
      item->setEnabled(false);
   }
   ui->orbCentPA_on->setChecked(true);

   ui->orbCentKepPAWarning->setVisible(false);
   connect(ui->orbCentKepPeriAlt, &QLineEdit::textChanged, this,
           &ORB_Menu::checkKepPA);
   connect(ui->orbCentKepApoAlt, &QLineEdit::textChanged, this,
           &ORB_Menu::checkKepPA);
   connect(ui->orbCentICParam, &QComboBox::textActivated, this,
           &ORB_Menu::checkKepPA);
   connect(ui->orbCentPA, &QButtonGroup::idToggled, this,
           &ORB_Menu::checkKepPA);
   connect(ui->orbList, &QListWidget::currentRowChanged, this,
           &ORB_Menu::clear_data);

   connect(ui->orbListAdd, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
   connect(ui->orbListRemove, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
   connect(ui->orbListDuplicate, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
   connect(ui->loadDefaultButton, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
}

void ORB_Menu::receive_orbpath(QString path) {
   inout_path           = path;
   QStringList orbFiles = QDir(inout_path).entryList({"Orb_*.yaml"});

   orbFileHash.clear();
   ui->orbList->clear();

   for (int i = 0; i < orbFiles.length(); i++)
      orbFileHash.insert(orbFiles[i].chopped(5).mid(4),
                         inout_path + orbFiles[i].remove("\""));

   ui->orbList->addItems(orbFileHash.keys());
}

void ORB_Menu::receive_data(QString file_path) {
   clear_data();
   ui->orbLabel->setText(ui->orbList->currentItem()->text());

   YAML::Node orb_file_yaml = YAML::LoadFile(file_path.toStdString());
   YAML::Node orb_yaml      = orb_file_yaml["Orbit"];

   const QString orb_type_str    = orb_yaml["Type"].as<QString>();
   const enum orb_types orb_type = orbTypeHash[orb_type_str];
   setQComboBox(ui->orbType, orbTypeInputs.value(orb_type_str));
   ui->orbDescription->setText(
       orb_file_yaml["Configuration"]["Description"].as<QString>());

   switch (orb_type) {
      case ORB_ZERO: {
         const QString world = orb_yaml["World"].as<QString>();
         if (world.contains("MINORBODY")) {
            dsm_gui_lib::setQComboBox(ui->orbZeroWorld, "MINORBODY");
            ui->orbZeroMinorBodyNum->setValue(world.rightRef(1).toInt());
         } else {
            dsm_gui_lib::setQComboBox(ui->orbZeroWorld, world);
            ui->orbZeroMinorBodyNum->setValue(0);
         }
         const bool poly_grav = orb_yaml["Polyhedron Grav"].as<bool>();
         ui->orbZeroPolyGrav->button(poly_grav)->setChecked(true);
      } break;
      case ORB_FLIGHT: {
         ui->orbFlightRegion->setValue(orb_yaml["Region"].as<int>());
         const bool poly_grav = orb_yaml["Polyhedron Grav"].as<bool>();
         ui->orbZeroPolyGrav->button(poly_grav)->setChecked(true);
      } break;
      case ORB_CENTRAL: {
         const QString world = orb_yaml["World"].as<QString>();
         if (world.contains("MINORBODY")) {
            dsm_gui_lib::setQComboBox(ui->orbCentWorld, "MINORBODY");
            ui->orbCentMinorBodyNum->setValue(world.rightRef(1).toInt());
         } else {
            dsm_gui_lib::setQComboBox(ui->orbCentWorld, world);
            ui->orbCentMinorBodyNum->setValue(0);
         }
         const bool j2_enabled = orb_yaml["J2 Secular Drift"].as<bool>();
         ui->orbCentJ2->button(j2_enabled)->setChecked(true);
         YAML::Node init_yaml   = orb_yaml["Init"];
         const QString init_str = init_yaml["Method"].as<QString>();
         setQComboBox(ui->orbCentICParam, orbCentICTypeInputs.value(init_str));
         const enum cent_init_types init_type = orbCentICTypeHash[init_str];
         switch (init_type) {
            case CENT_KEP: {
               const QString pa_type =
                   init_yaml["SMA Parameterization"].as<QString>();
               const bool is_pa = !pa_type.compare("PA");
               ui->orbCentPA->button(is_pa)->setChecked(true);
               if (is_pa) {
                  ui->orbCentKepPeriAlt->setText(
                      init_yaml["Periapsis"].as<QString>());
                  ui->orbCentKepApoAlt->setText(
                      init_yaml["Apoapsis"].as<QString>());
                  ui->orbCentKepPeriAlt->setValidator(new QDoubleValidator(
                      -INFINITY, ui->orbCentKepApoAlt->text().toDouble(), 5,
                      this));
                  ui->orbCentKepApoAlt->setValidator(new QDoubleValidator(
                      ui->orbCentKepPeriAlt->text().toDouble(), INFINITY, 5,
                      this));
               } else {
                  ui->orbCentKepMinAlt->setText(
                      init_yaml["Minimum Altitude"].as<QString>());
                  ui->orbCentKepEcc->setValue(
                      init_yaml["Eccentricity"].as<double>());
               }

               ui->orbCentKepInc->setText(
                   init_yaml["Inclination"].as<QString>());
               ui->orbCentKepRAAN->setText(init_yaml["RAAN"].as<QString>());
               ui->orbCentKepArgPeri->setText(
                   init_yaml["Arg of Periapsis"].as<QString>());
               ui->orbCentKepTA->setText(
                   init_yaml["True Anomaly"].as<QString>());
            } break;
            case CENT_RV: {
               const QList<QLineEdit *> position_line = {
                   ui->orbCentPVPos_1, ui->orbCentPVPos_2, ui->orbCentPVPos_3};
               const QList<QLineEdit *> velocity_line = {
                   ui->orbCentPVVel_1, ui->orbCentPVVel_2, ui->orbCentPVVel_3};

               const QList<QString> pos_strings =
                   init_yaml["Position"].as<QList<QString>>();
               const QList<QString> vel_strings =
                   init_yaml["Velocity"].as<QList<QString>>();

               for (int i = 0; i < 3; i++) {
                  position_line[i]->setText(pos_strings[i]);
                  velocity_line[i]->setText(vel_strings[i]);
               }
            } break;
            case CENT_FILE: {
               const QString file_type = init_yaml["File Type"].as<QString>();
               setQComboBox(ui->orbCentFileType,
                            orbFileTypeInputs.value(file_type));
               ui->orbCentFileName->setText(
                   init_yaml["File Name"].as<QString>());
               ui->orbCentFileLabel->setText(
                   init_yaml["Label in File"].as<QString>());
            } break;
         }
      } break;
      case ORB_THREE_BODY: {
         const QString lag_sys = orb_yaml["Lagrange System"].as<QString>();
         setQComboBox(ui->orbTBodyLSystem, orbTBodyLSysInputs.value(lag_sys));
         const QString prop_meth = orb_yaml["Propagation Method"].as<QString>();
         setQComboBox(ui->orbTBodyProp, orbTBodyPropInputs.value(prop_meth));

         YAML::Node init_yaml   = orb_yaml["Init"];
         const QString init_str = init_yaml["Method"].as<QString>();
         setQComboBox(ui->orbTBodyICParam,
                      orbTBodyICTypeInputs.value(init_str));
         const enum tbp_init_types init_type = orbTBodyICTypeHash[init_str];
         switch (init_type) {
            case TBP_MODES: {
               const QString l_point =
                   init_yaml["Lagrange Point"].as<QString>();
               setQComboBox(ui->orbTBodyLPoint, l_point);
               ui->orbTBodyModeXYSMA->setText(
                   init_yaml["XY SMA"].as<QString>());
               ui->orbTBodyModeXYPhase->setText(
                   init_yaml["XY Phase"].as<QString>());
               ui->orbTBodyModeSense
                   ->button(!init_yaml["Sense"].as<QString>().compare("CW"))
                   ->setChecked(true);
               ui->orbTBodyModeZSMA->setText(init_yaml["Z SMA"].as<QString>());
               ui->orbTBodyModeZPhase->setText(
                   init_yaml["Z Phase"].as<QString>());
               if (!l_point.compare("L4") || !l_point.compare("L5")) {
                  ui->orbTBodyModeXYSMA_2->setText(
                      init_yaml["XY 2nd SMA"].as<QString>());
                  ui->orbTBodyModeXYPhase_2->setText(
                      init_yaml["XY 2nd Phase"].as<QString>());
                  ui->orbTBodyModeSense_2
                      ->button(
                          !init_yaml["2nd Sense"].as<QString>().compare("CW"))
                      ->setChecked(true);
               }
            } break;
            case TBP_XYZ: {
               const QList<QLineEdit *> position_line = {
                   ui->orbTBodyCowellPos_1, ui->orbTBodyCowellPos_2,
                   ui->orbTBodyCowellPos_3};
               const QList<QLineEdit *> velocity_line = {
                   ui->orbTBodyCowellVel_1, ui->orbTBodyCowellVel_2,
                   ui->orbTBodyCowellVel_3};

               const QList<QString> pos_strings =
                   init_yaml["Position"].as<QList<QString>>();
               const QList<QString> vel_strings =
                   init_yaml["Velocity"].as<QList<QString>>();

               for (int i = 0; i < 3; i++) {
                  position_line[i]->setText(pos_strings[i]);
                  velocity_line[i]->setText(vel_strings[i]);
               }
            } break;
            case TBP_FILE: {
               const QString file_type = init_yaml["File Type"].as<QString>();
               setQComboBox(ui->orbTBodyFileType,
                            orbFileTypeInputs.value(file_type));
               ui->orbTBodyFileName->setText(
                   init_yaml["File Name"].as<QString>());
               ui->orbTBodyFileLabel->setText(
                   init_yaml["Label in File"].as<QString>());
            } break;
         }
      } break;
   }
   const Formation form        = orb_file_yaml["Formation"].as<Formation>();
   const EulerAngles euler_ang = form.getEulerAngles();

   const QList<QLineEdit *> euler_angle_edit = {ui->orbFormFrameEuler_1,
                                                ui->orbFormFrameEuler_2,
                                                ui->orbFormFrameEuler_3};
   const QList<QLineEdit *> pos_edit         = {
       ui->orbFormOriginPos_1, ui->orbFormOriginPos_2, ui->orbFormOriginPos_3};

   setQComboBox(ui->orbFormFrame, form.getFFrame());
   setQComboBox(ui->orbFormOrigin, form.getEFrame());
   setQComboBox(ui->orbFormFrameEulerSeq,
                QVariant(euler_ang.getSequence()).toString());
   for (int i = 0; i < 3; i++) {
      euler_angle_edit[i]->setText(QVariant(euler_ang.getAngles(i)).toString());
      pos_edit[i]->setText(QVariant(form.getPosition(i)).toString());
   }
}

void ORB_Menu::write_data(YAML::Node orb_file_yaml) {
   QStringList params;
   const QString label = orb_file_yaml["Configuration"]["Name"].as<QString>();
   const QString file_path = orbFileHash[label];
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      YAML::Emitter out;
      out.SetIndent(4);
      out.SetMapFormat(YAML::EMITTER_MANIP::Block);
      out << orb_file_yaml;
      in << out.c_str();
   }

   file.close();
}

void ORB_Menu::on_orbListRemove_clicked() {
   int removeItem = ui->orbList->currentRow();
   QString removeOrb;
   if (removeItem == -1)
      return;
   else {
      removeOrb = ui->orbList->currentItem()->text();
      ui->orbList->takeItem(removeItem);
      QFile::remove(orbFileHash[removeOrb]);
      orbFileHash.remove(removeOrb);
      ui->orbList->setCurrentRow(-1);
      global_orb_index = -1;
   }
}

void ORB_Menu::on_orbListAdd_clicked() {

   QString newOrb = "New";

   QStringList orbFileHashKeys = orbFileHash.keys();
   if (ui->orbList->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         QString newOrbTest = newOrb;
         if (i > 0)
            newOrbTest += "_" + QString::number(i);
         if (!orbFileHashKeys.contains(newOrbTest, Qt::CaseInsensitive)) {
            newOrb = newOrbTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }
   orbFileHash.insert(newOrb, inout_path + "Orb_" + newOrb + ".yaml");

   // TODO: make this copy from whatever default is in InOut/__default__
   QFile::copy(":/data/__default__/Orb_LEO.yaml", orbFileHash[newOrb]);

   ui->orbList->addItem(newOrb);

   receive_data(orbFileHash[newOrb]);
}

void ORB_Menu::on_orbList_itemClicked() {
   int index        = ui->orbList->currentRow();
   QString itemText = ui->orbList->currentItem()->text();

   if (index == global_orb_index) {
      return;
   } else {

      if ((global_orb_index != -1) && (global_orb_ignore == 0)) {
         int response = dsm_gui_lib::warning_message(
             "Note that changes to the previous selected Orbit are lost unless "
             "you first select \"Apply\"! This is your only warning.");
         if (response == QMessageBox::Cancel) {
            ui->orbList->setCurrentRow(global_orb_index);
            global_orb_ignore = 1;
            return;
         } else if (response == QMessageBox::Ok) {
            global_orb_ignore = 1;
         }
      }

      receive_data(orbFileHash[itemText]);

      global_orb_index = index;
   }
}

void ORB_Menu::on_loadDefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message(
       "Delete all Orbit files and load current Default Orbits?");
   if (response == QMessageBox::Ok) {
      for (auto i = orbFileHash.cbegin(), end = orbFileHash.cend(); i != end;
           ++i)
         QFile::remove(i.value());
      orbFileHash.clear();
      ui->orbList->clear();

      QStringList orbFiles =
          QDir(inout_path + "__default__/").entryList({"Orb_*.yaml"});

      for (int i = 0; i < orbFiles.length(); i++) {
         QString orb_name = orbFiles[i].chopped(5).mid(4);
         orbFileHash.insert(orb_name,
                            inout_path +
                                orbFiles[i]); // Full file path of Orbit file
         ui->orbList->addItem(
             orb_name); // Everything between "Orb_" and ".yaml"
         QFile::copy(inout_path + "__default__/" + orbFiles[i],
                     orbFileHash[orb_name]);
      }
   } else
      return;
}

void ORB_Menu::on_saveDefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message(
       "Delete all Default Orbit files and replace with current Orbits?");
   if (response == QMessageBox::Ok) {
      QStringList defOrbFiles =
          QDir(inout_path + "__default__/").entryList({"Orb_*"});
      for (int i = 0; i < defOrbFiles.count(); i++)
         QFile::remove(inout_path + "__default__/" + defOrbFiles[i]);

      QStringList orbFiles = QDir(inout_path).entryList({"Orb_*"});
      for (auto i = orbFileHash.cbegin(), end = orbFileHash.cend(); i != end;
           ++i)
         QFile::copy(i.value(),
                     inout_path + "__default__/" + "Orb_" + i.key() + ".yaml");
   } else
      return;
}

void ORB_Menu::on_closeButton_clicked() {
   ORB_Menu::close();
}

void ORB_Menu::on_applyButton_clicked() {

   int index = ui->orbList->currentRow();
   if (index == -1) {
      return;
   }

   QString oldOrb              = ui->orbList->currentItem()->text();
   QString oldOrbFile          = orbFileHash.take(oldOrb);
   QStringList orbFileHashKeys = orbFileHash.keys();

   QString newLabel = ui->orbLabel->text();

   if (newLabel.compare(oldOrb) != 0) {
      connect(this, SIGNAL(orbit_changed()), this->parent(),
              SLOT(disable_sub_menus()));
      emit orbit_changed();
      disconnect(this, SIGNAL(orbit_changed()), 0, 0);
   }

   if (orbFileHashKeys.contains(newLabel, Qt::CaseInsensitive)) {
      dsm_gui_lib::warning_message(
          "Orbit \"" + newLabel +
          "\" already exists. Orbit names are NOT case sensitive.");
      orbFileHash.insert(oldOrb, oldOrbFile);
      return;
   }

   newLabel = newLabel.remove("\"");
   ui->orbList->currentItem()->setText(newLabel);
   orbFileHash.insert(newLabel, inout_path + "Orb_" + newLabel + ".yaml");
   QFile::rename(oldOrbFile, orbFileHash[newLabel]);
   QString file_path = orbFileHash[newLabel];

   YAML::Node orb_file_yaml(YAML::NodeType::Map);
   YAML::Node config_yaml(YAML::NodeType::Map);
   orb_file_yaml["Configuration"] = config_yaml;
   config_yaml["Name"]            = newLabel;
   config_yaml["Description"]     = ui->orbDescription->text().remove("\"");

   YAML::Node orb_yaml(YAML::NodeType::Map);
   orb_file_yaml["Orbit"]     = orb_yaml;
   const QString orb_type_str = orbTypeInputs.key(ui->orbType->currentText());
   orb_yaml["Type"]           = orb_type_str;
   const enum orb_types orb_type = orbTypeHash[orb_type_str];
   switch (orb_type) {
      case ORB_ZERO: {
         QString world = ui->orbZeroWorld->currentText();
         if (world.contains("MINORBODY")) {
            world += "_" + ui->orbZeroMinorBodyNum->cleanText();
         }
         orb_yaml["World"]           = world;
         orb_yaml["Polyhedron Grav"] = ui->orbZeroPolyGrav->checkedId() == 1;
      } break;
      case ORB_FLIGHT: {
         orb_yaml["Region"]          = ui->orbFlightRegion->cleanText();
         orb_yaml["Polyhedron Grav"] = ui->orbFlightPolyGrav->checkedId() == 1;
      } break;
      case ORB_CENTRAL: {
         QString world = ui->orbCentWorld->currentText();
         if (world.contains("MINORBODY")) {
            world += "_" + ui->orbCentMinorBodyNum->cleanText();
         }
         orb_yaml["World"]            = world;
         orb_yaml["J2 Secular Drift"] = ui->orbCentJ2->checkedId() == 1;
         YAML::Node init_yaml(YAML::NodeType::Map);
         orb_yaml["Init"] = init_yaml;
         const QString init_str =
             orbCentICTypeInputs.key(ui->orbCentICParam->currentText());
         init_yaml["Method"]                  = init_str;
         const enum cent_init_types init_type = orbCentICTypeHash[init_str];
         switch (init_type) {
            case CENT_KEP: {
               const bool is_pa      = ui->orbCentPA->checkedId() == 1;
               const QString pa_type = (is_pa) ? "PA" : "AE";
               init_yaml["SMA Parameterization"] = pa_type;
               if (is_pa) {
                  init_yaml["Periapsis"] = ui->orbCentKepPeriAlt->text();
                  init_yaml["Apoapsis"]  = ui->orbCentKepApoAlt->text();
               } else {
                  init_yaml["Minimum Altitude"] = ui->orbCentKepMinAlt->text();
                  init_yaml["Eccentricity"]     = ui->orbCentKepEcc->text();
               }
               init_yaml["Inclination"]      = ui->orbCentKepInc->text();
               init_yaml["RAAN"]             = ui->orbCentKepRAAN->text();
               init_yaml["Arg of Periapsis"] = ui->orbCentKepArgPeri->text();
               init_yaml["True Anomaly"]     = ui->orbCentKepTA->text();
            } break;
            case CENT_RV: {
               const QList<QLineEdit *> position_line = {
                   ui->orbCentPVPos_1, ui->orbCentPVPos_2, ui->orbCentPVPos_3};
               const QList<QLineEdit *> velocity_line = {
                   ui->orbCentPVVel_1, ui->orbCentPVVel_2, ui->orbCentPVVel_3};
               QList<double> pos, vel;
               for (int i = 0; i < 3; i++) {
                  pos.append(position_line[i]->text().toDouble());
                  vel.append(velocity_line[i]->text().toDouble());
               }
               init_yaml["Position"] = pos;
               init_yaml["Velocity"] = vel;
            } break;
            case CENT_FILE: {
               init_yaml["File Type"] =
                   orbFileTypeInputs.key(ui->orbCentFileType->currentText());
               init_yaml["File Name"]     = ui->orbCentFileName->toPlainText();
               init_yaml["Label in File"] = ui->orbCentFileLabel->text();
            } break;
         }
      } break;
      case ORB_THREE_BODY: {
         orb_yaml["Lagrange System"] =
             orbTBodyLSysInputs.key(ui->orbTBodyLSystem->currentText());
         orb_yaml["Propagation Method"] =
             orbTBodyPropInputs.key(ui->orbTBodyProp->currentText());

         YAML::Node init_yaml(YAML::NodeType::Map);
         orb_yaml["Init"] = init_yaml;
         const QString init_str =
             orbTBodyICTypeInputs.key(ui->orbTBodyICParam->currentText());
         init_yaml["Method"]                 = init_str;
         const enum tbp_init_types init_type = orbTBodyICTypeHash[init_str];
         switch (init_type) {
            case TBP_MODES: {
               const QString l_point       = ui->orbTBodyLPoint->currentText();
               init_yaml["Lagrange Point"] = l_point;
               init_yaml["XY SMA"]         = ui->orbTBodyModeXYSMA->text();
               init_yaml["XY Phase"]       = ui->orbTBodyModeXYPhase->text();
               init_yaml["Sense"] =
                   (ui->orbTBodyModeSense->checkedId()) ? "CW" : "CCW";
               init_yaml["Z SMA"]   = ui->orbTBodyModeZSMA->text();
               init_yaml["Z Phase"] = ui->orbTBodyModeZPhase->text();
               if (!l_point.compare("L4") || !l_point.compare("L5")) {
                  init_yaml["XY 2nd SMA"]   = ui->orbTBodyModeXYSMA_2->text();
                  init_yaml["XY 2nd Phase"] = ui->orbTBodyModeXYPhase_2->text();
                  init_yaml["2nd Sense"] =
                      (ui->orbTBodyModeSense_2->checkedId()) ? "CW" : "CCW";
               }
            } break;
            case TBP_XYZ: {
               const QList<QLineEdit *> position_line = {
                   ui->orbTBodyCowellPos_1, ui->orbTBodyCowellPos_2,
                   ui->orbTBodyCowellPos_3};
               const QList<QLineEdit *> velocity_line = {
                   ui->orbTBodyCowellVel_1, ui->orbTBodyCowellVel_2,
                   ui->orbTBodyCowellVel_3};
               QList<double> pos, vel;
               for (int i = 0; i < 3; i++) {
                  pos.append(position_line[i]->text().toDouble());
                  vel.append(velocity_line[i]->text().toDouble());
               }
               init_yaml["Position"] = pos;
               init_yaml["Velocity"] = vel;
            } break;
            case TBP_FILE: {
               init_yaml["File Type"] =
                   orbFileTypeInputs.key(ui->orbTBodyFileType->currentText());
               init_yaml["File Name"]     = ui->orbTBodyFileName->toPlainText();
               init_yaml["Label in File"] = ui->orbTBodyFileLabel->text();
            } break;
         }
      } break;
   }

   const QList<QLineEdit *> frame_euler_ang = {ui->orbFormFrameEuler_1,
                                               ui->orbFormFrameEuler_2,
                                               ui->orbFormFrameEuler_3};
   const QList<QLineEdit *> frame_pos       = {
       ui->orbFormOriginPos_1, ui->orbFormOriginPos_2, ui->orbFormOriginPos_3};

   QVector3D pos, ang;
   for (int i = 0; i < 3; i++) {
      pos[i] = frame_pos[i]->text().toDouble();
      ang[i] = frame_euler_ang[i]->text().toDouble();
   }
   const QString f_frame = ui->orbFormFrame->currentText();
   const QString e_frame = ui->orbFormOrigin->currentText();
   const int seq         = ui->orbFormFrameEulerSeq->currentText().toInt();
   Formation form(f_frame, ang, seq, e_frame, pos);
   orb_file_yaml["Formation"] = form;

   write_data(orb_file_yaml);
}

void ORB_Menu::clear_data() {
   ui->orbLabel->clear();

   ui->orbDescription->clear();
   ui->orbType->setCurrentIndex(0);

   ui->orbZeroWorld->setCurrentIndex(0);
   ui->orbZeroMinorBodyNum->setValue(0);
   ui->orbZeroPolyGrav->button(0)->setChecked(true);

   ui->orbFlightRegion->setValue(0);
   ui->orbFlightPolyGrav->button(0)->setChecked(true);

   ui->orbCentWorld->setCurrentIndex(0);
   ui->orbCentMinorBodyNum->setValue(0);
   ui->orbCentJ2->button(0)->setChecked(true);
   ui->orbCentICParam->setCurrentIndex(0);
   ui->orbCentPA->button(0)->setChecked(true);
   ui->orbCentKepPeriAlt->setText("0");
   ui->orbCentKepApoAlt->setText("0");
   ui->orbCentKepMinAlt->setText("0");
   ui->orbCentKepEcc->setValue(0);
   ui->orbCentKepInc->setText("0");
   ui->orbCentKepRAAN->setText("0");
   ui->orbCentKepArgPeri->setText("0");
   ui->orbCentKepTA->setText("0");
   ui->orbCentPVPos_1->setText("0");
   ui->orbCentPVPos_2->setText("0");
   ui->orbCentPVPos_3->setText("0");
   ui->orbCentPVVel_1->setText("0");
   ui->orbCentPVVel_2->setText("0");
   ui->orbCentPVVel_3->setText("0");
   ui->orbCentFileType->setCurrentIndex(0);
   ui->orbCentFileName->clear();
   ui->orbCentFileLabel->clear();

   ui->orbTBodyLSystem->setCurrentIndex(0);
   ui->orbTBodyProp->setCurrentIndex(0);
   ui->orbTBodyICParam->setCurrentIndex(0);
   ui->orbTBodyLPoint->setCurrentIndex(0);

   ui->orbTBodyModeXYSMA->setText("0");
   ui->orbTBodyModeXYPhase->setText("0");
   ui->orbTBodyModeSense->button(0)->setChecked(true);
   ui->orbTBodyModeXYSMA_2->setText("0");
   ui->orbTBodyModeXYPhase_2->setText("0");
   ui->orbTBodyModeSense_2->button(0)->setChecked(true);
   ui->orbTBodyModeZSMA->setText("0");
   ui->orbTBodyModeZPhase->setText("0");
   ui->orbTBodyCowellPos_1->setText("0");
   ui->orbTBodyCowellPos_2->setText("0");
   ui->orbTBodyCowellPos_3->setText("0");
   ui->orbTBodyCowellVel_1->setText("0");
   ui->orbTBodyCowellVel_2->setText("0");
   ui->orbTBodyCowellVel_3->setText("0");
   ui->orbTBodyFileType->setCurrentIndex(0);
   ui->orbTBodyFileLabel->clear();
   ui->orbTBodyFileName->clear();

   ui->orbFormFrame->setCurrentIndex(0);
   ui->orbFormFrameEulerSeq->setCurrentIndex(0);
   ui->orbFormFrameEuler_1->setText("0");
   ui->orbFormFrameEuler_2->setText("0");
   ui->orbFormFrameEuler_3->setText("0");
   ui->orbFormOrigin->setCurrentIndex(0);
   ui->orbFormOriginPos_1->setText("0");
   ui->orbFormOriginPos_2->setText("0");
   ui->orbFormOriginPos_3->setText("0");
}

void ORB_Menu::string2radiobool(QString boolString, QButtonGroup *buttonGroup) {
   if (!boolString.compare(
           "TRUE", Qt::CaseInsensitive)) { // compare returns zero if equal
      buttonGroup->button(1)->setChecked(true);
   } else {
      buttonGroup->button(0)->setChecked(true);
   }
}

void ORB_Menu::setQComboBox(QComboBox *comboBox, QString string) {
   comboBox->setCurrentIndex(comboBox->findText(string));
}

void ORB_Menu::on_orbType_currentTextChanged(const QString &arg1) {
   for (int i = 0; i < ui->orbTab->count(); i++)
      ui->orbTab->setTabVisible(i, !arg1.compare(ui->orbTab->tabText(i)));
}

void ORB_Menu::on_orbCentICParam_currentTextChanged(const QString &arg1) {
   for (int i = 0; i < ui->orbCentTypeTab->count(); i++)
      ui->orbCentTypeTab->setTabVisible(
          i, !arg1.compare(ui->orbCentTypeTab->tabText(i)));
}

void ORB_Menu::on_orbTBodyICParam_currentTextChanged(const QString &arg1) {
   for (int i = 0; i < ui->orbTBodyTypeTab->count(); i++)
      ui->orbTBodyTypeTab->setTabVisible(
          i, !arg1.compare(ui->orbTBodyTypeTab->tabText(i)));
}

void ORB_Menu::on_orbZeroWorld_currentTextChanged(const QString &arg1) {
   ui->orbZeroMinorBodyNum->setEnabled(!arg1.compare("MINORBODY"));
   ui->orbZeroMinorBodyLabel->setEnabled(!arg1.compare("MINORBODY"));
}

void ORB_Menu::on_orbCentWorld_currentTextChanged(const QString &arg1) {
   ui->orbCentMinorBodyNum->setEnabled(!arg1.compare("MINORBODY"));
   ui->orbCentMinorBodyLabel->setEnabled(!arg1.compare("MINORBODY"));
}

void ORB_Menu::on_orbCentFileSelect_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->orbCentFileName->setText(rel_file_path);
}

void ORB_Menu::on_orbTBodyFileSelect_clicked() {
   QString file_name = QFileDialog::getOpenFileName(
       this, tr("Choose Folder"), inout_path, QString(), nullptr,
       QFileDialog::DontUseNativeDialog);

   QDir dir(inout_path);
   QString rel_file_path = dir.relativeFilePath(file_name);

   if (file_name.isEmpty())
      return;

   ui->orbTBodyFileName->setText(rel_file_path);
}

void ORB_Menu::on_orbListDuplicate_clicked() {
   int index = ui->orbList->currentRow();
   if (index == -1)
      return;
   QString oldOrb              = ui->orbList->currentItem()->text();
   QString newOrb              = oldOrb + "_Copy";
   QStringList orbFileHashKeys = orbFileHash.keys();
   for (int i = 0; i <= 30; i++) {
      QString newOrbTest = newOrb;
      if (i > 0)
         newOrbTest += "_" + QString::number(i);
      if (!orbFileHashKeys.contains(newOrbTest, Qt::CaseInsensitive)) {
         newOrb = newOrbTest;
         break;
      }
   }
   orbFileHash.insert(newOrb, inout_path + "Orb_" + newOrb + ".yaml");
   QFile::copy(orbFileHash[oldOrb], orbFileHash[newOrb]);

   ui->orbList->addItem(newOrb);
}

void ORB_Menu::on_orbCentPA_on_toggled(bool checked) {
   ui->orbCentKepPALabel->setEnabled(checked);
   ui->orbCentKepPeriAlt->setEnabled(checked);
   ui->orbCentKepApoAlt->setEnabled(checked);
   ui->orbCentKepAELabel->setEnabled(!checked);
   ui->orbCentKepMinAlt->setEnabled(!checked);
   ui->orbCentKepEcc->setEnabled(!checked);
}

void ORB_Menu::on_orbTBodyLPoint_currentTextChanged(const QString &text) {
   bool isTriPt = ((!text.compare("L4")) || (!text.compare("L5")));
   ui->orbTBodyModeXYSMALabel_2->setEnabled(isTriPt);
   ui->orbTBodyModeXYSMA_2->setEnabled(isTriPt);
   ui->orbTBodyModeXYPhaseLabel_2->setEnabled(isTriPt);
   ui->orbTBodyModeXYPhase_2->setEnabled(isTriPt);
   ui->orbTBodyModeSenseLabel_2->setEnabled(isTriPt);
   ui->orbTBodyModeSense_CW_2->setEnabled(isTriPt);
   ui->orbTBodyModeSense_CCW_2->setEnabled(isTriPt);
}

void ORB_Menu::checkKepPA() {
   int i;
   QString apoAlt  = ui->orbCentKepApoAlt->text();
   QString periAlt = ui->orbCentKepPeriAlt->text();
   ui->orbCentKepPeriAlt->setValidator(
       new QDoubleValidator(-INFINITY, apoAlt.toDouble(), 5));
   ui->orbCentKepApoAlt->setValidator(
       new QDoubleValidator(periAlt.toDouble(), INFINITY, 5));
   if (!(ui->orbCentPA->checkedId() == 1) ||
       ui->orbCentICParam->currentText().compare("Keplerian")) {
      ui->orbCentKepPAWarning->setVisible(false);
      ui->applyButton->setEnabled(true);
   } else if (ui->orbCentKepPeriAlt->validator()->validate(periAlt, i) ==
              QValidator::Intermediate) {
      ui->orbCentKepPAWarning->setText(
          "***Periapsis Altitude cannot be greater than Apoapsis Altitude!***");
      ui->orbCentKepPAWarning->setVisible(true);
      ui->applyButton->setEnabled(false);
   } else {
      ui->orbCentKepPAWarning->setVisible(false);
      ui->applyButton->setEnabled(true);
   }
}
