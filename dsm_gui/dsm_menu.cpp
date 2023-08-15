#include "dsm_menu.h"
#include "ui_dsm_menu.h"

DSM_Menu::DSM_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DSM_Menu)
{
    ui->setupUi(this);

    okTextPalette = ui->cmdLabel->palette();
    badTextPalette = okTextPalette;
    badTextPalette.setColor(QPalette::Text,Qt::red);


    set_validators();

    okTextBrush = entryCmdParents[cmdTypes::cmdTrn]->background(0);
    badTextBrush = okTextBrush;
    badTextBrush.setColor(Qt::red);
    badTextBrush.setStyle(Qt::SolidPattern);
}

DSM_Menu::~DSM_Menu() {
    delete ui;
}

void DSM_Menu::set_validators() {
    QRegularExpression noCmdSpacerRx("[^"+QRegularExpression::escape(cmdDelimiter)+"]*");
    QValidator *noCmdSpacer = new QRegularExpressionValidator(noCmdSpacerRx);

    ui->dsmTabs->setCurrentIndex(0);
    ui->dsmSubTabs->setCurrentIndex(0);

    ui->cmdTime->setMaximum(INFINITY);
    ui->cmdTime->setDecimals(3);

    ui->cmdLabel->setValidator( noCmdSpacer);
    ui->gainKpX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKpY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKpZ->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKrX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKrY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKrZ->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKiX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKiY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKiZ->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKiLimX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKiLimY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainKiLimZ->setValidator( new QDoubleValidator(0,INFINITY,5));

    ui->gainBandwidth->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainDampingRatio->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainAlpha->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainILim->setValidator( new QDoubleValidator(0,INFINITY,5));

    ui->gainMomDumpKpX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainMomDumpKpY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainMomDumpKpZ->setValidator( new QDoubleValidator(0,INFINITY,5));

    ui->gainLyaGain->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->gainLyaGain_2->setValidator( new QDoubleValidator(0,INFINITY,5));

    ui->limFrcX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->limFrcY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->limFrcZ->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->limRateX->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->limRateY->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->limRateZ->setValidator( new QDoubleValidator(0,INFINITY,5));

    ui->cmdTrnX->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdTrnY->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdTrnZ->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdPvX->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdPvY->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdPvZ->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdPvTgtX->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdPvTgtY->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdPvTgtZ->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdSvX->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdSvY->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdSvZ->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdSvTgtX->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdSvTgtY->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdSvTgtZ->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdQv1->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdQv2->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdQv3->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdQs->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdHManageMax->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->cmdHManageMin->setValidator( new QDoubleValidator(0,INFINITY,5));
    ui->cmdActDutyCycle->setValidator( new QDoubleValidator(0,100.0,5));
    ui->cmdManX->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdManY->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdManZ->setValidator( new QDoubleValidator(-INFINITY, INFINITY,5));
    ui->cmdManTime->setValidator( new QDoubleValidator(0,INFINITY,5));


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

    ui->cmdConfigTree->hideColumn(cmdCols::cmdColInd); // Hide this for now, I don't think this needs to be user facing

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
            break;
        case dsmSectionTypes::PRIMARY_VEC:
            newItem = new QTreeWidgetItem(parent,{"Primary Vector Command"});
            break;
        case dsmSectionTypes::SECONDARY_VEC:
            newItem = new QTreeWidgetItem(parent,{"Secondary Vector Command"});
            break;
        case dsmSectionTypes::QUATERION:
            newItem = new QTreeWidgetItem(parent,{"Quaternion Command"});
            break;
        case dsmSectionTypes::MIRROR:
            newItem = new QTreeWidgetItem(parent,{"Mirror Command"});
            break;
        case dsmSectionTypes::DETUMBLE:
            newItem = new QTreeWidgetItem(parent,{"Detumble Command"});
            break;
        case dsmSectionTypes::WHLHMANAGEMENT:
            newItem = new QTreeWidgetItem(parent,{"Wheel Momentum Management Command"});
            break;
        case dsmSectionTypes::ACTUATOR_CMD:
            newItem = new QTreeWidgetItem(parent,{"Actuator Command"});
            break;
        case dsmSectionTypes::MANEUVER:
            newItem = new QTreeWidgetItem(parent,{"Maneuver Command"});
            break;
        default:
            dsm_gui_lib::inexplicable_error_message();
            newItem = NULL;
            break;
        }
        if (newItem!=NULL) {
            newItem->setFirstColumnSpanned(true);
            entryCmdParents[section2Cmd[type]]=newItem;
            parent->addTopLevelItem(newItem);
        }
    }

    ui->actType->clear();
    ui->actType->addItems(dsm_gui_lib::sortStringList(actTypes.values()));

    ui->gainType->clear();
    ui->gainType->addItems(dsm_gui_lib::sortStringList(gainsTypes.values()));

    ui->ctrlType->clear();
    ui->ctrlType->addItems(dsm_gui_lib::sortStringList(ctrlTypes.values()));

    QStringList cmdActActuators = {actTypes["WHL"], actTypes["MTB"], "Thruster"};
    cmdActActuators.sort();
    ui->cmdActType->addItems(cmdActActuators);

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

    connect(ui->cmdActType, &QComboBox::textActivated, this, &DSM_Menu::cmd_act_data_changed);
    connect(ui->cmdActNum, &QSpinBox::textChanged, this, &DSM_Menu::cmd_act_data_changed);
    connect(ui->cmdActDutyCycle, &QLineEdit::textEdited, this, &DSM_Menu::cmd_act_data_changed);

    connect(ui->cmdManX, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManY, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManZ, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManFrm, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManType, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManLimits, &QComboBox::textActivated, this, &DSM_Menu::cmd_data_changed);
    connect(ui->cmdManTime, &QLineEdit::textEdited, this, &DSM_Menu::cmd_data_changed);

    connect(ui->cmdSC, &QComboBox::textActivated, this, &DSM_Menu::timeline_data_changed);
    connect(ui->cmdTime, &QDoubleSpinBox::textChanged, this, &DSM_Menu::timeline_data_changed);
    connect(ui->cmdTrnLabel, &QComboBox::textActivated, this, &DSM_Menu::timeline_data_changed);
    connect(ui->cmdAttLabel, &QComboBox::textActivated, this, &DSM_Menu::timeline_data_changed);
    connect(ui->cmdAttSVLabel, &QComboBox::textActivated, this, &DSM_Menu::timeline_data_changed);
    connect(ui->cmdActLabel, &QComboBox::textActivated, this, &DSM_Menu::timeline_data_changed);

    connect(ui->gainKpX, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKpY, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKpZ, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKrX, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKrY, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKrZ, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKiX, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKiY, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKiZ, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKiLimX, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKiLimY, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainKiLimZ, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);

    connect(ui->gainBandwidth, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainDampingRatio, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainAlpha, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainILim, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainMomDumpKpX, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainMomDumpKpY, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainMomDumpKpZ, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainLyaGain, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);
    connect(ui->gainLyaGain_2, &QLineEdit::textEdited, this, &DSM_Menu::gain_data_changed);

    connect(ui->limFrcX, &QLineEdit::textEdited, this, &DSM_Menu::lim_data_changed);
    connect(ui->limFrcY, &QLineEdit::textEdited, this, &DSM_Menu::lim_data_changed);
    connect(ui->limFrcZ, &QLineEdit::textEdited, this, &DSM_Menu::lim_data_changed);
    connect(ui->limRateX, &QLineEdit::textEdited, this, &DSM_Menu::lim_data_changed);
    connect(ui->limRateY, &QLineEdit::textEdited, this, &DSM_Menu::lim_data_changed);
    connect(ui->limRateZ, &QLineEdit::textEdited, this, &DSM_Menu::lim_data_changed);


}

void DSM_Menu::receive_dsmpath(QString path) {
    inoutPath = path;
    filePath = inoutPath+"Inp_DSM.txt";

    receive_data();
}

void DSM_Menu::receive_data() {
    static QRegularExpression rxSC("SC_(.*).txt");

    ui->cmdTimelineTree->clear();
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

    ui->actList->clear();
    ui->ctrlConfigTree->clear();
    ui->gainList->clear();
    ui->limList->clear();

    for (dsmSectionTypes type : section2CmdKeys) {
        int cmdType = section2Cmd[type];
        QTreeWidgetItem *parent = entryCmdParents[cmdType];
        QList<QTreeWidgetItem*> children;
        for (int i=0; i<parent->childCount(); i++)
            children << parent->child(i);
        for (QTreeWidgetItem *child : qAsConst(children))
            delete child;
    }

    trnCmdsHash.clear();
    attCmdsHash.clear();
    actCmdsHash.clear();
    attSVCmdsHash.clear();
    ctlsHash.clear();
    gainsHash.clear();
    limsHash.clear();
    actsHash.clear();

    for (QString ctrlType : ctrlValidGains.keys())
        ctrlValidGains[ctrlType].clear();

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
                if (badScNames.contains(name)) {
                    QString errMsg = "Spacecraft name \""+name+"\" is invalid.\n";
                    errMsg.append("Spacecraft cannot be named:\n");
                    for (QString badScName : badScNames)
                        errMsg.append("\t\""+badScName+"\"\n");
                    errMsg.append("Please rename your spacecraft.");
                    dsm_gui_lib::error_message(errMsg);
                    dsm_gui_lib::inexplicable_error_message();
                    DSM_Menu::close();
                }
                scNames.append(name);
            }
            break;
        }
    }
    simFile.close();
    scNames.sort();

    ui->cmdSC->addItems(scNames);
    ui->cmdTrnOri->addItems(scNames);
    ui->cmdTrnFrm->addItems(scNames);
    ui->cmdPvTgtSc->addItems(scNames);
    ui->cmdSvTgtSc->addItems(scNames);
    ui->cmdMirrorTgt->addItems(scNames);


    QFile dsmFile(filePath);
    if(!dsmFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", dsmFile.errorString());
    QTextStream in(&dsmFile);

    for (int key : cmdValidCtrls.keys())
        cmdValidCtrls[key].clear();
    for (int key : cmdValidActs.keys())
        cmdValidActs[key].clear();

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

    for (int col=0; col<ui->cmdTimelineTree->columnCount(); col++)
        ui->cmdTimelineTree->resizeColumnToContents(col);

    for (int col=0; col<ui->ctrlConfigTree->columnCount(); col++)
        ui->ctrlConfigTree->resizeColumnToContents(col);

    ui->cmdConfigTree->setAnimated(false);
    for (QTreeWidgetItem *item : entryCmdParents.values())
        item->setExpanded(true);
    for (int col=0; col<ui->cmdConfigTree->columnCount(); col++)
        ui->cmdConfigTree->resizeColumnToContents(col);
    for (QTreeWidgetItem *item : entryCmdParents.values())
        item->setExpanded(false);
    ui->cmdConfigTree->setAnimated(true);

    trnCmdsHash.insert(trnCmdsHashConst);
    attCmdsHash.insert(attCmdsHashConst);
    actCmdsHash.insert(actCmdsHashConst);

    populate_cmdtl_dropdowns(cmdTypes::cmdTrn);
    populate_cmdtl_dropdowns(cmdTypes::cmdAtt);
    populate_cmdtl_dropdowns(cmdTypes::cmdAct);
    populate_ctrl_dropdowns();

    ui->cmdManLimits->clear();
    ui->cmdManLimits->addItems(dsm_gui_lib::sortStringList(limsHash.keys()));

    ui->cmdTimelineTree->sortByColumn(tlCols::tlColSC,Qt::AscendingOrder);
    ui->cmdTimelineTree->sortByColumn(tlCols::tlColTime,Qt::AscendingOrder);
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
        descrip.append("# If Col: 3 -> CmdTime, then Col: 5 is \"NUM_CMD[#]\" (\"#\" is the number of commands)");
        descrip.append("#                       Command Types:");
        descrip.append("#                       Translation Command Method_Mode (\"PASSIVE_TRN\" for none)");
        descrip.append("#                       Attitude Command Method_Mode (\"PASSIVE_ATT\" for none)");
        descrip.append("#                       Optional Actuator Command_Mode");
        break;
    case dsmSectionTypes::TRANSLATION:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                             Translation Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"TranslationCmd_[#]\"");
        descrip.append("# Col: 2   ->  Position 1: X (meters)");
        descrip.append("# Col: 3   ->  Position 2: Y (meters)");
        descrip.append("# Col: 4   ->  Position 3: Z (meters)");
        descrip.append("# Col: 5   ->  Ref Origin (ctrl wrt): Orbit Point \"OP\" or SC ID Number (\"SC[#].B[#]\")");
        descrip.append("# Col: 6   ->  Ref Frame Directions: \"N\", \"F\", \"L\", or SC ID Number (\"SC[#].B[#]\")");
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
        descrip.append("# Col: 6   ->  Primary Target: Specify Body, SC[#].B[#] or Vec");
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
        descrip.append("# Col: 6   ->  Secondary Target: Specify Body, SC[#].B[#] or Vec");
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
        if (itemNum >= scNames.length()) {
            dsm_gui_lib::error_message("More Spacecraft in \"Inp_DSM.txt\" than in \"Inp_Sim.txt\".");
            dsm_gui_lib::inexplicable_error_message();
            DSM_Menu::close();
            return; // won't get here, realistically
        }

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
            else
                label = "Passive";
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
        newTreeItem->setText(cmdCols::cmdColData, dataSplit.join(cmdDataSpacer));

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

        for (QString ctrl : allowableCtrl[dataSplit[0]])
            ctrlValidGains[ctrl].append(label);

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
        dsm_gui_lib::inexplicable_error_message();
        return;
        break;
    }
}

void DSM_Menu::set_command_ctrl_act(QTreeWidgetItem *item, const QString dataString, QStringList *dataList) {
    static QRegularExpression rxCtrls("Controller_\\[([0-9]+)]");
    static QRegularExpression   rxActs("Actuators_\\[([0-9]+)]");
    QRegularExpressionMatch match;
    int searchNum;

    match = rxCtrls.match(dataString);
    if (match.captured(1).isEmpty()) {
        dsm_gui_lib::error_message(item->text(cmdCols::cmdColLabel)+R"( is missing "Controller_[#]".)");
        return;
    }
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
    if (match.captured(1).isEmpty()) {
        dsm_gui_lib::error_message(item->text(cmdCols::cmdColLabel)+R"( is missing "Actuator_[#]".)");
        return;
    }
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
    default:
        dsm_gui_lib::inexplicable_error_message();
        return "";
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

/* This function sets up the Regular Expression to parse lines from the Inp_DSM.txt file */
QRegularExpression DSM_Menu::entryItemRegEx(const dsmSectionTypes type) {
    static const QString format = "^\\s*%1\\[([0-9]+)]\\s*([^"+labelMkr
                                  +"]*)\\s*(?(?="+labelMkr+"+)"+labelMkr+"+\\s*(.*))$";

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
            cmdData.append("NUM_CMD["+QString::number(ui->cmdActList->count())+"]");
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
            dsm_gui_lib::inexplicable_error_message();
            return;
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

    curItem->setText(cmdCols::cmdColAct,arg1);
}

void DSM_Menu::on_cmdLabel_textEdited(const QString &arg1) {
    QTreeWidgetItem *curItem = ui->cmdConfigTree->currentItem();
    if (curItem->parent()==NULL)
        return;

    int itemNum = curItem->data(cmdCols::cmdColInd,Qt::DisplayRole).toInt();
    int cmdType = entryCmdParents.key(curItem->parent());
    int checkCol;
    QString value = entryItemFormat(section2Cmd.key(cmdType)).arg(itemNum);
    QComboBox *checkBox;
    QString oldKey;
    QHash <QString,QString> *checkHash;

    if (trnCmds.contains(cmdType)){
        checkHash=&trnCmdsHash;
        checkCol = tlCols::tlColTrn;
        checkBox = ui->cmdTrnLabel;
    }
    else if (attCmds.contains(cmdType)){
        checkHash=&attCmdsHash;
        checkCol = tlCols::tlColAtt;
        checkBox = ui->cmdAttLabel;
    }
    else if (cmdType == cmdSV) {
        checkHash=&attSVCmdsHash;
        checkCol = tlCols::tlColAtt;
        checkBox = ui->cmdAttSVLabel;
    }
    else {
        checkHash=&actCmdsHash;
        checkCol = tlCols::tlColAct;
        checkBox = ui->cmdActLabel;
    }

    oldKey = checkHash->key(value);
    if (arg1.isEmpty() || checkHash->keys().contains(arg1)) {
        ui->cmdLabel->setPalette(badTextPalette);
        return;
    }
    else {
        ui->cmdLabel->setPalette(okTextPalette);
    }

    checkHash->take(oldKey);
    checkHash->insert(arg1,value);

    QList<QTreeWidgetItem*> changeItems = ui->cmdTimelineTree->findItems("*",Qt::MatchWildcard,checkCol);
    static QRegularExpression oldKeyRx;

    if (cmdType == cmdPV)
        oldKeyRx = QRegularExpression("^(?(?="+QRegularExpression::escape(oldKey+cmdDelimiter+cmdDataSpacer)+".*)"
                                      +QRegularExpression::escape(oldKey)+"(?="
                                      +QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+")|"
                                      +QRegularExpression::escape(oldKey)+")");
    else if (cmdType == cmdSV)
        oldKeyRx = QRegularExpression(QRegularExpression::escape(cmdDelimiter+cmdDataSpacer+oldKey)+"$");

    QString removeTxt;
    for (QTreeWidgetItem *item : qAsConst(changeItems)) {
        removeTxt = item->text(checkCol);
        if (cmdType == cmdPV) {
            removeTxt.replace(oldKeyRx,arg1);
        }
        else if (cmdType == cmdSV) {
            removeTxt.replace(oldKeyRx,cmdDelimiter+cmdDataSpacer+arg1);
        }
        else if (removeTxt.compare(oldKey)==0) {
            removeTxt = arg1;
        }
        item->setText(checkCol,removeTxt);
        int checkInd = checkBox->findText(oldKey);
        if (checkInd != -1)
            checkBox->setItemText(checkInd,arg1);
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

    populate_cmd_dropdowns();

    setQComboBox(ui->cmdController,item->text(DSM_Menu::cmdColCtl));
    setQComboBox(ui->cmdActuator,item->text(DSM_Menu::cmdColAct));
    ui->cmdLabel->setText(item->text(DSM_Menu::cmdColLabel));

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
        ui->cmdActList->clear();
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
        dsm_gui_lib::inexplicable_error_message();
        return;
        break;
    }

}

void DSM_Menu::on_closeButton_clicked() {
    DSM_Menu::close();
}

void DSM_Menu::on_loadDefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Delete Inp_DSM file and load current Default?");
    if (response == QMessageBox::Ok) {

        QFile::remove(filePath);
        QFile::copy(inoutPath+"__default__/Inp_DSM.txt", filePath);

        receive_data();
    }
    else return;
}

void DSM_Menu::on_saveDefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Delete Default Inp_DSM file and replace with current?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inoutPath+"__default__/Inp_DSM.txt");
        QFile::copy(filePath, inoutPath+"__default__/Inp_DSM.txt");
    }
    else return;


}

void DSM_Menu::on_cmdRemove_clicked() {
    QTreeWidgetItem *item = ui->cmdConfigTree->currentItem();
    if (item==NULL || item->parent()==NULL)
        return;

    ui->cmdConfigTree->setCurrentItem(item->parent());
    item->parent()->removeChild(item);

    // need to remove this command from timeline & comboboxes in timeline
}

void DSM_Menu::on_cmdAdd_clicked() {
    QTreeWidgetItem *newCmdTypeParent = ui->cmdConfigTree->currentItem();
    if (newCmdTypeParent == NULL) return;
    if (newCmdTypeParent->parent()!=NULL)
        newCmdTypeParent = newCmdTypeParent->parent();

    QList<int> usedCmdInds;
    for (int i=0; i<newCmdTypeParent->childCount(); i++)
        usedCmdInds.append(newCmdTypeParent->child(i)->data(cmdCols::cmdColLabel,cmdData::cmdNum).toInt());

    int newCmdInd;
    for (newCmdInd=0; newCmdInd<5e3; newCmdInd++)
        if (!usedCmdInds.contains(newCmdInd)) break;

    int cmdType = entryCmdParents.key(newCmdTypeParent);
    dsmSectionTypes type = section2Cmd.key(cmdType);
    QString newLabel = entryItemFormat(type).arg(newCmdInd);

    QString ctrlName, actName, limName;
    QList<QTreeWidgetItem*> tmpCtrlItems;
    QList<QListWidgetItem*> tmpActItems;


    if (cmdType==cmdTrn) {
        if (cmdValidCtrls[cmdType].isEmpty()) {
            dsm_gui_lib::error_message("You must first define a valid Translation Controller.");
            return;
        }
        tmpCtrlItems = ui->ctrlConfigTree->findItems(cmdValidCtrls[cmdType].first(),Qt::MatchExactly,cmdCols::cmdColLabel);
        ctrlName = entryItemFormat(dsmSectionTypes::CONTROLLERS).arg(tmpCtrlItems.first()->data(cmdCols::cmdColLabel,cmdData::cmdNum).toInt());
    }
    else if (attCmds.contains(cmdType)) {
        if (cmdValidCtrls[cmdType].isEmpty()) {
            dsm_gui_lib::error_message("You must first define a valid Attitude Controller.");
            return;
        }
        tmpCtrlItems = ui->ctrlConfigTree->findItems(cmdValidCtrls[cmdType].first(),Qt::MatchExactly,cmdCols::cmdColLabel);
        ctrlName = entryItemFormat(dsmSectionTypes::CONTROLLERS).arg(tmpCtrlItems.first()->data(cmdCols::cmdColLabel,cmdData::cmdNum).toInt());
    }

    if (trnCmds.contains(cmdType)) {
        if (cmdValidActs[cmdType].isEmpty()) {
            dsm_gui_lib::error_message("You must first define a valid Translation Actuator.");
            return;
        }
        tmpActItems = ui->actList->findItems(cmdValidActs[cmdType].first(),Qt::MatchExactly);
        actName = entryItemFormat(dsmSectionTypes::ACTUATORS).arg(tmpActItems.first()->data(actData::actNum).toInt());
    }
    else if (attCmds.contains(cmdType)) {
        if (cmdValidActs[cmdType].isEmpty()) {
            dsm_gui_lib::error_message("You must first define a valid Attitude Actuator.");
            return;
        }
        tmpActItems = ui->actList->findItems(cmdValidActs[cmdType].first(),Qt::MatchExactly);
        actName = entryItemFormat(dsmSectionTypes::ACTUATORS).arg(tmpActItems.first()->data(actData::actNum).toInt());
    }

    QString newCmdData;
    switch (type) {
    case dsmSectionTypes::TRANSLATION:
        newCmdData = "0.0  0.0  0.0  OP  F";
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(ctrlName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    case dsmSectionTypes::PRIMARY_VEC:
        newCmdData = "VEC  1  0  0  L  1  0  0";
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(ctrlName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    case dsmSectionTypes::SECONDARY_VEC:
        newCmdData = "VEC  0  1  0  L  0  1  0";
        break;
    case dsmSectionTypes::QUATERION:
        newCmdData = "0  0  0  1  L";
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(ctrlName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    case dsmSectionTypes::MIRROR:
        newCmdData = "SC[0]";
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(ctrlName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    case dsmSectionTypes::DETUMBLE:
        newCmdData = "";
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(ctrlName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    case dsmSectionTypes::WHLHMANAGEMENT:
        newCmdData = "OFF  0  0";
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(ctrlName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    case dsmSectionTypes::ACTUATOR_CMD:
        newCmdData = "NUM_CMD[0]";
        break;
    case dsmSectionTypes::MANEUVER:
        newCmdData = "0.0  0.0  0.0  B  SMOOTHED  0.0";
        newCmdData.append(cmdDataSpacer);
        if (ui->limList->count()==0) {
            dsm_gui_lib::error_message("You must first define a valid Limit.");
            return;
        }

        limName = entryItemFormat(dsmSectionTypes::LIMITS).arg(ui->limList->item(0)->data(limData::limNum).toInt());
        newCmdData.append(limName);
        newCmdData.append(cmdDataSpacer);
        newCmdData.append(actName);
        break;
    default:
        dsm_gui_lib::inexplicable_error_message();
        return;
        break;
    }

    new_entry_item(type, newLabel, newCmdInd, newCmdData.split(QRegExp("//s"),Qt::SkipEmptyParts).join(cmdDataSpacer));
}

void DSM_Menu::on_cmdDuplicate_clicked() {
    QTreeWidgetItem *curItem = ui->cmdConfigTree->currentItem();
    if (curItem==NULL || curItem->parent()==NULL)
        return;

    QList<int> usedCmdInds;
    QStringList curLabels;
    for (int i=0; i<curItem->parent()->childCount(); i++) {
        curLabels.append(curItem->parent()->child(i)->text(cmdCols::cmdColLabel));
        usedCmdInds.append(curItem->parent()->child(i)->data(cmdCols::cmdColLabel,cmdData::cmdNum).toInt());
    }

    int newCmdInd;
    for (newCmdInd=0; newCmdInd<5e3; newCmdInd++)
        if (!usedCmdInds.contains(newCmdInd)) break;

    QString newLabel = curItem->text(cmdCols::cmdColLabel)+"_Copy";
    for (int i=0; i<5e3; i++) {
        QString newLabelTest = newLabel;
        if (i>0) newLabelTest += "_"+QString::number(i);
        if (!curLabels.contains(newLabelTest,Qt::CaseInsensitive)) {
            newLabel = newLabelTest;
            break;
        }
    }

    int newCmdType = entryCmdParents.key(curItem->parent());
    dsmSectionTypes newSecType = section2Cmd.key(newCmdType);
    QString newData = curItem->text(cmdCols::cmdColData);
    new_entry_item(newSecType,newLabel,newCmdInd,newData);

    QTreeWidgetItem *curTime = ui->cmdTimelineTree->currentItem();
    if (curTime != NULL) {
        populate_cmdtl_dropdowns(newCmdType);
    }

//    QTreeWidgetItem *newItem = curItem->clone();
//    newItem->setText(cmdCols::cmdColLabel,newLabel);
//    newItem->setData(cmdCols::cmdColLabel,cmdData::cmdNum,newCmdInd);
//    newItem->setData(cmdCols::cmdColInd,Qt::DisplayRole,newCmdInd);
//    curItem->parent()->addChild(newItem);
}

void DSM_Menu::cmd_act_data_changed() {
    QListWidgetItem *item = ui->cmdActList->currentItem();
    QString tmpStr = ui->cmdActType->currentText();
    if (tmpStr.compare(actTypes["WHL"])==0)
        tmpStr = "WHL_[%1]_[%2]";
    else if (tmpStr.compare(actTypes["MTB"])==0)
        tmpStr = "MTB_[%1]_[%2]";
    else if (tmpStr.compare("Thruster")==0)
        tmpStr = "THR_[%1]_[%2]";

    tmpStr = tmpStr.arg(ui->cmdActNum->value()).arg(ui->cmdActDutyCycle->text());
    item->setText(tmpStr);

    cmd_data_changed();
}

void DSM_Menu::on_cmdActAdd_clicked() {
     QListWidgetItem *item = new QListWidgetItem("WHL_[0]_[0.0]");

    ui->cmdActList->addItem(item);
    cmd_data_changed();
}

void DSM_Menu::on_cmdActRemove_clicked() {
    int item = ui->cmdActList->currentRow();
    if (item == -1) return;

    ui->cmdActList->takeItem(item);
    cmd_data_changed();
}

void DSM_Menu::on_cmdActList_currentItemChanged(QListWidgetItem *current, QListWidgetItem*) {
    static QRegularExpression actRx("^(.*)_\\[([0-9]+)]_\\[([0-9.]+)]");
    QRegularExpressionMatch match = actRx.match(current->text());

    if (match.captured(1).compare("WHL")==0) {
        ui->cmdActType->setCurrentText(actTypes["WHL"]);
    }
    else if (match.captured(1).compare("MTB")==0) {
        ui->cmdActType->setCurrentText(actTypes["MTB"]);
    }
    else if (match.captured(1).compare("THR")==0) {
        ui->cmdActType->setCurrentText("Thruster");
    }
    ui->cmdActNum->setValue(match.captured(2).toInt());
    ui->cmdActDutyCycle->setText(match.captured(3));
}

void DSM_Menu::on_cmdTimelineTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem*) {
//    static QRegularExpression rxPV("^((?(?=.*"+QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+".*).*(?="
//                                   +QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+")|.*))");

    bool isNull = current==NULL;
    ui->cmdSC->setEnabled(!isNull);
    ui->cmdSCLabel->setEnabled(!isNull);
    ui->cmdTime->setEnabled(!isNull);
    ui->cmdTimeLabel->setEnabled(!isNull);
    ui->cmdLabelSecLabel->setEnabled(!isNull);
    ui->cmdTypeSecLabel->setEnabled(!isNull);
    ui->cmdSVSecLabel->setEnabled(!isNull);
    ui->cmdTrnLabelLabel->setEnabled(!isNull);
    ui->cmdAttLabelLabel->setEnabled(!isNull);
    ui->cmdActLabelLabel->setEnabled(!isNull);
    ui->cmdTrnLabel->setEnabled(!isNull);
    ui->cmdAttLabel->setEnabled(!isNull);
    ui->cmdAttSVLabel->setEnabled(!isNull);
    ui->cmdActLabel->setEnabled(!isNull);
    if (isNull) {
        ui->cmdSC->setCurrentText(scNames[0]);
        ui->cmdTime->setValue(0.0);
        ui->cmdTrnLabel->setCurrentIndex(0);
        ui->cmdAttLabel->setCurrentIndex(0);
        ui->cmdActLabel->setCurrentIndex(0);
        ui->cmdAttSVLabel->setCurrentText("");
        return;
    }

    QString curAttCmd = current->text(tlCols::tlColAtt);

    ui->cmdSC->setCurrentText(current->text(tlCols::tlColSC));

    if (current->text(tlCols::tlColTrn).isEmpty())
        ui->cmdTrnLabel->setCurrentText("No Change");
    else
        ui->cmdTrnLabel->setCurrentText(current->text(tlCols::tlColTrn));

    if (current->text(tlCols::tlColAct).isEmpty())
        ui->cmdActLabel->setCurrentText("No Change");
    else
        ui->cmdActLabel->setCurrentText(current->text(tlCols::tlColAct));

    if (curAttCmd.isEmpty())
        ui->cmdAttLabel->setCurrentText("No Change");
    else
        populate_cmdtl_dropdowns(cmdTypes::cmdSV);

    ui->cmdTime->setValue(current->data(tlCols::tlColTime,Qt::DisplayRole).toDouble());
}

void DSM_Menu::timeline_data_changed() {
    QTreeWidgetItem *curItem = ui->cmdTimelineTree->currentItem();
    if (curItem == NULL) return;

    curItem->setText(tlCols::tlColSC,ui->cmdSC->currentText());
    curItem->setData(tlCols::tlColTime,Qt::DisplayRole,ui->cmdTime->value());

    if (ui->cmdTrnLabel->currentText().compare("No Change")==0)
        curItem->setText(tlCols::tlColTrn,"");
    else
        curItem->setText(tlCols::tlColTrn,ui->cmdTrnLabel->currentText());

    if (ui->cmdActLabel->currentText().compare("No Change")==0)
        curItem->setText(tlCols::tlColAct,"");
    else
        curItem->setText(tlCols::tlColAct,ui->cmdActLabel->currentText());

    QString attCmd = ui->cmdAttLabel->currentText();
    if (attCmd.compare("No Change")==0)
        curItem->setText(tlCols::tlColAtt,"");
    else {

        QString svCmd = ui->cmdAttSVLabel->currentText();
        ui->cmdAttSVLabel->setEnabled(false);
        if (attCmdsHash[attCmd].contains(entryItemName(dsmSectionTypes::PRIMARY_VEC))) {
            ui->cmdAttSVLabel->setEnabled(true);
            validate_sv_cmds(attCmd+cmdDelimiter+cmdDataSpacer+svCmd);
            svCmd = ui->cmdAttSVLabel->currentText();

            if (!svCmd.isEmpty())
                attCmd += cmdDelimiter+cmdDataSpacer+svCmd;
        }
        curItem->setText(tlCols::tlColAtt,attCmd);
    }

}

void DSM_Menu::validate_sv_cmds(QString curAttCmd) {
    static QRegularExpression rxPV("^((?(?=.*"+QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+".*).*(?="
                                   +QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+")|.*))");
    static QRegularExpression rxSV(QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+"(.*)$");

    QString pvLabel = rxPV.match(curAttCmd).captured(1);
    QString svLabel = rxSV.match(curAttCmd).captured(1);

    QStringList validCmdSV={""};
    QString cmdPVData;
    for (int i=0; i<entryCmdParents[cmdPV]->childCount(); i++) {
        if (entryCmdParents[cmdPV]->child(i)->text(cmdCols::cmdColLabel).compare(pvLabel)==0) {
            cmdPVData = entryCmdParents[cmdPV]->child(i)->text(cmdCols::cmdColData);
            break;
        }
        if (i==(entryCmdParents[cmdPV]->childCount()-1)) {
            dsm_gui_lib::inexplicable_error_message();
            return;
        }
    }
    QStringList dataSplit = cmdPVData.split(QRegExp("\\s"),Qt::SkipEmptyParts);
    QString cmdPVType = dataSplit[0];
    QVector3D cmdPVAxis(dataSplit[1].toDouble(),dataSplit[2].toDouble(),dataSplit[3].toDouble());

    QString cmdPVTgtThing = dataSplit[4];
    QVector3D cmdPVTgtAxis;
    bool isVEC = cmdPVType.compare("VEC")==0;
    if (isVEC) {
        cmdPVTgtAxis.setX(dataSplit[5].toDouble());
        cmdPVTgtAxis.setY(dataSplit[6].toDouble());
        cmdPVTgtAxis.setZ(dataSplit[7].toDouble());
    }

    for (int i=0; i<entryCmdParents[cmdSV]->childCount(); i++) {
        bool svIsValid = true;
        QTreeWidgetItem *svCmd = entryCmdParents[cmdSV]->child(i);
        QString svData = svCmd->text(cmdCols::cmdColData);
        QStringList svDataSplit = svData.split(QRegExp("\\s"),Qt::SkipEmptyParts);
        QString svType = svDataSplit[0];
        QVector3D svAxis(svDataSplit[1].toDouble(),svDataSplit[2].toDouble(),svDataSplit[3].toDouble());
        QString svTgtType = svDataSplit[4];

        QVector3D crossAxis = QVector3D::crossProduct(cmdPVAxis/cmdPVAxis.length(),svAxis/svAxis.length());

        double smallCheck = std::numeric_limits<double>::epsilon();

        if (crossAxis.length()<=smallCheck) svIsValid = false;

        if (isVEC && (svType.compare("VEC")==0)) {
            QVector3D svTgtAxis(svDataSplit[5].toDouble(),svDataSplit[6].toDouble(),svDataSplit[7].toDouble());
            if (cmdPVTgtThing.compare(svTgtType)==0) {
                crossAxis = QVector3D::crossProduct(svTgtAxis/svTgtAxis.length(),cmdPVTgtAxis/cmdPVTgtAxis.length());
                if (crossAxis.length()<=smallCheck) svIsValid = false;
            }

        }
        else if (cmdPVTgtThing.compare(svTgtType)==0) svIsValid=false;

        if (svIsValid) validCmdSV.append(svCmd->text(cmdCols::cmdColLabel));
    }
    ui->cmdAttSVLabel->clear();
    ui->cmdAttSVLabel->addItems(validCmdSV);

    if (validCmdSV.contains(svLabel))
        ui->cmdAttSVLabel->setCurrentText(svLabel);
    else
        ui->cmdAttSVLabel->setCurrentText("");
}

void DSM_Menu::on_cmdTimeDuplicate_clicked() {
    QTreeWidgetItem *curItem = ui->cmdTimelineTree->currentItem();
    if (curItem==NULL) return;

    QTreeWidgetItem *newItem = curItem->clone();
    ui->cmdTimelineTree->addTopLevelItem(newItem);
}

void DSM_Menu::on_cmdTimeAdd_clicked() {
    QTreeWidgetItem *newItem = new QTreeWidgetItem(ui->cmdTimelineTree);
    newItem->setText(tlCols::tlColSC,scNames[0]);
    newItem->setData(tlCols::tlColTime,Qt::DisplayRole,0.0);
    newItem->setTextAlignment(tlCols::tlColTime,Qt::AlignRight);
    ui->cmdTimelineTree->addTopLevelItem(newItem);
}

void DSM_Menu::on_cmdTimeRemove_clicked() {
    QTreeWidgetItem *curItem = ui->cmdTimelineTree->currentItem();
    if (curItem==NULL) return;

    ui->cmdTimelineTree->takeTopLevelItem(ui->cmdTimelineTree->indexOfTopLevelItem(curItem));
    ui->cmdTimelineTree->setCurrentItem(NULL);
}

void DSM_Menu::on_actList_currentItemChanged(QListWidgetItem *current, QListWidgetItem*) {
    bool isNull = current == NULL;
    ui->actType->setEnabled(!isNull);
    ui->actActTypeLabel->setEnabled(!isNull);
    ui->actLabel->setEnabled(!isNull);
    ui->actLabelLabel->setEnabled(!isNull);

    if (isNull) {
        ui->actType->setCurrentIndex(0);
        ui->actLabel->clear();
        return;
    }

    setQComboBox(ui->actType,actTypes[current->data(actData::actType).toString()]);
    ui->actLabel->setText(current->text());
}

void DSM_Menu::on_actType_textActivated(const QString &arg1) {
    QListWidgetItem *current = ui->actList->currentItem();
    if (current==NULL)
        return;

    QString oldType = current->data(actData::actType).toString();
    QString label   = current->text();

    for (int cmd : actValidCmds[oldType]) {
        cmdValidActs[cmd].removeOne(label);
    }

    QString newType = actTypes.key(arg1);

    current->setData(actData::actType,newType);
    for (int cmd : actValidCmds[newType]) {
        cmdValidActs[cmd].append(label);
    }

    QList<int> checkCmds = actValidCmds[oldType];
    for (int cmd: actValidCmds[newType]) {
        if (!checkCmds.contains(cmd))
            checkCmds.append(cmd);
    }

    for (int cmd : checkCmds) {
        for (int i=0 ; i<entryCmdParents[cmd]->childCount(); i++) {
            QTreeWidgetItem* item = entryCmdParents[cmd]->child(i);
            if (item->text(cmdCols::cmdColAct).compare(label)==0)
                on_cmdConfigTree_itemChanged(item,cmdCols::cmdColAct);//revalid actuator type
        }
    }
}

void DSM_Menu::on_actLabel_textEdited(const QString &arg1) {
    QListWidgetItem *current = ui->actList->currentItem();
    if (current==NULL)
        return;

    int itemNum = current->data(actData::actNum).toInt();

    QString value = entryItemFormat(dsmSectionTypes::ACTUATORS).arg(itemNum);
    QString oldKey = actsHash.key(value);

    if (arg1.isEmpty() || actsHash.keys().contains(arg1)) {
        ui->actLabel->setPalette(badTextPalette);
        return;
    }
    else {
        ui->actLabel->setPalette(okTextPalette);
    }

    actsHash.remove(oldKey);
    actsHash.insert(arg1,value);

    QString actType = current->data(actData::actType).toString();

    current->setText(arg1);
    for (int cmd : actValidCmds[actType]) {
        QStringList *validActList = &cmdValidActs[cmd];
        validActList->replace(validActList->indexOf(oldKey),arg1);
        for (int i=0 ; i<entryCmdParents[cmd]->childCount(); i++) {
            QTreeWidgetItem* item = entryCmdParents[cmd]->child(i);
            if (item->text(cmdCols::cmdColAct).compare(oldKey)==0) {
                item->setText(cmdCols::cmdColAct,arg1);
            }
        }
    }
    populate_cmd_dropdowns();
}

void DSM_Menu::on_actRemove_clicked() {
    QListWidgetItem *current = ui->actList->currentItem();
    if (current==NULL)
        return;

    QString label = current->text();
    QString actType = current->data(actData::actType).toString();
    for (int cmd : actValidCmds[actType]) {
        for (int i=0 ; i<entryCmdParents[cmd]->childCount(); i++) {
            QTreeWidgetItem* item = entryCmdParents[cmd]->child(i);
            if (item->text(cmdCols::cmdColAct).compare(label)==0) {
                item->setText(cmdCols::cmdColAct,"");
            }
        }
    }

    for (int cmd : actValidCmds[actType])
        cmdValidActs[cmd].removeOne(label);

    delete current;
    ui->actList->setCurrentItem(NULL);
    populate_cmd_dropdowns();
}

void DSM_Menu::on_cmdConfigTree_itemChanged(QTreeWidgetItem *item, int column) {
    if (item == NULL || item->parent() == NULL)
        return;

    int cmdType = entryCmdParents.key(item->parent());

    QString text = item->text(column);

    if (text.isEmpty()) {
        item->setBackground(column,badTextBrush);
        return;
    }

    QListWidgetItem* checkListItem;
    QTreeWidgetItem* checkTreeItem;

    switch (column) {
    case cmdCols::cmdColAct:
        checkListItem = ui->actList->findItems(text,Qt::MatchExactly).at(0);
        if (actValidCmds[checkListItem->data(actData::actType).toString()].contains(cmdType))
            item->setBackground(column,okTextBrush);
        else
            item->setBackground(column,badTextBrush);
        break;
    case cmdCols::cmdColCtl:
        checkTreeItem = ui->ctrlConfigTree->findItems(text,Qt::MatchExactly,ctrlCols::ctrlColLabel).at(0);
        if (ctrlValidCmds[ctrlTypes.key(checkTreeItem->text(ctrlCols::ctrlColType))].contains(cmdType))
            item->setBackground(column,okTextBrush);
        else
            item->setBackground(column,badTextBrush);
        break;
    default:
        return;
    }
}

void DSM_Menu::on_actAdd_clicked() {
    QList<int> usedCmdInds;
    QStringList usedLabels;
    for (int i=0; i<ui->actList->count(); i++) {
        usedCmdInds.append(ui->actList->item(i)->data(actData::actNum).toInt());
        usedLabels.append(ui->actList->item(i)->text());
    }

    int newActInd;
    QString newLabel;
    for (newActInd=0; newActInd<5e3; newActInd++) {
        newLabel = entryItemFormat(dsmSectionTypes::ACTUATORS).arg(newActInd);
        if (!usedCmdInds.contains(newActInd)&&!usedLabels.contains(newLabel)) break;
    }

    QString actType = "Ideal";

    new_entry_item(dsmSectionTypes::ACTUATORS,newLabel,newActInd,{actType});

    populate_cmd_dropdowns();

}

void DSM_Menu::on_actDuplicate_clicked() {
    QListWidgetItem *curItem = ui->actList->currentItem();
    if (curItem == NULL)
        return;

    QList <int> usedCmdInds;
    QStringList usedLabels;
    for (int i=0; i<ui->actList->count(); i++) {
        usedCmdInds.append(ui->actList->item(i)->data(actData::actNum).toInt());
        usedLabels.append(ui->actList->item(i)->text());
    }

    int newActInd;
    for (newActInd=0; newActInd<5e3; newActInd++)
        if (!usedCmdInds.contains(newActInd)) break;

    QString newLabel = curItem->text()+"_Copy";
    for (int i=0;i<5e3; i++) {
        QString newLabelTest = newLabel;
        if (i>0) newLabelTest += "_"+QString::number(i);
        if (!usedLabels.contains(newLabelTest,Qt::CaseInsensitive)) {
            newLabel = newLabelTest;
            break;
        }
    }

    new_entry_item(dsmSectionTypes::ACTUATORS,newLabel,newActInd,curItem->data(actData::actType).toString());

    populate_cmd_dropdowns();

}

void DSM_Menu::populate_cmdtl_dropdowns(int cmdtype) {
    static QRegularExpression rxPV("^((?(?=.*"+QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+".*).*(?="
                                   +QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+")|.*))");
    if (trnCmds.contains(cmdtype)) {
        ui->cmdTrnLabel->clear();
        QStringList newCmdList = trnCmdsHash.keys();
        newCmdList.sort();
        QStringList constCmds = dsm_gui_lib::sortStringList(trnCmdsHashConst.keys());

        for (auto i=constCmds.crbegin(); i!=constCmds.crend(); i++) {
            newCmdList.removeOne(*i);
            newCmdList.prepend(*i);
        }

        ui->cmdTrnLabel->addItems(newCmdList);
    }
    else if (attCmds.contains(cmdtype)) {
        ui->cmdAttLabel->clear();
        QStringList newCmdList = attCmdsHash.keys();
        newCmdList.sort();
        QStringList constCmds = dsm_gui_lib::sortStringList(attCmdsHashConst.keys());

        for (auto i=constCmds.crbegin(); i!=constCmds.crend(); i++) {
            newCmdList.removeOne(*i);
            newCmdList.prepend(*i);
        }

        ui->cmdAttLabel->addItems(newCmdList);
    }
    else if (cmdtype == cmdSV) {
        QString curAttCmd = ui->cmdTimelineTree->currentItem()->text(tlCols::tlColAtt);
        QString pvLabel = rxPV.match(curAttCmd).captured(1);
        ui->cmdAttLabel->setCurrentText(pvLabel);
        ui->cmdAttSVLabel->clear();
        bool isPVCmd = attCmdsHash[pvLabel].contains(entryItemName(dsmSectionTypes::PRIMARY_VEC));
        ui->cmdAttSVLabel->setEnabled(isPVCmd);
        if (isPVCmd) validate_sv_cmds(curAttCmd);
    }
    else if (cmdtype == cmdAct) {
        ui->cmdActLabel->clear();
        QStringList newCmdList = actCmdsHash.keys();
        newCmdList.sort();
        QStringList constCmds = dsm_gui_lib::sortStringList(actCmdsHashConst.keys());

        for (auto i=constCmds.crbegin(); i!=constCmds.crend(); i++) {
            newCmdList.removeOne(*i);
            newCmdList.prepend(*i);
        }

        ui->cmdActLabel->addItems(newCmdList);

    }
}

void DSM_Menu::populate_cmd_dropdowns() {
    // isCtrl if !isAct
    QTreeWidgetItem *curCmd = ui->cmdConfigTree->currentItem();
    ui->cmdActuator->clear();
    ui->cmdController->clear();
    if (curCmd!=NULL && curCmd->parent()!=NULL) {
        int cmd = entryCmdParents.key(curCmd->parent());
        ui->cmdActuator->addItems(cmdValidActs[cmd]);
        ui->cmdController->addItems(cmdValidCtrls[cmd]);
    }
}

void DSM_Menu::on_ctrlConfigTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    bool isNull = current==NULL;
    ui->ctrlTypeLabel->setEnabled(!isNull);
    ui->ctrlType->setEnabled(!isNull);
    ui->ctrlLabel->setEnabled(!isNull);
    ui->ctrlLabelLabel->setEnabled(!isNull);
    ui->ctrlGainsLabel->setEnabled(!isNull);
    ui->ctrlGains->setEnabled(!isNull);
    ui->ctrlLims->setEnabled(!isNull);
    ui->ctrlLimsLabel->setEnabled(!isNull);
    if (isNull) {
        ui->ctrlLabel->clear();
        ui->ctrlType->setCurrentIndex(0);
        ui->ctrlGains->clear();
        ui->ctrlLims->setCurrentIndex(0);
        return;
    }

    setQComboBox(ui->ctrlType,current->text(ctrlCols::ctrlColType));
    ui->ctrlLabel->setText(current->text(ctrlCols::ctrlColLabel));
    setQComboBox(ui->ctrlLims,current->text(ctrlCols::ctrlColLims));

    QString ctrlType = ctrlTypes.key(current->text(ctrlCols::ctrlColType));

    ui->ctrlGains->clear();
    ui->ctrlGains->addItems(dsm_gui_lib::sortStringList(ctrlValidGains[ctrlType]));
    setQComboBox(ui->ctrlGains,current->text(ctrlCols::ctrlColGains));
}

void DSM_Menu::on_ctrlConfigTree_itemChanged(QTreeWidgetItem *item, int column) {
    if (item==NULL)
        return;
    QString text = item->text(column);

    if (text.isEmpty()) {
        item->setBackground(column,badTextBrush);
        return;
    }

    QListWidgetItem *checkItem;
    QString ctrlType = item->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString();

    switch (column) {
    case ctrlCols::ctrlColGains:
        checkItem = ui->gainList->findItems(item->text(column),Qt::MatchExactly).at(0);
        if (allowableGains[ctrlType].contains(checkItem->data(gainsData::gainsType).toString()))
            item->setBackground(column,okTextBrush);
        else
            item->setBackground(column,badTextBrush);
        break;
    case ctrlCols::ctrlColLims:
        if (!item->text(column).isEmpty())
            item->setBackground(column,okTextBrush);
        break;
    default:
        break;
    }
}

void DSM_Menu::on_ctrlType_textActivated(const QString &arg1) {
    QTreeWidgetItem *current = ui->ctrlConfigTree->currentItem();
    if (current == NULL)
        return;

    QString oldType = current->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString();
    QString label = current->text(ctrlCols::ctrlColLabel);

    for (int cmd : ctrlValidCmds[oldType])
        cmdValidCtrls[cmd].removeOne(label);

    QString newType = ctrlTypes.key(arg1);
    current->setData(ctrlCols::ctrlColLabel,ctrlData::ctrlType,newType);
    for (int cmd : ctrlValidCmds[newType])
        cmdValidCtrls[cmd].append(label);

    QList<int> checkCmds = ctrlValidCmds[oldType];
    for (int cmd: ctrlValidCmds[newType]) {
        if (!checkCmds.contains(cmd))
            checkCmds.append(cmd);
    }

    for (QTreeWidgetItem *item : ui->cmdConfigTree->findItems(label,Qt::MatchExactly|Qt::MatchRecursive,cmdCols::cmdColCtl))
        on_cmdConfigTree_itemChanged(item,cmdCols::cmdColCtl);//revalid controller type

    ui->ctrlGains->clear();
    ui->ctrlGains->addItems(dsm_gui_lib::sortStringList(ctrlValidGains[newType]));
    if (ctrlValidGains[newType].contains(current->text(ctrlCols::ctrlColGains)))
        setQComboBox(ui->ctrlGains,current->text(ctrlCols::ctrlColGains));
    on_ctrlConfigTree_itemChanged(current, ctrlCols::ctrlColGains);
}

void DSM_Menu::on_ctrlLabel_textEdited(const QString &arg1) {
    QTreeWidgetItem *current = ui->ctrlConfigTree->currentItem();
    if (current == NULL)
        return;

    int itemNum=current->data(ctrlCols::ctrlColLabel,ctrlData::ctrlNum).toInt();
    QString value = entryItemFormat(dsmSectionTypes::CONTROLLERS).arg(itemNum);
    QString oldKey = ctlsHash.key(value);

    if (arg1.isEmpty() || ctlsHash.keys().contains(arg1)) {
        ui->ctrlLabel->setPalette(badTextPalette);
        return;
    }
    else
        ui->ctrlLabel->setPalette(okTextPalette);

    ctlsHash.remove(oldKey);
    ctlsHash.insert(arg1,value);

    QString ctlType = current->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString();

    current->setText(ctrlCols::ctrlColLabel,arg1);

    for (int cmd : ctrlValidCmds[ctlType]) {
        QStringList *validCtlList = &cmdValidCtrls[cmd];
        validCtlList->replace(validCtlList->indexOf(oldKey),arg1);
        for (int i=0; i<entryCmdParents[cmd]->childCount(); i++) {
            QTreeWidgetItem* item = entryCmdParents[cmd]->child(i);
            if (item->text(cmdCols::cmdColCtl).compare(oldKey)==0) {
                item->setText(cmdCols::cmdColCtl,arg1);
            }
        }
    }
    populate_cmd_dropdowns();
}

void DSM_Menu::on_ctrlGains_textActivated(const QString &arg1) {
    QTreeWidgetItem *current = ui->ctrlConfigTree->currentItem();
    if (current == NULL)
        return;

    current->setText(ctrlCols::ctrlColGains,arg1);
}

void DSM_Menu::on_ctrlLims_textActivated(const QString &arg1) {
    QTreeWidgetItem *current = ui->ctrlConfigTree->currentItem();
    if (current == NULL)
        return;

    current->setText(ctrlCols::ctrlColLims,arg1);
}

void DSM_Menu::on_ctrlRemove_clicked() {
    QTreeWidgetItem *current = ui->ctrlConfigTree->currentItem();
    if (current==NULL)
        return;

    QString label = current->text(ctrlCols::ctrlColLabel);
    QString ctlType = current->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString();

    for (int cmd : ctrlValidCmds[ctlType]) {
        for (int i=0 ; i<entryCmdParents[cmd]->childCount(); i++) {
            QTreeWidgetItem* item = entryCmdParents[cmd]->child(i);
            if (item->text(cmdCols::cmdColCtl).compare(label)==0) {
                item->setText(cmdCols::cmdColCtl,"");
            }
        }
    }

    for (int cmd : actValidCmds[ctlType])
        cmdValidCtrls[cmd].removeOne(label);

    delete current;
    ui->ctrlConfigTree->setCurrentItem(NULL);
    populate_cmd_dropdowns();
}

void DSM_Menu::on_ctrlAdd_clicked() {

    QList<int> usedCmdInds;
    QStringList usedLabels;
    for (int i=0; i<ui->ctrlConfigTree->topLevelItemCount(); i++) {
        usedCmdInds.append(ui->ctrlConfigTree->topLevelItem(i)->data(ctrlCols::ctrlColLabel,ctrlData::ctrlNum).toInt());
        usedLabels.append(ui->ctrlConfigTree->topLevelItem(i)->text(cmdCols::cmdColLabel));
    }

    int newInd;
    QString newLabel;
    for (newInd=0; newInd<5e3; newInd++) {
        newLabel = entryItemFormat(dsmSectionTypes::CONTROLLERS).arg(newInd);
        if (!usedCmdInds.contains(newInd)&&!usedLabels.contains(newLabel)) break;
    }

    QString newType;
    QString gains;
    for (QString testNewType : ctrlValidGains.keys()) {
        if (!ctrlValidGains[testNewType].isEmpty()) {
            newType = testNewType;
            gains = ctrlValidGains[testNewType].at(0);
            gains = entryItemFormat(dsmSectionTypes::GAINS).arg(ui->gainList->findItems(gains,Qt::MatchExactly).at(0)->data(gainsData::gainsNum).toInt());
            break;
        }
    }
    if (newType.isEmpty()) {
        dsm_gui_lib::error_message("You must first define a valid set of Gains.");
        return;
    }

    if (ui->limList->count()==0) {
        dsm_gui_lib::error_message("You must first define a valid set of Limits.");
        return;
    }

    QString lims = entryItemFormat(dsmSectionTypes::LIMITS).arg(ui->limList->item(0)->data(limData::limNum).toInt());

    new_entry_item(dsmSectionTypes::CONTROLLERS,newLabel,newInd,newType+cmdDataSpacer+gains+cmdDataSpacer+lims);
    populate_cmd_dropdowns();
}

void DSM_Menu::on_ctrlDuplicate_clicked() {
    QTreeWidgetItem *curItem = ui->ctrlConfigTree->currentItem();
    if (curItem == NULL)
        return;

    QList <int> usedCmdInds;
    QStringList usedLabels;
    for (int i=0; i<ui->ctrlConfigTree->topLevelItemCount(); i++) {
        usedCmdInds.append(ui->ctrlConfigTree->topLevelItem(i)->data(ctrlCols::ctrlColLabel,ctrlData::ctrlNum).toInt());
        usedLabels.append(ui->ctrlConfigTree->topLevelItem(i)->text(ctrlCols::ctrlColLabel));
    }

    int newInd;
    for (newInd=0; newInd<5e3; newInd++)
        if (!usedCmdInds.contains(newInd)) break;

    QString newLabel = curItem->text(ctrlCols::ctrlColLabel)+"_Copy";
    for (int i=0;i<5e3; i++) {
        QString newLabelTest = newLabel;
        if (i>0) newLabelTest += "_"+QString::number(i);
        if (!usedLabels.contains(newLabelTest,Qt::CaseInsensitive)) {
            newLabel = newLabelTest;
            break;
        }
    }

    QString newType = ctrlTypes.key(curItem->text(ctrlCols::ctrlColType));
    QString gains = curItem->text(ctrlCols::ctrlColGains);
    gains = entryItemFormat(dsmSectionTypes::GAINS).arg(ui->gainList->findItems(gains,Qt::MatchExactly).at(0)->data(gainsData::gainsNum).toInt());
    QString lims = curItem->text(ctrlCols::ctrlColLims);
    lims = entryItemFormat(dsmSectionTypes::LIMITS).arg(ui->limList->findItems(lims,Qt::MatchExactly).at(0)->data(limData::limNum).toInt());

    new_entry_item(dsmSectionTypes::CONTROLLERS,newLabel,newInd,newType+cmdDataSpacer+gains+cmdDataSpacer+lims);

    populate_cmd_dropdowns();
}

void DSM_Menu::on_gainList_currentItemChanged(QListWidgetItem *current, QListWidgetItem*) {
    bool isNull = current == NULL;
    ui->gainType->setEnabled(!isNull);
    ui->gainTypeLabel->setEnabled(!isNull);
    ui->gainLabel->setEnabled(!isNull);
    ui->gainLabelLabel->setEnabled(!isNull);
    if (isNull) {
        ui->gainType->setCurrentIndex(0);
        ui->gainLabel->clear();
        return;
    }

    QString gainType = current->data(gainsData::gainsType).toString();
    for (int i=0; i<ui->gainConfigurator->count(); i++) {
        QString tabText = ui->gainConfigurator->tabText(i);
        ui->gainConfigurator->setTabVisible(i,gainType.compare(gainsTypes.key(tabText))==0);
    }

    setQComboBox(ui->gainType,gainsTypes[gainType]);

    QStringList dataSplit = current->data(gainsData::gainsData).toString().split(QRegExp("\\s"),Qt::SkipEmptyParts);

    const int bufferSize = 16;
    if (dataSplit.length() < bufferSize) {
        for (int i = 0; i<(bufferSize-dataSplit.length()); i++)
            dataSplit.append("0.0");
    }

    ui->gainLabel->setText(current->text());
    if (gainType.compare("PID")==0) {
        ui->gainKpX->setText(dataSplit[1]);
        ui->gainKpY->setText(dataSplit[2]);
        ui->gainKpZ->setText(dataSplit[3]);
        ui->gainKrX->setText(dataSplit[5]);
        ui->gainKrY->setText(dataSplit[6]);
        ui->gainKrZ->setText(dataSplit[7]);
        ui->gainKiX->setText(dataSplit[9]);
        ui->gainKiY->setText(dataSplit[10]);
        ui->gainKiZ->setText(dataSplit[11]);
        ui->gainKiLimX->setText(dataSplit[13]);
        ui->gainKiLimY->setText(dataSplit[14]);
        ui->gainKiLimZ->setText(dataSplit[15]);
    }
    else if (gainType.compare("PID_WN")==0) {
        ui->gainBandwidth->setText(dataSplit[0]);
        ui->gainDampingRatio->setText(dataSplit[1]);
        ui->gainAlpha->setText(dataSplit[2]);
        ui->gainILim->setText(dataSplit[3]);
    }
    else if (gainType.compare("MomentumDump")==0) {
        ui->gainMomDumpKpX->setText(dataSplit[0]);
        ui->gainMomDumpKpY->setText(dataSplit[1]);
        ui->gainMomDumpKpZ->setText(dataSplit[2]);
    }
    else if (gainType.compare("FC_LYA")==0) {
        ui->gainLyaGain->setText(dataSplit[0]);
        ui->gainLyaGain_2->setText(dataSplit[1]);
    }
    else
        dsm_gui_lib::inexplicable_error_message();
}

void DSM_Menu::gain_data_changed() {
    QListWidgetItem *current  = ui->gainList->currentItem();
    if (current == NULL)
        return;

    QStringList data;
    if (current->data(gainsData::gainsType).toString().compare("PID")==0) {
        data.append("Kp");
        data.append(ui->gainKpX->text());
        data.append(ui->gainKpY->text());
        data.append(ui->gainKpZ->text());
        data.append("Kr");
        data.append(ui->gainKrX->text());
        data.append(ui->gainKrY->text());
        data.append(ui->gainKrZ->text());
        data.append("Ki");
        data.append(ui->gainKiX->text());
        data.append(ui->gainKiY->text());
        data.append(ui->gainKiZ->text());
        data.append("Ki_Limit");
        data.append(ui->gainKiLimX->text());
        data.append(ui->gainKiLimY->text());
        data.append(ui->gainKiLimZ->text());
    }
    else if (current->data(gainsData::gainsType).toString().compare("PID_WN")==0) {
        data.append(ui->gainBandwidth->text());
        data.append(ui->gainDampingRatio->text());
        data.append(ui->gainAlpha->text());
        data.append(ui->gainILim->text());
    }
    else if (current->data(gainsData::gainsType).toString().compare("MomentumDump")==0) {
        data.append(ui->gainMomDumpKpX->text());
        data.append(ui->gainMomDumpKpY->text());
        data.append(ui->gainMomDumpKpZ->text());
    }
    else if (current->data(gainsData::gainsType).toString().compare("FC_LYA")==0) {
        data.append(ui->gainLyaGain->text());
        data.append(ui->gainLyaGain_2->text());
    }
    else
        dsm_gui_lib::inexplicable_error_message();

    current->setData(gainsData::gainsData,data.join(cmdDataSpacer));
}

void DSM_Menu::on_gainType_textActivated(const QString &arg1) {
    QListWidgetItem *current  = ui->gainList->currentItem();
    if (current == NULL)
        return;

    QString newType = gainsTypes.key(arg1);
    QString oldType = current->data(gainsData::gainsType).toString();
    QString label = current->text();
    if (newType.compare(oldType)==0)
        return;

    QStringList data;
    if (newType.compare("PID")==0) {
        data.append("Kp");
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
        data.append("Kr");
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
        data.append("Ki");
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
        data.append("Ki_Limit");
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
    }
    else if (newType.compare("PID_WN")==0) {
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
    }
    else if (newType.compare("MomentumDump")==0) {
        data.append("0.0");
        data.append("0.0");
        data.append("0.0");
    }
    else if (newType.compare("FC_LYA")==0) {
        data.append("0.0");
        data.append("0.0");
    }
    else
        dsm_gui_lib::inexplicable_error_message();

    current->setData(gainsData::gainsType,newType);
    current->setData(gainsData::gainsData,data.join(cmdDataSpacer));

    QTreeWidgetItem *currentCtrl = ui->ctrlConfigTree->currentItem();
    QStringList ctrlValidGainTypes;
    if (currentCtrl!=NULL)
        ctrlValidGainTypes = ctrlValidGains[currentCtrl->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString()];

    for (QString ctrl : allowableCtrl[oldType])
        ctrlValidGains[ctrl].removeOne(label);

    for (QString ctrl : allowableCtrl[newType])
        ctrlValidGains[ctrl].append(label);

    if (currentCtrl!=NULL) {
        if (ctrlValidGainTypes.contains(oldType) || ctrlValidGainTypes.contains(newType)) {
            ui->ctrlGains->clear();
            ui->ctrlGains->addItems(ctrlValidGains[currentCtrl->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString()]);
            setQComboBox(ui->ctrlGains,label);
        }
    }

    for (QTreeWidgetItem *item : ui->ctrlConfigTree->findItems("*",Qt::MatchWildcard)) {
        if (label.compare(item->text(ctrlCols::ctrlColGains))==0) {
            QString ctrlType = item->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString();
            if (ctrlValidGains[ctrlType].contains(label))
                item->setBackground(ctrlCols::ctrlColGains,badTextBrush);
            else
                item->setBackground(ctrlCols::ctrlColGains,okTextBrush);
        }
    }
}

void DSM_Menu::on_gainLabel_textEdited(const QString &arg1) {
    QListWidgetItem *current  = ui->gainList->currentItem();
    if (current == NULL)
        return;

    int itemNum = current->data(gainsData::gainsNum).toInt();
    QString value = entryItemFormat(dsmSectionTypes::GAINS).arg(itemNum);
    QString oldKey = gainsHash.key(value);

    if (arg1.isEmpty() || gainsHash.keys().contains(arg1)) {
        ui->gainLabel->setPalette(badTextPalette);
        return;
    }
    else
        ui->gainLabel->setPalette(okTextPalette);

    gainsHash.remove(oldKey);
    gainsHash.insert(arg1,value);

    QString gainType = current->data(gainsData::gainsType).toString();
    current->setText(arg1);

    for (QString ctrlType : allowableCtrl[gainType]) {
        QStringList *validGainList = &ctrlValidGains[ctrlType];
        validGainList->replace(validGainList->indexOf(oldKey),arg1);
    }
    for (QTreeWidgetItem *item : ui->ctrlConfigTree->findItems(oldKey,Qt::MatchExactly|Qt::MatchRecursive,ctrlCols::ctrlColGains))
        item->setText(ctrlCols::ctrlColGains,arg1);

    populate_ctrl_dropdowns();
}

void DSM_Menu::on_gainRemove_clicked() {
    QListWidgetItem *current = ui->gainList->currentItem();
    if (current==NULL)
        return;

    QString label = current->text();
    QString type = current->data(gainsData::gainsType).toString();
    for (QTreeWidgetItem *item :ui->ctrlConfigTree->findItems(label,Qt::MatchExactly,ctrlCols::ctrlColGains))
        item->setText(ctrlCols::ctrlColGains,"");

    for (QString ctrlType : allowableCtrl[type])
        ctrlValidGains[ctrlType].removeOne(label);

    delete current;
    ui->gainList->setCurrentItem(NULL);

    populate_ctrl_dropdowns();
}

void DSM_Menu::on_gainAdd_clicked() {
    QList<int> usedInds;
    QStringList usedLabels;
    for (int i=0; i<ui->gainList->count(); i++) {
        usedInds.append(ui->gainList->item(i)->data(gainsData::gainsNum).toInt());
        usedLabels.append(ui->gainList->item(i)->text());
    }

    int newInd;
    QString newLabel;
    for (newInd=0; newInd<5e3; newInd++) {
        newLabel = entryItemFormat(dsmSectionTypes::GAINS).arg(newInd);
        if (!usedInds.contains(newInd) && !usedLabels.contains(newLabel)) break;
    }

    QStringList data;
    data.append("PID_WN");
    data.append("0.0");
    data.append("0.0");
    data.append("0.0");
    data.append("0.0");

    new_entry_item(dsmSectionTypes::GAINS,newLabel,newInd,data.join(cmdDataSpacer));

    populate_ctrl_dropdowns();
}

void DSM_Menu::on_gainDuplicate_clicked() {
    QListWidgetItem *current = ui->gainList->currentItem();
    if (current == NULL)
        return;

    QList<int> usedInds;
    QStringList usedLabels;
    for (int i=0; i<ui->gainList->count(); i++) {
        usedInds.append(ui->gainList->item(i)->data(gainsData::gainsNum).toInt());
        usedLabels.append(ui->gainList->item(i)->text());
    }

    int newInd;
    for (newInd=0;newInd<5e3;newInd++)
        if (!usedInds.contains(newInd)) break;

    QString newLabel = current->text()+"_Copy";
    for (int i=0;i<5e3; i++) {
        QString newLabelTest = newLabel;
        if (i>0) newLabelTest += "_"+QString::number(i);
        if (!usedLabels.contains(newLabelTest,Qt::CaseInsensitive)) {
            newLabel = newLabelTest;
            break;
        }
    }

    new_entry_item(dsmSectionTypes::GAINS,newLabel,newInd,current->data(gainsData::gainsData).toString());

    populate_ctrl_dropdowns();
}

void DSM_Menu::populate_ctrl_dropdowns() {
    QTreeWidgetItem *curCtrl = ui->ctrlConfigTree->currentItem();

    ui->ctrlLims->clear();
    QStringList lims = dsm_gui_lib::getTextFromList(ui->limList);
    lims.sort();
    ui->ctrlLims->addItems(lims);

    if (curCtrl!=NULL) {
        ui->ctrlGains->clear();
        QStringList validGains = ctrlValidGains[curCtrl->data(ctrlCols::ctrlColLabel,ctrlData::ctrlType).toString()];
        validGains.sort();
        ui->ctrlGains->addItems(validGains);
        setQComboBox(ui->ctrlGains,curCtrl->text(ctrlCols::ctrlColGains));

        setQComboBox(ui->ctrlLims,curCtrl->text(ctrlCols::ctrlColLims));
    }

}

void DSM_Menu::on_limList_currentItemChanged(QListWidgetItem *current, QListWidgetItem*){
    bool isNull = current == NULL;
    ui->limLabel->setEnabled(!isNull);
    ui->limLabelLabel->setEnabled(!isNull);
    ui->limFrcLabel->setEnabled(!isNull);
    ui->limFrcX->setEnabled(!isNull);
    ui->limFrcY->setEnabled(!isNull);
    ui->limFrcZ->setEnabled(!isNull);
    ui->limRateLabel->setEnabled(!isNull);
    ui->limRateX->setEnabled(!isNull);
    ui->limRateY->setEnabled(!isNull);
    ui->limRateZ->setEnabled(!isNull);
    if (isNull) {
        ui->limLabel->clear();
        ui->limFrcX->clear();
        ui->limFrcY->clear();
        ui->limFrcZ->clear();
        ui->limRateX->clear();
        ui->limRateY->clear();
        ui->limRateZ->clear();
        return;
    }

    QStringList dataSplit = current->data(limData::limData).toString().split(QRegExp("\\s"),Qt::SkipEmptyParts);

    const int bufferSize = 6;
    if (dataSplit.length() < bufferSize) {
        for (int i = 0; i<(bufferSize-dataSplit.length()); i++)
            dataSplit.append("0.0");
    }

    ui->limLabel->setText(current->text());
    ui->limFrcX->setText(dataSplit[0]);
    ui->limFrcY->setText(dataSplit[1]);
    ui->limFrcZ->setText(dataSplit[2]);
    ui->limRateX->setText(dataSplit[3]);
    ui->limRateY->setText(dataSplit[4]);
    ui->limRateZ->setText(dataSplit[5]);
}

void DSM_Menu::lim_data_changed() {
    QListWidgetItem *current = ui->limList->currentItem();
    if (current == NULL)
        return;

    QStringList data;
    data.append(ui->limFrcX->text());
    data.append(ui->limFrcY->text());
    data.append(ui->limFrcZ->text());
    data.append(ui->limRateX->text());
    data.append(ui->limRateY->text());
    data.append(ui->limRateZ->text());
    current->setData(limData::limData,data.join(cmdDataSpacer));
}

void DSM_Menu::on_limLabel_textEdited(const QString &arg1) {
    QListWidgetItem *current = ui->limList->currentItem();
    if (current == NULL)
        return;

    int itemNum = current->data(limData::limNum).toInt();
    QString value = entryItemFormat(dsmSectionTypes::LIMITS).arg(itemNum);
    QString oldKey = limsHash.key(value);

    if (arg1.isEmpty() || limsHash.keys().contains(arg1)) {
        ui->limLabel->setPalette(badTextPalette);
        return;
    }
    else
        ui->limLabel->setPalette(okTextPalette);

    limsHash.remove(oldKey);
    limsHash.insert(arg1,value);

    current->setText(arg1);
    for (QTreeWidgetItem *item : ui->ctrlConfigTree->findItems(oldKey,Qt::MatchExactly|Qt::MatchRecursive,ctrlCols::ctrlColLims))
        item->setText(ctrlCols::ctrlColLims,arg1);

    populate_ctrl_dropdowns();
}

void DSM_Menu::on_limRemove_clicked() {
    QListWidgetItem *current = ui->limList->currentItem();
    if (current==NULL)
        return;

    QString label = current->text();
    for (QTreeWidgetItem *item : ui->ctrlConfigTree->findItems(label,Qt::MatchExactly|Qt::MatchRecursive,ctrlCols::ctrlColLims))
        item->setText(ctrlCols::ctrlColLims,"");
    delete current;
    limsHash.remove(label);
    populate_ctrl_dropdowns();
}

void DSM_Menu::on_limAdd_clicked() {
    QList<int> usedInds;
    QStringList usedLabels;
    for (int i=0; i<ui->limList->count(); i++) {
        usedInds.append(ui->limList->item(i)->data(limData::limNum).toInt());
        usedLabels.append(ui->limList->item(i)->text());
    }

    int newInd;
    QString newLabel;
    for (newInd=0; newInd<5e3; newInd++) {
        newLabel = entryItemFormat(dsmSectionTypes::LIMITS).arg(newInd);
        if (!usedInds.contains(newInd) && !usedLabels.contains(newLabel)) break;
    }

    QStringList data;
    data.append("0.0");
    data.append("0.0");
    data.append("0.0");
    data.append("0.0");
    data.append("0.0");
    data.append("0.0");

    new_entry_item(dsmSectionTypes::LIMITS,newLabel,newInd,data.join(cmdDataSpacer));

    populate_ctrl_dropdowns();

}

void DSM_Menu::on_limDuplicate_clicked() {
    QListWidgetItem *current = ui->limList->currentItem();
    if (current==NULL)
        return;

    QList<int> usedInds;
    QStringList usedLabels;
    for (int i=0; i<ui->limList->count(); i++) {
        usedInds.append(ui->limList->item(i)->data(limData::limNum).toInt());
        usedLabels.append(ui->limList->item(i)->text());
    }

    int newInd;
    for (newInd=0;newInd<5e3;newInd++)
        if (!usedInds.contains(newInd)) break;

    QString newLabel = current->text()+"_Copy";
    for (int i=0;i<5e3; i++) {
        QString newLabelTest = newLabel;
        if (i>0) newLabelTest += "_"+QString::number(i);
        if (!usedLabels.contains(newLabelTest,Qt::CaseInsensitive)) {
            newLabel = newLabelTest;
            break;
        }
    }

    new_entry_item(dsmSectionTypes::LIMITS,newLabel,newInd,current->data(limData::limData).toString());
    populate_ctrl_dropdowns();




}


void DSM_Menu::on_applyButton_clicked() {
    static QRegularExpression rxPV("^((?(?=.*"+QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+".*).*(?="
                                   +QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+")|.*))");
    static QRegularExpression rxSV(QRegularExpression::escape(cmdDelimiter+cmdDataSpacer)+"(.*)$");

    dsmUpdate.append("<<<<<<<<<<<<<<<<<<<<<<<<  42:  DSM Command Script File  >>>>>>>>>>>>>>>>>>>>>>>>");

    // write to file in reverse search order
    for (auto i = searchOrd.crbegin(); i!= searchOrd.crend(); i++) {
        dsmSectionTypes type = *i;
        dsmUpdate.append(secDescription(type));
        dsmUpdate.append("");

        QRegularExpressionMatch match;
        QList<QTreeWidgetItem*> treeItems, checkItems;
        QTreeWidgetItem *treeItem,*parent,*checkItem;
        QList<QListWidgetItem*> listItems;
        QListWidgetItem* listItem;
        QString format = entryItemFormat(type);
        QStringList dataList, tmpList;
        QString data,tmp,label;
        int num, cmdType;
        double time, lastTime=0;

        switch (type) {
        case dsmSectionTypes::COMMANDS:
            ui->cmdTimelineTree->sortByColumn(tlCols::tlColSC,Qt::AscendingOrder);
            ui->cmdTimelineTree->sortByColumn(tlCols::tlColTime,Qt::AscendingOrder);
            treeItems = ui->cmdTimelineTree->findItems("*",Qt::MatchWildcard,tlCols::tlColSC);
            label = format+cmdDataSpacer+"CmdTime"+cmdDataSpacer+"%2";

            for (QTreeWidgetItem *cmdItem : qAsConst(treeItems)) {
                dataList.clear();

                time = cmdItem->data(tlCols::tlColTime,Qt::DisplayRole).toDouble();
                dataList.append(label.arg(scNames.indexOf(cmdItem->text(tlCols::tlColSC))).arg(time,15,'f',3));

                num = 0;
                tmpList.clear();
                if (!cmdItem->text(tlCols::tlColTrn).isEmpty()) {
                    num++;
                    data = cmdItem->text(tlCols::tlColTrn);
                    tmpList.append(trnCmdsHash[data]);
                }
                if (!cmdItem->text(tlCols::tlColAtt).isEmpty()) {
                    num++;
                    tmp.clear();
                    data = rxPV.match(cmdItem->text(tlCols::tlColAtt)).captured(1);
                    tmp.append(attCmdsHash[data]);
                    data = rxSV.match(cmdItem->text(tlCols::tlColAtt)).captured(1);
                    if (!data.isEmpty()) {
                        checkItems = ui->cmdConfigTree->findItems(data,Qt::MatchExactly|Qt::MatchRecursive,cmdCols::cmdColLabel);
                        for (QTreeWidgetItem *checkItem : qAsConst(checkItems)) {
                            cmdType = entryCmdParents.key(checkItem->parent());
                            if (cmdType==cmdTypes::cmdSV) {
                                tmp.append("_SV["+checkItem->data(cmdCols::cmdColLabel,cmdData::cmdNum).toString()+"]");
                                break;
                            }
                        }

                    }
                    tmpList.append(tmp);
                }
                if (!cmdItem->text(tlCols::tlColAct).isEmpty()) {
                    num++;
                    data = cmdItem->text(tlCols::tlColAct);
                    tmpList.append(actCmdsHash[data]);
                }
                dataList.append("NUM_CMD["+QString::number(num)+"]");
                dataList.append(tmpList);
                if (num>0) {
                    if (time>lastTime)
                        dsmUpdate.append("");
                    dsmUpdate.append(dataList.join(cmdDataSpacer));
                    lastTime=time;
                }
            }
            dsmUpdate.append("");
            dsmUpdate.append("End_Of_File");
            break;
        case dsmSectionTypes::CONTROLLERS:
            treeItems = ui->ctrlConfigTree->findItems("*",Qt::MatchWildcard,ctrlCols::ctrlColLabel);

            for (QTreeWidgetItem *ctrlItem: qAsConst(treeItems)) {
                dataList.clear();

                label = ctrlItem->text(ctrlCols::ctrlColLabel);
                dataList.append(ctlsHash[label]);
                dataList.append(ctrlTypes.key(ctrlItem->text(ctrlCols::ctrlColType)));

                data = ctrlItem->text(ctrlCols::ctrlColGains);
                if (data.isEmpty()) {
                    dsm_gui_lib::error_message("Gains are missing for Controller: "+label);
                    dsmUpdate.clear();
                    return;
                }
                if (ctrlItem->background(ctrlCols::ctrlColGains) == badTextBrush) {
                    dsm_gui_lib::error_message("Controller """+label+""" has invalid gains: "+data);
                    dsmUpdate.clear();
                    return;
                }

                listItem = ui->gainList->findItems(data,Qt::MatchExactly).at(0);
                dataList.append(entryItemFormat(dsmSectionTypes::GAINS).arg(listItem->data(gainsData::gainsNum).toInt()));

                data = ctrlItem->text(ctrlCols::ctrlColLims);
                if (data.isEmpty()) {
                    dsm_gui_lib::error_message("Limits are missing for Controller: "+label);
                    dsmUpdate.clear();
                    return;
                }
                if (ctrlItem->background(ctrlCols::ctrlColLims) == badTextBrush) {
                    dsm_gui_lib::error_message("Controller """+label+""" has invalid Limits: "+data);
                    dsmUpdate.clear();
                    // This really shouldn't happen except in the case of missing text
                    dsm_gui_lib::inexplicable_error_message();
                    return;
                }

                listItem = ui->limList->findItems(data,Qt::MatchExactly).at(0);
                dataList.append(entryItemFormat(dsmSectionTypes::LIMITS).arg(listItem->data(limData::limNum).toInt()));

                dataList.append(labelMkr);
                dataList.append(label);

                dsmUpdate.append(dataList.join(cmdDataSpacer));
            }

            break;
        case dsmSectionTypes::ACTUATORS:
            listItems = ui->actList->findItems("*",Qt::MatchWildcard);
            for (QListWidgetItem *item : qAsConst(listItems)) {
                dataList.clear();
                label = item->text();
                dataList.append(actsHash[label]);
                dataList.append(item->data(actData::actType).toString());

                dataList.append(labelMkr);
                dataList.append(label);

                dsmUpdate.append(dataList.join(cmdDataSpacer));
            }

            break;
        case dsmSectionTypes::GAINS:
            listItems = ui->gainList->findItems("*",Qt::MatchWildcard);
            for (QListWidgetItem *item : qAsConst(listItems)) {
                dataList.clear();
                label = item->text();
                dataList.append(gainsHash[label]);
                dataList.append(item->data(gainsData::gainsType).toString());
                dataList.append(item->data(gainsData::gainsData).toString());

                dataList.append(labelMkr);
                dataList.append(label);

                dsmUpdate.append(dataList.join(cmdDataSpacer));
            }
            break;
        case dsmSectionTypes::LIMITS:
            listItems = ui->limList->findItems("*",Qt::MatchWildcard);
            for (QListWidgetItem *item : qAsConst(listItems)) {
                dataList.clear();
                label = item->text();
                dataList.append(limsHash[label]);
                dataList.append(item->data(limData::limData).toString());

                dataList.append(labelMkr);
                dataList.append(label);

                dsmUpdate.append(dataList.join(cmdDataSpacer));
            }
            break;
        default: // handle commands
            cmdType = section2Cmd[type];
            parent = entryCmdParents[cmdType];
            for (int i = 0; i<parent->childCount(); i++) {
                treeItem = parent->child(i);
                dataList.clear();

                label = treeItem->text(cmdCols::cmdColLabel);
                dataList.append(format.arg(treeItem->data(cmdCols::cmdColLabel,cmdData::cmdNum).toInt()));
                dataList.append(treeItem->text(cmdCols::cmdColData));

                data = treeItem->text(cmdCols::cmdColCtl);
                if (treeItem->background(cmdCols::cmdColCtl)==badTextBrush) {
                    dsm_gui_lib::error_message("Command """+label+""" has invalid Controller: "+data);
                    dsmUpdate.clear();
                    return;
                }
                if (!data.isEmpty()) {
                    checkItem = ui->ctrlConfigTree->findItems(data,Qt::MatchExactly,ctrlCols::ctrlColLabel).at(0);
                    dataList.append(entryItemFormat(dsmSectionTypes::CONTROLLERS).arg(checkItem->data(ctrlCols::ctrlColLabel,ctrlData::ctrlNum).toInt()));
                }

                data = treeItem->text(cmdCols::cmdColAct);
                if (treeItem->background(cmdCols::cmdColCtl)==badTextBrush) {
                    dsm_gui_lib::error_message("Command """+label+""" has invalid Actuator: "+data);
                    dsmUpdate.clear();
                    return;
                }
                if (!data.isEmpty()) {
                    listItem = ui->actList->findItems(data,Qt::MatchExactly).at(0);
                    dataList.append(entryItemFormat(dsmSectionTypes::ACTUATORS).arg(listItem->data(actData::actNum).toInt()));
                }

                dataList.append(labelMkr);
                dataList.append(label);

                dsmUpdate.append(dataList.join(cmdDataSpacer));
            }
            break;
        }
        dsmUpdate.append("");
    }

    dsmUpdate.append("");
    dsmUpdate.append("EOF");

    for (int i =0; i<dsmUpdate.length(); i++)
        dsmUpdate[i].append("\n");
    write_data();

}

void DSM_Menu::on_cmdQuatNormalize_clicked() {
    QVector4D quat;
    quat[0] = ui->cmdQv1->text().toDouble();
    quat[1] = ui->cmdQv2->text().toDouble();
    quat[2] = ui->cmdQv3->text().toDouble();
    quat[3] = ui->cmdQs->text().toDouble();

    if (quat.length()<std::numeric_limits<double>::epsilon()) {
        dsm_gui_lib::inexplicable_error_message();
        return;
    }
    if (abs(quat[3])>std::numeric_limits<double>::epsilon()) {
        quat = quat[3]*quat/abs(quat[3]);
    }
    quat.normalize();

    ui->cmdQv1->setText(QString::number(quat[0]));
    ui->cmdQv2->setText(QString::number(quat[1]));
    ui->cmdQv3->setText(QString::number(quat[2]));
    ui->cmdQs->setText(QString::number(quat[3]));
}
