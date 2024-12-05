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

      QString worldName  = dsm_gui_lib::ID2World(Iw);
      worldConfig *world = new worldConfig(Iw);
      switch (Iw) {
         case dsm_gui_lib::WorldID::SOL:
            world->setHasChildren(true);
            break;
         case dsm_gui_lib::WorldID::MERCURY:
            world->setEnable(ui->simMercuryEn);
            break;
         case dsm_gui_lib::WorldID::VENUS:
            world->setEnable(ui->simVenusEn);
            break;
         case dsm_gui_lib::WorldID::EARTH: {
            const atmoConfig atmo =
                atmoConfig(worldName, ui->simF107, ui->simAp, ui->simF107Ap);
            const gravConfig grav =
                gravConfig(worldName, ui->simEarthHarmN, ui->simEarthHarmM);
            const magConfig mag =
                magConfig(worldName, ui->simIGRFDegree, ui->simIGRFOrder,
                          ui->simMagfieldType);
            world->setEnable(ui->simEarthEn);
            world->set(atmo);
            world->set(grav);
            world->set(mag);
         } break;
         case dsm_gui_lib::WorldID::MARS: {
            const gravConfig grav =
                gravConfig(worldName, ui->simMarsHarmN, ui->simMarsHarmM);
            world->setEnable(ui->simMarsEn);
            world->set(grav);
         } break;
         case dsm_gui_lib::WorldID::LUNA: {
            const gravConfig grav =
                gravConfig(worldName, ui->simLunaHarmN, ui->simLunaHarmM);
            world->setEnable(ui->simEarthEn);
            world->set(grav);
         } break;
         case dsm_gui_lib::WorldID::JUPITER:
            world->setEnable(ui->simJupiterEn);
            break;
         case dsm_gui_lib::WorldID::SATURN:
            world->setEnable(ui->simSaturnEn);
            break;
         case dsm_gui_lib::WorldID::URANUS:
            world->setEnable(ui->simUranusEn);
            break;
         case dsm_gui_lib::WorldID::NEPTUNE:
            world->setEnable(ui->simNeptuneEn);
            break;
         case dsm_gui_lib::WorldID::PLUTO:
            world->setEnable(ui->simPlutoEn);
            break;
         case dsm_gui_lib::WorldID::MINORBODY:
            world->setEnable(ui->simAsteroidsEn);
            break;
         default:
            break;
      }
      worldConfigHash.insert(worldName, world);
   }
   connect(ui->simF107, SIGNAL(textChanged(QString)), this,
           SLOT(UpdateModels()));
   connect(ui->simAp, SIGNAL(textChanged(QString)), this, SLOT(UpdateModels()));
   connect(ui->simF107Ap, SIGNAL(textActivated(QString)), this,
           SLOT(UpdateModels()));
   connect(ui->simEarthHarmN, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
   connect(ui->simEarthHarmM, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
   connect(ui->simIGRFDegree, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
   connect(ui->simIGRFOrder, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
   connect(ui->simMagfieldType, SIGNAL(textActivated(QString)), this,
           SLOT(UpdateModels()));

   connect(ui->simMarsHarmN, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
   connect(ui->simMarsHarmM, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));

   connect(ui->simLunaHarmN, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
   connect(ui->simLunaHarmM, SIGNAL(valueChanged(int)), this,
           SLOT(UpdateModels()));
}

void SIM_Menu::receive_simpath(QString path) {
   inoutPath = path;
   filePath  = inoutPath + "Inp_Sim.yaml";

   receive_data();
}
void SIM_Menu::receive_data() {
   orbitHash.clear();
   scHash.clear();
   QString newKey;
   QStringList orbFiles = QDir(inoutPath).entryList({"Orb_*.yaml"});

   // orbFileHash.clear();
   ui->simOrbList->clear();
   ui->simSCOrbit->clear();

   for (int i = 0; i < orbFiles.length(); i++) {
      newKey               = orbFiles[i].chopped(5).mid(4);
      orbitConfig *new_orb = new orbitConfig(newKey, false);
      orbFileHash.insert(newKey, orbFiles[i].remove("\""));
      QListWidgetItem *newOrb = new QListWidgetItem(newKey, ui->simOrbList);
      orbitHash.insert(newOrb, new_orb);
   }
   ui->simSCOrbit->addItems(dsm_gui_lib::sortStringList(orbFileHash.keys()));

   QStringList scFiles = QDir(inoutPath).entryList({"SC_*.yaml"});

   scFileHash.clear();
   ui->simSCList->clear();
   orbitConfig *const first_orbit = orbitHash.value(ui->simOrbList->item(0));

   for (int i = 0; i < scFiles.length(); i++) {
      newKey           = scFiles[i].chopped(5).mid(3);
      scConfig *new_sc = new scConfig(newKey, false, first_orbit);
      scFileHash.insert(newKey, scFiles[i].remove("\""));
      QListWidgetItem *newSC = new QListWidgetItem(newKey, ui->simSCList);
      scHash.insert(newSC, new_sc);
   }

   /* Load Yaml file */
   YAML::Node inp_sim = YAML::LoadFile(filePath.toStdString());

   /* Configure General Configuration Settings */
   QMap<QString, QString> sim_config =
       inp_sim["Simulation Control"].as<QMap<QString, QString>>();

   dsm_gui_lib::setQComboBox(ui->simTimeMode,
                             timeModeInputs.value(sim_config["Mode"]));
   ui->simSimDur->setText(sim_config["Duration"]);
   ui->simSimStep->setText(sim_config["Step Size"]);
   ui->simFileInterval->setText(sim_config["File Interval"]);
   ui->simRNGSeed->setText(sim_config["RNG Seed"]);
   ui->simGraphicsEn->setChecked(
       QVariant(sim_config["Enable Graphics"]).toBool());
   // ui->simCmdFile->setText(sim_config["Command File"]);

   /* Configure Orbits */
   QList<orbitConfig> orbits = inp_sim["Orbits"].as<QList<orbitConfig>>();
   foreach (auto orbit_data, orbits) {
      orbitConfig *orbit =
          dsm_gui_lib::getObjectFromItemName(orbit_data.name(), orbitHash);
      if (orbit != nullptr) {
         orbit->setEnabled(orbit_data.enabled());
      }
   }

   /* Configure Spacecraft */
   QList<scConfig> scs = inp_sim["SCs"].as<QList<scConfig>>();
   foreach (auto sc_data, scs) {
      scConfig *sc = dsm_gui_lib::getObjectFromItemName(sc_data.name(), scHash);
      if (sc != nullptr) {
         sc->setEnabled(sc_data.enabled());
         sc->setOrbit(sc_data.orbit());
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
   QList<atmoConfig> atmo_models = atmo_conf["Models"].as<QList<atmoConfig>>();
   foreach (atmoConfig atmo, atmo_models) {
      const QString world = atmo.getWorld().toUpper();
      if (worldConfigHash.find(world) != worldConfigHash.end() &&
          worldConfigHash[world]->hasAtmo()) {
         worldConfigHash[world]->set(atmo);
      } else {
         dsm_gui_lib::warning_message(
             world + " does not have a configurable atmospheric model.");
      }
   }

   /* Magnetic */
   YAML::Node mag_conf = pert_conf["Magnetic"];
   ui->simResidualMagEn->setChecked(mag_conf["Residual Mag Moment"].as<bool>());
   QList<magConfig> mag_models = mag_conf["Models"].as<QList<magConfig>>();
   foreach (magConfig mag, mag_models) {
      const QString world = mag.getWorld().toUpper();
      if (worldConfigHash.find(world) != worldConfigHash.end() &&
          worldConfigHash[world]->hasMag()) {
         worldConfigHash[world]->set(mag);
      } else {
         dsm_gui_lib::warning_message(
             world + " does not have a configurable magnetic field model.");
      }
   }
   /* Gravitation */
   YAML::Node grav_conf = pert_conf["Gravitation"];
   ui->simGravPertEn->setChecked(grav_conf["Enabled"].as<bool>());
   ui->simGravGradientEn->setChecked(grav_conf["Gravity Gradient"].as<bool>());
   QList<gravConfig> grav_models = grav_conf["Models"].as<QList<gravConfig>>();
   foreach (gravConfig grav, grav_models) {
      const QString world = grav.getWorld().toUpper();
      if (worldConfigHash.find(world) != worldConfigHash.end() &&
          worldConfigHash[world]->hasGrav()) {
         worldConfigHash[world]->set(grav);
      } else {
         dsm_gui_lib::warning_message(
             world +
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
   dsm_gui_lib::setQComboBox(
       ui->simEphem, ephemInputs.value(inp_sim["Ephem Type"].as<QString>()));
   /* Celestial Bodies */
   YAML::Node celest_conf = inp_sim["Celestial Bodies"];
   for (QHash<QString, QString>::const_iterator it =
            worldConfNames.constBegin();
        it != worldConfNames.constEnd(); ++it) {
      worldConfigHash[it.key()]->setEnabled(celest_conf[it.value()].as<bool>());
   }

   /* Lagrange Systems */
   YAML::Node lag_sys = inp_sim["Lagrange Systems"];
   ui->simEarthMoonEn->setChecked(lag_sys["Earth-Moon"].as<bool>());
   ui->simSunEarthEn->setChecked(lag_sys["Sun-Earth"].as<bool>());
   ui->simSunJupiterEn->setChecked(lag_sys["Sun-Jupiter"].as<bool>());

   /* Ground Stations */
   ui->simGSList->clear();
   QList<GroundStation> ground_stations =
       inp_sim["Ground Stations"].as<QList<GroundStation>>();

   foreach (GroundStation gs, ground_stations) {
      QListWidgetItem *const item = new QListWidgetItem(ui->simGSList);
      setGSData(gs, item);
   }
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

   QList<orbitConfig> orbits = {};
   for (int i = 0; i < ui->simOrbList->count(); i++) {
      QListWidgetItem *const item  = ui->simOrbList->item(i);
      orbitConfig *const orbit_ptr = orbitHash.value(item);
      orbits.push_back(*orbit_ptr);
   }
   inp_sim["Orbits"] = orbits;

   QList<scConfig> scs = {};
   for (int i = 0; i < ui->simSCList->count(); i++) {
      QListWidgetItem *const item = ui->simSCList->item(i);
      scConfig *const sc_ptr      = scHash.value(item);
      scs.push_back(*sc_ptr);
   }
   inp_sim["SCs"] = scs;

   YAML::Node pert_conf               = inp_sim["Perturbation Models"];
   pert_conf["Atmosphere"]["Enabled"] = ui->simAeroPertEn->isChecked();
   pert_conf["Atmosphere"]["Shadows"] = ui->simAeroPertShadow->isChecked();

   pert_conf["Magnetic"]["Residual Mag Moment"] =
       ui->simResidualMagEn->isChecked();

   pert_conf["Gravitation"]["Enabled"] = ui->simGravPertEn->isChecked();
   pert_conf["Gravitation"]["Gravity Gradient"] =
       ui->simGravGradientEn->isChecked();

   pert_conf["SRP"]["Enabled"] = ui->simSRPPertEn->isChecked();
   pert_conf["SRP"]["Shadows"] = ui->simSRPPertShadow->isChecked();

   pert_conf["Thruster Plume"] = ui->simThrusterPlumeEn->isChecked();
   pert_conf["Contact"]        = ui->simContactEn->isChecked();
   pert_conf["CFD Slosh"]      = ui->simSloshEn->isChecked();
   pert_conf["Albedo on CSS"]  = ui->simAlbedoEn->isChecked();
   pert_conf["Output Env Torques to File"] = ui->simOutputTorqueEn->isChecked();

   QList<atmoConfig> atmo_models = {};
   QList<gravConfig> grav_models = {};
   QList<magConfig> mag_models   = {};

   for (auto it = worldConfigHash.begin(); it != worldConfigHash.end(); ++it) {
      if ((*it)->hasAtmo())
         atmo_models.push_back((*it)->getAtmo());

      if ((*it)->hasMag())
         mag_models.push_back((*it)->getMag());

      if ((*it)->hasGrav())
         grav_models.push_back((*it)->getGrav());
   }
   pert_conf["Atmosphere"]["Models"]  = atmo_models;
   pert_conf["Magnetic"]["Models"]    = mag_models;
   pert_conf["Gravitation"]["Models"] = grav_models;

   inp_sim["Ephem Type"]  = ephemInputs.key(ui->simEphem->currentText());
   YAML::Node celest_conf = inp_sim["Celestial Bodies"];
   for (auto it = worldConfNames.constBegin(); it != worldConfNames.constEnd();
        ++it) {
      celest_conf[it.value()] = worldConfigHash[it.key()]->getEnabled();
   }

   YAML::Node lag_sys     = inp_sim["Lagrange Systems"];
   lag_sys["Earth-Moon"]  = ui->simEarthMoonEn->isChecked();
   lag_sys["Sun-Earth"]   = ui->simSunEarthEn->isChecked();
   lag_sys["Sun-Jupiter"] = ui->simSunJupiterEn->isChecked();

   QList<GroundStation> ground_stations;

   for (int i = 0; i < ui->simGSList->count(); i++) {
      const QListWidgetItem *const item = ui->simGSList->item(i);
      ground_stations.push_back(getGSData(i, item));
   }
   inp_sim["Ground Stations"] = ground_stations;

   dsm_gui_lib::write_data(filePath, inp_sim);
}

void SIM_Menu::on_simOrbList_itemClicked(QListWidgetItem *item) {
   const bool enabled = orbitHash.value(item)->enabled();
   ui->simOrbitEn->setChecked(enabled);
}

void SIM_Menu::on_simOrbitEn_toggled(bool checked) {
   QListWidgetItem *const cur_item = ui->simOrbList->currentItem();
   if (cur_item == nullptr)
      return;
   orbitHash.value(cur_item)->setEnabled(checked);
}

void SIM_Menu::on_simSCList_itemClicked(QListWidgetItem *item) {
   scConfig *sc = scHash.value(item);
   ui->simSCEn->setChecked(sc->enabled());
   ui->simSCOrbit->setCurrentText(sc->orbit()->name());
}

void SIM_Menu::on_simSCEn_toggled(bool checked) {
   QListWidgetItem *const cur_item = ui->simSCList->currentItem();
   if (cur_item == nullptr)
      return;
   scConfig *sc = scHash.value(cur_item);
   sc->setEnabled(checked);
}

void SIM_Menu::on_simSCOrbit_currentTextChanged(const QString &arg1) {
   QListWidgetItem *const cur_item = ui->simSCList->currentItem();
   if (cur_item == nullptr)
      return;
   scConfig *sc = scHash.value(cur_item);
   sc->setOrbit(dsm_gui_lib::getObjectFromItemName(arg1, orbitHash));
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

   QListWidgetItem *newGSItem = new QListWidgetItem(ui->simGSList);
   GroundStation new_gs;
   new_gs.setLabel(newGS);
   setGSData(new_gs, newGSItem);
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

GroundStation SIM_Menu::getGSData(const long i,
                                  const QListWidgetItem *const item) {
   const bool en       = item->data(gsEnabledRole).toBool();
   const QString world = item->data(gsWorldRole).toString();
   const double lng    = item->data(gsLongRole).toDouble();
   const double lat    = item->data(gsLatRole).toDouble();
   const QString lab   = item->text();
   GroundStation gs(i, en, world, lng, lat, lab);
   return gs;
}

void SIM_Menu::setGSData(const GroundStation &gs, QListWidgetItem *const item) {
   item->setText(gs.label());
   item->setData(gsEnabledRole, gs.enabled());
   item->setData(gsWorldRole, gs.world());
   item->setData(gsLongRole, gs.longitude());
   item->setData(gsLatRole, gs.latitude());
}

void SIM_Menu::UpdateModels() {
   for (auto it = worldConfigHash.begin(); it != worldConfigHash.end(); ++it)
      (*it)->UpdateAll();
}