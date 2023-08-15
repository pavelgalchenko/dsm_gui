#ifndef DSM_GUI_LIB_H
#define DSM_GUI_LIB_H

#include "qlistwidget.h"
#include <QDialog>
#include <QCheckBox>
#include <QVariant>
#include <QRandomGenerator>

class dsm_gui_lib
{
public:
    dsm_gui_lib();

    static int warning_message(QString warningText);
    static int error_message(QString errorText);
    static QString whitespace(QString data);
    static QStringList sortStringList(QStringList unsorted);
    static QStringList getTextFromList(QListWidget *list);
    static QString toString(QCheckBox* box) {
        QString output = QVariant(box->isChecked()).toString().toUpper();
        if (box->isChecked()) output += " ";
        return output;
    }

    inline static const QStringList eulerInputs = {"121", "123", "131", "132", "212", "213", "231", "232", "312", "313", "321", "323"};
    inline static const QStringList worldInputs = {"SOL","MERCURY","VENUS","EARTH","MARS","JUPITER","SATURN","URANUS",
                                                   "NEPTUNE","PLUTO","LUNA","PHOBOS","DEIMOS","IO","EUROPA","GANYMEDE","CALLISTO",
                                                   "AMALTHEA","HIMALITA","ELARA","PASIPHAE","SINOPE","LYSITHEA","CARME","ANANKE",
                                                   "LEDA","THEBE","ADRASTEA","METIS","MIMAS","ENCELADUS","TETHYS","DIONE","RHEA",
                                                   "TITAN","HYPERION","IAPETUS","PHOEBE","JANUS","EPIMETHEUS","HELENE","TELESTO",
                                                   "CALYPSO","ATLAS","PROMETHEUS","PANDORA","PAN","ARIEL","UMBRIEL",
                                                   "TITANIA","OBERON","MIRANDA","TRITON","NERIED","CHARON","MINORBODY"};


    static int inexplicable_error_message() {
        const QStringList inexplicableErrorMsg = {"In your attempt to fly, you failed to miss the ground...",
                                                  "You appear to have misplaced your towel...",
                                                  "If you are reading this, you are not a cool frood...",
                                                  "You have the distinct feeling of your brain being smashed out by a slice of lemon wrapped around a large gold brick...",
                                                  "The sound of Vogon poetry grows in the distance...",
                                                  "Oh no... Not again..."};
        static int lineCount = inexplicableErrorMsg.count();

        int randomNum = QRandomGenerator::global()->bounded(lineCount);
        return error_message(inexplicableErrorMsg[randomNum]);
    }
};

//const QStringList dsm_gui_lib::eulerInputs
//const QStringList dsm_gui_lib::worldInputs

#endif // DSM_GUI_LIB_H
