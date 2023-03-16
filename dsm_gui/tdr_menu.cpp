#include "tdr_menu.h"
#include "ui_tdr_menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>

TDR_Menu::TDR_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDR_Menu)
{
    ui->setupUi(this);
}

TDR_Menu::~TDR_Menu()
{
    delete ui;
}

void TDR_Menu::receive_tdrpath(QString path)
{
    inout_path = path;
    file_path = path+"Inp_TDRS.txt";
    receive_data();
    apply_data();
}

void TDR_Menu::receive_data()
{
    tdr_data.clear();
    tdr_string.clear();
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
            tdr_data.append(capture);
        }
        else {
            tdr_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            tdr_string.append(capture);
        }
        else {
            tdr_string.append("blankline");
        }
    }
    file.close();
}

void TDR_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < tdr_update.size(); i++){
            in << tdr_update.at(i);
        }
    }
    file.close();
}

void TDR_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int line_num = 0;

    //line_num = 1; // blank line

    line_num = 2;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs01_on->setChecked(true);
    else ui->tdrs01_off->setChecked(true);
    ui->tdrs01_name->setText(line_item);

    line_num = 3;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs02_on->setChecked(true);
    else ui->tdrs02_off->setChecked(true);
    ui->tdrs02_name->setText(line_item);

    line_num = 4;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs03_on->setChecked(true);
    else ui->tdrs03_off->setChecked(true);
    ui->tdrs03_name->setText(line_item);

    line_num = 5;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs04_on->setChecked(true);
    else ui->tdrs04_off->setChecked(true);
    ui->tdrs04_name->setText(line_item);

    line_num = 6;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs05_on->setChecked(true);
    else ui->tdrs05_off->setChecked(true);
    ui->tdrs05_name->setText(line_item);

    line_num = 7;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs06_on->setChecked(true);
    else ui->tdrs06_off->setChecked(true);
    ui->tdrs06_name->setText(line_item);

    line_num = 8;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs07_on->setChecked(true);
    else ui->tdrs07_off->setChecked(true);
    ui->tdrs07_name->setText(line_item);

    line_num = 9;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs08_on->setChecked(true);
    else ui->tdrs08_off->setChecked(true);
    ui->tdrs08_name->setText(line_item);

    line_num = 10;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs09_on->setChecked(true);
    else ui->tdrs09_off->setChecked(true);
    ui->tdrs09_name->setText(line_item);

    line_num = 11;
    line_items = tdr_data[line_num-1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = tdr_string[line_num-1];
    if (line_items[0] == "TRUE") ui->tdrs10_on->setChecked(true);
    else ui->tdrs10_off->setChecked(true);
    ui->tdrs10_name->setText(line_item);
}

int TDR_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

void TDR_Menu::on_loaddefaultButton_clicked()
{
    int response = warning_message("Overwrite TDRS file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_TDRS.txt");
        QFile::copy(inout_path+"__default__/Inp_TDRS.txt", inout_path+"Inp_TDRS.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void TDR_Menu::on_savedefaultButton_clicked()
{
    int response = warning_message("Overwrite Default TDRS file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_TDRS.txt");
        QFile::copy(inout_path+"Inp_TDRS.txt",inout_path+"__default__/Inp_TDRS.txt");
        receive_data();
        apply_data();
    }
    else {
        return;
    }
}

void TDR_Menu::on_closeButton_clicked()
{
    TDR_Menu::close();
}

void TDR_Menu::on_applyButton_clicked()
{
    QString data_inp;

    tdr_update.clear();

    tdr_update.append("<<<<<<<<<<<<<<<<<<<  42 TDRS Configuration File  >>>>>>>>>>>>>>>>>>>>>>\n");

    if (ui->tdrs01_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs01_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-1 Exists, Designation\n");

    if (ui->tdrs02_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs02_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-2 was lost along with Challenger\n");

    if (ui->tdrs03_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs03_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-3 Exists, Designation\n");

    if (ui->tdrs04_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs04_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-4 Exists, Designation\n");

    if (ui->tdrs05_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs05_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-5 Exists, Designation\n");

    if (ui->tdrs06_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs06_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-6 Exists, Designation\n");

    if (ui->tdrs07_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs07_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-7 Exists, Designation\n");

    if (ui->tdrs08_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs08_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-8 Exists, Designation\n");

    if (ui->tdrs09_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs09_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-9 Exists, Designation\n");

    if (ui->tdrs10_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    data_inp.append("  "+ui->tdrs10_name->text());
    tdr_update.append(whitespace(data_inp) + " ! TDRS-10 Exists, Designation\n");

    write_data();

}

QString TDR_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}
