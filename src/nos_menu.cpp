#include "nos_menu.h"
#include "ui_nos_menu.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

NOS_Menu::NOS_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::NOS_Menu) {
   ui->setupUi(this);
}

NOS_Menu::~NOS_Menu() {
   delete ui;
}

void NOS_Menu::receive_nospath(QString path) {
   inout_path = path;
   file_path  = path + "Inp_NOS3.yaml";
   apply_data();
}

void NOS_Menu::write_data(YAML::Node yaml) {
   QFile::remove(file_path);
   QFile file(file_path);
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

void NOS_Menu::apply_data() {
   nos3_yaml = YAML::LoadFile(file_path.toStdString());

   YAML::Node config_node = nos3_yaml["Configuration"];

   ui->time_bus->setText(config_node["Bus"].as<QString>());
   ui->time_string->setText(config_node["Connection String"].as<QString>());
}

void NOS_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite NOS3 file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_NOS3.yaml");
      QFile::copy(inout_path + "__default__/Inp_NOS3.yaml",
                  inout_path + "Inp_NOS3.yaml");
      apply_data();
   } else {
      return;
   }
}

void NOS_Menu::on_savedefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Default NOS3 file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_NOS3.yaml");
      QFile::copy(inout_path + "Inp_NOS3.yaml",
                  inout_path + "__default__/Inp_NOS3.yaml");
      apply_data();
   } else {
      return;
   }
}

void NOS_Menu::on_closeButton_clicked() {
   NOS_Menu::close();
}

void NOS_Menu::on_applyButton_clicked() {
   YAML::Node config_node           = nos3_yaml["Configuration"];
   config_node["Bus"]               = ui->time_bus->text();
   config_node["Connection String"] = ui->time_string->text();
   write_data(nos3_yaml);
}
