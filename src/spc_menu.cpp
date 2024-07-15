#include "spc_menu.h"
#include "dsm_gui_lib.h"
#include "ui_spc_menu.h"
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>
#include <QVector>

SPC_Menu::SPC_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::SPC_Menu) {
   ui->setupUi(this);
   ui->quick_tabs->setCurrentIndex(0);
   set_validators();

   new_item   = 0;
   counter3u  = 0;
   counter6u  = 0;
   counter12u = 0;
}

SPC_Menu::~SPC_Menu() {
   delete ui;
}

void SPC_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");
   QRegularExpression rx1("[^\" ]*");

   QValidator *noQuotes       = new QRegularExpressionValidator(rx, this);
   QValidator *noQuotesSpaces = new QRegularExpressionValidator(rx1, this);

   // Combo Boxes
   ui->spc_fswid->addItems(dsm_gui_lib::sortStringList(fswid_types));

   ui->spc_cur_orb_type->addItems(dsm_gui_lib::sortStringList(orbit_type));
   ui->spc_cur_pos_ref->addItems(dsm_gui_lib::sortStringList(pos_wrt_F));

   ui->spc_cur_angvel_frame1->addItems(dsm_gui_lib::sortStringList(angvel_wrt));
   ui->spc_cur_angvel_frame2->addItems(dsm_gui_lib::sortStringList(att_wrt));
   ui->spc_cur_att_param->addItems(dsm_gui_lib::sortStringList(att_params));

   ui->spc_cur_initeul_seq->addItems(dsm_gui_lib::eulerInputs);

   // Data Type Validators
   ui->spc_name->setValidator(noQuotesSpaces);
   ui->spc_label->setValidator(noQuotes);
   ui->spc_desc->setValidator(noQuotes);
   ui->spc_sprite->setValidator(noQuotes);

   ui->spc_fswsamp->setValidator(new QDoubleValidator(0, INFINITY, 5));

   QLineEdit *ninf_pinf[16] = {
       ui->spc_cur_xpos_form, ui->spc_cur_ypos_form, ui->spc_cur_zpos_form,
       ui->spc_cur_xvel_form, ui->spc_cur_yvel_form, ui->spc_cur_zvel_form,
       ui->spc_cur_angvel_1,  ui->spc_cur_angvel_2,  ui->spc_cur_angvel_3,
       ui->spc_cur_q1,        ui->spc_cur_q2,        ui->spc_cur_q3,
       ui->spc_cur_q4,        ui->spc_cur_initeul_1, ui->spc_cur_initeul_2,
       ui->spc_cur_initeul_3};

   dsm_gui_lib::set_mult_validators(ninf_pinf, 16, -INFINITY, INFINITY, 5);

   connect(ui->spc_add, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
   connect(ui->spc_remove, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
   connect(ui->spc_duplicate, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
   connect(ui->spc_load, SIGNAL(clicked(bool)), this->parent(),
           SLOT(disable_sub_menus()));
}

void SPC_Menu::receive_spcpath(QString path) {
   inout_path = path;

   file_paths_default.clear();
   file_paths.clear();
   file_path.clear();
   spc_names.clear();

   QStringList spcDefaultFiles =
       QDir(inout_path + "__default__/").entryList({"SC_*.yaml"});
   for (int i = 0; i < spcDefaultFiles.length(); i++) {
      file_paths_default.append(
          inout_path + "__default__/" +
          spcDefaultFiles[i]); // Full file path of SC file
      spc_names_default.append(spcDefaultFiles[i].chopped(5).mid(
          3)); // Everything between "SC_" and ".yaml"
   }

   QStringList spcFiles = QDir(inout_path).entryList({"SC_*.yaml"});
   if (spcFiles.length() > 0) {
      for (int i = 0; i < spcFiles.length(); i++) {
         file_paths.append(inout_path +
                           spcFiles[i]); // Full file path of SC file
         spc_names.append(spcFiles[i].chopped(5).mid(
             3)); // Everything between "SC_" and ".yaml"
      }

      for (int i = 0; i < file_paths.length(); i++) {
         file_path      = file_paths[i];
         spc_name_index = i;

         ui->spc_list->addItem(spc_names[i]);

         ui->spc_list->setCurrentRow(spc_name_index);

         receive_data();
      }
      on_spc_list_itemClicked(ui->spc_list->item(0));
      ui->spc_list->setCurrentRow(0);
   }

   if (ui->spc_list->count() == 0)
      ui->spc_conf->setEnabled(false);
   else
      ui->spc_conf->setEnabled(true);
}

void SPC_Menu::receive_apppath(QString path) {
   appPath = path;
}

void SPC_Menu::receive_pythoncmd(QString cmd) {
   pythonCmd = cmd;
}

void SPC_Menu::receive_data() {
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   spc_data.clear();
   spc_string.clear();

   /* Load Yaml File */
   cur_spc_yaml = YAML::LoadFile(file_path.toStdString());

   QStringList tmp_data;

   /* Configuration */
   tmp_data.append(cur_spc_yaml["Configuration"]["Description"].as<QString>());
   tmp_data.append(cur_spc_yaml["Configuration"]["Label"].as<QString>());
   tmp_data.append(cur_spc_yaml["Configuration"]["Sprite File"].as<QString>());
   tmp_data.append(
       cur_spc_yaml["Configuration"]["FSW Identifier"].as<QString>());
   tmp_data.append(
       cur_spc_yaml["Configuration"]["FSW Sample Time"].as<QString>());

   /* Orbit */
   tmp_data.append(cur_spc_yaml["Orbit"]["Prop Type"].as<QString>());
   tmp_data.append(cur_spc_yaml["Orbit"]["Pos Specifier"].as<QString>());

   QVector<QString> data_vector =
       cur_spc_yaml["Orbit"]["Pos wrt F"].as<QVector<QString>>();
   for (int i = 0; i < 3; i++)
      tmp_data.append(data_vector[i]);

   data_vector = cur_spc_yaml["Orbit"]["Vel wrt F"].as<QVector<QString>>();
   for (int i = 0; i < 3; i++)
      tmp_data.append(data_vector[i]);

   /* Attitude */
   tmp_data.append(cur_spc_yaml["Attitude"]["Ang Vel Frame"].as<QString>());
   tmp_data.append(
       cur_spc_yaml["Attitude"]["Att Representation"].as<QString>());
   tmp_data.append(cur_spc_yaml["Attitude"]["Att Frame"].as<QString>());
   data_vector = cur_spc_yaml["Attitude"]["Ang Vel"].as<QVector<QString>>();
   for (int i = 0; i < 3; i++)
      tmp_data.append(data_vector[i]);
   data_vector = cur_spc_yaml["Attitude"]["Quaternion"].as<QVector<QString>>();
   for (int i = 0; i < 4; i++)
      tmp_data.append(data_vector[i]);
   data_vector = cur_spc_yaml["Attitude"]["Euler Angles"]["Angles"]
                     .as<QVector<QString>>();
   for (int i = 0; i < 3; i++)
      tmp_data.append(data_vector[i]);
   tmp_data.append(
       cur_spc_yaml["Attitude"]["Euler Angles"]["Sequence"].as<QString>());

   ui->spc_list->currentItem()->setData(256, spc_names[spc_name_index]);
   ui->spc_list->currentItem()->setData(257, tmp_data);

   file.close();
}

void SPC_Menu::on_spc_apply_clicked() {
   int index = ui->spc_list->currentRow();
   if (index == -1) {
      return;
   }

   QStringList other_names = spc_names;
   other_names.removeOne(ui->spc_list->currentItem()->text());

   QString cur_name = ui->spc_name->text();
   if (other_names.contains(cur_name, Qt::CaseInsensitive)) {
      dsm_gui_lib::error_message(
          "SC \"" + cur_name +
          "\" already exists.\nSC names are NOT case sensitive.");
      return;
   }

   if (cur_name.compare(ui->spc_list->currentItem()->text()) != 0) {
      connect(this, SIGNAL(name_changed()), this->parent(),
              SLOT(disable_sub_menus()));
      emit name_changed();
      disconnect(this, SIGNAL(name_changed()), 0, 0);
   }

   file_path = file_paths[index];
   ui->spc_list->currentItem()->setText(cur_name);
   spc_names[index]  = cur_name;
   file_paths[index] = inout_path + "SC_" + cur_name + ".yaml";
   QFile::rename(file_path, file_paths[index]);
   file_path = file_paths[index];

   QStringList tmp_data = {};
   QString data_inp     = "";

   cur_spc_yaml["Name"] = ui->spc_name->text();

   /* Configuration */
   YAML::Node conf_node         = cur_spc_yaml["Configuration"];
   conf_node["Description"]     = ui->spc_desc->text();
   conf_node["Label"]           = ui->spc_label->text();
   conf_node["Sprite File"]     = ui->spc_sprite->text();
   conf_node["FSW Identifier"]  = ui->spc_fswid->currentText();
   conf_node["FSW Sample Time"] = ui->spc_fswsamp->text();

   /* Orbit */
   YAML::Node orb_node       = cur_spc_yaml["Orbit"];
   orb_node["Prop Type"]     = ui->spc_cur_orb_type->currentText();
   orb_node["Pos Specifier"] = ui->spc_cur_pos_ref->currentText();

   QVector<QString> data_vector = {ui->spc_cur_xpos_form->text(),
                                   ui->spc_cur_ypos_form->text(),
                                   ui->spc_cur_zpos_form->text()};
   orb_node["Pos wrt F"]        = data_vector;

   data_vector = {ui->spc_cur_xvel_form->text(), ui->spc_cur_yvel_form->text(),
                  ui->spc_cur_zvel_form->text()};
   orb_node["Vel wrt F"] = data_vector;

   /* Attitude */
   YAML::Node att_node            = cur_spc_yaml["Attitude"];
   att_node["Ang Vel Frame"]      = ui->spc_cur_angvel_frame1->currentText();
   att_node["Att Representation"] = ui->spc_cur_att_param->currentText();
   att_node["Att Frame"]          = ui->spc_cur_angvel_frame2->currentText();

   data_vector = {ui->spc_cur_angvel_1->text(), ui->spc_cur_angvel_2->text(),
                  ui->spc_cur_angvel_3->text()};
   att_node["Ang Vel"] = ui->spc_cur_angvel_frame2->currentText();

   data_vector = {ui->spc_cur_q1->text(), ui->spc_cur_q2->text(),
                  ui->spc_cur_q3->text(), ui->spc_cur_q4->text()};

   if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q")) {
      ui->spc_cur_initeul_1->setEnabled(false);
      ui->spc_cur_initeul_2->setEnabled(false);
      ui->spc_cur_initeul_3->setEnabled(false);

      ui->spc_cur_initeul_seq->setEnabled(false);

      ui->spc_cur_q1->setEnabled(true);
      ui->spc_cur_q2->setEnabled(true);
      ui->spc_cur_q3->setEnabled(true);
      ui->spc_cur_q4->setEnabled(true);
   } else {
      ui->spc_cur_initeul_1->setEnabled(true);
      ui->spc_cur_initeul_2->setEnabled(true);
      ui->spc_cur_initeul_3->setEnabled(true);

      ui->spc_cur_initeul_seq->setEnabled(true);

      ui->spc_cur_q1->setEnabled(false);
      ui->spc_cur_q2->setEnabled(false);
      ui->spc_cur_q3->setEnabled(false);
      ui->spc_cur_q4->setEnabled(false);
   }

   ui->spc_list->currentItem()->setData(256, cur_name);
   ui->spc_list->currentItem()->setData(257, tmp_data);

   tmp_data.clear();

   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   file.close();

   index            = file_paths.indexOf(file_path);
   global_spc_index = index;
   spc_name_index   = index;
   ui->spc_list->setCurrentRow(index);

   write_data(cur_spc_yaml);
   on_spc_list_currentTextChanged(ui->spc_list->currentItem()->text());
}

void SPC_Menu::write_data(YAML::Node inp_spc) {
   QStringList params;
   QProcess p;
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      YAML::Emitter out;
      out.SetIndent(4);
      out.SetMapFormat(YAML::EMITTER_MANIP::Block);
      out << inp_spc;
      in << out.c_str();
   }
   file.close();
   params << appPath + "/__python__/AddYAMLComments.py" << appPath << inout_path
          << "SC_" << ui->spc_label->text() << ".yaml";
   p.start(pythonCmd, params);
   p.waitForFinished(-1);
}

void SPC_Menu::on_spc_add_clicked() // Add S/C
{
   if (spc_submenu != nullptr) {
      new_item = 1;
   }

   QStringList tmp_data = {"Simple generic S/C",
                           "S/C",
                           "GenScSpriteAlpha.ppm",
                           "PROTOTYPE_FSW",
                           "0.2",
                           "FIXED",
                           "CM",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "N",
                           "A",
                           "N",
                           "0.3",
                           "0.4",
                           "0.5",
                           "0.0",
                           "0.0",
                           "0.0",
                           "1.0",
                           "60.0",
                           "40.0",
                           "20.0",
                           "213"};

   QStringList all_names;
   for (int i = 0; i < ui->spc_list->count(); i++) {
      all_names.append(ui->spc_list->item(i)->text());
   }

   QString new_name = "DEFAULT";
   if (ui->spc_list->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         QString newNameTest = new_name;
         if (i > 0)
            newNameTest += " " + QString::number(i);
         if (!all_names.contains(newNameTest, Qt::CaseInsensitive)) {
            new_name = newNameTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }

   ui->spc_list->addItem(new_name);
   QList<QListWidgetItem *> cur_items =
       ui->spc_list->findItems(new_name, Qt::MatchExactly);

   QListWidgetItem *cur_item = cur_items[0]; // there can only be one match

   ui->spc_list->setCurrentItem(cur_item);

   ui->spc_list->currentItem()->setData(256, new_name);

   ui->spc_list->currentItem()->setData(257, tmp_data);
   on_spc_list_itemClicked(ui->spc_list->currentItem());

   spc_names.append(new_name);
   file_path = inout_path + "SC_" + new_name + ".txt";
   file_paths.append(file_path);

   if (dsm_gui_lib::fileExists(inout_path + "__default__/__SCDEFAULT__.txt")) {
      QFile::copy(inout_path + "__default__/__SCDEFAULT__.txt",
                  inout_path + "SC_" + new_name + ".txt");

   } else
      QFile::copy(":/data/__default__/SC_Simple.txt",
                  inout_path + "SC_" + new_name + ".txt");
   ui->spc_list->sortItems();
   ui->spc_conf->setEnabled(true);

   new_item = 0;
   if (spc_submenu != nullptr) {
      on_spc_list_currentTextChanged(
          new_name); // click on the current item to reload submenu
   }
}

void SPC_Menu::on_spc_remove_clicked() // Remove S/C
{
   if (ui->spc_list->count() == 0)
      return;

   int remove_Item = ui->spc_list->currentRow();
   int name_index  = spc_names.indexOf(ui->spc_list->item(remove_Item)->text());

   delete ui->spc_list->item(remove_Item);

   QString file_path_delete = file_paths[name_index];

   spc_names.removeAt(name_index);
   file_paths.removeAt(name_index);

   QFile::remove(file_path_delete);

   QListWidgetItem *prev_item;
   if (ui->spc_list->count() > 0) {
      prev_item = ui->spc_list->currentItem();
      ui->spc_list->sortItems();
      on_spc_list_itemClicked(prev_item);
   } else {
      ui->spc_list->setCurrentRow(-1);
      ui->spc_conf->setEnabled(false);
   }
}

void SPC_Menu::on_spc_duplicate_clicked() // Duplicate currently selected S/C
{
   if (spc_submenu != nullptr) {
      new_item = 1;
   }

   int index = ui->spc_list->currentRow();
   if (ui->spc_list->count() == 0)
      return;

   QStringList current_item_data =
       ui->spc_list->currentItem()->data(257).toStringList();

   if (index == -1)
      return;
   QString old_spc = ui->spc_list->currentItem()->text();
   QString new_spc = old_spc + " Copy";
   for (int i = 0; i <= 30; i++) {
      QString newSCTest = new_spc;
      if (i > 0)
         newSCTest += " " + QString::number(i);
      if (!spc_names.contains(newSCTest, Qt::CaseInsensitive)) {
         new_spc = newSCTest;
         break;
      }
   }

   spc_names.append(new_spc);
   file_path = inout_path + "SC_" + new_spc + ".txt";
   QFile::copy(file_paths[index], file_path);
   file_paths.append(file_path);

   ui->spc_list->addItem(new_spc);
   QList<QListWidgetItem *> cur_items =
       ui->spc_list->findItems(new_spc, Qt::MatchExactly);

   QListWidgetItem *cur_item = cur_items[0]; // there can only be one match

   ui->spc_list->setCurrentItem(cur_item);

   ui->spc_list->currentItem()->setData(256, new_spc);
   ui->spc_list->currentItem()->setData(257,
                                        current_item_data); // set item data

   on_spc_list_itemClicked(
       ui->spc_list->item(ui->spc_list->count() - 1)); // click the item

   if (ui->spc_list->count() > 0) {
      on_spc_list_itemClicked(ui->spc_list->item(ui->spc_list->count() - 2));
      ui->spc_list->setCurrentRow(ui->spc_list->count() - 2);
   }
   ui->spc_list->sortItems();

   new_item = 0;
   if (spc_submenu != nullptr)
      on_spc_list_currentTextChanged(
          new_spc); // click on the current item to reload submenu
}

void SPC_Menu::on_spc_load_clicked() // Load default S/C
{
   int response = dsm_gui_lib::warning_message("Load All Default S/C?");
   if (response == QMessageBox::Ok) {
      if (spc_submenu != nullptr) {
         new_item = 1;
         on_SPC_Menu_rejected();
      }

      int num_items = ui->spc_list->count();
      ui->spc_list->clear();
      for (int i = 0; i < num_items; i++) {
         file_path = file_paths[i];

         QFile::remove(file_path);
      }

      file_paths.clear();
      spc_names.clear();

      for (int i = 0; i < spc_names_default.length(); i++) {
         file_path = inout_path + "SC_" + spc_names_default[i] + ".txt";
         file_paths.append(file_path);
         spc_names.append(spc_names_default[i]);

         spc_name_index = i;

         QFile::copy(file_paths_default[i], file_path);
         ui->spc_list->addItem(spc_names_default[i]);

         ui->spc_list->setCurrentRow(ui->spc_list->count() - 1);

         receive_data();
      }
      ui->spc_list->setCurrentRow(0);
      on_spc_list_itemClicked(ui->spc_list->item(0));
      if (new_item == 1) {
         on_spc_conf_clicked();
         new_item = 0;
      }
   } else
      return;
}

void SPC_Menu::on_spc_save_clicked() {
   if (ui->spc_list->count() == 0)
      return;
   int response =
       dsm_gui_lib::warning_message("Save All Current S/C to Defaults?");
   if (response == QMessageBox::Ok) {

      if (spc_submenu != nullptr) {
         new_item = 1;
         on_SPC_Menu_rejected();
      }

      for (int i = 0; i < file_paths_default.length(); i++)
         QFile::remove(file_paths_default[i]);

      spc_names_default.clear();
      file_paths_default.clear();

      for (int i = 0; i < ui->spc_list->count(); i++) {
         spc_names_default.append(spc_names[i]);
         file_paths_default.append(inout_path + "__default__/" + "SC_" +
                                   spc_names[i] + ".txt");

         QFile::copy(file_paths[i], file_paths_default[i]);
      }
      if (new_item == 1) {
         on_spc_conf_clicked();
         new_item = 0;
      }
   }
}

void SPC_Menu::on_spc_close_clicked() {
   SPC_Menu::close();
}

void SPC_Menu::on_spc_conf_clicked() {
   on_spc_apply_clicked();
   receive_data();

   if (spc_submenu == nullptr) {
      spc_submenu = new SPC_submenu(this);
      spc_submenu->setModal(false);
      spc_submenu->show();

      connect(this, SIGNAL(send_data(QString, QString)), spc_submenu,
              SLOT(receive_spc_sm_path(QString, QString)));
      emit send_data(ui->spc_name->text(), inout_path);
      disconnect(this, SIGNAL(send_data(QString, QString)), 0, 0);

      spc_submenu->move(SPC_Menu::geometry().x() + spc_submenu->width(),
                        SPC_Menu::geometry().y() - spc_submenu->height() / 2);
   } else {
      spc_submenu->show();
      spc_submenu->raise();
   }
}

void SPC_Menu::on_spc_list_itemClicked(QListWidgetItem *item) {

   QStringList current_data = item->data(257).toStringList();

   ui->spc_name->setText(item->data(256).toString());

   ui->spc_desc->setText(current_data[0]);

   ui->spc_label->setText(current_data[1]);

   ui->spc_sprite->setText(current_data[2]);

   setQComboBox(ui->spc_fswid, current_data[3]);

   ui->spc_fswsamp->setText(current_data[4]);

   setQComboBox(ui->spc_cur_orb_type, current_data[5]);

   setQComboBox(ui->spc_cur_pos_ref, current_data[6]);

   ui->spc_cur_xpos_form->setText(current_data[7]);
   ui->spc_cur_ypos_form->setText(current_data[8]);
   ui->spc_cur_zpos_form->setText(current_data[9]);

   ui->spc_cur_xvel_form->setText(current_data[10]);
   ui->spc_cur_yvel_form->setText(current_data[11]);
   ui->spc_cur_zvel_form->setText(current_data[12]);

   setQComboBox(ui->spc_cur_angvel_frame1, current_data[13]);
   setQComboBox(ui->spc_cur_att_param, current_data[14]);
   setQComboBox(ui->spc_cur_angvel_frame2, current_data[15]);

   ui->spc_cur_angvel_1->setText(current_data[16]);
   ui->spc_cur_angvel_2->setText(current_data[17]);
   ui->spc_cur_angvel_3->setText(current_data[18]);

   ui->spc_cur_q1->setText(current_data[19]);
   ui->spc_cur_q2->setText(current_data[20]);
   ui->spc_cur_q3->setText(current_data[21]);
   ui->spc_cur_q4->setText(current_data[22]);

   ui->spc_cur_initeul_1->setText(current_data[23]);
   ui->spc_cur_initeul_2->setText(current_data[24]);
   ui->spc_cur_initeul_3->setText(current_data[25]);
   setQComboBox(ui->spc_cur_initeul_seq, current_data[26]);
}

void SPC_Menu::on_spc_list_itemActivated(QListWidgetItem *item) {
   ui->spc_conf->setEnabled(true);
}

void SPC_Menu::setQComboBox(QComboBox *comboBox, QString string) {
   comboBox->setCurrentIndex(comboBox->findText(string));
}

void SPC_Menu::on_spc_cur_att_param_currentTextChanged(const QString &arg1) {
   if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q")) {
      ui->spc_cur_initeul_1->setEnabled(false);
      ui->spc_cur_initeul_2->setEnabled(false);
      ui->spc_cur_initeul_3->setEnabled(false);

      ui->spc_cur_initeul_seq->setEnabled(false);

      ui->spc_cur_q1->setEnabled(true);
      ui->spc_cur_q2->setEnabled(true);
      ui->spc_cur_q3->setEnabled(true);
      ui->spc_cur_q4->setEnabled(true);
   } else {
      ui->spc_cur_initeul_1->setEnabled(true);
      ui->spc_cur_initeul_2->setEnabled(true);
      ui->spc_cur_initeul_3->setEnabled(true);

      ui->spc_cur_initeul_seq->setEnabled(true);

      ui->spc_cur_q1->setEnabled(false);
      ui->spc_cur_q2->setEnabled(false);
      ui->spc_cur_q3->setEnabled(false);
      ui->spc_cur_q4->setEnabled(false);
   }
}

void SPC_Menu::on_spc_list_currentTextChanged(const QString &currentText) {
   if (spc_submenu != nullptr && new_item == 0) {
      connect(this, SIGNAL(send_data(QString, QString)), spc_submenu,
              SLOT(receive_spc_sm_path(QString, QString)));
      emit send_data(currentText, inout_path);
      disconnect(this, SIGNAL(send_data(QString, QString)), 0, 0);
   }
}

void SPC_Menu::on_SPC_Menu_rejected() {
   if (spc_submenu != nullptr) {
      connect(this, SIGNAL(send_data(QString, QString)), spc_submenu,
              SLOT(destroy_submenu(QString, QString)));
      emit send_data("Done", "");
      disconnect(this, SIGNAL(send_data(QString, QString)), 0, 0);
   }
}

void SPC_Menu::proc_add_template(QString sc_template_name, long counter) {
   int response =
       dsm_gui_lib::warning_message("Load " + sc_template_name + " template?");
   if (response == QMessageBox::Ok) {
      on_spc_remove_clicked();
      load_specific_file(sc_template_name, counter);

   } else
      return;
}

void SPC_Menu::load_1SC_default(QString sc_string) {
   on_spc_remove_clicked();
   on_spc_add_clicked();
}

void SPC_Menu::on_sc3u_clicked() {
   proc_add_template("3U_CubeSat", counter3u);
   counter3u++;
}

void SPC_Menu::on_sc6u_clicked() {
   proc_add_template("6U_CubeSat", counter3u);
   counter6u++;
}

void SPC_Menu::on_sc12u_clicked() {
   proc_add_template("12U_CubeSat", counter3u);
   counter12u++;
}

void SPC_Menu::load_specific_file(QString load_sc_name, long counter) {
   if (spc_submenu != nullptr) {
      new_item = 1;
   }

   QStringList tmp_data = {"Simple generic S/C",
                           "S/C",
                           "GenScSpriteAlpha.ppm",
                           "PROTOTYPE_FSW",
                           "0.2",
                           "FIXED",
                           "CM",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "0.0",
                           "N",
                           "A",
                           "N",
                           "0.3",
                           "0.4",
                           "0.5",
                           "0.0",
                           "0.0",
                           "0.0",
                           "1.0",
                           "60.0",
                           "40.0",
                           "20.0",
                           "213"};

   QStringList all_names;
   for (int i = 0; i < ui->spc_list->count(); i++) {
      all_names.append(ui->spc_list->item(i)->text());
   }

   QString new_name;

   if (counter >= 0)
      new_name = load_sc_name + QString::number(counter);
   else
      new_name = load_sc_name;

   if (ui->spc_list->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         QString newNameTest = new_name;
         if (i > 0)
            newNameTest += " " + QString::number(i);
         if (!all_names.contains(newNameTest, Qt::CaseInsensitive)) {
            new_name = newNameTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }

   ui->spc_list->addItem(new_name);
   QList<QListWidgetItem *> cur_items =
       ui->spc_list->findItems(new_name, Qt::MatchExactly);

   QListWidgetItem *cur_item = cur_items[0]; // there can only be one match

   ui->spc_list->setCurrentItem(cur_item);

   ui->spc_list->currentItem()->setData(256, new_name);

   ui->spc_list->currentItem()->setData(257, tmp_data);
   on_spc_list_itemClicked(ui->spc_list->currentItem());

   spc_names.append(new_name);
   file_path = inout_path + "SC_" + new_name + ".txt";
   file_paths.append(file_path);

   QFile::copy(":/data/__default__/SC_" + load_sc_name + ".txt",
               inout_path + "SC_" + new_name + ".txt");

   ui->spc_list->sortItems();
   ui->spc_conf->setEnabled(true);

   new_item = 0;
   if (spc_submenu != nullptr)
      on_spc_list_currentTextChanged(
          new_name); // click on the current item to reload submenu
}
