#ifndef DSM_GUI_LIB_H
#define DSM_GUI_LIB_H

#include "qtyaml.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFile>
#include <QListWidget>
#include <QProcess>
#include <QRandomGenerator>
#include <QTextStream>
#include <QVariant>

#define STR2(x) #x
#define STR(X)  STR2(X)

class dsm_gui_lib {
   public:
   dsm_gui_lib();

   static int warning_message(QString warningText);
   static int error_message(QString errorText);
   static QString whitespace(QString data);
   static QStringList sortStringList(QStringList unsorted);
   static QStringList getTextFromList(QListWidget *list);
   static QString toString(QCheckBox *box) {
      QString output = QVariant(box->isChecked()).toString().toUpper();
      if (box->isChecked())
         output += " ";
      return output;
   }

   template <class T>
   static T *getObjectFromItemName(const QString name,
                                   QHash<QListWidgetItem *, T *> hash) {
      QList<QListWidgetItem *> items = hash.keys();
      for (auto it = items.begin(); it != items.end(); ++it) {
         if (!(*it)->text().compare(name)) {
            return hash.value((*it));
         }
      }
      return nullptr;
   }
   template <class T>
   static QList<T> getOrderedListFromHash(QListWidget *list,
                                          QHash<QListWidgetItem *, T> hash) {
      QList<T> out = {};
      for (int i = 0; i < list->count(); i++) {
         QListWidgetItem *cur_item = list->item(i);
         out.push_back(hash.value(cur_item));
      }
      return out;
   }
   template <class T>
   static QList<T> getOrderedListFromHash(QListWidget *list,
                                          QHash<QListWidgetItem *, T *> hash) {
      QList<T> out = {};
      for (int i = 0; i < list->count(); i++) {
         QListWidgetItem *cur_item = list->item(i);
         out.push_back(*hash.value(cur_item));
      }
      return out;
   }

   template <class T, class U>
   static void set_mult_validators(const QList<T *> ui_elem, U *u) {
      for (T *elem : ui_elem)
         elem->setValidator(u);
   }
   template <class T> static T limit(T x, T min, T max) {
      return (x >= max ? max : (x <= min ? min : x));
   }
   template <class T, class U, class V> static T limit(T x, U min, V max) {
      return limit(x, (T)min, (T)max);
   }
   static void set_mult_cbox_validators(QList<QComboBox *> ui_elem,
                                        const QStringList string_list);

   static QStringList apply_data_section_end(long cur_item,
                                             QListWidget *ui_elem,
                                             QStringList tmp_data,
                                             QString cur_item_name);
   static std::tuple<long, int, QStringList>
   item_entry_lineitems(QStringList spc_data, int line_num, long reset_ind,
                        long entries, long headers);
   static void mult_setText(QLineEdit *ui_elem[], int array_length,
                            QStringList current_data, int data_inds[]);

   static bool fileExists(QString path);

   static QVector2D create_QVec2(QString arg1, QString arg2);
   static QVector3D create_QVec3(QString arg1, QString arg2, QString arg3);
   static QVector4D create_QVec4(QString arg1, QString arg2, QString arg3,
                                 QString arg4);

   static QString generate_comment(QString str_search, QString cur_line,
                                   YAML::Node comments);

   static void write_data(const QString, const YAML::Node);

   inline static const QStringList eulerInputs = {"121", "123", "131", "132",
                                                  "212", "213", "231", "232",
                                                  "312", "313", "321", "323"};
   enum class WorldID {
      SOL,
      MERCURY,
      VENUS,
      EARTH,
      MARS,
      JUPITER,
      SATURN,
      URANUS,
      NEPTUNE,
      PLUTO,
      LUNA,
      PHOBOS,
      DEIMOS,
      IO,
      EUROPA,
      GANYMEDE,
      CALLISTO,
      AMALTHEA,
      HIMALITA,
      ELARA,
      PASIPHAE,
      SINOPE,
      LYSITHEA,
      CARME,
      ANANKE,
      LEDA,
      THEBE,
      ADRASTEA,
      METIS,
      MIMAS,
      ENCELADUS,
      TETHYS,
      DIONE,
      RHEA,
      TITAN,
      HYPERION,
      IAPETUS,
      PHOEBE,
      JANUS,
      EPIMETHEUS,
      HELENE,
      TELESTO,
      CALYPSO,
      ATLAS,
      PROMETHEUS,
      PANDORA,
      PAN,
      ARIEL,
      UMBRIEL,
      TITANIA,
      OBERON,
      MIRANDA,
      TRITON,
      NERIED,
      CHARON,
      MINORBODY,
   };

   inline static const QStringList worldInputs = {
       "SOL",       "MERCURY",  "VENUS",    "EARTH",    "MARS",
       "JUPITER",   "SATURN",   "URANUS",   "NEPTUNE",  "PLUTO",
       "LUNA",      "PHOBOS",   "DEIMOS",   "IO",       "EUROPA",
       "GANYMEDE",  "CALLISTO", "AMALTHEA", "HIMALITA", "ELARA",
       "PASIPHAE",  "SINOPE",   "LYSITHEA", "CARME",    "ANANKE",
       "LEDA",      "THEBE",    "ADRASTEA", "METIS",    "MIMAS",
       "ENCELADUS", "TETHYS",   "DIONE",    "RHEA",     "TITAN",
       "HYPERION",  "IAPETUS",  "PHOEBE",   "JANUS",    "EPIMETHEUS",
       "HELENE",    "TELESTO",  "CALYPSO",  "ATLAS",    "PROMETHEUS",
       "PANDORA",   "PAN",      "ARIEL",    "UMBRIEL",  "TITANIA",
       "OBERON",    "MIRANDA",  "TRITON",   "NERIED",   "CHARON",
       "MINORBODY"};
   static QString getWorldName(const WorldID IDw) {
      const long Iw = (long)IDw;
      return worldInputs[Iw];
   }

   static int inexplicable_error_message() {
      const QStringList inexplicableErrorMsg = {
          "In your attempt to fly, you failed to miss the ground...",
          "You appear to have misplaced your towel...",
          "If you are reading this, you are not a cool frood...",
          "You have the distinct feeling of your brain being smashed out by a "
          "slice of lemon wrapped around a large gold brick...",
          "The sound of Vogon poetry grows in the distance...",
          "Oh no... Not again..."};
      static int lineCount = inexplicableErrorMsg.count();

      int randomNum = QRandomGenerator::global()->bounded(lineCount);
      return error_message(inexplicableErrorMsg[randomNum]);
   }

   enum class scSectionType {
      BODY,
      WHEEL,
      MTB,
      THRUSTER,
      GYRO,
      MAGNETOMETER,
      CSS,
      FSS,
      STARTRACKER,
      GPS,
      ACCEL,
   };

   static int get_sc_nitems(const QString inout_path, const QString sc_name,
                            const scSectionType type);

   inline static void setQComboBox(QComboBox *box, const QString text,
                                   bool remove = false) {
      box->setCurrentText(text);
      int ind = box->findText(text);
      if (remove && ind != -1) {
         if (box->currentIndex() == ind) {
            box->setCurrentIndex(-1);
         }
         box->removeItem(ind);
      } else
         box->setCurrentIndex(ind);
   }

   inline static WorldID World2ID(QString world) {
      return (WorldID)worldInputs.indexOf(world);
   };
   inline static QString ID2World(WorldID id) {
      return worldInputs[(int)id];
   }

   private:
   inline static QString scSectionIdentifier(scSectionType type) {
      switch (type) {
         case scSectionType::BODY:
            return "Bodies";
         case scSectionType::WHEEL:
            return "Wheels";
         case scSectionType::MTB:
            return "MTBs";
         case scSectionType::THRUSTER:
            return "Thrusters";
         case scSectionType::GYRO:
            return "Gyros";
         case scSectionType::MAGNETOMETER:
            return "Magnetometers";
         case scSectionType::CSS:
            return "CSSs";
         case scSectionType::FSS:
            return "FSSs";
         case scSectionType::STARTRACKER:
            return "STs";
         case scSectionType::GPS:
            return "GPSs";
         case scSectionType::ACCEL:
            return "Accelerometers";
      }
   }
};

class EulerAngles {
   private:
   inline const static QList<int> valid_sequences = {
       121, 123, 131, 132, 212, 213, 231, 232, 312, 313, 321, 323};

   protected:
   QVector3D angles = {0, 0, 0};
   int sequence     = 123;

   public:
   EulerAngles(const QVector3D ang = {0, 0, 0}, const int seq = 123) {
      setAngles(ang);
      setSequence(seq);
   }
   void setAngles(const QVector3D ang) {
      angles = ang;
   }
   void setSequence(const int seq) {
      if (valid_sequences.contains(seq))
         sequence = seq;
      else {
         sequence = valid_sequences[0];
         QString warn_msg =
             QString(
                 "Invalid Sequence, %1, detected. Valid Euler Sequences are: ")
                 .arg(seq);

         for (auto it : valid_sequences) {
            warn_msg.append(QString(" %1,").arg(it));
         }
         warn_msg.chop(1);
         warn_msg.append(".");
         dsm_gui_lib::warning_message(warn_msg);
      }
   }
   QVector3D getAngles() const {
      return angles;
   }
   double getAngles(int i) const {
      if (i < 0 || i >= 3)
         return 0;
      return angles[i];
   }
   int getSequence() const {
      return sequence;
   }
};

// Configure YAML conversions
namespace YAML {
// Euler Angles
template <> struct convert<EulerAngles> {
   static Node encode(const EulerAngles &rhs) {
      Node node(NodeType::Map);
      node["Angles"]   = rhs.getAngles();
      node["Sequence"] = rhs.getSequence();
      return node;
   }
   static bool decode(const Node &node, EulerAngles &rhs) {
      if (!node.IsMap())
         return false;

      rhs.setAngles(node["Angles"].as<QVector3D>());
      rhs.setSequence(node["Sequence"].as<int>());
      return true;
   }
};
} // namespace YAML

#endif // DSM_GUI_LIB_H
