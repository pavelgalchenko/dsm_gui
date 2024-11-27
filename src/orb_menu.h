#ifndef ORB_MENU_H
#define ORB_MENU_H

#include <dsm_gui_lib.h>

#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>

class Formation {
   public:
   enum valid_frames { N = 0, L };

   protected:
   enum valid_frames fixed_frame = N;
   EulerAngles euler_angles      = EulerAngles();
   enum valid_frames expr_frame  = N;
   QVector3D position            = {0, 0, 0};

   const QMap<QString, enum valid_frames> valid_frame_strs = {{"N", N},
                                                              {"L", L}};

   public:
   Formation(const QString f_frame = "N", const QVector3D ang = {0, 0, 0},
             const int seq = 123, const QString e_frame = "N",
             const QVector3D pos = {0, 0, 0}) {
      euler_angles = EulerAngles(ang, seq);
      fixed_frame  = valid_frame_strs.value(f_frame);
      expr_frame   = valid_frame_strs.value(e_frame);
      euler_angles = ang;
      position     = pos;
   }
   void setPosition(const QVector3D pos) {
      position = pos;
   }
   void setFFrame(const QString f_frame) {
      fixed_frame = valid_frame_strs.value(f_frame);
   }
   void setEFrame(const QString e_frame) {
      expr_frame = valid_frame_strs.value(e_frame);
   }
   void setEulerAngles(const EulerAngles &ang) {
      euler_angles = ang;
   }
   void setEulerAngles(const QVector3D ang, const int seq) {
      euler_angles = EulerAngles(ang, seq);
   }
   QVector3D getPosition() const {
      return position;
   }
   double getPosition(int i) const {
      if (i < 0 || i >= 3)
         return 0;
      return position[i];
   }
   QString getFFrame() const {
      return valid_frame_strs.key(fixed_frame);
   }
   QString getEFrame() const {
      return valid_frame_strs.key(expr_frame);
   }
   EulerAngles getEulerAngles() const {
      return euler_angles;
   }
};

namespace Ui {
class ORB_Menu;
}

class ORB_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit ORB_Menu(QWidget *parent = nullptr);
   ~ORB_Menu();

   signals:
   void orbit_changed();

   private slots:
   void set_validators();
   void receive_orbpath(QString);
   void receive_data(QString file_path);
   void clear_data();

   void string2radiobool(QString boolString, QButtonGroup *buttonGroup);
   void setQComboBox(QComboBox *comboBox, QString string);

   void on_orbListRemove_clicked();
   void on_orbListAdd_clicked();
   void on_orbList_itemClicked();

   void on_loadDefaultButton_clicked();
   void on_saveDefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   void on_orbType_currentTextChanged(const QString &arg1);

   void on_orbCentICParam_currentTextChanged(const QString &arg1);

   void on_orbTBodyICParam_currentTextChanged(const QString &arg1);

   void on_orbZeroWorld_currentTextChanged(const QString &arg1);

   void on_orbCentWorld_currentTextChanged(const QString &arg1);

   void on_orbCentFileSelect_clicked();
   void on_orbTBodyFileSelect_clicked();

   void on_orbListDuplicate_clicked();

   void on_orbCentPA_on_toggled(bool checked);

   void on_orbTBodyLPoint_currentTextChanged(const QString &text);

   void checkKepPA();

   private:
   Ui::ORB_Menu *ui;
   enum orb_types { ORB_ZERO = 0, ORB_FLIGHT, ORB_CENTRAL, ORB_THREE_BODY };
   enum cent_init_types { CENT_KEP = 0, CENT_RV, CENT_FILE };
   enum tbp_init_types { TBP_MODES = 0, TBP_XYZ, TBP_FILE };

   // do unique ptrs so they only last as long as the ORB_Menu object
   std::unique_ptr<QDoubleValidator> double_valid;
   std::unique_ptr<QDoubleValidator> zero_pinf_valid;
   std::unique_ptr<QDoubleValidator> ninf_pinf_valid;
   std::unique_ptr<QRegularExpressionValidator> no_quotes_valid;
   std::unique_ptr<QRegularExpressionValidator> no_quotes_spaces_valid;

   int global_orb_index  = -1;
   int global_orb_ignore = 0;

   QString oldPeriAlt = "";
   QString oldApoAlt  = "";

   QString inout_path;
   QStringList orb_data;
   QStringList orb_string;
   QStringList orb_file_headers; // section headers in the file
   QStringList orb_file_descrip; // data descriptors in the file
   QStringList orb_update;

   QHash<QString, QString> orbFileHash;

   const QHash<QString, QString> orbTypeInputs      = {{"ZERO", "Zero"},
                                                       {"FLIGHT", "Flight"},
                                                       {"CENTRAL", "Central"},
                                                       {"THREE_BODY", "Three Body"}};
   const QHash<QString, enum orb_types> orbTypeHash = {
       {"ZERO", ORB_ZERO},
       {"FLIGHT", ORB_FLIGHT},
       {"CENTRAL", ORB_CENTRAL},
       {"THREE_BODY", ORB_THREE_BODY}};

   const QHash<QString, QString> orbTBodyLSysInputs = {
       {"EARTHMOON", "Earth/Moon"},
       {"SUNEARTH", "Sun/Earth"},
       {"SUNJUPITER", "Sun/Jupiter"}};

   const QHash<QString, QString> orbCentICTypeInputs = {
       {"KEP", "Keplerian"}, {"RV", "Position/Velocity"}, {"FILE", "File"}};
   const QHash<QString, enum cent_init_types> orbCentICTypeHash = {
       {"KEP", CENT_KEP}, {"RV", CENT_RV}, {"FILE", CENT_FILE}};

   const QHash<QString, QString> orbTBodyPropInputs = {
       {"LAGDOF_MODES", "Modes"},
       {"LAGDOF_COWELL", "Cowell"},
       {"LAGDOF_SPLINE", "Spline"}};

   const QHash<QString, QString> orbTBodyICTypeInputs = {
       {"MODES", "Modes"}, {"XYZ", "Position/Velocity"}, {"FILE", "File"}};
   const QHash<QString, enum tbp_init_types> orbTBodyICTypeHash = {
       {"MODES", TBP_MODES}, {"XYZ", TBP_XYZ}, {"FILE", TBP_FILE}};

   const QHash<QString, QString> orbFileTypeInputs = {
       {"TLE", "TLE"}, {"TRV", "TRV"}, {"SPLINE", "Spline"}};

   const QStringList orbFrameInputs      = {"N", "L"};
   const QStringList lagrangePointInputs = {"L1", "L2", "L3", "L4", "L5"};
};

// Configure YAML conversions
namespace YAML {
// Formation
template <> struct convert<Formation> {
   static Node encode(const Formation &rhs) {
      Node node(NodeType::Map);
      node["Fixed Frame"]      = rhs.getFFrame();
      node["Euler Angles"]     = rhs.getEulerAngles();
      node["Expression Frame"] = rhs.getEFrame();
      node["Position"]         = rhs.getPosition();
      return node;
   }
   static bool decode(const Node &node, Formation &rhs) {
      if (!node.IsMap())
         return false;
      rhs.setFFrame(node["Fixed Frame"].as<QString>());
      rhs.setEFrame(node["Expression Frame"].as<QString>());
      rhs.setPosition(node["Position"].as<QVector3D>());
      rhs.setEulerAngles(node["Euler Angles"].as<EulerAngles>());
      return true;
   }
};
}; // namespace YAML

#endif // ORB_MENU_H
