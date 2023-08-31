#include "nos_menu.h"
#include "ui_nos_menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>

NOS_Menu::NOS_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NOS_Menu)
{
    ui->setupUi(this);
}

NOS_Menu::~NOS_Menu()
{
    delete ui;
}

void NOS_Menu::receive_nospath(QString path)
{
    inout_path = path;
    file_path = path+"Inp_NOS3.txt";
    receive_data();
    apply_data();
}

void NOS_Menu::receive_data()
{
    nos_data.clear();
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
            nos_data.append(capture);
        }
        else {
            nos_data.append("blankline");
        }
    }
    file.close();
}

void NOS_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < nos_update.size(); i++){
            in << nos_update.at(i);
        }
    }
    file.close();
}

void NOS_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int line_num = 0;

    //line_num = 1; // blank line

    line_num = 2;
    line_items = nos_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->time_bus->setText(line_items[0]);

    line_num = 3;
    line_items = nos_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    ui->time_string->setText(line_items[0]);
}

int NOS_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

void NOS_Menu::on_loaddefaultButton_clicked()
{
    int response = warning_message("Overwrite NOS3 file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_NOS3.txt");
        QFile::copy(inout_path+"__default__/Inp_NOS3.txt", inout_path+"Inp_NOS3.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void NOS_Menu::on_savedefaultButton_clicked()
{
    int response = warning_message("Overwrite Default NOS3 file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_NOS3.txt");
        QFile::copy(inout_path+"Inp_NOS3.txt",inout_path+"__default__/Inp_NOS3.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void NOS_Menu::on_closeButton_clicked()
{
    NOS_Menu::close();
}

void NOS_Menu::on_applyButton_clicked()
{
    QString data_inp;

    nos_update.clear();

    nos_update.append("<<<<<<<<<<<<<<<<<<  42 NOS3 Time Configuration File  >>>>>>>>>>>>>>>>>>>\n");

    data_inp = ui->time_bus->text();
    nos_update.append(whitespace(data_inp) + " !  NOS3 Time Bus\n");

    data_inp = ui->time_string->text();
    nos_update.append(whitespace(data_inp) + " !  NOS3 Time Connection String\n");

    write_data();

}

QString NOS_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}
