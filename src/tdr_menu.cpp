#include "tdr_menu.h"
#include "ui_tdr_menu.h"

TDR_Menu::TDR_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::TDR_Menu) {
   ui->setupUi(this);
   set_validators();
}

TDR_Menu::~TDR_Menu() {
   delete ui;
}

void TDR_Menu::set_validators() {
   const QRegularExpression rx("[^\"]*");
   tdrsEnabled = {ui->tdrs01, ui->tdrs02, ui->tdrs03, ui->tdrs04, ui->tdrs05,
                  ui->tdrs06, ui->tdrs07, ui->tdrs08, ui->tdrs09, ui->tdrs10};
   tdrsNames   = {ui->tdrs01_name, ui->tdrs02_name, ui->tdrs03_name,
                  ui->tdrs04_name, ui->tdrs05_name, ui->tdrs06_name,
                  ui->tdrs07_name, ui->tdrs08_name, ui->tdrs09_name,
                  ui->tdrs10_name};

   for (int i = 0; i < tdrsNames.length(); i++)
      tdrsNames[i]->setValidator(new QRegularExpressionValidator(rx));
}

void TDR_Menu::receive_tdrpath(QString path) {
   inout_path = path;
   file_path  = path + "Inp_TDRS.yaml";
   apply_data();
}

void TDR_Menu::write_data(YAML::Node yaml) {
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

void TDR_Menu::apply_data() {
   YAML::Node tdrs_yaml  = YAML::LoadFile(file_path.toStdString());
   QList<TDRS> tdrs_list = tdrs_yaml["TDRSs"].as<QList<TDRS>>();

   for (int i = 0; i < tdrsEnabled.length(); i++) {
      TDRS tdrs_data = tdrs_list[i];
      tdrsEnabled[i]->setChecked(tdrs_data.exists());
      tdrsNames[i]->setText(tdrs_data.label());
   }
}

void TDR_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite TDRS file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_TDRS.yaml");
      QFile::copy(inout_path + "__default__/Inp_TDRS.yaml",
                  inout_path + "Inp_TDRS.yaml");
      apply_data();
   } else {
      return;
   }
}

void TDR_Menu::on_savedefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Default TDRS file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_TDRS.yaml");
      QFile::copy(inout_path + "Inp_TDRS.yaml",
                  inout_path + "__default__/Inp_TDRS.yaml");
      apply_data();
   } else {
      return;
   }
}

void TDR_Menu::on_closeButton_clicked() {
   TDR_Menu::close();
}

void TDR_Menu::on_applyButton_clicked() {
   QList<TDRS> tdrs_list;

   for (int i = 0; i < tdrsEnabled.length(); i++) {
      TDRS tdrs_data(tdrsNames[i]->text(), i + 1, tdrsEnabled[i]->isChecked());
      tdrs_list.append(tdrs_data);
   }

   YAML::Node tdrs_yaml(YAML::NodeType::Sequence);
   tdrs_yaml["TDRSs"] = tdrs_list;

   write_data(tdrs_yaml);
}
