#ifndef SIM_MENU_H
#define SIM_MENU_H

#include <dsm_gui_lib.h>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>

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

    void on_simGSListRemove_clicked();

    void on_simGSListAdd_clicked();

    void on_simGSListDuplicate_clicked();

    void on_simF107Ap_currentTextChanged(const QString &arg1);

    void on_simMagfieldType_currentTextChanged(const QString &arg1);

    void on_simGSMinorBodyNum_textChanged(const QString &arg1);

    void on_simAeroPertShadow_toggled(bool checked);

    void on_simSRPPertShadow_toggled(bool checked);

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

    enum class headerLineNames { // Doing this as a class so the values can be used as variables elsewhere
        HEADER,
        CONTROL,
        ORBITS,
        SPACECRAFT,
        ENVIRONMENT,
        BODIES,
        LAGRANGE,
        GROUND
    };

    // Here is the only place that we need to change text if inp sim headers change
    static QString toString(const headerLineNames h) {
        switch (h) {
        case headerLineNames::HEADER: return "HEADER";
        case headerLineNames::CONTROL: return "CONTROL";
        case headerLineNames::ORBITS: return "ORBITS";
        case headerLineNames::SPACECRAFT: return "SPACECRAFT";
        case headerLineNames::ENVIRONMENT: return "ENVIRONMENT";
        case headerLineNames::BODIES: return "BODIES";
        case headerLineNames::LAGRANGE: return "LAGRANGE";
        case headerLineNames::GROUND: return "GROUND";
        }
    }

    enum orbListData {
        orbEnabledRole = Qt::UserRole,
        orbNumberRole
    };

    enum scListData {
        scEnabledRole = Qt::UserRole,
        scOrbNameRole
    };

    enum gsListData {
        gsEnabledRole = Qt::UserRole,
        gsWorldRole,
        gsLongRole,
        gsLatRole
    };

    QHash<QString, int> headerLines = {{toString(headerLineNames::HEADER),-1},
                                       {toString(headerLineNames::CONTROL),-1},
                                       {toString(headerLineNames::ORBITS),-1},
                                       {toString(headerLineNames::SPACECRAFT),-1},
                                       {toString(headerLineNames::ENVIRONMENT),-1},
                                       {toString(headerLineNames::BODIES),-1},
                                       {toString(headerLineNames::LAGRANGE),-1},
                                       {toString(headerLineNames::GROUND),-1}};

    QHash<QString, QString> orbFileHash;
    QHash<QString, QString> scFileHash;

};

#endif // SIM_MENU_H
