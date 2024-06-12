#include "dsm_gui_lib.h"
#include <QMessageBox>
#include <QComboBox>

dsm_gui_lib::dsm_gui_lib() {

}

int dsm_gui_lib::warning_message(QString warningText) {
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

int dsm_gui_lib::error_message(QString errorText) {
    QMessageBox errorMsg;
    errorMsg.setIcon(QMessageBox::Critical);
    errorMsg.setText(errorText);
    errorMsg.setStandardButtons(QMessageBox::Ok);
    int ret = errorMsg.exec();
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

QStringList dsm_gui_lib::getTextFromList(QListWidget *list){
    QStringList output = {};
    QList<QListWidgetItem*> listItems = list->findItems("*",Qt::MatchWildcard);
    for (QListWidgetItem *item : listItems)
        output.append(item->text());

    output.sort(Qt::CaseInsensitive);

    return output;
}

int dsm_gui_lib::get_sc_nitems(const QString inout_path, const QString sc_name, const scSectionType type) {
    QString scFileName = "SC_"+sc_name+".txt";

    QFile scFile(inout_path + scFileName);
    if(!scFile.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", scFile.errorString());
    QTextStream in(&scFile);

    QString searchStr = scSectionIdentifier(type);
    int skipLines = scSectionLineToNum(type);
    int nItems = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains(searchStr,Qt::CaseSensitive)) {
            for (int i=0; i<skipLines; i++)
                line = in.readLine();
            QStringList line_items = line.remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
            nItems = line_items[0].toInt();
            break;
        }
    }
    scFile.close();

    return std::max(nItems,0);

}

void dsm_gui_lib::set_mult_validators(QLineEdit *ui_elem[], int array_length, double lower, double upper, int decimals){
    for (int i = 0; i < array_length; i++){
        ui_elem[i]->setValidator(new QDoubleValidator(lower, upper, decimals));
    }
}

void dsm_gui_lib::set_mult_name_validators(QLineEdit *ui_elem[], int array_length, QValidator *validator){
    for (int i = 0; i < array_length; i++){
        ui_elem[i]->setValidator(validator);
    }
}

void dsm_gui_lib::set_mult_cbox_validators(QComboBox *ui_elem[], int array_length, const QStringList string_list){
    for (int i = 0; i < array_length; i++){
        ui_elem[i]->addItems(dsm_gui_lib::eulerInputs);
    }
}
