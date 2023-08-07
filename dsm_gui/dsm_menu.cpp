#include "dsm_menu.h"
#include "ui_dsm_menu.h"

DSM_Menu::DSM_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DSM_Menu)
{
    ui->setupUi(this);
}

DSM_Menu::~DSM_Menu()
{
    delete ui;
}

/* Edit this function to change the headers in Inp_DSM.txt */
QStringList DSM_Menu::secDescription(const dsmSectionTypes type) {
    QStringList descrip;
    descrip.clear();

    switch (type) {
    case dsmSectionTypes::COMMANDS:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                              Spacecraft Commands");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)");
        descrip.append("# Col: 2   ->  Spacecraft ID Number: \"SC[#]\"");
        descrip.append("# Col: 3   ->  Command Sequence Flag: \"FswMode\" or \"CmdTime\"");
        descrip.append("# Col: 4   ->  Command Sequence Parameter: FSW Mode ID or Command Execution Time");
        descrip.append("# Col: 5   ->  Translation Control Method_Mode (\"PASSIVE\" for none, \"NO_CHANGE\" for no change)");
        descrip.append("# Col: 6   ->  Attitude Control Method_Mode (\"PASSIVE\" for none)");
        descrip.append("# Col: 7   ->  Optional Actuator Command_Mode");
        break;
    case dsmSectionTypes::TRANSLATION:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                             Translation Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"TranslationCmd_[#]\"");
        descrip.append("# Col: 2   ->  Position 1: X (meters)");
        descrip.append("# Col: 3   ->  Position 2: Y (meters)");
        descrip.append("# Col: 4   ->  Position 3: Z (meters)");
        descrip.append("# Col: 5   ->  Ref Origin (ctrl wrt): Orbit Point \"OP\" or SC ID Number (\"SC[#]\")");
        descrip.append("# Col: 6   ->  Ref Frame Directions: \"N\", \"F\", \"L\", or SC ID Number (\"SC[#]\")");
        descrip.append("# Col: 8   ->  Set of Control Gains");
        descrip.append("# Col: 9   ->  Set of Control Limits");
        descrip.append("# Col: 10  ->  Actuator Mode");
        break;
    case dsmSectionTypes::PRIMARY_VEC:
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#                              Attitude Command");
        descrip.append("#-------------------------------------------------------------------------------");
        descrip.append("#------------------------------Primary Axis Cmd---------------------------------");
        descrip.append("# Col: 1   ->  Command Interp ID Flag: \"AttitudeCmd_PV[#]\"");
        descrip.append("# Col: 2   ->  Primary Target Type: BODY, SC, VEC");
        descrip.append("# Col: 3   ->  Primary Axis Direction 1");
        descrip.append("# Col: 4   ->  Primary Axis Direction 2");
        descrip.append("# Col: 5   ->  Primary Axis Direction 3");
        descrip.append("# Col: 6   ->  Primary Target: Specify Body, SC[#] or Vec");
        descrip.append("# Col: 7   ->  Set of Control Gains");
        descrip.append("# Col: 8   ->  Set of Control Limits");
        descrip.append("# Col: 9   ->  Actuator Mode");
        descrip.append("# If Col: 2 -> VEC, then Col: 6 is the Ref. Frame of the pointing vector: \"N\", \"F\", \"L\", \"B\"");
        descrip.append("#                        Col: 7-9 are the pointing vec for Primary axis");
        descrip.append("#                        Col: 10-12 are Control Gain, Limit and Actuator Sets");
        break;
    case dsmSectionTypes::SECONDARY_VEC:
        break;
    case dsmSectionTypes::QUATERION:
        break;
    case dsmSectionTypes::MIRROR:
        break;
    case dsmSectionTypes::DETUMBLE:
        break;
    case dsmSectionTypes::ACTUATOR_CMD:
        break;
    case dsmSectionTypes::CONTROLLERS:
        break;
    case dsmSectionTypes::ACTUATORS:
        break;
    case dsmSectionTypes::GAINS:
        break;
    case dsmSectionTypes::LIMITS:
        break;
    case dsmSectionTypes::MANEUVER:
        break;
    case dsmSectionTypes::MOMENTUM_DUMP:
        break;

    }

    return descrip;
}
