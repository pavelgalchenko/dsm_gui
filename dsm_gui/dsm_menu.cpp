#include "dsm_menu.h"
#include "ui_dsm_menu.h"

DSM_Menu::DSM_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DSM_Menu)
{
    ui->setupUi(this);
    set_validators();
}

DSM_Menu::~DSM_Menu() {
    delete ui;
}

void DSM_Menu::set_validators() {



    ui->cmdTimelineTree->setHeaderLabels(tlColNames.values());
    ui->cmdConfigTree->setHeaderLabels(cmdColNames.values());
    ui->ctrlConfigTree->setHeaderLabels(ctrlColNames.values());



    QTreeWidget *parent = ui->cmdConfigTree;
    QTreeWidgetItem *newItem;
    foreach (dsmSectionTypes type, section2Cmd.keys()) {
        switch (type) {
        case dsmSectionTypes::TRANSLATION:
            newItem = new QTreeWidgetItem(parent,{"Translation Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::PRIMARY_VEC:
            newItem = new QTreeWidgetItem(parent,{"Primary Vector Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::SECONDARY_VEC:
            newItem = new QTreeWidgetItem(parent,{"Secondary Vector Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::QUATERION:
            newItem = new QTreeWidgetItem(parent,{"Quaternion Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::MIRROR:
            newItem = new QTreeWidgetItem(parent,{"Mirror Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::DETUMBLE:
            newItem = new QTreeWidgetItem(parent,{"Detumble Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::WHLHMANAGEMENT:
            newItem = new QTreeWidgetItem(parent,{"Wheel Momentum Management Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::ACTUATOR_CMD:
            newItem = new QTreeWidgetItem(parent,{"Actuator Command"});
            parent->addTopLevelItem(newItem);
            break;
        case dsmSectionTypes::MANEUVER:
            newItem = new QTreeWidgetItem(parent,{"Maneuver Command"});
            parent->addTopLevelItem(newItem);
            break;
        default:
            newItem = NULL;
            break;
        }
        if (newItem!=NULL)
            entryCmdParents[section2Cmd[type]]=newItem;
    }
}

void DSM_Menu::receive_dsmpath(QString path) {
    inoutPath = path;
    filePath = inoutPath+"Inp_DSM.txt";

    receive_data();
    apply_data();
}

void DSM_Menu::receive_data() {
    dsmData.clear();
    dsmString.clear();
    dsmUpdate.clear();
    static QRegularExpression rxSC("SC_(.*).txt");

    QFile simFile(inoutPath + "Inp_Sim.txt");
    if(!simFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", simFile.errorString());
    QTextStream simIn(&simFile);
    QTreeWidgetItem *newSC;
    while (!simIn.atEnd()) {
        QString line = simIn.readLine();
        if (line.contains("Spacecraft",Qt::CaseInsensitive)) {
            line = simIn.readLine();
            QStringList line_items = line.remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
            int nSC = line_items[0].toInt();
            for (int i=0; i<nSC; i++) {
                line = simIn.readLine();
                QString name = rxSC.match(line).captured(1);
                scNames.append(name);
                newSC = new QTreeWidgetItem(ui->cmdTimelineTree,{name});
                newSC->setData(0,timelineSC,i);
                ui->cmdTimelineTree->addTopLevelItem(newSC);
            }
            break;
        }
    }
    simFile.close();

    QFile dsmFile(filePath);
    if(!dsmFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", dsmFile.errorString());
    QTextStream in(&dsmFile);


    foreach (dsmSectionTypes type, searchOrd) {
        in.seek(0);
        while(!in.atEnd()) {
            QString line = in.readLine();
            QRegularExpressionMatch match = entryItemRegEx(type).match(line);
            if (!match.captured(1).isEmpty()) {
                new_entry_item(type,match.captured(3),match.captured(1).toInt(),match.captured(2));
            }
        }

    }

    dsmFile.close();
}

void DSM_Menu::apply_data() {

}

void DSM_Menu::write_data() {
    QFile::remove(filePath);
    QFile file(filePath);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < dsmUpdate.size(); i++)
            in << dsmUpdate.at(i);
    }
    dsmUpdate.clear();
    file.close();
}

/* Edit this function to change the headers in Inp_DSM.txt */
QStringList DSM_Menu::secDescription(const dsmSectionTypes type) {
    QStringList descrip;
    descrip.clear();

    switch (type) {
    case dsmSectionTypes::COMMANDS:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                              Spacecraft Commands");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)");
        descrip.append("# Col: 2   ->  Spacecraft ID Number: \"SC[#]\"");
        descrip.append("# Col: 3   ->  Command Sequence Flag: \"FswMode\" or \"CmdTime\"");
        descrip.append("# Col: 4   ->  Command Sequence Parameter: FSW Mode ID or Command Execution Time");
        descrip.append("# Col: 5   ->  Translation Control Method_Mode (\"PASSIVE\" for none, \"NO_CHANGE\" for no change)");
        descrip.append("# Col: 6   ->  Attitude Control Method_Mode (\"PASSIVE\" for none)");
        descrip.append("# Col: 7   ->  Optional Actuator Command_Mode");
        break;
    case dsmSectionTypes::TRANSLATION:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                             Translation Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"TranslationCmd_[#]\"");
        descrip.append("# Col: 2   ->  Position 1: X (meters)");
        descrip.append("# Col: 3   ->  Position 2: Y (meters)");
        descrip.append("# Col: 4   ->  Position 3: Z (meters)");
        descrip.append("# Col: 5   ->  Ref Origin (ctrl wrt): Orbit Point \"OP\" or SC ID Number (\"SC[#]\")");
        descrip.append("# Col: 6   ->  Ref Frame Directions: \"N\", \"F\", \"L\", or SC ID Number (\"SC[#]\")");
        descrip.append("# Col: 7   ->  Controller Mode");
        descrip.append("# Col: 8   ->  Actuator Mode");
        break;
    case dsmSectionTypes::PRIMARY_VEC:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                              Attitude Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#------------------------------Primary Axis Cmd---------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"AttitudeCmd_PV[#]\"");
        descrip.append("# Col: 2   ->  Primary Target Type: BODY, SC, VEC");
        descrip.append("# Col: 3   ->  Primary Axis Direction 1");
        descrip.append("# Col: 4   ->  Primary Axis Direction 2");
        descrip.append("# Col: 5   ->  Primary Axis Direction 3");
        descrip.append("# Col: 6   ->  Primary Target: Specify Body, SC[#] or Vec");
        descrip.append("# Col: 7   ->  Controller Mode");
        descrip.append("# Col: 8   ->  Actuator Mode");
        descrip.append("# If Col: 2 -> VEC, then Col: 6 is the Ref. Frame of the pointing vector: \"N\", \"F\", \"L\", \"B\"");
        descrip.append("#                        Col: 7-9 are the pointing vec for Primary axis");
        descrip.append("#                        Col: 10-11 are Controller and Actuator Sets");
        break;
    case dsmSectionTypes::SECONDARY_VEC:
        descrip.append("#------------------------------Secondary Axis Cmd-------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"AttitudeCmd_SV[0]\"");
        descrip.append("# Col: 2   ->  Secondary Target Type: BODY, SC, VEC");
        descrip.append("# Col: 3   ->  Secondary Axis Direction 1");
        descrip.append("# Col: 4   ->  Secondary Axis Direction 2");
        descrip.append("# Col: 5   ->  Secondary Axis Direction 3");
        descrip.append("# Col: 6   ->  Secondary Target: Specify Body, SC[#] or Vec");
        descrip.append("# If Col: 2 -> VEC, then Col: 6 is the Ref. Frame of the pointing vector: \"N\", \"F\", \"L\"");
        descrip.append("#                        Col: 7-9 are the pointing vec for Secondary axis");
        break;
    case dsmSectionTypes::QUATERION:
        descrip.append("#------------------------Quaternion Attitude Cmd--------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"QuaternionCmd_[#]\"");
        descrip.append("# Col: 2   ->  Quaternion Component, q0");
        descrip.append("# Col: 3   ->  Quaternion Component, q1");
        descrip.append("# Col: 4   ->  Quaternion Component, q2");
        descrip.append("# Col: 5   ->  Quaternion Component, q3");
        descrip.append("# Col: 6   ->  Reference Frame of Quaternion");
        descrip.append("# Col: 7   ->  Controller Mode");
        descrip.append("# Col: 8   ->  Actuator Mode");
        break;
    case dsmSectionTypes::MIRROR:
        descrip.append("#----------------------------Mirror Attitude Cmd--------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"MirrorCmd_[#]\"");
        descrip.append("# Col: 2   ->  Spacecraft to mirror the attitude of: \"SC[#]\"");
        descrip.append("# Col: 3   ->  Controller Mode");
        descrip.append("# Col: 4   ->  Actuator Mode");
        break;
    case dsmSectionTypes::DETUMBLE:
        descrip.append("#---------------------------Detumble Attitude Cmd-------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"DetumbleCmd_[#]\"");
        descrip.append("# Col: 2   ->  Controller Mode");
        descrip.append("# Col: 3   ->  Actuator Mode");
        break;
    case dsmSectionTypes::WHLHMANAGEMENT:
        descrip.append("#---------------------------Wheel Momentum Management Cmd-------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"WhlHManagementCmd_[#]\"");
        descrip.append("# Col: 2   ->  Momentum Dumping [ON/OFF]");
        descrip.append("# Col: 3   ->  Minimum H_norm [Nms] (0 to always dump, *large* to never dump)");
        descrip.append("# Col: 4   ->  Maximum H_norm [Nms] (0 to always dump, *large* to never dump)");
        descrip.append("# Col: 5   ->  Controller Mode");
        descrip.append("# Col: 6   ->  Actuator Mode");
        break;
    case dsmSectionTypes::ACTUATOR_CMD:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                             Actuator Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"ActuatorCmd[#]\"");
        descrip.append("# Col: 2   ->  Number of Commands Declared");
        descrip.append("# Col: 3-N ->  Actuator[Number]_[Duty Cycle], e.g. WHL[0]_[100]");
        break;
    case dsmSectionTypes::CONTROLLERS:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                              Controllers");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"Controller_[#]\"");
        descrip.append("# Col: 2   ->  Controller Type: \"PID_CNTRL\", \"LYA_ATT_CNTRL\", \"LYA_2BODY_CNTRL\", \"H_DUMP_CNTRL\"");
        descrip.append("# Col: 3   ->  Set of Control Gains");
        descrip.append("# Col: 4   ->  Set of Control Limits");
        break;
    case dsmSectionTypes::ACTUATORS:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                             Control Actuators");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Cmd Interp ID Flag: \"Actuators_[#]\"");
        descrip.append("# Col: 2   ->  Actuators: \"WHL_[#]\", \"MTB\", \"THR\", \"Ideal\"");
        descrip.append("# Col: 3   ->  Actuator Mode: \"Translation\", \"Attitude\", \"Full\"");
        break;
    case dsmSectionTypes::GAINS:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                   Translational / Attitude Control Parameters");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Cmd Interp ID Flag: \"Gains_[#]\"");
        descrip.append("# Col: 2   ->  Gain Mode: \"PID\", \"PID_WN\", \"MomentumDump\", \"FC_LYA\"");
        descrip.append("# Col: 3   ->  Proportional Gain Direction 1, Kp");
        descrip.append("# Col: 4   ->  Proportional Gain Direction 2, Kp");
        descrip.append("# Col: 5   ->  Proportional Gain Direction 3, Kp");
        descrip.append("# Col: 6   ->  Rate Gain Direction 1, Kr");
        descrip.append("# Col: 7   ->  Rate Gain Direction 2, Kr");
        descrip.append("# Col: 8   ->  Rate Gain Direction 3, Kr");
        descrip.append("# Col: 9   ->  Integral Gain Direction 1, Ki");
        descrip.append("# Col: 10  ->  Integral Gain Direction 2, Ki");
        descrip.append("# Col: 11  ->  Integral Gain Direction 3, Ki");
        break;
    case dsmSectionTypes::LIMITS:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                   Translational / Attitude Control Limits");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Cmd Interp ID Flag: \"Limits_[#]\"");
        descrip.append("# Col: 2   ->  Max. Acceleration Direction 1, frc_max/trq_max [N, Nm]");
        descrip.append("# Col: 3   ->  Max. Acceleration Direction 2, frc_max/trq_max [N, Nm]");
        descrip.append("# Col: 4   ->  Max. Acceleration Direction 3, frc_max/trq_max [N, Nm]");
        descrip.append("# Col: 5   ->  Max. Velocity Direction 1, vel_max/w_max [m/s, deg/s]");
        descrip.append("# Col: 6   ->  Max. Velocity Direction 2, vel_max/w_max [m/s, deg/s]");
        descrip.append("# Col: 7   ->  Max. Velocity Direction 3, vel_max/w_max [m/s, deg/s]");
        break;
    case dsmSectionTypes::MANEUVER:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                              Maneuver Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Cmd Interp ID Flag: \"ManeuverCmd_[#]\"");
        descrip.append("# Col: 2   ->  Delta-V in x direction [m/s]");
        descrip.append("# Col: 3   ->  Delta-V in y direction [m/s]");
        descrip.append("# Col: 4   ->  Delta-V in z direction [m/s]");
        descrip.append("# Col: 5   ->  Maneuver Frame [%s]: \"N\", \"B\"");
        descrip.append("# Col: 6   ->  Approximation Type [%s]: \"CONSTANT\", \"SMOOTHED\". Note that");
        descrip.append("#    \"SMOOTHED\" Cmd must start at half of burn duration before the equivalent DeltaV");
        descrip.append("#    occurs.");
        descrip.append("# Col: 7   ->  Delta-V burn duration [s]");
        descrip.append("# Col: 8   ->  Set of Control Limits");
        descrip.append("# Col: 9   ->  Actuator Mode");
        break;
//    case dsmSectionTypes::MOMENTUM_DUMP:
//        descrip.append("#-------------------------------------------------------------------------------");
//        descrip.append("#                             Momentum Dump");
//        descrip.append("#-------------------------------------------------------------------------------");
//        descrip.append("# Col: 1   ->  Cmd Interp ID Flag: \"MomentumDump_[#]\"");
//        descrip.append("# Col: 2   ->  Minimum H_norm [Nms] (0 to always dump, *large* to never dump)");
//        descrip.append("# Col: 3   ->  Maximum H_norm [Nms] (0 to always dump, *large* to never dump)");
//        descrip.append("# Col: 4   ->  Set of Control Gains");
//        descrip.append("# Col: 5   ->  Actuator Mode for Dumping");
//        break;
    }
    return descrip;
}

void DSM_Menu::new_entry_item(const dsmSectionTypes type, QString label, const int itemNum, const QString itemData) {
    QStringList dataSplit = itemData.split(QRegExp("\\s"),Qt::SkipEmptyParts);
    static QRegularExpression rxGains("Gains_\\[([0-9]+)]");
    static QRegularExpression  rxLims("Limits_\\[([0-9]+)]");
    static QRegularExpression  rxActs("Actuators_\\[([0-9]+)]");
    static QRegularExpression  rxCmdParse("\\s*CmdTime\\s*([0-9.]*)\\s*NUM_CMD\\[[0-9]+]\\s*(.*)");
    QRegularExpressionMatch match;

    if (label.isEmpty())
        label = entryItemFormat(type).arg(itemNum);

    QTreeWidgetItem *newTreeItem;
    QListWidgetItem *newListItem;

    QList<QListWidgetItem*> listItems;
    QTreeWidgetItem *treeItem, *treeParentItem;
    int searchNum;

    switch (type) {
    case dsmSectionTypes::COMMANDS:
        treeItem = ui->cmdTimelineTree->topLevelItem(0);
        for (int i=0; i<ui->cmdTimelineTree->topLevelItemCount(); i++) {
            treeItem = ui->cmdTimelineTree->topLevelItem(i);
            if (treeItem->data(tlCols::tlColSCTime,timelineSC)==itemNum) {
                break;
            }
        }
        newTreeItem = new QTreeWidgetItem(treeItem);
        match = rxCmdParse.match(itemData);
        dataSplit = match.captured(2).split(QRegExp("\\s"),Qt::SkipEmptyParts);
        newTreeItem->setText(tlCols::tlColSCTime,match.captured(1)+" [sec]");
        newTreeItem->setTextAlignment(tlCols::tlColSCTime,Qt::AlignRight);
        foreach (QString cmd, dataSplit) {
            int writeCol = -1;
            QString label = cmd;
            int cmdType = getCmdType(cmd);
            if (trnCmds.contains(cmdType)) {
                writeCol=1;
            }
            else if (attCmds.contains(cmdType)) {
                writeCol=2;
            }
            else if (cmdType==DSM_Menu::cmdAct) {
                writeCol=3;
            }
            if (cmdType!=DSM_Menu::cmdPsvTrn && cmdType!=DSM_Menu::cmdPsvAtt) {
                if (cmdType!=cmdTypes::cmdAtt) {
                    treeParentItem = entryCmdParents[cmdType];
                    QRegularExpressionMatch match = cmdRegEx(cmdType).match(cmd);
                    searchNum = match.captured(1).toInt();
                    for (int i=0; i<treeParentItem->childCount(); i++) {
                        treeItem = treeParentItem->child(i);
                        if (treeItem->data(cmdCols::cmdColLabel,cmdData::cmdNum)==searchNum) {
                            label = treeItem->text(cmdCols::cmdColLabel);
                            break;
                        }
                    }
                }
            }
            newTreeItem->setText(writeCol,label);
            treeItem->addChild(newTreeItem);
        }
        break;
    case dsmSectionTypes::TRANSLATION:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::PRIMARY_VEC:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData,dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::SECONDARY_VEC:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::QUATERION:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::MIRROR:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::DETUMBLE:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::WHLHMANAGEMENT:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::ACTUATOR_CMD:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
//        newTreeItem->setData(0,cmdData::cmdType,section2Cmd[type]);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.mid(1).join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::CONTROLLERS:
        newTreeItem = new QTreeWidgetItem(ui->ctrlConfigTree,{label});
        newTreeItem->setData(ctrlCols::ctrlColLabel,ctrlData::ctrlNum,itemNum);
        newTreeItem->setData(ctrlCols::ctrlColLabel,ctrlData::ctrlType,dataSplit[0]);
        newTreeItem->setText(ctrlCols::ctrlColType,ctrlTypes[dataSplit[0]]);

        searchNum = rxGains.match(itemData).captured(1).toInt();
        listItems = ui->gainList->findItems("*",Qt::MatchWildcard);
        foreach (QListWidgetItem *item,listItems) {
            if (item->data(gainsData::gainsNum).toInt()==searchNum) {
                newTreeItem->setText(ctrlCols::ctrlColGains,item->text());
                break;
            }
        }

        searchNum = rxLims.match(itemData).captured(1).toInt();
        listItems = ui->limList->findItems("*",Qt::MatchWildcard);
        foreach (QListWidgetItem *item,listItems) {
            if (item->data(limData::limNum).toInt()==searchNum) {
                newTreeItem->setText(ctrlCols::ctrlColLims,item->text());
                break;
            }
        }
        ui->ctrlConfigTree->addTopLevelItem(newTreeItem);
        break;
    case dsmSectionTypes::ACTUATORS:
        newListItem = new QListWidgetItem(label);
        newListItem->setData(actData::actType,dataSplit[0]);
        newListItem->setData(actData::actNum,itemNum);
        ui->actList->addItem(newListItem);
        break;
    case dsmSectionTypes::GAINS:
        newListItem = new QListWidgetItem(label);
        newListItem->setData(gainsData::gainsType,dataSplit[0]);
        newListItem->setData(gainsData::gainsNum,itemNum);
        newListItem->setData(gainsData::gainsData,dataSplit.mid(1).join("  "));
        ui->gainList->addItem(newListItem);
        break;
    case dsmSectionTypes::LIMITS:
        newListItem = new QListWidgetItem(label);
        newListItem->setData(limData::limNum,itemNum);
        newListItem->setData(limData::limData,itemData);
        ui->limList->addItem(newListItem);
        break;
    case dsmSectionTypes::MANEUVER:
        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});

        match = rxActs.match(itemData);
        searchNum = match.captured(1).toInt();
        listItems = ui->actList->findItems("*",Qt::MatchWildcard);
        foreach (QListWidgetItem *listItem, listItems) {
            if (listItem->data(actData::actNum).toInt()==searchNum) {
                dataSplit.takeAt(dataSplit.indexOf(match.captured(0)));
                newTreeItem->setText(cmdCols::cmdColAct,listItem->text());
                break;
            }
        }
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setText(cmdCols::cmdColInd,QVariant(itemNum).toString());
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join("  "));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    default:
        break;
    }
}

void DSM_Menu::set_command_ctrl_act(QTreeWidgetItem *item, const QString dataString, QStringList *dataList) {
    static QRegularExpression rxCtrls("Controller_\\[([0-9]+)]");
    static QRegularExpression   rxActs("Actuators_\\[([0-9]+)]");
    QRegularExpressionMatch match;
    int searchNum;

    match = rxCtrls.match(dataString);
    searchNum = match.captured(1).toInt();
    QList<QTreeWidgetItem*> treeItems = ui->ctrlConfigTree->findItems("*",Qt::MatchWildcard);
    foreach (QTreeWidgetItem *treeItem, treeItems) {
        if (treeItem->data(ctrlCols::ctrlColLabel,ctrlData::ctrlNum)==searchNum) {
            dataList->takeAt(dataList->indexOf(match.captured(0)));
            item->setText(cmdCols::cmdColCtl,treeItem->text(0));
            break;
        }
    }

    match = rxActs.match(dataString);
    searchNum = match.captured(1).toInt();
    QList<QListWidgetItem*> listItems = ui->actList->findItems("*",Qt::MatchWildcard);
    foreach (QListWidgetItem *listItem, listItems) {
        if (listItem->data(actData::actNum).toInt()==searchNum) {
            dataList->takeAt(dataList->indexOf(match.captured(0)));
            item->setText(cmdCols::cmdColAct,listItem->text());
            break;
        }
    }

}

QString DSM_Menu::entryItemName(const dsmSectionTypes type) {
    switch (type) {
    case dsmSectionTypes::COMMANDS: return "DSM_Cmd  SC";
    case dsmSectionTypes::TRANSLATION: return "TranslationCmd_";
    case dsmSectionTypes::PRIMARY_VEC: return "AttitudeCmd_PV";
    case dsmSectionTypes::SECONDARY_VEC: return "AttitudeCmd_SV";
    case dsmSectionTypes::QUATERION: return "QuaternionCmd_";
    case dsmSectionTypes::MIRROR: return "MirrorCmd_";
    case dsmSectionTypes::DETUMBLE: return "DetumbleCmd_";
    case dsmSectionTypes::WHLHMANAGEMENT: return "WhlHManagementCmd_";
    case dsmSectionTypes::ACTUATOR_CMD: return "ActuatorCmd_";
    case dsmSectionTypes::CONTROLLERS: return "Controller_";
    case dsmSectionTypes::ACTUATORS: return "Actuators_";
    case dsmSectionTypes::GAINS: return "Gains_";
    case dsmSectionTypes::LIMITS: return "Limits_";
    case dsmSectionTypes::MANEUVER: return "ManeuverCmd_";
    default: return "";
//    case dsmSectionTypes::MOMENTUM_DUMP: return "MomentumDump_";
    }
}

int DSM_Menu::getCmdType(QString cmdString) {

    if (cmdString.contains("PASSIVE_TRN")) return cmdPsvTrn;
    else if (cmdString.contains("PASSIVE_ATT")) return cmdPsvAtt;
    else if (cmdString.contains("TranslationCmd_")) return cmdTrn;
    else if (cmdString.contains("AttitudeCmd_PV")){
        if (cmdRegEx(cmdAtt).match(cmdString).captured(2).isEmpty())
            return cmdPV;
        else
            return cmdAtt;
    }
    else if (cmdString.contains("AttitudeCmd_SV")) return cmdSV;
    else if (cmdString.contains("QuaternionCmd_")) return cmdQuat;
    else if (cmdString.contains("MirrorCmd_")) return cmdMirror;
    else if (cmdString.contains("DetumbleCmd_")) return cmdDetumble;
    else if (cmdString.contains("WhlHManagementCmd")) return cmdWhlHManage;
    else if (cmdString.contains("ActuatorCmd_")) return cmdAct;
    else if (cmdString.contains("ManeuverCmd_")) return cmdManeuver;
    else return -1;
}

QRegularExpression DSM_Menu::entryItemRegEx(const dsmSectionTypes type) {
    static QString format = "^\\s*%1\\[([0-9]+)]\\s*([^#]*)\\s*(?(?=#+)#+\\s*(.*))$";

    static QRegularExpression rxCmd(format.arg(entryItemName(dsmSectionTypes::COMMANDS).split(QRegExp("\\s"),Qt::SkipEmptyParts).join("\\s*")));
    static QRegularExpression rxTrn(format.arg(entryItemName(dsmSectionTypes::TRANSLATION)));
    static QRegularExpression rxPV(format.arg(entryItemName(dsmSectionTypes::PRIMARY_VEC)));
    static QRegularExpression rxSV(format.arg(entryItemName(dsmSectionTypes::SECONDARY_VEC)));
    static QRegularExpression rxQuat(format.arg(entryItemName(dsmSectionTypes::QUATERION)));
    static QRegularExpression rxMirror(format.arg(entryItemName(dsmSectionTypes::MIRROR)));
    static QRegularExpression rxDetumble(format.arg(entryItemName(dsmSectionTypes::DETUMBLE)));
    static QRegularExpression rxWhlHManage(format.arg(entryItemName(dsmSectionTypes::WHLHMANAGEMENT)));
    static QRegularExpression rxActCmd(format.arg(entryItemName(dsmSectionTypes::ACTUATOR_CMD)));
    static QRegularExpression rxCtrl(format.arg(entryItemName(dsmSectionTypes::CONTROLLERS)));
    static QRegularExpression rxAct(format.arg(entryItemName(dsmSectionTypes::ACTUATORS)));
    static QRegularExpression rxGains(format.arg(entryItemName(dsmSectionTypes::GAINS)));
    static QRegularExpression rxLims(format.arg(entryItemName(dsmSectionTypes::LIMITS)));
    static QRegularExpression rxManeuver(format.arg(entryItemName(dsmSectionTypes::MANEUVER)));

    switch (type) {
    case dsmSectionTypes::COMMANDS: return rxCmd;
    case dsmSectionTypes::TRANSLATION: return rxTrn;
    case dsmSectionTypes::PRIMARY_VEC: return rxPV;
    case dsmSectionTypes::SECONDARY_VEC: return rxSV;
    case dsmSectionTypes::QUATERION: return rxQuat;
    case dsmSectionTypes::MIRROR: return rxMirror;
    case dsmSectionTypes::DETUMBLE: return rxDetumble;
    case dsmSectionTypes::WHLHMANAGEMENT: return rxWhlHManage;
    case dsmSectionTypes::ACTUATOR_CMD: return rxActCmd;
    case dsmSectionTypes::CONTROLLERS: return rxCtrl;
    case dsmSectionTypes::ACTUATORS: return rxAct;
    case dsmSectionTypes::GAINS: return rxGains;
    case dsmSectionTypes::LIMITS: return rxLims;
    case dsmSectionTypes::MANEUVER: return rxManeuver;
    default: return QRegularExpression(format.arg(entryItemName(type)));
    }


}

QRegularExpression DSM_Menu::cmdRegEx(const int cmdType) {
    static QString format = "%1\\[([0-9]+)]";

    static QRegularExpression rxTrn(format.arg(entryItemName(dsmSectionTypes::TRANSLATION)));
    static QRegularExpression rxAtt(format.arg(entryItemName(dsmSectionTypes::PRIMARY_VEC))+format.arg("_SV"));
    static QRegularExpression rxPV(format.arg(entryItemName(dsmSectionTypes::PRIMARY_VEC)));
    static QRegularExpression rxSV(format.arg(entryItemName(dsmSectionTypes::SECONDARY_VEC)));
    static QRegularExpression rxQuat(format.arg(entryItemName(dsmSectionTypes::QUATERION)));
    static QRegularExpression rxMirror(format.arg(entryItemName(dsmSectionTypes::MIRROR)));
    static QRegularExpression rxDetumble(format.arg(entryItemName(dsmSectionTypes::DETUMBLE)));
    static QRegularExpression rxWhlHManage(format.arg(entryItemName(dsmSectionTypes::WHLHMANAGEMENT)));
    static QRegularExpression rxAct(format.arg(entryItemName(dsmSectionTypes::ACTUATOR_CMD)));
    static QRegularExpression rxManeuver(format.arg(entryItemName(dsmSectionTypes::MANEUVER)));

    switch (cmdType) {
    case cmdTrn: return rxTrn;
    case cmdAtt: return rxAtt;
    case cmdPV: return rxPV;
    case cmdSV: return rxSV;
    case cmdQuat: return rxQuat;
    case cmdMirror: return rxMirror;
    case cmdDetumble: return rxDetumble;
    case cmdWhlHManage: return rxWhlHManage;
    case cmdAct: return rxAct;
    case cmdManeuver: return rxManeuver;
    default: return QRegularExpression(format.arg(entryItemName(section2Cmd.key(cmdType))));
    }
}

QString DSM_Menu::entryItemFormat(const dsmSectionTypes type) {
    QString string = entryItemName(type)+"[%1]";
    return string;
}
















