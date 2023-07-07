#ifndef FOV_MENU_H
#define FOV_MENU_H

#include "color_select.h"

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>

namespace Ui {
class FOV_Menu;
}

class FOV_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit FOV_Menu(QWidget *parent = nullptr);
    ~FOV_Menu();

signals:
    void send_rgbavalues(QStringList);

private slots:
    void set_validators();
    void receive_fovpath(QString);
    void receive_newrgbavalues(QStringList);
    void receive_data();
    void apply_data();
    void populate_list();
    void write_data();
    int warning_message(QString);
    QString whitespace(QString);

    void on_fov_remove_clicked();
    void on_fov_add_clicked();
    void on_fovlist_itemClicked(QListWidgetItem *item);
    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void on_pickcolor_clicked();

private:
    Ui::FOV_Menu *ui;

    COLOR_Select *color_menu;

    int global_fov_index = -1;
    int global_fov_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList fov_names;
    QStringList fov_data;
    QStringList fov_update;
    QStringList fov_string;

    QStringList axis_inputs = {"X_AXIS","Y_AXIS","Z_AXIS"};
    QStringList fovtype_inputs = {"WIREFRAME","SOLID","VECTOR","PLANE"};
    QStringList euler_inputs = {"121","123","131","132","212","213","231","232","312","313","321","323"};

};

#endif // FOV_MENU_H
