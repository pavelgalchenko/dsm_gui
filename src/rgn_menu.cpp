#include "rgn_menu.h"
#include "ui_rgn_menu.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

RGN_Menu::RGN_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::RGN_Menu) {
   ui->setupUi(this);
   set_validators();
}

RGN_Menu::~RGN_Menu() {
   delete ui;
}

void RGN_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");

   ui->world->addItems(dsm_gui_lib::worldInputs);
   ui->location->addItems(
       dsm_gui_lib::sortStringList(location_inputs.values()));
   ui->posw_x->setValidator(new QDoubleValidator);
   ui->posw_y->setValidator(new QDoubleValidator);
   ui->posw_z->setValidator(new QDoubleValidator);
   ui->lng->setValidator(new QDoubleValidator);
   ui->lat->setValidator(new QDoubleValidator);
   ui->alt->setValidator(new QDoubleValidator);
   ui->elasticity->setValidator(new QDoubleValidator);
   ui->damping->setValidator(new QDoubleValidator);
   ui->friction->setValidator(new QDoubleValidator);
   ui->regionname->setValidator(new QRegularExpressionValidator(rx));
   ui->geometryname->setValidator(new QRegularExpressionValidator(rx));

   ui->rgnlist->setSortingEnabled(true);
   ui->rgnlist->sortItems(Qt::AscendingOrder);

   connect(ui->world, &QComboBox::currentTextChanged, this,
           &RGN_Menu::world_changed);
   connect(ui->minorbodynum, &QSpinBox::textChanged, this,
           &RGN_Menu::world_changed);

   connect(ui->location, &QComboBox::currentTextChanged, this,
           &RGN_Menu::location_changed);

   connect(ui->posw_x, &QLineEdit::textChanged, this, &RGN_Menu::posw_changed);
   connect(ui->posw_y, &QLineEdit::textChanged, this, &RGN_Menu::posw_changed);
   connect(ui->posw_z, &QLineEdit::textChanged, this, &RGN_Menu::posw_changed);

   connect(ui->lng, &QLineEdit::textChanged, this, &RGN_Menu::lla_changed);
   connect(ui->lat, &QLineEdit::textChanged, this, &RGN_Menu::lla_changed);
   connect(ui->alt, &QLineEdit::textChanged, this, &RGN_Menu::lla_changed);

   connect(ui->elasticity, &QLineEdit::textChanged, this,
           &RGN_Menu::coeffs_changed);
   connect(ui->damping, &QLineEdit::textChanged, this,
           &RGN_Menu::coeffs_changed);
   connect(ui->friction, &QLineEdit::textChanged, this,
           &RGN_Menu::coeffs_changed);

   connect(ui->geometryname, &QLineEdit::textChanged, this,
           &RGN_Menu::geometry_changed);
}

void RGN_Menu::receive_rgnpath(QString path) {
   inout_path = path;
   file_path  = path + "Inp_Region.txt";
   receive_data();
   apply_data();
}

void RGN_Menu::receive_data() {
   rgn_data.clear();
   rgn_string.clear();
   rgn_update.clear();
   static QRegularExpression rx1("(.*?)!");
   static QRegularExpression rx2("\"(.*?)\"");

   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   }

   QTextStream in(&file);
   while (!in.atEnd()) {
      QString line                   = in.readLine();
      QRegularExpressionMatch match1 = rx1.match(line);
      rgn_data.append(match1.captured(1));

      QRegularExpressionMatch match2 = rx2.match(line);
      rgn_string.append(match2.captured(1));
   }
   file.close();
}

void RGN_Menu::write_data() {
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      for (int i = 0; i < rgn_update.size(); i++) {
         in << rgn_update.at(i);
      }
   }
   file.close();
   rgn_update.clear();
}

void RGN_Menu::apply_data() {
   QStringList line_items;
   QListWidgetItem *newRGN;
   int num_rgn = 0;

   QStringList tmpData;

   line_items = rgn_data[1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
   num_rgn    = line_items[0].toInt();

   ui->rgnlist->clear();
   clear_fields();

   for (int i = 0; i < num_rgn; i++) {
      newRGN = new QListWidgetItem();
      for (int j = 0; j < rgnNLines - 1; j++) {
         int k      = i * rgnNLines + 3 + j;
         line_items = rgn_data[k].split(QRegExp("\\s"), Qt::SkipEmptyParts);
         switch (j) {
            case 0:
               tmpData.append(line_items[0]);
               newRGN->setData(RGN_Menu::Exists, tmpData);
               break;
            case 1:
               tmpData.append(rgn_string[k]);
               newRGN->setData(RGN_Menu::Name, tmpData);
               break;
            case 2:
               tmpData.append(line_items[0]);
               newRGN->setData(RGN_Menu::World, tmpData);
               break;
            case 3:
               tmpData.append(line_items[0]);
               newRGN->setData(RGN_Menu::Location, tmpData);
               break;
            case 4:
               tmpData.append(line_items[0]);
               tmpData.append(line_items[1]);
               tmpData.append(line_items[2]);
               newRGN->setData(RGN_Menu::PosW, tmpData);
               break;
            case 5:
               tmpData.append(line_items[0]);
               tmpData.append(line_items[1]);
               tmpData.append(line_items[2]);
               newRGN->setData(RGN_Menu::LLA, tmpData);
               break;
            case 6:
               tmpData.append(line_items[0]);
               tmpData.append(line_items[1]);
               tmpData.append(line_items[2]);
               newRGN->setData(RGN_Menu::Coeffs, tmpData);
               break;
            case 7:
               tmpData.append(line_items[0]);
               newRGN->setData(RGN_Menu::GeometryFile, tmpData);
               break;
            default:
               break;
         }
         tmpData.clear();
      }
      ui->rgnlist->addItem(newRGN);
   }
}

void RGN_Menu::on_rgn_remove_clicked() {
   int removeitem = ui->rgnlist->currentRow();
   if (removeitem == -1) {
      return;
   } else {
      ui->rgnlist->takeItem(removeitem);
      ui->rgnlist->setCurrentRow(-1);
      clear_fields();
   }
}

void RGN_Menu::on_rgn_add_clicked() {
   QListWidgetItem *newRGN = new QListWidgetItem();
   QStringList line_items;

   QStringList newData;
   QStringList tmpData;

   QString newName      = "NEW REGION";
   QStringList curNames = dsm_gui_lib::getTextFromList(ui->rgnlist);
   if (ui->rgnlist->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         QString newNameTest = newName;
         if (i > 0)
            newNameTest += "_" + QString::number(i);
         if (!curNames.contains(newNameTest)) {
            newName = newNameTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }

   newData.append("TRUE                         ");
   newData.append("");
   newData.append("EARTH                        ");
   newData.append("LLA                          ");
   newData.append("0.0   0.0  0.0               ");
   newData.append("-80.53  28.46 1000.0         ");
   newData.append("1.0E6  1.0E4  0.1            ");
   newData.append("Rgn_Terrain.obj              ");
   newData.append("");

   for (int j = 0; j < rgnNLines - 1; j++) {
      line_items = newData[j].split(QRegExp("\\s"), Qt::SkipEmptyParts);
      switch (j) {
         case 0:
            tmpData.append(line_items[0]);
            newRGN->setData(RGN_Menu::Exists, tmpData);
            break;
         case 1:
            tmpData.append(newName);
            newRGN->setData(RGN_Menu::Name, tmpData);
            break;
         case 2:
            tmpData.append(line_items[0]);
            newRGN->setData(RGN_Menu::World, tmpData);
            break;
         case 3:
            tmpData.append(line_items[0]);
            newRGN->setData(RGN_Menu::Location, tmpData);
            break;
         case 4:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            tmpData.append(line_items[2]);
            newRGN->setData(RGN_Menu::PosW, tmpData);
            break;
         case 5:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            tmpData.append(line_items[2]);
            newRGN->setData(RGN_Menu::LLA, tmpData);
            break;
         case 6:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            tmpData.append(line_items[2]);
            newRGN->setData(RGN_Menu::Coeffs, tmpData);
            break;
         case 7:
            tmpData.append(line_items[0]);
            newRGN->setData(RGN_Menu::GeometryFile, tmpData);
            break;
         default:
            break;
      }
      tmpData.clear();
   }

   ui->rgnlist->addItem(newRGN);

   ui->rgnlist->setCurrentRow(-1);
   clear_fields();
}

void RGN_Menu::on_rgnlist_itemClicked(QListWidgetItem *item) {
   QStringList tmpData = {};

   for (int i = 0; i < rgnNLines; i++) {
      switch (i) {
         case 0:
            tmpData = item->data(RGN_Menu::Exists).toStringList();
            ui->exists->setChecked(QVariant(tmpData[0]).toBool());
            break;
         case 1:
            tmpData = item->data(RGN_Menu::Name).toStringList();
            ui->regionname->setText(tmpData[0]);
            break;
         case 2:
            tmpData = item->data(RGN_Menu::World).toStringList();
            if (tmpData.contains("MINORBODY")) {
               ui->world->setCurrentText("MINORBODY");
               QStringList split =
                   tmpData[0].split(QRegExp("_"), Qt::SkipEmptyParts);
               ui->minorbodynum->setValue(split[1].toInt());
            } else {
               ui->minorbodynum->setValue(0);
               ui->world->setCurrentText(tmpData[0]);
            }
            break;
         case 3:
            tmpData = item->data(RGN_Menu::Location).toStringList();
            ui->location->setCurrentText(location_inputs[tmpData[0]]);
            break;
         case 4:
            tmpData = item->data(RGN_Menu::PosW).toStringList();
            ui->posw_x->setText(tmpData[0]);
            ui->posw_y->setText(tmpData[1]);
            ui->posw_z->setText(tmpData[2]);
            break;
         case 5:
            tmpData = item->data(RGN_Menu::LLA).toStringList();
            ui->lng->setText(tmpData[0]);
            ui->lat->setText(tmpData[1]);
            ui->alt->setText(tmpData[2]);
            break;
         case 6:
            tmpData = item->data(RGN_Menu::Coeffs).toStringList();
            ui->elasticity->setText(tmpData[0]);
            ui->damping->setText(tmpData[1]);
            ui->friction->setText(tmpData[2]);
            break;
         case 7:
            tmpData = item->data(RGN_Menu::GeometryFile).toStringList();
            ui->geometryname->setText(tmpData[0]);
            break;
         default:
            break;
      }
      tmpData.clear();
   }
}

void RGN_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Region file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_Region.txt");
      QFile::copy(inout_path + "__default__/Inp_Region.txt",
                  inout_path + "Inp_Region.txt");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void RGN_Menu::on_savedefaultButton_clicked() {
   int response =
       dsm_gui_lib::warning_message("Overwrite Default Region file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_Region.txt");
      QFile::copy(inout_path + "Inp_Region.txt",
                  inout_path + "__default__/Inp_Region.txt");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void RGN_Menu::on_closeButton_clicked() {
   RGN_Menu::close();
}

void RGN_Menu::on_applyButton_clicked() {
   QString data_inp;
   QStringList tmpData = {};
   QListWidgetItem *item;

   int rgn_num = ui->rgnlist->count();

   rgn_update.append("***************************  Regions for 42  "
                     "***************************\n");

   data_inp = QString::number(rgn_num);
   rgn_update.append(dsm_gui_lib::whitespace(data_inp) +
                     " !  Number of Regions\n");

   for (int i = 0; i < rgn_num; i++) {
      item = ui->rgnlist->item(i);
      for (int j = 0; j < rgnNLines; j++) {
         switch (j) {
            case 0:
               data_inp = "----------------------------------------------------"
                          "--------------------\n";
               break;
            case 1:
               tmpData   = item->data(RGN_Menu::Exists).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData[0]);
               data_inp += " !  Exists\n";
               break;
            case 2:
               tmpData   = item->data(RGN_Menu::Name).toStringList();
               data_inp  = dsm_gui_lib::whitespace("\"" + tmpData[0] + "\"");
               data_inp += " !  Name\n";
               break;
            case 3:
               tmpData   = item->data(RGN_Menu::World).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData[0]);
               data_inp += " !  World\n";
               break;
            case 4:
               tmpData   = item->data(RGN_Menu::Location).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData[0]);
               data_inp += " !  POSW or LLA\n";
               break;
            case 5:
               tmpData   = item->data(RGN_Menu::PosW).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData.join("  "));
               data_inp += " !  Position in W [m]\n";
               break;
            case 6:
               tmpData   = item->data(RGN_Menu::LLA).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData.join("  "));
               data_inp += " !  Lng, Lat [deg], Alt [m]\n";
               break;
            case 7:
               tmpData   = item->data(RGN_Menu::Coeffs).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData.join("  "));
               data_inp += " !  Elasticity, Damping, Friction Coef\n";
               break;
            case 8:
               tmpData   = item->data(RGN_Menu::GeometryFile).toStringList();
               data_inp  = dsm_gui_lib::whitespace(tmpData[0]);
               data_inp += " !  Geometry File Name\n";
               break;
         }
         rgn_update.append(data_inp);
         data_inp.clear();
      }
   }
   write_data();
}

void RGN_Menu::world_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QString tmpData = {};
   tmpData         = ui->world->currentText();

   bool isMinor = !tmpData.compare("MINORBODY");
   ui->minorbodylabel->setEnabled(isMinor);
   ui->minorbodynum->setEnabled(isMinor);

   if (isMinor)
      tmpData += "_" + ui->minorbodynum->text();
   else
      ui->minorbodynum->setValue(0);

   ui->rgnlist->currentItem()->setData(RGN_Menu::World, tmpData);
}

void RGN_Menu::location_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QString tmpData = {};
   tmpData.append(location_inputs.key(ui->location->currentText()));
   ui->rgnlist->currentItem()->setData(RGN_Menu::Location, tmpData);
}

void RGN_Menu::posw_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QStringList tmpData = {};
   tmpData.append(ui->posw_x->text());
   tmpData.append(ui->posw_y->text());
   tmpData.append(ui->posw_z->text());
   ui->rgnlist->currentItem()->setData(RGN_Menu::PosW, tmpData);
}

void RGN_Menu::lla_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QStringList tmpData = {};
   tmpData.append(ui->lng->text());
   tmpData.append(ui->lat->text());
   tmpData.append(ui->alt->text());
   ui->rgnlist->currentItem()->setData(RGN_Menu::LLA, tmpData);
}

void RGN_Menu::coeffs_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QStringList tmpData = {};
   tmpData.append(ui->elasticity->text());
   tmpData.append(ui->damping->text());
   tmpData.append(ui->friction->text());
   ui->rgnlist->currentItem()->setData(RGN_Menu::Coeffs, tmpData);
}

void RGN_Menu::geometry_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QString tmpData = {};
   tmpData         = ui->geometryname->text();
   ui->rgnlist->currentItem()->setData(RGN_Menu::GeometryFile, tmpData);
}

void RGN_Menu::on_exists_toggled(bool checked) {
   if (ui->rgnlist->currentRow() == -1)
      return;
   ui->rgnlist->currentItem()->setData(RGN_Menu::Exists,
                                       QVariant(checked).toString().toUpper());
}

void RGN_Menu::on_regionname_textChanged(const QString &arg1) {
   if (ui->rgnlist->currentRow() == -1)
      return;
   ui->rgnlist->currentItem()->setData(RGN_Menu::Name, arg1);
}

void RGN_Menu::clear_fields() {
   ui->exists->setChecked(false);
   ui->regionname->clear();
   ui->world->setCurrentIndex(0);
   ui->minorbodynum->setValue(0);
   ui->location->setCurrentIndex(0);
   ui->posw_x->clear();
   ui->posw_y->clear();
   ui->posw_z->clear();
   ui->lng->clear();
   ui->lat->clear();
   ui->alt->clear();
   ui->elasticity->clear();
   ui->damping->clear();
   ui->friction->clear();
   ui->geometryname->clear();
}

void RGN_Menu::on_rgn_duplicate_clicked() {
   int index                = ui->rgnlist->currentRow();
   QListWidgetItem *curItem = ui->rgnlist->currentItem();
   QStringList curNames     = dsm_gui_lib::getTextFromList(ui->rgnlist);

   if (index == -1)
      return;
   QString oldName = curItem->text();
   QString newName = oldName + "_Copy";
   for (int i = 0; i <= 30; i++) {
      QString newNameTest = newName;
      if (i > 0)
         newNameTest += "_" + QString::number(i);
      if (!curNames.contains(newNameTest)) {
         newName = newNameTest;
         break;
      }
   }
   QListWidgetItem *newItem = curItem->clone();
   newItem->setData(RGN_Menu::Name, newName);
   ui->rgnlist->addItem(newItem);
}

void RGN_Menu::on_location_currentTextChanged(const QString &arg1) {
   bool isLLA = !location_inputs["LLA"].compare(arg1);

   ui->posw_label->setEnabled(!isLLA);
   ui->posw_x->setEnabled(!isLLA);
   ui->posw_y->setEnabled(!isLLA);
   ui->posw_z->setEnabled(!isLLA);
   ui->lla_label->setEnabled(isLLA);
   ui->lng->setEnabled(isLLA);
   ui->lat->setEnabled(isLLA);
   ui->alt->setEnabled(isLLA);
}
