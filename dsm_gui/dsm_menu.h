#ifndef DSM_MENU_H
#define DSM_MENU_H

#include "qtreewidget.h"
#include <dsm_gui_lib.h>

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>

namespace Ui {
class DSM_Menu;
}

class DSM_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit DSM_Menu(QWidget *parent = nullptr);
    ~DSM_Menu();

private slots:
    void set_validators();
    void receive_dsmpath(QString);
    void receive_data();
    void apply_data();
    void write_data();


private:
    Ui::DSM_Menu *ui;

    QString inoutPath;
    QString filePath;
    QStringList dsmData;
    QStringList dsmString;
    QStringList dsmFileHeaders; // section headers in the file
    QStringList dsmFileDescrip; // data descriptors in the file
    QStringList dsmUpdate;

    QStringList scNames;

    enum class dsmSectionTypes {
        COMMANDS,
        TRANSLATION,
        PRIMARY_VEC,
        SECONDARY_VEC,
        QUATERION,
        MIRROR,
        DETUMBLE,
        WHLHMANAGEMENT,
        ACTUATOR_CMD,
        CONTROLLERS,
        ACTUATORS,
        GAINS,
        LIMITS,
        MANEUVER,
        MOMENTUM_DUMP,
    };
    const QList<dsmSectionTypes> searchOrd = {dsmSectionTypes::LIMITS,dsmSectionTypes::GAINS,dsmSectionTypes::ACTUATORS,dsmSectionTypes::CONTROLLERS,
                                              dsmSectionTypes::MANEUVER,dsmSectionTypes::ACTUATOR_CMD,dsmSectionTypes::WHLHMANAGEMENT,dsmSectionTypes::DETUMBLE,
                                              dsmSectionTypes::MIRROR,dsmSectionTypes::QUATERION,dsmSectionTypes::SECONDARY_VEC,dsmSectionTypes::PRIMARY_VEC,
                                              dsmSectionTypes::TRANSLATION,dsmSectionTypes::COMMANDS};

    const QList<dsmSectionTypes> cmdTypes = {dsmSectionTypes::TRANSLATION,dsmSectionTypes::PRIMARY_VEC,dsmSectionTypes::SECONDARY_VEC,dsmSectionTypes::QUATERION,
                                             dsmSectionTypes::MIRROR,dsmSectionTypes::DETUMBLE,dsmSectionTypes::WHLHMANAGEMENT,dsmSectionTypes::ACTUATOR_CMD,
                                             dsmSectionTypes::MANEUVER};

    static QStringList secDescription(const dsmSectionTypes type);
    static QString entryItemName(const dsmSectionTypes type);
    static QString entryItemRegEx(const dsmSectionTypes type);
    static QString entryItemFormat(const dsmSectionTypes type);

    void new_entry_item(const dsmSectionTypes, const QString);
    QHash<dsmSectionTypes,QTreeWidgetItem*> entryCmdParents;

    enum timelineItemData {
        timelineSC = Qt::UserRole, // Might not need, might be able to reference parent item
        timelineTime,
        timelineTrnCmd,
        timelineAttCmd,
        timelineActCmd
    };

    enum cmdData {
        cmdType = Qt::UserRole, // Might not need, might be able to reference parent item
        cmdController,
        cmdActuator,
        cmdData
    };

    enum ctrlData {
        ctrlType = Qt::UserRole,
        ctrlGains,
        ctrlLimits
    };

    enum gainsData {
        gainsType = Qt::UserRole,
        gainsData
    };

    enum limData {
        limData = Qt::UserRole
    };
};

#endif // DSM_MENU_H
