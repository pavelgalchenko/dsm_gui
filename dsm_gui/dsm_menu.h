#ifndef DSM_MENU_H
#define DSM_MENU_H

#include "qtreewidget.h"
#include <dsm_gui_lib.h>

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>

namespace Ui {
class DSM_Menu;
}

class DSM_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit DSM_Menu(QWidget *parent = nullptr);
    ~DSM_Menu();

private slots:
    void set_validators();
    void receive_dsmpath(QString);
    void receive_data();
    void apply_data();
    void write_data();

    void set_command_ctrl_act(QTreeWidgetItem*,const QString, QStringList*);

    void setQComboBox(QComboBox *comboBox, QString string);

    void on_cmdConfigTree_itemClicked(QTreeWidgetItem *item, int column);


    void on_cmdTrnOri_currentTextChanged(const QString &arg1);

    void on_cmdTrnFrm_currentTextChanged(const QString &arg1);

    void on_cmdPvTgtType_currentTextChanged(const QString &arg1);

    void on_cmdSvTgtType_currentTextChanged(const QString &arg1);

    void cmd_Trn_data_changed();
    void cmd_PV_data_changed();
    void cmd_SV_data_changed();
    void cmd_Quat_data_changed();
    void cmd_Mirror_data_changed();
    void cmd_Detumble_data_changed();
    void cmd_WhlHManage_data_changed();
    void cmd_Act_data_changed();
    void cmd_Maneuver_data_changed();

private:
    Ui::DSM_Menu *ui;

    QString inoutPath;
    QString filePath;
    QStringList dsmData;
    QStringList dsmString;
    QStringList dsmFileHeaders; // section headers in the file
    QStringList dsmFileDescrip; // data descriptors in the file
    QStringList dsmUpdate;

    QStringList scNames;


    /* Change these enums to change the column order in the QTreeWidgets */
    enum tlCols {
        tlColSC=0,
        tlColTime,
        tlColTrn,
        tlColAtt,
        tlColAct,
    };

    enum cmdCols {
        cmdColLabel=0,
        cmdColInd,
        cmdColAct,
        cmdColCtl,
        cmdColData,
    };

    enum ctrlCols {
        ctrlColLabel=0,
        ctrlColType,
        ctrlColGains,
        ctrlColLims,
    };
    /* END CHANGE ENUMS */

    /* Change these maps to change the column names in the QTreeWidgets */
    const QMap<int,QString> tlColNames = { {tlColSC,"Spacecraft"},
                                           {tlColTime,"Time [sec]"},
                                           {tlColTrn,"Translation Command"},
                                           {tlColAtt,"Attitude Command"},
                                           {tlColAct,"Actuator Command"}};

    const QMap<int,QString> cmdColNames = { {cmdColLabel,"Command/Label"},
                                            {cmdColAct,"Actuator"},
                                            {cmdColInd,"Command Index"},
                                            {cmdColCtl,"Controller"},
                                            {cmdColData,"Command Data"}};

    const QMap<int,QString> ctrlColNames = {{ctrlColLabel,"Controller/Label"},
                                             {ctrlColType,"Controller Type"},
                                             {ctrlColGains,"Gains"},
                                             {ctrlColLims,"Limits"}};
    /* END CHANGE MAPS */

    enum class dsmSectionTypes {
        COMMANDS,
        TRANSLATION,
        PRIMARY_VEC,
        SECONDARY_VEC,
        QUATERION,
        MIRROR,
        DETUMBLE,
        WHLHMANAGEMENT,
        ACTUATOR_CMD,
        CONTROLLERS,
        ACTUATORS,
        GAINS,
        LIMITS,
        MANEUVER,
    };

    const QVector<dsmSectionTypes> searchOrd = {dsmSectionTypes::LIMITS,dsmSectionTypes::GAINS,dsmSectionTypes::ACTUATORS,dsmSectionTypes::CONTROLLERS,
                                                dsmSectionTypes::MANEUVER,dsmSectionTypes::ACTUATOR_CMD,dsmSectionTypes::WHLHMANAGEMENT,dsmSectionTypes::DETUMBLE,
                                                dsmSectionTypes::MIRROR,dsmSectionTypes::QUATERION,dsmSectionTypes::SECONDARY_VEC,dsmSectionTypes::PRIMARY_VEC,
                                                dsmSectionTypes::TRANSLATION,dsmSectionTypes::COMMANDS};

    const QVector<dsmSectionTypes> cmdTypes = {dsmSectionTypes::TRANSLATION,dsmSectionTypes::PRIMARY_VEC,dsmSectionTypes::SECONDARY_VEC,dsmSectionTypes::QUATERION,
                                               dsmSectionTypes::MIRROR,dsmSectionTypes::DETUMBLE,dsmSectionTypes::WHLHMANAGEMENT,dsmSectionTypes::ACTUATOR_CMD,
                                               dsmSectionTypes::MANEUVER};

    QStringList secDescription(const dsmSectionTypes type);
    QString entryItemName(const dsmSectionTypes type);
    QRegularExpression entryItemRegEx(const dsmSectionTypes type);
    QString entryItemFormat(const dsmSectionTypes type);

    void new_entry_item(const dsmSectionTypes, QString, const int, const QString);
    QHash<int,QTreeWidgetItem*> entryCmdParents;

    enum timelineItemData {
        timelineSC = Qt::UserRole, // Might not need, might be able to reference parent item
        timelineTime,
        timelineTrnCmd,
        timelineAttCmd,
        timelineActCmd
    };

    enum cmdData {
        cmdType = Qt::UserRole, // Might not need, might be able to reference parent item
        cmdNum,
        cmdController,
        cmdActuator,
        cmdData
    };

    enum cmdTypes {
        cmdTrn=0,
        cmdPV,
        cmdSV,
        cmdQuat,
        cmdMirror,
        cmdDetumble,
        cmdWhlHManage,
        cmdAct,
        cmdManeuver,
        cmdPsvTrn,
        cmdPsvAtt,
        cmdAtt,
    };
    const QVector<int> trnCmds = {cmdPsvTrn,cmdTrn,cmdManeuver};
    const QVector<int> attCmds = {cmdPsvAtt,cmdAtt,cmdQuat,cmdMirror,cmdDetumble,cmdWhlHManage};

    const QMap<dsmSectionTypes,int> section2Cmd = { {dsmSectionTypes::TRANSLATION,cmdTrn},
                                                    {dsmSectionTypes::PRIMARY_VEC,cmdPV},
                                                    {dsmSectionTypes::SECONDARY_VEC,cmdSV},
                                                    {dsmSectionTypes::QUATERION,cmdQuat},
                                                    {dsmSectionTypes::MIRROR,cmdMirror},
                                                    {dsmSectionTypes::DETUMBLE,cmdDetumble},
                                                    {dsmSectionTypes::WHLHMANAGEMENT,cmdWhlHManage},
                                                    {dsmSectionTypes::ACTUATOR_CMD,cmdAct},
                                                    {dsmSectionTypes::MANEUVER,cmdManeuver}};

    const QHash<QString,QString> cmdAttTgtTypes = { {"VEC","Vector"},
                                                    {"SC","Spacecraft"},
                                                    {"BODY","World"}};

    const QHash<QString,QString> cmdAttTgtFrm = {{"N","Inertial"},
                                                 {"F","Formation"},
                                                 {"L","Local Vert"},
                                                 {"B","Body"}};

    const QHash<QString,QString> cmdManFrm = { {"N","Inertial"},
                                               {"B","Body"}};
    const QHash<QString,QString> cmdManTypes = {    {"CONSTANT","Constant Thrust"},
                                                    {"SMOOTHED","Smoothed Thrust"}};

    const QStringList worldInputs = {"SOL","MERCURY","VENUS","EARTH","MARS","JUPITER","SATURN","URANUS",
                                     "NEPTUNE","PLUTO","LUNA","PHOBOS","DEIMOS","IO","EUROPA","GANYMEDE","CALLISTO",
                                     "AMALTHEA","HIMALITA","ELARA","PASIPHAE","SINOPE","LYSITHEA","CARME","ANANKE",
                                     "LEDA","THEBE","ADRASTEA","METIS","MIMAS","ENCELADUS","TETHYS","DIONE","RHEA",
                                     "TITAN","HYPERION","IAPETUS","PHOEBE","JANUS","EPIMETHEUS","HELENE","TELESTO",
                                     "CALYPSO","ATLAS","PROMETHEUS","PANDORA","PAN","ARIEL","UMBRIEL",
                                     "TITANIA","OBERON","MIRANDA","TRITON","NERIED","CHARON","MINORBODY"};

    int getCmdType(QString);
    QRegularExpression cmdRegEx(const int);

    enum ctrlData {
        ctrlType = Qt::UserRole,
        ctrlNum,
        ctrlGains,
        ctrlLimits
    };


    const QHash<QString,QString> ctrlTypes = { {"PID_CNTRL",        "PID Control"},
                                               {"LYA_ATT_CNTRL",    "Lyapunov Attitude Control"},
                                               {"LYA_2BODY_CNTRL",  "Lyapunov 2-Body Control"},
                                               {"H_DUMP_CNTRL",     "Momentum Dumping Control"}};

    enum gainsData {
        gainsType = Qt::UserRole,
        gainsNum,
        gainsData
    };

    const QHash<QString,QString> gainsTypes = { {"PID",             "PID Direct"},
                                                {"PID_WN",          "PID Tuning"},
                                                {"MomentumDump",    "Momentum Dumping"},
                                                {"FC_LYA",          "Lyapunov"}};

    // This is how controllers are mapped to valid gains
    const QHash<QString,QStringList> allowableGains = { {"PID_CNTRL",       {"PID","PID_WN"}},
                                                        {"LYA_ATT_CNTRL",   {"FC_LYA"}},
                                                        {"LYA_2BODY_CNTRL", {"FC_LYA"}},
                                                        {"H_DUMP_CNRL",     {"MomentumDump"}}};

    // This is how valid gains are mapped to controllers
    const QHash<QString,QStringList> allowableCtrl  = { {"PID",             {"PID_CNTRL"}},
                                                        {"PID_WN",          {"PID_CNTRL"}},
                                                        {"MomentumDump",    {"H_DUMP_CNTRL"}},
                                                        {"FC_LYA",          {"LYA_ATT_CNTRL","LYA_2BODY_CNTRL"}}};

    enum limData {
        limData = Qt::UserRole,
        limNum
    };

    enum actData {
        actType = Qt::UserRole,
        actNum
    };

    const QHash<QString,QString> actTypes = {  {"WHL", "Wheel"},
                                               {"Ideal","Ideal"},
                                               {"MTB", "Magnetorquers"},
                                               {"THR_3DOF", "3DOF Thrusters"},
                                               {"THR_6DOF", "6DOF Thrusters"}};

    const QHash<QString,QString> trnCmdsHashConst = {{"Passive", "PASSIVE_TRN"},
                                                     {"No Change",""}};
    const QHash<QString,QString> attCmdsHashConst = {{"Passive","PASSIVE_ATT"},
                                                     {"No Change",""}};
    const QHash<QString,QString> actCmdsHashConst = {{"No Change",""}};

    QHash<QString,QString> trnCmdsHash = trnCmdsHashConst;
    QHash<QString,QString> attCmdsHash = attCmdsHashConst;
    QHash<QString,QString> actCmdsHash = actCmdsHashConst;
    QHash<QString,QString> attSVCmdsHash;
    QHash<QString,QString> ctlsHash;
    QHash<QString,QString> gainsHash;
    QHash<QString,QString> limsHash;
    QHash<QString,QString> actsHash;

    const QHash<QString,QString> cmdTrnOriConst = {{"OP","Orbit Point"}};
    const QHash<QString,QString> cmdTrnFrmConst = {{"N","Inertial"},
                                                   {"F","Formation"},
                                                   {"L","Local Vert"}};
};

#endif // DSM_MENU_H
