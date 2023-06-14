#ifndef ORB_MENU_H
#define ORB_MENU_H

#include "dsm_gui_lib.h"

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
//#include <QHash>
#include <QRadioButton>
#include <QComboBox>

namespace Ui {
class ORB_Menu;
}

class ORB_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit ORB_Menu(QWidget *parent = nullptr);
    ~ORB_Menu();

private slots:
    void initialize_maps();
    void set_validators();
    void receive_orbpath(QString);
    void receive_data();
    void apply_data();
    void write_data();

//    int warning_message(QString);
//    QString whitespace(QString);
    QString radiobool2string(QButtonGroup *buttonGroup);
    void string2radiobool(QString boolString, QButtonGroup *buttonGroup);
    void setQComboBox(QComboBox *comboBox, QString string);
    QStringList multiMapValue2QStringList(QMultiMap<int, QString> map);

    void on_orbListRemove_clicked();
    void on_orbListAdd_clicked();
    void on_orbList_itemClicked(QListWidgetItem *item);

    void on_loadDefaultButton_clicked();
    void on_saveDefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void on_orbType_currentIndexChanged(int index);

    void on_orbCentICParam_currentIndexChanged(int index);

    void on_orbTBodyICParam_currentIndexChanged(int index);

    void on_orbZeroWorld_currentTextChanged(const QString &arg1);

    void on_orbCentWorld_currentTextChanged(const QString &arg1);

    void on_orbCentFileSelect_clicked();
    void on_orbTBodyFileSelect_clicked();

private:
    Ui::ORB_Menu *ui;

    int global_orb_index = -1;
    int global_orb_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList file_paths;
    QStringList orb_names;
    int orb_name_index;
    int orb_name_size;
    QStringList orb_data;
    QStringList orb_file_headers; // section headers in the file
    QStringList orb_file_descrip; // data descriptors in the file
    QStringList orb_update;
    QStringList orb_string;

    const QStringList eulerSeqInputs = {"123", "231", "312", "132", "213", "321", "121", "131", "212", "232", "313","323"};
    const QStringList worldInputs = {"SOL","MERCURY","VENUS","EARTH","MARS","JUPITER","SATURN","URANUS",
                                     "NEPTUNE","PLUTO","LUNA","PHOBOS","DEIMOS","IO","EUROPA","GANYMEDE","CALLISTO",
                                     "AMALTHEA","HIMALITA","ELARA","PASIPHAE","SINOPE","LYSITHEA","CARME","ANANKE",
                                     "LEDA","THEBE","ADRASTEA","METIS","MIMAS","ENCELADUS","TETHYS","DIONE","RHEA",
                                     "TITAN","HYPERION","IAPETUS","PHOEBE","JANUS","EPIMETHEUS","HELENE","TELESTO",
                                     "CALYPSO","ATLAS","PROMETHEUS","PANDORA","PAN","ARIEL","UMBRIEL",
                                     "TITANIA","OBERON","MIRANDA","TRITON","NERIED","CHARON","MINORBODY"};
    QMultiMap<int, QString> orbTypeInputs;
    QMultiMap<int, QString> orbTBodyLSysInputs;
    QMultiMap<int, QString> orbCentICTypeInputs;
    QMultiMap<int, QString> orbTBodyPropInputs;
    QMultiMap<int, QString> orbTBodyICTypeInputs;
    QMultiMap<int, QString> orbFileTypeInputs;

    const QStringList orbFrameInputs = {"N", "L"};
    const QStringList lagrangePointInputs   = {"L1", "L2", "L3", "L4", "L5"};

};

#endif // ORB_MENU_H
