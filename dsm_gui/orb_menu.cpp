#include "orb_menu.h"
#include "ui_orb_menu.h"

#include <QFile>
#include <QDebug>
#include <QHash>
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
    ui->orbType->addItems(hashValue2QStringList(orbTypeInputs));
    ui->orbFormFrame->addItems(orbFrameInputs);
    ui->orbFormOrigin->addItems(orbFrameInputs);
    ui->orbFormFrameEulerSeq->addItems(eulerSeqInputs);
    ui->orbFormFrameEuler_1->setValidator(new QDoubleValidator);
    ui->orbFormFrameEuler_2->setValidator(new QDoubleValidator);
    ui->orbFormFrameEuler_3->setValidator(new QDoubleValidator);
    ui->orbFormOriginPos_1->setValidator(new QDoubleValidator);
    ui->orbFormOriginPos_2->setValidator(new QDoubleValidator);
    ui->orbFormOriginPos_3->setValidator(new QDoubleValidator);

    ui->orbZeroWorld->addItems(worldInputs);

    ui->orbCentWorld->addItems(worldInputs);
    ui->orbCentICParam->addItems(hashValue2QStringList(orbCentICTypeInputs));
    ui->orbCentKepPeriAlt->setValidator(new QDoubleValidator);
    ui->orbCentKepApoAlt->setValidator(new QDoubleValidator);
    ui->orbCentKepMinAlt->setValidator(new QDoubleValidator);
    ui->orbCentKepEcc->setValidator(new QDoubleValidator);
    ui->orbCentKepInc->setValidator(new QDoubleValidator);
    ui->orbCentKepRAAN->setValidator(new QDoubleValidator);
    ui->orbCentKepTA->setValidator(new QDoubleValidator);
    ui->orbCentPVPos_1->setValidator(new QDoubleValidator);
    ui->orbCentPVPos_2->setValidator(new QDoubleValidator);
    ui->orbCentPVPos_3->setValidator(new QDoubleValidator);
    ui->orbCentPVVel_1->setValidator(new QDoubleValidator);
    ui->orbCentPVVel_2->setValidator(new QDoubleValidator);
    ui->orbCentPVVel_3->setValidator(new QDoubleValidator);
    ui->orbCentFileType->addItems(hashValue2QStringList(orbFileTypeInputs));

    ui->orbTBodyLSystem->addItems(hashValue2QStringList(orbTBodyLSysInputs));
    ui->orbTBodyProp->addItems(hashValue2QStringList(orbTBodyPropInputs));
    ui->orbTBodyICParam->addItems(hashValue2QStringList(orbTBodyICTypeInputs));
    ui->orbTBodyLPoint->addItems(lagrangePointInputs);
    ui->orbTBodyModeXYSMA->setValidator(new QDoubleValidator);
    ui->orbTBodyModeXYPhase->setValidator(new QDoubleValidator);
    ui->orbTBodyModeXYSMA_2->setValidator(new QDoubleValidator);
    ui->orbTBodyModeXYPhase_2->setValidator(new QDoubleValidator);
    ui->orbTBodyModeZSMA->setValidator(new QDoubleValidator);
    ui->orbTBodyModeZPhase->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellPos_1->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellPos_2->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellPos_3->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellVel_1->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellVel_2->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellVel_3->setValidator(new QDoubleValidator);
    ui->orbTBodyFileType->addItems(hashValue2QStringList(orbFileTypeInputs));
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

        receive_data();
        apply_data();
        populate_list();
    }

}

void ORB_Menu::receive_data() {
    orb_data.clear();
    orb_string.clear();
    orb_update.clear();

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
        orb_data.append(match1.captured(1)); // index 0 includes ! character, index 1 does not

        QRegularExpressionMatch match2 = rx2.match(line);
        orb_string.append(match2.captured(1)); // index 0 includes "" characters, index 1 does not

        QRegularExpressionMatch match3 = rx3.match(line);
        if (match3.hasMatch()) {
            QString capture = match3.captured(1);
            if (!capture.isEmpty()) capture += "/n";
            orb_file_headers.append(capture);
            capture = match3.captured(2);
            if (!capture.isEmpty()) capture += "/n";
            orb_file_descrip.append(capture);
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
    QString line_string;

    // Iterate over the number of lines in the file. Last line can end in newline "\n" character.
    for(int line_num=1; line_num<=orb_data.length(); line_num++){
        line_string = orb_string[line_num-1];
        line_items = orb_data[line_num-1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
        switch (line_num) {
/******************************************* HEADER ***************************************************/
        case 1: // File Header
            break;
        case 2: // Orbit Description
            ui->orbDescription->setText(line_string);
            break;
        case 3: // Orbit Type
            setQComboBox(ui->orbType,orbTypeInputs.value(line_items[0]));
            break;
/***************************************** ZERO ORBIT *************************************************/
        case 4: // Zero Orbit Header
            break;
        case 5: // Zero Orbit Body
            if(line_items[0].contains("MINORBODY")) {
                ui->orbZeroWorld->setCurrentIndex(worldInputs.length()-1);
                ui->orbZeroMinorBodyNum->setValue(line_items[0].rightRef(1).toInt());
            }
            else ui->orbZeroWorld->setCurrentIndex(worldInputs.indexOf(line_items[0]));
            break;
        case 6: // Zero Orbit Polyhedral Gravity
            string2radiobool(line_items[0],ui->orbZeroPolyGrav_on,ui->orbZeroPolyGrav_off);
            break;
/**************************************** FLIGHT ORBIT ************************************************/
        case 7: // Flight Orbit Header
            break;
        case 8: // Flight Orbit Region
            ui->orbFlightRegion->setValue(line_items[0].toInt());
            break;
        case 9: // Flight Orbit Polyhedral Gravity
            string2radiobool(line_items[0],ui->orbFlightPolyGrav_on,ui->orbFlightPolyGrav_off);
            break;
/**************************************** CENTRAL ORBIT ***********************************************/
        case 10: // Central Orbit Header
            break;
        case 11: // Central Orbit World
            if(line_items[0].contains("MINORBODY")) {
                ui->orbCentWorld->setCurrentIndex(worldInputs.length());
                ui->orbCentMinorBodyNum->setValue(line_items[0].rightRef(1).toInt());
            }
            else ui->orbCentWorld->setCurrentIndex(worldInputs.indexOf(line_items[0]));
            break;
        case 12: // Central Orbit J2 Secular Drift
            string2radiobool(line_items[0],ui->orbCentJ2_on,ui->orbCentJ2_off);
            break;
        case 13: // Central Orbit Initial Condition Elements
            setQComboBox(ui->orbCentICParam,orbCentICTypeInputs.value(line_items[0]));
            break;
        case 14: // Central Orbit Peri/Apoapsis Alt or Min Alt & Ecc
            if(!line_items[0].compare("PA")) string2radiobool("TRUE",ui->orbCentPA_on,ui->orbCentPA_off);
            else string2radiobool("FALSE",ui->orbCentPA_on,ui->orbCentPA_off);
            break;
        case 15: // Central Orbit Peri/Apoapsis Alt
            ui->orbCentKepPeriAlt->setText(line_items[0]);
            ui->orbCentKepApoAlt->setText(line_items[1]);
            break;
        case 16: // Central Orbit Min Alt & Ecc
            ui->orbCentKepMinAlt->setText(line_items[0]);
            ui->orbCentKepEcc->setText(line_items[1]);
            break;
        case 17: // Central Orbit Inclination
            ui->orbCentKepInc->setText(line_items[0]);
            break;
        case 18: // Central Orbit RAAN
            ui->orbCentKepRAAN->setText(line_items[0]);
            break;
        case 19: // Central Orbit Argument of Periapsis
            ui->orbCentKepArgPeri->setText(line_items[0]);
            break;
        case 20: // Central Orbit True Anomaly
            ui->orbCentKepTA->setText(line_items[0]);
            break;
        case 21: // Central Orbit Initial Position
            ui->orbCentPVPos_1->setText(line_items[0]);
            ui->orbCentPVPos_2->setText(line_items[1]);
            ui->orbCentPVPos_3->setText(line_items[2]);
            break;
        case 22: // Central Orbit Initial Velocity
            ui->orbCentPVVel_1->setText(line_items[0]);
            ui->orbCentPVVel_2->setText(line_items[1]);
            ui->orbCentPVVel_3->setText(line_items[2]);
            break;
        case 23: // Central Orbit File Format Type
            setQComboBox(ui->orbCentFileType,orbFileTypeInputs.value(line_items[0]));
            break;
        case 24: // Central Orbit File Name
            ui->orbCentFileName->setText(line_string);
            break;
        case 25: // Central Orbit Label in File
            ui->orbCentFileLabel->setText(line_string);
            break;
/************************************** THREE BODY ORBIT **********************************************/
        case 26: // Three Body Orbit Header
            break;
        case 27: // Three Body Orbit Lagrange System
            setQComboBox(ui->orbTBodyLSystem,orbTBodyLSysInputs.value(line_items[0]));
            break;
        case 28: // Three Body Orbit Propagation Method
            setQComboBox(ui->orbTBodyProp,orbTBodyPropInputs.value(line_items[0]));
            break;
        case 29: // Three Body Orbit Initialization Method
            setQComboBox(ui->orbTBodyICParam,orbTBodyICTypeInputs.value(line_items[0]));
            break;
        case 30: // Three Body Orbit Lagrange Point
            setQComboBox(ui->orbTBodyLPoint,line_items[0]);
            break;
        case 31: // Three Body Orbit XY First Semimajor Axis
            ui->orbTBodyModeXYSMA->setText(line_items[0]);
            break;
        case 32: // Three Body Orbit XY First Phase
            ui->orbTBodyModeXYPhase->setText(line_items[0]);
            break;
        case 33: // Three Body Orbit XY First Sense
            if(!line_items[0].compare("CW")) string2radiobool("TRUE",ui->orbTBodyModeSense_CW,ui->orbTBodyModeSense_CCW);
            else string2radiobool("FALSE",ui->orbTBodyModeSense_CW,ui->orbTBodyModeSense_CCW);
            break;
        case 34: // Three Body Orbit XY Second Semimajor Axis
            ui->orbTBodyModeXYSMA_2->setText(line_items[0]);
            break;
        case 35: // Three Body Orbit XY Second Phase
            ui->orbTBodyModeXYPhase_2->setText(line_items[0]);
            break;
        case 36: // Three Body Orbit XY Second Sense
            if(!line_items[0].compare("CW")) string2radiobool("TRUE",ui->orbTBodyModeSense_CW_2,ui->orbTBodyModeSense_CCW_2);
            else string2radiobool("FALSE",ui->orbTBodyModeSense_CW_2,ui->orbTBodyModeSense_CCW_2);
            break;
        case 37: // Three Body Orbit Z Semimajor Axis
            ui->orbTBodyModeZSMA->setText(line_items[0]);
            break;
        case 38: // Three Body Orbit Z Phase
            ui->orbTBodyModeZPhase->setText(line_items[0]);
            break;
        case 39: // Three Body Orbit Initial Position
            ui->orbTBodyCowellPos_1->setText(line_items[0]);
            ui->orbTBodyCowellPos_2->setText(line_items[1]);
            ui->orbTBodyCowellPos_3->setText(line_items[2]);
            break;
        case 40: // Three Body Orbit Initial Velocity
            ui->orbTBodyCowellVel_1->setText(line_items[0]);
            ui->orbTBodyCowellVel_2->setText(line_items[1]);
            ui->orbTBodyCowellVel_3->setText(line_items[2]);
            break;
        case 41: // Three Body Orbit File Type and Label
            setQComboBox(ui->orbTBodyFileType,orbFileTypeInputs.value(line_items[0]));
            ui->orbTBodyFileName->setText(line_string);
            break;
        case 42: // Three Body Orbit File Name
            ui->orbTBodyFileLabel->setText(line_string);
            break;
/*************************************** FORMATION FRAME **********************************************/
        case 43: // Formation Frame Header
            break;
        case 44: // Formation Frame Fixed Frame
            setQComboBox(ui->orbFormFrame,line_items[0]);
            break;
        case 45: // Formation Frame Euler Angles and Sequence
            ui->orbFormFrameEuler_1->setText(line_items[0]);
            ui->orbFormFrameEuler_2->setText(line_items[1]);
            ui->orbFormFrameEuler_3->setText(line_items[2]);
            setQComboBox(ui->orbFormFrameEulerSeq,line_items[3]);
            break;
        case 46: // Formation Frame Origin Position Frame Expression
            setQComboBox(ui->orbFormOrigin,line_items[0]);
            break;
        case 47: // Formation Frame Origin Position
            ui->orbFormOriginPos_1->setText(line_items[0]);
            ui->orbFormOriginPos_2->setText(line_items[1]);
            ui->orbFormOriginPos_3->setText(line_items[2]);
            break;
        default: // Will go here if there are too many lines in the file
            /* error out */
            break;
        }
    }

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



QString ORB_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}

QString ORB_Menu::radiobool2string(QRadioButton *onButton){
    QString output;
    if(onButton->isChecked()) output = "TRUE";
    else output = "FALSE";
    return output;
}

void ORB_Menu::string2radiobool(QString boolString, QRadioButton *onButton, QRadioButton *offButton){
    if(!boolString.compare("TRUE",Qt::CaseInsensitive)) { // compare returns zero if equal
        onButton->setChecked(true);
        offButton->setChecked(false);
    }else {
        onButton->setChecked(false);
        offButton->setChecked(true);
    }
}

void ORB_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

QStringList ORB_Menu::hashValue2QStringList(QHash<QString, QString> hash) {
    QStringList output;
    for(auto i = hash.cbegin(), end = hash.cend(); i != end; ++i)
        output.append(i.value());
    output.sort();
    return output;
}

