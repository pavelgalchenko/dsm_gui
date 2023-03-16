#ifndef NOS_MENU_H
#define NOS_MENU_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class NOS_Menu;
}

class NOS_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit NOS_Menu(QWidget *parent = nullptr);
    ~NOS_Menu();

private slots:
    void receive_nospath(QString);
    void receive_data();
    void apply_data();
    void write_data();
    int warning_message(QString);
    QString whitespace(QString);

    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

private:
    Ui::NOS_Menu *ui;

    QString inout_path;
    QString file_path;
    QStringList nos_data;
    QStringList nos_update;
};

#endif // NOS_MENU_H
