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
    set_validators();
}

SPC_Menu::~SPC_Menu()
{
    delete ui;
}

void SPC_Menu::set_validators()
{
    ui->spc_fswid->addItems(dsm_gui_lib::sortStringList(fswid_types));
}


void SPC_Menu::on_spc_add_clicked() // Add S/C
{
    QString new_spc = "Simple";
    for(int i = 0; i <= 50; i++) {
        QString new_spc_test = new_spc;
        if (i>0) new_spc_test += "_" + QString::number(i);
        if (!spc_names.contains(new_spc_test)) {
            new_spc = new_spc_test;
            break;
        }
        if (i==50) return; // Nothing happens if too many
    }
    spc_names.append(new_spc);
    file_path = inout_path+"SC_"+new_spc+".txt";
    file_paths.append(file_path);

    spc_names.sort();
    file_paths.sort();

    QFile::copy(inout_path + "__default__/SC_Simple.txt", file_path);

    ui->spc_list->addItem(new_spc);
    spc_name_index = spc_names.indexOf(new_spc);

    ui->spc_list->setCurrentRow(spc_name_index);

    receive_data();
    apply_data();
}


void SPC_Menu::on_spc_remove_clicked() // Remove S/C
{
    int remove_Item = ui->spc_list->currentRow();
    if(remove_Item == -1) return;
    else{
        file_path = file_paths[remove_Item];
        ui->spc_list->takeItem(remove_Item);
        spc_names.removeAt(remove_Item);
        file_paths.removeAt(remove_Item);
        QFile::remove(file_path);
        ui->spc_list->setCurrentRow(-1);
        global_spc_index = -1;
        spc_name_index = -1;
    }

}


void SPC_Menu::on_spc_duplicate_clicked() // Duplicate currently selected S/C
{
    int index = ui->spc_list->currentRow();
    if (index == -1) return;
    QString old_spc = spc_names[index];
    QString new_spc = old_spc +"_Copy";
    for(int i = 0; i <= 30; i++) {
        QString new_spc_test = new_spc;
        if(i>0) new_spc_test += "_" + QString::number(i);
        if(!spc_names.contains(new_spc_test)) {
            new_spc = new_spc_test;
            break;
        }
    }
    spc_names.append(new_spc);
    file_path = inout_path+"SC_"+new_spc+".txt";
    QFile::copy(file_paths[index], file_path);
    file_paths.append(file_path);

    ui->spc_list->addItem(new_spc);
    spc_names.sort();
    file_paths.sort();


}


void SPC_Menu::on_spc_load_clicked() // Load default S/C
{
    int index = ui->spc_list->currentRow();
    if(index == -1) return;
    else {
        file_path = file_paths[index];
        int response = dsm_gui_lib::warning_message("Overwrite SC file?");
        if (response == QMessageBox::Ok) {
            QFile::remove(file_path);
            QFile::copy(inout_path+"__default__/SC_Simple.txt", file_path);

            spc_name_index = index;
            receive_data();
            apply_data();
        }
        else return;
    }

}


void SPC_Menu::on_spc_save_clicked()
{
    int index = ui->spc_list->currentRow();
    if(index == -1) return;
    else {
        file_path = file_paths[index];
        int response = dsm_gui_lib::warning_message("Overwrite Default SC file?");
        if (response == QMessageBox::Ok) {
            QFile::remove(inout_path+"__default__/SC_Simple.txt");
            QFile::copy(file_path,inout_path+"__default__/SC_Simple.txt");
            spc_name_index = index;
            receive_data();
            apply_data();
        }
        else return;
    }

}


void SPC_Menu::on_spc_close_clicked()
{
    SPC_Menu::close();
}


void SPC_Menu::on_spc_apply_clicked()
{
    int index = ui->spc_list->currentRow();
    if(index == -1){
        return;
    }

    QString newLabel = ui->spc_name->text();
    if ( spc_names.indexOf(newLabel) != index && spc_names.contains(newLabel,Qt::CaseInsensitive)) {
        dsm_gui_lib::warning_message("Spacecraft \"" + newLabel + "\" already exists. Spacecraft names are NOT case sensitive.");
        return;
    }

    file_path = file_paths[index];
    ui->spc_list->currentItem()->setText(newLabel);
    spc_names[index] = newLabel;
    file_paths[index] = inout_path + "SC_" + newLabel + ".txt";
    QFile::rename(file_path,file_paths[index]);
    file_path = file_paths[index];

    for(int line_num=1; line_num<=6; line_num++)
    { // append data from "general" information
        QString data_inp = "";
        switch (line_num) {
            /******************************************* HEADER ***************************************************/
        case 1: // File Header
            break;
        case 2: // Spacecraft Description
            data_inp = ui->spc_desc->text();
            break;
        case 3: // Spacecraft Name
            data_inp = "\"" + ui->spc_name->text() + "\"";
            break;
        case 4: // Sprite File Name
            data_inp = ui->spc_sprite->text();
            break;
        case 5: // Flight Software Identifier
            data_inp = ui->spc_fswid->currentText();
            break;
        case 6: // Flight Software Sample Time
            data_inp = ui->spc_fswsamp->text();
            break;
        }

        if (spc_file_headers[line_num-1].isEmpty())
        {
            spc_update.append(dsm_gui_lib::whitespace(data_inp)+spc_file_descrip[line_num-1]);
        }
        else
        {
            spc_update.append(spc_file_headers[line_num-1]);
        }

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
        if (counter > 6)
        {
            spc_update.append(line);
            spc_update.append("\n");
        }
        counter++;
    }
    file.close();

    spc_names.sort();
    file_paths.sort();
    index = file_paths.indexOf(file_path);
    global_spc_index=index;
    spc_name_index=index;
    ui->spc_list->setCurrentRow(index);

    write_data();


}


void SPC_Menu::on_spc_conf_clicked()
{
    spc_submenu = new SPC_submenu(this);
    spc_submenu->setModal(true);
    spc_submenu->show();

    connect(this, SIGNAL(send_data(QString, QString)), spc_submenu, SLOT(receive_spc_sm_path(QString, QString)));
    emit send_data(ui->spc_name->text(), inout_path);
    disconnect(this, SIGNAL(send_data(QString, QString)), 0, 0);
}


void SPC_Menu::on_spc_list_itemClicked(QListWidgetItem *item)
{
     int index = ui->spc_list->row(item);

    if (index == global_spc_index) {
        return;
    }
    else {
        if ( (global_spc_index != -1) && (global_spc_ignore == 0) ) {
            int response = dsm_gui_lib::warning_message("Note that changes to the previous selected spacecraft are lost unless you first select \"Apply\"! This is your only warning.");
            if (response == QMessageBox::Cancel) {
                ui->spc_list->setCurrentRow(global_spc_index);
                global_spc_ignore = 1;
                return;
            }
            else if (response == QMessageBox::Ok) {
                global_spc_ignore = 1;
            }
        }
    }

    file_path = file_paths[index];
    spc_name_index = index;

    receive_data();
    apply_data();

    global_spc_index = index;


}

void SPC_Menu::receive_data()
{
    // Daniel's regex from ORB_Menu
    // Return everything up to and including ! (exclamation point)
    static QRegularExpression rx1("(.*?)!");

    // Return everything between a set of " " (quotation marks)
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

    ui->spc_name->setText(spc_names[spc_name_index]);

    for(int line_num=1; line_num<=6; line_num++)
    { // stop after general information
        line_string = spc_string[line_num-1];
        line_items = spc_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

        switch (line_num) {
            /******************************************* HEADER ***************************************************/
        case 1: // File Header
            break;
        case 2: // Spacecraft Description
            ui->spc_desc->setText(spc_data[line_num-1]);
            break;
        case 3: // Spacecraft Name
            ui->spc_label->setText(line_string);
            break;
        case 4: // Sprite File Name
            ui->spc_sprite->setText(line_items[0]);
        case 5: // Flight Software Identifier
            setQComboBox(ui->spc_fswid, line_items[0]);
        case 6: // Flight Software Sample Time
            ui->spc_fswsamp->setText(line_items[0]);
        }
    }


}

void SPC_Menu::receive_spcpath(QString path)
{
    inout_path = path;

    QStringList spcFiles = QDir(inout_path).entryList({"SC_*"});
    for (int i = 0; i<spcFiles.length(); i++) {
        file_paths.append(inout_path+spcFiles[i]); // Full file path of SC file
        spc_names.append(spcFiles[i].chopped(4).mid(3)); // Everything between "SC_" and ".txt"
    }

    file_paths.sort();
    spc_names.sort();

    ui->spc_list->clear();
    for(int i=0; i<file_paths.length();i++) {
        file_path = file_paths[i];
        spc_name_index = i;

        receive_data();
        apply_data();
    }
    ui->spc_list->addItems(spc_names);

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


void SPC_Menu::on_spc_list_itemActivated(QListWidgetItem *item)
{
    ui->spc_conf->setEnabled(true);
}

void SPC_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

