#ifndef DSM_GUI_LIB_H
#define DSM_GUI_LIB_H

#include "qlistwidget.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFile>
#include <QLineEdit>
#include <QRandomGenerator>
#include <QTextStream>
#include <QVariant>

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

   static void set_mult_validators(QLineEdit *ui_elem[], int array_length,
                                   double lower, double upper, int decimals);
   static void set_mult_name_validators(QLineEdit *ui_elem[], int array_length,
                                        QValidator *validator);
   static void set_mult_cbox_validators(QComboBox *ui_elem[], int array_length,
                                        const QStringList string_list);
   static QStringList
   apply_data_section_end(long cur_entry, long section_entries, long cur_item,
                          QListWidget *ui_elem, QStringList tmp_data,
                          QString cur_item_name);
   static std::tuple<long, int, QStringList>
   item_entry_lineitems(QStringList spc_data, int line_num, long reset_ind,
                        long entries, long headers);
   static void mult_setText(QLineEdit *ui_elem[], int array_length,
                            QStringList current_data, int data_inds[]);

   inline static const QStringList eulerInputs = {"121", "123", "131", "132",
                                                  "212", "213", "231", "232",
                                                  "312", "313", "321", "323"};
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
      int ind = box->findText(text);
      if (remove && ind != -1) {
         if (box->currentIndex() == ind) {
            box->setCurrentIndex(-1);
         }
         box->removeItem(ind);
      } else
         box->setCurrentIndex(ind);
   }

   private:
   inline static QString scSectionIdentifier(scSectionType type) {
      switch (type) {
         case scSectionType::BODY:
            return "Body Parameters";
         case scSectionType::WHEEL:
            return "Wheel Parameters";
         case scSectionType::MTB:
            return "MTB Parameters";
         case scSectionType::THRUSTER:
            return "Thruster Parameters";
         case scSectionType::GYRO:
            return "Gyro";
         case scSectionType::MAGNETOMETER:
            return "Magnetometer";
         case scSectionType::CSS:
            return "Coarse Sun Sensor";
         case scSectionType::FSS:
            return "Fine Sun Sensor";
         case scSectionType::STARTRACKER:
            return "Star Tracker";
         case scSectionType::GPS:
            return "GPS";
         case scSectionType::ACCEL:
            return "Accelerometer";
      }
   }

   inline static int scSectionLineToNum(scSectionType type) {
      switch (type) {
         case scSectionType::BODY:
            return 2;
         case scSectionType::WHEEL:
            return 3;
         case scSectionType::MTB:
            return 1;
         case scSectionType::THRUSTER:
            return 1;
         case scSectionType::GYRO:
            return 1;
         case scSectionType::MAGNETOMETER:
            return 1;
         case scSectionType::CSS:
            return 1;
         case scSectionType::FSS:
            return 1;
         case scSectionType::STARTRACKER:
            return 1;
         case scSectionType::GPS:
            return 1;
         case scSectionType::ACCEL:
            return 1;
      }
   }
};

#endif // DSM_GUI_LIB_H
