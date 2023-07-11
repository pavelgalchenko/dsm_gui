#ifndef RGN_MENU_H
#define RGN_MENU_H

#include "dsm_gui_lib.h"

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>

namespace Ui {
class RGN_Menu;
}

class RGN_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit RGN_Menu(QWidget *parent = nullptr);
    ~RGN_Menu();

private slots:
    void set_validators();
    void receive_rgnpath(QString);
    void receive_data();
    void apply_data();
    void write_data();

    void on_rgn_remove_clicked();
    void on_rgn_add_clicked();
    void on_rgnlist_itemClicked(QListWidgetItem *item);
    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void clear_fields();

    void world_changed();
    void location_changed();
    void posw_changed();
    void lla_changed();
    void coeffs_changed();
    void geometry_changed();

    void on_exists_toggled(bool checked);

    void on_regionname_textChanged(const QString &arg1);

    void on_rgn_duplicate_clicked();

    void on_location_currentTextChanged(const QString &arg1);

private:
    Ui::RGN_Menu *ui;

    enum rgnDataRoles {
        Name = Qt::DisplayRole,
        Exists = Qt::UserRole,
        World,
        Location,
        PosW,
        LLA,
        Coeffs,
        GeometryFile
    };

    const int rgnNLines = 9;

    QString inout_path;
    QString file_path;
    QStringList rgn_data;
    QStringList rgn_update;
    QStringList rgn_string;

    const QHash<QString, QString> location_inputs = {{"POSW","Position in World"},
                                                    {"LLA","Lat, Long, Alt"}};
    const QStringList world_inputs = {"SOL","MERCURY","VENUS","EARTH","MARS","JUPITER","SATURN","URANUS",
                               "NEPTUNE","PLUTO","LUNA","PHOBOS","DEIMOS","IO","EUROPA","GANYMEDE","CALLISTO",
                               "AMALTHEA","HIMALITA","ELARA","PASIPHAE","SINOPE","LYSITHEA","CARME","ANANKE",
                               "LEDA","THEBE","ADRASTEA","METIS","MIMAS","ENCELADUS","TETHYS","DIONE","RHEA",
                               "TITAN","HYPERION","IAPETUS","PHOEBE","JANUS","EPIMETHEUS","HELENE","TELESTO",
                               "CALYPSO","ATLAS","PROMETHEUS","PANDORA","PAN","ARIEL","UMBRIEL","UMBRIEL",
                               "TITANIA","OBERON","MIRANDA","TRITON","NERIED","CHARON","MINORBODY"};

};

#endif // RGN_MENU_H
