#include "grh_menu.h"
#include "ui_grh_menu.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

GRH_Menu::GRH_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::GRH_Menu) {
   ui->setupUi(this);
   set_validators();
}

GRH_Menu::~GRH_Menu() {
   delete ui;
}

void GRH_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");

   ui->star_catalog->setValidator(new QRegularExpressionValidator(rx));
   ui->gl_output->setValidator(new QDoubleValidator);
   ui->povMode->addItems(dsm_gui_lib::sortStringList(povMode_inputs.values()));
   ui->hostType->addItems(
       dsm_gui_lib::sortStringList(hosttarget_inputs.values()));
   ui->hostBDY->setMinimum(0);
   ui->hostBDY->setMaximum(INFINITY);
   ui->hostFrame->addItems(dsm_gui_lib::sortStringList(frame_inputs.values()));
   ui->targetType->addItems(
       dsm_gui_lib::sortStringList(hosttarget_inputs.values()));
   ui->targetBDY->setMinimum(0);
   ui->targetBDY->setMaximum(INFINITY);
   ui->targetFrame->addItems(
       dsm_gui_lib::sortStringList(frame_inputs.values()));
   ui->boresightaxis->addItems(
       dsm_gui_lib::sortStringList(axis_inputs.values()));

   ui->camtitle->setValidator(new QRegularExpressionValidator(rx));
   ui->show_naxis_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_laxis_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_faxis_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_baxis_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_ngrid_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_lgrid_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_fgrid_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_bgrid_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_galgrid_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_fov_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_proxops_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_tdrs_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_shdws_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_astro_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_truth_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_fsw_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_milky_name->setValidator(new QRegularExpressionValidator(rx));
   ui->show_fermi_name->setValidator(new QRegularExpressionValidator(rx));

   ui->maptitle->setValidator(new QRegularExpressionValidator(rx));
   ui->showclock_name->setValidator(new QRegularExpressionValidator(rx));
   ui->showtlmclock_name->setValidator(new QRegularExpressionValidator(rx));
   ui->showcredits_name->setValidator(new QRegularExpressionValidator(rx));
   ui->shownight_name->setValidator(new QRegularExpressionValidator(rx));

   ui->povrange->setValidator(new QDoubleValidator);
   ui->povangle->setValidator(new QDoubleValidator);
   ui->povinhost_x->setValidator(new QDoubleValidator);
   ui->povinhost_y->setValidator(new QDoubleValidator);
   ui->povinhost_z->setValidator(new QDoubleValidator);
   ui->povview->addItems(dsm_gui_lib::sortStringList(view_inputs.values()));
   ui->camwidth->setValidator(new QIntValidator);
   ui->camheight->setValidator(new QIntValidator);
   ui->mousescale->setValidator(new QDoubleValidator);
   ui->mapwidth->setValidator(new QIntValidator);
   ui->mapheight->setValidator(new QIntValidator);

   ui_pov              = {ui->povinhost_x, ui->povinhost_y, ui->povinhost_z};
   cam_show_fields     = {"N Axes",         "L Axes",       "F Axes",
                          "B Axes",         "N Grid",       "L Grid",
                          "F Grid",         "B Grid",       "G Grid",
                          "Fields of View", "Prox Ops",     "TDRS Satellites",
                          "Shadows",        "Astro Labels", "Truth Vectors",
                          "FSW Vectors",    "Milky Way",    "Fermi Sky"};
   cam_show_checkboxes = {
       {"N Axes", ui->showNAxis},        {"L Axes", ui->showLAxis},
       {"F Axes", ui->showFAxis},        {"B Axes", ui->showBAxis},
       {"N Grid", ui->showNGrid},        {"L Grid", ui->showLGrid},
       {"F Grid", ui->showFGrid},        {"B Grid", ui->showBGrid},
       {"G Grid", ui->showGalGrid},      {"Fields of View", ui->showFOV},
       {"Prox Ops", ui->showProxOps},    {"TDRS Satellites", ui->showTDRS},
       {"Shadows", ui->showShadows},     {"Astro Labels", ui->showAstro},
       {"Truth Vectors", ui->showTruth}, {"FSW Vectors", ui->showFSW},
       {"Milky Way", ui->showMilky},     {"Fermi Sky", ui->showFermi}};
   cam_show_names = {{"N Axes", ui->show_naxis_name},
                     {"L Axes", ui->show_laxis_name},
                     {"F Axes", ui->show_faxis_name},
                     {"B Axes", ui->show_baxis_name},
                     {"N Grid", ui->show_ngrid_name},
                     {"L Grid", ui->show_lgrid_name},
                     {"F Grid", ui->show_fgrid_name},
                     {"B Grid", ui->show_bgrid_name},
                     {"G Grid", ui->show_galgrid_name},
                     {"Fields of View", ui->show_fov_name},
                     {"Prox Ops", ui->show_proxops_name},
                     {"TDRS Satellites", ui->show_tdrs_name},
                     {"Shadows", ui->show_shdws_name},
                     {"Astro Labels", ui->show_astro_name},
                     {"Truth Vectors", ui->show_truth_name},
                     {"FSW Vectors", ui->show_fsw_name},
                     {"Milky Way", ui->show_milky_name},
                     {"Fermi Sky", ui->show_fermi_name}};

   map_show_fields     = {"Clock", "Tlm Clock", "Credits", "Night"};
   map_show_checkboxes = {{"Clock", ui->showClock},
                          {"Tlm Clock", ui->showTlmClock},
                          {"Credits", ui->showCredits},
                          {"Night", ui->showNight}};
   map_show_names      = {{"Clock", ui->showclock_name},
                          {"Tlm Clock", ui->showtlmclock_name},
                          {"Credits", ui->showcredits_name},
                          {"Night", ui->shownight_name}};

   const_show_fields     = {"Major", "Zodiac", "Minor"};
   const_show_checkboxes = {{"Major", ui->showMajorConst},
                            {"Zodiac", ui->showZodiac},
                            {"Minor", ui->showMinorConst}};
}

void GRH_Menu::receive_grhpath(QString path) {
   inout_path    = path;
   graphics_path = path + "Inp_Graphics.yaml";
   receive_data();
   apply_data();
}

void GRH_Menu::receive_data() {

   grh_file_yaml = YAML::LoadFile(graphics_path.toStdString());

   QFile file(graphics_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   QStringList scFiles = QDir(inout_path).entryList({"SC_*"});
   QStringList scNames;
   for (int i = 0; i < scFiles.length(); i++)
      scNames.append(scFiles[i].chopped(4).mid(3));
   QSet<QString> scNamesSet(scNames.begin(), scNames.end());

   const YAML::Node sim_yaml =
       YAML::LoadFile((inout_path + "Inp_Sim.yaml").toStdString());
   const YAML::Node scs = sim_yaml["SCs"];

   QStringList simSCNames;
   QHash<QString, int> simSCNum;
   for (YAML::const_iterator it = scs.begin(); it != scs.end(); ++it) {
      QMap<QString, QString> scConf = it->as<QMap<QString, QString>>();
      QString label                 = scConf["Name"].mid(3);
      simSCNames.append(label);
      simSCNum.insert(label, std::distance(scs.begin(), it));
   }
   QSet<QString> simSCNamesSet(simSCNames.begin(), simSCNames.end());

   scNums.clear();
   QStringList spacecrafts = scNamesSet.intersect(simSCNamesSet).values();
   for (int i = 0; i < spacecrafts.count(); i++)
      scNums.insert(spacecrafts[i], simSCNum[spacecrafts[i]]);

   ui->hostSC->clear();
   ui->hostSC->addItems(dsm_gui_lib::sortStringList(scNums.keys()));

   ui->targetSC->clear();
   ui->targetSC->addItems(dsm_gui_lib::sortStringList(scNums.keys()));
}

void GRH_Menu::write_data(YAML::Node yaml) {
   QStringList params;

   QFile::remove(graphics_path);
   QFile file(graphics_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      YAML::Emitter out;
      out.SetIndent(2);
      out.SetMapFormat(YAML::EMITTER_MANIP::Block);
      out << yaml;
      in << out.c_str();
   }
   file.close();
}

void GRH_Menu::apply_data() {

   const YAML::Node config_yaml = grh_file_yaml["Configuration"];
   ui->gl_output->setText(config_yaml["Output Interval"].as<QString>());
   ui->star_catalog->setText(config_yaml["Star Catalog File"].as<QString>());
   ui->mapWindow->setChecked(config_yaml["Map Exists"].as<bool>());
   ui->orreyWindow->setChecked(config_yaml["Orrery Exists"].as<bool>());
   ui->sphereWindow->setChecked(config_yaml["Unit Sphere Exists"].as<bool>());
   ui->pause->setChecked(config_yaml["Pause on Startup"].as<bool>());

   const YAML::Node pov_yaml = grh_file_yaml["POV"];
   dsm_gui_lib::setQComboBox(ui->povMode,
                             povMode_inputs[pov_yaml["Mode"].as<QString>()]);

   const HostTarget host = pov_yaml["Host"].as<HostTarget>();
   dsm_gui_lib::setQComboBox(ui->hostType, hosttarget_inputs[host.type()]);
   dsm_gui_lib::setQComboBox(ui->hostSC, scNums.key(host.sc()));
   ui->hostBDY->setValue(host.body());
   dsm_gui_lib::setQComboBox(ui->hostFrame, frame_inputs[host.frame()]);

   const HostTarget target = pov_yaml["Target"].as<HostTarget>();
   dsm_gui_lib::setQComboBox(ui->targetType, hosttarget_inputs[target.type()]);
   dsm_gui_lib::setQComboBox(ui->targetSC, scNums.key(target.sc()));
   ui->targetBDY->setValue(target.body());
   dsm_gui_lib::setQComboBox(ui->targetFrame, frame_inputs[target.frame()]);

   dsm_gui_lib::setQComboBox(
       ui->boresightaxis,
       axis_inputs[pov_yaml["Boresight Axis"].as<QString>()]);
   dsm_gui_lib::setQComboBox(ui->upaxis,
                             axis_inputs[pov_yaml["Up Axis"].as<QString>()]);
   ui->povrange->setText(pov_yaml["POV Range"].as<QString>());
   ui->povangle->setText(pov_yaml["POV Vertical Angle"].as<QString>());
   QVector3D pov = pov_yaml["POV Host Position"].as<QVector3D>();
   for (int i = 0; i < 3; i++)
      ui_pov[i]->setText(QString::number(pov[i]));

   dsm_gui_lib::setQComboBox(ui->povview,
                             view_inputs[pov_yaml["POV View"].as<QString>()]);

   const YAML::Node cam_yaml = grh_file_yaml["Cam"];
   ui->camtitle->setText(cam_yaml["Title"].as<QString>());
   const YAML::Node cam_dim_yaml = cam_yaml["Dimensions"];
   ui->camwidth->setText(cam_dim_yaml["Width"].as<QString>());
   ui->camheight->setText(cam_dim_yaml["Height"].as<QString>());
   ui->mousescale->setText(
       QString::number(cam_yaml["Mouse Scale Factor"].as<double>()));
   ui->displaygamma->setValue(cam_yaml["Gamma Exponent"].as<double>());

   const YAML::Node cam_show_yaml = cam_yaml["Cam Show"];
   foreach (auto field, cam_show_fields) {
      const YAML::Node node = cam_show_yaml[field];
      bool is_checked       = false;
      QString name          = field;
      if (!node.IsNull()) {
         is_checked = node["Show"].as<bool>();
         name       = node["Label"].as<QString>();
      }
      cam_show_checkboxes[field]->setChecked(is_checked);
      cam_show_names[field]->setText(name);
   }

   const YAML::Node map_yaml = grh_file_yaml["Map"];
   ui->maptitle->setText(map_yaml["Title"].as<QString>());
   const YAML::Node map_dim_yaml = map_yaml["Dimensions"];
   ui->mapwidth->setText(map_dim_yaml["Width"].as<QString>());
   ui->mapheight->setText(map_dim_yaml["Height"].as<QString>());

   const YAML::Node map_show_yaml = map_yaml["Map Show"];
   foreach (auto field, map_show_fields) {
      const YAML::Node node = map_show_yaml[field];
      bool is_checked       = false;
      QString name          = field;
      if (!node.IsNull()) {
         is_checked = node["Show"].as<bool>();
         name       = node["Label"].as<QString>();
      }
      map_show_checkboxes[field]->setChecked(is_checked);
      map_show_names[field]->setText(name);
   }

   const YAML::Node const_show_yaml = grh_file_yaml["Constellations Show"];
   foreach (auto field, const_show_fields) {
      const YAML::Node node = const_show_yaml[field];
      bool is_checked       = false;
      if (!node.IsNull())
         is_checked = node["Show"].as<bool>();
      const_show_checkboxes[field]->setChecked(is_checked);
   }
}

void GRH_Menu::on_boresightaxis_currentTextChanged(const QString &arg1) {
   upaxis_inputs.clear();
   upaxis_inputs = axis_inputs;

   QString axis           = arg1.right(1);
   QStringList keyList    = dsm_gui_lib::sortStringList(axis_inputs.keys());
   QStringList removeList = keyList.filter(axis, Qt::CaseInsensitive);

   upaxis_inputs.take(removeList[0]);
   upaxis_inputs.take(removeList[1]);

   ui->upaxis->clear();
   ui->upaxis->addItems(dsm_gui_lib::sortStringList(upaxis_inputs.values()));
}

void GRH_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Graphics file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_Graphics.yaml");
      QFile::copy(inout_path + "__default__/Inp_Graphics.yaml",
                  inout_path + "Inp_Graphics.yaml");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void GRH_Menu::on_savedefaultButton_clicked() {
   int response =
       dsm_gui_lib::warning_message("Overwrite Default Graphics file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_Graphics.yaml");
      QFile::copy(inout_path + "Inp_Graphics.yaml",
                  inout_path + "__default__/Inp_Graphics.yaml");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void GRH_Menu::on_closeButton_clicked() {
   GRH_Menu::close();
}

void GRH_Menu::on_applyButton_clicked() {

   YAML::Node config_yaml            = grh_file_yaml["Configuration"];
   config_yaml["Name"]               = "Inp_Graphics.yaml";
   config_yaml["Output Interval"]    = ui->gl_output->text().toDouble();
   config_yaml["Star Catalog File"]  = ui->star_catalog->text();
   config_yaml["Map Exists"]         = ui->mapWindow->isChecked();
   config_yaml["Orrery Exists"]      = ui->orreyWindow->isChecked();
   config_yaml["Unit Sphere Exists"] = ui->sphereWindow->isChecked();
   config_yaml["Pause on Startup"]   = ui->pause->isChecked();

   YAML::Node pov_yaml = grh_file_yaml["POV"];
   pov_yaml["Mode"]    = povMode_inputs.key(ui->povMode->currentText());
   HostTarget host;
   host.setType(hosttarget_inputs.key(ui->hostType->currentText()));
   host.setSC(scNums[ui->hostSC->currentText()], ui->hostBDY->value());
   host.setFrame(frame_inputs.key(ui->hostFrame->currentText()));
   pov_yaml["Host"] = host;

   HostTarget target;
   target.setType(hosttarget_inputs.key(ui->targetType->currentText()));
   target.setSC(scNums[ui->hostSC->currentText()], ui->targetBDY->value());
   target.setFrame(frame_inputs.key(ui->targetFrame->currentText()));
   pov_yaml["Target"] = target;

   pov_yaml["Boresight Axis"] =
       axis_inputs.key(ui->boresightaxis->currentText());
   pov_yaml["Up Axis"]            = axis_inputs.key(ui->upaxis->currentText());
   pov_yaml["POV Range"]          = ui->povrange->text().toDouble();
   pov_yaml["POV Vertical Angle"] = ui->povangle->text().toDouble();
   QVector3D pos;
   for (int i = 0; i < 3; i++)
      pos[i] = ui_pov[i]->text().toDouble();
   pov_yaml["POV Host Position"] = pos;
   pov_yaml["POV View"]          = view_inputs.key(ui->povview->currentText());

   YAML::Node cam_yaml            = grh_file_yaml["Cam"];
   cam_yaml["Title"]              = ui->camtitle->text();
   YAML::Node cam_dim_yaml        = cam_yaml["Dimensions"];
   cam_dim_yaml["Width"]          = ui->camwidth->text().toInt();
   cam_dim_yaml["Height"]         = ui->camheight->text().toInt();
   cam_yaml["Mouse Scale Factor"] = ui->mousescale->text().toDouble();
   cam_yaml["Gamma Exponent"]     = ui->displaygamma->value();

   YAML::Node cam_show_yaml = cam_yaml["Cam Show"];
   foreach (auto field, cam_show_fields) {
      YAML::Node node = cam_show_yaml[field];
      node["Show"]    = cam_show_checkboxes[field]->isChecked();
      node["Label"]   = cam_show_names[field]->text();
   }

   YAML::Node map_yaml     = grh_file_yaml["Map"];
   map_yaml["Title"]       = ui->camtitle->text();
   YAML::Node map_dim_yaml = map_yaml["Dimensions"];
   map_dim_yaml["Width"]   = ui->mapwidth->text().toInt();
   map_dim_yaml["Height"]  = ui->mapheight->text().toInt();

   YAML::Node map_show_yaml = map_yaml["Map Show"];
   foreach (auto field, map_show_fields) {
      YAML::Node node = map_show_yaml[field];
      node["Show"]    = map_show_checkboxes[field]->isChecked();
      node["Label"]   = map_show_names[field]->text();
   }

   YAML::Node const_show_yaml = grh_file_yaml["Constellations Show"];
   foreach (auto field, const_show_fields) {
      YAML::Node node = const_show_yaml[field];
      node["Show"]    = const_show_checkboxes[field]->isChecked();
   }

   write_data(grh_file_yaml);
}

void GRH_Menu::on_hostSC_currentTextChanged(const QString &arg1) {
   QStringList scFileNames =
       QDir(inout_path).entryList({"SC_" + arg1 + ".yaml"});
   if (scFileNames.isEmpty())
      return;

   ui->hostBDY->setMaximum(
       dsm_gui_lib::get_sc_nitems(inout_path, arg1,
                                  dsm_gui_lib::scSectionType::BODY) -
       1);
}

void GRH_Menu::on_targetSC_currentTextChanged(const QString &arg1) {
   QStringList scFileNames =
       QDir(inout_path).entryList({"SC_" + arg1 + ".yaml"});
   if (scFileNames.isEmpty())
      return;

   ui->targetBDY->setMaximum(
       dsm_gui_lib::get_sc_nitems(inout_path, arg1,
                                  dsm_gui_lib::scSectionType::BODY) -
       1);
}
