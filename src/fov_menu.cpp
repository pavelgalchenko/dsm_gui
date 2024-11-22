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

   connect(ui->num_sides, &QLineEdit::textChanged, this,
           &FOV_Menu::sides_changed);
   connect(ui->length_sides, &QLineEdit::textChanged, this,
           &FOV_Menu::sides_changed);

   connect(ui->horizontal_width, &QLineEdit::textChanged, this,
           &FOV_Menu::dims_changed);
   connect(ui->vertical_height, &QLineEdit::textChanged, this,
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

   connect(ui->sc_name, &QComboBox::currentTextChanged, this,
           &FOV_Menu::scbody_changed);
   connect(ui->bdy_num, &QSpinBox::textChanged, this,
           &FOV_Menu::scbody_changed);

   connect(ui->pos_x, &QLineEdit::textChanged, this, &FOV_Menu::pos_changed);
   connect(ui->pos_y, &QLineEdit::textChanged, this, &FOV_Menu::pos_changed);
   connect(ui->pos_z, &QLineEdit::textChanged, this, &FOV_Menu::pos_changed);

   connect(ui->rot1, &QLineEdit::textChanged, this, &FOV_Menu::euler_changed);
   connect(ui->rot2, &QLineEdit::textChanged, this, &FOV_Menu::euler_changed);
   connect(ui->rot3, &QLineEdit::textChanged, this, &FOV_Menu::euler_changed);
   connect(ui->euler_seq, &QComboBox::currentTextChanged, this,
           &FOV_Menu::euler_changed);
}

void FOV_Menu::receive_fovpath(QString path) {
   inout_path = path;
   file_path  = path + "Inp_FOV.txt";
   receive_data();
   apply_data();
}

void FOV_Menu::receive_data() {

   fov_file_yaml = YAML::LoadFile(file_path.toStdString());

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

void FOV_Menu::write_data() {
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      for (int i = 0; i < fov_update.size(); i++) {
         in << fov_update.at(i);
      }
   }
   fov_update.clear();
   file.close();
}

void FOV_Menu::apply_data() {
   fov_list_hash.clear();
   QStringList line_items;
   int num_fov = 0;

   QStringList tmpData;

   YAML::Node fovs = fov_file_yaml["FOVs"];

   for (YAML::iterator it = fovs.begin(); it != fovs.end(); ++it) {
      QListWidgetItem *newFOV =
          new QListWidgetItem((*it)["Label"].as<QString>(), ui->fovlist);
      fov_list_hash.insert(newFOV, *it);
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

   size_t n_fovs = fov_file_yaml["FOVs"].size();
   fov_file_yaml["FOVs"].push_back(new_fov);
   fov_list_hash.insert(newFOV, fov_file_yaml["FOVs"][n_fovs]);

   ui->fovlist->setCurrentRow(-1);
   clear_fields();
}

void FOV_Menu::on_fovlist_itemClicked(QListWidgetItem *item) {
   const FOV fov           = fov_list_hash[item].as<FOV>();
   const Sides sides       = fov.sides();
   const Color color       = fov.color();
   const EulerAngles euler = fov.euler_angles();

   const QList<QLineEdit *> ui_pos = {ui->pos_x, ui->pos_y, ui->pos_z};
   const QList<QLineEdit *> ui_ang = {ui->rot1, ui->rot2, ui->rot3};

   ui->fov_name->setText(fov.label());
   ui->horizontal_width->setText(QString::number(fov.width()));
   ui->vertical_height->setText(QString::number(fov.height()));
   dsm_gui_lib::setQComboBox(ui->fov_type, fov.type());
   ui->nearfield->setChecked(fov.near_field());
   ui->farfield->setChecked(fov.far_field());
   dsm_gui_lib::setQComboBox(ui->sc_name, scNums.key(fov.sc()));
   ui->bdy_num->setValue(fov.body());
   dsm_gui_lib::setQComboBox(ui->boresightaxis, fov.boresight());

   QVector3D pos = fov.position();
   QVector3D ang = euler.getAngles();
   for (int i = 0; i < 3; i++) {
      ui_pos[i]->setText(QString::number(pos[i]));
      ui_ang[i]->setText(QString::number(ang[i]));
   }
   dsm_gui_lib::setQComboBox(ui->euler_seq,
                             QString::number(euler.getSequence()));

   ui->num_sides->setText(QString::number(sides.n()));
   ui->length_sides->setText(QString::number(sides.length()));

   ui->redvalue->setValue(color.red());
   ui->greenvalue->setValue(color.green());
   ui->bluevalue->setValue(color.blue());
   ui->alphavalue->setValue(color.alpha());
}

void FOV_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite FOV file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_FOV.txt");
      QFile::copy(inout_path + "__default__/Inp_FOV.txt",
                  inout_path + "Inp_FOV.txt");
      receive_data();
      apply_data();
   } else {
      return;
   }
}

void FOV_Menu::on_savedefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Default FOV file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_FOV.txt");
      QFile::copy(inout_path + "Inp_FOV.txt",
                  inout_path + "__default__/Inp_FOV.txt");
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
   QString data_inp;
   QStringList tmpData = {};
   QListWidgetItem *item;

   int fov_num = ui->fovlist->count();

   fov_update.append("**************************** Fields of View "
                     "****************************\n");

   data_inp = QString::number(fov_num);
   fov_update.append(dsm_gui_lib::whitespace(data_inp) +
                     " !  Number of FOVs\n");

   for (int i = 0; i < fov_num; i++) {
      item = ui->fovlist->item(i);
      for (int j = 0; j < fovNLines; j++) {
         switch (j) {
            case 0:
               data_inp = "----------------------------------------------------"
                          "--------------------\n";
               break;
            case 1:
               tmpData  = item->data(FOV_Menu::Label).toStringList();
               data_inp = "\"" + tmpData[0] + "\"";
               data_inp = dsm_gui_lib::whitespace(data_inp) + " !  Label\n";
               break;
            case 2:
               tmpData  = item->data(FOV_Menu::Sides).toStringList();
               data_inp = tmpData.join("  ");
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  Number of Sides, Length [m]\n";
               break;
            case 3:
               tmpData  = item->data(FOV_Menu::Dims).toStringList();
               data_inp = tmpData.join("  ");
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  H Width, V Height [deg]\n";
               break;
            case 4:
               tmpData = item->data(FOV_Menu::Color).toStringList();
               for (int k = 0; k < 4; k++) {
                  double colorChannel = tmpData[k].toDouble() / 255.0;
                  data_inp += QString::number(colorChannel, 'g', 3) + "  ";
               }
               data_inp =
                   dsm_gui_lib::whitespace(data_inp) + " !  Color RGB+Alpha\n";
               break;
            case 5:
               tmpData  = item->data(FOV_Menu::Type).toStringList();
               data_inp = tmpData[0];
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  WIREFRAME, SOLID, VECTOR, or PLANE\n";
               break;
            case 6:
               tmpData  = item->data(FOV_Menu::DrawField).toStringList();
               data_inp = tmpData.join("  ");
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  Draw Near Field, Draw Far Field\n";
               break;
            case 7:
               tmpData  = item->data(FOV_Menu::SCBody).toStringList();
               data_inp = tmpData.join("  ");
               data_inp = dsm_gui_lib::whitespace(data_inp) + " !  SC, Body\n";
               break;
            case 8:
               tmpData  = item->data(FOV_Menu::BodyPos).toStringList();
               data_inp = tmpData.join("  ");
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  Position in Body [m]\n";
               break;
            case 9:
               tmpData  = item->data(FOV_Menu::Euler).toStringList();
               data_inp = tmpData.join("  ");
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  Euler Angles [deg], Sequence\n";
               break;
            case 10:
               tmpData  = item->data(FOV_Menu::Boresight).toStringList();
               data_inp = tmpData[0];
               data_inp = dsm_gui_lib::whitespace(data_inp) +
                          " !  Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n";
               break;
         }
         fov_update.append(data_inp);
         data_inp.clear();
      }
   }

   write_data();
}

void FOV_Menu::sides_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;
   Sides sides(ui->num_sides->text().toInt(),
               ui->length_sides->text().toDouble());
   YAML::Node fov = fov_list_hash[ui->fovlist->currentItem()];
   fov["Sides"]   = sides;
}

void FOV_Menu::dims_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   YAML::Node fov = fov_list_hash[ui->fovlist->currentItem()];
   fov["Width"]   = ui->horizontal_width->text().toDouble();
   fov["Height"]  = ui->vertical_height->text().toDouble();
}

void FOV_Menu::color_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   QList<QSpinBox *> ui_rgb = {ui->redvalue, ui->greenvalue, ui->bluevalue};
   QVector3D rgb;
   for (int i = 0; i < 3; i++)
      rgb[i] = ui_rgb[i]->value() / 255.0;
   Color color(rgb, ui->alphavalue->value() / 255.0);

   YAML::Node fov = fov_list_hash[ui->fovlist->currentItem()];
   fov["Color"]   = color;
}

void FOV_Menu::on_fov_name_textChanged(const QString &arg1) {
   if (ui->fovlist->currentRow() == -1)
      return;
   YAML::Node fov = fov_list_hash[ui->fovlist->currentItem()];
   fov["Label"]   = arg1;
}

void FOV_Menu::field_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;
   QStringList tmpData;

   YAML::Node fov    = fov_list_hash[ui->fovlist->currentItem()];
   fov["Far Field"]  = ui->farfield->isChecked();
   fov["Near Field"] = ui->nearfield->isChecked();
}

void FOV_Menu::scbody_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   YAML::Node fov = fov_list_hash[ui->fovlist->currentItem()];
   int scNum      = scNums[ui->sc_name->currentText()];
   fov["SC"]      = scNum;
   fov["Body"]    = ui->bdy_num->value();
}

void FOV_Menu::pos_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   const QList<QLineEdit *> ui_pos = {ui->pos_x, ui->pos_y, ui->pos_z};
   YAML::Node fov                  = fov_list_hash[ui->fovlist->currentItem()];
   QVector3D pos;
   for (int i = 0; i < 3; i++)
      pos[i] = ui_pos[i]->text().toDouble();
   fov["Position"] = pos;
}

void FOV_Menu::euler_changed() {
   if (ui->fovlist->currentRow() == -1)
      return;

   YAML::Node fov                  = fov_list_hash[ui->fovlist->currentItem()];
   const QList<QLineEdit *> ui_ang = {ui->rot1, ui->rot2, ui->rot3};
   QVector3D ang;
   for (int i = 0; i < 3; i++)
      ang[i] = ui_ang[i]->text().toDouble();

   EulerAngles euler(ang, ui->euler_seq->currentText().toInt());
   fov["Euler Angles"] = euler;
}

void FOV_Menu::on_boresightaxis_currentTextChanged(const QString &arg1) {
   if (ui->fovlist->currentRow() == -1)
      return;

   YAML::Node fov   = fov_list_hash[ui->fovlist->currentItem()];
   fov["Boresight"] = axis_inputs.key(arg1);
}

void FOV_Menu::on_fov_type_currentTextChanged(const QString &arg1) {
   if (ui->fovlist->currentRow() == -1)
      return;

   YAML::Node fov = fov_list_hash[ui->fovlist->currentItem()];
   fov["Type"]    = fovtype_inputs.key(arg1);
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
}

void FOV_Menu::on_sc_name_currentTextChanged(const QString &arg1) {
   QStringList scFileNames =
       QDir(inout_path).entryList({"SC_" + arg1 + ".txt"});
   if (scFileNames.isEmpty())
      return;

   ui->bdy_num->setMaximum(
       dsm_gui_lib::get_sc_nitems(inout_path, arg1,
                                  dsm_gui_lib::scSectionType::BODY) -
       1);
}
