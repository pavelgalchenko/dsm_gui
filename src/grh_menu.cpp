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
}

void GRH_Menu::receive_grhpath(QString path) {
   inout_path    = path;
   graphics_path = path + "Inp_Graphics.txt";
   receive_data();
   apply_data();
}

void GRH_Menu::receive_data() {
   grh_data.clear();
   grh_string.clear();
   static QRegularExpression rx1("(.*?)!");
   static QRegularExpression rx2("\"(.*?)\"");
   static QRegularExpression rx3("SC_(.*).txt");

   QFile file(graphics_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   QTextStream in(&file);
   while (!in.atEnd()) {
      QString line                   = in.readLine();
      QRegularExpressionMatch match1 = rx1.match(line);
      grh_data.append(match1.captured(1));

      QRegularExpressionMatch match2 = rx2.match(line);
      grh_string.append(match2.captured(1));
   }
   file.close();

   QStringList scFiles = QDir(inout_path).entryList({"SC_*"});
   QStringList scNames;
   for (int i = 0; i < scFiles.length(); i++)
      scNames.append(scFiles[i].chopped(4).mid(3));
   QSet<QString> scNamesSet(scNames.begin(), scNames.end());

   QFile simFile(inout_path + "Inp_Sim.txt");
   if (!simFile.open(QIODevice::ReadOnly))
      QMessageBox::information(0, "error", simFile.errorString());
   QTextStream simIn(&simFile);

   QStringList simSCNames;
   QHash<QString, int> simSCNum;
   while (!simIn.atEnd()) {
      QString line = simIn.readLine();
      if (line.contains("Spacecraft", Qt::CaseInsensitive)) {
         line = simIn.readLine();
         QStringList line_items =
             line.remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
         int nSC = line_items[0].toInt();
         for (int i = 0; i < nSC; i++) {
            line         = simIn.readLine();
            QString name = rx3.match(line).captured(1);
            simSCNames.append(name);
            simSCNum.insert(name, i);
         }
         break;
      }
   }
   simFile.close();
   QSet<QString> simSCNamesSet(simSCNames.begin(), simSCNames.end());

   scNums.clear();
   QStringList spaceCrafts = scNamesSet.intersect(simSCNamesSet).values();
   for (int i = 0; i < spaceCrafts.count(); i++)
      scNums.insert(spaceCrafts[i], simSCNum[spaceCrafts[i]]);

   ui->hostSC->clear();
   ui->hostSC->addItems(dsm_gui_lib::sortStringList(scNums.keys()));

   ui->targetSC->clear();
   ui->targetSC->addItems(dsm_gui_lib::sortStringList(scNums.keys()));
}

void GRH_Menu::write_data() {
   QFile::remove(graphics_path);
   QFile file(graphics_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      for (int i = 0; i < grh_update.size(); i++) {
         in << grh_update.at(i);
      }
   }
   grh_update.clear();
   file.close();
}

void GRH_Menu::apply_data() {
   QStringList line_items;
   QString line_string;

   for (int line_num = 1; line_num < grh_data.length(); line_num++) {
      line_string = grh_string[line_num - 1];
      line_items  = grh_data[line_num - 1].remove("\"").split(
          QRegExp("\\s"), Qt::SkipEmptyParts);
      switch (line_num) {
         case 1: // File header
            break;
         case 2:
            ui->gl_output->setText(line_items[0]);
            break;
         case 3:
            ui->star_catalog->setText(line_items[0]);
            break;
         case 4:
            ui->mapWindow->setChecked(QVariant(line_items[0]).toBool());
            break;
         case 5:
            ui->orreyWindow->setChecked(QVariant(line_items[0]).toBool());
            break;
         case 6:
            ui->sphereWindow->setChecked(QVariant(line_items[0]).toBool());
            break;
         case 7: // POV Header
            break;
         case 8:
            ui->pause->setChecked(QVariant(line_items[0]).toBool());
            break;
         case 9:
            ui->povMode->setCurrentText(povMode_inputs[line_items[0]]);
            break;
         case 10:
            ui->hostType->setCurrentText(hosttarget_inputs[line_items[0]]);
            break;
         case 11:
            ui->hostSC->setCurrentText(scNums.key(line_items[0].toInt()));
            ui->hostBDY->setValue(line_items[1].toInt());
            ui->hostFrame->setCurrentText(frame_inputs[line_items[2]]);
            break;
         case 12:
            ui->targetType->setCurrentText(hosttarget_inputs[line_items[0]]);
            break;
         case 13:
            ui->targetSC->setCurrentText(scNums.key(line_items[0].toInt()));
            ui->targetBDY->setValue(line_items[1].toInt());
            ui->targetFrame->setCurrentText(frame_inputs[line_items[2]]);
            break;
         case 14:
            ui->boresightaxis->setCurrentText(axis_inputs[line_items[0]]);
            break;
         case 15:
            ui->upaxis->setCurrentText(axis_inputs[line_items[0]]);
            break;
         case 16:
            ui->povrange->setText(line_items[0]);
            break;
         case 17:
            ui->povangle->setText(line_items[0]);
            break;
         case 18:
            ui->povinhost_x->setText(line_items[0]);
            ui->povinhost_y->setText(line_items[1]);
            ui->povinhost_z->setText(line_items[2]);
            break;
         case 19:
            ui->povview->setCurrentText(view_inputs[line_items[0]]);
            break;
         case 20: // CAM Header
            break;
         case 21:
            ui->camtitle->setText(line_string);
            break;
         case 22:
            ui->camwidth->setText(line_items[0]);
            ui->camheight->setText(line_items[1]);
            break;
         case 23:
            ui->mousescale->setText(line_items[0]);
            break;
         case 24:
            ui->displaygamma->setValue(line_items[0].toDouble());
            break;
         case 25: // CAM Show Header
            break;
         case 26:
            ui->showNAxis->setChecked(QVariant(line_items[0]).toBool());
            ui->show_naxis_name->setText(line_string);
            break;
         case 27:
            ui->showLAxis->setChecked(QVariant(line_items[0]).toBool());
            ui->show_laxis_name->setText(line_string);
            break;
         case 28:
            ui->showFAxis->setChecked(QVariant(line_items[0]).toBool());
            ui->show_faxis_name->setText(line_string);
            break;
         case 29:
            ui->showBAxis->setChecked(QVariant(line_items[0]).toBool());
            ui->show_baxis_name->setText(line_string);
            break;
         case 30:
            ui->showNGrid->setChecked(QVariant(line_items[0]).toBool());
            ui->show_ngrid_name->setText(line_string);
            break;
         case 31:
            ui->showLGrid->setChecked(QVariant(line_items[0]).toBool());
            ui->show_lgrid_name->setText(line_string);
            break;
         case 32:
            ui->showFGrid->setChecked(QVariant(line_items[0]).toBool());
            ui->show_fgrid_name->setText(line_string);
            break;
         case 33:
            ui->showBGrid->setChecked(QVariant(line_items[0]).toBool());
            ui->show_bgrid_name->setText(line_string);
            break;
         case 34:
            ui->showGalGrid->setChecked(QVariant(line_items[0]).toBool());
            ui->show_galgrid_name->setText(line_string);
            break;
         case 35:
            ui->showFOV->setChecked(QVariant(line_items[0]).toBool());
            ui->show_fov_name->setText(line_string);
            break;
         case 36:
            ui->showProxOps->setChecked(QVariant(line_items[0]).toBool());
            ui->show_proxops_name->setText(line_string);
            break;
         case 37:
            ui->showTDRS->setChecked(QVariant(line_items[0]).toBool());
            ui->show_tdrs_name->setText(line_string);
            break;
         case 38:
            ui->showShadows->setChecked(QVariant(line_items[0]).toBool());
            ui->show_shdws_name->setText(line_string);
            break;
         case 39:
            ui->showAstro->setChecked(QVariant(line_items[0]).toBool());
            ui->show_astro_name->setText(line_string);
            break;
         case 40:
            ui->showTruth->setChecked(QVariant(line_items[0]).toBool());
            ui->show_truth_name->setText(line_string);
            break;
         case 41:
            ui->showFSW->setChecked(QVariant(line_items[0]).toBool());
            ui->show_fsw_name->setText(line_string);
            break;
         case 42:
            ui->showMilky->setChecked(QVariant(line_items[0]).toBool());
            ui->show_milky_name->setText(line_string);
            break;
         case 43:
            ui->showFermi->setChecked(QVariant(line_items[0]).toBool());
            ui->show_fermi_name->setText(line_string);
            break;
         case 44: // MAP Header
            break;
         case 45:
            ui->maptitle->setText(line_string);
            break;
         case 46:
            ui->mapwidth->setText(line_items[0]);
            ui->mapheight->setText(line_items[1]);
            break;
         case 47: // MAP Show
            break;
         case 48:
            ui->showClock->setChecked(QVariant(line_items[0]).toBool());
            ui->showclock_name->setText(line_string);
            break;
         case 49:
            ui->showTlmClock->setChecked(QVariant(line_items[0]).toBool());
            ui->showtlmclock_name->setText(line_string);
            break;
         case 50:
            ui->showCredits->setChecked(QVariant(line_items[0]).toBool());
            ui->showcredits_name->setText(line_string);
            break;
         case 51:
            ui->showNight->setChecked(QVariant(line_items[0]).toBool());
            ui->shownight_name->setText(line_string);
            break;
         case 52: // Unit Sphere Header
            break;
         case 53:
            ui->showMajorConst->setChecked(QVariant(line_items[0]).toBool());
            break;
         case 54:
            ui->showZodiac->setChecked(QVariant(line_items[0]).toBool());
            break;
         case 55:
            ui->showMinorConst->setChecked(QVariant(line_items[0]).toBool());
            break;
         default:
            break;
      }
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
      QFile::remove(inout_path + "Inp_Graphics.txt");
      QFile::copy(inout_path + "__default__/Inp_Graphics.txt",
                  inout_path + "Inp_Graphics.txt");
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
      QFile::remove(inout_path + "__default__/Inp_Graphics.txt");
      QFile::copy(inout_path + "Inp_Graphics.txt",
                  inout_path + "__default__/Inp_Graphics.txt");
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
   QString data_inp;

   grh_update.clear();

   for (int line_num = 1; line_num <= grh_data.length(); line_num++) {
      switch (line_num) {
         case 1:
            data_inp = "<<<<<<<<<<<<<<<<<<<  42 Graphics Configuration File  "
                       ">>>>>>>>>>>>>>>>>>>\n";
            break;
         case 2:
            data_inp  = dsm_gui_lib::whitespace(ui->gl_output->text());
            data_inp += " !  GL Output Interval [sec]\n";
            break;
         case 3:
            data_inp  = dsm_gui_lib::whitespace(ui->star_catalog->text());
            data_inp += " !  Star Catalog File Name\n";
            break;
         case 4:
            data_inp =
                dsm_gui_lib::whitespace(dsm_gui_lib::toString(ui->mapWindow));
            data_inp += " !  Map Window Exists\n";
            break;
         case 5:
            data_inp =
                dsm_gui_lib::whitespace(dsm_gui_lib::toString(ui->orreyWindow));
            data_inp += " !  Orrery Window Exists\n";
            break;
         case 6:
            data_inp = dsm_gui_lib::whitespace(
                dsm_gui_lib::toString(ui->sphereWindow));
            data_inp += " !  Unit Sphere Window Exists\n";
            break;
         case 7:
            data_inp = "********************************* POV "
                       "**********************************\n";
            break;
         case 8:
            data_inp =
                dsm_gui_lib::whitespace(dsm_gui_lib::toString(ui->pause));
            data_inp += " !  Pause at Startup\n";
            break;
         case 9:
            data_inp = dsm_gui_lib::whitespace(
                povMode_inputs.key(ui->povMode->currentText()));
            data_inp +=
                " !  POV Mode (TRACK_HOST, TRACK_TARGET, FIXED_IN_HOST)\n";
            break;
         case 10:
            data_inp = dsm_gui_lib::whitespace(
                hosttarget_inputs.key(ui->hostType->currentText()));
            data_inp += " !  Host Type (WORLD, REFORB, FRM, SC, BODY)\n";
            break;
         case 11:
            data_inp =
                QString::number(scNums[ui->hostSC->currentText()]) + "  ";
            data_inp += QString::number(ui->hostBDY->value()) + "  ";
            data_inp += frame_inputs.key(ui->hostFrame->currentText());
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Initial Host SC, Body, POV Frame\n";
            break;
         case 12:
            data_inp = dsm_gui_lib::whitespace(
                hosttarget_inputs.key(ui->targetType->currentText()));
            data_inp += " !  Target Type (WORLD, REFORB, FRM, SC, BODY)\n";
            break;
         case 13:
            data_inp =
                QString::number(scNums[ui->targetSC->currentText()]) + "  ";
            data_inp += QString::number(ui->targetBDY->value()) + "  ";
            data_inp += frame_inputs.key(ui->targetFrame->currentText());
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Initial Host SC, Body, POV Frame\n";
            break;
         case 14:
            data_inp = dsm_gui_lib::whitespace(
                axis_inputs.key(ui->boresightaxis->currentText()));
            data_inp += " !  Boresight Axis\n";
            break;
         case 15:
            data_inp = dsm_gui_lib::whitespace(
                axis_inputs.key(ui->upaxis->currentText()));
            data_inp += " !  Up Axis\n";
            break;
         case 16:
            data_inp  = dsm_gui_lib::whitespace(ui->povrange->text());
            data_inp += " !  Initial POV Range from Target [m]\n";
            break;
         case 17:
            data_inp  = dsm_gui_lib::whitespace(ui->povangle->text());
            data_inp += " !  POV Angle (Vertical) [deg]\n";
            break;
         case 18:
            data_inp  = ui->povinhost_x->text() + "  ";
            data_inp += ui->povinhost_y->text() + "  ";
            data_inp += ui->povinhost_z->text();
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  POV Position in Host [m]\n";
            break;
         case 19:
            data_inp = dsm_gui_lib::whitespace(
                view_inputs.key(ui->povview->currentText()));
            data_inp += " !  Initial POV View (FRONT, FRONT_RIGHT, etc)\n";
            break;
         case 20:
            data_inp = "********************************* CAM "
                       "**********************************\n";
            break;
         case 21:
            data_inp =
                dsm_gui_lib::whitespace("\"" + ui->camtitle->text() + "\"");
            data_inp += " !  Cam Title [delimited by \"]\n";
            break;
         case 22:
            data_inp  = ui->camwidth->text() + "  ";
            data_inp += ui->camheight->text();
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Width, Height [pixels]\n";
            break;
         case 23:
            data_inp  = dsm_gui_lib::whitespace(ui->mousescale->text());
            data_inp += " !  Mouse Scale Factor\n";
            break;
         case 24:
            data_inp  = dsm_gui_lib::whitespace(ui->displaygamma->text());
            data_inp += " !  Display's Gamma Exponent (1.8-4.0)\n";
            break;
         case 25:
            data_inp = "**************************** CAM Show Menu "
                       "*****************************\n";
            break;
         case 26:
            data_inp  = dsm_gui_lib::toString(ui->showNAxis) + "  ";
            data_inp += "\"" + ui->show_naxis_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show N Axes\n";
            break;
         case 27:
            data_inp  = dsm_gui_lib::toString(ui->showLAxis) + "  ";
            data_inp += "\"" + ui->show_laxis_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show L Axes\n";
            break;
         case 28:
            data_inp  = dsm_gui_lib::toString(ui->showFAxis) + "  ";
            data_inp += "\"" + ui->show_faxis_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show F Axes\n";
            break;
         case 29:
            data_inp  = dsm_gui_lib::toString(ui->showBAxis) + "  ";
            data_inp += "\"" + ui->show_baxis_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show B Axes\n";
            break;
         case 30:
            data_inp  = dsm_gui_lib::toString(ui->showNGrid) + "  ";
            data_inp += "\"" + ui->show_ngrid_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show N Grid\n";
            break;
         case 31:
            data_inp  = dsm_gui_lib::toString(ui->showLGrid) + "  ";
            data_inp += "\"" + ui->show_lgrid_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show L Grid\n";
            break;
         case 32:
            data_inp  = dsm_gui_lib::toString(ui->showFGrid) + "  ";
            data_inp += "\"" + ui->show_fgrid_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show F Grid\n";
            break;
         case 33:
            data_inp  = dsm_gui_lib::toString(ui->showBGrid) + "  ";
            data_inp += "\"" + ui->show_bgrid_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show B Grid\n";
            break;
         case 34:
            data_inp  = dsm_gui_lib::toString(ui->showGalGrid) + "  ";
            data_inp += "\"" + ui->show_galgrid_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show G Grid\n";
            break;
         case 35:
            data_inp  = dsm_gui_lib::toString(ui->showFOV) + "  ";
            data_inp += "\"" + ui->show_fov_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Fields of View\n";
            break;
         case 36:
            data_inp  = dsm_gui_lib::toString(ui->showProxOps) + "  ";
            data_inp += "\"" + ui->show_proxops_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Prox Ops\n";
            break;
         case 37:
            data_inp  = dsm_gui_lib::toString(ui->showTDRS) + "  ";
            data_inp += "\"" + ui->show_tdrs_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show TDRS Satellites\n";
            break;
         case 38:
            data_inp  = dsm_gui_lib::toString(ui->showShadows) + "  ";
            data_inp += "\"" + ui->show_shdws_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Shadows\n";
            break;
         case 39:
            data_inp  = dsm_gui_lib::toString(ui->showAstro) + "  ";
            data_inp += "\"" + ui->show_astro_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Astro Labels\n";
            break;
         case 40:
            data_inp  = dsm_gui_lib::toString(ui->showTruth) + "  ";
            data_inp += "\"" + ui->show_truth_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Truth Vectors\n";
            break;
         case 41:
            data_inp  = dsm_gui_lib::toString(ui->showFSW) + "  ";
            data_inp += "\"" + ui->show_fsw_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show FSW Vectors\n";
            break;
         case 42:
            data_inp  = dsm_gui_lib::toString(ui->showMilky) + "  ";
            data_inp += "\"" + ui->show_milky_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Milky Way\n";
            break;
         case 43:
            data_inp  = dsm_gui_lib::toString(ui->showFermi) + "  ";
            data_inp += "\"" + ui->show_fermi_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Fermi Sky\n";
            break;
         case 44:
            data_inp = "********************************* MAP "
                       "**********************************\n";
            break;
         case 45:
            data_inp =
                dsm_gui_lib::whitespace("\"" + ui->maptitle->text() + "\"");
            data_inp += " !  Map Title [delimited by \"]\n";
            break;
         case 46:
            data_inp  = dsm_gui_lib::whitespace(ui->mapwidth->text() + "  " +
                                                ui->mapheight->text());
            data_inp += " !  Width, Height [pixels]\n";
            break;
         case 47:
            data_inp = "**************************** MAP Show Menu "
                       "*****************************\n";
            break;
         case 48:
            data_inp  = dsm_gui_lib::toString(ui->showClock) + "  ";
            data_inp += "\"" + ui->showclock_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Clock\n";
            break;
         case 49:
            data_inp  = dsm_gui_lib::toString(ui->showTlmClock) + "  ";
            data_inp += "\"" + ui->showtlmclock_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Tlm Clock\n";
            break;
         case 50:
            data_inp  = dsm_gui_lib::toString(ui->showCredits) + "  ";
            data_inp += "\"" + ui->showcredits_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Credits\n";
            break;
         case 51:
            data_inp  = dsm_gui_lib::toString(ui->showNight) + "  ";
            data_inp += "\"" + ui->shownight_name->text() + "\"";
            data_inp  = dsm_gui_lib::whitespace(data_inp);
            data_inp += " !  Show Night\n";
            break;
         case 52:
            data_inp = "************************ Unit Sphere Show Menu "
                       "*************************\n";
            break;
         case 53:
            data_inp = dsm_gui_lib::whitespace(
                dsm_gui_lib::toString(ui->showMajorConst));
            data_inp += " !  Show Major Constellations\n";
            break;
         case 54:
            data_inp =
                dsm_gui_lib::whitespace(dsm_gui_lib::toString(ui->showZodiac));
            data_inp += " !  Show Zodiac Constellations\n";
            break;
         case 55:
            data_inp = dsm_gui_lib::whitespace(
                dsm_gui_lib::toString(ui->showMinorConst));
            data_inp += " !  Show Minor Constellations\n";
            break;
         default:
            break;
      }
      grh_update.append(data_inp);
      data_inp.clear();
   }
   write_data();
}

void GRH_Menu::on_hostSC_currentTextChanged(const QString &arg1) {
   QStringList scFileNames =
       QDir(inout_path).entryList({"SC_" + arg1 + ".txt"});
   if (scFileNames.isEmpty())
      return;

   ui->hostBDY->setMaximum(
       dsm_gui_lib::get_sc_nitems(inout_path, arg1,
                                  dsm_gui_lib::scSectionType::BODY) -
       1);
}

void GRH_Menu::on_targetSC_currentTextChanged(const QString &arg1) {
   QStringList scFileNames =
       QDir(inout_path).entryList({"SC_" + arg1 + ".txt"});
   if (scFileNames.isEmpty())
      return;

   ui->targetBDY->setMaximum(
       dsm_gui_lib::get_sc_nitems(inout_path, arg1,
                                  dsm_gui_lib::scSectionType::BODY) -
       1);
}
