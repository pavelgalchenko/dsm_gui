#include "orb_menu.h"
#include "ui_orb_menu.h"

#include <QFile>
#include <QDebug>
//#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
//#include <QInputDialog>
#include <QDirIterator>

ORB_Menu::ORB_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ORB_Menu)
{
    ui->setupUi(this);
    set_validators();
}

ORB_Menu::~ORB_Menu()
{
    delete ui;
}

void ORB_Menu::set_validators()
{
    ui->orbFormFrame->addItems(orbFrameInputs);
    ui->orbFormOrigin->addItems(orbFrameInputs);
    ui->orbFormFrameEulerSeq->addItems(eulerSeqInputs);
    ui->orbZeroWorld->addItems(worldInputs);
    ui->orbCentWorld->addItems(worldInputs);
    ui->orbCentICParam->addItems(orbCentICTypeInputs);
    ui->orbCentFileType->addItems(orbFileTypeInputs);
    ui->orbTBodyLSystem->addItems(orbTBodyLSystemInputs);
    ui->orbTBodyICParam->addItems(orbTBodyICTypeInputs);
    ui->orbTBodyFileType->addItems(orbFileTypeInputs);

}

void ORB_Menu::receive_orbpath(QString path)
{
    QString target_str = "orb_";

    inout_path = path;

    QDirIterator it(path, QDirIterator::NoIteratorFlags);

    while (it.hasNext()) {
        QString filename = it.next();
        QFileInfo file(filename);

        if (file.isDir()) { // Skip over directories
            continue;
        }

        // If filename includes target string, add to file list
        if (file.fileName().contains(target_str, Qt::CaseInsensitive)) {
            file_paths.append(inout_path+file.fileName());
        }
    }

    for(int i=0; i<file_paths.length();i++) {
        file_path = file_paths[i];
        qDebug()<<file_path;
        receive_data();
        apply_data();
        populate_list();
    }

}

void ORB_Menu::receive_data() {
    orb_data.clear();
    orb_string.clear();
    orb_update.clear();
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
        if (match1.hasMatch()) {
            QString capture = match1.captured(1);
            orb_data.append(capture);
            qDebug()<<capture;
        }
        else {
            orb_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            orb_string.append(capture);
            qDebug()<<capture;
        }
        else {
            orb_string.append("blankline");
        }
        line.append("\n");
        orb_update.append(line);
    }
    file.close();
}

void ORB_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < orb_update.size(); i++){
            in << orb_update.at(i);
        }
    }
    file.close();
}

void ORB_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int line_num = 0;

    line_num = 1;
    line_num = 2;
    line_num = 3;
    line_num = 4;
    line_num = 5;
    line_num = 6;
    line_num = 7;
    line_num = 8;
    line_num = 9;
    line_num = 10;
    line_num = 11;
    line_num = 12;
    line_num = 13;
    line_num = 14;
    line_num = 15;
    line_num = 16;
    line_num = 17;
    line_num = 18;
    line_num = 19;
    line_num = 20;
    line_num = 21;
    line_num = 22;
    line_num = 23;
    line_num = 24;
    line_num = 25;
    line_num = 26;
    line_num = 27;
    line_num = 28;
    line_num = 29;
    line_num = 30;
    line_num = 31;
    line_num = 32;
    line_num = 33;
    line_num = 34;
    line_num = 35;
    line_num = 36;
    line_num = 37;
    line_num = 38;
    line_num = 39;
    line_num = 40;
    line_num = 41;
    line_num = 42;
    line_num = 43;
    line_num = 44;
    line_num = 45;
    line_num = 46;
    line_num = 47;


//    write_data();
}

void ORB_Menu::populate_list(){

}



void ORB_Menu::on_orbListRemove_clicked()
{

}


void ORB_Menu::on_orbListAdd_clicked()
{

}

