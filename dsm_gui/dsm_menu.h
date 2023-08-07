#ifndef DSM_MENU_H
#define DSM_MENU_H

#include <QDialog>

namespace Ui {
class DSM_Menu;
}

class DSM_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit DSM_Menu(QWidget *parent = nullptr);
    ~DSM_Menu();

private:
    Ui::DSM_Menu *ui;

    QString inoutPath;
    QString filePath;
    QStringList dsmData;
    QStringList dsmString;
    QStringList dsmFileHeaders; // section headers in the file
    QStringList dsmFileDescrip; // data descriptors in the file
    QStringList dsmUpdate;

    enum class dsmSectionTypes {
        COMMANDS,
        TRANSLATION,
        PRIMARY_VEC,
        SECONDARY_VEC,
        QUATERION,
        MIRROR,
        DETUMBLE,
        ACTUATOR_CMD,
        CONTROLLERS,
        ACTUATORS,
        GAINS,
        LIMITS,
        MANEUVER,
        MOMENTUM_DUMP
    };

    static QStringList secDescription(const dsmSectionTypes type);

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
