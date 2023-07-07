#include "fov_menu.h"
#include "ui_fov_menu.h"

FOV_Menu::FOV_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FOV_Menu)
{
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
    ui->euler_seq->addItems(euler_inputs);
    ui->boresightaxis->addItems(dsm_gui_lib::sortStringList(axis_inputs.values()));

    connect(ui->num_sides, &QLineEdit::textChanged, this, &FOV_Menu::sides_changed);
    connect(ui->length_sides, &QLineEdit::textChanged, this, &FOV_Menu::sides_changed);

    connect(ui->horizontal_width, &QLineEdit::textChanged, this, &FOV_Menu::dims_changed);
    connect(ui->vertical_height, &QLineEdit::textChanged, this, &FOV_Menu::dims_changed);

    connect(ui->redvalue, &QSpinBox::textChanged, this, &FOV_Menu::color_changed);
    connect(ui->greenvalue, &QSpinBox::textChanged, this, &FOV_Menu::color_changed);
    connect(ui->bluevalue, &QSpinBox::textChanged, this, &FOV_Menu::color_changed);
    connect(ui->alphavalue, &QSpinBox::textChanged, this, &FOV_Menu::color_changed);

    connect(ui->nearfield, &QCheckBox::toggled, this, &FOV_Menu::field_changed);
    connect(ui->farfield, &QCheckBox::toggled, this, &FOV_Menu::field_changed);

    connect(ui->sc_name, &QComboBox::currentTextChanged, this, &FOV_Menu::scbody_changed);
    connect(ui->bdy_num, &QSpinBox::textChanged, this, &FOV_Menu::scbody_changed);

    connect(ui->pos_x, &QLineEdit::textChanged, this, &FOV_Menu::pos_changed);
    connect(ui->pos_y, &QLineEdit::textChanged, this, &FOV_Menu::pos_changed);
    connect(ui->pos_z, &QLineEdit::textChanged, this, &FOV_Menu::pos_changed);

    connect(ui->rot1, &QLineEdit::textChanged, this, &FOV_Menu::euler_changed);
    connect(ui->rot2, &QLineEdit::textChanged, this, &FOV_Menu::euler_changed);
    connect(ui->rot3, &QLineEdit::textChanged, this, &FOV_Menu::euler_changed);
    connect(ui->euler_seq, &QComboBox::currentTextChanged, this, &FOV_Menu::euler_changed);
}

void FOV_Menu::receive_fovpath(QString path) {
    inout_path = path;
    file_path = path+"Inp_FOV.txt";
    receive_data();
    apply_data();
}

void FOV_Menu::receive_data() {
    fov_data.clear();
    fov_string.clear();
    fov_update.clear();
    static QRegularExpression rx1("(.*?)!");
    static QRegularExpression rx2("\"(.*?)\"");
    static QRegularExpression rx3("SC_(.*).txt");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        fov_data.append(match1.captured(1));

        QRegularExpressionMatch match2 = rx2.match(line);
        fov_string.append(match2.captured(1));
    }
    file.close();

    QStringList scFiles = QDir(inout_path).entryList({"SC_*"});
    QStringList scNames;
    for (int i = 0; i<scFiles.length(); i++)
        scNames.append(scFiles[i].chopped(4).mid(3));
    QSet<QString> scNamesSet(scNames.begin(),scNames.end());

    QFile simFile(inout_path + "Inp_Sim.txt");
    if(!simFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", simFile.errorString());
    QTextStream simIn(&simFile);

    QStringList simSCNames;
    QHash<QString,int> simSCNum;
    while (!simIn.atEnd()) {
        QString line = simIn.readLine();
        if (line.contains("Spacecraft",Qt::CaseInsensitive)) {
            line = simIn.readLine();
            QStringList line_items = line.remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
            int nSC = line_items[0].toInt();
            for (int i=0; i<nSC; i++) {
                line = simIn.readLine();
                QString name = rx3.match(line).captured(1);
                simSCNames.append(name);
                simSCNum.insert(name,i);
            }
            break;
        }
    }
    simFile.close();
    QSet<QString> simSCNamesSet(simSCNames.begin(),simSCNames.end());

    scNums.clear();
    QStringList spaceCrafts = scNamesSet.intersect(simSCNamesSet).values();
    for (int i = 0; i < spaceCrafts.count(); i++)
        scNums.insert(spaceCrafts[i],simSCNum[spaceCrafts[i]]);

    ui->sc_name->clear();
    ui->sc_name->addItems(dsm_gui_lib::sortStringList(scNums.keys()));
}

void FOV_Menu::write_data() {
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < fov_update.size(); i++){
            in << fov_update.at(i);
        }
    }
    fov_update.clear();
    file.close();
}

void FOV_Menu::apply_data() {
    QStringList line_items;
    QListWidgetItem* newFOV;
    int num_fov = 0;

    QStringList tmpData;

    line_items = fov_data[1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
    num_fov = line_items[0].toInt();

    ui->fovlist->clear();
    clear_fields();
    for (int i=0; i < num_fov; i++) {
        newFOV = new QListWidgetItem();
        for (int j = 0; j < fovNLines-1; j++) {
            int k = i*fovNLines + 3 + j;
            line_items = fov_data[k].split(QRegExp("\\s"), Qt::SkipEmptyParts);
            switch (j) {
            case 0:
                tmpData.append(fov_string[k]);
                newFOV->setData(FOV_Menu::Label,tmpData);
                break;
            case 1:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                newFOV->setData(FOV_Menu::Sides,tmpData);
                break;
            case 2:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                newFOV->setData(FOV_Menu::Dims, tmpData);
                break;
            case 3:
                for (int i=0; i<4; i++) {
                    int colorChannel = line_items[i].toDouble()*255.0;
                    tmpData.append(QString::number(colorChannel));
                }
                newFOV->setData(FOV_Menu::Color, tmpData);
                break;
            case 4:
                tmpData.append(line_items[0]);
                newFOV->setData(FOV_Menu::Type,tmpData);
                break;
            case 5:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                newFOV->setData(FOV_Menu::DrawField, tmpData);
                break;
            case 6:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                newFOV->setData(FOV_Menu::SCBody, tmpData);
                break;
            case 7:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                tmpData.append(line_items[2]);
                newFOV->setData(FOV_Menu::BodyPos, tmpData);
                break;
            case 8:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                tmpData.append(line_items[2]);
                tmpData.append(line_items[3]);
                newFOV->setData(FOV_Menu::Euler, tmpData);
                break;
            case 9:
                tmpData.append(line_items[0]);
                newFOV->setData(FOV_Menu::Boresight, tmpData);
                break;
            default:
                break;
            }

            tmpData.clear();
        }
        ui->fovlist->addItem(newFOV);
    }
}

void FOV_Menu::on_pickcolor_clicked() {

    QStringList rgbavalues;
    int r = ui->redvalue->value();
    int g = ui->greenvalue->value();
    int b = ui->bluevalue->value();
    int a = ui->alphavalue->value();

    QColor color(r,g,b,a);
    QColor newColor = QColorDialog::getColor(color,this,"Choose Color",QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

    ui->redvalue->setValue(newColor.red());
    ui->greenvalue->setValue(newColor.green());
    ui->bluevalue->setValue(newColor.blue());
    ui->alphavalue->setValue(newColor.alpha());
}

void FOV_Menu::on_fov_remove_clicked() {
    int removeitem = ui->fovlist->currentRow();
    if (removeitem == -1) {
        return;
    }
    else {
        ui->fovlist->takeItem(removeitem);
        ui->fovlist->setCurrentRow(-1);
        clear_fields();
    }
}

void FOV_Menu::on_fov_add_clicked() {
    QListWidgetItem *newFOV = new QListWidgetItem();
    QStringList line_items;

    QStringList newData;
    QStringList tmpData;

    QString newName = "NEW FOV";
    QStringList curNames = getTextFromList(ui->fovlist);
    if (ui->fovlist->count() != 0) {
        for(int i = 0; i <= 50; i++) {
            QString newNameTest = newName;
            if (i>0) newNameTest += "_" + QString::number(i);
            if (!curNames.contains(newNameTest)) {
                newName = newNameTest;
                break;
            }
            if (i==50) return; // Nothing happens if too many
        }
    }

    newData.append("");
    newData.append("4   4.0                           ");
    newData.append("8.0  4.0                          ");
    newData.append("0.0 1.0 0.0 0.5                   ");
    newData.append("SOLID                             ");
    newData.append("TRUE  TRUE                        ");
    newData.append("0  0                              ");
    newData.append("0.0  0.0  1.0                     ");
    newData.append("0.0  0.0  0.0  321                ");
    newData.append("Z_AXIS                            ");
    newData.append("");

    for (int j = 0; j < fovNLines-1; j++) {
        line_items = newData[j].split(QRegExp("\\s"), Qt::SkipEmptyParts);
        switch (j) {
        case 0:
            tmpData.append(newName);
            newFOV->setData(FOV_Menu::Label,tmpData);
            break;
        case 1:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            newFOV->setData(FOV_Menu::Sides,tmpData);
            break;
        case 2:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            newFOV->setData(FOV_Menu::Dims, tmpData);
            break;
        case 3:
            for (int i=0; i<4; i++) {
                int colorChannel = line_items[i].toDouble()*255.0;
                tmpData.append(QString::number(colorChannel));
            }
            newFOV->setData(FOV_Menu::Color, tmpData);
            break;
        case 4:
            tmpData.append(line_items[0]);
            newFOV->setData(FOV_Menu::Type,tmpData);
            break;
        case 5:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            newFOV->setData(FOV_Menu::DrawField, tmpData);
            break;
        case 6:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            newFOV->setData(FOV_Menu::SCBody, tmpData);
            break;
        case 7:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            tmpData.append(line_items[2]);
            newFOV->setData(FOV_Menu::BodyPos, tmpData);
            break;
        case 8:
            tmpData.append(line_items[1]);
            tmpData.append(line_items[2]);
            tmpData.append(line_items[3]);
            tmpData.append(line_items[3]);
            newFOV->setData(FOV_Menu::Euler, tmpData);
            break;
        case 9:
            tmpData.append(line_items[0]);
            newFOV->setData(FOV_Menu::Boresight, tmpData);
            break;
        default:
            break;
        }
        tmpData.clear();
    }

    ui->fovlist->addItem(newFOV);

    ui->fovlist->setCurrentRow(-1);
    clear_fields();
}

void FOV_Menu::on_fovlist_itemClicked(QListWidgetItem *item) {
    QStringList tmpData = {};

    for (int i=0; i<fovNLines; i++) {
        switch (i) {
        case 0:
            tmpData = item->data(FOV_Menu::Label).toStringList();
            ui->fov_name->setText(tmpData[0]);
            break;
        case 1:
            tmpData = item->data(FOV_Menu::Sides).toStringList();
            ui->num_sides->setText(tmpData[0]);
            ui->length_sides->setText(tmpData[1]);
            break;
        case 2:
            tmpData = item->data(FOV_Menu::Dims).toStringList();
            ui->horizontal_width->setText(tmpData[0]);
            ui->vertical_height->setText(tmpData[1]);
            break;
        case 3:
            tmpData = item->data(FOV_Menu::Color).toStringList();
            ui->redvalue->setValue(tmpData[0].toInt());
            ui->greenvalue->setValue(tmpData[1].toInt());
            ui->bluevalue->setValue(tmpData[2].toInt());
            ui->alphavalue->setValue(tmpData[3].toInt());
            break;
        case 4:
            tmpData = item->data(FOV_Menu::Type).toStringList();
            ui->fov_type->setCurrentText(fovtype_inputs[tmpData[0]]);
            break;
        case 5:
            tmpData = item->data(FOV_Menu::DrawField).toStringList();
            ui->nearfield->setChecked(QVariant(tmpData[0]).toBool());
            ui->farfield->setChecked(QVariant(tmpData[1]).toBool());
            break;
        case 6:
            tmpData = item->data(FOV_Menu::SCBody).toStringList();
            ui->sc_name->setCurrentText(scNums.key(tmpData[0].toInt()));
            ui->bdy_num->setValue(tmpData[1].toInt());
            break;
        case 7:
            tmpData = item->data(FOV_Menu::BodyPos).toStringList();
            ui->pos_x->setText(tmpData[0]);
            ui->pos_y->setText(tmpData[1]);
            ui->pos_z->setText(tmpData[2]);
            break;
        case 8:
            tmpData = item->data(FOV_Menu::Euler).toStringList();
            ui->rot1->setText(tmpData[0]);
            ui->rot2->setText(tmpData[1]);
            ui->rot3->setText(tmpData[2]);
            ui->euler_seq->setCurrentText(tmpData[3]);
            break;
        case 9:
            tmpData = item->data(FOV_Menu::Boresight).toStringList();
            ui->boresightaxis->setCurrentText(axis_inputs[tmpData[0]]);
            break;
        default:
            break;
        }
        tmpData.clear();
    }
}

void FOV_Menu::on_loaddefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Overwrite FOV file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_FOV.txt");
        QFile::copy(inout_path+"__default__/Inp_FOV.txt", inout_path+"Inp_FOV.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void FOV_Menu::on_savedefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Overwrite Default FOV file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_FOV.txt");
        QFile::copy(inout_path+"Inp_FOV.txt",inout_path+"__default__/Inp_FOV.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void FOV_Menu::on_closeButton_clicked() {
    FOV_Menu::close();
}

void FOV_Menu::on_applyButton_clicked() {
    QString data_inp;
    QStringList tmpData = {};
    QListWidgetItem* item;

    int fov_num = ui->fovlist->count();

    fov_update.append("************************* Fields of View ***************************\n");

    data_inp = QString::number(fov_num);
    fov_update.append(dsm_gui_lib::whitespace(data_inp) + " ! Number of FOVs\n");

    for (int i=0; i<fov_num; i++) {
        item = ui->fovlist->item(i);
        for (int j=0; j<fovNLines; j++) {
            switch (j) {
            case 0:
                data_inp = "--------------------------------------------------------------------\n";
                break;
            case 1:
                tmpData = item->data(FOV_Menu::Label).toStringList();
                data_inp = "\"" + tmpData[0] + "\"";
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Label\n";
                break;
            case 2:
                tmpData = item->data(FOV_Menu::Sides).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Number of Sides, Length [m]\n";
                break;
            case 3:
                tmpData = item->data(FOV_Menu::Dims).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! H Width, V Height [deg]\n";
                break;
            case 4:
                tmpData = item->data(FOV_Menu::Color).toStringList();
                for (int k =0; k<4; k++) {
                    double colorChannel = tmpData[k].toDouble()/255.0;
                    data_inp += QString::number(colorChannel,'g',3) + "  ";
                }
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Color RGB+Alpha\n";
                break;
            case 5:
                tmpData = item->data(FOV_Menu::Type).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! WIREFRAME, SOLID, VECTOR, or PLANE\n";
                break;
            case 6:
                tmpData = item->data(FOV_Menu::DrawField).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Draw Near Field, Draw Far Field\n";
                break;
            case 7:
                tmpData = item->data(FOV_Menu::SCBody).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! SC, Body\n";
                break;
            case 8:
                tmpData = item->data(FOV_Menu::BodyPos).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Position in Body [m]\n";
                break;
            case 9:
                tmpData = item->data(FOV_Menu::Euler).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Euler Angles [deg], Sequence\n";
                break;
            case 10:
                tmpData = item->data(FOV_Menu::Boresight).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n";
                break;
            }
            fov_update.append(data_inp);
            data_inp.clear();
        }
    }

    write_data();

}

void FOV_Menu::sides_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    tmpData.append(ui->num_sides->text());
    tmpData.append(ui->length_sides->text());
    ui->fovlist->currentItem()->setData(FOV_Menu::Sides,tmpData);
}

void FOV_Menu::dims_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    tmpData.append(ui->horizontal_width->text());
    tmpData.append(ui->vertical_height->text());
    ui->fovlist->currentItem()->setData(FOV_Menu::Dims,tmpData);
}

void FOV_Menu::color_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    tmpData.append(ui->redvalue->text());
    tmpData.append(ui->greenvalue->text());
    tmpData.append(ui->bluevalue->text());
    tmpData.append(ui->alphavalue->text());
    ui->fovlist->currentItem()->setData(FOV_Menu::Color,tmpData);
}

void FOV_Menu::on_fov_name_textChanged(const QString &arg1) {
    if (ui->fovlist->currentRow()==-1) return;
    ui->fovlist->currentItem()->setData(FOV_Menu::Label,arg1);
}

void FOV_Menu::field_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    tmpData.append(dsm_gui_lib::toString(ui->nearfield));
    tmpData.append(dsm_gui_lib::toString(ui->farfield));
    ui->fovlist->currentItem()->setData(FOV_Menu::DrawField,tmpData);
}

void FOV_Menu::scbody_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    int scNum = scNums[ui->sc_name->currentText()];
    tmpData.append(QString::number(scNum));
    tmpData.append(ui->bdy_num->text());
    ui->fovlist->currentItem()->setData(FOV_Menu::SCBody,tmpData);
}

void FOV_Menu::pos_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    tmpData.append(ui->pos_x->text());
    tmpData.append(ui->pos_y->text());
    tmpData.append(ui->pos_z->text());
    ui->fovlist->currentItem()->setData(FOV_Menu::BodyPos,tmpData);
}

void FOV_Menu::euler_changed() {
    if (ui->fovlist->currentRow()==-1) return;
    QStringList tmpData;
    tmpData.append(ui->rot1->text());
    tmpData.append(ui->rot2->text());
    tmpData.append(ui->rot3->text());
    tmpData.append(ui->euler_seq->currentText());
    ui->fovlist->currentItem()->setData(FOV_Menu::Euler,tmpData);
}

void FOV_Menu::on_boresightaxis_currentTextChanged(const QString &arg1) {
    if (ui->fovlist->currentRow()==-1) return;
    ui->fovlist->currentItem()->setData(FOV_Menu::Boresight,axis_inputs.key(arg1));
}

void FOV_Menu::on_fov_type_currentTextChanged(const QString &arg1) {
    if (ui->fovlist->currentRow()==-1) return;
    ui->fovlist->currentItem()->setData(FOV_Menu::Type,fovtype_inputs.key(arg1));
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
    int index = ui->fovlist->currentRow();
    QListWidgetItem* curItem = ui->fovlist->currentItem();
    QStringList curNames = getTextFromList(ui->fovlist);

    if (index == -1) return;
    QString oldName = curItem->text();
    QString newName = oldName +"_Copy";
    for(int i = 0; i <= 30; i++) {
        QString newNameTest = newName;
        if(i>0) newNameTest += "_" + QString::number(i);
        if(!curNames.contains(newNameTest)) {
            newName = newNameTest;
            break;
        }
    }
    QListWidgetItem* newItem = curItem->clone();
    newItem->setText(newName);
    ui->fovlist->addItem(newItem);

}

QStringList FOV_Menu::getTextFromList(QListWidget *list){
    QStringList output;
    foreach(QListWidgetItem *item, list->findItems("*",Qt::MatchWildcard))
        output << item->text();
    output.sort(Qt::CaseInsensitive);
    return output;
}

void FOV_Menu::on_sc_name_currentTextChanged(const QString &arg1) {
    QStringList scFileNames = QDir(inout_path).entryList({"SC_"+arg1+".txt"});
    QString scFileName = scFileNames[0];

    QFile scFile(inout_path + scFileName);
    if(!scFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", scFile.errorString());
    QTextStream in(&scFile);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains("Body Parameters",Qt::CaseInsensitive)) {
            line = in.readLine();
            line = in.readLine();
            QStringList line_items = line.remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
            int nBodies = line_items[0].toInt();
            ui->bdy_num->setMaximum(nBodies-1);
            break;
        }

    }
    scFile.close();

}

