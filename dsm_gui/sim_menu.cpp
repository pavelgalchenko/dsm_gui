#include "sim_menu.h"
#include "ui_sim_menu.h"

SIM_Menu::SIM_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SIM_Menu)
{
    ui->setupUi(this);
    set_validators();
}

SIM_Menu::~SIM_Menu()
{
    delete ui;
}

void SIM_Menu::set_validators() {
    QRegularExpression rx("[^\"]*");
    QValidator *noQuotes = new QRegularExpressionValidator(rx,this);

    ui->simTimeMode->addItems(dsm_gui_lib::sortStringList(timeModeInputs.values()));
    ui->simSimDur->setValidator(new QDoubleValidator);
    ui->simSimStep->setValidator(new QDoubleValidator);
    ui->simFileInterval->setValidator(new QDoubleValidator);
    ui->simRNGSeed->setValidator(new QIntValidator);
    // ui->simCmdFile->setValidattor(noQuotes);

    ui->simOrbList->setSortingEnabled(true);
    ui->simOrbList->sortItems(Qt::AscendingOrder);

    ui->simSCList->setSortingEnabled(true);
    ui->simSCList->sortItems(Qt::AscendingOrder);

    ui->simGSList->setSortingEnabled(true);
    ui->simGSList->sortItems(Qt::AscendingOrder);

    ui->simDateTime->setCalendarPopup(true);
    ui->simDateTime->setTimeSpec(Qt::UTC);
    ui->simDateTime->setDisplayFormat("MM/dd/yyyy hh:mm:ss.zz");
    ui->simLeapSec->setValidator(new QDoubleValidator);
    ui->simF107Ap->addItems(dsm_gui_lib::sortStringList(f107Inputs.values()));
    ui->simF107->setValidator(new QDoubleValidator);
    ui->simAp->setValidator(new QDoubleValidator);
    ui->simMagfieldType->addItems(dsm_gui_lib::sortStringList(magfieldInputs.values()));

    ui->simEphem->addItems(dsm_gui_lib::sortStringList(ephemInputs.values()));

    ui->simGSWorld->addItems(worldInputs);
    ui->simGSLong->setValidator(new QDoubleValidator);
    ui->simGSLat->setValidator(new QDoubleValidator);
    ui->simGSLabel->setValidator(noQuotes);

    celestialBodies = {ui->simMercuryEn,
                       ui->simVenusEn,
                       ui->simEarthEn,
                       ui->simMarsEn,
                       ui->simJupiterEn,
                       ui->simSaturnEn,
                       ui->simUranusEn,
                       ui->simNeptuneEn,
                       ui->simPlutoEn,
                       ui->simAsteroidsEn};

    lagrangeSystems = {ui->simEarthMoonEn,
                       ui->simSunEarthEn,
                       ui->simSunJupiterEn};

}

void SIM_Menu::receive_simpath(QString path) {
    inoutPath = path;
    filePath = inoutPath+"Inp_Sim.txt"; // Maybe add ability for multiple sim files?

    receive_data();
    apply_data();
}

void SIM_Menu::receive_data() {
    int i = 0;
    simData.clear();
    simString.clear();
    simFileHeaders.clear();
    simFileDescrip.clear();

    QString newKey;
    QStringList orbFiles = QDir(inoutPath).entryList({"Orb_*"});

    orbFileHash.clear();
    ui->simOrbList->clear();
    ui->simSCOrbit->clear();

    for (int i = 0; i<orbFiles.length(); i++) {
        newKey = orbFiles[i].chopped(4).mid(4);
        orbFileHash.insert(newKey,orbFiles[i].remove("\""));
        ui->simOrbList->addItem(newKey);
        ui->simOrbList->findItems(newKey,Qt::MatchExactly).at(0)->setData(Qt::UserRole+0,false);
        ui->simOrbList->findItems(newKey,Qt::MatchExactly).at(0)->setData(Qt::UserRole+1,-1);
    }
    ui->simSCOrbit->addItems(orbFileHash.keys());

    QStringList scFiles = QDir(inoutPath).entryList({"SC_*"});

    scFileHash.clear();
    ui->simSCList->clear();

    for (int i = 0; i<scFiles.length(); i++) {
        newKey = scFiles[i].chopped(4).mid(3);
        scFileHash.insert(newKey,scFiles[i].remove("\""));
        ui->simSCList->addItem(newKey);
        ui->simSCList->findItems(newKey,Qt::MatchExactly).at(0)->setData(Qt::UserRole+0,false);
        ui->simSCList->findItems(newKey,Qt::MatchExactly).at(0)->setData(Qt::UserRole+1,"");
    }

    // Return everything up to and including ! (exclamation point)
    static QRegularExpression rx1("(.*?)!");

    // Return everything between a set of " " (quotation marks)
    static QRegularExpression rx2("\"(.*?)\"");

    // If the line does NOT start with an alphanumeric character or " (single quotation), then return the line as first group.
    // Otherwise return everything after ! (exclamation point) as second group
    static QRegularExpression rx3("(?:(?=^[^[:alnum:]|\"])([^[:alnum:]|\"].*)|(!.*))");

    // Matches only with numbers
    static QRegularExpression rx4("[0-9]+");

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    headerLines["HEADER"]=1;
    while(!in.atEnd()) {
        i++;

        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        simData.append(match1.captured(1)); // index 0 includes ! character, index 1 does not

        QRegularExpressionMatch match2 = rx2.match(line);
        simString.append(match2.captured(1)); // index 0 includes "" characters, index 1 does not

        QRegularExpressionMatch match3 = rx3.match(line);
        if (match3.hasMatch()) {
            QString capture = match3.captured(1);
            if (!capture.isEmpty()) {
                foreach (QString key, headerLines.keys()) {
                    if (capture.contains(key,Qt::CaseInsensitive))
                        headerLines[key] = i;
                }
            }
            else
                capture = match3.captured(2);
            capture += "\n";

            simFileHeaders.append(capture);
            simFileDescrip.append(capture);
        }
    }
    orbDescription = simFileDescrip[headerLines["ORBITS"]+1].replace(rx4,"%1");
    scDescription = simFileDescrip[headerLines["SPACECRAFT"]+1].replace(rx4,"%1");
    gsDescription = simFileDescrip[headerLines["GROUND"]+1];

    file.close();
}

void SIM_Menu::apply_data() {
    QStringList line_items;
    QString line_string;
    QString label;
    QStringList refs;
    QListWidgetItem *item;

    ui->simGSList->clear();

    for(int lineNum=1; lineNum<=simData.length(); lineNum++) {
        line_string = simString[lineNum-1];
        line_items = simData[lineNum-1].remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
        if (lineNum < headerLines["ORBITS"]) {
            switch (lineNum-headerLines["CONTROL"]) {
            case 1:
                setQComboBox(ui->simTimeMode,timeModeInputs.value(line_items[0]));
                break;
            case 2:
                ui->simSimDur->setText(line_items[0]);
                ui->simSimStep->setText(line_items[1]);
                break;
            case 3:
                ui->simFileInterval->setText(line_items[0]);
                break;
            case 4:
                ui->simRNGSeed->setText(line_items[0]);
                break;
            case 5:
                ui->simGraphicsEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 6:
                // ui->simCmdFile->setText(line_items[0]);
                break;
            default:
                break;
            }
        }
        else if (lineNum > headerLines["ORBITS"] && lineNum < headerLines["SPACECRAFT"]) {
            refs = orbFileHash.keys();
            if (lineNum - headerLines["ORBITS"] >= 2) {
                label = line_items[1].chopped(4).mid(4);
                if (refs.contains(label)) {
                    item = ui->simOrbList->findItems(label,Qt::MatchExactly).at(0);
                    item->setData(Qt::UserRole+0,QVariant(line_items[0]).toBool());
                    item->setData(Qt::UserRole+1,lineNum-headerLines["ORBITS"]-2);
                }
            }
        }
        else if (lineNum > headerLines["SPACECRAFT"] && lineNum < headerLines["ENVIRONMENT"]) {
            // Right now, cannot have duplicate spacecraft in the list
            refs = scFileHash.keys();
            if (lineNum - headerLines["SPACECRAFT"] >= 2) {
                label = line_items[2].chopped(4).mid(3);
                if (refs.contains(label)) {
                    item = ui->simSCList->findItems(label,Qt::MatchExactly).at(0);
                    item->setData(Qt::UserRole+0,QVariant(line_items[0]).toBool());
                    foreach (QListWidgetItem* orbItem, ui->simOrbList->findItems("*",Qt::MatchWildcard)) {
                        if (orbItem->data(Qt::UserRole+1).toInt() == line_items[1].toInt()) {
                            item->setData(Qt::UserRole+1,orbItem->text());
                        }

                }
                }
            }
        }
        else if (lineNum > headerLines["ENVIRONMENT"] && lineNum < headerLines["BODIES"]) {
            switch (lineNum-headerLines["ENVIRONMENT"]) {
            case 1:
                ui->simDateTime->setDate(QDate::fromString(line_items[2]+"-"+line_items[0]+"-"+line_items[1],Qt::ISODate));
                break;
            case 2:
                ui->simDateTime->setTime(QTime::fromString(line_items[0]+":"+line_items[1]+":"+line_items[2],Qt::TextDate));
                break;
            case 3:
                ui->simLeapSec->setText(line_items[0]);
                break;
            case 4:
                setQComboBox(ui->simF107Ap,f107Inputs.value(line_items[0]));
                break;
            case 5:
                ui->simF107->setText(line_items[0]);
                break;
            case 6:
                ui->simAp->setText(line_items[0]);
                break;
            case 7:
                setQComboBox(ui->simMagfieldType,magfieldInputs.value(line_items[0]));
                break;
            case 8:
                ui->simIGRFDegree->setValue(line_items[0].toInt());
                ui->simIGRFOrder->setValue(line_items[1].toInt());
                break;
            case 9:
                ui->simEarthHarmN->setValue(line_items[0].toInt());
                ui->simEarthHarmM->setValue(line_items[1].toInt());
                break;
            case 10:
                ui->simMarsHarmN->setValue(line_items[0].toInt());
                ui->simMarsHarmM->setValue(line_items[1].toInt());
                break;
            case 11:
                ui->simLunaHarmN->setValue(line_items[0].toInt());
                ui->simLunaHarmM->setValue(line_items[1].toInt());
                break;
            case 12:
                ui->simAeroPertEn->setChecked(QVariant(line_items[0]).toBool());
                ui->simAeroPertShadow->setChecked(QVariant(line_items[1]).toBool());
                break;
            case 13:
                ui->simGravGradientEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 14:
                ui->simSRPPertEn->setChecked(QVariant(line_items[0]).toBool());
                ui->simSRPPertShadow->setChecked(QVariant(line_items[1]).toBool());
                break;
            case 15:
                ui->simResidualMagEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 16:
                ui->simGravPertEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 17:
                ui->simThrusterPlumeEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 18:
                ui->simContactEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 19:
                ui->simSloshEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 20:
                ui->simAlbedoEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            case 21:
                ui->simGravGradientEn->setChecked(QVariant(line_items[0]).toBool());
                break;
            default:
                break;
            }
        }
        else if (lineNum > headerLines["BODIES"] && lineNum < headerLines["LAGRANGE"]) {
            switch (lineNum-headerLines["BODIES"]) {
            case 1:
                setQComboBox(ui->simEphem,ephemInputs.value(line_items[0]));
                break;
            default:
                celestialBodies[lineNum-headerLines["BODIES"]-2]->setChecked(QVariant(line_items[0]).toBool());
                break;
            }
        }
        else if (lineNum > headerLines["LAGRANGE"] && lineNum < headerLines["GROUND"]) {
            lagrangeSystems[lineNum-headerLines["LAGRANGE"]-1]->setChecked(QVariant(line_items[0]).toBool());
        }
        else if (lineNum > headerLines["GROUND"]) {
            if (lineNum - headerLines["GROUND"] >=2) {
                ui->simGSList->addItem(line_string);
                item = ui->simGSList->findItems(line_string,Qt::MatchExactly).at(0);
                item->setData(Qt::UserRole+0,QVariant(line_items[0]).toBool());
                item->setData(Qt::UserRole+1,line_items[1]);
                item->setData(Qt::UserRole+2,line_items[2].toDouble());
                item->setData(Qt::UserRole+3,line_items[3].toDouble());
            }
        }
    }
}

void SIM_Menu::write_data()
{
    QFile::remove(filePath);
    QFile file(filePath);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < simUpdate.size(); i++)
            in << simUpdate.at(i);
    }
    simUpdate.clear();
    file.close();
}

void SIM_Menu::clear_data() {
    ui->simTimeMode->setCurrentIndex(0);
    ui->simSimDur->clear();
    ui->simSimStep->clear();
    ui->simFileInterval->clear();
    ui->simRNGSeed->clear();
    ui->simGraphicsEn->setChecked(false);
    // ui->simCmdFile->clear();

    ui->simOrbList->clear();
    ui->simOrbitEn->setChecked(false);

    ui->simSCList->clear();
    ui->simSCEn->setChecked(false);
    ui->simSCOrbit->clear();

    ui->simDateTime->clear();
    ui->simLeapSec->clear();
    ui->simF107Ap->setCurrentIndex(0);
    ui->simF107->clear();
    ui->simAp->clear();
    ui->simMagfieldType->setCurrentIndex(0);
    ui->simIGRFDegree->setValue(0);
    ui->simIGRFOrder->setValue(0);
    ui->simEarthHarmN->setValue(0);
    ui->simEarthHarmM->setValue(0);
    ui->simMarsHarmN->setValue(0);
    ui->simMarsHarmM->setValue(0);
    ui->simLunaHarmN->setValue(0);
    ui->simLunaHarmM->setValue(0);
    ui->simAeroPertEn->setChecked(false);
    ui->simAeroPertShadow->setChecked(false);
    ui->simGravGradientEn->setChecked(false);
    ui->simSRPPertEn->setChecked(false);
    ui->simSRPPertShadow->setChecked(false);
    ui->simResidualMagEn->setChecked(false);
    ui->simGravPertEn->setChecked(false);
    ui->simThrusterPlumeEn->setChecked(false);
    ui->simContactEn->setChecked(false);
    ui->simSloshEn->setChecked(false);
    ui->simAlbedoEn->setChecked(false);
    ui->simOutputTorqueEn->setChecked(false);

    ui->simEphem->setCurrentIndex(0);
    for (int i=0; i<celestialBodies.count(); i++) celestialBodies[i]->setChecked(false);

    for (int i=0; i<lagrangeSystems.count(); i++) lagrangeSystems[i]->setChecked(false);

    ui->simGSList->clear();

    orbFileHash.clear();
    scFileHash.clear();
}

void SIM_Menu::on_loadDefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Delete Inp_Sim file and load current Default?");
    if (response == QMessageBox::Ok) {
        clear_data();

        QFile::remove(filePath);
        QFile::copy(inoutPath+"__default__/Inp_Sim.txt", filePath);

        receive_data();
        apply_data();
    }
    else return;
}

void SIM_Menu::on_saveDefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Delete Default Inp_Sim file and replace with current?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inoutPath+"__default__/Inp_Sim.txt");
        QFile::copy(filePath, inoutPath+"__default__/Inp_Sim.txt");
    }
    else return;
}

void SIM_Menu::on_closeButton_clicked() {
    SIM_Menu::close();
}

void SIM_Menu::on_applyButton_clicked() {
    QStringList refs;
    QString dataInp;

    simUpdate.append(simFileHeaders[headerLines["HEADER"]-1]);
    simUpdate.append(simFileHeaders[headerLines["CONTROL"]-1]);

    for (int lineNum=1; lineNum<headerLines["ORBITS"]-headerLines["CONTROL"]; lineNum++) {
        switch (lineNum) {
        case 1:
            dataInp = timeModeInputs.key(ui->simTimeMode->currentText());
            break;
        case 2:
            dataInp = ui->simSimDur->text() + "  " + ui->simSimStep->text();
            break;
        case 3:
            dataInp = ui->simFileInterval->text();
            break;
        case 4:
            dataInp = ui->simRNGSeed->text();
            break;
        case 5:
            dataInp = QVariant(ui->simGraphicsEn->isChecked()).toString().toUpper();
            break;
        case 6:
            dataInp = "Inp_Cmd.txt";
            break;
        default:
            break;
        }
        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+simFileDescrip[lineNum-1+headerLines["CONTROL"]]);
    }

    simUpdate.append(simFileHeaders[headerLines["ORBITS"]-1]);
    dataInp = dsm_gui_lib::whitespace(QString::number(ui->simOrbList->count()));
    simUpdate.append(dataInp+simFileDescrip[headerLines["ORBITS"]]);
    for (int i=0; i<ui->simOrbList->count(); i++) {
        QListWidgetItem* curItem = ui->simOrbList->item(i);

        curItem->setData(Qt::UserRole+1,i); // so Spacecraft can reference the number
        dataInp  = curItem->data(Qt::UserRole+0).toString().toUpper() + "  ";
        dataInp += orbFileHash[curItem->text()];

        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+orbDescription.arg(i));
    }

    simUpdate.append(simFileHeaders[headerLines["SPACECRAFT"]-1]);
    dataInp = dsm_gui_lib::whitespace(QString::number(ui->simSCList->count()));
    simUpdate.append(dataInp+simFileDescrip[headerLines["SPACECRAFT"]]);
    for (int i=0; i<ui->simSCList->count(); i++) {
        QListWidgetItem* curItem = ui->simSCList->item(i);

        dataInp  = curItem->data(Qt::UserRole+0).toString().toUpper() + "  ";
        dataInp += ui->simOrbList->findItems(curItem->data(Qt::UserRole+1).toString(),Qt::MatchExactly).at(0)->data(Qt::UserRole+1).toString() + "  ";
        dataInp += scFileHash[curItem->text()];

        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+scDescription.arg(i));
    }

    simUpdate.append(simFileHeaders[headerLines["ENVIRONMENT"]-1]);

    for (int lineNum=1; lineNum<headerLines["BODIES"]-headerLines["ENVIRONMENT"]; lineNum++) {
        switch (lineNum) {
        case 1:
            dataInp = ui->simDateTime->date().toString("MM dd yyyy");
            break;
        case 2:
            dataInp = ui->simDateTime->time().toString("HH mm ss.zz");
            break;
        case 3:
            dataInp = ui->simLeapSec->text();
            break;
        case 4:
            dataInp = f107Inputs.key(ui->simF107Ap->currentText());
            break;
        case 5:
            dataInp = ui->simF107->text();
            break;
        case 6:
            dataInp = ui->simAp->text();
            break;
        case 7:
            dataInp = ui->simMagfieldType->currentText();
            break;
        case 8:
            dataInp = ui->simIGRFDegree->text() + "  " + ui->simIGRFOrder->text();
            break;
        case 9:
            dataInp = ui->simEarthHarmN->text() + "  " + ui->simEarthHarmM->text();
            break;
        case 10:
            dataInp = ui->simMarsHarmN->text() + "  " + ui->simMarsHarmM->text();
            break;
        case 11:
            dataInp = ui->simLunaHarmN->text() + "  " + ui->simLunaHarmM->text();
            break;
        case 12:
            dataInp = QVariant(ui->simAeroPertEn->isChecked()).toString().toUpper() + "  ";
            dataInp += QVariant(ui->simAeroPertShadow->isChecked()).toString().toUpper();
            break;
        case 13:
            dataInp = QVariant(ui->simGravGradientEn->isChecked()).toString().toUpper();
            break;
        case 14:
            dataInp = QVariant(ui->simSRPPertEn->isChecked()).toString().toUpper() + "  ";
            dataInp += QVariant(ui->simSRPPertShadow->isChecked()).toString().toUpper();
            break;
        case 15:
            dataInp = QVariant(ui->simResidualMagEn->isChecked()).toString().toUpper();
            break;
        case 16:
            dataInp = QVariant(ui->simGravPertEn->isChecked()).toString().toUpper();
            break;
        case 17:
            dataInp = QVariant(ui->simThrusterPlumeEn->isChecked()).toString().toUpper();
            break;
        case 18:
            dataInp = QVariant(ui->simContactEn->isChecked()).toString().toUpper();
            break;
        case 19:
            dataInp = QVariant(ui->simSloshEn->isChecked()).toString().toUpper();
            break;
        case 20:
            dataInp = QVariant(ui->simAlbedoEn->isChecked()).toString().toUpper();
            break;
        case 21:
            dataInp = QVariant(ui->simOutputTorqueEn->isChecked()).toString().toUpper();
            break;
        default:
            break;
        }
        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+simFileDescrip[lineNum-1+headerLines["ENVIRONMENT"]]);
    }

    simUpdate.append(simFileHeaders[headerLines["BODIES"]-1]);
    dataInp = dsm_gui_lib::whitespace(ephemInputs.key(ui->simEphem->currentText()));
    simUpdate.append(dataInp+simFileDescrip[headerLines["BODIES"]]);
    for (int i = 0; i<celestialBodies.count(); i++) {
        dataInp = QVariant(celestialBodies[i]->isChecked()).toString().toUpper();
        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+simFileDescrip[i+headerLines["BODIES"]+1]);
    }

    simUpdate.append(simFileHeaders[headerLines["LAGRANGE"]-1]);
    for (int i = 0; i<lagrangeSystems.count(); i++) {
        dataInp = QVariant(lagrangeSystems[i]->isChecked()).toString().toUpper();
        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+simFileDescrip[i+headerLines["LAGRANGE"]]);
    }

    simUpdate.append(simFileHeaders[headerLines["GROUND"]-1]);
    dataInp = dsm_gui_lib::whitespace(QString::number(ui->simGSList->count()));
    simUpdate.append(dataInp+simFileDescrip[headerLines["GROUND"]]);
    for (int i=0; i<ui->simGSList->count(); i++) {
        QListWidgetItem* curItem = ui->simGSList->item(i);
        dataInp = curItem->data(Qt::UserRole+0).toString().toUpper() + " ";
        for (int j=1; j<4; j++)
            dataInp += curItem->data(Qt::UserRole+j).toString() + " ";
        dataInp += "\"" + curItem->text() + "\"";

        simUpdate.append(dsm_gui_lib::whitespace(dataInp)+gsDescription);
    }
    write_data();
}


void SIM_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

QStringList SIM_Menu::getTextFromList(QListWidget *list){
    QStringList output;
    foreach(QListWidgetItem *item, list->findItems("*",Qt::MatchWildcard))
        output.append(item->text());
    output.sort(Qt::CaseInsensitive);
    return output;
}


void SIM_Menu::on_simOrbList_itemClicked(QListWidgetItem *item) {
    ui->simOrbitEn->setChecked(item->data(Qt::UserRole+0).toBool());
}

void SIM_Menu::on_simOrbitEn_toggled(bool checked) {
    if (ui->simOrbList->currentRow()==-1) return;
    ui->simOrbList->currentItem()->setData(Qt::UserRole+0,checked);
}

void SIM_Menu::on_simSCList_itemClicked(QListWidgetItem *item) {
    ui->simSCEn->setChecked(item->data(Qt::UserRole+0).toBool());
    ui->simSCOrbit->setCurrentText(item->data(Qt::UserRole+1).toString());
}

void SIM_Menu::on_simSCEn_toggled(bool checked) {
    if (ui->simSCList->currentRow()==-1) return;
    ui->simSCList->currentItem()->setData(Qt::UserRole+0,checked);
}

void SIM_Menu::on_simSCOrbit_currentTextChanged(const QString &arg1) {
    if (ui->simSCList->currentRow()==-1) return;
    ui->simSCList->currentItem()->setData(Qt::UserRole+1,arg1);
}

void SIM_Menu::on_simGSList_itemClicked(QListWidgetItem *item) {
    ui->simGSEn->setChecked(item->data(Qt::UserRole+0).toBool());
    ui->simGSWorld->setCurrentText(item->data(Qt::UserRole+1).toString());
    ui->simGSLong->setText(item->data(Qt::UserRole+2).toString());
    ui->simGSLat->setText(item->data(Qt::UserRole+3).toString());
    ui->simGSLabel->setText(item->text());
}

void SIM_Menu::on_simGSEn_toggled(bool checked) {
    if (ui->simGSList->currentRow()==-1) return;
    ui->simGSList->currentItem()->setData(Qt::UserRole+0,checked);
}

void SIM_Menu::on_simGSWorld_currentTextChanged(const QString &arg1) {
    if (ui->simGSList->currentRow()==-1) return;
    ui->simGSList->currentItem()->setData(Qt::UserRole+1,arg1);
}

void SIM_Menu::on_simGSLat_textEdited(const QString &arg1) {
    if (ui->simGSList->currentRow()==-1) return;
    ui->simGSList->currentItem()->setData(Qt::UserRole+2,arg1.toDouble());
}

void SIM_Menu::on_simGSLong_textEdited(const QString &arg1) {
    if (ui->simGSList->currentRow()==-1) return;
    ui->simGSList->currentItem()->setData(Qt::UserRole+3,arg1.toDouble());
}

void SIM_Menu::on_simGSLabel_textEdited(const QString &arg1) {
    if (ui->simGSList->currentRow()==-1) return;
    ui->simGSList->currentItem()->setText(arg1);
}
