#ifndef GRH_MENU_H
#define GRH_MENU_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class GRH_Menu;
}

class GRH_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit GRH_Menu(QWidget *parent = nullptr);
    ~GRH_Menu();

private slots:
    void set_validators();
    void receive_path(QString);
    void receive_data();
    void apply_data();
    void write_data();
    int warning_message(QString);
    QString whitespace(QString);

    void on_boresightaxis_activated(int index);
    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

private:
    Ui::GRH_Menu *ui;

    QString inout_path;
    QString graphics_path;
    QStringList grh_data;
    QStringList grh_update;
    QStringList grh_string;
    QStringList upaxis_inputs;

    QStringList povMode_inputs = {"TRACK_HOST", "TRACK_TARGET", "FIXED_IN_HOST"};
    QStringList hosttarget_inputs = {"WORLD", "REFORB", "FRM", "SC", "BODY"};
    QStringList frame_inputs = {"N","L","F","S","B"};
    QStringList axis_inputs = {"POS_X","POS_Y","POS_Z","NEG_X","NEG_Y","NEG_Z"};
    QStringList view_inputs = {"FRONT","FRONT_RIGHT","FRONT_LEFT","REAR","REAR_LEFT","REAR_RIGHT","UP","DOWN","LEFT","RIGHT"};

};

#endif // GRH_MENU_H
