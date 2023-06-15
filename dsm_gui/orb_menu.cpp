#include "orb_menu.h"
#include "ui_orb_menu.h"
#include "dsm_gui_lib.h"

#include <QFile>
#include <QDebug>
#include <QHash>
#include <QFileDialog>
//#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
//#include <QInputDialog>
#include <QDirIterator>

ORB_Menu::ORB_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ORB_Menu)
{
    initialize_maps();
    ui->setupUi(this);
    set_validators();
}

ORB_Menu::~ORB_Menu()
{
    delete ui;
}

void ORB_Menu::initialize_maps() {
    QStringList orbTypeInputNames = {"Zero", "Flight", "Central", "Three Body"};
    QStringList orbTBodyLSysInputsNames = {"Earth/Moon", "Sun/Earth", "Sun/Jupiter"};
    QStringList orbCentICTypeInputsNames = {"Keplerian", "Position/Velocity", "File"};
    QStringList orbTBodyPropInputsNames = {"Modes", "Cowell", "Spline"};
    QStringList orbTBodyICTypeInputsNames = {"Modes", "Position/Velocity", "File"};
    QStringList orbFileTypeInputsNames = {"TLE", "TRV", "Spline"};

    this->orbTypeInputs = QMultiMap<int, QString> {{0, "ZERO"},
                                                   {1, "FLIGHT"},
                                                   {2, "CENTRAL"},
                                                   {3, "THREE_BODY"}};
    for(int i=0; i<orbTypeInputNames.count(); i++) orbTypeInputs.insert(i,orbTypeInputNames[i]);

    this->orbTBodyLSysInputs = QMultiMap<int, QString> {{0, "EARTHMOON"},
                                                        {1, "SUNEARTH"},
                                                        {2, "SUNJUPITER"}};
    for(int i=0; i<orbTBodyLSysInputsNames.count(); i++) orbTBodyLSysInputs.insert(i,orbTBodyLSysInputsNames[i]);

    this->orbCentICTypeInputs = QMultiMap<int, QString> {{0, "KEP"},
                                                         {1, "RV"},
                                                         {2, "FILE"}};
    for(int i=0; i<orbCentICTypeInputsNames.count(); i++) orbCentICTypeInputs.insert(i,orbCentICTypeInputsNames[i]);

    this->orbTBodyPropInputs = QMultiMap<int, QString> {{0, "LAGDOF_MODES"},
                                                        {1, "LAGDOF_COWELL"},
                                                        {2, "LAGDOF_SPLINE"}};
    for(int i=0; i<orbTBodyPropInputsNames.count(); i++) orbTBodyPropInputs.insert(i,orbTBodyPropInputsNames[i]);

    this->orbTBodyICTypeInputs = QMultiMap<int, QString> {{0, "MODES"},
                                                          {1, "XYZ"},
                                                          {2, "FILE"}};
    for(int i=0; i<orbTBodyICTypeInputsNames.count(); i++) orbTBodyICTypeInputs.insert(i,orbTBodyICTypeInputsNames[i]);

    this->orbFileTypeInputs = QMultiMap<int, QString> {{0, "TLE"},
                                                       {1, "TRV"},
                                                       {2, "SPLINE"}};
    for(int i=0; i<orbFileTypeInputsNames.count(); i++) orbFileTypeInputs.insert(i,orbFileTypeInputsNames[i]);
}

void ORB_Menu::set_validators()
{
    ui->orbType->addItems(multiMapValue2QStringList(orbTypeInputs));
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
    ui->orbZeroWorld->setMaxVisibleItems(10);

    ui->orbCentWorld->addItems(worldInputs);
    ui->orbCentWorld->setMaxVisibleItems(10);
    ui->orbCentICParam->addItems(multiMapValue2QStringList(orbCentICTypeInputs));
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
    ui->orbCentFileType->addItems(multiMapValue2QStringList(orbFileTypeInputs));

    ui->orbTBodyLSystem->addItems(multiMapValue2QStringList(orbTBodyLSysInputs));
    ui->orbTBodyProp->addItems(multiMapValue2QStringList(orbTBodyPropInputs));
    ui->orbTBodyICParam->addItems(multiMapValue2QStringList(orbTBodyICTypeInputs));
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
    ui->orbTBodyFileType->addItems(multiMapValue2QStringList(orbFileTypeInputs));

    ui->orbEnabled->setId(ui->orbEnabled_on,0); ui->orbEnabled->setId(ui->orbEnabled_off,1);
    ui->orbZeroPolyGrav->setId(ui->orbZeroPolyGrav_on,0); ui->orbZeroPolyGrav->setId(ui->orbZeroPolyGrav_off,1);
    ui->orbFlightPolyGrav->setId(ui->orbFlightPolyGrav_on,0); ui->orbFlightPolyGrav->setId(ui->orbFlightPolyGrav_off,1);
    ui->orbCentJ2->setId(ui->orbCentJ2_on,0); ui->orbCentJ2->setId(ui->orbCentJ2_off,1);
    ui->orbCentPA->setId(ui->orbCentPA_on,0); ui->orbCentPA->setId(ui->orbCentPA_off,1);
    ui->orbTBodyModeSense->setId(ui->orbTBodyModeSense_CW,0); ui->orbTBodyModeSense->setId(ui->orbTBodyModeSense_CCW,1);
    ui->orbTBodyModeSense_2->setId(ui->orbTBodyModeSense_CW_2,0); ui->orbTBodyModeSense_2->setId(ui->orbTBodyModeSense_CCW_2,1);

    ui->orbList->setSortingEnabled(true);
    ui->orbList->sortItems(Qt::AscendingOrder);
}

void ORB_Menu::receive_orbpath(QString path)
{

    inout_path = path;

    QStringList orbFiles = QDir(inout_path).entryList({"Orb_*"});
    for (int i = 0; i<orbFiles.length(); i++) {
        file_paths.append(inout_path+orbFiles[i]); // Full file path of Orbit file
        orb_names.append(orbFiles[i].chopped(4).mid(4)); // Everything between "Orb_" and ".txt"
    }

    file_paths.sort();
    orb_names.sort();

    ui->orbList->clear();
    for(int i=0; i<file_paths.length();i++) {
        file_path = file_paths[i];
        orb_name_index = i;

        receive_data();
        apply_data();
    }
    ui->orbList->addItems(orb_names);

}

void ORB_Menu::receive_data() {
    orb_data.clear();
    orb_string.clear();
    orb_file_headers.clear();
    orb_file_descrip.clear();

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
            if (!capture.isEmpty()) capture += "\n";
            orb_file_headers.append(capture);
            capture = match3.captured(2);
            if (!capture.isEmpty()) capture += "\n";
            orb_file_descrip.append(capture);
        }

//        line.append("\n");
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
    orb_update.clear();
    file.close();
}

void ORB_Menu::apply_data()
{
    QStringList line_items;
    QString line_string;

    ui->orbLabel->setText(orb_names[orb_name_index]);

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
            setQComboBox(ui->orbType,orbTypeInputs.values(orbTypeInputs.key(line_items[0])).at(0));
            break;
/***************************************** ZERO ORBIT *************************************************/
        case 4: // Zero Orbit Header
            break;
        case 5: // Zero Orbit Body
            if(line_items[0].contains("MINORBODY")) {
                ui->orbZeroWorld->setCurrentIndex(worldInputs.indexOf("MINORBODY"));
                ui->orbZeroMinorBodyNum->setValue(line_items[0].rightRef(1).toInt());
            } else {
                ui->orbZeroMinorBodyNum->setValue(0);
                ui->orbZeroWorld->setCurrentIndex(worldInputs.indexOf(line_items[0]));
            }
            break;
        case 6: // Zero Orbit Polyhedral Gravity
            string2radiobool(line_items[0],ui->orbZeroPolyGrav);
            break;
/**************************************** FLIGHT ORBIT ************************************************/
        case 7: // Flight Orbit Header
            break;
        case 8: // Flight Orbit Region
            ui->orbFlightRegion->setValue(line_items[0].toInt());
            break;
        case 9: // Flight Orbit Polyhedral Gravity
            string2radiobool(line_items[0],ui->orbFlightPolyGrav);
            break;
/**************************************** CENTRAL ORBIT ***********************************************/
        case 10: // Central Orbit Header
            break;
        case 11: // Central Orbit World
            if(line_items[0].contains("MINORBODY")) {
                ui->orbCentWorld->setCurrentIndex(worldInputs.indexOf("MINORBODY"));
                ui->orbCentMinorBodyNum->setValue(line_items[0].rightRef(1).toInt());
            } else {
                ui->orbCentWorld->setCurrentIndex(worldInputs.indexOf(line_items[0]));
                ui->orbCentMinorBodyNum->setValue(0);
            }
            break;
        case 12: // Central Orbit J2 Secular Drift
            string2radiobool(line_items[0],ui->orbCentJ2);
            break;
        case 13: // Central Orbit Initial Condition Elements
            setQComboBox(ui->orbCentICParam,orbCentICTypeInputs.values(orbCentICTypeInputs.key(line_items[0])).at(0));
            break;
        case 14: // Central Orbit Peri/Apoapsis Alt or Min Alt & Ecc
            string2radiobool((!line_items[0].compare("PA"))?"TRUE":"FALSE",ui->orbCentPA);
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
            setQComboBox(ui->orbCentFileType,orbFileTypeInputs.values(orbFileTypeInputs.key(line_items[0])).at(0));
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
            setQComboBox(ui->orbTBodyLSystem,orbTBodyLSysInputs.values(orbTBodyLSysInputs.key(line_items[0])).at(0));
            break;
        case 28: // Three Body Orbit Propagation Method
            setQComboBox(ui->orbTBodyProp,orbTBodyPropInputs.values(orbTBodyPropInputs.key(line_items[0])).at(0));
            break;
        case 29: // Three Body Orbit Initialization Method
            setQComboBox(ui->orbTBodyICParam,orbTBodyICTypeInputs.values(orbTBodyICTypeInputs.key(line_items[0])).at(0));
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
            string2radiobool((!line_items[0].compare("CW"))?"TRUE":"FALSE",ui->orbTBodyModeSense);
            break;
        case 34: // Three Body Orbit XY Second Semimajor Axis
            ui->orbTBodyModeXYSMA_2->setText(line_items[0]);
            break;
        case 35: // Three Body Orbit XY Second Phase
            ui->orbTBodyModeXYPhase_2->setText(line_items[0]);
            break;
        case 36: // Three Body Orbit XY Second Sense
            string2radiobool((!line_items[0].compare("CW"))?"TRUE":"FALSE",ui->orbTBodyModeSense_2);
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
            setQComboBox(ui->orbTBodyFileType,orbFileTypeInputs.values(orbFileTypeInputs.key(line_items[0])).at(0));
            ui->orbTBodyFileLabel->setText(line_string);
            break;
        case 42: // Three Body Orbit File Name
            ui->orbTBodyFileName->setText(line_string);
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

void ORB_Menu::on_orbListRemove_clicked() {
    int remove_Item = ui->orbList->currentRow();
    if(remove_Item == -1) return;
    else{
        file_path = file_paths[remove_Item];
        ui->orbList->takeItem(remove_Item);
        orb_names.removeAt(remove_Item);
        file_paths.removeAt(remove_Item);
        QFile::remove(file_path);
        ui->orbList->setCurrentRow(-1);
        global_orb_index = -1;
        orb_name_index = -1;
    }
}

void ORB_Menu::on_orbListAdd_clicked()
{
    QString newOrb = "New";
    for(int i = 0; i <= 50; i++) {
        QString newOrbTest = newOrb;
        if (i>0) newOrbTest += "_" + QString::number(i);
        if (!orb_names.contains(newOrbTest)) {
            newOrb = newOrbTest;
            break;
        }
        if (i==50) return; // Nothing happens if too many
    }
    orb_names.append(newOrb);
    file_path = inout_path+"Orb_"+newOrb+".txt";
    file_paths.append(file_path);

    orb_names.sort();
    file_paths.sort();

    QFile::copy(inout_path + "__default__/Orb_Default.txt", file_path);

    ui->orbList->addItem(newOrb);
    orb_name_index = orb_names.indexOf(newOrb);
    global_orb_index = orb_name_index;

    ui->orbList->setCurrentRow(orb_name_index);

    receive_data();
    apply_data();
}

void ORB_Menu::on_orbList_itemClicked(QListWidgetItem *item) {
    int index = ui->orbList->row(item);

    if (index == global_orb_index) {
        return;
    }
    else {
        if ( (global_orb_index != -1) && (global_orb_ignore == 0) ) {
            int response = dsm_gui_lib::warning_message("Note that changes to the previous selected Orbit are lost unless you first select \"Apply\"! This is your only warning.");
            if (response == QMessageBox::Cancel) {
                ui->orbList->setCurrentRow(global_orb_index);
                global_orb_ignore = 1;
                return;
            }
            else if (response == QMessageBox::Ok) {
                global_orb_ignore = 1;
            }
        }

        file_path = file_paths[index];
        orb_name_index = index;

        receive_data();
        apply_data();

        global_orb_index = index;
    }
}

void ORB_Menu::on_loadDefaultButton_clicked()
{
    int index = ui->orbList->currentRow();
    if(index == -1) return;
    else {
        file_path = file_paths[index];
        int response = dsm_gui_lib::warning_message("Overwrite Orb file?");
        if (response == QMessageBox::Ok) {
            QFile::remove(file_path);
            QFile::copy(inout_path+"__default__/Orb_Default.txt", file_path);

            orb_name_index = index;
            receive_data();
            apply_data();
        }
        else return;
    }
}

void ORB_Menu::on_saveDefaultButton_clicked()
{
    int index = ui->orbList->currentRow();
    if(index == -1) return;
    else {
        file_path = file_paths[index];
        int response = dsm_gui_lib::warning_message("Overwrite Default Orb file?");
        if (response == QMessageBox::Ok) {
            QFile::remove(inout_path+"__default__/Orb_Default.txt");
            QFile::copy(file_path,inout_path+"__default__/Orb_Default.txt");
            orb_name_index = index;
            receive_data();
            apply_data();
        }
        else return;
    }
}

void ORB_Menu::on_closeButton_clicked() {
    ORB_Menu::close();
}

void ORB_Menu::on_applyButton_clicked() {

    int index = ui->orbList->currentRow();
    if(index == -1){
        return;
    }

    QString newLabel = ui->orbLabel->text();
    if ( orb_names.indexOf(newLabel) != index && orb_names.contains(newLabel,Qt::CaseInsensitive)) {
        dsm_gui_lib::warning_message("Orbit \"" + newLabel + "\" already exists. Orbit names are NOT case sensitive.");
        return;
    }

    file_path = file_paths[index];
    ui->orbList->currentItem()->setText(newLabel);
    orb_names[index] = newLabel;
    file_paths[index] = inout_path + "Orb_" + newLabel + ".txt";
    QFile::rename(file_path,file_paths[index]);
    file_path = file_paths[index];

    for(int line_num=1; line_num<=orb_data.length(); line_num++){
        QString data_inp = "";
        switch (line_num) {
            /******************************************* HEADER ***************************************************/
        case 1: // File Header
            break;
        case 2: // Orbit Description
            data_inp = "\"" + ui->orbDescription->text() + "\"";
            break;
        case 3: // Orbit Type
            data_inp = orbTypeInputs.values(orbTypeInputs.key(ui->orbType->currentText())).at(1);
            break;
            /***************************************** ZERO ORBIT *************************************************/
        case 4: // Zero Orbit Header
            break;
        case 5: // Zero Orbit Body
            data_inp = ui->orbZeroWorld->currentText();
            if(data_inp.contains("MINORBODY")) {
                data_inp += "_" + ui->orbZeroMinorBodyNum->cleanText();
            }
            break;
        case 6: // Zero Orbit Polyhedral Gravity
            data_inp = radiobool2string(ui->orbZeroPolyGrav);
            break;
            /**************************************** FLIGHT ORBIT ************************************************/
        case 7: // Flight Orbit Header
            break;
        case 8: // Flight Orbit Region
            data_inp = ui->orbFlightRegion->cleanText();
            break;
        case 9: // Flight Orbit Polyhedral Gravity
            data_inp = radiobool2string(ui->orbFlightPolyGrav);
            break;
            /**************************************** CENTRAL ORBIT ***********************************************/
        case 10: // Central Orbit Header
            break;
        case 11: // Central Orbit World
            data_inp = ui->orbCentWorld->currentText();
            if(data_inp.contains("MINORBODY")) {
                data_inp += "_" + ui->orbCentMinorBodyNum->cleanText();
            }
            break;
        case 12: // Central Orbit J2 Secular Drift
            data_inp = radiobool2string(ui->orbCentJ2);
            break;
        case 13: // Central Orbit Initial Condition Elements
            data_inp = orbCentICTypeInputs.values(orbCentICTypeInputs.key(ui->orbCentICParam->currentText())).at(1);
            break;
        case 14: // Central Orbit Peri/Apoapsis Alt or Min Alt & Ecc
            if(ui->orbCentPA->checkedId()==0) data_inp = "PA";
            else data_inp = "AE";
            break;
        case 15: // Central Orbit Peri/Apoapsis Alt
            data_inp = ui->orbCentKepPeriAlt->text() + "  " + ui->orbCentKepApoAlt->text();
            break;
        case 16: // Central Orbit Min Alt & Ecc
            data_inp = ui->orbCentKepMinAlt->text() + "  " + ui->orbCentKepEcc->text();
            break;
        case 17: // Central Orbit Inclination
            data_inp = ui->orbCentKepInc->text();
            break;
        case 18: // Central Orbit RAAN
            data_inp = ui->orbCentKepRAAN->text();
            break;
        case 19: // Central Orbit Argument of Periapsis
            data_inp = ui->orbCentKepArgPeri->text();
            break;
        case 20: // Central Orbit True Anomaly
            data_inp = ui->orbCentKepTA->text();
            break;
        case 21: // Central Orbit Initial Position
            data_inp = ui->orbCentPVPos_1->text() + "  " + ui->orbCentPVPos_2->text() + "  " + ui->orbCentPVPos_3->text();
            break;
        case 22: // Central Orbit Initial Velocity
            data_inp = ui->orbCentPVVel_1->text() + "  " + ui->orbCentPVVel_2->text() + "  " + ui->orbCentPVVel_3->text();
            break;
        case 23: // Central Orbit File Format Type
            data_inp = orbFileTypeInputs.values(orbFileTypeInputs.key(ui->orbCentFileType->currentText())).at(1);
            break;
        case 24: // Central Orbit File Name
            data_inp = "\"" + ui->orbCentFileName->toPlainText() + "\"";
            break;
        case 25: // Central Orbit Label in File
            data_inp = "\"" + ui->orbCentFileLabel->text() + "\"";
            break;
            /************************************** THREE BODY ORBIT **********************************************/
        case 26: // Three Body Orbit Header
            break;
        case 27: // Three Body Orbit Lagrange System
            data_inp = orbTBodyLSysInputs.values(orbTBodyLSysInputs.key(ui->orbTBodyLSystem->currentText())).at(1);
            break;
        case 28: // Three Body Orbit Propagation Method
            data_inp = orbTBodyPropInputs.values(orbTBodyPropInputs.key(ui->orbTBodyProp->currentText())).at(1);
            break;
        case 29: // Three Body Orbit Initialization Method
            data_inp = orbTBodyICTypeInputs.values(orbTBodyICTypeInputs.key(ui->orbTBodyICParam->currentText())).at(1);
            break;
        case 30: // Three Body Orbit Lagrange Point
            data_inp = ui->orbTBodyLPoint->currentText();
            break;
        case 31: // Three Body Orbit XY First Semimajor Axis
            data_inp = ui->orbTBodyModeXYSMA->text();
            break;
        case 32: // Three Body Orbit XY First Phase
            data_inp = ui->orbTBodyModeXYPhase->text();
            break;
        case 33: // Three Body Orbit XY First Sense
            if(ui->orbTBodyModeSense->checkedId()==0) data_inp = "CW";
            else data_inp = "CCW";
            break;
        case 34: // Three Body Orbit XY Second Semimajor Axis
            data_inp = ui->orbTBodyModeXYSMA_2->text();
            break;
        case 35: // Three Body Orbit XY Second Phase
            data_inp = ui->orbTBodyModeXYPhase_2->text();
            break;
        case 36: // Three Body Orbit XY Second Sense
            if(ui->orbTBodyModeSense_2->checkedId()==0) data_inp = "CW";
            else data_inp = "CCW";
            break;
        case 37: // Three Body Orbit Z Semimajor Axis
            data_inp = ui->orbTBodyModeZSMA->text();
            break;
        case 38: // Three Body Orbit Z Phase
            data_inp = ui->orbTBodyModeZPhase->text();
            break;
        case 39: // Three Body Orbit Initial Position
            data_inp = ui->orbTBodyCowellPos_1->text() + "  " + ui->orbTBodyCowellPos_2->text() + "  " + ui->orbTBodyCowellPos_3->text();
            break;
        case 40: // Three Body Orbit Initial Velocity
            data_inp = ui->orbTBodyCowellVel_1->text() + "  " + ui->orbTBodyCowellVel_2->text() + "  " + ui->orbTBodyCowellVel_3->text();
            break;
        case 41: // Three Body Orbit File Type and Label
            data_inp = orbFileTypeInputs.values(orbFileTypeInputs.key(ui->orbTBodyFileType->currentText())).at(1) + "  \"" + ui->orbTBodyFileLabel->text() + "\"";
            break;
        case 42: // Three Body Orbit File Name
            data_inp = "\"" + ui->orbTBodyFileName->toPlainText() + "\"";
            break;
            /*************************************** FORMATION FRAME **********************************************/
        case 43: // Formation Frame Header
            break;
        case 44: // Formation Frame Fixed Frame
            data_inp = ui->orbFormFrame->currentText();
            break;
        case 45: // Formation Frame Euler Angles and Sequence
            data_inp =  ui->orbFormFrameEuler_1->text() + "  " + ui->orbFormFrameEuler_2->text() + "  ";
            data_inp += ui->orbFormFrameEuler_3->text() + "  " + ui->orbFormFrameEulerSeq->currentText();
            break;
        case 46: // Formation Frame Origin Position Frame Expression
            data_inp = ui->orbFormOrigin->currentText();
            break;
        case 47: // Formation Frame Origin Position
            data_inp = ui->orbFormOriginPos_1->text() + "  " + ui->orbFormOriginPos_2->text() + "  " + ui->orbFormOriginPos_3->text();
            break;
        default: // Will go here if there are too many lines in the file
            /* error out */
            break;
        }
        if(orb_file_headers[line_num-1].isEmpty()){
            orb_update.append(dsm_gui_lib::whitespace(data_inp)+orb_file_descrip[line_num-1]);
        } else {
            orb_update.append(orb_file_headers[line_num-1]);
        }
    }

    orb_names.sort();
    file_paths.sort();
    index = file_paths.indexOf(file_path);
    global_orb_index=index;
    orb_name_index=index;
    ui->orbList->setCurrentRow(index);

    write_data();
}

QString ORB_Menu::radiobool2string(QButtonGroup *buttonGroup){
    QString output;
    if(buttonGroup->checkedId()==0) output = "TRUE";
    else output = "FALSE";
    return output;
}

void ORB_Menu::string2radiobool(QString boolString, QButtonGroup *buttonGroup){
    if(!boolString.compare("TRUE",Qt::CaseInsensitive)) { // compare returns zero if equal
        buttonGroup->button(0)->setChecked(true);
//        emit buttonGroup->idClicked(0);
    }else {
        buttonGroup->button(1)->setChecked(true);
//        emit buttonGroup->idClicked(1);
    }
}

void ORB_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

QStringList ORB_Menu::multiMapValue2QStringList(QMultiMap<int, QString> map) {
    QStringList output;
    for (int i = 0; i < map.uniqueKeys().length(); i++)
        output.append(map.values(i).at(0));
    return output;
}

void ORB_Menu::on_orbType_currentIndexChanged(int index)
{
    for (int i = 0; i < orbTypeInputs.uniqueKeys().count(); i++)
        ui->orbTab->setTabVisible(i,index==i);
    ui->orbTab->setCurrentIndex(index);
}

void ORB_Menu::on_orbCentICParam_currentIndexChanged(int index)
{
    for (int i = 0; i < orbCentICTypeInputs.uniqueKeys().count(); i++)
        ui->orbCentTypeTab->setTabVisible(i,index==i);
    ui->orbCentTypeTab->setCurrentIndex(index);
}

void ORB_Menu::on_orbTBodyICParam_currentIndexChanged(int index)
{
    for (int i = 0; i < orbTBodyICTypeInputs.uniqueKeys().count(); i++)
        ui->orbTBodyTypeTab->setTabVisible(i,index==i);
    ui->orbTBodyTypeTab->setCurrentIndex(index);
}

void ORB_Menu::on_orbZeroWorld_currentTextChanged(const QString &arg1)
{
    ui->orbZeroMinorBodyNum->setEnabled(!arg1.compare("MINORBODY"));
    ui->orbZeroMinorBodyLabel->setEnabled(!arg1.compare("MINORBODY"));
}

void ORB_Menu::on_orbCentWorld_currentTextChanged(const QString &arg1)
{
    ui->orbCentMinorBodyNum->setEnabled(!arg1.compare("MINORBODY"));
    ui->orbCentMinorBodyLabel->setEnabled(!arg1.compare("MINORBODY"));
}

void ORB_Menu::on_orbCentFileSelect_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->orbCentFileName->setText(rel_file_path);
}

void ORB_Menu::on_orbTBodyFileSelect_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->orbTBodyFileName->setText(rel_file_path);
}

void ORB_Menu::on_orbListDuplicate_clicked()
{
    int index = ui->orbList->currentRow();
    QString oldOrb = orb_names[index];
    QString newOrb = oldOrb +"_Copy";
    for(int i = 0; i <= 30; i++) {
        QString newOrbTest = newOrb;
        if(i>0) newOrbTest += "_" + QString::number(i);
        if(!orb_names.contains(newOrbTest)) {
            newOrb = newOrbTest;
            break;
        }
    }
    orb_names.append(newOrb);
    file_path = inout_path+"Orb_"+newOrb+".txt";
    QFile::copy(file_paths[index], file_path);
    file_paths.append(file_path);

    ui->orbList->addItem(newOrb);
    orb_names.sort();
    file_paths.sort();
}

void ORB_Menu::on_orbCentPA_on_toggled(bool checked)
{
    ui->orbCentKepPALabel->setEnabled(checked);
    ui->orbCentKepPeriAlt->setEnabled(checked);
    ui->orbCentKepApoAlt->setEnabled(checked);
    ui->orbCentKepAELabel->setEnabled(!checked);
    ui->orbCentKepMinAlt->setEnabled(!checked);
    ui->orbCentKepEcc->setEnabled(!checked);
}


void ORB_Menu::on_orbTBodyLPoint_currentIndexChanged(int index)
{
    bool isTriPt = ((lagrangePointInputs[index] == "L4") || (lagrangePointInputs[index] == "L5"));
    ui->orbTBodyModeXYSMALabel_2->setEnabled(isTriPt);
    ui->orbTBodyModeXYSMA_2->setEnabled(isTriPt);
    ui->orbTBodyModeXYPhaseLabel_2->setEnabled(isTriPt);
    ui->orbTBodyModeXYPhase_2->setEnabled(isTriPt);
    ui->orbTBodyModeSenseLabel_2->setEnabled(isTriPt);
//    qDebug()<<ui->orbTBodyModeSense_2->id(ui->orbTBodyModeSense_CCW_2);
//    for (int i = 0; i < ui->orbTBodyModeSense_2->buttons().count(); i++) {
//        qDebug()<<i;
//        qDebug()<<ui->orbTBodyModeSense_2->button(i)->isChecked();
    ui->orbTBodyModeSense_CW_2->setEnabled(isTriPt);
    ui->orbTBodyModeSense_CCW_2->setEnabled(isTriPt);
//    }
}

