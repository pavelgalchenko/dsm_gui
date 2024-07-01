#include "spc_menu.h"
#include "dsm_gui_lib.h"
#include "ui_spc_menu.h"
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

SPC_Menu::SPC_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::SPC_Menu) {
   ui->setupUi(this);
   ui->quick_tabs->setCurrentIndex(0);
   set_validators();

   new_item = 0;
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

   QStringList spcDefaultFiles =
       QDir(inout_path + "__default__/").entryList({"SC_*"});
   for (int i = 0; i < spcDefaultFiles.length(); i++) {
      file_paths_default.append(
          inout_path + "__default__/" +
          spcDefaultFiles[i]); // Full file path of SC file
      spc_names_default.append(spcDefaultFiles[i].chopped(4).mid(
          3)); // Everything between "SC_" and ".txt"
   }

   QStringList spcFiles = QDir(inout_path).entryList({"SC_*"});
   if (spcFiles.length() > 0) {
      for (int i = 0; i < spcFiles.length(); i++) {
         file_paths.append(inout_path +
                           spcFiles[i]); // Full file path of SC file
         spc_names.append(spcFiles[i].chopped(4).mid(
             3)); // Everything between "SC_" and ".txt"
      }

      ui->spc_list->clear();
      for (int i = 0; i < file_paths.length(); i++) {
         file_path      = file_paths[i];
         spc_name_index = i;

         ui->spc_list->addItem(spc_names[i]);

         ui->spc_list->setCurrentRow(spc_name_index);

         receive_data();
         apply_data();
      }

      on_spc_list_itemClicked(ui->spc_list->item(0));
      ui->spc_list->setCurrentRow(0);
   }

   if (ui->spc_list->count() == 0)
      ui->spc_conf->setEnabled(false);
   else
      ui->spc_conf->setEnabled(true);
}

void SPC_Menu::receive_data() {
   // Daniel's regex from ORB_Menu
   // Return everything up to and including ! (exclamation point)
   static QRegularExpression rx1("(.*?)!");

   // Return everything between a set of "  " (quotation marks)
   static QRegularExpression rx2("\"(.*?)\"");

   // If the line does NOT start with an alphanumeric character or " (single
   // quotation), then return the line as first group. Otherwise return
   // everything after ! (exclamation point) as second group
   static QRegularExpression rx3(
       "(?:(?=^[^[:alnum:]|\"])([^[:alnum:]|\"].*)|(!.*))");

   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   spc_data.clear();
   spc_string.clear();

   QTextStream in(&file);
   while (!in.atEnd()) {
      QString line                   = in.readLine();
      QRegularExpressionMatch match1 = rx1.match(line);
      spc_data.append(
          match1.captured(1)); // index 0 includes ! character, index 1 does not

      QRegularExpressionMatch match2 = rx2.match(line);
      spc_string.append(match2.captured(
          1)); // index 0 includes "" characters, index 1 does not

      //        line.append("\n");
      QRegularExpressionMatch match3 = rx3.match(line);
      if (match3.hasMatch()) {
         QString capture = match3.captured(1);
         if (!capture.isEmpty())
            capture += "\n";
         spc_file_headers.append(capture);
         capture = match3.captured(2);
         if (!capture.isEmpty())
            capture += "\n";
         spc_file_descrip.append(capture);
      }
   }
   file.close();
}

void SPC_Menu::apply_data() {
   QStringList line_items;
   QString line_string;

   QStringList tmp_data = {};

   if (ui->spc_list->count() > 0) {
      for (int line_num = 1; line_num < 17;
           line_num++) { // stop after general information
         line_string = spc_string[line_num - 1];
         line_items =
             spc_data[line_num - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

         switch (line_num) {
               /******************************************* HEADER
                * ***************************************************/
            case 1: // File Header
               break;
            case 2: // Spacecraft Description
               tmp_data.append(spc_data[line_num - 1].simplified());
               // ui->spc_desc->setText(spc_data[line_num-1]);
               break;
            case 3: // Spacecraft Label
               tmp_data.append(line_string.simplified());
               // ui->spc_label->setText(line_string);
               break;
            case 4: // Sprite File Name
               tmp_data.append(line_items[0]);
               // ui->spc_sprite->setText(line_items[0]);
               break;
            case 5: // Flight Software Identifier
               tmp_data.append(line_items[0]);
               // setQComboBox(ui->spc_fswid, line_items[0]);
               break;
            case 6: // Flight Software Sample Time
               tmp_data.append(line_items[0]);
               // ui->spc_fswsamp->setText(line_items[0]);
               break;
            case 7:
               /******************************************* HEADER
                * ***************************************************/
               // Orbit Parameters
               break;
            case 8: // Orbit Prop
               tmp_data.append(line_items[0]);
               break;
            case 9: // Pos of CM or ORIGIN wrt F
               tmp_data.append(line_items[0]);
               break;
            case 10: // Pos wrt Formation (m) expressed in F
               tmp_data.append(line_items[0]);
               tmp_data.append(line_items[1]);
               tmp_data.append(line_items[2]);
               break;
            case 11: // Vel wrt Formation (m) expressed in F
               tmp_data.append(line_items[0]);
               tmp_data.append(line_items[1]);
               tmp_data.append(line_items[2]);
               break;
            case 12: // Initial Attitude Header
               break;
            case 13: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
               line_items = spc_data[line_num - 1].split(QRegExp(""),
                                                         Qt::SkipEmptyParts);
               tmp_data.append(line_items[0]);
               tmp_data.append(line_items[1]);
               tmp_data.append(line_items[2]);
               break;
            case 14:
               tmp_data.append(line_items[0]);
               tmp_data.append(line_items[1]);
               tmp_data.append(line_items[2]);
               break;
            case 15:
               tmp_data.append(line_items[0]);
               tmp_data.append(line_items[1]);
               tmp_data.append(line_items[2]);
               tmp_data.append(line_items[3]);
               break;
            case 16:
               tmp_data.append(line_items[0]);
               tmp_data.append(line_items[1]);
               tmp_data.append(line_items[2]);

               tmp_data.append(line_items[3]);
               break;
         }
      }
      ui->spc_list->currentItem()->setData(256, spc_names[spc_name_index]);
      ui->spc_list->currentItem()->setData(257, tmp_data);
   }
}

void SPC_Menu::on_spc_apply_clicked() {
   int index = ui->spc_list->currentRow();
   if (index == -1) {
      return;
   }

   spc_names.sort();
   file_paths.sort();
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
   file_paths[index] = inout_path + "SC_" + cur_name + ".txt";
   QFile::rename(file_path, file_paths[index]);
   file_path = file_paths[index];

   QStringList tmp_data = {};
   QString data_inp     = "";

   for (int line_num = 1; line_num < 17;
        line_num++) { // append data from "general" information
      switch (line_num) {
            /******************************************* HEADER
             * ***************************************************/
         case 1: // File Header
            break;
         case 2: // Spacecraft Description
            tmp_data.append(ui->spc_desc->text());
            break;
         case 3: // Spacecraft label
            tmp_data.append(ui->spc_label->text());
            break;
         case 4: // Sprite File Name
            tmp_data.append(ui->spc_sprite->text());
            break;
         case 5: // Flight Software Identifier
            tmp_data.append(ui->spc_fswid->currentText());
            break;
         case 6: // Flight Software Sample Time
            tmp_data.append(ui->spc_fswsamp->text());
            break;
         case 7:
            /******************************************* HEADER
             * ***************************************************/
            // Orbit Parameters
            break;
         case 8: // Orbit Prop
            tmp_data.append(ui->spc_cur_orb_type->currentText());
            break;
         case 9: // Pos of CM or ORIGIN wrt F
            tmp_data.append(ui->spc_cur_pos_ref->currentText());
            break;
         case 10: // Pos wrt Formation (m) expressed in F
            tmp_data.append(ui->spc_cur_xpos_form->text());
            tmp_data.append(ui->spc_cur_ypos_form->text());
            tmp_data.append(ui->spc_cur_zpos_form->text());
            break;
         case 11: // Vel wrt Formation (m) expressed in F
            tmp_data.append(ui->spc_cur_xvel_form->text());
            tmp_data.append(ui->spc_cur_yvel_form->text());
            tmp_data.append(ui->spc_cur_zvel_form->text());
            break;
         case 12: // Initial Attitude Header
            break;
         case 13: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
            tmp_data.append(ui->spc_cur_angvel_frame1->currentText());
            tmp_data.append(ui->spc_cur_att_param->currentText());
            tmp_data.append(ui->spc_cur_angvel_frame2->currentText());
            break;
         case 14:
            tmp_data.append(ui->spc_cur_angvel_1->text());
            tmp_data.append(ui->spc_cur_angvel_2->text());
            tmp_data.append(ui->spc_cur_angvel_3->text());
            break;
         case 15:
            tmp_data.append(ui->spc_cur_q1->text());
            tmp_data.append(ui->spc_cur_q2->text());
            tmp_data.append(ui->spc_cur_q3->text());
            tmp_data.append(ui->spc_cur_q4->text());

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
            break;
         case 16:
            tmp_data.append(ui->spc_cur_initeul_1->text());
            tmp_data.append(ui->spc_cur_initeul_2->text());
            tmp_data.append(ui->spc_cur_initeul_3->text());
            tmp_data.append(ui->spc_cur_initeul_seq->currentText());

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
            break;
      }
   }

   ui->spc_list->currentItem()->setData(256, cur_name);
   ui->spc_list->currentItem()->setData(257, tmp_data);

   tmp_data.clear();

   QStringList current_data =
       ui->spc_list->currentItem()->data(257).toStringList();

   for (int line_num = 1; line_num < 17;
        line_num++) { // append data from "general" information

      switch (line_num) {
         case 1:
            spc_update.append("<<<<<<<<<<<<<<<<<  42: Spacecraft Description "
                              "File   >>>>>>>>>>>>>>>>>>>\n");
            break; // header
         case 2:
            data_inp = current_data[0];
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Description\n");
            break;
         case 3:
            data_inp = "\"" + current_data[1] + "\"";
            spc_update.append(dsm_gui_lib::whitespace(data_inp) + "!  Label\n");
            break;
         case 4:
            data_inp = current_data[2];
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Sprite File Name\n");
            break;
         case 5:
            data_inp = current_data[3];
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Flight Software Identifier\n");
            break;
         case 6:
            data_inp = current_data[4];
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  FSW Sample Time, sec\n");
            break;
         case 7:
            /******************************************* HEADER
             * ***************************************************/
            // Orbit Parameters
            spc_update.append("************************* Orbit Parameters "
                              "*****************************\n");
            break;
         case 8: // Orbit Prop
            data_inp = ui->spc_cur_orb_type->currentText();
            spc_update.append(
                dsm_gui_lib::whitespace(data_inp) +
                "!  Orbit Prop FIXED, EULER_HILL, ENCKE, or COWELL\n");
            break;
         case 9: // Pos of CM or ORIGIN wrt F
            data_inp = ui->spc_cur_pos_ref->currentText();
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Pos of CM or ORIGIN, wrt F\n");
            break;
         case 10: // Pos wrt Formation (m) expressed in F
            data_inp = ui->spc_cur_xpos_form->text() + "  " +
                       ui->spc_cur_ypos_form->text() + "  " +
                       ui->spc_cur_zpos_form->text();
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Pos wrt Formation (m), expressed in F\n");
            break;
         case 11: // Vel wrt Formation (m) expressed in F
            data_inp = ui->spc_cur_xvel_form->text() + "  " +
                       ui->spc_cur_yvel_form->text() + "  " +
                       ui->spc_cur_zvel_form->text();
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Vel wrt Formation (m/s), expressed in F\n");
            break;
         case 12: // Initial Attitude Header
            spc_update.append("*************************** Initial Attitude "
                              "***************************\n");
            break;
         case 13: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
            data_inp = ui->spc_cur_angvel_frame1->currentText() +
                       ui->spc_cur_att_param->currentText() +
                       ui->spc_cur_angvel_frame2->currentText();
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Ang Vel wrt [NL], Att [QA] wrt [NLF]\n");
            break;
         case 14:
            data_inp = ui->spc_cur_angvel_1->text() + "  " +
                       ui->spc_cur_angvel_2->text() + "  " +
                       ui->spc_cur_angvel_3->text();
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Ang Vel (deg/sec)\n");
            break;
         case 15:
            data_inp = ui->spc_cur_q1->text() + "  " + ui->spc_cur_q2->text() +
                       "  " + ui->spc_cur_q3->text() + "  " +
                       ui->spc_cur_q4->text();

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
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Quaternion\n");
            break;
         case 16:
            data_inp = ui->spc_cur_initeul_1->text() + "  " +
                       ui->spc_cur_initeul_2->text() + "  " +
                       ui->spc_cur_initeul_3->text() + "  " +
                       ui->spc_cur_initeul_seq->currentText();

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
            spc_update.append(dsm_gui_lib::whitespace(data_inp) +
                              "!  Angles (deg) & Euler Sequence\n");
            break;
      }
   }

   // Read in the rest of the file and append it to the end. Functionally
   // equivalent to only changing the first 16 lines.
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   QTextStream in(&file);
   long counter = 1;
   while (!in.atEnd()) {
      QString line = in.readLine();
      if (counter > 16) {
         spc_update.append(line);
         spc_update.append("\n");
      }
      counter++;
   }
   file.close();

   index            = file_paths.indexOf(file_path);
   global_spc_index = index;
   spc_name_index   = index;
   ui->spc_list->setCurrentRow(index);

   write_data();
   on_spc_list_currentTextChanged(ui->spc_list->currentItem()->text());
}

void SPC_Menu::write_data() {
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      for (int i = 0; i < spc_update.size(); i++) {
         in << spc_update.at(i);
      }
   }
   spc_update.clear();
   file.close();
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

   QString new_name = "New";
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

   QFile::copy(":/data/__default__/SC_Simple.txt",
               inout_path + "SC_" + new_name + ".txt");
   ui->spc_list->sortItems();
   ui->spc_conf->setEnabled(true);

   new_item = 0;
   if (spc_submenu != nullptr)
      on_spc_list_currentTextChanged(
          new_name); // click on the current item to reload submenu
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
         newSCTest += "_" + QString::number(i);
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
         apply_data();
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
   apply_data();

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
      spc_submenu->activateWindow();
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
