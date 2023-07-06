#include "spc_menu.h"
#include "ui_spc_menu.h"
#include "dsm_gui_lib.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QComboBox>
#include <QRegularExpression>
#include <QDir>

SPC_Menu::SPC_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SPC_Menu)
{
    ui->setupUi(this);
    ui->quick_tabs->setCurrentIndex(0);
    set_validators();
}

SPC_Menu::~SPC_Menu()
{
    delete ui;
}

void SPC_Menu::set_validators()
{
    QRegularExpression rx("[^\"]*");
    QRegularExpression rx1("[^\" ]*");

    QValidator *noQuotes = new QRegularExpressionValidator(rx,this);
    QValidator *noQuotesSpaces = new QRegularExpressionValidator(rx1,this);

    // Combo Boxes
    ui->spc_fswid->addItems(dsm_gui_lib::sortStringList(fswid_types));

    ui->spc_cur_orb_type->addItems(dsm_gui_lib::sortStringList(orbit_type));
    ui->spc_cur_pos_ref->addItems(dsm_gui_lib::sortStringList(pos_wrt_F));

    ui->spc_cur_angvel_frame1->addItems(dsm_gui_lib::sortStringList(angvel_wrt));
    ui->spc_cur_angvel_frame2->addItems(dsm_gui_lib::sortStringList(att_wrt));
    ui->spc_cur_att_param->addItems(dsm_gui_lib::sortStringList(att_params));

    ui->spc_cur_initeul_seq->addItems(dsm_gui_lib::sortStringList(euler_seq));

    // Data Type Validators
    ui->spc_name->setValidator(noQuotesSpaces);
    ui->spc_label->setValidator(noQuotes);
    ui->spc_desc->setValidator(noQuotes);
    ui->spc_sprite->setValidator(noQuotes);

    ui->spc_fswsamp->setValidator(new QDoubleValidator(0, INFINITY, 5));


    ui->spc_cur_xpos_form->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_ypos_form->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_zpos_form->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));

    ui->spc_cur_xvel_form->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_yvel_form->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_zvel_form->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));


    ui->spc_cur_angvel_1->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_angvel_2->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_angvel_3->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));

    ui->spc_cur_q1->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_q2->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_q3->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_q4->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));

    ui->spc_cur_initeul_1->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_initeul_2->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));
    ui->spc_cur_initeul_3->setValidator(new QDoubleValidator(-INFINITY, INFINITY, 5));

}

void SPC_Menu::receive_spcpath(QString path)
{
    inout_path = path;

    QStringList spcFiles = QDir(inout_path).entryList({"SC_*"});
    if (spcFiles.length() > 0){
        for (int i = 0; i<spcFiles.length(); i++) {
            file_paths.append(inout_path+spcFiles[i]); // Full file path of SC file
            spc_names.append(spcFiles[i].chopped(4).mid(3)); // Everything between "SC_" and ".txt"
        }

        QStringList spcDefaultFiles = QDir(inout_path+"__default__/").entryList({"SC_*"});
        for (int i = 0; i<spcDefaultFiles.length(); i++) {
            file_paths_default.append(inout_path+"__default__/"+spcDefaultFiles[i]); // Full file path of SC file
            spc_names_default.append(spcDefaultFiles[i].chopped(4).mid(3)); // Everything between "SC_" and ".txt"
        }

        ui->spc_list->clear();
        for(int i=0; i<file_paths.length();i++) {
            file_path = file_paths[i];
            spc_name_index = i;

            ui->spc_list->addItem(spc_names[i]);

            ui->spc_list->setCurrentRow(spc_name_index);

            receive_data();
            apply_data();
        }

        on_spc_list_itemClicked(ui->spc_list->item(0));
    }

}

void SPC_Menu::receive_data()
{
    // Daniel's regex from ORB_Menu
    // Return everything up to and including ! (exclamation point)
    static QRegularExpression rx1("(.*?)!");

    // Return everything between a set of "  " (quotation marks)
    static QRegularExpression rx2("\"(.*?)\"");

    // If the line does NOT start with an alphanumeric character or " (single quotation), then return the line as first group.
    // Otherwise return everything after ! (exclamation point) as second group
    static QRegularExpression rx3("(?:(?=^[^[:alnum:]|\"])([^[:alnum:]|\"].*)|(!.*))");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        spc_data.append(match1.captured(1)); // index 0 includes ! character, index 1 does not

        QRegularExpressionMatch match2 = rx2.match(line);
        spc_string.append(match2.captured(1)); // index 0 includes "" characters, index 1 does not

        //        line.append("\n");
        QRegularExpressionMatch match3 = rx3.match(line);
        if (match3.hasMatch()) {
            QString capture = match3.captured(1);
            if (!capture.isEmpty()) capture += "\n";
            spc_file_headers.append(capture);
            capture = match3.captured(2);
            if (!capture.isEmpty()) capture += "\n";
            spc_file_descrip.append(capture);
        }

    }
    file.close();

}

void SPC_Menu::apply_data()
{
    QStringList line_items;
    QString line_string;

    QStringList tmp_data = {};

    if (ui->spc_list->count() > 0)
    {
        for(int line_num=1; line_num<17; line_num++)
        { // stop after general information
            line_string = spc_string[line_num-1];
            line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

            switch (line_num) {
                /******************************************* HEADER ***************************************************/
            case 1: // File Header
                break;
            case 2: // Spacecraft Description
                tmp_data.append(spc_data[line_num-1].simplified());
                //ui->spc_desc->setText(spc_data[line_num-1]);
                break;
            case 3: // Spacecraft Label
                tmp_data.append(line_string.simplified());
                //ui->spc_label->setText(line_string);
                break;
            case 4: // Sprite File Name
                tmp_data.append(line_items[0]);
                //ui->spc_sprite->setText(line_items[0]);
                break;
            case 5: // Flight Software Identifier
                tmp_data.append(line_items[0]);
                //setQComboBox(ui->spc_fswid, line_items[0]);
                break;
            case 6: // Flight Software Sample Time
                tmp_data.append(line_items[0]);
                //ui->spc_fswsamp->setText(line_items[0]);
                break;
            case 7:
                /******************************************* HEADER ***************************************************/
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
                line_items = spc_data[line_num-1].split(QRegExp(""), Qt::SkipEmptyParts);
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

void SPC_Menu::on_spc_apply_clicked()
{
    int index = ui->spc_list->currentRow();
    if(index == -1){
        return;
    }

    spc_names.sort();
    file_paths.sort();

    qDebug() << spc_names;
    QStringList other_names = spc_names;
    other_names.removeAt(spc_names.indexOf(ui->spc_list->currentItem()->text()));
    qDebug() <<spc_names;

    QString cur_name = ui->spc_name->text();
    if (other_names.contains(cur_name, Qt::CaseInsensitive)) {
        dsm_gui_lib::warning_message("SC \"" + cur_name + "\" already exists. SC names are NOT case sensitive.");
        return;
    }

    file_path = file_paths[index];
    ui->spc_list->currentItem()->setText(cur_name);
    spc_names[index] = cur_name;
    file_paths[index] = inout_path + "SC_" + cur_name + ".txt";
    QFile::rename(file_path,file_paths[index]);
    file_path = file_paths[index];

    QStringList tmp_data = {};
    QString data_inp = "";

    for(int line_num=1; line_num<17; line_num++)
    { // append data from "general" information
        switch (line_num) {
            /******************************************* HEADER ***************************************************/
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
            /******************************************* HEADER ***************************************************/
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

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
            {
                ui->spc_cur_initeul_1->setEnabled(false);
                ui->spc_cur_initeul_2->setEnabled(false);
                ui->spc_cur_initeul_3->setEnabled(false);

                ui->spc_cur_initeul_seq->setEnabled(false);

                ui->spc_cur_q1->setEnabled(true);
                ui->spc_cur_q2->setEnabled(true);
                ui->spc_cur_q3->setEnabled(true);
                ui->spc_cur_q4->setEnabled(true);
            }
            else
            {
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

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
            {
                ui->spc_cur_initeul_1->setEnabled(false);
                ui->spc_cur_initeul_2->setEnabled(false);
                ui->spc_cur_initeul_3->setEnabled(false);

                ui->spc_cur_initeul_seq->setEnabled(false);

                ui->spc_cur_q1->setEnabled(true);
                ui->spc_cur_q2->setEnabled(true);
                ui->spc_cur_q3->setEnabled(true);
                ui->spc_cur_q4->setEnabled(true);
            }
            else
            {
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

    QStringList current_data = ui->spc_list->currentItem()->data(257).toStringList();

    for(int line_num=1; line_num<17; line_num++)
    { // append data from "general" information

        switch (line_num) {
        case 1:
            break; // header
        case 2:
            data_inp = current_data[0];
            break;
        case 3:
            data_inp = "\"" + current_data[1] + "\"";
            break;
        case 4:
            data_inp = current_data[2];
            break;
        case 5:
            data_inp = current_data[3];
            break;
        case 6:
            data_inp = current_data[4];
            break;
        case 7:
            /******************************************* HEADER ***************************************************/
            // Orbit Parameters
            break;
        case 8: // Orbit Prop
            data_inp = ui->spc_cur_orb_type->currentText();
            break;
        case 9: // Pos of CM or ORIGIN wrt F
            data_inp = ui->spc_cur_pos_ref->currentText();
            break;
        case 10: // Pos wrt Formation (m) expressed in F
            data_inp = ui->spc_cur_xpos_form->text() + "  " + ui->spc_cur_ypos_form->text() + "  " + ui->spc_cur_zpos_form->text();
            break;
        case 11: // Vel wrt Formation (m) expressed in F
            data_inp = ui->spc_cur_xvel_form->text() + "  " + ui->spc_cur_yvel_form->text() + "  " + ui->spc_cur_zvel_form->text();
            break;
        case 12: // Initial Attitude Header
            break;
        case 13: // Ang Vel wrt [NL], Att [QA] wrt [NLF]
            data_inp = ui->spc_cur_angvel_frame1->currentText() + ui->spc_cur_att_param->currentText() + ui->spc_cur_angvel_frame2->currentText();
            break;
        case 14:
            data_inp = ui->spc_cur_angvel_1->text() + "  " + ui->spc_cur_angvel_2->text() + "  " + ui->spc_cur_angvel_3->text();
            break;
        case 15:
            data_inp = ui->spc_cur_q1->text() + "  " +  ui->spc_cur_q2->text() + "  " + ui->spc_cur_q3->text() + "  " + ui->spc_cur_q4->text();

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
            {
                ui->spc_cur_initeul_1->setEnabled(false);
                ui->spc_cur_initeul_2->setEnabled(false);
                ui->spc_cur_initeul_3->setEnabled(false);

                ui->spc_cur_initeul_seq->setEnabled(false);

                ui->spc_cur_q1->setEnabled(true);
                ui->spc_cur_q2->setEnabled(true);
                ui->spc_cur_q3->setEnabled(true);
                ui->spc_cur_q4->setEnabled(true);
            }
            else
            {
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
            data_inp = ui->spc_cur_initeul_1->text() + "  " + ui->spc_cur_initeul_2->text() + "  " + ui->spc_cur_initeul_3->text() + "  " + ui->spc_cur_initeul_seq->currentText();

            if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
            {
                ui->spc_cur_initeul_1->setEnabled(false);
                ui->spc_cur_initeul_2->setEnabled(false);
                ui->spc_cur_initeul_3->setEnabled(false);

                ui->spc_cur_initeul_seq->setEnabled(false);

                ui->spc_cur_q1->setEnabled(true);
                ui->spc_cur_q2->setEnabled(true);
                ui->spc_cur_q3->setEnabled(true);
                ui->spc_cur_q4->setEnabled(true);
            }
            else
            {
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

        if(spc_file_headers[line_num-1].isEmpty())
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+spc_file_descrip[line_num-1]);
        else
            spc_update.append(spc_file_headers[line_num-1]);
    }

    // Read in the rest of the file and append it to the end. Functionally equivalent to only changing the first 6 lines.
    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    long counter = 1;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (counter > 16)
        {
            spc_update.append(line);
            spc_update.append("\n");
        }
        counter++;
    }
    file.close();

    index = file_paths.indexOf(file_path);
    global_spc_index=index;
    spc_name_index=index;
    ui->spc_list->setCurrentRow(index);

    write_data();


}


void SPC_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < spc_update.size(); i++){
            in << spc_update.at(i);
        }
    }
    spc_update.clear();
    file.close();
}

void SPC_Menu::on_spc_add_clicked() // Add S/C
{

    QStringList tmp_data = {};

    tmp_data.append("Simple generic S/C");
    tmp_data.append("S/C");
    tmp_data.append("GenScSpriteAlpha.ppm");
    tmp_data.append("PROTOTYPE_FSW");
    tmp_data.append("0.2");

    tmp_data.append("FIXED");
    tmp_data.append("CM");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");

    tmp_data.append("N");
    tmp_data.append("A");
    tmp_data.append("N");
    tmp_data.append("0.3");
    tmp_data.append("0.4");
    tmp_data.append("0.5");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("0.0");
    tmp_data.append("1.0");
    tmp_data.append("60.0");
    tmp_data.append("40.0");
    tmp_data.append("20.0");
    tmp_data.append("213");

    QStringList all_names;
    for (int i = 0; i < ui->spc_list->count(); i++)
    {
        all_names.append(ui->spc_list->item(i)->text());
    }

    QString new_name = "New";
    if (ui->spc_list->count() != 0) {
        for(int i = 0; i <= 50; i++) {
            QString newNameTest = new_name;
            if (i>0) newNameTest += "_" + QString::number(i);
            if (!all_names.contains(newNameTest, Qt::CaseInsensitive)) {
                new_name = newNameTest;
                break;
            }
            if (i==50) return; // Nothing happens if too many
        }
    }

    ui->spc_list->addItem(new_name);
    ui->spc_list->setCurrentRow(ui->spc_list->count()-1);

    ui->spc_list->currentItem()->setData(256, new_name);

    ui->spc_list->currentItem()->setData(257, tmp_data);
    on_spc_list_itemClicked(ui->spc_list->currentItem());

    spc_names.append(new_name);
    file_paths.append(inout_path+"SC_"+new_name+".txt");

    QFile::copy(file_paths_default[0], inout_path+"SC_"+new_name+".txt");
    ui->spc_list->sortItems();
    ui->spc_conf->setEnabled(true);
}


void SPC_Menu::on_spc_remove_clicked() // Remove S/C
{
    int remove_Item = ui->spc_list->currentRow();
    int name_index = spc_names.indexOf(ui->spc_list->item(remove_Item)->text());
    if(remove_Item == -1) return;
    else{
        if (ui->spc_list->count() > 0)
        {
            delete ui->spc_list->item(remove_Item);

            QString file_path_delete = file_paths[name_index];

            spc_names.removeAt(name_index);
            file_paths.removeAt(name_index);

            QFile::remove(file_path_delete);
        }
    }

    if (ui->spc_list->count() > 0) {
        ui->spc_list->setCurrentRow(ui->spc_list->count() - 1);
        on_spc_list_itemClicked(ui->spc_list->item(ui->spc_list->count() - 1));
        ui->spc_list->sortItems();
    }
    else {
        ui->spc_list->setCurrentRow(-1);
        ui->spc_conf->setEnabled(false);
    }


}


void SPC_Menu::on_spc_duplicate_clicked() // Duplicate currently selected S/C
{
    int index = ui->spc_list->currentRow();

    QStringList current_item_data = ui->spc_list->currentItem()->data(257).toStringList();

    if (index == -1) return;
    QString old_spc = ui->spc_list->currentItem()->text();
    QString new_spc = old_spc +"_Copy";
    for(int i = 0; i <= 30; i++) {
        QString newSCTest = new_spc;
        if(i>0) newSCTest += "_" + QString::number(i);
        if(!spc_names.contains(newSCTest, Qt::CaseInsensitive)) {
            new_spc = newSCTest;
            break;
        }
    }

    spc_names.append(new_spc);
    file_path = inout_path+"SC_"+new_spc+".txt";
    QFile::copy(file_paths[index], file_path);
    file_paths.append(file_path);

    ui->spc_list->addItem(new_spc);
    ui->spc_list->setCurrentRow(ui->spc_list->count()-1); // add item, set it as current item

    ui->spc_list->currentItem()->setData(256, new_spc);
    ui->spc_list->currentItem()->setData(257,current_item_data); // set item data

    on_spc_list_itemClicked(ui->spc_list->item(ui->spc_list->count()-1)); // click the item

    spc_names.append(new_spc);
    file_paths.append(inout_path+"SC_"+new_spc+".txt");

    QFile::copy(inout_path+"SC_" + old_spc + ".txt", inout_path+"SC_"+new_spc+".txt"); //create the file associated with the S/C and append the file information to the right variables

    if (ui->spc_list->count() > 0) {
        on_spc_list_itemClicked(ui->spc_list->item(ui->spc_list->count() - 2));
        ui->spc_list->setCurrentRow(ui->spc_list->count() - 2);
    }
    ui->spc_list->sortItems();
}


void SPC_Menu::on_spc_load_clicked() // Load default S/C
{
    int response = dsm_gui_lib::warning_message("Load All Default S/C?");
    if (response == QMessageBox::Ok) {
        int num_items = ui->spc_list->count();
        ui->spc_list->clear();
        for (int i = 0; i<num_items; i++)
        {
            file_path = file_paths[i];

            QFile::remove(file_path);
        }

        file_paths.clear();
        spc_names.clear();

        for (int i = 0; i<spc_names_default.length(); i++)
        {
            file_path = inout_path+"SC_"+spc_names_default[i]+".txt";
            file_paths.append(file_path);
            spc_names.append(spc_names_default[i]);

            spc_name_index = i;

            QFile::copy(file_paths_default[i], file_path);
            ui->spc_list->addItem(spc_names_default[i]);

            ui->spc_list->setCurrentRow(ui->spc_list->count()-1);

            receive_data();
            apply_data();

        }
        on_spc_list_itemClicked(ui->spc_list->item(0));
    }
    else return;
}


void SPC_Menu::on_spc_save_clicked()
{
    int response = dsm_gui_lib::warning_message("Save All Current S/C to Defaults?");
    if (response == QMessageBox::Ok) {
        spc_names_default.clear();
        file_paths_default.clear();

        for (int i = 0; i<ui->spc_list->count(); i++)
        {
            spc_names_default.append(spc_names[i]);
            file_paths_default.append(inout_path + "__default__/" + "SC_"+spc_names[i]);

            QFile::copy(file_paths[i], file_paths_default[i]);
        }
    }

}


void SPC_Menu::on_spc_close_clicked()
{
    SPC_Menu::close();
}


void SPC_Menu::on_spc_conf_clicked()
{
    receive_data();
    apply_data();

    spc_submenu = new SPC_submenu(this);
    spc_submenu->setModal(true);
    spc_submenu->show();

    connect(this, SIGNAL(send_data(QString, QString)), spc_submenu, SLOT(receive_spc_sm_path(QString, QString)));
    emit send_data(ui->spc_name->text(), inout_path);
    disconnect(this, SIGNAL(send_data(QString, QString)), 0, 0);

}


void SPC_Menu::on_spc_list_itemClicked(QListWidgetItem *item)
{

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


void SPC_Menu::on_spc_list_itemActivated(QListWidgetItem *item)
{
    ui->spc_conf->setEnabled(true);
}

void SPC_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}


void SPC_Menu::on_spc_cur_att_param_currentTextChanged(const QString &arg1)
{
    if (!QString::compare(ui->spc_cur_att_param->currentText(), "Q"))
    {
        ui->spc_cur_initeul_1->setEnabled(false);
        ui->spc_cur_initeul_2->setEnabled(false);
        ui->spc_cur_initeul_3->setEnabled(false);

        ui->spc_cur_initeul_seq->setEnabled(false);

        ui->spc_cur_q1->setEnabled(true);
        ui->spc_cur_q2->setEnabled(true);
        ui->spc_cur_q3->setEnabled(true);
        ui->spc_cur_q4->setEnabled(true);
    }
    else
    {
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
