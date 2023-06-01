#ifndef RGN_MENU_H
#define RGN_MENU_H

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
    void populate_list();
    void write_data();
    int warning_message(QString);
    QString whitespace(QString);

    void on_rgn_remove_clicked();
    void on_rgn_add_clicked();
    void on_rgnlist_itemClicked(QListWidgetItem *item);
    void on_loaddefaultButton_clicked();
    void on_savedefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void on_world_currentIndexChanged(int index);

private:
    Ui::RGN_Menu *ui;

    int global_rgn_index = -1;
    int global_rgn_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList rgn_names;
    QStringList rgn_data;
    QStringList rgn_update;
    QStringList rgn_string;

    QStringList location_inputs = {"POSW","LLA"};
    QStringList world_inputs = {"SOL","MERCURY","VENUS","EARTH","MARS","JUPITER","SATURN","URANUS",
                               "NEPTUNE","PLUTO","LUNA","PHOBOS","DEIMOS","IO","EUROPA","GANYMEDE","CALLISTO",
                               "AMALTHEA","HIMALITA","ELARA","PASIPHAE","SINOPE","LYSITHEA","CARME","ANANKE",
                               "LEDA","THEBE","ADRASTEA","METIS","MIMAS","ENCELADUS","TETHYS","DIONE","RHEA",
                               "TITAN","HYPERION","IAPETUS","PHOEBE","JANUS","EPIMETHEUS","HELENE","TELESTO",
                               "CALYPSO","ATLAS","PROMETHEUS","PANDORA","PAN","ARIEL","UMBRIEL","UMBRIEL",
                               "TITANIA","OBERON","MIRANDA","TRITON","NERIED","CHARON","MINORBODY"};

};

#endif // RGN_MENU_H
