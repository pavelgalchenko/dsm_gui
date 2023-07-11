#ifndef TDR_MENU_H
#define TDR_MENU_H

#include "dsm_gui_lib.h"
#include "qlineedit.h"

#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QValidator>

namespace Ui {
class TDR_Menu;
}

class TDR_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit TDR_Menu(QWidget *parent = nullptr);
    ~TDR_Menu();

private slots:
    void set_validators();
    void receive_tdrpath(QString);
    void receive_data();
    void apply_data();
    void write_data();

    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

private:
    Ui::TDR_Menu *ui;

    QString inout_path;
    QString file_path;
    QStringList tdr_data;
    QStringList tdr_update;
    QStringList tdr_string;

    QList<QCheckBox*> tdrsEnabled;
    QList<QLineEdit*> tdrsNames;

};

#endif // TDR_MENU_H
