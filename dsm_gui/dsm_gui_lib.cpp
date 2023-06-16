#include "dsm_gui_lib.h"
#include <QMessageBox>
#include <QComboBox>

dsm_gui_lib::dsm_gui_lib()
{

}

int dsm_gui_lib::warning_message(QString warningText) {
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

QString dsm_gui_lib::whitespace(QString data) {
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}

QStringList dsm_gui_lib::sortStringList(QStringList unsorted) {
    QStringList sorted;
    unsorted.sort();
    sorted = unsorted;
    return sorted;
}

