#ifndef DSM_MENU_H
#define DSM_MENU_H

#include "qtreewidget.h"
#include <dsm_gui_lib.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QRadioButton>
#include <QVector3D>
#include <QVector4D>

namespace Ui {
class DSM_Menu;
class dsmSectionTypes;
} // namespace Ui

class DSM_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit DSM_Menu(QWidget *parent = nullptr);
   ~DSM_Menu();

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

   private slots:
   void set_connections();
   void set_validators();
   void receive_dsmpath(QString);
   void receive_data();
   void write_data();

   void set_command_ctrl_act(QTreeWidgetItem *, const QString, QStringList *);

   void on_cmdTrnOri_currentTextChanged(const QString &arg1);

   void on_cmdTrnFrm_currentTextChanged(const QString &arg1);

   void on_cmdPvTgtType_currentTextChanged(const QString &arg1);

   void on_cmdSvTgtType_currentTextChanged(const QString &arg1);

   void cmd_data_changed();
   void cmd_act_data_changed();
   void gain_data_changed();
   void lim_data_changed();

   void on_cmdController_textActivated(const QString &arg1);

   void on_cmdActuator_textActivated(const QString &arg1);

   void on_cmdLabel_textEdited(const QString &arg1);

   void on_cmdConfigTree_currentItemChanged(QTreeWidgetItem *current,
                                            QTreeWidgetItem *previous);

   void on_closeButton_clicked();

   void on_saveDefaultButton_clicked();

   void on_loadDefaultButton_clicked();

   void on_cmdRemove_clicked();

   void on_cmdAdd_clicked();

   void on_cmdDuplicate_clicked();

   void on_cmdActAdd_clicked();

   void on_cmdActRemove_clicked();

   void on_cmdActList_currentItemChanged(QListWidgetItem *current,
                                         QListWidgetItem *previous);

   void on_cmdTimelineTree_currentItemChanged(QTreeWidgetItem *current,
                                              QTreeWidgetItem *previous);

   void timeline_data_changed();

   void validate_sv_cmds(QString);

   void on_cmdTimeDuplicate_clicked();

   void on_cmdTimeAdd_clicked();

   void on_cmdTimeRemove_clicked();

   void on_actList_currentItemChanged(QListWidgetItem *current,
                                      QListWidgetItem *previous);

   void on_actType_textActivated(const QString &arg1);

   void on_actLabel_textEdited(const QString &arg1);

   void on_actRemove_clicked();

   void on_actAdd_clicked();

   void on_actDuplicate_clicked();

   void populate_cmdtl_dropdowns(int cmdType);
   void populate_cmd_dropdowns();
   void populate_ctrl_dropdowns();

   void on_cmdConfigTree_itemChanged(QTreeWidgetItem *item, int column);

   void on_ctrlConfigTree_currentItemChanged(QTreeWidgetItem *current,
                                             QTreeWidgetItem *previous);

   void on_ctrlConfigTree_itemChanged(QTreeWidgetItem *item, int column);

   void on_ctrlType_textActivated(const QString &arg1);

   void on_ctrlLabel_textEdited(const QString &arg1);

   void on_ctrlGains_textActivated(const QString &arg1);

   void on_ctrlLims_textActivated(const QString &arg1);

   void on_ctrlRemove_clicked();

   void on_ctrlAdd_clicked();

   void on_ctrlDuplicate_clicked();

   void on_gainList_currentItemChanged(QListWidgetItem *current,
                                       QListWidgetItem *previous);

   void on_gainType_textActivated(const QString &arg1);

   void on_gainLabel_textEdited(const QString &arg1);

   void on_gainRemove_clicked();

   void on_gainAdd_clicked();

   void on_gainDuplicate_clicked();

   void on_limList_currentItemChanged(QListWidgetItem *current,
                                      QListWidgetItem *previous);

   void on_limLabel_textEdited(const QString &arg1);

   void on_limRemove_clicked();

   void on_limAdd_clicked();

   void on_limDuplicate_clicked();

   void on_applyButton_clicked();

   void on_cmdQuatNormalize_clicked();

   bool check_actuator_cmds(const QString scName,
                            const QTreeWidgetItem *actCmd);
   bool check_actuator_cmds(const QString scName, const QString actCmdLabel);

   private:
   Ui::DSM_Menu *ui;

   // do unique ptrs so they only last as long as the DSM_menu object
   std::unique_ptr<QDoubleValidator> zero_pinf_valid;
   std::unique_ptr<QDoubleValidator> ninf_pinf_valid;

   QString inoutPath;
   QString filePath;
   QStringList dsmUpdate;

   QStringList scNames;
   QHash<QString, int> nBdys;
   QHash<QString, int> nWhls;
   QHash<QString, int> nMtbs;
   QHash<QString, int> nThrs;

   const QString cmdDelimiter  = ";";
   const QString cmdDataSpacer = "  "; // must be whitespace character(s)
   const QString labelMkr      = "#";
   const QString scBdyFrmt     = "SC[%1].B[%2]";

   QPalette okTextPalette;
   QPalette badTextPalette;

   QBrush okTextBrush;
   QBrush badTextBrush;

   /* Change these enums to change the column order in the QTreeWidgets */
   enum tlCols {
      tlColSC = 0,
      tlColTime,
      tlColTrn,
      tlColAtt,
      tlColHMan,
      tlColAct,
   };

   enum cmdCols {
      cmdColLabel = 0,
      cmdColInd,
      cmdColAct,
      cmdColCtl,
      cmdColData,
   };

   enum ctrlCols {
      ctrlColLabel = 0,
      ctrlColInd,
      ctrlColType,
      ctrlColGains,
      ctrlColLims,
   };
   /* END CHANGE ENUMS */

   /* Change these maps to change the column names in the QTreeWidgets */
   const QMap<int, QString> tlColNames = {{tlColSC, "Spacecraft"},
                                          {tlColTime, "Time [sec]"},
                                          {tlColTrn, "Translation Command"},
                                          {tlColAtt, "Attitude Command"},
                                          {tlColHMan, "Wheel H Manage Command"},
                                          {tlColAct, "Actuator Command"}};

   const QMap<int, QString> cmdColNames = {{cmdColLabel, "Command/Label"},
                                           {cmdColAct, "Actuator"},
                                           {cmdColInd, "Command Index"},
                                           {cmdColCtl, "Controller"},
                                           {cmdColData, "Command Data"}};

   const QMap<int, QString> ctrlColNames = {{ctrlColLabel, "Controller Label"},
                                            {ctrlColInd, "Controller Index"},
                                            {ctrlColType, "Controller Type"},
                                            {ctrlColGains, "Gains"},
                                            {ctrlColLims, "Limits"}};
   /* END CHANGE MAPS */

   const QVector<dsmSectionTypes> searchOrd = {
       dsmSectionTypes::LIMITS,         dsmSectionTypes::GAINS,
       dsmSectionTypes::ACTUATORS,      dsmSectionTypes::CONTROLLERS,
       dsmSectionTypes::MANEUVER,       dsmSectionTypes::ACTUATOR_CMD,
       dsmSectionTypes::WHLHMANAGEMENT, dsmSectionTypes::DETUMBLE,
       dsmSectionTypes::MIRROR,         dsmSectionTypes::QUATERION,
       dsmSectionTypes::SECONDARY_VEC,  dsmSectionTypes::PRIMARY_VEC,
       dsmSectionTypes::TRANSLATION,    dsmSectionTypes::COMMANDS};

   const QVector<dsmSectionTypes> cmdTypes = {
       dsmSectionTypes::TRANSLATION,    dsmSectionTypes::PRIMARY_VEC,
       dsmSectionTypes::SECONDARY_VEC,  dsmSectionTypes::QUATERION,
       dsmSectionTypes::MIRROR,         dsmSectionTypes::DETUMBLE,
       dsmSectionTypes::WHLHMANAGEMENT, dsmSectionTypes::ACTUATOR_CMD,
       dsmSectionTypes::MANEUVER};

   QStringList secDescription(const dsmSectionTypes type);
   QString entryItemName(const dsmSectionTypes type);
   QRegularExpression entryItemRegEx(const dsmSectionTypes type);
   QString entryItemFormat(const dsmSectionTypes type);

   void new_entry_item(const dsmSectionTypes, QString, const int,
                       const QString);
   QHash<int, QTreeWidgetItem *> entryCmdParents;

   enum cmdData { cmdNum = Qt::DisplayRole, cmdType = Qt::UserRole, cmdData };

   enum cmdTypes {
      cmdTrn = 0,
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
   const QVector<int> trnCmds = {cmdPsvTrn, cmdTrn, cmdManeuver};
   const QVector<int> attCmds = {cmdPsvAtt, cmdAtt,      cmdQuat,
                                 cmdMirror, cmdDetumble, cmdPV};

   const QMap<dsmSectionTypes, int> section2Cmd = {
       {dsmSectionTypes::TRANSLATION, cmdTrn},
       {dsmSectionTypes::PRIMARY_VEC, cmdPV},
       {dsmSectionTypes::SECONDARY_VEC, cmdSV},
       {dsmSectionTypes::QUATERION, cmdQuat},
       {dsmSectionTypes::MIRROR, cmdMirror},
       {dsmSectionTypes::DETUMBLE, cmdDetumble},
       {dsmSectionTypes::WHLHMANAGEMENT, cmdWhlHManage},
       {dsmSectionTypes::ACTUATOR_CMD, cmdAct},
       {dsmSectionTypes::MANEUVER, cmdManeuver}};
   const QList<dsmSectionTypes> section2CmdKeys = section2Cmd.keys();

   const QHash<dsmSectionTypes, QString> sectionCmdNames = {
       {dsmSectionTypes::TRANSLATION, "Translation Command"},
       {dsmSectionTypes::PRIMARY_VEC, "Primary Vector Command"},
       {dsmSectionTypes::SECONDARY_VEC, "Secondary Vector Command"},
       {dsmSectionTypes::QUATERION, "Quaternion Command"},
       {dsmSectionTypes::MIRROR, "Mirror Command"},
       {dsmSectionTypes::DETUMBLE, "Detumble Command"},
       {dsmSectionTypes::WHLHMANAGEMENT, "Wheel H Manage Command"},
       {dsmSectionTypes::ACTUATOR_CMD, "Actuator Command"},
       {dsmSectionTypes::MANEUVER, "Maneuver Command"}};

   const QHash<QString, QString> cmdAttTgtTypes = {
       {"VEC", "Vector"}, {"SC", "Spacecraft"}, {"BODY", "World"}};

   const QHash<QString, QString> cmdAttTgtFrm = {
       {"N", "Inertial"}, {"F", "Formation"}, {"L", "Local Vert"}};
   //                                                 {"B","Body"}};

   const QHash<QString, QString> cmdManFrm = {{"N", "Inertial"}, {"B", "Body"}};
   const QHash<QString, QString> cmdManTypes = {
       {"CONSTANT", "Constant Thrust"}, {"SMOOTHED", "Smoothed Thrust"}};

   int getCmdType(QString);
   QRegularExpression cmdRegEx(const int);

   enum ctrlData {
      ctrlNum  = Qt::DisplayRole,
      ctrlType = Qt::UserRole,
      ctrlGains,
      ctrlLimits
   };

   const QHash<QString, QString> ctrlTypes = {
       {"PID_CNTRL", "PID Control"},
       {"LYA_ATT_CNTRL", "Lyapunov Attitude Control"},
       {"LYA_2BODY_CNTRL", "Lyapunov 2-Body Control"},
       {"H_DUMP_CNTRL", "Momentum Dumping Control"},
       {"CUSTOM_CNTRL", "Custom Control"}};

   enum gainsData { gainsType = Qt::UserRole, gainsNum, gainsData };

   const QHash<QString, QString> gainsTypes = {
       {"PID", "PID Direct"},
       {"PID_WN", "PID Tuning"},
       {"MomentumDump", "Momentum Dumping"},
       {"FC_LYA", "Lyapunov"},
       {"CUSTOM", "Custom"}};

   // This is how controllers are mapped to valid gains
   const QHash<QString, QStringList> allowableGains = {
       {"PID_CNTRL", {"PID", "PID_WN"}},
       {"LYA_ATT_CNTRL", {"FC_LYA"}},
       {"LYA_2BODY_CNTRL", {"FC_LYA"}},
       {"H_DUMP_CNTRL", {"MomentumDump"}},
       {"CUSTOM_CNTRL", {"CUSTOM"}}};

   // This is how valid gains are mapped to controllers
   const QHash<QString, QStringList> allowableCtrl = {
       {"PID", {"PID_CNTRL"}},
       {"PID_WN", {"PID_CNTRL"}},
       {"MomentumDump", {"H_DUMP_CNTRL"}},
       {"FC_LYA", {"LYA_ATT_CNTRL", "LYA_2BODY_CNTRL"}},
       {"CUSTOM", {"CUSTOM_CNTRL"}}};

   QHash<QString, QStringList> ctrlValidGains = {{"PID_CNTRL", {}},
                                                 {"LYA_ATT_CNTRL", {}},
                                                 {"LYA_2BODY_CNTRL", {}},
                                                 {"H_DUMP_CNTRL", {}},
                                                 {"CUSTOM_CNTRL", {}}};

   /* START HATE */
   // This....
   // I hate this...
   // This is how I chose to coorelate controllers and actuators to command
   // types Edit, Aug 18th: actually, this is pretty cool
   const QHash<QString, QList<int>> ctrlValidCmds = {
       {"PID_CNTRL", {cmdTrn, cmdPV, cmdQuat, cmdMirror, cmdDetumble}},
       {"LYA_ATT_CNTRL", {cmdPV, cmdQuat, cmdMirror, cmdDetumble}},
       {"LYA_2BODY_CNTRL", {cmdTrn}},
       {"H_DUMP_CNTRL", {cmdWhlHManage}},
       {"CUSTOM_CNTRL",
        {cmdTrn, cmdPV, cmdSV, cmdQuat, cmdMirror, cmdDetumble, cmdWhlHManage,
         cmdAct, cmdManeuver}}};

   QHash<int, QStringList> cmdValidCtrls = {
       {cmdTrn, {}},    {cmdPV, {}},       {cmdSV, {}},         {cmdQuat, {}},
       {cmdMirror, {}}, {cmdDetumble, {}}, {cmdWhlHManage, {}}, {cmdAct, {}}};

   const QHash<QString, QList<int>> actValidCmds = {
       {"WHL", {cmdPV, cmdQuat, cmdMirror, cmdDetumble}},
       {"MTB", {cmdPV, cmdQuat, cmdMirror, cmdDetumble, cmdWhlHManage}},
       {"Ideal",
        {cmdTrn, cmdPV, cmdQuat, cmdMirror, cmdDetumble, cmdWhlHManage,
         cmdManeuver}},
       {"THR_3DOF",
        {cmdTrn, cmdPV, cmdQuat, cmdMirror, cmdDetumble, cmdWhlHManage,
         cmdManeuver}},
       {"THR_6DOF",
        {cmdTrn, cmdPV, cmdQuat, cmdMirror, cmdDetumble, cmdWhlHManage,
         cmdManeuver}}};

   QHash<int, QStringList> cmdValidActs = {
       {cmdTrn, {}},        {cmdPV, {}},     {cmdSV, {}},
       {cmdQuat, {}},       {cmdMirror, {}}, {cmdDetumble, {}},
       {cmdWhlHManage, {}}, {cmdAct, {}},    {cmdManeuver, {}}};
   /* END HATE */

   enum limData { limData = Qt::UserRole, limNum };

   enum actData { actType = Qt::UserRole, actNum };

   const QHash<QString, QString> actTypes = {{"WHL", "Wheel"},
                                             {"Ideal", "Ideal"},
                                             {"MTB", "Magnetorquers"},
                                             {"THR_3DOF", "3DOF Thrusters"},
                                             {"THR_6DOF", "6DOF Thrusters"}};

   inline static const QHash<QString, QString> trnCmdsHashConst = {
       {"Passive", "PASSIVE_TRN"}, {"No Change", ""}};
   inline static const QHash<QString, QString> attCmdsHashConst = {
       {"Passive", "PASSIVE_ATT"}, {"No Change", ""}};
   inline static const QHash<QString, QString> actCmdsHashConst = {
       {"No Change", ""}};
   inline static const QHash<QString, QString> attSVCmdsHashConst = {{"", ""}};
   inline static const QHash<QString, QString> hManCmdsHashConst  = {
       {"No Change", ""}};

   QHash<QString, QString> trnCmdsHash;
   QHash<QString, QString> attCmdsHash;
   QHash<QString, QString> actCmdsHash;
   QHash<QString, QString> attSVCmdsHash;
   QHash<QString, QString> hManCmdsHash;
   QHash<QString, QString> ctlsHash;
   QHash<QString, QString> gainsHash;
   QHash<QString, QString> limsHash;
   QHash<QString, QString> actsHash;

   const QHash<dsmSectionTypes, const QHash<QString, QString> *> metaConstHash =
       {{DSM_Menu::dsmSectionTypes::TRANSLATION, &trnCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::PRIMARY_VEC, &attCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::SECONDARY_VEC, &attSVCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::QUATERION, &attCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::MIRROR, &attCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::DETUMBLE, &attCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::WHLHMANAGEMENT, &hManCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::ACTUATOR_CMD, &actCmdsHashConst},
        {DSM_Menu::dsmSectionTypes::MANEUVER, &trnCmdsHashConst}};
   ;

   const QHash<dsmSectionTypes, QHash<QString, QString> *> metaHash = {
       {DSM_Menu::dsmSectionTypes::TRANSLATION, &trnCmdsHash},
       {DSM_Menu::dsmSectionTypes::PRIMARY_VEC, &attCmdsHash},
       {DSM_Menu::dsmSectionTypes::SECONDARY_VEC, &attSVCmdsHash},
       {DSM_Menu::dsmSectionTypes::QUATERION, &attCmdsHash},
       {DSM_Menu::dsmSectionTypes::MIRROR, &attCmdsHash},
       {DSM_Menu::dsmSectionTypes::DETUMBLE, &attCmdsHash},
       {DSM_Menu::dsmSectionTypes::WHLHMANAGEMENT, &hManCmdsHash},
       {DSM_Menu::dsmSectionTypes::ACTUATOR_CMD, &actCmdsHash},
       {DSM_Menu::dsmSectionTypes::CONTROLLERS, &ctlsHash},
       {DSM_Menu::dsmSectionTypes::ACTUATORS, &actsHash},
       {DSM_Menu::dsmSectionTypes::GAINS, &gainsHash},
       {DSM_Menu::dsmSectionTypes::LIMITS, &limsHash},
       {DSM_Menu::dsmSectionTypes::MANEUVER, &trnCmdsHash}};

   const QHash<QString, QString> cmdTrnOriConst = {{"OP", "Orbit Point"}};
   const QHash<QString, QString> cmdTrnFrmConst = {
       {"N", "Inertial"}, {"F", "Formation"}, {"L", "Local Vert"}};

   const QStringList badScNames = {cmdTrnOriConst["OP"], cmdTrnFrmConst["N"],
                                   cmdTrnFrmConst["F"], cmdTrnFrmConst["L"]};
};

inline uint qHash(const DSM_Menu::dsmSectionTypes &key, uint seed) {
   return ::qHash(static_cast<uint>(key), seed);
}

#endif // DSM_MENU_H
