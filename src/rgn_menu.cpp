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

   connect(ui->world, &QComboBox::textActivated, this,
           &RGN_Menu::world_changed);
   connect(ui->minorbodynum, &QSpinBox::textChanged, this,
           &RGN_Menu::world_changed);

   connect(ui->location, &QComboBox::textActivated, this,
           &RGN_Menu::location_changed);

   connect(ui->posw_x, &QLineEdit::textEdited, this, &RGN_Menu::posw_changed);
   connect(ui->posw_y, &QLineEdit::textEdited, this, &RGN_Menu::posw_changed);
   connect(ui->posw_z, &QLineEdit::textEdited, this, &RGN_Menu::posw_changed);

   connect(ui->lng, &QLineEdit::textEdited, this, &RGN_Menu::lla_changed);
   connect(ui->lat, &QLineEdit::textEdited, this, &RGN_Menu::lla_changed);
   connect(ui->alt, &QLineEdit::textEdited, this, &RGN_Menu::lla_changed);

   connect(ui->elasticity, &QLineEdit::textEdited, this,
           &RGN_Menu::coeffs_changed);
   connect(ui->damping, &QLineEdit::textEdited, this,
           &RGN_Menu::coeffs_changed);
   connect(ui->friction, &QLineEdit::textEdited, this,
           &RGN_Menu::coeffs_changed);

   connect(ui->geometryname, &QLineEdit::textEdited, this,
           &RGN_Menu::geometry_changed);
   ui_posw = {ui->posw_x, ui->posw_y, ui->posw_z};
   ui_lla  = {ui->lng, ui->lat, ui->alt};
}

void RGN_Menu::receive_rgnpath(QString path) {

   inout_path = path;
   file_path  = path + "Inp_Region.yaml";
   apply_data();
}

void RGN_Menu::apply_data() {
   rgn_list_hash.clear();
   ui->rgnlist->clear();
   YAML::Node rgn_file_yaml = YAML::LoadFile(file_path.toStdString());
   YAML::Node rgns          = rgn_file_yaml["Regions"];

   for (YAML::iterator it = rgns.begin(); it != rgns.end(); ++it) {
      Region new_rgn = (*it)["Region"].as<Region>();
      QListWidgetItem *newRGN =
          new QListWidgetItem(new_rgn.name(), ui->rgnlist);
      rgn_list_hash.insert(newRGN, new_rgn);
   }
}

void RGN_Menu::on_rgn_remove_clicked() {
   int removeitem = ui->rgnlist->currentRow();
   if (removeitem == -1) {
      return;
   } else {
      QListWidgetItem *cur_item = ui->rgnlist->currentItem();
      rgn_list_hash.remove(cur_item);
      ui->rgnlist->takeItem(removeitem);
      ui->rgnlist->setCurrentRow(-1);
      clear_fields();
   }
}

void RGN_Menu::on_rgn_add_clicked() {
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

   QListWidgetItem *newRGN = new QListWidgetItem(newName, ui->rgnlist);
   Region new_rgn          = Region(newName);
   rgn_list_hash.insert(newRGN, new_rgn);

   ui->rgnlist->setCurrentRow(-1);
   clear_fields();
}

void RGN_Menu::on_rgnlist_itemClicked(QListWidgetItem *item) {
   const Region *region            = &rgn_list_hash[item];
   const Location location         = region->location();
   const Coefficients coefficients = region->coefficients();

   ui->exists->setChecked(region->exists());
   ui->regionname->setText(region->name());

   const QString world_name = region->world();
   if (world_name.contains("MINORBODY")) {
      dsm_gui_lib::setQComboBox(ui->world, "MINORBODY");
      QStringList split = world_name.split(QRegExp("_"), Qt::SkipEmptyParts);
      ui->minorbodynum->setValue(split[1].toInt());
   } else {
      dsm_gui_lib::setQComboBox(ui->world, world_name);
      ui->minorbodynum->setValue(0);
   }

   const QString loc_type = location.type();
   dsm_gui_lib::setQComboBox(ui->location, location_inputs[loc_type]);
   QVector3D pos                = location.position();
   QList<QLineEdit *> *this_pos = NULL, *other_pos = NULL;
   if (!loc_type.compare("LLA")) {
      this_pos  = &ui_lla;
      other_pos = &ui_posw;
   } else {
      this_pos  = &ui_posw;
      other_pos = &ui_lla;
   }
   for (int i = 0; i < 3; i++) {
      (*this_pos)[i]->setText(QString::number(pos[i]));
      (*other_pos)[i]->setText("0");
   }
   ui->elasticity->setText(QString::number(coefficients.elasticity()));
   ui->damping->setText(QString::number(coefficients.damping()));
   ui->friction->setText(QString::number(coefficients.friction()));
   ui->geometryname->setText(region->file_name());
}

void RGN_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Region file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_Region.yaml");
      QFile::copy(inout_path + "__default__/Inp_Region.yaml",
                  inout_path + "Inp_Region.yaml");
      apply_data();
   } else {
      return;
   }
}

void RGN_Menu::on_savedefaultButton_clicked() {
   int response =
       dsm_gui_lib::warning_message("Overwrite Default Region file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_Region.yaml");
      QFile::copy(inout_path + "Inp_Region.yaml",
                  inout_path + "__default__/Inp_Region.yaml");
      apply_data();
   } else {
      return;
   }
}

void RGN_Menu::on_closeButton_clicked() {
   RGN_Menu::close();
}

void RGN_Menu::on_applyButton_clicked() {
   YAML::Node rgn_file_yaml(YAML::NodeType::Map);
   YAML::Node rgn_list_yaml(YAML::NodeType::Sequence);
   rgn_file_yaml["Regions"] = rgn_list_yaml;

   for (auto pair = rgn_list_hash.constKeyValueBegin();
        pair != rgn_list_hash.constKeyValueEnd(); ++pair) {
      YAML::Node item(YAML::NodeType::Map);
      item["Region"] = (*pair).second;
      rgn_list_yaml.push_back(item);
   }
   dsm_gui_lib::write_data(file_path, rgn_file_yaml);
}

void RGN_Menu::world_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QString world = ui->world->currentText();

   bool isMinor = !world.compare("MINORBODY");
   ui->minorbodylabel->setEnabled(isMinor);
   ui->minorbodynum->setEnabled(isMinor);

   if (isMinor)
      world += "_" + ui->minorbodynum->text();
   else
      ui->minorbodynum->setValue(0);

   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   region->setWorld(world);
}

void RGN_Menu::location_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   const QString loc_type    = location_inputs.key(ui->location->currentText());
   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   region->setLocation(loc_type);
   if (!loc_type.compare("LLA")) {
      lla_changed();
   } else {
      posw_changed();
   }
}

void RGN_Menu::posw_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;

   QVector3D pos;
   for (int i = 0; i < 3; i++)
      pos[i] = ui_posw[i]->text().toDouble();

   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   const Location loc        = region->location();
   const QString loc_type    = loc.type();
   if (loc_type.compare("POSW"))
      return;
   region->setLocation(pos);
}

void RGN_Menu::lla_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QVector3D pos;
   for (int i = 0; i < 3; i++)
      pos[i] = ui_lla[i]->text().toDouble();

   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   const Location loc        = region->location();
   const QString loc_type    = loc.type();
   if (loc_type.compare("LLA"))
      return;
   region->setLocation(pos);
}

void RGN_Menu::coeffs_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;

   Coefficients coeffs;

   coeffs.setElasticity(ui->elasticity->text().toDouble());
   coeffs.setDamping(ui->damping->text().toDouble());
   coeffs.setFriction(ui->friction->text().toDouble());
   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   region->setCoefficients(coeffs);
}

void RGN_Menu::geometry_changed() {
   if (ui->rgnlist->currentRow() == -1)
      return;

   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   region->setFileName(ui->geometryname->text());
}

void RGN_Menu::on_exists_toggled(bool checked) {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   region->setExists(checked);
}

void RGN_Menu::on_regionname_textChanged(const QString &arg1) {
   if (ui->rgnlist->currentRow() == -1)
      return;
   QListWidgetItem *cur_item = ui->rgnlist->currentItem();
   Region *region            = &rgn_list_hash[cur_item];
   region->setName(arg1);
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
   newItem->setText(newName);
   ui->rgnlist->addItem(newItem);

   Region new_rgn = rgn_list_hash[curItem];
   new_rgn.setName(newName);
   rgn_list_hash.insert(newItem, new_rgn);
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
