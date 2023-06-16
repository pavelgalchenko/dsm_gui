#include "orb_menu.h"
#include "ui_orb_menu.h"

ORB_Menu::ORB_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ORB_Menu)
{
    ui->setupUi(this);
    set_validators();
}

ORB_Menu::~ORB_Menu() {
    delete ui;
}

void ORB_Menu::set_validators() {
    QRegularExpression rx("[^\"]*");
    QValidator *noQuotes = new QRegularExpressionValidator(rx,this);

    ui->orbLabel->setValidator(noQuotes);
    ui->orbDescription->setValidator(noQuotes);
    ui->orbType->addItems(dsm_gui_lib::sortStringList(orbTypeInputs.values()));
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
    ui->orbCentICParam->addItems(dsm_gui_lib::sortStringList(orbCentICTypeInputs.values()));
    ui->orbCentKepPeriAlt->setValidator(new QDoubleValidator(-INFINITY,INFINITY,5));
    ui->orbCentKepApoAlt->setValidator(new QDoubleValidator(-INFINITY,INFINITY,5));
    ui->orbCentKepMinAlt->setValidator(new QDoubleValidator);
    ui->orbCentKepInc->setValidator(new QDoubleValidator);
    ui->orbCentKepRAAN->setValidator(new QDoubleValidator);
    ui->orbCentKepTA->setValidator(new QDoubleValidator);
    ui->orbCentPVPos_1->setValidator(new QDoubleValidator);
    ui->orbCentPVPos_2->setValidator(new QDoubleValidator);
    ui->orbCentPVPos_3->setValidator(new QDoubleValidator);
    ui->orbCentPVVel_1->setValidator(new QDoubleValidator);
    ui->orbCentPVVel_2->setValidator(new QDoubleValidator);
    ui->orbCentPVVel_3->setValidator(new QDoubleValidator);
    ui->orbCentFileType->addItems(dsm_gui_lib::sortStringList(orbFileTypeInputs.values()));
    ui->orbCentFileLabel->setValidator(noQuotes);

    ui->orbTBodyLSystem->addItems(dsm_gui_lib::sortStringList(orbTBodyLSysInputs.values()));
    ui->orbTBodyProp->addItems(dsm_gui_lib::sortStringList(orbTBodyPropInputs.values()));
    ui->orbTBodyICParam->addItems(dsm_gui_lib::sortStringList(orbTBodyICTypeInputs.values()));
    ui->orbTBodyLPoint->addItems(lagrangePointInputs);
    ui->orbTBodyModeXYSMA->setValidator(new QDoubleValidator(0,INFINITY,5));
    ui->orbTBodyModeXYPhase->setValidator(new QDoubleValidator);
    ui->orbTBodyModeXYSMA_2->setValidator(new QDoubleValidator(0,INFINITY,5));
    ui->orbTBodyModeXYPhase_2->setValidator(new QDoubleValidator);
    ui->orbTBodyModeZSMA->setValidator(new QDoubleValidator(0,INFINITY,5));
    ui->orbTBodyModeZPhase->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellPos_1->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellPos_2->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellPos_3->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellVel_1->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellVel_2->setValidator(new QDoubleValidator);
    ui->orbTBodyCowellVel_3->setValidator(new QDoubleValidator);
    ui->orbTBodyFileType->addItems(dsm_gui_lib::sortStringList(orbFileTypeInputs.values()));
    ui->orbTBodyFileLabel->setValidator(noQuotes);

    ui->orbEnabled->setId(ui->orbEnabled_on,1); ui->orbEnabled->setId(ui->orbEnabled_off,0);
    ui->orbZeroPolyGrav->setId(ui->orbZeroPolyGrav_on,1); ui->orbZeroPolyGrav->setId(ui->orbZeroPolyGrav_off,0);
    ui->orbFlightPolyGrav->setId(ui->orbFlightPolyGrav_on,1); ui->orbFlightPolyGrav->setId(ui->orbFlightPolyGrav_off,0);
    ui->orbCentJ2->setId(ui->orbCentJ2_on,1); ui->orbCentJ2->setId(ui->orbCentJ2_off,0);
    ui->orbCentPA->setId(ui->orbCentPA_on,1); ui->orbCentPA->setId(ui->orbCentPA_off,0);
    ui->orbTBodyModeSense->setId(ui->orbTBodyModeSense_CW,1); ui->orbTBodyModeSense->setId(ui->orbTBodyModeSense_CCW,0);
    ui->orbTBodyModeSense_2->setId(ui->orbTBodyModeSense_CW_2,1); ui->orbTBodyModeSense_2->setId(ui->orbTBodyModeSense_CCW_2,0);

    ui->orbList->setSortingEnabled(true);
    ui->orbList->sortItems(Qt::AscendingOrder);

    ui->orbCentKepPALabel->setEnabled(false);
    ui->orbCentKepPeriAlt->setEnabled(false);
    ui->orbCentKepApoAlt->setEnabled(false);
    ui->orbCentKepAELabel->setEnabled(false);
    ui->orbCentKepMinAlt->setEnabled(false);
    ui->orbCentKepEcc->setEnabled(false);
    ui->orbCentPA_on->setChecked(true);

    ui->orbCentKepPAWarning->setVisible(false);
    connect(ui->orbCentKepPeriAlt, &QLineEdit::textChanged, this, &ORB_Menu::checkKepPA);
    connect(ui->orbCentKepApoAlt, &QLineEdit::textChanged, this, &ORB_Menu::checkKepPA);
    connect(ui->orbCentICParam, &QComboBox::currentTextChanged, this, &ORB_Menu::checkKepPA);
    connect(ui->orbCentPA, &QButtonGroup::idToggled, this, &ORB_Menu::checkKepPA);
    connect(ui->orbList, &QListWidget::currentRowChanged, this, &ORB_Menu::clear_data);
}

void ORB_Menu::receive_orbpath(QString path) {
    inout_path = path;
    QStringList orbFiles = QDir(inout_path).entryList({"Orb_*"});

    orbFileHash.clear();
    ui->orbList->clear();

    for (int i = 0; i<orbFiles.length(); i++)
        orbFileHash.insert(orbFiles[i].chopped(4).mid(4),inout_path+orbFiles[i].remove("\""));

    ui->orbList->addItems(orbFileHash.keys());
}

void ORB_Menu::receive_data(QString file_path) {
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

    }
    file.close();
}

void ORB_Menu::write_data(QString file_path)
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < orb_update.size(); i++)
            in << orb_update.at(i);
    }
    orb_update.clear();
    file.close();
}

void ORB_Menu::apply_data()
{
    QStringList line_items;
    QString line_string;

    ui->orbLabel->setText(ui->orbList->currentItem()->text());

    // Iterate over the number of lines in the file. Last line can end in newline "\n" character.
    for(int line_num=1; line_num<=orb_data.length(); line_num++) {
        line_string = orb_string[line_num-1];
        line_items = orb_data[line_num-1].remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
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
            setQComboBox(ui->orbCentICParam,orbCentICTypeInputs.value(line_items[0]));
            break;
        case 14: // Central Orbit Peri/Apoapsis Alt or Min Alt & Ecc
            string2radiobool((!line_items[0].compare("PA"))?"TRUE":"FALSE",ui->orbCentPA);
            break;
        case 15: // Central Orbit Peri/Apoapsis Alt
            ui->orbCentKepPeriAlt->setText(line_items[0]);
            ui->orbCentKepApoAlt->setText(line_items[1]);
            ui->orbCentKepPeriAlt->setValidator(new QDoubleValidator(-INFINITY,ui->orbCentKepApoAlt->text().toDouble(), 5));
            ui->orbCentKepApoAlt->setValidator(new QDoubleValidator(ui->orbCentKepPeriAlt->text().toDouble(), INFINITY, 5));
            break;
        case 16: // Central Orbit Min Alt & Ecc
            ui->orbCentKepMinAlt->setText(line_items[0]);
            ui->orbCentKepEcc->setValue(line_items[1].toDouble());
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
            setQComboBox(ui->orbTBodyFileType,orbFileTypeInputs.value(line_items[0]));
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

}

void ORB_Menu::on_orbListRemove_clicked() {
    int removeItem = ui->orbList->currentRow();
    QString removeOrb;
    if(removeItem == -1) return;
    else{
        removeOrb = ui->orbList->currentItem()->text();
        ui->orbList->takeItem(removeItem);
        QFile::remove(orbFileHash[removeOrb]);
        orbFileHash.remove(removeOrb);
        ui->orbList->setCurrentRow(-1);
        global_orb_index = -1;
    }
}

void ORB_Menu::on_orbListAdd_clicked()
{

    QString newOrb = "New";
    if (ui->orbList->count() != 0) {
        for(int i = 0; i <= 50; i++) {
            QString newOrbTest = newOrb;
            if (i>0) newOrbTest += "_" + QString::number(i);
            if (!orbFileHash.contains(newOrbTest)) {
                newOrb = newOrbTest;
                break;
            }
            if (i==50) return; // Nothing happens if too many
        }
    }
    orbFileHash.insert(newOrb,inout_path+"Orb_"+newOrb+".txt");

    QFile::copy(":/data/__default__/Orb_Default.txt", orbFileHash[newOrb]);

    ui->orbList->addItem(newOrb);

    receive_data(orbFileHash[newOrb]);
}

void ORB_Menu::on_orbList_itemClicked() {
    int index = ui->orbList->currentRow();
    QString itemText = ui->orbList->currentItem()->text();

    if (index == global_orb_index) {
        return;
    } else {

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

        receive_data(orbFileHash[itemText]);
        apply_data();

        global_orb_index = index;
    }
}

void ORB_Menu::on_loadDefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Delete all Orbit files and load current Default Orbits?");
    if (response == QMessageBox::Ok) {
        for (auto i = orbFileHash.cbegin(), end = orbFileHash.cend(); i != end; ++i)
            QFile::remove(i.value());
        orbFileHash.clear();
        ui->orbList->clear();

        QStringList orbFiles = QDir(inout_path+"__default__/").entryList({"Orb_*"});

        for (int i = 0; i<orbFiles.length(); i++) {
            QString orb_name = orbFiles[i].chopped(4).mid(4);
            orbFileHash.insert(orb_name,inout_path+orbFiles[i]); // Full file path of Orbit file
            ui->orbList->addItem(orbFiles[i].chopped(4).mid(4)); // Everything between "Orb_" and ".txt"
            QFile::copy(inout_path+"__default__/"+orbFiles[i], orbFileHash[orb_name]);
        }
    }
    else return;
}

void ORB_Menu::on_saveDefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Delete all Default Orbit files and replace with current Orbits?");
    if (response == QMessageBox::Ok) {
        QStringList defOrbFiles = QDir(inout_path+"__default__/").entryList({"Orb_*"});
        for (int i = 0; i < defOrbFiles.count(); i++)
            QFile::remove(inout_path+"__default__/"+defOrbFiles[i]);

        QStringList orbFiles = QDir(inout_path).entryList({"Orb_*"});
        for (auto i = orbFileHash.cbegin(), end = orbFileHash.cend(); i != end; ++i)
            QFile::copy(i.value(),inout_path+"__default__/"+"Orb_" + i.key() + ".txt");
    }
    else return;
}

void ORB_Menu::on_closeButton_clicked() {
    ORB_Menu::close();
}

void ORB_Menu::on_applyButton_clicked() {

    int index = ui->orbList->currentRow();
    if(index == -1) {
        return;
    }
    QString oldOrb = ui->orbList->currentItem()->text();
    QString oldOrbFile = orbFileHash.take(oldOrb);

    QString newLabel = ui->orbLabel->text();
    if (orbFileHash.contains(newLabel)) {
        dsm_gui_lib::warning_message("Orbit \"" + newLabel + "\" already exists. Orbit names are NOT case sensitive.");
        return;
    }

    newLabel = newLabel.remove("\"");
    ui->orbList->currentItem()->setText(newLabel);
    orbFileHash.insert(newLabel,inout_path + "Orb_" + newLabel + ".txt");
    QFile::rename(oldOrbFile,orbFileHash[newLabel]);
    QString file_path = orbFileHash[newLabel];

    for(int line_num=1; line_num<=orb_data.length(); line_num++) {
        QString data_inp = "";
        switch (line_num) {
            /******************************************* HEADER ***************************************************/
        case 1: // File Header
            break;
        case 2: // Orbit Description
            data_inp = "\"" + ui->orbDescription->text().remove("\"") + "\"";
            break;
        case 3: // Orbit Type
            data_inp = orbTypeInputs.key(ui->orbType->currentText());
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
            data_inp = (ui->orbZeroPolyGrav->checkedId())?"TRUE":"FALSE";
            break;
            /**************************************** FLIGHT ORBIT ************************************************/
        case 7: // Flight Orbit Header
            break;
        case 8: // Flight Orbit Region
            data_inp = ui->orbFlightRegion->cleanText();
            break;
        case 9: // Flight Orbit Polyhedral Gravity
            data_inp = (ui->orbFlightPolyGrav->checkedId())?"TRUE":"FALSE";
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
            data_inp = (ui->orbCentJ2->checkedId())?"TRUE":"FALSE";
            break;
        case 13: // Central Orbit Initial Condition Elements
            data_inp = orbCentICTypeInputs.key(ui->orbCentICParam->currentText());
            break;
        case 14: // Central Orbit Peri/Apoapsis Alt or Min Alt & Ecc
            data_inp = (ui->orbCentPA->checkedId())?"PA":"AE";
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
            data_inp = orbFileTypeInputs.key(ui->orbCentFileType->currentText());
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
            data_inp = orbTBodyLSysInputs.key(ui->orbTBodyLSystem->currentText());
            break;
        case 28: // Three Body Orbit Propagation Method
            data_inp = orbTBodyPropInputs.key(ui->orbTBodyProp->currentText());
            break;
        case 29: // Three Body Orbit Initialization Method
            data_inp = orbTBodyICTypeInputs.key(ui->orbTBodyICParam->currentText());
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
            data_inp = (ui->orbTBodyModeSense->checkedId())?"CW":"CCW";
            break;
        case 34: // Three Body Orbit XY Second Semimajor Axis
            data_inp = ui->orbTBodyModeXYSMA_2->text();
            break;
        case 35: // Three Body Orbit XY Second Phase
            data_inp = ui->orbTBodyModeXYPhase_2->text();
            break;
        case 36: // Three Body Orbit XY Second Sense
            data_inp = (ui->orbTBodyModeSense_2->checkedId())?"CW":"CCW";
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
            data_inp = orbFileTypeInputs.key(ui->orbTBodyFileType->currentText()) + "  \"" + ui->orbTBodyFileLabel->text() + "\"";
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
        if(orb_file_headers[line_num-1].isEmpty())
            orb_update.append(dsm_gui_lib::whitespace(data_inp)+orb_file_descrip[line_num-1]);
        else
            orb_update.append(orb_file_headers[line_num-1]);
    }

    write_data(file_path);
}

void ORB_Menu::clear_data()
{    
    // If ui->orbList->currentRow()==1, set all fields to blank
    if (ui->orbList->currentRow()!=-1) return;

    ui->orbLabel->setText("");

    ui->orbDescription->setText("");
    ui->orbType->setCurrentIndex(0);

    ui->orbZeroWorld->setCurrentIndex(0);
    ui->orbZeroMinorBodyNum->setValue(0);
    ui->orbZeroPolyGrav->button(0)->setChecked(true);

    ui->orbFlightRegion->setValue(0);
    ui->orbFlightPolyGrav->button(0)->setChecked(true);

    ui->orbCentWorld->setCurrentIndex(0);
    ui->orbCentMinorBodyNum->setValue(0);
    ui->orbCentJ2->button(0)->setChecked(true);
    ui->orbCentICParam->setCurrentIndex(0);
    ui->orbCentPA->button(0)->setChecked(true);
    ui->orbCentKepPeriAlt->setText("");
    ui->orbCentKepApoAlt->setText("");
    ui->orbCentKepMinAlt->setText("");
    ui->orbCentKepEcc->setValue(0);
    ui->orbCentKepInc->setText("");
    ui->orbCentKepRAAN->setText("");
    ui->orbCentKepArgPeri->setText("");
    ui->orbCentKepTA->setText("");
    ui->orbCentPVPos_1->setText("");
    ui->orbCentPVPos_2->setText("");
    ui->orbCentPVPos_3->setText("");
    ui->orbCentPVVel_1->setText("");
    ui->orbCentPVVel_2->setText("");
    ui->orbCentPVVel_3->setText("");
    ui->orbCentFileType->setCurrentIndex(0);
    ui->orbCentFileName->setText("");
    ui->orbCentFileLabel->setText("");

    ui->orbTBodyLSystem->setCurrentIndex(0);
    ui->orbTBodyProp->setCurrentIndex(0);
    ui->orbTBodyICParam->setCurrentIndex(0);
    ui->orbTBodyLPoint->setCurrentIndex(0);

    ui->orbTBodyModeXYSMA->setText("");
    ui->orbTBodyModeXYPhase->setText("");
    ui->orbTBodyModeSense->button(0)->setChecked(true);
    ui->orbTBodyModeXYSMA_2->setText("");
    ui->orbTBodyModeXYPhase_2->setText("");
    ui->orbTBodyModeSense_2->button(0)->setChecked(true);
    ui->orbTBodyModeZSMA->setText("");
    ui->orbTBodyModeZPhase->setText("");
    ui->orbTBodyCowellPos_1->setText("");
    ui->orbTBodyCowellPos_2->setText("");
    ui->orbTBodyCowellPos_3->setText("");
    ui->orbTBodyCowellVel_1->setText("");
    ui->orbTBodyCowellVel_2->setText("");
    ui->orbTBodyCowellVel_3->setText("");
    ui->orbTBodyFileType->setCurrentIndex(0);
    ui->orbTBodyFileLabel->setText("");
    ui->orbTBodyFileName->setText("");

    ui->orbFormFrame->setCurrentIndex(0);
    ui->orbFormFrameEulerSeq->setCurrentIndex(0);
    ui->orbFormFrameEuler_1->setText("");
    ui->orbFormFrameEuler_2->setText("");
    ui->orbFormFrameEuler_3->setText("");
    ui->orbFormOrigin->setCurrentIndex(0);
    ui->orbFormOriginPos_1->setText("");
    ui->orbFormOriginPos_2->setText("");
    ui->orbFormOriginPos_3->setText("");
}

void ORB_Menu::string2radiobool(QString boolString, QButtonGroup *buttonGroup) {
    if(!boolString.compare("TRUE",Qt::CaseInsensitive)) { // compare returns zero if equal
        buttonGroup->button(1)->setChecked(true);
    }else {
        buttonGroup->button(0)->setChecked(true);
    }
}

void ORB_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

void ORB_Menu::on_orbType_currentTextChanged(const QString &arg1) {
    for (int i = 0; i < ui->orbTab->count(); i++)
        ui->orbTab->setTabVisible(i,!arg1.compare(ui->orbTab->tabText(i)));
}

void ORB_Menu::on_orbCentICParam_currentTextChanged(const QString &arg1) {
    for (int i = 0; i < ui->orbCentTypeTab->count(); i++)
        ui->orbCentTypeTab->setTabVisible(i,!arg1.compare(ui->orbCentTypeTab->tabText(i)));
}

void ORB_Menu::on_orbTBodyICParam_currentTextChanged(const QString &arg1) {
    for (int i = 0; i < ui->orbTBodyTypeTab->count(); i++)
        ui->orbTBodyTypeTab->setTabVisible(i,!arg1.compare(ui->orbTBodyTypeTab->tabText(i)));
}

void ORB_Menu::on_orbZeroWorld_currentTextChanged(const QString &arg1) {
    ui->orbZeroMinorBodyNum->setEnabled(!arg1.compare("MINORBODY"));
    ui->orbZeroMinorBodyLabel->setEnabled(!arg1.compare("MINORBODY"));
}

void ORB_Menu::on_orbCentWorld_currentTextChanged(const QString &arg1) {
    ui->orbCentMinorBodyNum->setEnabled(!arg1.compare("MINORBODY"));
    ui->orbCentMinorBodyLabel->setEnabled(!arg1.compare("MINORBODY"));
}

void ORB_Menu::on_orbCentFileSelect_clicked() {
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->orbCentFileName->setText(rel_file_path);
}

void ORB_Menu::on_orbTBodyFileSelect_clicked() {
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose Folder"), inout_path, QString(), nullptr,  QFileDialog::DontUseNativeDialog);

    QDir dir(inout_path);
    QString rel_file_path = dir.relativeFilePath(file_name);

    if (file_name.isEmpty())
        return;

    ui->orbTBodyFileName->setText(rel_file_path);
}

void ORB_Menu::on_orbListDuplicate_clicked() {
    int index = ui->orbList->currentRow();
    if (index == -1) return;
    QString oldOrb = ui->orbList->currentItem()->text();
    QString newOrb = oldOrb +"_Copy";
    for(int i = 0; i <= 30; i++) {
        QString newOrbTest = newOrb;
        if(i>0) newOrbTest += "_" + QString::number(i);
        if(!orbFileHash.contains(newOrbTest)) {
            newOrb = newOrbTest;
            break;
        }
    }
    orbFileHash.insert(newOrb,inout_path+"Orb_"+newOrb+".txt");
    QFile::copy(orbFileHash[oldOrb], orbFileHash[newOrb]);

    ui->orbList->addItem(newOrb);
}

void ORB_Menu::on_orbCentPA_on_toggled(bool checked) {
    ui->orbCentKepPALabel->setEnabled(checked);
    ui->orbCentKepPeriAlt->setEnabled(checked);
    ui->orbCentKepApoAlt->setEnabled(checked);
    ui->orbCentKepAELabel->setEnabled(!checked);
    ui->orbCentKepMinAlt->setEnabled(!checked);
    ui->orbCentKepEcc->setEnabled(!checked);
}

void ORB_Menu::on_orbTBodyLPoint_currentTextChanged(const QString &text) {
    bool isTriPt = ((!text.compare("L4")) || (!text.compare("L5")));
    ui->orbTBodyModeXYSMALabel_2->setEnabled(isTriPt);
    ui->orbTBodyModeXYSMA_2->setEnabled(isTriPt);
    ui->orbTBodyModeXYPhaseLabel_2->setEnabled(isTriPt);
    ui->orbTBodyModeXYPhase_2->setEnabled(isTriPt);
    ui->orbTBodyModeSenseLabel_2->setEnabled(isTriPt);
    ui->orbTBodyModeSense_CW_2->setEnabled(isTriPt);
    ui->orbTBodyModeSense_CCW_2->setEnabled(isTriPt);
}

void ORB_Menu::checkKepPA() {
    int i;
    QString apoAlt = ui->orbCentKepApoAlt->text();
    QString periAlt = ui->orbCentKepPeriAlt->text();
    ui->orbCentKepPeriAlt->setValidator(new QDoubleValidator(-INFINITY, apoAlt.toDouble(), 5));
    ui->orbCentKepApoAlt->setValidator(new QDoubleValidator(periAlt.toDouble(), INFINITY, 5));
    if (!(ui->orbCentPA->checkedId()==1) || ui->orbCentICParam->currentText().compare("Keplerian")) {
        ui->orbCentKepPAWarning->setVisible(false);
    }
    else if (ui->orbCentKepPeriAlt->validator()->validate(periAlt,i)==QValidator::Intermediate) {
        ui->orbCentKepPAWarning->setText("Periapsis Altitude cannot be greater than Apoapsis Altitude!");
        ui->orbCentKepPAWarning->setVisible(true);
    }
    else if (ui->orbCentKepApoAlt->validator()->validate(apoAlt,i)==QValidator::Intermediate) {
        ui->orbCentKepPAWarning->setText("Apoapsis Altitude cannot be less than Periapsis Altitude!");
        ui->orbCentKepPAWarning->setVisible(true);
    }
    else {
        ui->orbCentKepPAWarning->setVisible(false);
    }
}


