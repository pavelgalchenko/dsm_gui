#ifndef FOV_MENU_H
#define FOV_MENU_H

#include "qcheckbox.h"
#include "dsm_gui_lib.h"

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QColorDialog>

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
    void receive_data();
    void apply_data();
    void write_data();

    void on_fov_remove_clicked();
    void on_fov_add_clicked();
    void on_fovlist_itemClicked(QListWidgetItem *item);
    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void on_pickcolor_clicked();

    void sides_changed();
    void dims_changed();
    void color_changed();
    void on_fov_name_textChanged(const QString &arg1);
    void field_changed();
    void scbody_changed();
    void pos_changed();
    void euler_changed();

    void on_boresightaxis_currentTextChanged(const QString &arg1);

    void on_fov_type_currentTextChanged(const QString &arg1);
    void clear_fields();

    void on_fov_duplicate_clicked();

    QStringList getTextFromList(QListWidget *list);

    void on_sc_name_currentTextChanged(const QString &arg1);

private:
    Ui::FOV_Menu *ui;

    enum fovDataRoles {
        Label = Qt::DisplayRole,
        Sides = Qt::UserRole, // 2 element, n sides/ side length
        Dims, // 2 element ; h width/ v height
        Color, // 4 element ; rgb+alpha
        Type,
        DrawField, // 2 element ; near/far
        SCBody,
        BodyPos, // 3 element
        Euler, // 4 element; euler angs/sequence
        Boresight
    };

    const int fovNLines = 11;
    QHash<QString,int> scNums;

    QString inout_path;
    QString file_path;
    QStringList fov_data;
    QStringList fov_update;
    QStringList fov_string;

    const QHash<QString, QString> axis_inputs = {{"X_AXIS", "x-Axis"},
                                                 {"Y_AXIS", "y-Axis"},
                                                 {"Z_AXIS", "z-Axis"}};
    const QHash<QString, QString> fovtype_inputs = {{"WIREFRAME", "Wireframe"},
                                                    {"SOLID", "Solid"},
                                                    {"VECTOR", "Vector"},
                                                    {"PLANE", "Plane"}};
    const QStringList euler_inputs = {"121","123","131","132","212","213","231","232","312","313","321","323"};

};

#endif // FOV_MENU_H
