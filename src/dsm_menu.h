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

class configItem {
   protected:
   QString description;

   public:
   QString getDescription() const {
      return description;
   }
   void setDescription(const QString &desc) {
      description = desc;
   }
   configItem(QString desc = "") {
      setDescription(desc);
   }
   bool operator==(const configItem &rhs) {
      return !description.compare(rhs.getDescription()) && this->compare(rhs);
   }

   private:
   virtual bool compare(const configItem &) = 0;
};
class ctrlConfig;
class gainConfig : public configItem {
   friend ctrlConfig;

   protected:
   enum types { PID, PID_WN, MomentumDump, FC_LYA, CUSTOM };
   inline static const QMap<types, QString> typeStrings = {
       {PID, "PID Direct"},
       {PID_WN, "PID Tuning"},
       {MomentumDump, "Momentum Dumping"},
       {FC_LYA, "Lyapunov"},
       {CUSTOM, "Custom"}};
   types type;
   YAML::Node gains;
   types getTypeValue() const {
      return type;
   }
   bool compare(const configItem &rhs) {
      const gainConfig *pRHSGainConfig = dynamic_cast<const gainConfig *>(&rhs);
      if (getTypeValue() == pRHSGainConfig->getTypeValue() &&
          getGains() == pRHSGainConfig->getGains())
         return true;
      else
         return false;
   }

   public:
   void setType(const QString typ) {
      setType(typeStrings.key(typ, PID_WN));
   }
   void setType(const types typ) {
      type = typ;
   }
   QString getType() const {
      return typeStrings.value(type, typeStrings[PID_WN]);
   }
   void setGains(const YAML::Node node) {
      // TODO: more error handling
      gains = node;
   }
   YAML::Node getGains() const {
      return gains;
   }
   gainConfig(const QString desc    = "Gains",
              const QString typ     = typeStrings[PID_WN],
              const YAML::Node node = YAML::Load(
                  "{Gains: {Omega: 0, Zeta: 0, Alpha: 0, Ki_Limit: 0}}"))
       : configItem(desc) {
      setDescription(desc);
      setType(typ);
      setGains(node);
   }
};
class limConfig : public configItem {
   protected:
   QVector3D maxFrc;
   QVector3D maxVel;
   bool compare(const configItem &rhs) {
      const limConfig *pRHSLimConfig = dynamic_cast<const limConfig *>(&rhs);
      if (getMaxFrc() == pRHSLimConfig->getMaxFrc() &&
          getMaxVel() == pRHSLimConfig->getMaxVel())
         return true;
      else
         return false;
   }

   public:
   limConfig(const QString &lab = "Limit", const QVector3D &f = {0, 0, 0},
             const QVector3D &v = {0, 0, 0})
       : configItem(lab) {
      setMaxFrc(f);
      setMaxVel(v);
   }
   void setMaxFrc(const QVector3D frc) {
      maxFrc = frc;
   }
   void setMaxVel(const QVector3D vel) {
      maxVel = vel;
   }
   QVector3D getMaxFrc() const {
      return maxFrc;
   }
   QVector3D getMaxVel() const {
      return maxVel;
   }
};
class actConfig : public configItem {
   protected:
   enum types { WHL, IDEAL, MTB, THR_3DOF, THR_6DOF };
   inline static const QMap<types, QString> typeStrings = {
       {WHL, "Wheel"},
       {IDEAL, "Ideal"},
       {MTB, "Magnetorquers"},
       {THR_3DOF, "3DOF Thrusters"},
       {THR_6DOF, "6DOF Thrusters"}};
   types type;
   types getTypeValue() const {
      return type;
   }
   bool compare(const configItem &rhs) {
      const actConfig *pRHSActConfig = dynamic_cast<const actConfig *>(&rhs);
      if (getTypeValue() == pRHSActConfig->getTypeValue())
         return true;
      else
         return false;
   }

   public:
   void setType(const QString typ) {
      setType(typeStrings.key(typ));
   }
   void setType(const types typ) {
      type = typ;
   }
   QString getType() const {
      return typeStrings.value(type, typeStrings[IDEAL]);
   }
   actConfig(const QString desc = "Actuator",
             const QString typ  = typeStrings[IDEAL])
       : configItem(desc) {
      setType(typ);
   }
};

class ctrlConfig : public configItem {
   protected:
   enum types {
      PID_CNTRL,
      LYA_ATT_CNTRL,
      LYA_2BODY_CNTRL,
      H_DUMP_CNTRL,
      CUSTOM_CNTRL,
   };
   inline static const QMap<types, QString> typeStrings = {
       {LYA_ATT_CNTRL, "Lyapunov Attitude Control"},
       {PID_CNTRL, "PID Control"},
       {LYA_2BODY_CNTRL, "Lyapunov 2-Body Control"},
       {H_DUMP_CNTRL, "Momentum Dumping Control"},
       {CUSTOM_CNTRL, "Custom Control"}};
   types type;
   gainConfig gains;
   limConfig lims;
   bool compare(const configItem &rhs) {
      const ctrlConfig *pRHSctrlConfig = dynamic_cast<const ctrlConfig *>(&rhs);
      if (type == pRHSctrlConfig->getTypeValue() &&
          get<gainConfig>() == pRHSctrlConfig->get<gainConfig>() &&
          get<limConfig>() == pRHSctrlConfig->get<limConfig>())
         return true;
      else
         return false;
   }

   public:
   template <class V> void set(const V &thing) {
      if constexpr (std::is_same<V, gainConfig>::value)
         gains = thing;
      else if constexpr (std::is_same<V, limConfig>::value)
         lims = thing;
      else if (std::is_same<V, types>::value)
         type = thing;
      else {
         dsm_gui_lib::inexplicable_error_message();
         exit(EXIT_FAILURE);
      }
   }
   template <class V> V get() const {
      if constexpr (std::is_same<V, gainConfig>::value)
         return gains;
      else if constexpr (std::is_same<V, limConfig>::value)
         return lims;
      else {
         dsm_gui_lib::inexplicable_error_message();
         exit(EXIT_FAILURE);
      }
   }

   void setType(const QString typ) {
      set(typeStrings.key(typ, PID_CNTRL));
   }
   QString getType() const {
      return typeStrings.value(type);
   }
   types getTypeValue() const {
      return type;
   }
   ctrlConfig(const QString desc    = "Controller",
              const QString typ     = typeStrings[PID_CNTRL],
              const gainConfig &gai = gainConfig(),
              const limConfig &lim  = limConfig())
       : configItem(desc) {
      setType(typ);
      set(gai);
      set(lim);
   }
};
enum class cmdSubtypes {
   Position,
   Maneuver,
   PrimaryVec,
   SecondaryVec,
   Mirror,
   Quaternion,
   Detumble,
   WhlHManage,
   ActuatorCmd,
   Passive,
   TwoVec,
   NoChange,
};
class cmdConfig : public configItem {
   public:
   inline static const QMap<cmdSubtypes, QString> typeStrings = {
       {cmdSubtypes::Position, "Position"},
       {cmdSubtypes::Maneuver, "Maneuver"},
       {cmdSubtypes::PrimaryVec, "Primary Vector"},
       {cmdSubtypes::SecondaryVec, "Secondary Vector"},
       {cmdSubtypes::Mirror, "Mirror"},
       {cmdSubtypes::Quaternion, "Quaternion"},
       {cmdSubtypes::Detumble, "Detumble"},
       {cmdSubtypes::WhlHManage, "Whl H Manage"},
       {cmdSubtypes::ActuatorCmd, "Actuator Cmd"},
       {cmdSubtypes::Passive, "Passive"},
       {cmdSubtypes::TwoVec, "Two Vector"},
       {cmdSubtypes::NoChange, "No Change"},
   };

   protected:
   ctrlConfig controller;
   actConfig actuator;
   const cmdSubtypes cmdType = cmdSubtypes::Passive;
   virtual bool compare(const configItem &rhs) {
      const cmdConfig *pRHSCmdConfig = dynamic_cast<const cmdConfig *>(&rhs);
      if (getTypeValue() == pRHSCmdConfig->getTypeValue() &&
          get<ctrlConfig>() == pRHSCmdConfig->get<ctrlConfig>() &&
          get<actConfig>() == pRHSCmdConfig->get<actConfig>() &&
          this->compareCmd(*pRHSCmdConfig))
         return true;
      else
         return false;
   }
   virtual bool compareCmd(const cmdConfig &rhs) = 0;

   public:
   template <class V> void set(const V &thing) {
      if constexpr (std::is_same<V, ctrlConfig>::value)
         controller = thing;
      else if constexpr (std::is_same<V, actConfig>::value)
         actuator = thing;
      else {
         dsm_gui_lib::inexplicable_error_message();
         exit(EXIT_FAILURE);
      }
   }
   template <class V> V get() const {
      if constexpr (std::is_same<V, ctrlConfig>::value)
         return controller;
      else if constexpr (std::is_same<V, actConfig>::value)
         return actuator;
      else {
         dsm_gui_lib::inexplicable_error_message();
         exit(EXIT_FAILURE);
      }
   }
   cmdSubtypes getTypeValue() const {
      return cmdType;
   }
   cmdConfig(const QString desc     = "Command",
             const ctrlConfig &ctrl = ctrlConfig(),
             const actConfig &act   = actConfig(),
             const cmdSubtypes type = cmdSubtypes::Passive)
       : configItem(desc), cmdType(type) {
      controller = ctrl;
      actuator   = act;
   }
};

class posCmdConfig : public cmdConfig {
   private:
   QVector3D position;
   QString origin;
   QString frame;
   bool compareCmd(const cmdConfig &rhs) {
      const posCmdConfig *pRHSPosCmdConfig =
          dynamic_cast<const posCmdConfig *>(&rhs);
      if (position == pRHSPosCmdConfig->getPosition() &&
          origin == pRHSPosCmdConfig->getOrigin() &&
          frame == pRHSPosCmdConfig->getFrame())
         return true;
      else
         return false;
   }

   public:
   posCmdConfig(const QString desc     = typeStrings[cmdSubtypes::Position],
                const ctrlConfig &ctrl = ctrlConfig(),
                const actConfig &act   = actConfig(),
                const QVector3D pos = {0, 0, 0}, const QString ori = "OP",
                const QString f = "F")
       : cmdConfig(desc, ctrl, act, cmdSubtypes::Position) {
      position = pos;
      origin   = ori;
      frame    = f;
   }
   QVector3D getPosition() const {
      return position;
   }
   QString getOrigin() const {
      return origin;
   }
   QString getFrame() const {
      return frame;
   }
   void setPosition(const QVector3D thing) {
      position = thing;
   }
   void setOrigin(const QString thing) {
      origin = thing;
   }
   void setFrame(const QString thing) {
      frame = thing;
   }
};
class manCmdConfig : public cmdConfig {
   private:
   QVector3D deltaV;
   QString frame;
   QString type;
   double duration;
   limConfig limits;
   bool compareCmd(const cmdConfig &rhs) {
      const manCmdConfig *pRHSManCmdConfig =
          dynamic_cast<const manCmdConfig *>(&rhs);
      if (deltaV == pRHSManCmdConfig->getDeltaV() &&
          frame == pRHSManCmdConfig->getFrame() &&
          type == pRHSManCmdConfig->getType() &&
          duration == pRHSManCmdConfig->getDuration() &&
          limits == pRHSManCmdConfig->get<limConfig>())
         return true;
      else
         return false;
   }

   public:
   manCmdConfig(const QString desc   = typeStrings[cmdSubtypes::Maneuver],
                const actConfig &act = actConfig(),
                const QVector3D dV = {0, 0, 0}, const QString f = "B",
                const QString typ = "SMOOTHED", double dur = 0,
                const limConfig &lims = limConfig())
       : cmdConfig(desc, ctrlConfig(), act, cmdSubtypes::Maneuver) {
      deltaV   = dV;
      frame    = f;
      type     = typ;
      duration = dur;
      limits   = lims;
   }
   QVector3D getDeltaV() const {
      return deltaV;
   }
   QString getFrame() const {
      return frame;
   }
   QString getType() const {
      return type;
   }
   double getDuration() const {
      return duration;
   }
   void setDeltaV(const QVector3D thing) {
      deltaV = thing;
   }
   void setFrame(const QString thing) {
      frame = thing;
   }
   void setType(const QString thing) {
      type = thing;
   }
   void setDuration(const double thing) {
      duration = thing;
   }
   template <class V> void set(const V &thing) {
      if constexpr (std::is_same<V, limConfig>::value)
         limits = thing;
      else if constexpr (std::is_same<V, actConfig>::value)
         actuator = thing;
      else {
         dsm_gui_lib::inexplicable_error_message();
         exit(EXIT_FAILURE);
      }
   }
   template <class V> V get() const {
      if constexpr (std::is_same<V, limConfig>::value)
         return limits;
      else if constexpr (std::is_same<V, actConfig>::value)
         return actuator;
      else {
         dsm_gui_lib::inexplicable_error_message();
         exit(EXIT_FAILURE);
      }
   }
};
class tgtConfig {
   protected:
   enum types { VEC, BODY, SC };
   types type;
   QString frame;
   QString target;
   QVector3D axis;
   inline static const QMap<types, QString> typeStrings = {
       {VEC, "VEC"}, {BODY, "BODY"}, {SC, "SC"}};
   types getTypeValue() const {
      return type;
   }

   public:
   bool operator==(const tgtConfig &rhs) {
      if (type == rhs.getTypeValue() && frame == rhs.getFrame() &&
          target == rhs.getTarget() && axis == rhs.getAxis())
         return true;
      else
         return false;
   }
   tgtConfig(const QString typ = "VEC", const QString f = "L",
             const QVector3D ax = {1, 0, 0}) {
      type   = typeStrings.key(typ, VEC);
      frame  = f;
      axis   = ax;
      target = "";
   }
   QString getType() const {
      return typeStrings[type];
   }
   QString getFrame() const {
      if (type != VEC)
         return "";
      return frame;
   }
   QString getTarget() const {
      if (type == VEC)
         return "";
      return target;
   }
   QVector3D getAxis() const {
      if (type != VEC)
         return {0, 0, 0};
      return axis;
   }
   void setType(const QString thing) {
      type = typeStrings.key(thing, VEC);
   }
   void setFrame(const QString thing) {
      if (type != VEC)
         return;
      frame = thing;
   }
   void setTarget(const QString thing) {
      if (type == VEC)
         return;
      target = thing;
   }
   void setAxis(const QVector3D thing) {
      if (type != VEC)
         return;
      axis = thing;
   }
   QString asString() const;
};
class pvCmdConfig : public cmdConfig {
   private:
   tgtConfig target;
   QVector3D axis;
   bool compareCmd(const cmdConfig &rhs) {
      const pvCmdConfig *pRHSPVCmdConfig =
          dynamic_cast<const pvCmdConfig *>(&rhs);
      if (target == pRHSPVCmdConfig->getTarget() &&
          axis == pRHSPVCmdConfig->getAxis())
         return true;
      else
         return false;
   }

   public:
   pvCmdConfig(const QString desc     = typeStrings[cmdSubtypes::PrimaryVec],
               const ctrlConfig &ctrl = ctrlConfig(),
               const actConfig &act   = actConfig(),
               const tgtConfig &tgt   = tgtConfig(),
               const QVector3D ax     = {0, 0, 0})
       : cmdConfig(desc, ctrl, act, cmdSubtypes::PrimaryVec) {
      target = tgt;
      axis   = ax;
   }
   tgtConfig getTarget() const {
      return target;
   }
   QVector3D getAxis() const {
      return axis;
   }
   void setTarget(const tgtConfig thing) {
      target = thing;
   }
   void setAxis(const QVector3D thing) {
      axis = thing;
   }
};
class svCmdConfig : public cmdConfig {
   private:
   tgtConfig target;
   QVector3D axis;
   bool compareCmd(const cmdConfig &rhs) {
      const svCmdConfig *pRHSPVCmdConfig =
          dynamic_cast<const svCmdConfig *>(&rhs);
      if (target == pRHSPVCmdConfig->getTarget() &&
          axis == pRHSPVCmdConfig->getAxis())
         return true;
      else
         return false;
   }

   public:
   svCmdConfig(const QString desc   = typeStrings[cmdSubtypes::SecondaryVec],
               const tgtConfig &tgt = tgtConfig(),
               const QVector3D ax   = {0, 0, 0})
       : cmdConfig(desc, ctrlConfig(), actConfig(), cmdSubtypes::SecondaryVec) {
      target = tgt;
      axis   = ax;
   }
   tgtConfig getTarget() const {
      return target;
   }
   QVector3D getAxis() const {
      return axis;
   }
   void setTarget(const tgtConfig thing) {
      target = thing;
   }
   void setAxis(const QVector3D thing) {
      axis = thing;
   }
};
class mirrorCmdConfig : public cmdConfig {
   private:
   QString target;
   bool compareCmd(const cmdConfig &rhs) {
      const mirrorCmdConfig *pRHSMirrorCmdConfig =
          dynamic_cast<const mirrorCmdConfig *>(&rhs);
      if (target == pRHSMirrorCmdConfig->getTarget())
         return true;
      else
         return false;
   }

   public:
   mirrorCmdConfig(const QString desc     = typeStrings[cmdSubtypes::Mirror],
                   const ctrlConfig &ctrl = ctrlConfig(),
                   const actConfig &act   = actConfig(),
                   const QString tgt      = "SC[0].B[0]")
       : cmdConfig(desc, ctrl, act, cmdSubtypes::Mirror) {
      target = tgt;
   }
   QString getTarget() const {
      return target;
   }
   void setTarget(const QString thing) {
      target = thing;
   }
};
class quatCmdConfig : public cmdConfig {
   private:
   QVector4D quaternion;
   QString frame;
   bool compareCmd(const cmdConfig &rhs) {
      const quatCmdConfig *pRHSQuatCmdConfig =
          dynamic_cast<const quatCmdConfig *>(&rhs);
      if (quaternion == pRHSQuatCmdConfig->getQuaternion() &&
          frame == pRHSQuatCmdConfig->getFrame())
         return true;
      else
         return false;
   }

   public:
   quatCmdConfig(const QString desc     = typeStrings[cmdSubtypes::Quaternion],
                 const ctrlConfig &ctrl = ctrlConfig(),
                 const actConfig &act   = actConfig(),
                 const QVector4D quat = {0, 0, 0, 0}, const QString f = "F")
       : cmdConfig(desc, ctrl, act, cmdSubtypes::Quaternion) {
      quaternion = quat;
      frame      = f;
   }
   QVector4D getQuaternion() const {
      return quaternion;
   }
   QString getFrame() const {
      return frame;
   }
   void setQuaternion(const QVector4D thing) {
      quaternion = thing;
   }
   void setFrame(const QString thing) {
      frame = thing;
   }
};
class detumbleCmdConfig : public cmdConfig {
   private:
   bool compareCmd(const cmdConfig &rhs) {
      return true;
   }

   public:
   detumbleCmdConfig(const QString desc = typeStrings[cmdSubtypes::Detumble],
                     const ctrlConfig &ctrl = ctrlConfig(),
                     const actConfig &act   = actConfig())
       : cmdConfig(desc, ctrl, act, cmdSubtypes::Detumble) {}
};
class whlHManageCmdConfig : public cmdConfig {
   private:
   double max;
   double min;
   bool dumping;
   bool compareCmd(const cmdConfig &rhs) {
      const whlHManageCmdConfig *pRHSWhlHManageCmdConfig =
          dynamic_cast<const whlHManageCmdConfig *>(&rhs);
      if (max == pRHSWhlHManageCmdConfig->getMax() &&
          min == pRHSWhlHManageCmdConfig->getMin() &&
          dumping == pRHSWhlHManageCmdConfig->getDumping())
         return true;
      else
         return false;
   }

   public:
   whlHManageCmdConfig(
       const QString desc     = typeStrings[cmdSubtypes::WhlHManage],
       const ctrlConfig &ctrl = ctrlConfig(),
       const actConfig &act = actConfig(), const double mx = 0,
       const double mn = 0, const bool dump = false)
       : cmdConfig(desc, ctrl, act, cmdSubtypes::WhlHManage) {
      max     = mx;
      min     = mn;
      dumping = dump;
   }
   double getMax() const {
      return max;
   }
   double getMin() const {
      return min;
   }
   bool getDumping() const {
      return dumping;
   }
   void setMax(const double thing) {
      max = thing;
   }
   void setMin(const double thing) {
      min = thing;
   }
   void setDumping(const bool thing) {
      dumping = thing;
   }
};
class cmdActConfig {
   public:
   enum types { THR, WHL, MTB };
   inline static const QMap<types, QString> typeNames = {
       {THR, "THR"}, {WHL, "WHL"}, {MTB, "MTB"}};

   private:
   types type;
   double dutyCycle;
   int index;
   types getTypeValue() {
      return type;
   }

   public:
   bool operator==(const cmdActConfig &rhs) {
      if (type == rhs.getType() && dutyCycle == rhs.getDutyCycle() &&
          index == rhs.getIndex())
         return true;
      else
         return false;
   }
   bool operator!=(const cmdActConfig &rhs) {
      if (type != rhs.getType() || dutyCycle != rhs.getDutyCycle() ||
          index != rhs.getIndex())
         return true;
      else
         return false;
   }
   cmdActConfig(types typ = WHL, const double d = 0, const int ind = 0) {
      type      = typ;
      dutyCycle = d > 100 ? 100 : (d < 0 ? 0 : d);
      index     = ind;
   }
   double getDutyCycle() const {
      return dutyCycle;
   }
   int getIndex() const {
      return index;
   }
   QString getType() const {
      return typeNames[type];
   }
   void setDutyCycle(const double thing) {
      dutyCycle = thing;
   }
   void setIndex(const int thing) {
      index = thing;
   }
   void setType(const QString thing) {
      type = typeNames.key(thing, WHL);
   }
   QString asString() const {
      const QString format = "%1_[%2]_[%3]";
      return format.arg(typeNames[type]).arg(index).arg(dutyCycle);
   }
};
class actCmdConfig : public cmdConfig {
   private:
   QList<cmdActConfig> actuators;
   bool compareCmd(const cmdConfig &rhs) {
      const actCmdConfig *pRHSActCmdConfig =
          dynamic_cast<const actCmdConfig *>(&rhs);
      const QList<cmdActConfig> rhsActuators = pRHSActCmdConfig->getActuators();
      const int actLength                    = actuators.length();
      if (actLength != rhsActuators.length()) {
         return false;
      }
      for (int i = 0; i <= actLength; i++) {
         if (actuators[i] != rhsActuators[i])
            return false;
      }
      return true;
   }

   public:
   actCmdConfig(const QString desc = typeStrings[cmdSubtypes::ActuatorCmd],
                const QList<cmdActConfig> acts = {})
       : cmdConfig(desc, ctrlConfig(), actConfig(), cmdSubtypes::ActuatorCmd) {
      actuators = acts;
   }
   QList<cmdActConfig> getActuators() const {
      return actuators;
   }
   void setActuators(const QList<cmdActConfig> thing) {
      actuators = thing;
   }
   cmdActConfig getActuator(const int i) const {
      return actuators[i];
   }
};

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
      POSITION,
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

   private: // TODO: slots
   void set_connections();
   void set_validators();
   void receive_dsmpath(QString);
   void receive_data();
   void write_data();

   void set_command_ctrl_act(QTreeWidgetItem *, const YAML::Node);

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
   QString yaml2DataString(const YAML::Node node);
   QString yaml2DataString(const YAML::Node node, const dsmSectionTypes type);

   public:
   inline static const QString cmdDelimiter = ";";
   inline static const QString labelMkr     = "#";
   inline static const QString scBdyFrmt    = "SC[%1].B[%2]";
   // must be whitespace character(s)
   inline static const QString cmdDataSpacer = "  ";

   private:
   Ui::DSM_Menu *ui;

   // do unique ptrs so they only last as long as the DSM_menu object
   std::unique_ptr<QDoubleValidator> z_pinf_valid;
   std::unique_ptr<QDoubleValidator> ninf_pinf_valid;

   // I'm pretty sure at this point that YAML::Node is a smart pointer
   YAML::Node inp_dsm; // where all the data is stored

   QString inoutPath;
   QString filePath;
   QStringList dsmUpdate;

   QStringList scNames;
   QHash<QString, int> nBdys;
   QHash<QString, int> nWhls;
   QHash<QString, int> nMtbs;
   QHash<QString, int> nThrs;

   const QHash<dsmSectionTypes, QString> yamlSectionHeaders = {
       {dsmSectionTypes::COMMANDS, "DSM Commands"},
       {dsmSectionTypes::POSITION, "Position Configurations"},
       {dsmSectionTypes::PRIMARY_VEC, "Primary Vector Configurations"},
       {dsmSectionTypes::SECONDARY_VEC, "Secondary Vector Configurations"},
       {dsmSectionTypes::QUATERION, "Quaternion Configurations"},
       {dsmSectionTypes::MIRROR, "Mirror Configurations"},
       {dsmSectionTypes::DETUMBLE, "Detumble Configurations"},
       {dsmSectionTypes::WHLHMANAGEMENT, "Whl H Manage Configurations"},
       {dsmSectionTypes::ACTUATOR_CMD, "Actuator Cmd Configurations"},
       {dsmSectionTypes::CONTROLLERS, "Controller Configurations"},
       {dsmSectionTypes::ACTUATORS, "Actuator Configurations"},
       {dsmSectionTypes::GAINS, "Gains Configurations"},
       {dsmSectionTypes::LIMITS, "Limits Configurations"},
       {dsmSectionTypes::MANEUVER, "Maneuver Configurations"}};
   const QHash<dsmSectionTypes, QString> yamlSectionNames = {
       {dsmSectionTypes::POSITION, "Position"},
       {dsmSectionTypes::PRIMARY_VEC, "Primary Vector"},
       {dsmSectionTypes::SECONDARY_VEC, "Secondary Vector"},
       {dsmSectionTypes::QUATERION, "Quaternion"},
       {dsmSectionTypes::MIRROR, "Mirror"},
       {dsmSectionTypes::DETUMBLE, "Detumble"},
       {dsmSectionTypes::WHLHMANAGEMENT, "Whl H Manage"},
       {dsmSectionTypes::ACTUATOR_CMD, "Actuator Cmd"},
       {dsmSectionTypes::CONTROLLERS, "Controller"},
       {dsmSectionTypes::ACTUATORS, "Actuator"},
       {dsmSectionTypes::GAINS, "Gains"},
       {dsmSectionTypes::LIMITS, "Limits"},
       {dsmSectionTypes::MANEUVER, "Maneuver"}};

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
       dsmSectionTypes::POSITION,       dsmSectionTypes::COMMANDS};

   const QVector<dsmSectionTypes> cmdTypes = {
       dsmSectionTypes::POSITION,       dsmSectionTypes::PRIMARY_VEC,
       dsmSectionTypes::SECONDARY_VEC,  dsmSectionTypes::QUATERION,
       dsmSectionTypes::MIRROR,         dsmSectionTypes::DETUMBLE,
       dsmSectionTypes::WHLHMANAGEMENT, dsmSectionTypes::ACTUATOR_CMD,
       dsmSectionTypes::MANEUVER};

   QString entryItemName(const dsmSectionTypes type);
   QRegularExpression entryItemRegEx(const dsmSectionTypes type);
   QString entryItemFormat(const dsmSectionTypes type);

   void new_entry_item(const dsmSectionTypes, const YAML::Node);
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
       {dsmSectionTypes::POSITION, cmdTrn},
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
       {dsmSectionTypes::POSITION, "Translation Command"},
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

   inline static const QHash<QString, YAML::Node> trnCmdsHashConst = {
       {"Passive", YAML::Load("{Type: Translation, Subtype: Passive}")},
       {"No Change", YAML::Load("")}};
   inline static const QHash<QString, YAML::Node> attCmdsHashConst = {
       {"Passive", YAML::Load("{Type: Attitude, Subtype: Passive}")},
       {"No Change", YAML::Load("")}};
   inline static const QHash<QString, YAML::Node> actCmdsHashConst = {
       {"No Change", YAML::Load("")}};
   inline static const QHash<QString, YAML::Node> attSVCmdsHashConst = {
       {"", YAML::Load("")}};
   inline static const QHash<QString, YAML::Node> hManCmdsHashConst = {
       {"No Change", YAML::Load("")}};

   QHash<QString, YAML::Node> trnCmdsHash;
   QHash<QString, YAML::Node> attCmdsHash;
   QHash<QString, YAML::Node> actCmdsHash;
   QHash<QString, YAML::Node> attSVCmdsHash;
   QHash<QString, YAML::Node> hManCmdsHash;
   QHash<QString, YAML::Node> ctlsHash;
   QHash<QString, YAML::Node> gainsHash;
   QHash<QString, YAML::Node> limsHash;
   QHash<QString, YAML::Node> actsHash;

   const QHash<dsmSectionTypes, const QHash<QString, YAML::Node> *>
       metaConstHash = {
           {DSM_Menu::dsmSectionTypes::POSITION, &trnCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::PRIMARY_VEC, &attCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::SECONDARY_VEC, &attSVCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::QUATERION, &attCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::MIRROR, &attCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::DETUMBLE, &attCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::WHLHMANAGEMENT, &hManCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::ACTUATOR_CMD, &actCmdsHashConst},
           {DSM_Menu::dsmSectionTypes::MANEUVER, &trnCmdsHashConst}};
   ;

   const QHash<dsmSectionTypes, QHash<QString, YAML::Node> *> metaHash = {
       {DSM_Menu::dsmSectionTypes::POSITION, &trnCmdsHash},
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

namespace YAML {
// gainConfig
template <> struct convert<gainConfig> {
   static Node encode(const gainConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Gains"]   = node;
      node["Description"] = rhs.getDescription();
      node["Type"]        = rhs.getType();
      node["Gains"]       = rhs.getGains();
      return headNode;
   }
   static bool decode(const Node &headNode, gainConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Gains"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setGains(YAML::Node(node["Gains"]));
      rhs.setType(node["Type"].as<QString>());
      return true;
   }
};

// limConfig
template <> struct convert<limConfig> {
   static Node encode(const limConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Limits"]   = node;
      node["Description"]  = rhs.getDescription();
      node["Force Max"]    = rhs.getMaxFrc();
      node["Velocity Max"] = rhs.getMaxVel();
      return headNode;
   }

   static bool decode(const Node &headNode, limConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Limits"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setMaxFrc(node["Force Max"].as<QVector3D>());
      rhs.setMaxVel(node["Velocity Max"].as<QVector3D>());
      return true;
   }
};

// actConfig
template <> struct convert<actConfig> {
   static Node encode(const actConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Actuator"] = node;
      node["Description"]  = rhs.getDescription();
      node["Type"]         = rhs.getType();
      return headNode;
   }

   static bool decode(const Node &headNode, actConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Actuator"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setType(node["Type"].as<QString>());
      return true;
   }
};

// ctrlConfig
template <> struct convert<ctrlConfig> {
   static Node encode(const ctrlConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Controller"] = node;
      node["Description"]    = rhs.getDescription();
      node["Type"]           = rhs.getType();
      node["Gains"]          = rhs.get<gainConfig>();
      node["Limits"]         = rhs.get<limConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, ctrlConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Controller"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setType(node["Type"].as<QString>());
      rhs.set<gainConfig>(node["Gains"]["Description"].as<gainConfig>());
      rhs.set<limConfig>(node["Limits"]["Description"].as<limConfig>());
      return true;
   }
};

// posCmdConfig
template <> struct convert<posCmdConfig> {
   static Node encode(const posCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Position"] = node;
      node["Description"]  = rhs.getDescription();
      node["Position"]     = rhs.getPosition();
      node["Origin"]       = rhs.getOrigin();
      node["Frame"]        = rhs.getFrame();
      node["Controller"]   = rhs.get<ctrlConfig>();
      node["Actuator"]     = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, posCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Position"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setPosition(node["Position"].as<QVector3D>());
      rhs.setOrigin(node["Origin"].as<QString>());
      rhs.setFrame(node["Frame"].as<QString>());
      rhs.set<ctrlConfig>(node["Controller"]["Description"].as<ctrlConfig>());
      rhs.set<actConfig>(node["Actuator"]["Description"].as<actConfig>());
      return true;
   }
};

// manCmdConfig
template <> struct convert<manCmdConfig> {
   static Node encode(const manCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Maneuver"] = node;
      node["Description"]  = rhs.getDescription();
      node["Delta V"]      = rhs.getDeltaV();
      node["Frame"]        = rhs.getFrame();
      node["Duration"]     = rhs.getDuration();
      node["Limits"]       = rhs.get<limConfig>();
      node["Actuator"]     = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, manCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Maneuver"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setDeltaV(node["DeltaV"].as<QVector3D>());
      rhs.setDuration(node["Duration"].as<double>());
      rhs.setFrame(node["Frame"].as<QString>());
      rhs.set<limConfig>(node["Limits"].as<limConfig>());
      rhs.set<actConfig>(node["Actuator"].as<actConfig>());
      return true;
   }
};

// tgtConfig
template <> struct convert<tgtConfig> {
   static Node encode(const tgtConfig &rhs) {
      Node node(NodeType::Map);
      const QString type = rhs.getType();
      node["Type"]       = type;
      if (!type.compare("VEC")) {
         node["Frame"] = rhs.getFrame();
         node["Axis"]  = rhs.getAxis();
      } else {
         node["Target"] = rhs.getTarget();
      }
      return node;
   }
   static bool decode(const Node &node, tgtConfig &rhs) {
      if (!node.IsMap())
         return false;
      const QString type = node["Type"].as<QString>();
      rhs.setType(type);
      if (!type.compare("VEC")) {
         rhs.setFrame(node["Frame"].as<QString>());
         rhs.setAxis(node["Axis"].as<QVector3D>());
      } else {
         rhs.setTarget(node["Target"].as<QString>());
      }
      return true;
   }
};

// pvCmdConfig
template <> struct convert<pvCmdConfig> {
   static Node encode(const pvCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Primary Vector"] = node;
      node["Description"]        = rhs.getDescription();
      node["Target"]             = rhs.getTarget();
      node["Axis"]               = rhs.getAxis();
      node["Controller"]         = rhs.get<ctrlConfig>();
      node["Actuator"]           = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, pvCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Primary Vector"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setTarget(node["Target"].as<tgtConfig>());
      rhs.setAxis(node["Axis"].as<QVector3D>());
      rhs.set<ctrlConfig>(node["Controller"].as<ctrlConfig>());
      rhs.set<actConfig>(node["Actuator"].as<actConfig>());
      return true;
   }
};

// svCmdConfig
template <> struct convert<svCmdConfig> {
   static Node encode(const svCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Secondary Vector"] = node;
      node["Description"]          = rhs.getDescription();
      node["Target"]               = rhs.getTarget();
      node["Axis"]                 = rhs.getAxis();
      return headNode;
   }
   static bool decode(const Node &headNode, svCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Secondary Vector"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setTarget(node["Target"].as<tgtConfig>());
      rhs.setAxis(node["Axis"].as<QVector3D>());
      return true;
   }
};

// mirrorCmdConfig
template <> struct convert<mirrorCmdConfig> {
   static Node encode(const mirrorCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Mirror"]  = node;
      node["Description"] = rhs.getDescription();
      node["Target"]      = rhs.getTarget();
      node["Controller"]  = rhs.get<ctrlConfig>();
      node["Actuator"]    = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, mirrorCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Mirror"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setTarget(node["Target"].as<QString>());
      rhs.set<ctrlConfig>(node["Controller"].as<ctrlConfig>());
      rhs.set<actConfig>(node["Actuator"].as<actConfig>());
      return true;
   }
};

// quatCmdConfig
template <> struct convert<quatCmdConfig> {
   static Node encode(const quatCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Quaternion"] = node;
      node["Description"]    = rhs.getDescription();
      node["Quaternion"]     = rhs.getQuaternion();
      node["Frame"]          = rhs.getFrame();
      node["Controller"]     = rhs.get<ctrlConfig>();
      node["Actuator"]       = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, quatCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Quaternion"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setFrame(node["Frame"].as<QString>());
      rhs.setQuaternion(node["Quaternion"].as<QVector4D>());
      rhs.set<ctrlConfig>(node["Controller"].as<ctrlConfig>());
      rhs.set<actConfig>(node["Actuator"].as<actConfig>());
      return true;
   }
};

// detumbleCmdConfig
template <> struct convert<detumbleCmdConfig> {
   static Node encode(const detumbleCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Detumble"] = node;
      node["Description"]  = rhs.getDescription();
      node["Controller"]   = rhs.get<ctrlConfig>();
      node["Actuator"]     = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, detumbleCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Detumble"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.set<ctrlConfig>(node["Controller"].as<ctrlConfig>());
      rhs.set<actConfig>(node["Actuator"].as<actConfig>());
      return true;
   }
};

// whlHManageCmdConfig
template <> struct convert<whlHManageCmdConfig> {
   static Node encode(const whlHManageCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Whl H Manage"] = node;
      node["Description"]      = rhs.getDescription();
      node["Dumping"]          = rhs.getDumping();
      node["Minimum H_norm"]   = rhs.getMin();
      node["Maximum H_norm"]   = rhs.getMax();
      node["Controller"]       = rhs.get<ctrlConfig>();
      node["Actuator"]         = rhs.get<actConfig>();
      return headNode;
   }
   static bool decode(const Node &headNode, whlHManageCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Whl H Manage"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setDumping(node["Dumping"].as<bool>());
      rhs.setMin(node["Minimum H_norm"].as<double>());
      rhs.setMax(node["Maximum H_norm"].as<double>());
      rhs.set<ctrlConfig>(node["Controller"].as<QString>());
      rhs.set<actConfig>(node["Actuator"].as<QString>());
      return true;
   }
};

// cmdActConfig
template <> struct convert<cmdActConfig> {
   static Node encode(const cmdActConfig &rhs) {
      Node node(NodeType::Map);
      node["Type"]       = rhs.getType();
      node["Index"]      = rhs.getIndex();
      node["Duty Cycle"] = rhs.getDutyCycle();
      return node;
   }
   static bool decode(const Node &node, cmdActConfig &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setType(node["Type"].as<QString>());
      rhs.setIndex(node["Index"].as<int>());
      rhs.setDutyCycle(node["Duty Cycle"].as<double>());
      return true;
   }
};

// actCmdConfig
template <> struct convert<actCmdConfig> {
   static Node encode(const actCmdConfig &rhs) {
      Node headNode(NodeType::Map);
      Node node(NodeType::Map);
      headNode["Actuator Cmd"] = node;
      node["Description"]      = rhs.getDescription();
      node["Actuators"]        = rhs.getActuators();
      return headNode;
   }
   static bool decode(const Node &headNode, actCmdConfig &rhs) {
      if (!headNode.IsMap())
         return false;
      const Node node = headNode["Actuator Cmd"];
      if (!node.IsMap())
         return false;
      rhs.setDescription(node["Description"].as<QString>());
      rhs.setActuators(node["Actuators"].as<QList<cmdActConfig>>());
      return true;
   }
};

} // namespace YAML

#endif // DSM_MENU_H
