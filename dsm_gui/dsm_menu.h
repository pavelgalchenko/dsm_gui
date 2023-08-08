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
        cmdPsvTrn,
        cmdPsvAtt,
        cmdTrn,
        cmdAtt,
        cmdPV,
        cmdSV,
        cmdQuat,
        cmdMirror,
        cmdDetumble,
        cmdWhlHManage,
        cmdAct,
        cmdManeuver,
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
};

#endif // DSM_MENU_H
