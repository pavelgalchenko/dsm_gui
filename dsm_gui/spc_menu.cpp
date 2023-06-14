#include "SPC_Menu.h"
#include "ui_SPC_Menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDir>

SPC_Menu::SPC_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SPC_Menu)
{
    ui->setupUi(this);
}

SPC_Menu::~SPC_Menu()
{
    delete ui;
}



void SPC_Menu::on_spc_add_clicked()
{

}


void SPC_Menu::on_spc_remove_clicked()
{

}


void SPC_Menu::on_spc_duplicate_clicked()
{

}


void SPC_Menu::on_spc_load_clicked()
{

}


void SPC_Menu::on_spc_save_clicked()
{

}


void SPC_Menu::on_spc_close_clicked()
{

}


void SPC_Menu::on_spc_apply_clicked()
{

}


void SPC_Menu::on_spc_conf_clicked()
{

}


QString SPC_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}


void SPC_Menu::on_spc_list_itemClicked(QListWidgetItem *item)
{
     int index = ui->spc_list->row(item);

    if (index == global_spc_index) {
        return;
    }
    else {
        if ( (global_spc_index != -1) && (global_spc_ignore == 0) ) {
            int response = warning_message("Note that changes to the previous selected spacecraft are lost unless you first select \"Apply\"! This is your only warning.");
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

}

void SPC_Menu::receive_spcpath(QString path)
{
    inout_path = path;

    QStringList spcFiles = QDir(inout_path).entryList({"SC_*"});
    for (int i = 0; i<spcFiles.length(); i++) {
        file_paths.append(inout_path+spcFiles[i]); // Full file path of SC file
        spc_names.append(spcFiles[i].chopped(4).mid(4)); // Everything between "SC_" and ".txt"
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

int SPC_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

