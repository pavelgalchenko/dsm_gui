#include "orb_menu.h"
#include "ui_orb_menu.h"

#include <QFile>
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
            file_paths.append(file.fileName());
        }
    }

    for(int i=0; i<file_paths.length();i++) {
        file_path = file_paths[i];
        receive_data();
        apply_data();
        populate_list();
    }

}

void ORB_Menu::receive_data() {
    orb_data.clear();
    orb_string.clear();
    orb_update.clear();
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
            orb_data.append(capture);
        }
        else {
            orb_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            orb_string.append(capture);
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
//    QStringList line_items;
//    QString line_item;
//    int current_index = 2;

//    orb_names.clear();
//    orb_name_index.clear();
//    orb_name_size.clear();

//    line_items = orb_data[1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//    line_item = line_items[0];

//    for (int i=0; i < num_ipc; i++) {

//        line_items = orb_data[current_index+8].split(QRegExp("\\s"), Qt::SkipEmptyParts);
//        line_item = line_items[0];
//        num_prefixes = line_item.toInt();

//        orb_names.append("IPC "+QString::number(i));
//        orb_update.replace(current_index,"********************************  "+orb_names.at(i)+"  ********************************\n");
//        orb_name_index.append(current_index);
//        orb_name_size.append(9+num_prefixes);
//        current_index = current_index+9+num_prefixes;
//    }
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

