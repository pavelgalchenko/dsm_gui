#include "sim_menu.h"
#include "ui_sim_menu.h"
#include <qvariant.h>

SIM_Menu::SIM_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::SIM_Menu) {
   ui->setupUi(this);
   double_valid = std::make_unique<QDoubleValidator>();
   set_validators();
}

SIM_Menu::~SIM_Menu() {
   delete ui;
}

void SIM_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");
   QValidator *noQuotes = new QRegularExpressionValidator(rx);

   QList<QWidget *> init_disabled_items = {ui->simOrbitEn, ui->simSCEn,
                                           ui->simSCOrbit, ui->simSCOrbitLabel};
   for (auto item : init_disabled_items) {
      item->setEnabled(false);
   }

   // Numerical Validators
   // All validators that can be any double
   QList<QLineEdit *> double_line_edit = {
       ui->simSimDur,  ui->simSimStep, ui->simFileInterval,
       ui->simRNGSeed, ui->simLeapSec, ui->simF107,
       ui->simAp,      ui->simGSLong,  ui->simGSLat};

   dsm_gui_lib::set_mult_validators(double_line_edit, double_valid.get());

   ui->simTimeMode->addItems(
       dsm_gui_lib::sortStringList(timeModeInputs.values()));

   ui->simOrbList->setSortingEnabled(true);
   ui->simOrbList->sortItems(Qt::AscendingOrder);

   ui->simSCList->setSortingEnabled(true);
   ui->simSCList->sortItems(Qt::AscendingOrder);
   ui->simSCOrbit->setMaxVisibleItems(10);

   ui->simGSList->setSortingEnabled(true);
   ui->simGSList->sortItems(Qt::AscendingOrder);

   ui->simDate->setCalendarPopup(true);
   ui->simDate->setDisplayFormat("MM/dd/yyyy");
   ui->simTime->setTimeSpec(Qt::UTC);
   ui->simTime->setDisplayFormat("hh:mm:ss.zzz");
   ui->simF107Ap->addItems(dsm_gui_lib::sortStringList(f107Inputs.values()));
   ui->simMagfieldType->addItems(
       dsm_gui_lib::sortStringList(magfieldInputs.values()));

   ui->simEphem->addItems(dsm_gui_lib::sortStringList(ephemInputs.values()));

   ui->simGSWorld->addItems(dsm_gui_lib::worldInputs);
   ui->simGSLabel->setValidator(noQuotes);

   celestialBodies = {ui->simMercuryEn,  ui->simVenusEn,   ui->simEarthEn,
                      ui->simMarsEn,     ui->simJupiterEn, ui->simSaturnEn,
                      ui->simUranusEn,   ui->simNeptuneEn, ui->simPlutoEn,
                      ui->simAsteroidsEn};

   lagrangeSystems = {ui->simEarthMoonEn, ui->simSunEarthEn,
                      ui->simSunJupiterEn};

   connect(ui->applyButton, SIGNAL(clicked(bool)), this->parent(),
           SLOT(enable_sub_menus()));

   worldConfigHash.clear();

   for (QList<QString>::ConstIterator it =
            dsm_gui_lib::worldInputs.constBegin();
        it != dsm_gui_lib::worldInputs.constEnd(); ++it) {
      dsm_gui_lib::WorldID Iw = dsm_gui_lib::World2ID(*it);

      QString worldName = dsm_gui_lib::ID2World(Iw);
      switch (Iw) {
         case dsm_gui_lib::WorldID::SOL:
            worldConfigHash.insert(
                worldName,
                worldConfig(nullptr, nullptr, nullptr, nullptr, Iw, true));
            break;
         case dsm_gui_lib::WorldID::MERCURY:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simMercuryEn, Iw, false));
            break;
         case dsm_gui_lib::WorldID::VENUS:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simVenusEn, Iw, false));
            break;
         case dsm_gui_lib::WorldID::EARTH:
            worldConfigHash.insert(
                worldName,
                worldConfig(
                    new atmoConfig(ui->simF107, ui->simAp, ui->simF107Ap),
                    new gravConfig(ui->simEarthHarmN, ui->simEarthHarmM),
                    new magConfig(ui->simIGRFDegree, ui->simIGRFOrder,
                                  ui->simMagfieldType),
                    ui->simEarthEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::MARS:
            worldConfigHash.insert(
                worldName,
                worldConfig(nullptr,
                            new gravConfig(ui->simMarsHarmN, ui->simMarsHarmM),
                            nullptr, ui->simMarsEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::LUNA:
            worldConfigHash.insert(
                worldName,
                worldConfig(nullptr,
                            new gravConfig(ui->simLunaHarmN, ui->simLunaHarmM),
                            nullptr, nullptr, Iw, false));
            break;
         case dsm_gui_lib::WorldID::JUPITER:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simJupiterEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::SATURN:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simSaturnEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::URANUS:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simUranusEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::NEPTUNE:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simNeptuneEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::PLUTO:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simPlutoEn, Iw, true));
            break;
         case dsm_gui_lib::WorldID::MINORBODY:
            worldConfigHash.insert(worldName,
                                   worldConfig(nullptr, nullptr, nullptr,
                                               ui->simAsteroidsEn, Iw, false));
            break;
         default:
            worldConfigHash.insert(
                worldName,
                worldConfig(nullptr, nullptr, nullptr, nullptr, Iw, false));
            break;
      }
   }
}

void SIM_Menu::receive_simpath(QString path) {
   inoutPath = path;
   filePath  = inoutPath + "Inp_Sim.yaml";

   receive_data();
}
void SIM_Menu::receive_data() {
   QString newKey;
   QStringList orbFiles = QDir(inoutPath).entryList({"Orb_*.yaml"});

   // orbFileHash.clear();
   ui->simOrbList->clear();
   ui->simSCOrbit->clear();

   for (int i = 0; i < orbFiles.length(); i++) {
      newKey = orbFiles[i].chopped(5).mid(4);
      orbFileHash.insert(newKey, orbFiles[i].remove("\""));
      QListWidgetItem *newOrb = new QListWidgetItem(newKey);
      newOrb->setData(orbEnabledRole, false);
      newOrb->setData(orbNumberRole, -1);
      ui->simOrbList->addItem(newOrb);
   }
   ui->simSCOrbit->addItems(dsm_gui_lib::sortStringList(orbFileHash.keys()));

   QStringList scFiles = QDir(inoutPath).entryList({"SC_*.yaml"});

   scFileHash.clear();
   ui->simSCList->clear();

   for (int i = 0; i < scFiles.length(); i++) {
      newKey = scFiles[i].chopped(5).mid(3);
      scFileHash.insert(newKey, scFiles[i].remove("\""));
      QListWidgetItem *newSC = new QListWidgetItem(newKey);
      newSC->setData(scEnabledRole, false);
      newSC->setData(scOrbNameRole, "");
      ui->simSCList->addItem(newSC);
   }

   /* Load Yaml file */
   YAML::Node inp_sim = YAML::LoadFile(filePath.toStdString());

   /* Configure General Configuration Settings */
   QMap<QString, QString> sim_config =
       inp_sim["Simulation Control"].as<QMap<QString, QString>>();

   setQComboBox(ui->simTimeMode, timeModeInputs.value(sim_config["Mode"]));
   ui->simSimDur->setText(sim_config["Duration"]);
   ui->simSimStep->setText(sim_config["Step Size"]);
   ui->simFileInterval->setText(sim_config["File Interval"]);
   ui->simRNGSeed->setText(sim_config["RNG Seed"]);
   ui->simGraphicsEn->setChecked(
       QVariant(sim_config["Enable Graphics"]).toBool());
   // ui->simCmdFile->setText(sim_config["Command File"]);

   /* Configure Orbits */
   QStringList refs  = orbFileHash.keys();
   YAML::Node orbits = inp_sim["Orbits"];
   for (YAML::const_iterator it = orbits.begin(); it != orbits.end(); ++it) {
      QMap<QString, QString> orbConf = it->as<QMap<QString, QString>>();
      QString label                  = orbConf["Name"].mid(4);
      if (refs.contains(label)) {
         QListWidgetItem *item =
             ui->simOrbList->findItems(label, Qt::MatchExactly).at(0);
         item->setData(orbEnabledRole, QVariant(orbConf["Enabled"]).toBool());
      }
   }

   /* Configure Spacecraft */
   refs           = scFileHash.keys();
   YAML::Node scs = inp_sim["SCs"];
   for (YAML::const_iterator it = scs.begin(); it != scs.end(); ++it) {
      QMap<QString, QString> scConf = it->as<QMap<QString, QString>>();
      QString label                 = scConf["Name"].mid(3);
      if (refs.contains(label)) {
         QListWidgetItem *item =
             ui->simSCList->findItems(label, Qt::MatchExactly).at(0);
         item->setData(scEnabledRole, QVariant(scConf["Enabled"]).toBool());
         item->setData(scOrbNameRole, scConf["Orbit"].mid(4));
      }
   }
   QString defaultOrbName = ui->simSCOrbit->currentText();
   foreach (QListWidgetItem *item,
            ui->simSCList->findItems("*", Qt::MatchWildcard)) {
      if (item->data(scOrbNameRole).toString().isEmpty()) {
         item->setData(scOrbNameRole, defaultOrbName);
      }
   }

   /* Configure Time */
   YAML::Node time_conf = inp_sim["Time"];
   ui->simDate->setDate(time_conf["Date"].as<QDate>());
   ui->simTime->setTime(time_conf["Time"].as<QTime>());
   ui->simLeapSec->setText(time_conf["Leap Seconds"].as<QString>());

   /* Configure Perturbations and Models */
   YAML::Node pert_conf = inp_sim["Perturbation Models"];
   /* Atmosphere */
   YAML::Node atmo_conf = pert_conf["Atmosphere"];
   ui->simAeroPertEn->setChecked(atmo_conf["Enabled"].as<bool>());
   ui->simAeroPertShadow->setChecked(atmo_conf["Shadows"].as<bool>());
   YAML::Node atmo_model_conf = atmo_conf["Models"];
   for (YAML::const_iterator it = atmo_model_conf.begin();
        it != atmo_model_conf.end(); it++) {
      QString World = (*it)["World"].as<QString>().toUpper();
      if (worldConfigHash.find(World) != worldConfigHash.end() &&
          worldConfigHash[World].hasAtmo()) {
         worldConfigHash[World].setAtmo(
             f107Inputs.value((*it)["Method"].as<QString>()),
             (*it)["F10.7"].as<QString>(), (*it)["Ap"].as<QString>());
      } else {
         dsm_gui_lib::warning_message(
             World + " does not have a configurable atmospheric model.");
      }
   }
   /* Magnetic */
   YAML::Node mag_conf = pert_conf["Magnetic"];
   ui->simResidualMagEn->setChecked(mag_conf["Residual Mag Moment"].as<bool>());
   YAML::Node mag_model_conf = mag_conf["Models"];
   for (YAML::const_iterator it = mag_model_conf.begin();
        it != mag_model_conf.end(); ++it) {
      QString World = (*it)["World"].as<QString>().toUpper();
      if (worldConfigHash.find(World) != worldConfigHash.end() &&
          worldConfigHash[World].hasMag()) {
         worldConfigHash[World].setMag(
             magfieldInputs.value((*it)["Method"].as<QString>()),
             (*it)["Degree"].as<int>(), (*it)["Order"].as<int>());
      } else {
         dsm_gui_lib::warning_message(
             World + " does not have a configurable magnetic field model.");
      }
   }
   /* Gravitation */
   YAML::Node grav_conf = pert_conf["Gravitation"];
   ui->simGravPertEn->setChecked(grav_conf["Enabled"].as<bool>());
   ui->simGravGradientEn->setChecked(grav_conf["Gravity Gradient"].as<bool>());
   YAML::Node grav_model_conf = grav_conf["Models"];
   for (YAML::const_iterator it = grav_model_conf.begin();
        it != grav_model_conf.end(); ++it) {
      YAML::Node grav_model_node = it->as<YAML::Node>();
      QString World              = grav_model_node["World"].as<QString>();
      if (worldConfigHash.find(World) != worldConfigHash.end() &&
          worldConfigHash[World].hasGrav()) {
         worldConfigHash[World].setGrav(grav_model_node["Degree"].as<int>(),
                                        grav_model_node["Order"].as<int>());
      } else {
         dsm_gui_lib::warning_message(
             World +
             " does not have a configurable gravitational field model.");
      }
   }
   /* Solar Radiation Pressure */
   YAML::Node srp_conf = pert_conf["SRP"];
   ui->simSRPPertEn->setChecked(srp_conf["Enabled"].as<bool>());
   ui->simSRPPertShadow->setChecked(srp_conf["Shadows"].as<bool>());
   /* Thruster Plume */
   ui->simThrusterPlumeEn->setChecked(pert_conf["Thruster Plume"].as<bool>());
   /* Contact */
   ui->simContactEn->setChecked(pert_conf["Contact"].as<bool>());
   /* CFD Slosh */
   ui->simSloshEn->setChecked(pert_conf["CFD Slosh"].as<bool>());
   /* CSS Albedo */
   ui->simAlbedoEn->setChecked(pert_conf["Albedo on CSS"].as<bool>());
   /* Output Torques */
   ui->simOutputTorqueEn->setChecked(
       pert_conf["Output Env Torques to File"].as<bool>());

   /* Configure Ephemeris */
   setQComboBox(ui->simEphem,
                ephemInputs.value(inp_sim["Ephem Type"].as<QString>()));
   /* Celestial Bodies */
   YAML::Node celest_conf = inp_sim["Celestial Bodies"];
   for (QHash<QString, QString>::const_iterator it =
            worldConfNames.constBegin();
        it != worldConfNames.constEnd(); ++it) {
      worldConfigHash[it.key()].setEnabled(celest_conf[it.value()].as<bool>());
   }

   /* Lagrange Systems */
   YAML::Node lag_sys = inp_sim["Lagrange Systems"];
   ui->simEarthMoonEn->setChecked(lag_sys["Earth-Moon"].as<bool>());
   ui->simSunEarthEn->setChecked(lag_sys["Sun-Earth"].as<bool>());
   ui->simSunJupiterEn->setChecked(lag_sys["Sun-Jupiter"].as<bool>());

   /* Ground Stations */
   ui->simGSList->clear();
   YAML::Node gs_list = inp_sim["Ground Stations"];
   for (YAML::const_iterator it = gs_list.begin(); it != gs_list.end(); ++it) {
      YAML::Node gs_node = (*it)["Ground Station"];
      QString label      = gs_node["Label"].as<QString>();
      ui->simGSList->addItem(label);
      QListWidgetItem *item =
          ui->simGSList->findItems(label, Qt::MatchExactly).at(0);
      item->setData(gsEnabledRole, gs_node["Enabled"].as<bool>());
      item->setData(gsWorldRole, gs_node["World"].as<QString>());
      item->setData(gsLongRole, gs_node["Longitude"].as<double>());
      item->setData(gsLatRole, gs_node["Latitude"].as<double>());
   }
}

void SIM_Menu::write_data(YAML::Node inp_sim) {
   QStringList params;
   QFile::remove(filePath);
   QFile file(filePath);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      YAML::Emitter out;
      out.SetIndent(4);
      out.SetMapFormat(YAML::EMITTER_MANIP::Block);
      out << inp_sim;
      in << out.c_str();
   }
   file.close();
}

void SIM_Menu::clear_data() {
   ui->simTimeMode->setCurrentIndex(0);
   ui->simSimDur->clear();
   ui->simSimStep->clear();
   ui->simFileInterval->clear();
   ui->simRNGSeed->clear();
   ui->simGraphicsEn->setChecked(false);

   ui->simOrbList->clear();
   ui->simOrbitEn->setChecked(false);

   ui->simSCList->clear();
   ui->simSCEn->setChecked(false);
   ui->simSCOrbit->clear();

   ui->simDate->clear();
   ui->simTime->clear();
   ui->simLeapSec->clear();
   ui->simF107Ap->setCurrentIndex(0);
   ui->simF107->clear();
   ui->simAp->clear();
   ui->simMagfieldType->setCurrentIndex(0);
   ui->simIGRFDegree->setValue(0);
   ui->simIGRFOrder->setValue(0);
   ui->simEarthHarmN->setValue(0);
   ui->simEarthHarmM->setValue(0);
   ui->simMarsHarmN->setValue(0);
   ui->simMarsHarmM->setValue(0);
   ui->simLunaHarmN->setValue(0);
   ui->simLunaHarmM->setValue(0);
   ui->simAeroPertEn->setChecked(false);
   ui->simAeroPertShadow->setChecked(false);
   ui->simGravGradientEn->setChecked(false);
   ui->simSRPPertEn->setChecked(false);
   ui->simSRPPertShadow->setChecked(false);
   ui->simResidualMagEn->setChecked(false);
   ui->simGravPertEn->setChecked(false);
   ui->simThrusterPlumeEn->setChecked(false);
   ui->simContactEn->setChecked(false);
   ui->simSloshEn->setChecked(false);
   ui->simAlbedoEn->setChecked(false);
   ui->simOutputTorqueEn->setChecked(false);

   ui->simEphem->setCurrentIndex(0);
   for (int i = 0; i < celestialBodies.count(); i++)
      celestialBodies[i]->setChecked(false);

   for (int i = 0; i < lagrangeSystems.count(); i++)
      lagrangeSystems[i]->setChecked(false);

   ui->simGSList->clear();
   ui->simGSEn->setChecked(false);
   ui->simGSLat->clear();
   ui->simGSLong->clear();
   ui->simGSLabel->clear();

   orbFileHash.clear();
   scFileHash.clear();
}

void SIM_Menu::on_loadDefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message(
       "Delete Inp_Sim file and load current Default?");
   if (response == QMessageBox::Ok) {
      clear_data();

      QFile::remove(filePath);
      QFile::copy(inoutPath + "__default__/Inp_Sim.yaml", filePath);

      receive_data();
   } else
      return;
}

void SIM_Menu::on_saveDefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message(
       "Delete Default Inp_Sim file and replace with current?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inoutPath + "__default__/Inp_Sim.yaml");
      QFile::copy(filePath, inoutPath + "__default__/Inp_Sim.yaml");
   } else
      return;
}

void SIM_Menu::on_closeButton_clicked() {
   SIM_Menu::close();
}

void SIM_Menu::on_applyButton_clicked() {
   QStringList refs;

   // enable orbits associated with enabled spacecraft
   foreach (QListWidgetItem *scItem,
            ui->simSCList->findItems("*", Qt::MatchWildcard)) {
      if (scItem->data(scEnabledRole).toBool()) {
         QString scOrb = scItem->data(scOrbNameRole).toString();
         QListWidgetItem *orbItem =
             ui->simOrbList->findItems(scOrb, Qt::MatchExactly).at(0);
         orbItem->setData(orbEnabledRole, true);
      }
   }

   /* Load Yaml file */
   YAML::Node inp_sim      = YAML::LoadFile(filePath.toStdString());
   YAML::Node sim_config   = inp_sim["Simulation Control"];
   sim_config["Mode"]      = timeModeInputs.key(ui->simTimeMode->currentText());
   sim_config["Duration"]  = ui->simSimDur->text();
   sim_config["Step Size"] = ui->simSimStep->text();
   sim_config["File Interval"]   = ui->simFileInterval->text();
   sim_config["RNG Seed"]        = ui->simRNGSeed->text().toInt();
   sim_config["Enable Graphics"] = ui->simGraphicsEn->isChecked();
   sim_config["Command File"]    = "Inp_Cmd.txt"; // ui->simCmdFile->text();

   YAML::Node time_conf      = inp_sim["Time"];
   time_conf["Date"]         = ui->simDate->date();
   time_conf["Time"]         = ui->simTime->time();
   time_conf["Leap Seconds"] = ui->simLeapSec->text();

   YAML::Node orbits = inp_sim["Orbits"];
   orbits.reset();
   for (int i = 0; i < ui->simOrbList->count(); i++) {
      QListWidgetItem *item = ui->simOrbList->item(i);
      YAML::Node it(YAML::NodeType::Map);
      it["Name"]    = "Orb_" + item->text();
      it["Enabled"] = item->data(orbEnabledRole).toBool();
      orbits.push_back(it);
   }

   YAML::Node scs = inp_sim["SCs"];
   scs.reset();
   for (int i = 0; i < ui->simSCList->count(); i++) {
      QListWidgetItem *item = ui->simSCList->item(i);
      YAML::Node it(YAML::NodeType::Map);
      it["Name"]    = "SC_" + item->text();
      it["Enabled"] = item->data(scEnabledRole).toBool();
      it["Orbit"]   = "Orb_" + item->data(scOrbNameRole).toString();
      scs.push_back(it);
   }

   YAML::Node pert_conf               = inp_sim["Perturbation Models"];
   pert_conf["Atmosphere"]["Enabled"] = ui->simAeroPertEn->isChecked();
   pert_conf["Atmosphere"]["Shadows"] = ui->simAeroPertShadow->isChecked();
   YAML::Node atmo_models             = pert_conf["Atmosphere"]["Models"];
   atmo_models.reset();

   pert_conf["Magnetic"]["Residual Mag Moment"] =
       ui->simResidualMagEn->isChecked();
   YAML::Node mag_models = pert_conf["Magnetic"]["Models"];
   mag_models.reset();

   pert_conf["Gravitation"]["Enabled"] = ui->simGravPertEn->isChecked();
   pert_conf["Gravitation"]["Gravity Gradient"] =
       ui->simGravGradientEn->isChecked();
   YAML::Node grav_models = pert_conf["Gravitation"]["Models"];
   grav_models.reset();

   pert_conf["SRP"]["Enabled"] = ui->simSRPPertEn->isChecked();
   pert_conf["SRP"]["Shadows"] = ui->simSRPPertShadow->isChecked();

   pert_conf["Thruster Plume"] = ui->simThrusterPlumeEn->isChecked();
   pert_conf["Contact"]        = ui->simContactEn->isChecked();
   pert_conf["CFD Slosh"]      = ui->simSloshEn->isChecked();
   pert_conf["Albedo on CSS"]  = ui->simAlbedoEn->isChecked();
   pert_conf["Output Env Torques to File"] = ui->simOutputTorqueEn->isChecked();

   for (QHash<QString, worldConfig>::iterator it = worldConfigHash.begin();
        it != worldConfigHash.end(); ++it) {
      if (it->hasAtmo()) {
         YAML::Node atmoNode(YAML::NodeType::Map);
         atmoNode["World"]  = dsm_gui_lib::ID2World(it->getID());
         atmoNode["Method"] = f107Inputs.key(it->getAtmoMethod());
         atmoNode["F10.7"]  = it->getAtmoF107();
         atmoNode["Ap"]     = it->getAtmoAp();
         atmo_models.push_back(atmoNode);
      }
      if (it->hasMag()) {
         YAML::Node magNode(YAML::NodeType::Map);
         magNode["World"]  = dsm_gui_lib::ID2World(it->getID());
         magNode["Method"] = magfieldInputs.key(it->getMagMethod());
         magNode["Degree"] = it->getMagDegree();
         magNode["Order"]  = it->getMagOrder();
         mag_models.push_back(magNode);
      }
      if (it->hasGrav()) {
         YAML::Node gravNode(YAML::NodeType::Map);
         gravNode["World"]  = dsm_gui_lib::ID2World(it->getID());
         gravNode["Degree"] = it->getGravDegree();
         gravNode["Order"]  = it->getGravOrder();
         grav_models.push_back(gravNode);
      }
   }

   inp_sim["Ephem Type"]  = ephemInputs.key(ui->simEphem->currentText());
   YAML::Node celest_conf = inp_sim["Celestial Bodies"];
   for (QHash<QString, QString>::const_iterator it =
            worldConfNames.constBegin();
        it != worldConfNames.constEnd(); ++it) {
      celest_conf[it.value()] = worldConfigHash[it.key()].getEnabled();
   }

   YAML::Node lag_sys     = inp_sim["Lagrange Systems"];
   lag_sys["Earth-Moon"]  = ui->simEarthMoonEn->isChecked();
   lag_sys["Sun-Earth"]   = ui->simSunEarthEn->isChecked();
   lag_sys["Sun-Jupiter"] = ui->simSunJupiterEn->isChecked();

   YAML::Node gs_list = inp_sim["Ground Stations"];
   gs_list.reset();

   for (int i = 0; i < ui->simGSList->count(); i++) {
      YAML::Node it(YAML::NodeType::Map);
      YAML::Node gs_node(YAML::NodeType::Map);
      it["Ground Station"] = gs_node;

      QListWidgetItem *item = ui->simGSList->item(i);
      gs_node["Index"]      = i;
      gs_node["Enabled"]    = item->data(gsEnabledRole).toBool();
      gs_node["World"]      = item->data(gsWorldRole).toString();
      gs_node["Longitude"]  = item->data(gsLongRole).toString();
      gs_node["Lattiude"]   = item->data(gsLatRole).toString();
      gs_node["Label"]      = item->text();
      gs_list.push_back(it);
   }

   write_data(inp_sim);
}

void SIM_Menu::setQComboBox(QComboBox *comboBox, QString string) {
   comboBox->setCurrentIndex(comboBox->findText(string));
}

void SIM_Menu::on_simOrbList_itemClicked(QListWidgetItem *item) {
   ui->simOrbitEn->setChecked(item->data(orbEnabledRole).toBool());
}

void SIM_Menu::on_simOrbitEn_toggled(bool checked) {
   if (ui->simOrbList->currentRow() == -1)
      return;
   ui->simOrbList->currentItem()->setData(orbEnabledRole, checked);
}

void SIM_Menu::on_simSCList_itemClicked(QListWidgetItem *item) {
   ui->simSCEn->setChecked(item->data(scEnabledRole).toBool());
   ui->simSCOrbit->setCurrentText(item->data(scOrbNameRole).toString());
}

void SIM_Menu::on_simSCEn_toggled(bool checked) {
   if (ui->simSCList->currentRow() == -1)
      return;
   ui->simSCList->currentItem()->setData(scEnabledRole, checked);
}

void SIM_Menu::on_simSCOrbit_currentTextChanged(const QString &arg1) {
   if (ui->simSCList->currentRow() == -1)
      return;
   ui->simSCList->currentItem()->setData(scOrbNameRole, arg1);
}

void SIM_Menu::on_simGSList_itemClicked(QListWidgetItem *item) {
   ui->simGSEn->setChecked(item->data(gsEnabledRole).toBool());
   QString gsWorld = item->data(gsWorldRole).toString();
   if (gsWorld.contains("MINORBODY")) {
      dsm_gui_lib::setQComboBox(ui->simGSWorld, "MINORBODY");
      QStringList split = gsWorld.split(QRegExp("_"), Qt::SkipEmptyParts);
      ui->simGSMinorBodyNum->setValue(split[1].toInt());
   } else {
      dsm_gui_lib::setQComboBox(ui->simGSWorld, gsWorld);
      ui->simGSMinorBodyNum->setValue(0);
   }
   ui->simGSLat->setText(item->data(gsLatRole).toString());
   ui->simGSLong->setText(item->data(gsLongRole).toString());
   ui->simGSLabel->setText(item->text());
}

void SIM_Menu::on_simGSEn_toggled(bool checked) {
   if (ui->simGSList->currentRow() == -1)
      return;
   ui->simGSList->currentItem()->setData(gsEnabledRole, checked);
}

void SIM_Menu::on_simGSWorld_currentTextChanged(const QString &arg1) {
   QString worldName = arg1;
   if (ui->simGSList->currentRow() == -1)
      return;
   bool isMinor = !worldName.compare("MINORBODY");
   ui->simGSMinorBodyLabel->setEnabled(isMinor);
   ui->simGSMinorBodyNum->setEnabled(isMinor);
   if (isMinor)
      worldName += "_" + ui->simGSMinorBodyNum->text();
   else
      ui->simGSMinorBodyNum->setValue(0);
   ui->simGSList->currentItem()->setData(gsWorldRole, worldName);
}

void SIM_Menu::on_simGSLat_textEdited(const QString &arg1) {
   if (ui->simGSList->currentRow() == -1)
      return;
   ui->simGSList->currentItem()->setData(gsLatRole, arg1.toDouble());
}

void SIM_Menu::on_simGSLong_textEdited(const QString &arg1) {
   if (ui->simGSList->currentRow() == -1)
      return;
   ui->simGSList->currentItem()->setData(gsLongRole, arg1.toDouble());
}

void SIM_Menu::on_simGSLabel_textEdited(const QString &arg1) {
   if (ui->simGSList->currentRow() == -1)
      return;
   ui->simGSList->currentItem()->setText(arg1);
}

void SIM_Menu::on_simGSListRemove_clicked() {
   int removeItem = ui->simGSList->currentRow();
   QString removeGS;
   if (removeItem == -1)
      return;
   else {
      removeGS = ui->simGSList->currentItem()->text();
      ui->simGSList->takeItem(removeItem);
      ui->simGSList->setCurrentRow(-1);
   }
}

void SIM_Menu::on_simGSListAdd_clicked() {
   QString newGS          = "GroundStation";
   QStringList curGSNames = dsm_gui_lib::getTextFromList(ui->simGSList);

   if (ui->simGSList->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         QString newGSTest = newGS;
         if (i > 0)
            newGSTest += "_" + QString::number(i);
         if (!curGSNames.contains(newGSTest)) {
            newGS = newGSTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }

   QListWidgetItem *newGSItem = new QListWidgetItem(newGS);
   newGSItem->setData(gsEnabledRole, true);
   newGSItem->setData(gsWorldRole, "EARTH");
   newGSItem->setData(gsLongRole, -76.852);
   newGSItem->setData(gsLatRole, 38.995);
   ui->simGSList->addItem(newGSItem);
}

void SIM_Menu::on_simGSListDuplicate_clicked() {
   int index                = ui->simGSList->currentRow();
   QListWidgetItem *curItem = ui->simGSList->currentItem();
   QStringList curGSNames   = dsm_gui_lib::getTextFromList(ui->simGSList);

   if (index == -1)
      return;
   QString oldGS = curItem->text();
   QString newGS = oldGS + "_Copy";
   for (int i = 0; i <= 30; i++) {
      QString newGSTest = newGS;
      if (i > 0)
         newGSTest += "_" + QString::number(i);
      if (!curGSNames.contains(newGSTest)) {
         newGS = newGSTest;
         break;
      }
   }
   QListWidgetItem *newItem = curItem->clone();
   newItem->setText(newGS);
   ui->simGSList->addItem(newItem);
}

void SIM_Menu::on_simF107Ap_currentTextChanged(const QString &arg1) {
   bool isUser = arg1 == f107Inputs["USER"];
   ui->simF107->setEnabled(isUser);
   ui->simAp->setEnabled(isUser);
   ui->simUserF107ApLabel->setEnabled(isUser);
}

void SIM_Menu::on_simMagfieldType_currentTextChanged(const QString &arg1) {
   bool isIGRF = arg1 == magfieldInputs["IGRF"];
   ui->simIGRFDegree->setEnabled(isIGRF);
   ui->simIGRFOrder->setEnabled(isIGRF);
   ui->simIGRFLabel->setEnabled(isIGRF);
}

void SIM_Menu::on_simGSMinorBodyNum_textChanged(const QString &arg1) {
   ui->simGSList->currentItem()->setData(gsWorldRole, "MINORBODY_" + arg1);
}

void SIM_Menu::on_simAeroPertShadow_toggled(bool checked) {
   if (checked)
      ui->simAeroPertEn->setChecked(true);
   ui->simAeroPertEn->setEnabled(!checked);
}

void SIM_Menu::on_simSRPPertShadow_toggled(bool checked) {
   if (checked)
      ui->simSRPPertEn->setChecked(true);
   ui->simSRPPertEn->setEnabled(!checked);
}

void SIM_Menu::on_simOrbList_currentItemChanged(QListWidgetItem *current,
                                                QListWidgetItem *) {
   bool isNull = current == NULL;
   ui->simOrbitEn->setEnabled(!isNull);
}

void SIM_Menu::on_simSCList_currentItemChanged(QListWidgetItem *current,
                                               QListWidgetItem *) {
   bool isNull = current == NULL;

   ui->simSCEn->setEnabled(!isNull);
   ui->simSCOrbit->setEnabled(!isNull);
   ui->simSCOrbitLabel->setEnabled(!isNull);
}
