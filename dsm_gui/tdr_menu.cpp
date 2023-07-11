#include "tdr_menu.h"
#include "ui_tdr_menu.h"

TDR_Menu::TDR_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDR_Menu)
{
    ui->setupUi(this);
    set_validators();
}

TDR_Menu::~TDR_Menu() {
    delete ui;
}

void TDR_Menu::set_validators() {
    QRegularExpression rx("[^\"]*");
    tdrsEnabled.append(ui->tdrs01);
    tdrsEnabled.append(ui->tdrs02);
    tdrsEnabled.append(ui->tdrs03);
    tdrsEnabled.append(ui->tdrs04);
    tdrsEnabled.append(ui->tdrs05);
    tdrsEnabled.append(ui->tdrs06);
    tdrsEnabled.append(ui->tdrs07);
    tdrsEnabled.append(ui->tdrs08);
    tdrsEnabled.append(ui->tdrs09);
    tdrsEnabled.append(ui->tdrs10);

    tdrsNames.append(ui->tdrs01_name);
    tdrsNames.append(ui->tdrs02_name);
    tdrsNames.append(ui->tdrs03_name);
    tdrsNames.append(ui->tdrs04_name);
    tdrsNames.append(ui->tdrs05_name);
    tdrsNames.append(ui->tdrs06_name);
    tdrsNames.append(ui->tdrs07_name);
    tdrsNames.append(ui->tdrs08_name);
    tdrsNames.append(ui->tdrs09_name);
    tdrsNames.append(ui->tdrs10_name);

    for (int i=0; i<tdrsNames.length(); i++)
        tdrsNames[i]->setValidator(new QRegularExpressionValidator(rx));
}

void TDR_Menu::receive_tdrpath(QString path) {
    inout_path = path;
    file_path = path+"Inp_TDRS.txt";
    receive_data();
    apply_data();
}

void TDR_Menu::receive_data() {
    tdr_data.clear();
    tdr_string.clear();
    static QRegularExpression rx1("(.*?)!");
    static QRegularExpression rx2("\"(.*?)\"");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        tdr_data.append(match1.captured(1));

        QRegularExpressionMatch match2 = rx2.match(line);
        tdr_string.append(match2.captured(1));
    }
    file.close();
}

void TDR_Menu::write_data() {
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
    tdr_update.clear();
}

void TDR_Menu::apply_data() {
    QStringList line_items;
    QString line_string;

    for (int i=0; i<tdrsEnabled.length(); i++) {
        line_items = tdr_data[i+1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
        line_string = tdr_string[i+1];
        tdrsEnabled[i]->setChecked(QVariant(line_items[0]).toBool());
        tdrsNames[i]->setText(line_string);
    }
}

void TDR_Menu::on_loaddefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Overwrite TDRS file?");
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

void TDR_Menu::on_savedefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Overwrite Default TDRS file?");
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

void TDR_Menu::on_closeButton_clicked() {
    TDR_Menu::close();
}

void TDR_Menu::on_applyButton_clicked() {
    QString data_inp = {};

    const QString tdrsDesc = " ! TDRS-%1 Exists, Designation\n";


    tdr_update.append("<<<<<<<<<<<<<<<<<<<  42 TDRS Configuration File  >>>>>>>>>>>>>>>>>>>>>>\n");

    for (int i = 0; i<tdrsEnabled.length(); i++) {
        data_inp = dsm_gui_lib::toString(tdrsEnabled[i]) + "  ";
        data_inp += "\"" + tdrsNames[i]->text() + "\"";

        data_inp = dsm_gui_lib::whitespace(data_inp);
        if (i == 1)
            data_inp += " ! TDRS-2 was lost along with Challenger\n";
        else {
            data_inp += tdrsDesc.arg(i+1);
        }
        tdr_update.append(data_inp);
        data_inp.clear();
    }

    write_data();

}
