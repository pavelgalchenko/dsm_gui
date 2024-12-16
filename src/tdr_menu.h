#ifndef TDR_MENU_H
#define TDR_MENU_H

#include "dsm_gui_lib.h"
#include "qlineedit.h"

#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>
#include <QValidator>

class TDRS {
   private:
   int _number    = 1;
   bool _exists   = false;
   QString _label = "TDRS-1";

   public:
   TDRS(const QString label = "TDRS-1", const int number = 1,
        const bool exists = false) {
      setNumber(number);
      setLabel(label);
      setExists(exists);
   }
   void setNumber(const int number) {
      _number = number;
   }
   void setLabel(const QString label) {
      _label = label;
   }
   void setExists(const bool exists) {
      _exists = exists;
   }
   int number() const {
      return _number;
   }
   bool exists() const {
      return _exists;
   }
   QString label() const {
      return _label;
   }
};

namespace Ui {
class TDR_Menu;
}

class TDR_Menu : public QDialog {
   Q_OBJECT

   public:
   explicit TDR_Menu(QWidget *parent = nullptr);
   ~TDR_Menu();

   private slots:
   void set_validators();
   void receive_tdrpath(QString);
   void apply_data();

   void on_loaddefaultButton_clicked();
   void on_savedefaultButton_clicked();
   void on_closeButton_clicked();
   void on_applyButton_clicked();

   private:
   Ui::TDR_Menu *ui;

   QString inout_path;
   QString file_path;

   QList<QCheckBox *> tdrsEnabled;
   QList<QLineEdit *> tdrsNames;
};

// Configure YAML conversions
namespace YAML {
// TDRS
template <> struct convert<TDRS> {
   static Node encode(const TDRS &rhs) {
      Node node(NodeType::Map);
      Node data_node      = node["TDRS"];
      data_node["Label"]  = rhs.label();
      data_node["Exists"] = rhs.exists();
      data_node["Number"] = rhs.number();
      return node;
   }
   static bool decode(const Node &node, TDRS &rhs) {
      if (!node.IsMap())
         return false;
      Node data_node = node["TDRS"];
      if (!data_node.IsMap())
         return false;
      rhs.setLabel(data_node["Label"].as<QString>());
      rhs.setExists(data_node["Exists"].as<bool>());
      rhs.setNumber(data_node["Number"].as<int>());
      return true;
   }
};
} // namespace YAML
#endif // TDR_MENU_H
