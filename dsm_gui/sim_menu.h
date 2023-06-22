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
    void populate_lists();

    void string2radiobool(QString boolString, QButtonGroup *buttonGroup);
    void setQComboBox(QComboBox *comboBox, QString string);

    QStringList getTextFromList(QListWidget *list);

private:
    Ui::SIM_Menu *ui;

    int global_orb_index = -1;
    int global_orb_ignore = 0;

    QString inoutPath;
    QString filePath;
    QStringList simData;
    QStringList simString;
    QStringList simFileHeaders; // section headers in the file
    QStringList simFileDescrip; // data descriptors in the file
    QStringList simUpdate;

    QString simFile;

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

    QHash<QString, bool> orbitList; //
    QHash<QString, bool> scList; // ui->simSCList will have order, will need to pull it out
    QHash<QString, QString> scOrbs; //

};

#endif // SIM_MENU_H
