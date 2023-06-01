#include "rgn_menu.h"
#include "ui_rgn_menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>

RGN_Menu::RGN_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RGN_Menu)
{
    ui->setupUi(this);
    set_validators();
}

RGN_Menu::~RGN_Menu()
{
    delete ui;
}

void RGN_Menu::set_validators()
{
    ui->world->addItems(world_inputs);
    ui->location->addItems(location_inputs);
    ui->posw_x->setValidator(new QDoubleValidator);
    ui->posw_y->setValidator(new QDoubleValidator);
    ui->posw_z->setValidator(new QDoubleValidator);
    ui->lng->setValidator(new QDoubleValidator);
    ui->lat->setValidator(new QDoubleValidator);
    ui->alt->setValidator(new QDoubleValidator);
    ui->elasticity->setValidator(new QDoubleValidator);
    ui->damping->setValidator(new QDoubleValidator);
    ui->friction->setValidator(new QDoubleValidator);
}

void RGN_Menu::receive_rgnpath(QString path)
{
    inout_path = path;
    file_path = path+"Inp_Region.txt";
    receive_data();
    apply_data();
    populate_list();
}

void RGN_Menu::receive_data()
{
    rgn_data.clear();
    rgn_string.clear();
    rgn_update.clear();
    static QRegularExpression rx1("(.*?)*!");
    static QRegularExpression rx2("\"(.*?)\"");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        if (match1.hasMatch()) {
            QString capture = match1.captured(0);
            rgn_data.append(capture);
        }
        else {
            rgn_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            rgn_string.append(capture);
        }
        else {
            rgn_string.append("blankline");
        }
        line.append("\n");
        rgn_update.append(line);
    }
    file.close();
}

void RGN_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < rgn_update.size(); i++){
            in << rgn_update.at(i);
        }
    }
    file.close();
}


void RGN_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int num_rgn = 0;

    rgn_names.clear();

    line_items = rgn_data[1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = line_items[0];
    num_rgn = line_item.toInt();

    for (int i=0; i < num_rgn; i++) {
        line_item = rgn_string[9*i+4];
        rgn_names.append(line_item);
    }
}

void RGN_Menu::populate_list()
{
    ui->rgnlist->clear();
    ui->rgnlist->addItems(rgn_names);
}

void RGN_Menu::on_rgn_remove_clicked()
{
    int removeitem = ui->rgnlist->currentRow();
    if (removeitem == -1) {
        return;
    }
    else {
        int rgn_index = 9*(removeitem)+2;
        rgn_names.removeAt(removeitem);
        for (int i = 0; i < 9; i++) {
            rgn_data.removeAt(rgn_index);
            rgn_string.removeAt(rgn_index);
            rgn_update.removeAt(rgn_index);
        }

        delete ui->rgnlist->takeItem(removeitem);

        QString data_inp;
        int rgn_num = ui->rgnlist->count();

        data_inp = QString::number(rgn_num);
        rgn_update[1] = whitespace(data_inp) + " ! Number of Regions\n";

        write_data();

        ui->rgnlist->setCurrentRow(-1);
        global_rgn_index = -1;
    }
}

void RGN_Menu::on_rgn_add_clicked()
{
    QString newrgn = "\"NEWREGION\"";

    rgn_names.append(newrgn);

    rgn_data.append("blankline");
    rgn_data.append("TRUE                         !");
    rgn_data.append("\"NEWREGION\"                  !");
    rgn_data.append("EARTH                        !");
    rgn_data.append("LLA                          !");
    rgn_data.append("0.0   0.0  0.0               !");
    rgn_data.append("-80.53  28.46 1000.0         !");
    rgn_data.append("1.0E6  1.0E4  0.1            !");
    rgn_data.append("Rgn_Terrain.obj              !");

    rgn_string.append("blankline");
    rgn_string.append("blankline");
    rgn_string.append(newrgn);
    for (int i = 0; i < 6; i++) rgn_string.append("blankline");

    rgn_update.append("--------------------------------------------------------------------\n");
    rgn_update.append("TRUE                         ! Exists\n");
    rgn_update.append("\"NEWREGION\"                  ! Name\n");
    rgn_update.append("EARTH                        ! World\n");
    rgn_update.append("LLA                          ! POSW or LLA\n");
    rgn_update.append("0.0   0.0  0.0               ! Position in W, m\n");
    rgn_update.append("-80.53  28.46 1000.0         ! Lng, Lat (deg), Alt (m)\n");
    rgn_update.append("1.0E6  1.0E4  0.1            ! Elasticity, Damping, Friction Coef\n");
    rgn_update.append("Rgn_Terrain.obj              ! Geometry File Name\n");

    ui->rgnlist->addItem(newrgn);

    write_data();

    ui->rgnlist->setCurrentRow(ui->rgnlist->count()-1);
    on_rgnlist_itemClicked(ui->rgnlist->currentItem());
}

void RGN_Menu::on_rgnlist_itemClicked(QListWidgetItem *item)
{
    int index = ui->rgnlist->row(item);

    if (index == global_rgn_index) {
        return;
    }
    else {
        if ( (global_rgn_index != -1) && (global_rgn_ignore == 0) ) {
            int response = warning_message("Note that changes to the previous selected Region are lost unless you first select \"Apply\"! This is your only warning.");
            if (response == QMessageBox::Cancel) {
                ui->rgnlist->setCurrentRow(global_rgn_index);
                global_rgn_ignore = 1;
                return;
            }
            else if (response == QMessageBox::Ok) {
                global_rgn_ignore = 1;
            }
        }

        receive_data();
        apply_data();

        global_rgn_index = index;

        int string_index = 9*index+4;
        int data_index = 9*index+3;
        int line_num;

        QStringList line_items;
        QString line_item;

        line_item = rgn_string[string_index];
        ui->regionname->setText(line_item);

        line_num = data_index+0;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        if (line_items[0] == "TRUE") ui->exists_on->setChecked(true);
        else ui->exists_off->setChecked(true);

        line_num = data_index+2;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        for( int i=0; i<world_inputs.count(); ++i )
        {
            if (line_items[0] == world_inputs[i]) ui->world->setCurrentIndex(i);
        }
        line_item = line_items[0];
        if (line_item.left(9) == "MINORBODY") {
            QString minorbodynum = line_item.right(1);
            ui->minorbodynum->setValue(minorbodynum.toInt());
            ui->world->setCurrentText("MINORBODY");
        }

        line_num = data_index+3;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        for( int i=0; i<location_inputs.count(); ++i )
        {
            if (line_items[0] == location_inputs[i]) ui->location->setCurrentIndex(i);
        }

        line_num = data_index+4;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->posw_x->setText(line_items[0]);
        ui->posw_y->setText(line_items[1]);
        ui->posw_z->setText(line_items[2]);

        line_num = data_index+5;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->lng->setText(line_items[0]);
        ui->lat->setText(line_items[1]);
        ui->alt->setText(line_items[2]);

        line_num = data_index+6;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->elasticity->setText(line_items[0]);
        ui->damping->setText(line_items[1]);
        ui->friction->setText(line_items[2]);

        line_num = data_index+7;
        line_items = rgn_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->geometeryname->setText(line_items[0]);
    }
}

int RGN_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

void RGN_Menu::on_loaddefaultButton_clicked()
{
    int response = warning_message("Overwrite Region file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_Region.txt");
        QFile::copy(inout_path+"__default__/Inp_Region.txt", inout_path+"Inp_Region.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void RGN_Menu::on_savedefaultButton_clicked()
{
    int response = warning_message("Overwrite Default Region file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_Region.txt");
        QFile::copy(inout_path+"Inp_Region.txt",inout_path+"__default__/Inp_Region.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void RGN_Menu::on_closeButton_clicked()
{
    RGN_Menu::close();
}

void RGN_Menu::on_applyButton_clicked()
{
    QString data_inp;

    int index = ui->rgnlist->currentRow();

    if (index == -1) {
        return;
    }

    int rgn_num = ui->rgnlist->count();

    int data_index = 9*index+2;

    ui->rgnlist->item(index)->setText(ui->regionname->text());

    rgn_update[0] = "********************  Regions for 42  *******************\n";

    data_inp = QString::number(rgn_num);
    rgn_update[1] = whitespace(data_inp) + " ! Number of Regions\n";

    rgn_update[data_index+0] = "---------------------------------------------------------\n";

    if (ui->exists_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    rgn_update[data_index+1] = whitespace(data_inp) + " ! Exists\n";

    data_inp = ui->regionname->text();
    rgn_update[data_index+2] = whitespace(data_inp) + " ! Name\n";

    data_inp = ui->world->currentText();
    if (data_inp == "MINORBODY") {
        data_inp.append("_"+QString::number(ui->minorbodynum->value()));
    }
    rgn_update[data_index+3] = whitespace(data_inp) + " ! World\n";

    data_inp = ui->location->currentText();
    rgn_update[data_index+4] = whitespace(data_inp) + " ! POSW or LLA\n";

    data_inp = ui->posw_x->text() + "  " + ui->posw_y->text()+ "  " + ui->posw_z->text();
    rgn_update[data_index+5] = whitespace(data_inp) + " ! Position in W [m]\n";

    data_inp = ui->lng->text() + "  " + ui->lat->text()+ "  " + ui->alt->text();
    rgn_update[data_index+6] = whitespace(data_inp) + " ! Lng, Lat [deg], Alt [m]\n";

    data_inp = ui->elasticity->text() + "  " + ui->damping->text()+ "  " + ui->friction->text();
    rgn_update[data_index+7] = whitespace(data_inp) + " ! Elasticity, Damping, Friction Coef\n";

    data_inp = ui->geometeryname->text();
    rgn_update[data_index+8] = whitespace(data_inp) + " ! Geometry File Name\n";

    write_data();
}

QString RGN_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}

void RGN_Menu::on_world_currentIndexChanged(int index)
{
    if (ui->world->currentText() == "MINORBODY") {
        ui->minorbodylabel->setEnabled(true);
        ui->minorbodynum->setEnabled(true);
    }
    else {
        ui->minorbodylabel->setEnabled(false);
        ui->minorbodynum->setEnabled(false);
        ui->minorbodynum->setValue(0);
    }
}

