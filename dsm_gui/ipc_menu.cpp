#include "ipc_menu.h"
#include "ui_ipc_menu.h"

IPC_Menu::IPC_Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IPC_Menu)
{
    ui->setupUi(this);
    set_validators();
}

IPC_Menu::~IPC_Menu()
{
    delete ui;
}

void IPC_Menu::set_validators() {
    QRegularExpression rx("[^\"]*");

    ui->ipclist->setSortingEnabled(true);
    ui->ipclist->sortItems(Qt::AscendingOrder);

    ui->prefixlist->setSortingEnabled(true);
    ui->prefixlist->sortItems(Qt::AscendingOrder);

    ui->ipcmode->addItems(dsm_gui_lib::sortStringList(ipcmodeinputs.values()));
    ui->socketrole->addItems(dsm_gui_lib::sortStringList(socketrole_inputs.values()));
    ui->filename->setValidator(new QRegularExpressionValidator(rx));
    ui->acs_id->setValidator(new QIntValidator(0,INFINITY));
    ui->portnum->setValidator(new QIntValidator(0,INFINITY));

    connect(ui->servername, &QLineEdit::textChanged, this, &IPC_Menu::server_changed);
    connect(ui->portnum, &QLineEdit::textChanged, this, &IPC_Menu::server_changed);
}

void IPC_Menu::receive_ipcpath(QString path) {
    inout_path = path;
    file_path = path+"Inp_IPC.txt";
    receive_data();
    apply_data();
}

void IPC_Menu::receive_data() {
    ipc_data.clear();
    ipc_string.clear();
    ipc_update.clear();
    static QRegularExpression rx1("(.*?)!");
    static QRegularExpression rx2("\"(.*?)\"");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        ipc_data.append(match1.captured(1));

        QRegularExpressionMatch match2 = rx2.match(line);
        ipc_string.append(match2.captured(1));

        line.append("\n");
    }
    file.close();
}

void IPC_Menu::write_data() {
    QFile::remove(file_path);
    QFile file(file_path);
    if(!file.open(QFile::WriteOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    else {
        QTextStream in(&file);
        for (int i=0; i < ipc_update.size(); i++){
            in << ipc_update.at(i);
        }
    }
    file.close();
    ipc_update.clear();
}

void IPC_Menu::apply_data() {
    const QString ipcName = "IPC ";
    QStringList line_items;
    QString line_string;
    QListWidgetItem* newIPC;
    int num_ipc = 0;

    int current_index = 2;

    QStringList tmpData;

    line_items = ipc_data[1].split(QRegExp("\\s"), Qt::SkipEmptyParts);
    num_ipc = line_items[0].toInt();

    for (int i=0; i < num_ipc; i++) {
        newIPC = new QListWidgetItem();
        newIPC->setData(IPC_Menu::Name,ipcName + QString::number(i));
        for (int j=0; j<ipcNLines; j++) {
            line_items = ipc_data[current_index].split(QRegExp("\\s"), Qt::SkipEmptyParts);
            line_string = ipc_string[current_index];
            switch (j) {
            case 1:
                tmpData.append(line_items[0]);
                newIPC->setData(IPC_Menu::Mode,tmpData);
                break;
            case 2:
                tmpData.append(line_items[0]);
                newIPC->setData(IPC_Menu::ACID,tmpData);
                break;
            case 3:
                tmpData.append(line_string);
                newIPC->setData(IPC_Menu::FileName,tmpData);
                break;
            case 4:
                tmpData.append(line_items[0]);
                newIPC->setData(IPC_Menu::Role,tmpData);
                break;
            case 5:
                tmpData.append(line_items[0]);
                tmpData.append(line_items[1]);
                newIPC->setData(IPC_Menu::Server,tmpData);
                break;
            case 6:
                tmpData.append(line_items[0]);
                newIPC->setData(IPC_Menu::Blocking,tmpData);
                break;
            case 7:
                tmpData.append(line_items[0]);
                newIPC->setData(IPC_Menu::Echo,tmpData);
                break;
            case 8:
                newIPC->setData(IPC_Menu::nTX,line_items[0]);
                tmpData.clear();
                for (int k=0; k<line_items[0].toInt(); k++) {
                    current_index++;
                    tmpData.append(ipc_string[current_index]);
                }
                newIPC->setData(IPC_Menu::Prefixes,tmpData);
                break;
            default:
                break;
            }
            current_index++;
            tmpData.clear();
        }
        ui->ipclist->addItem(newIPC);
    }
}

void IPC_Menu::on_ipc_remove_clicked() {
    int removeitem = ui->ipclist->currentRow();

    if (removeitem == -1) return;
    else {
        delete ui->ipclist->takeItem(removeitem);
        ui->ipclist->setCurrentRow(-1);
        clear_fields();
    }

    if (ui->ipclist->count() != 0) {
        QList<QListWidgetItem*> itemList = ui->ipclist->findItems("*",Qt::MatchWildcard);
        for (int i=0; i<ui->ipclist->count(); i++) {
            itemList[i]->setData(IPC_Menu::Name,"IPC "+QString::number(i));
        }
    }
}

void IPC_Menu::on_ipc_add_clicked() {
    QListWidgetItem *newIPC = new QListWidgetItem;
    QStringList line_items;

    QStringList newData;
    QStringList newStrings;
    QStringList tmpData;
    QString line_string;

    QString newName = "IPC ";
    QStringList curNames = dsm_gui_lib::getTextFromList(ui->ipclist);
    int ipcNum = 0;
    for(int i = 0; i <= 50; i++) {
        ipcNum = i;
        QString newNameTest = newName+QString::number(ipcNum);
        if (!curNames.contains(newNameTest)) {
            newName = newNameTest;
            break;
        }
        if (i==50) return; // Nothing happens if too many
    }

    newData.append("");
    newData.append("OFF                            ");
    newData.append("0                              ");
    newData.append("");
    newData.append("CLIENT                         ");
    newData.append("localhost  10001               ");
    newData.append("TRUE                           ");
    newData.append("FALSE                          ");
    newData.append("1                              ");
    newData.append("");

    newStrings.append("");
    newStrings.append("");
    newStrings.append("");
    newStrings.append("State0" + QString::number(ipcNum) + ".42");
    newStrings.append("");
    newStrings.append("");
    newStrings.append("");
    newStrings.append("");
    newStrings.append("");
    newStrings.append("SC[0].AC");

    for (int j=0; j<ipcNLines; j++) {
        line_items = newData[j].split(QRegExp("\\s"), Qt::SkipEmptyParts);
        switch (j) {
        case 0:
            newIPC->setData(IPC_Menu::Name,newName);
            break;
        case 1:
            tmpData.append(line_items[0]);
            newIPC->setData(IPC_Menu::Mode,tmpData);
            break;
        case 2:
            tmpData.append(line_items[0]);
            newIPC->setData(IPC_Menu::ACID,tmpData);
            break;
        case 3:
            tmpData.append(line_string);
            newIPC->setData(IPC_Menu::FileName,newStrings[j]);
            break;
        case 4:
            tmpData.append(line_items[0]);
            newIPC->setData(IPC_Menu::Role,tmpData);
            break;
        case 5:
            tmpData.append(line_items[0]);
            tmpData.append(line_items[1]);
            newIPC->setData(IPC_Menu::Server,tmpData);
            break;
        case 6:
            tmpData.append(line_items[0]);
            newIPC->setData(IPC_Menu::Blocking,tmpData);
            break;
        case 7:
            tmpData.append(line_items[0]);
            newIPC->setData(IPC_Menu::Echo,tmpData);
            break;
        case 8:
            newIPC->setData(IPC_Menu::nTX,line_items[0]);
            tmpData.clear();
            tmpData.append(newStrings[j+1]);
            newIPC->setData(IPC_Menu::Prefixes,tmpData);
            break;
        default:
            break;
        }
        tmpData.clear();
    }
    ui->ipclist->addItem(newIPC);

    ui->ipclist->setCurrentRow(-1);
    clear_fields();
}

void IPC_Menu::on_ipc_duplicate_clicked(){
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();
    QStringList curNames = dsm_gui_lib::getTextFromList(ui->ipclist);

    if (index == -1) return;
    QString newName = "IPC ";
    int ipcNum;
    if (ui->ipclist->count() != 0) {
        for(int i = 0; i <= 50; i++) {
            ipcNum = i;
            QString newNameTest = newName+QString::number(ipcNum);
            if (!curNames.contains(newNameTest)) {
                newName = newNameTest;
                break;
            }
            if (i==50) return; // Nothing happens if too many
        }
    }

    QListWidgetItem* newItem = curItem->clone();
    newItem->setText(newName);
    ui->ipclist->addItem(newItem);

}

void IPC_Menu::on_ipclist_itemClicked(QListWidgetItem *item) {
    QStringList tmpData = {};

    for (int i=0; i<ipcNLines; i++) {
        switch (i) {
        case 0:
            tmpData = item->data(IPC_Menu::Mode).toStringList();
            ui->ipcmode->setCurrentText(ipcmodeinputs[tmpData[0]]);
            break;
        case 1:
            tmpData = item->data(IPC_Menu::ACID).toStringList();
            ui->acs_id->setText(tmpData[0]);
            break;
        case 2:
            tmpData = item->data(IPC_Menu::FileName).toStringList();
            ui->filename->setText(tmpData[0]);
            break;
        case 3:
            tmpData = item->data(IPC_Menu::Role).toStringList();
            ui->socketrole->setCurrentText(socketrole_inputs[tmpData[0]]);
            break;
        case 4:
            tmpData = item->data(IPC_Menu::Server).toStringList();
            ui->servername->setText(tmpData[0]);
            ui->portnum->setText(tmpData[1]);
            break;
        case 5:
            tmpData = item->data(IPC_Menu::Blocking).toStringList();
            ui->blocking->setChecked(QVariant(tmpData[0]).toBool());
            break;
        case 6:
            tmpData = item->data(IPC_Menu::Echo).toStringList();
            ui->echo->setChecked(QVariant(tmpData[0]).toBool());
            break;
        case 7:
            tmpData = item->data(IPC_Menu::nTX).toStringList();
            ui->prefixnum->display(tmpData[0]);
            tmpData = item->data(IPC_Menu::Prefixes).toStringList();
            ui->prefixlist->clear();
            ui->prefixlist->addItems(tmpData);
            break;
        default:
            break;
        }
    }
}

void IPC_Menu::on_loaddefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Overwrite IPC file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_IPC.txt");
        QFile::copy(inout_path+"__default__/Inp_IPC.txt", inout_path+"Inp_IPC.txt");
        receive_data();
        apply_data();
        ui->ipclist->setCurrentRow(-1);
    }
    else {
        return;
    }
}

void IPC_Menu::on_savedefaultButton_clicked() {
    int response = dsm_gui_lib::warning_message("Overwrite Default IPC file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_IPC.txt");
        QFile::copy(inout_path+"Inp_IPC.txt",inout_path+"__default__/Inp_IPC.txt");
        receive_data();
        apply_data();
        ui->ipclist->setCurrentRow(-1);
    }
    else {
        return;
    }
}

void IPC_Menu::on_closeButton_clicked() {
    IPC_Menu::close();
}

void IPC_Menu::on_applyButton_clicked() {
    QString data_inp;
    QString prefix_name;
    QStringList tmpData = {};
    QListWidgetItem* item;

    int ipc_num = ui->ipclist->count();
    int nPrefixes;

    ipc_update.append("<<<<<<<<<<<<<<< 42: InterProcess Comm Configuration File >>>>>>>>>>>>>>>>\n");

    data_inp = QString::number(ipc_num);
    ipc_update.append(dsm_gui_lib::whitespace(data_inp) + " ! Number of Sockets\n");

    for (int i=0; i<ipc_num; i++) {
        item = ui->ipclist->item(i);

        for (int j=0; j<ipcNLines; j++) {
            switch (j) {
            case 0:
                data_inp = "********************************  IPC "+QString::number(i)+"  ********************************\n";
                break;
            case 1:
                tmpData = item->data(IPC_Menu::Mode).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n";
                break;
            case 2:
                tmpData = item->data(IPC_Menu::ACID).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! AC.ID for ACS mode\n";
                break;
            case 3:
                tmpData = item->data(IPC_Menu::FileName).toStringList();
                data_inp = "\"" + tmpData[0] + "\"";
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! File name for WRITE or READ\n";
                break;
            case 4:
                tmpData = item->data(IPC_Menu::Role).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n";
                break;
            case 5:
                tmpData = item->data(IPC_Menu::Server).toStringList();
                data_inp = tmpData.join("  ");
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Server Host Name, Port\n";
                break;
            case 6:
                tmpData = item->data(IPC_Menu::Blocking).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Allow Blocking (i.e. wait on RX)\n";
                break;
            case 7:
                tmpData = item->data(IPC_Menu::Echo).toStringList();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Echo to stdout\n";
                break;
            case 8:
                tmpData = item->data(IPC_Menu::nTX).toStringList();
                nPrefixes = tmpData[0].toInt();
                data_inp = tmpData[0];
                data_inp = dsm_gui_lib::whitespace(data_inp) + " ! Number of TX prefixes\n";
                if (nPrefixes>0) {
                    tmpData = item->data(IPC_Menu::Prefixes).toStringList();
                    for (int k=0; k<nPrefixes; k++) {
                        prefix_name = "\"" + tmpData[k] + "\"";
                        data_inp += dsm_gui_lib::whitespace(prefix_name) + " ! Prefix "+QString::number(k)+"\n";
                    }
                }
                break;
            default:
                break;
            }
            ipc_update.append(data_inp);
            data_inp.clear();
        }
    }

    write_data();
}

void IPC_Menu::on_prefixlist_currentRowChanged(int currentRow) {
    bool isItem = !(currentRow == -1);
    ui->prefix_remove->setEnabled(isItem);
    ui->prefix_rename->setEnabled(isItem);
}

void IPC_Menu::on_prefix_remove_clicked() {
    int removeitem = ui->prefixlist->currentRow();
    if (removeitem == -1) {
        return;
    }
    else {
        ui->prefixlist->setCurrentRow(-1);
        int ipcindex = ui->ipclist->currentRow();
        QListWidgetItem* curItem = ui->ipclist->currentItem();

        if (ipcindex == -1) return;
        else if (ui->prefixlist->count() < 2) {
            int response = dsm_gui_lib::error_message("Must have at least one prefix declared!");
            if (response == QMessageBox::Ok)
                return;
        }
        else {
            delete ui->prefixlist->takeItem(removeitem);

            update_prefixes(curItem);
        }
    }
}

void IPC_Menu::on_prefix_add_clicked() {
    int ipcindex = ui->ipclist->currentRow();

    if (ipcindex == -1) return;
    else {
        ui->prefixlist->setCurrentRow(-1);
        QListWidgetItem* curItem = ui->ipclist->currentItem();

        QString newprefix = "SC[0].AC";

        ui->prefixlist->addItem(newprefix);
        update_prefixes(curItem);
    }
}

void IPC_Menu::on_prefix_rename_clicked() {
    int renameitem = ui->prefixlist->currentRow();
    if (renameitem == -1) return;
    else {
        int ipcindex = ui->ipclist->currentRow();
        if (ipcindex == -1) return;
        else {
            QListWidgetItem* curIPC = ui->ipclist->currentItem();
            bool ok;
            QString text = QInputDialog::getText(this, tr("Input Prefix Name"), tr("Prefix:"), QLineEdit::Normal, ui->prefixlist->currentItem()->text(), &ok);
            if (ok && !text.isEmpty()) {
                text.remove("\"");
                ui->prefixlist->currentItem()->setText(text);

                update_prefixes(curIPC);
            }
        }
    }
}

void IPC_Menu::clear_fields() {
    ui->ipcmode->setCurrentIndex(0);
    ui->acs_id->clear();
    ui->filename->clear();
    ui->socketrole->setCurrentIndex(0);
    ui->servername->clear();
    ui->portnum->clear();
    ui->blocking->setChecked(false);
    ui->echo->setChecked(false);
    ui->prefixlist->clear();
}

void IPC_Menu::on_ipcmode_currentTextChanged(const QString &arg1) {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();

    if (index == -1) return;
    curItem->setData(IPC_Menu::Mode,ipcmodeinputs.key(arg1));
}

void IPC_Menu::on_acs_id_textChanged(const QString &arg1) {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();

    if (index == -1) return;
    curItem->setData(IPC_Menu::ACID,arg1);
}

void IPC_Menu::on_filename_textChanged(const QString &arg1) {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();

    if (index == -1) return;
    curItem->setData(IPC_Menu::FileName,arg1);
}

void IPC_Menu::on_socketrole_currentTextChanged(const QString &arg1) {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();

    if (index == -1) return;
    curItem->setData(IPC_Menu::Role,socketrole_inputs.key(arg1));
}

void IPC_Menu::on_blocking_toggled(bool checked) {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();

    if (index == -1) return;
    curItem->setData(IPC_Menu::Blocking,QVariant(checked).toString().toUpper());
}

void IPC_Menu::on_echo_toggled(bool checked) {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();

    if (index == -1) return;
    curItem->setData(IPC_Menu::Echo,QVariant(checked).toString().toUpper());
}

void IPC_Menu::server_changed() {
    int index = ui->ipclist->currentRow();
    QListWidgetItem* curItem = ui->ipclist->currentItem();
    QStringList serverPort;

    if (index == -1) return;
    serverPort.append(ui->servername->text());
    serverPort.append(ui->portnum->text());
    curItem->setData(IPC_Menu::Server,serverPort);
}

void IPC_Menu::update_prefixes(QListWidgetItem* item) {
    QStringList prefixes = dsm_gui_lib::getTextFromList(ui->prefixlist);
    int prefix_num = ui->prefixlist->count();
    item->setData(IPC_Menu::Prefixes,prefixes);
    item->setData(IPC_Menu::nTX,QString::number(prefix_num));
    ui->prefixnum->display(prefix_num);
}
