#ifndef DSM_GUI_LIB_H
#define DSM_GUI_LIB_H

#include <QDialog>
#include <QComboBox>

class dsm_gui_lib
{
public:
    dsm_gui_lib();

    static int warning_message(QString warningText);
    static int error_message(QString errorText);
    static QString whitespace(QString data);
    static QStringList sortStringList(QStringList unsorted);
};

#endif // DSM_GUI_LIB_H
