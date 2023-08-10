#include "dsm_menu.h"
#include "qstringliteral.h"
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



    ui->cmdPvTgtType->clear();
    ui->cmdPvTgtType->addItems(dsm_gui_lib::sortStringList(cmdAttTgtTypes.values()));
    ui->cmdPvTgtAxisFrm->clear();
    ui->cmdPvTgtAxisFrm->addItems(dsm_gui_lib::sortStringList(cmdAttTgtFrm.values()));
    ui->cmdPvTgtWld->clear();
    ui->cmdPvTgtWld->addItems(dsm_gui_lib::sortStringList(worldInputs));

    ui->cmdSvTgtType->clear();
    ui->cmdSvTgtType->addItems(dsm_gui_lib::sortStringList(cmdAttTgtTypes.values()));
    ui->cmdSvTgtAxisFrm->clear();
    ui->cmdSvTgtAxisFrm->addItems(dsm_gui_lib::sortStringList(cmdAttTgtFrm.values()));
    ui->cmdSvTgtWld->clear();
    ui->cmdSvTgtWld->addItems(dsm_gui_lib::sortStringList(worldInputs));

    ui->cmdQuatFrm->clear();
    ui->cmdQuatFrm->addItems(dsm_gui_lib::sortStringList(cmdTrnFrmConst.values()));

    ui->cmdManFrm->clear();
    ui->cmdManFrm->addItems(dsm_gui_lib::sortStringList(cmdManFrm.values()));
    ui->cmdManType->clear();
    ui->cmdManType->addItems(dsm_gui_lib::sortStringList(cmdManTypes.values()));

    ui->cmdTimelineTree->setHeaderLabels(tlColNames.values());
    ui->cmdConfigTree->setHeaderLabels(cmdColNames.values());
    ui->ctrlConfigTree->setHeaderLabels(ctrlColNames.values());

    for (int i=0; i<ui->cmdConfigurator->count(); i++)
        ui->cmdConfigurator->setTabVisible(i,false);

    for (int i=0; i<ui->gainConfigurator->count(); i++)
        ui->gainConfigurator->setTabVisible(i,false);

    QTreeWidget *parent = ui->cmdConfigTree;
    QTreeWidgetItem *newItem;
    for (dsmSectionTypes type : section2CmdKeys) {
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

    ui->actType->clear();
    ui->actType->addItems(dsm_gui_lib::sortStringList(actTypes.values()));

    ui->gainType->clear();
    ui->gainType->addItems(dsm_gui_lib::sortStringList(gainsTypes.values()));

    ui->ctrlType->clear();
    ui->ctrlType->addItems(dsm_gui_lib::sortStringList(ctrlTypes.values()));

    connect(ui->cmdTrnX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdTrnY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdTrnZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdTrnOri, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdTrnFrm, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdTrnOriScBdyNum, &QSpinBox::textChanged, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdTrnFrmScBdyNum, &QSpinBox::textChanged, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdPvTgtType, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtSc, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtWld, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtScBdyNum, &QSpinBox::textChanged, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtAxisFrm, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdPvTgtZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdSvTgtType, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtSc, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtWld, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtScBdyNum, &QSpinBox::textChanged, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtAxisFrm, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdSvTgtZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdQv1, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdQv2, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdQv3, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdQs, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdQuatFrm, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdHManageEnabled, &QCheckBox::stateChanged, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdHManageMax, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdHManageMin, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdMirrorTgt, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdMirrorTgtBdyNum, &QSpinBox::textChanged, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdManX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManFrm, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManType, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManLimits, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManTime, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
}

void DSM_Menu::receive_dsmpath(QString path) {
    inoutPath = path;
    filePath = inoutPath+"Inp_DSM.txt";

    receive_data();
    apply_data();
}

void DSM_Menu::receive_data() {
    static QRegularExpression rxSC("SC_(.*).txt");

    dsmData.clear();
    dsmString.clear();
    dsmUpdate.clear();
    scNames.clear();
    ui->cmdSC->clear();
    ui->cmdTrnOri->clear();
    ui->cmdTrnOri->addItems(dsm_gui_lib::sortStringList(cmdTrnOriConst.values()));
    ui->cmdTrnFrm->clear();
    ui->cmdTrnFrm->addItems(dsm_gui_lib::sortStringList(cmdTrnFrmConst.values()));
    ui->cmdPvTgtSc->clear();
    ui->cmdSvTgtSc->clear();
    ui->cmdController->clear();
    ui->cmdActuator->clear();
    ui->cmdLabel->clear();
    ui->ctrlGains->clear();
    ui->ctrlLims->clear();
    ui->cmdManLimits->clear();

    QFile simFile(inoutPath + "Inp_Sim.txt");
    if(!simFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", simFile.errorString());
    QTextStream simIn(&simFile);

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
            }
            break;
        }
    }
    simFile.close();
    ui->cmdSC->addItems(dsm_gui_lib::sortStringList(scNames));
    ui->cmdTrnOri->addItems(dsm_gui_lib::sortStringList(scNames));
    ui->cmdTrnFrm->addItems(dsm_gui_lib::sortStringList(scNames));
    ui->cmdPvTgtSc->addItems(dsm_gui_lib::sortStringList(scNames));
    ui->cmdSvTgtSc->addItems(dsm_gui_lib::sortStringList(scNames));


    QFile dsmFile(filePath);
    if(!dsmFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", dsmFile.errorString());
    QTextStream in(&dsmFile);


    trnCmdsHash = trnCmdsHashConst;
    attCmdsHash = attCmdsHashConst;
    actCmdsHash = actCmdsHashConst;
    attSVCmdsHash.clear();
    ctlsHash.clear();
    gainsHash.clear();
    limsHash.clear();
    actsHash.clear();

    for (int key : cmdValidCtrls.keys())
        cmdValidCtrls[key].clear();
    for (int key : cmdValidActs.keys())
        cmdValidActs[key].clear();

    ui->cmdTrnLabel->clear();
    ui->cmdTrnLabel->addItems(dsm_gui_lib::sortStringList(trnCmdsHashConst.keys()));
    ui->cmdAttLabel->clear();
    ui->cmdAttLabel->addItems(dsm_gui_lib::sortStringList(attCmdsHashConst.keys()));
    ui->cmdActLabel->clear();
    ui->cmdActLabel->addItems(dsm_gui_lib::sortStringList(actCmdsHashConst.keys()));

    for (dsmSectionTypes type : searchOrd) {
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

    QStringList newKeys = trnCmdsHash.keys();
    for (QString &constKey : trnCmdsHashConst.keys())
        newKeys.takeAt(newKeys.indexOf(constKey));
    ui->cmdTrnLabel->addItems(dsm_gui_lib::sortStringList(newKeys));

    newKeys = attCmdsHash.keys();
    for (QString &constKey : attCmdsHashConst.keys())
        newKeys.takeAt(newKeys.indexOf(constKey));
    ui->cmdAttLabel->addItems(dsm_gui_lib::sortStringList(newKeys));

    newKeys = trnCmdsHash.keys();
    for (QString &constKey : actCmdsHashConst.keys())
        newKeys.takeAt(newKeys.indexOf(constKey));
    ui->cmdActLabel->addItems(dsm_gui_lib::sortStringList(newKeys));

    ui->cmdManLimits->clear();
    ui->cmdManLimits->addItems(dsm_gui_lib::sortStringList(limsHash.keys()));
    ui->ctrlLims->clear();
    ui->ctrlLims->addItems(dsm_gui_lib::sortStringList(limsHash.keys()));

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
    static QRegularExpression  rxCmdParse("\\s*CmdTime\\s*([0-9.]+)\\s*NUM_CMD\\[[0-9]+]\\s*(.*)");
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

        newTreeItem = new QTreeWidgetItem(ui->cmdTimelineTree);
        match = rxCmdParse.match(itemData);
        dataSplit = match.captured(2).split(QRegExp("\\s"),Qt::SkipEmptyParts);
        newTreeItem->setText(tlCols::tlColSC,scNames[itemNum]);
        newTreeItem->setData(tlCols::tlColTime,Qt::DisplayRole,match.captured(1).toDouble());
        newTreeItem->setTextAlignment(tlCols::tlColTime,Qt::AlignRight);
        for (const QString &cmd : qAsConst(dataSplit)) {
            int writeCol = -1;
            QString label;
            int cmdType = getCmdType(cmd);

            if (trnCmds.contains(cmdType))
                writeCol=tlColTrn;
            else if (attCmds.contains(cmdType))
                writeCol=tlColAtt;
            else if (cmdType==DSM_Menu::cmdAct)
                writeCol=tlColAct;

            if (cmdType!=DSM_Menu::cmdPsvTrn && cmdType!=DSM_Menu::cmdPsvAtt) {
                label.clear();
                QRegularExpressionMatch match = cmdRegEx(cmdType).match(cmd);

                if (cmdType!=cmdTypes::cmdAtt) {
                    treeParentItem = entryCmdParents[cmdType];
                    searchNum = match.captured(1).toInt();
                    for (int i=0; i<treeParentItem->childCount(); i++) {
                        treeItem = treeParentItem->child(i);
                        if (treeItem->data(cmdCols::cmdColLabel,cmdData::cmdNum)==searchNum) {
                            label = treeItem->text(cmdCols::cmdColLabel);
                            break;
                        }
                    }
                }
                else {
                    for (int subCmd : {cmdPV, cmdSV}) {
                        treeParentItem = entryCmdParents[subCmd];
                        if (subCmd == cmdPV)
                            searchNum = match.captured(1).toInt();
                        else
                            searchNum = match.captured(2).toInt();

                        for (int i=0; i<treeParentItem->childCount(); i++) {
                            treeItem = treeParentItem->child(i);
                            if (treeItem->data(cmdCols::cmdColLabel,cmdData::cmdNum)==searchNum) {
                                label.append(treeItem->text(cmdCols::cmdColLabel));
                                break;
                            }
                        }
                        if (subCmd == cmdPV)
                            label.append(cmdDelimiter+cmdDataSpacer);
                    }

                }
            }
            newTreeItem->setText(writeCol,label);
        }
        ui->cmdTimelineTree->addTopLevelItem(newTreeItem);
        break;
    case dsmSectionTypes::TRANSLATION:
        trnCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::PRIMARY_VEC:
        attCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData,dataSplit.join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::SECONDARY_VEC:
        attSVCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::QUATERION:
        attCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::MIRROR:
        attCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::DETUMBLE:
        attCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::WHLHMANAGEMENT:
        attCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));
        ui->cmdAttLabel->addItem(label);

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        set_command_ctrl_act(newTreeItem, itemData, &dataSplit);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::ACTUATOR_CMD:
        actCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
//        newTreeItem->setData(0,cmdData::cmdType,section2Cmd[type]);
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.mid(1).join(cmdDataSpacer));

        entryCmdParents[section2Cmd[type]]->addChild(newTreeItem);
        break;
    case dsmSectionTypes::CONTROLLERS:
        ctlsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newTreeItem = new QTreeWidgetItem(ui->ctrlConfigTree,{label});
        newTreeItem->setData(ctrlCols::ctrlColLabel,ctrlData::ctrlNum,itemNum);
        newTreeItem->setData(ctrlCols::ctrlColLabel,ctrlData::ctrlType,dataSplit[0]);
        newTreeItem->setText(ctrlCols::ctrlColType,ctrlTypes[dataSplit[0]]);

        for (int cmdType : ctrlValidCmds[dataSplit[0]])
            cmdValidCtrls[cmdType].append(label);

        searchNum = rxGains.match(itemData).captured(1).toInt();
        listItems = ui->gainList->findItems("*",Qt::MatchWildcard);
        for (QListWidgetItem *item : qAsConst(listItems)) {
            if (item->data(gainsData::gainsNum).toInt()==searchNum) {
                newTreeItem->setText(ctrlCols::ctrlColGains,item->text());
                break;
            }
        }

        searchNum = rxLims.match(itemData).captured(1).toInt();
        listItems = ui->limList->findItems("*",Qt::MatchWildcard);
        for (QListWidgetItem *item : qAsConst(listItems)) {
            if (item->data(limData::limNum).toInt()==searchNum) {
                newTreeItem->setText(ctrlCols::ctrlColLims,item->text());
                break;
            }
        }
        ui->ctrlConfigTree->addTopLevelItem(newTreeItem);
        break;
    case dsmSectionTypes::ACTUATORS:
        actsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newListItem = new QListWidgetItem(label);
        newListItem->setData(actData::actType,dataSplit[0]);
        newListItem->setData(actData::actNum,itemNum);

        for (int cmdType : actValidCmds[dataSplit[0]])
            cmdValidActs[cmdType].append(label);

        ui->actList->addItem(newListItem);
        break;
    case dsmSectionTypes::GAINS:
        gainsHash.insert(label, entryItemFormat(type).arg(itemNum));

        newListItem = new QListWidgetItem(label);
        newListItem->setData(gainsData::gainsType,dataSplit[0]);
        newListItem->setData(gainsData::gainsNum,itemNum);
        newListItem->setData(gainsData::gainsData,dataSplit.mid(1).join(cmdDataSpacer));
        ui->gainList->addItem(newListItem);
        break;
    case dsmSectionTypes::LIMITS:
        limsHash.insert(label, entryItemFormat(type).arg(itemNum));
        ui->ctrlLims->addItem(label);
        ui->cmdManLimits->addItem(label);

        newListItem = new QListWidgetItem(label);
        newListItem->setData(limData::limNum,itemNum);
        newListItem->setData(limData::limData,itemData);
        ui->limList->addItem(newListItem);
        break;
    case dsmSectionTypes::MANEUVER:
        trnCmdsHash.insert(label, entryItemFormat(type).arg(itemNum));
        ui->cmdTrnLabel->addItem(label);

        newTreeItem = new QTreeWidgetItem(entryCmdParents[section2Cmd[type]],{label});
        match = rxActs.match(itemData);
        searchNum = match.captured(1).toInt();
        listItems = ui->actList->findItems("*",Qt::MatchWildcard);
        for (QListWidgetItem *listItem : qAsConst(listItems)) {
            if (listItem->data(actData::actNum).toInt()==searchNum) {
                dataSplit.takeAt(dataSplit.indexOf(match.captured(0)));
                newTreeItem->setText(cmdCols::cmdColAct,listItem->text());
                break;
            }
        }
        newTreeItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,itemNum);
        newTreeItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,itemNum);
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

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

void DSM_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

void DSM_Menu::on_cmdTrnOri_currentTextChanged(const QString &arg1) {
    if (cmdTrnOriConst.values().contains(arg1)){
        ui->cmdTrnOriScBdyLabel->setEnabled(false);
        ui->cmdTrnOriScBdyNum->setEnabled(false);
    }
    else {
        ui->cmdTrnOriScBdyLabel->setEnabled(true);
        ui->cmdTrnOriScBdyNum->setEnabled(true);
    }
    ui->cmdTrnOriScBdyNum->setValue(0);
    return;
}

void DSM_Menu::on_cmdTrnFrm_currentTextChanged(const QString &arg1) {
    if (cmdTrnFrmConst.values().contains(arg1)){
        ui->cmdTrnFrmScBdyLabel->setEnabled(false);
        ui->cmdTrnFrmScBdyNum->setEnabled(false);
    }
    else {
        ui->cmdTrnFrmScBdyLabel->setEnabled(true);
        ui->cmdTrnFrmScBdyNum->setEnabled(true);
    }
    ui->cmdTrnFrmScBdyNum->setValue(0);
    return;

}

void DSM_Menu::on_cmdPvTgtType_currentTextChanged(const QString &arg1) {
    ui->cmdPvVecData->setVisible(arg1.compare("Vector")==0);
    ui->cmdPvScData->setVisible(arg1.compare("Spacecraft")==0);
    ui->cmdPvWldData->setVisible(arg1.compare("World")==0);
}

void DSM_Menu::on_cmdSvTgtType_currentTextChanged(const QString &arg1) {
    ui->cmdSvVecData->setVisible(arg1.compare("Vector")==0);
    ui->cmdSvScData->setVisible(arg1.compare("Spacecraft")==0);
    ui->cmdSvWldData->setVisible(arg1.compare("World")==0);
}

void DSM_Menu::cmd_data_changed() {
    QTreeWidgetItem *curItem = ui->cmdConfigTree->currentItem();
    if (curItem->parent()==NULL)
        return;

    QStringList cmdData;
    QString tmpStr;
    QStringList checkList;
    double checkSum;

    switch (entryCmdParents.key(curItem->parent())) {
        case cmdTrn:
            cmdData.append(ui->cmdTrnX->text());
            cmdData.append(ui->cmdTrnY->text());
            cmdData.append(ui->cmdTrnZ->text());

            tmpStr = ui->cmdTrnOri->currentText();
            checkList = cmdTrnOriConst.values();
            if (checkList.contains(tmpStr))
                cmdData.append(cmdTrnOriConst.key(tmpStr,"OP"));
            else
                cmdData.append(scBdyFrmt.arg(scNames.indexOf(tmpStr)).arg(ui->cmdTrnOriScBdyNum->value()));

            tmpStr = ui->cmdTrnFrm->currentText();
            checkList = cmdTrnFrmConst.values();
            if (checkList.contains(tmpStr))
                cmdData.append(cmdTrnFrmConst.key(tmpStr,"N"));
            else
                cmdData.append(scBdyFrmt.arg(scNames.indexOf(tmpStr)).arg(ui->cmdTrnFrmScBdyNum->value()));
            break;
        case cmdPV:
            tmpStr = cmdAttTgtTypes.key(ui->cmdPvTgtType->currentText(),"VEC");
            cmdData.append(tmpStr);
            cmdData.append(ui->cmdPvX->text());
            cmdData.append(ui->cmdPvY->text());
            cmdData.append(ui->cmdPvZ->text());
            if (tmpStr.compare("VEC") == 0) {
                cmdData.append(cmdAttTgtFrm.key(ui->cmdPvTgtAxisFrm->currentText(),"N"));
                cmdData.append(ui->cmdPvTgtX->text());
                cmdData.append(ui->cmdPvTgtY->text());
                cmdData.append(ui->cmdPvTgtZ->text());
            }
            else if (tmpStr.compare("SC") == 0) {
                cmdData.append(scBdyFrmt.arg(scNames.indexOf(ui->cmdPvTgtSc->currentText())).arg(ui->cmdPvTgtScBdyNum->value()));
            }
            else if (tmpStr.compare("BODY") == 0) {
                cmdData.append(ui->cmdPvTgtWld->currentText());
            }
            break;
        case cmdSV:
            tmpStr = cmdAttTgtTypes.key(ui->cmdSvTgtType->currentText(),"VEC");
            cmdData.append(tmpStr);
            cmdData.append(ui->cmdSvX->text());
            cmdData.append(ui->cmdSvY->text());
            cmdData.append(ui->cmdSvZ->text());
            if (tmpStr.compare("VEC") == 0) {
                cmdData.append(cmdAttTgtFrm.key(ui->cmdSvTgtAxisFrm->currentText(),"N"));
                cmdData.append(ui->cmdSvTgtX->text());
                cmdData.append(ui->cmdSvTgtY->text());
                cmdData.append(ui->cmdSvTgtZ->text());
            }
            else if (tmpStr.compare("SC") == 0)
                cmdData.append(scBdyFrmt.arg(scNames.indexOf(ui->cmdSvTgtSc->currentText())).arg(ui->cmdSvTgtScBdyNum->value()));
            else if (tmpStr.compare("BODY") == 0)
                cmdData.append(ui->cmdSvTgtWld->currentText());
            break;
        case cmdQuat:
            cmdData.append(ui->cmdQv1->text());
            cmdData.append(ui->cmdQv2->text());
            cmdData.append(ui->cmdQv3->text());
            cmdData.append(ui->cmdQs->text());
            checkSum=0.0;
            for (int i=0;i<4;i++) checkSum += cmdData[i].toDouble();
            ui->cmdQuatNormalize->setEnabled(checkSum>std::numeric_limits<double>::epsilon());
            cmdData.append(cmdTrnFrmConst.key(ui->cmdQuatFrm->currentText(),"N"));
            break;
        case cmdMirror:
            cmdData.append(scBdyFrmt.arg(scNames.indexOf(ui->cmdMirrorTgt->currentText())).arg(ui->cmdMirrorTgtBdyNum->value()));
            break;
        case cmdDetumble:
            // Nothing here
            break;
        case cmdWhlHManage:
            cmdData.append( ui->cmdHManageEnabled->isChecked() ? "ON" : "OFF" );
            cmdData.append(ui->cmdHManageMin->text());
            cmdData.append(ui->cmdHManageMax->text());
            break;
        case cmdAct:
            cmdData.append("NUM_CMD["+QVariant(ui->cmdActList->count()).toString()+"]");
            cmdData.append(dsm_gui_lib::getTextFromList(ui->cmdActList));
            break;
        case cmdManeuver:
            cmdData.append(ui->cmdManX->text());
            cmdData.append(ui->cmdManY->text());
            cmdData.append(ui->cmdManZ->text());
            cmdData.append(cmdManFrm.key(ui->cmdManFrm->currentText(),"N"));
            cmdData.append(cmdManTypes.key(ui->cmdManType->currentText(),"SMOOTHED"));
            break;
        default:
            break;
    }

    curItem->setText(DSM_Menu::cmdColData, cmdData.join(cmdDataSpacer));
}

void DSM_Menu::on_cmdController_textActivated(const QString &arg1) {
    QTreeWidgetItem *curItem = ui->cmdConfigTree->currentItem();
    if (curItem->parent()==NULL)
        return;

    curItem->setText(DSM_Menu::cmdColCtl,arg1);
}

void DSM_Menu::on_cmdActuator_textActivated(const QString &arg1) {
    QTreeWidgetItem *curItem = ui->cmdConfigTree->currentItem();
    if (curItem->parent()==NULL)
        return;

    curItem->setText(DSM_Menu::cmdColAct,arg1);
}

void DSM_Menu::on_cmdLabel_textEdited(const QString &arg1) {
    QTreeWidgetItem *curItem = ui->cmdConfigTree->currentItem();
    if (curItem->parent()==NULL)
        return;

    int itemNum = curItem->data(cmdCols::cmdColInd,Qt::DisplayRole).toInt();
    int cmdType = entryCmdParents.key(curItem->parent());
    int checkCol;
    QString value = entryItemFormat(section2Cmd.key(cmdType)).arg(itemNum);
    QString oldKey;
    QHash <QString,QString> *checkHash;

    if (trnCmds.contains(cmdType)){
        checkHash=&trnCmdsHash;
        checkCol = tlCols::tlColTrn;
    }
    else if (attCmds.contains(cmdType)){
        checkHash=&attCmdsHash;
        checkCol = tlCols::tlColAtt;
    }
    else if (cmdType == cmdSV) {
        checkHash=&attSVCmdsHash;
        checkCol = tlCols::tlColAtt;
    }
    else {
        checkHash=&actCmdsHash;
        checkCol = tlCols::tlColAct;
    }

    oldKey = checkHash->key(value);
    if (checkHash->keys().contains(arg1))
        return;

    checkHash->take(oldKey);
    checkHash->insert(arg1,value);

    QList<QTreeWidgetItem*> changeItems = ui->cmdTimelineTree->findItems("*",Qt::MatchWildcard,checkCol);
    static QRegularExpression oldKeyRx;

    if (cmdType == cmdPV)
        oldKeyRx = QRegularExpression("^"+QRegularExpression::escape(oldKey+cmdDelimiter+cmdDataSpacer));
    else if (cmdType == cmdSV)
        oldKeyRx = QRegularExpression(QRegularExpression::escape(cmdDelimiter+cmdDataSpacer+oldKey)+"$");

    QString removeTxt;
    for (QTreeWidgetItem *item : qAsConst(changeItems)) {
        removeTxt = item->text(checkCol);
        if (cmdType == cmdPV) {
            removeTxt.replace(oldKeyRx,arg1+cmdDelimiter+cmdDataSpacer);
            item->setText(checkCol,removeTxt);
        }
        else if (cmdType == cmdSV) {
            removeTxt.replace(oldKeyRx,cmdDelimiter+cmdDataSpacer+arg1);
            item->setText(checkCol,removeTxt);
        }
        else if (removeTxt.compare(oldKey)==0) {
            removeTxt = arg1;
            item->setText(checkCol,removeTxt);
        }
    }
    curItem->setText(DSM_Menu::cmdColLabel,arg1);
}

void DSM_Menu::on_cmdConfigTree_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem*) {
    if (item->parent()==NULL) {
        ui->cmdConfigurator->setTabVisible(ui->cmdConfigurator->currentIndex(),false);
        ui->cmdController->setEnabled(false);
        ui->cmdControllerLabel->setEnabled(false);
        ui->cmdActuator->setEnabled(false);
        ui->cmdActuatorLabel->setEnabled(false);
        ui->cmdLabel->setEnabled(false);
        ui->cmdLabelLabel->setEnabled(false);
        ui->cmdLabel->clear();
        return;
    }
    ui->cmdController->setEnabled(true);
    ui->cmdControllerLabel->setEnabled(true);
    ui->cmdActuator->setEnabled(true);
    ui->cmdActuatorLabel->setEnabled(true);
    ui->cmdLabel->setEnabled(true);
    ui->cmdLabelLabel->setEnabled(true);

    setQComboBox(ui->cmdController,item->text(DSM_Menu::cmdColCtl));
    setQComboBox(ui->cmdActuator,item->text(DSM_Menu::cmdColAct));
    ui->cmdLabel->setText(item->text(DSM_Menu::cmdColLabel));

    int cmdType = entryCmdParents.key(item->parent());
    QString cmdData = item->text(cmdColData);
    QStringList cmdDataSplit = cmdData.split(QRegExp("\\s"),Qt::SkipEmptyParts);
    static QRegularExpression rxScBdy("SC\\[([0-9]+)](?(?=\\.B\\[[0-9]+])\\.B\\[([0-9]+)])");
    static QRegularExpression rxLim("Limits_\\[([0-9]+)]");
    QRegularExpressionMatch match;

    for (int i=0; i<ui->cmdConfigurator->count(); i++) {
        bool test=false;
        if (cmdType!=cmdDetumble)
                test=i==cmdType;
        ui->cmdConfigurator->setTabVisible(i,test);
    }

    ui->cmdController->clear();
    ui->cmdController->addItems(cmdValidCtrls[cmdType]);
    ui->cmdActuator->clear();
    ui->cmdActuator->addItems(cmdValidActs[cmdType]);

    switch (cmdType) {
    case cmdTrn:
        ui->cmdTrnX->setText(cmdDataSplit[0]);
        ui->cmdTrnY->setText(cmdDataSplit[1]);
        ui->cmdTrnZ->setText(cmdDataSplit[2]);
        if (cmdDataSplit[3].contains("SC")) {
                match = rxScBdy.match(cmdDataSplit[3]);
                setQComboBox(ui->cmdTrnOri,scNames[match.captured(1).toInt()]);
                if (match.captured(2).isEmpty())
                ui->cmdTrnOriScBdyNum->setValue(0);
                else
                ui->cmdTrnOriScBdyNum->setValue(match.captured(2).toInt());
        }
        else {
                setQComboBox(ui->cmdTrnOri,cmdTrnOriConst[cmdDataSplit[3]]);
        }
        if (cmdDataSplit[4].contains("SC")) {
                match = rxScBdy.match(cmdDataSplit[4]);
                setQComboBox(ui->cmdTrnFrm,scNames[match.captured(1).toInt()]);
                if (match.captured(2).isEmpty())
                ui->cmdTrnFrmScBdyNum->setValue(0);
                else
                ui->cmdTrnFrmScBdyNum->setValue(match.captured(2).toInt());
        }
        else {
                setQComboBox(ui->cmdTrnFrm,cmdTrnFrmConst[cmdDataSplit[4]]);
        }
        break;
    case cmdPV:
        setQComboBox(ui->cmdPvTgtType,cmdAttTgtTypes[cmdDataSplit[0]]);
        ui->cmdPvX->setText(cmdDataSplit[1]);
        ui->cmdPvY->setText(cmdDataSplit[2]);
        ui->cmdPvZ->setText(cmdDataSplit[3]);
        if (cmdDataSplit[0].compare("VEC")==0) {
                setQComboBox(ui->cmdPvTgtAxisFrm,cmdAttTgtFrm[cmdDataSplit[4]]);
                ui->cmdPvTgtX->setText(cmdDataSplit[5]);
                ui->cmdPvTgtY->setText(cmdDataSplit[6]);
                ui->cmdPvTgtZ->setText(cmdDataSplit[7]);
        }
        else if (cmdDataSplit[0].compare("SC")==0) {
                match = rxScBdy.match(cmdDataSplit[4]);
                setQComboBox(ui->cmdPvTgtSc,scNames[match.captured(1).toInt()]);
                if (match.captured(2).isEmpty())
                ui->cmdPvTgtScBdyNum->setValue(0);
                else
                ui->cmdPvTgtScBdyNum->setValue(match.captured(2).toInt());
        }
        else if (cmdDataSplit[0].compare("BODY")==0) {
                if (cmdDataSplit[4].compare("SUN",Qt::CaseInsensitive)==0)
                cmdDataSplit[4]="SOL";
                setQComboBox(ui->cmdPvTgtWld,cmdDataSplit[4]);
        }
        break;
    case cmdSV:
        ui->cmdController->setEnabled(false);
        ui->cmdActuator->setEnabled(false);
        setQComboBox(ui->cmdSvTgtType,cmdAttTgtTypes[cmdDataSplit[0]]);
        ui->cmdSvX->setText(cmdDataSplit[1]);
        ui->cmdSvY->setText(cmdDataSplit[2]);
        ui->cmdSvZ->setText(cmdDataSplit[3]);
        if (cmdDataSplit[0].compare("VEC")==0) {
                setQComboBox(ui->cmdSvTgtAxisFrm,cmdAttTgtFrm[cmdDataSplit[4]]);
                ui->cmdSvTgtX->setText(cmdDataSplit[5]);
                ui->cmdSvTgtY->setText(cmdDataSplit[6]);
                ui->cmdSvTgtZ->setText(cmdDataSplit[7]);
        }
        else if (cmdDataSplit[0].compare("SC")==0) {
                match = rxScBdy.match(cmdDataSplit[4]);
                setQComboBox(ui->cmdSvTgtSc,scNames[match.captured(1).toInt()]);
                if (match.captured(2).isEmpty())
                ui->cmdSvTgtScBdyNum->setValue(0);
                else
                ui->cmdSvTgtScBdyNum->setValue(match.captured(2).toInt());
        }
        else if (cmdDataSplit[0].compare("BODY")==0) {
                if (cmdDataSplit[4].compare("SUN",Qt::CaseInsensitive)==0)
                cmdDataSplit[4]="SOL";
                setQComboBox(ui->cmdSvTgtWld,cmdDataSplit[4]);
        }
        break;
    case cmdQuat:
        ui->cmdQv1->setText(cmdDataSplit[0]);
        ui->cmdQv2->setText(cmdDataSplit[1]);
        ui->cmdQv3->setText(cmdDataSplit[2]);
        ui->cmdQs->setText(cmdDataSplit[3]);
        setQComboBox(ui->cmdQuatFrm,cmdTrnFrmConst[cmdDataSplit[4]]);
        break;
    case cmdMirror:
        match = rxScBdy.match(cmdDataSplit[0]);
        setQComboBox(ui->cmdMirrorTgt,scNames[match.captured(1).toInt()]);
        if (match.captured(2).isEmpty())
                ui->cmdPvTgtScBdyNum->setValue(0);
        else
                ui->cmdPvTgtScBdyNum->setValue(match.captured(2).toInt());
        break;
    case cmdDetumble:
        // Nothing Here
        break;
    case cmdWhlHManage:
        ui->cmdHManageEnabled->setChecked(cmdDataSplit[0].compare("ON",Qt::CaseInsensitive)==0);
        ui->cmdHManageMin->setText(cmdDataSplit[1]);
        ui->cmdHManageMax->setText(cmdDataSplit[2]);
        break;
    case cmdAct:
        ui->cmdActList->addItems(cmdDataSplit.mid(1));
        break;
    case cmdManeuver:
        ui->cmdManX->setText(cmdDataSplit[0]);
        ui->cmdManY->setText(cmdDataSplit[1]);
        ui->cmdManZ->setText(cmdDataSplit[2]);
        setQComboBox(ui->cmdManFrm,cmdManFrm[cmdDataSplit[3]]);
        setQComboBox(ui->cmdManType,cmdManTypes[cmdDataSplit[4]]);
        ui->cmdManTime->setText(cmdDataSplit[5]);
        setQComboBox(ui->cmdManLimits,cmdDataSplit[6]);

        break;
    default:
        break;
    }

}












