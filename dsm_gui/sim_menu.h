#ifndef SIM_MENU_H
#define SIM_MENU_H

#include <dsm_gui_lib.h>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>

namespace Ui {
class SIM_Menu;
}

class SIM_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit SIM_Menu(QWidget *parent = nullptr);
    ~SIM_Menu();

private slots:
    void set_validators();
    void receive_simpath(QString path);
    void receive_data();
    void apply_data();
    void clear_data();
    void write_data();

    void on_loadDefaultButton_clicked();
    void on_saveDefaultButton_clicked();
    void on_closeButton_clicked();
    void on_applyButton_clicked();

    void setQComboBox(QComboBox *comboBox, QString string);

    QStringList getTextFromList(QListWidget *list);

    void on_simOrbList_itemClicked(QListWidgetItem *item);

    void on_simOrbitEn_toggled(bool checked);

    void on_simSCList_itemClicked(QListWidgetItem *item);

    void on_simSCEn_toggled(bool checked);

    void on_simSCOrbit_currentTextChanged(const QString &arg1);

    void on_simGSList_itemClicked(QListWidgetItem *item);

    void on_simGSEn_toggled(bool checked);

    void on_simGSWorld_currentTextChanged(const QString &arg1);

    void on_simGSLat_textEdited(const QString &arg1);

    void on_simGSLong_textEdited(const QString &arg1);

    void on_simGSLabel_textEdited(const QString &arg1);

private:
    Ui::SIM_Menu *ui;

    QString inoutPath;
    QString filePath;
    QStringList simData;
    QStringList simString;
    QStringList simFileHeaders; // section headers in the file
    QStringList simFileDescrip; // data descriptors in the file
    QStringList simUpdate;

    QString orbDescription, scDescription, gsDescription;

    QString simFile;

    QList<QCheckBox*> celestialBodies;
    QList<QCheckBox*> lagrangeSystems;

    const QStringList worldInputs = {"SOL","MERCURY","VENUS","EARTH","MARS","JUPITER","SATURN","URANUS",
                                     "NEPTUNE","PLUTO","LUNA","PHOBOS","DEIMOS","IO","EUROPA","GANYMEDE","CALLISTO",
                                     "AMALTHEA","HIMALITA","ELARA","PASIPHAE","SINOPE","LYSITHEA","CARME","ANANKE",
                                     "LEDA","THEBE","ADRASTEA","METIS","MIMAS","ENCELADUS","TETHYS","DIONE","RHEA",
                                     "TITAN","HYPERION","IAPETUS","PHOEBE","JANUS","EPIMETHEUS","HELENE","TELESTO",
                                     "CALYPSO","ATLAS","PROMETHEUS","PANDORA","PAN","ARIEL","UMBRIEL",
                                     "TITANIA","OBERON","MIRANDA","TRITON","NERIED","CHARON","MINORBODY"};

    const QHash<QString, QString> timeModeInputs = {{"FAST", "Fast"},
                                                    {"REAL", "Real"},
                                                    {"EXTERNAL", "External"},
                                                    {"NOS3", "NOS3"}};

    const QHash<QString, QString> f107Inputs = {{"USER", "User"},
                                                {"NOMINAL", "Nominal"},
                                                {"TWOSIGMA", "2σ"}};

    const QHash<QString, QString> magfieldInputs = {{"NONE", "None"},
                                                    {"DIPOLE", "Dipole"},
                                                    {"IGRF", "IGRF"}};

    const QHash<QString, QString> ephemInputs = {{"MEAN", "Mean"},
                                                 {"DE430", "DE430"},
                                                 {"DE440", "DE440"}};

    QHash<QString, int> headerLines = {{"HEADER",-1},
                                      {"CONTROL",-1},
                                      {"ORBITS",-1},
                                      {"SPACECRAFT",-1},
                                      {"ENVIRONMENT",-1},
                                      {"BODIES",-1},
                                      {"LAGRANGE",-1},
                                      {"GROUND",-1}};

    QHash<QString, QString> orbFileHash;
    QHash<QString, QString> scFileHash;

};

#endif // SIM_MENU_H
