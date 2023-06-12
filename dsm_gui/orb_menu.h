#ifndef ORB_MENU_H
#define ORB_MENU_H

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
    void set_validators();
    void receive_orbpath(QString);
    void receive_data();
    void apply_data();
    void write_data();
    void populate_list();
//    void write_data();
    QString whitespace(QString);
    QString radiobool2string(QRadioButton *onButton);
    void string2radiobool(QString boolString, QRadioButton *onButton, QRadioButton *offButton);
    void setQComboBox(QComboBox *comboBox, QString string);
    QStringList hashValue2QStringList(QHash<QString, QString> hash);

    void on_orbListRemove_clicked();
    void on_orbListAdd_clicked();

private:
    Ui::ORB_Menu *ui;

    int global_orb_index = -1;
    int global_orb_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList file_paths;
    QStringList orb_names;
    QVector<int> orb_name_index;
    QVector<int> orb_name_size;
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
    const QHash<QString, QString> orbTypeInputs = {{"ZERO","Zero"},
                                                   {"FLIGHT","Flight"},
                                                   {"CENTRAL","Central"},
                                                   {"THREE_BODY","Three Body"}};
    const QHash<QString, QString> orbTBodyLSysInputs = {{"EARTHMOON","Earth/Moon"},
                                                        {"SUNEARTH","Sun/Earth"},
                                                        {"SUNJUPITER","Sun/Jupiter"}};
    const QHash<QString, QString> orbCentICTypeInputs = {{"KEP","Keplerian"},
                                                         {"RV","Position/Velocity"},
                                                         {"FILE","File"}};
    const QHash<QString, QString> orbTBodyPropInputs = {{"LAGDOF_MODES","Modes"},
                                                        {"LAGDOF_COWELL","Cowell"},
                                                        {"LAGDOF_SPLINE","Spline"}};
    const QHash<QString, QString> orbTBodyICTypeInputs = {{"MODES","Modes"},
                                                          {"XYZ","Position/Velocity"},
                                                          {"FILE","File"}};
    const QHash<QString, QString> orbFileTypeInputs = {{"TLE","TLE"},
                                                       {"TRV","TRV"},
                                                       {"SPLINE","Spline"}};
    const QStringList orbFrameInputs = {"N", "L"};
    const QStringList lagrangePointInputs   = {"L1", "L2", "L3", "L4", "L5"};

};

#endif // ORB_MENU_H
