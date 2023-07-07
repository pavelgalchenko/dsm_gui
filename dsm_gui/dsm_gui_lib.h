#ifndef DSM_GUI_LIB_H
#define DSM_GUI_LIB_H

#include <QDialog>
#include <QCheckBox>
#include <QVariant>


class dsm_gui_lib
{
public:
    dsm_gui_lib();

    static int warning_message(QString warningText);
    static QString whitespace(QString data);
    static QStringList sortStringList(QStringList unsorted);
    static QString toString(QCheckBox* box) {
        QString output = QVariant(box->isChecked()).toString().toUpper();
        if (box->isChecked()) output += " ";
        return output;
    }
};

#endif // DSM_GUI_LIB_H
