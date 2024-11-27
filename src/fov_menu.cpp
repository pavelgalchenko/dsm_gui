#include "fov_menu.h"
#include "ui_fov_menu.h"

FOV_Menu::FOV_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::FOV_Menu) {
   ui->setupUi(this);
   set_validators();
}

FOV_Menu::~FOV_Menu() {
   delete ui;
}

void FOV_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");

   ui->fovlist->setSortingEnabled(true);
   ui->fovlist->sortItems(Qt::AscendingOrder);

   ui->num_sides->setValidator(new QIntValidator);
   ui->length_sides->setValidator(new QDoubleValidator);
   ui->horizontal_width->setValidator(new QDoubleValidator);
   ui->vertical_height->setValidator(new QDoubleValidator);
   ui->fov_name->setValidator(new QRegularExpressionValidator(rx));
   ui->fov_type->addItems(dsm_gui_lib::sortStringList(fovtype_inputs.values()));
   ui->bdy_num->setMinimum(0);
   ui->bdy_num->setMaximum(INFINITY);
   ui->pos_x->setValidator(new QDoubleValidator);
   ui->pos_y->setValidator(new QDoubleValidator);
   ui->pos_z->setValidator(new QDoubleValidator);
   ui->rot1->setValidator(new QDoubleValidator);
   ui->rot2->setValidator(new QDoubleValidator);
   ui->rot3->setValidator(new QDoubleValidator);
   ui->euler_seq->addItems(dsm_gui_lib::eulerInputs);
   ui->boresightaxis->addItems(
       dsm_gui_lib::sortStringList(axis_inputs.values()));

   connect(ui->num_sides, &QLineEdit::textEdited, this,
           &FOV_Menu::sides_changed);
   connect(ui->length_sides, &QLineEdit::textEdited, this,
           &FOV_Menu::sides_changed);

   connect(ui->horizontal_width, &QLineEdit::textEdited, this,
           &FOV_Menu::dims_changed);
   connect(ui->vertical_height, &QLineEdit::textEdited, this,
           &FOV_Menu::dims_changed);

   connect(ui->redvalue, &QSpinBox::textChanged, this,
           &FOV_Menu::color_changed);
   connect(ui->greenvalue, &QSpinBox::textChanged, this,
           &FOV_Menu::color_changed);
   connect(ui->bluevalue, &QSpinBox::textChanged, this,
           &FOV_Menu::color_changed);
   connect(ui->alphavalue, &QSpinBox::textChanged, this,
           &FOV_Menu::color_changed);

   connect(ui->nearfield, &QCheckBox::toggled, this, &FOV_Menu::field_changed);
   connect(ui->farfield, &QCheckBox::toggled, this, &FOV_Menu::field_changed);

   connect(ui->sc_name, &QComboBox::textActivated, this,
           &FOV_Menu::scbody_changed);
   connect(ui->bdy_num, &QSpinBox::textChanged, this,
           &FOV_Menu::scbody_changed);

   connect(ui->pos_x, &QLineEdit::textEdited, this, &FOV_Menu::pos_changed);
   connect(ui->pos_y, &QLineEdit::textEdited, this, &FOV_Menu::pos_changed);
   connect(ui->pos_z, &QLineEdit::textEdited, this, &FOV_Menu::pos_changed);

   connect(ui->rot1, &QLineEdit::textEdited, this, &FOV_Menu::euler_changed);
   connect(ui->rot2, &QLineEdit::textEdited, this, &FOV_Menu::euler_changed);
   connect(ui->rot3, &QLineEdit::textEdited, this, &FOV_Menu::euler_changed);
   connect(ui->euler_seq, &QComboBox::textActivated, this,
           &FOV_Menu::euler_changed);
}

void FOV_Menu::receive_fovpath(QString path) {
   inout_path = path;
   file_path  = path + "Inp_FOV.yaml";
   receive_data();
   apply_data();
}

void FOV_Menu::receive_data() {

   YAML::Node fov_file_yaml = YAML::LoadFile(file_path.toStdString());

   QStringList scFiles = QDir(inout_path).entryList({"SC_*.yaml"});
   QStringList scNames;
   for (int i = 0; i < scFiles.length(); i++)
      scNames.append(scFiles[i].chopped(5).mid(3));
   QSet<QString> scNamesSet(scNames.begin(), scNames.end());

   YAML::Node sim_yaml =
       YAML::LoadFile((inout_path + "Inp_Sim.yaml").toStdString());
   YAML::Node scs = sim_yaml["SCs"];

   QStringList simSCNames;
   QHash<QString, int> simSCNum;
   for (YAML::iterator it = scs.begin(); it != scs.end(); ++it) {
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

   ui->sc_name->clear();
   ui->sc_name->addItems(dsm_gui_lib::sortStringList(scNums.keys()));
}

void FOV_Menu::write_data(YAML::Node yaml) {
   QStringList params;

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

void FOV_Menu::apply_data() {
   fov_list_hash.clear();
   ui->fovlist->clear();

   YAML::Node fov_file_yaml = YAML::LoadFile(file_path.toStdString());
   YAML::Node fovs          = fov_file_yaml["FOVs"];

   for (YAML::iterator it = fovs.begin(); it != fovs.end(); ++it) {
      FOV new_fov = (*it).as<FOV>();
      QListWidgetItem *newFOV =
          new QListWidgetItem(new_fov.label(), ui->fovlist);
      fov_list_hash.insert(newFOV, new_fov);
   }
}

void FOV_Menu::on_pickcolor_clicked() {

   QStringList rgbavalues;
   int r = ui->redvalue->value();
   int g = ui->greenvalue->value();
   int b = ui->bluevalue->value();
   int a = ui->alphavalue->value();

   QColor color(r, g, b, a);
   QColor newColor = QColorDialog::getColor(color, this, "Choose Color",
                                            QColorDialog::ShowAlphaChannel);

   ui->redvalue->setValue(newColor.red());
   ui->greenvalue->setValue(newColor.green());
   ui->bluevalue->setValue(newColor.blue());
   ui->alphavalue->setValue(newColor.alpha());
}

void FOV_Menu::on_fov_remove_clicked() {
   int removeitem = ui->fovlist->currentRow();
   if (removeitem == -1) {
      return;
   } else {
      QListWidgetItem *cur_item = ui->fovlist->currentItem();
      fov_list_hash.remove(cur_item);
      ui->fovlist->takeItem(removeitem);
      ui->fovlist->setCurrentRow(-1);
      clear_fields();
   }
}

void FOV_Menu::on_fov_add_clicked() {
   QString newName      = "NEW FOV";
   QStringList curNames = dsm_gui_lib::getTextFromList(ui->fovlist);
   if (ui->fovlist->count() != 0) {
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
   QListWidgetItem *newFOV = new QListWidgetItem(newName, ui->fovlist);
   FOV new_fov             = FOV(newName);

   fov_list_hash.insert(newFOV, new_fov);

   ui->fovlist->setCurrentRow(-1);
   clear_fields();
}

void FOV_Menu::on_fovlist_itemClicked(QListWidgetItem *item) {
   const FOV *fov          = &fov_list_hash[item];
   const Sides sides       = fov->sides();
   const Color color       = fov->color();
   const EulerAngles euler = fov->euler_angles();

   const QList<QLineEdit *> ui_pos = {ui->pos_x, ui->pos_y, ui->pos_z};
   const QList<QLineEdit *> ui_ang = {ui->rot1, ui->rot2, ui->rot3};

   ui->fov_name->setText(fov->label());
   ui->horizontal_width->setText(QString::number(fov->width()));
   ui->vertical_height->setText(QString::number(fov->height()));
   dsm_gui_lib::setQComboBox(ui->fov_type, fovtype_inputs[fov->type()]);
   ui->nearfield->setChecked(fov->near_field());
   ui->farfield->setChecked(fov->far_field());
   dsm_gui_lib::setQComboBox(ui->sc_name, scNums.key(fov->sc()));
   ui->bdy_num->setValue(fov->body());
   dsm_gui_lib::setQComboBox(ui->boresightaxis, axis_inputs[fov->boresight()]);

   QVector3D pos = fov->position();
   QVector3D ang = euler.getAngles();
   for (int i = 0; i < 3; i++) {
      ui_pos[i]->setText(QString::number(pos[i]));
      ui_ang[i]->setText(QString::number(ang[i]));
   }
   dsm_gui_lib::setQComboBox(ui->euler_seq,
                             QString::number(euler.getSequence()));

   ui->num_sides->setText(QString::number(sides.n()));
   ui->length_sides->setText(QString::number(sides.length()));

   ui->redvalue->setValue(color.red() * 255);
   ui->greenvalue->setValue(color.green() * 255);
   ui->bluevalue->setValue(color.blue() * 255);
   ui->alphavalue->setValue(color.alpha() * 255);
}

void FOV_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite FOV file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_FOV.yaml");
      QFile::copy(inout_path + "__default__/Inp_FOV.yaml",
                  inout_path + "Inp_FOV.yaml");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void FOV_Menu::on_savedefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Default FOV file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_FOV.yaml");
      QFile::copy(inout_path + "Inp_FOV.yaml",
                  inout_path + "__default__/Inp_FOV.yaml");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void FOV_Menu::on_closeButton_clicked() {
   FOV_Menu::close();
}

void FOV_Menu::on_applyButton_clicked() {
   YAML::Node fov_file_yaml(YAML::NodeType::Map);
   fov_file_yaml["FOVs"] = fov_list_hash.values();

   write_data(fov_file_yaml);
}

void FOV_Menu::sides_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;
   Sides sides(ui->num_sides->text().toInt(),
               ui->length_sides->text().toDouble());
   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setSides(sides);
}

void FOV_Menu::dims_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setHeight(ui->horizontal_width->text().toDouble());
   fov->setWidth(ui->vertical_height->text().toDouble());
}

void FOV_Menu::color_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   QList<QSpinBox *> ui_rgb = {ui->redvalue, ui->greenvalue, ui->bluevalue};
   QVector3D rgb;
   for (int i = 0; i < 3; i++)
      rgb[i] = ui_rgb[i]->value() / 255.0;
   Color color(rgb, ui->alphavalue->value() / 255.0);

   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setColor(color);
}

void FOV_Menu::on_fov_name_textChanged(const QString &arg1) {
   if (ui->fovlist->currentRow() == -1)
      return;
   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setLabel(arg1);
}

void FOV_Menu::field_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;
   QStringList tmpData;

   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setFarField(ui->farfield->isChecked());
   fov->setNearField(ui->nearfield->isChecked());
}

void FOV_Menu::scbody_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   FOV *fov  = &fov_list_hash[ui->fovlist->currentItem()];
   int scNum = scNums[ui->sc_name->currentText()];
   fov->setSC(scNum);
   fov->setBody(ui->bdy_num->value());
}

void FOV_Menu::pos_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   const QList<QLineEdit *> ui_pos = {ui->pos_x, ui->pos_y, ui->pos_z};
   FOV *fov                        = &fov_list_hash[ui->fovlist->currentItem()];
   QVector3D pos;
   for (int i = 0; i < 3; i++)
      pos[i] = ui_pos[i]->text().toDouble();
   fov->setPosition(pos);
}

void FOV_Menu::euler_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   FOV *fov                        = &fov_list_hash[ui->fovlist->currentItem()];
   const QList<QLineEdit *> ui_ang = {ui->rot1, ui->rot2, ui->rot3};
   QVector3D ang;
   for (int i = 0; i < 3; i++)
      ang[i] = ui_ang[i]->text().toDouble();

   EulerAngles euler(ang, ui->euler_seq->currentText().toInt());
   fov->setEulerAngles(euler);
}

void FOV_Menu::on_boresightaxis_currentTextChanged(const QString &arg1) {
   if (ui->fovlist->currentRow() == -1)
      return;

   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setBoresight(axis_inputs.key(arg1));
}

void FOV_Menu::on_fov_type_currentTextChanged(const QString &arg1) {
   if (ui->fovlist->currentRow() == -1)
      return;

   FOV *fov = &fov_list_hash[ui->fovlist->currentItem()];
   fov->setType(fovtype_inputs.key(arg1));
}

void FOV_Menu::clear_fields() {
   ui->fov_name->clear();
   ui->num_sides->clear();
   ui->length_sides->clear();
   ui->horizontal_width->clear();
   ui->vertical_height->clear();
   ui->redvalue->setValue(0);
   ui->greenvalue->setValue(0);
   ui->bluevalue->setValue(0);
   ui->alphavalue->setValue(0);
   ui->fov_type->setCurrentIndex(0);
   ui->nearfield->setChecked(false);
   ui->farfield->setChecked(false);
   ui->sc_name->setCurrentIndex(0);
   ui->bdy_num->clear();
   ui->pos_x->clear();
   ui->pos_y->clear();
   ui->pos_z->clear();
   ui->rot1->clear();
   ui->rot2->clear();
   ui->rot3->clear();
   ui->euler_seq->setCurrentIndex(0);
   ui->boresightaxis->setCurrentIndex(0);
}

void FOV_Menu::on_fov_duplicate_clicked() {
   int index                = ui->fovlist->currentRow();
   QListWidgetItem *curItem = ui->fovlist->currentItem();
   QStringList curNames     = dsm_gui_lib::getTextFromList(ui->fovlist);

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
   ui->fovlist->addItem(newItem);

   FOV new_fov = fov_list_hash[curItem];
   new_fov.setLabel(newName);
   fov_list_hash.insert(newItem, new_fov);
}

void FOV_Menu::on_sc_name_currentTextChanged(const QString &arg1) {
   QStringList scFileNames =
       QDir(inout_path).entryList({"SC_" + arg1 + ".yaml"});
   if (scFileNames.isEmpty())
      return;

   ui->bdy_num->setMaximum(
       dsm_gui_lib::get_sc_nitems(inout_path, arg1,
                                  dsm_gui_lib::scSectionType::BODY) -
       1);
}
