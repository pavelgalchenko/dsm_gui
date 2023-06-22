#include "sim_menu.h"
#include "qbuttongroup.h"
#include "ui_sim_menu.h"

SIM_Menu::SIM_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SIM_Menu)
{
    ui->setupUi(this);
}

SIM_Menu::~SIM_Menu()
{
    delete ui;
}

void SIM_Menu::receive_simpath(QString path) {
    inoutPath = path;
    filePath = inoutPath+"Inp_Sim.txt"; // Maybe add ability for multiple sim files?

    receive_data();
    apply_data();
    populate_lists();
}


void SIM_Menu::receive_data() {
    int i = 0;
    simData.clear();
    simString.clear();
    simFileHeaders.clear();
    simFileDescrip.clear();

    // Return everything up to and including ! (exclamation point)
    static QRegularExpression rx1("(.*?)!");

    // Return everything between a set of " " (quotation marks)
    static QRegularExpression rx2("\"(.*?)\"");

    // If the line does NOT start with an alphanumeric character or " (single quotation), then return the line as first group.
    // Otherwise return everything after ! (exclamation point) as second group
    static QRegularExpression rx3("(?:(?=^[^[:alnum:]|\"])([^[:alnum:]|\"].*)|(!.*))");

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    headerLines["HEADER"]=1;
    while(!in.atEnd()) {
        i++;

        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        simData.append(match1.captured(1)); // index 0 includes ! character, index 1 does not

        QRegularExpressionMatch match2 = rx2.match(line);
        simString.append(match2.captured(1)); // index 0 includes "" characters, index 1 does not

        QRegularExpressionMatch match3 = rx3.match(line);
        if (match3.hasMatch()) {
            QString capture = match3.captured(1);
            if (!capture.isEmpty()) {
                capture += "\n";
                foreach (QString key, headerLines.keys()) {
                    if (capture.contains(key,Qt::CaseInsensitive))
                        headerLines[key] = i;
                }
            }
            simFileHeaders.append(capture);
            capture = match3.captured(2);
            if (!capture.isEmpty()) {
                capture += "\n";
            }
            simFileDescrip.append(capture);
        }
    }
    file.close();
}

void SIM_Menu::apply_data() {
    QStringList line_items;
    QString line_string;
    for(int lineNum=1; lineNum<=simData.length(); lineNum++) {
        line_string = simString[lineNum-1];
        line_items = simData[lineNum-1].remove("\"").split(QRegExp("\\s"), Qt::SkipEmptyParts);
        if (lineNum < headerLines["ORBITS"]) {
            switch (lineNum-headerLines["CONTROL"]) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            default:
                break;
            }
        } // Skip over Orbits and Spacecraft;
        else if (lineNum > headerLines["ENVIRONMENT"] && lineNum < headerLines["BODIES"]) {
            switch (lineNum-headerLines["ENVIRONMENT"]) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 11:
                break;
            case 12:
                break;
            case 13:
                break;
            case 14:
                break;
            case 15:
                break;
            case 16:
                break;
            case 17:
                break;
            case 18:
                break;
            case 19:
                break;
            case 20:
                break;
            case 21:
                break;
            default:
                break;
            }
        }
        else if (lineNum > headerLines["BODIES"] && lineNum < headerLines["LAGRANGE"]) {
            switch (lineNum-headerLines["BODIES"]) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 11:
                break;
            default:
                break;
            }
        }
        else if (lineNum > headerLines["LAGRANGE"] && lineNum < headerLines["GROUND"]) {
            switch (lineNum-headerLines["LAGRANGE"]) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                break;
            }
        }
        else if (lineNum > headerLines["GROUND"]) {

        }
    }


}

void SIM_Menu::write_data()
{
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < simUpdate.size(); i++)
            in << simUpdate.at(i);
    }
    simUpdate.clear();
    file.close();
}

void SIM_Menu::string2radiobool(QString boolString, QButtonGroup *buttonGroup) {
    if(!boolString.compare("TRUE",Qt::CaseInsensitive)) { // compare returns zero if equal
        buttonGroup->button(1)->setChecked(true);
    }else {
        buttonGroup->button(0)->setChecked(true);
    }
}

void SIM_Menu::setQComboBox(QComboBox *comboBox, QString string) {
    comboBox->setCurrentIndex(comboBox->findText(string));
}

QStringList SIM_Menu::getTextFromList(QListWidget *list){
    QStringList output;
    foreach(QListWidgetItem *item, list->findItems("*",Qt::MatchWildcard))
        output.append(item->text());
    output.sort(Qt::CaseInsensitive);
    return output;
}






