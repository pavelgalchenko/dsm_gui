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
        ipc_update.append(line);
    }
    file.close();
}

void IPC_Menu::write_data()
{
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
}

void IPC_Menu::apply_data() {
    QStringList line_items;
    QString line_string;
    QString ipcName = "IPC ";
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
                tmpData.append(line_items[0]);
                newIPC->setData(IPC_Menu::nTX,tmpData);
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
    write_data();
}

void IPC_Menu::on_ipc_remove_clicked() {
    int removeitem = ui->ipclist->currentRow();

    if (removeitem == -1) {
        return;
    }
    else {
        ui->ipclist->takeItem(removeitem);
        ui->ipclist->setCurrentRow(-1);
        clear_fields();
    }
}

void IPC_Menu::on_ipc_add_clicked() {
    int ipc_count = ui->ipclist->count();

    QString data_inp = QString::number(ipc_count+1);
    ipc_update[1] = dsm_gui_lib::whitespace(data_inp) + " ! Number of Sockets\n";

    ipc_update.append("********************************  IPC "+QString::number(ipc_count)+"  ********************************\n");
    ipc_update.append("OFF                            ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n");
    ipc_update.append("0                              ! AC.ID for ACS mode\n");
    ipc_update.append("\"State0" + QString::number(ipc_count) + ".42\"                   ! File name for WRITE or READ\n");
    ipc_update.append("CLIENT                         ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n");
    ipc_update.append("localhost  10001               ! Server Host Name, Port\n");
    ipc_update.append("TRUE                           ! Allow Blocking (i.e. wait on RX)\n");
    ipc_update.append("FALSE                          ! Echo to stdout\n");
    ipc_update.append("1                              ! Number of TX prefixes\n");
    ipc_update.append("\"SC[0].AC\"                     ! Prefix 0\n");

    write_data();

    receive_data();
    apply_data();
}

void IPC_Menu::on_ipclist_itemClicked(QListWidgetItem *item) {
    QStringList tmpData = {};
    clear_fields();
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

    int index = ui->ipclist->currentRow();

    if (index == -1) {
        return;
    }

    int ipc_num = ui->ipclist->count();
    int data_index = ipc_name_index.at(index);

    ipc_update[0] = "<<<<<<<<<<<<<<< 42: InterProcess Comm Configuration File >>>>>>>>>>>>>>>>\n";

    data_inp = QString::number(ipc_num);
    ipc_update[1] = dsm_gui_lib::whitespace(data_inp) + " ! Number of Sockets\n";

    ipc_update[data_index+0] = "********************************  IPC "+QString::number(index)+"  ********************************\n";

    data_inp = ui->ipcmode->currentText();
    ipc_update[data_index+1] = dsm_gui_lib::whitespace(data_inp) + " ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n";

    data_inp = ui->acs_id->text();
    ipc_update[data_index+2] = dsm_gui_lib::whitespace(data_inp) + " ! AC.ID for ACS mode\n";

    data_inp = ui->filename->text();
    ipc_update[data_index+3] = dsm_gui_lib::whitespace(data_inp) + " ! File name for WRITE or READ\n";

    data_inp = ui->socketrole->currentText();
    ipc_update[data_index+4] = dsm_gui_lib::whitespace(data_inp) + " ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n";

    data_inp = ui->servername->text() + "  " + ui->portnum->text();
    ipc_update[data_index+5] = dsm_gui_lib::whitespace(data_inp) + " ! Server Host Name, Port\n";

    if (ui->blocking->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    ipc_update[data_index+6] = dsm_gui_lib::whitespace(data_inp) + " ! Allow Blocking (i.e. wait on RX)\n";

    if (ui->echo->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    ipc_update[data_index+7] = dsm_gui_lib::whitespace(data_inp) + " ! Echo to stdout\n";

    data_inp = QString::number(ui->prefixnum->value());
    ipc_update[data_index+8] = dsm_gui_lib::whitespace(data_inp) + " ! Number of TX prefixes\n";

    for (int i = 0; i < ipc_name_prefixes[index]; i++) {
        data_inp = ui->prefixlist->item(i)->text();
        ipc_update[data_index+9+i] = dsm_gui_lib::whitespace(data_inp) + " ! Prefix "+QString::number(i)+" \n";
    }

    write_data();
}

void IPC_Menu::on_prefixlist_itemClicked(QListWidgetItem *item) {
    ui->prefix_remove->setEnabled(true);
    ui->prefix_rename->setEnabled(true);
}

void IPC_Menu::on_prefix_remove_clicked()
{
    int removeitem = ui->prefixlist->currentRow();
    if (removeitem == -1) {
        return;
    }
    else {
        ui->prefixlist->setCurrentRow(-1);
        ui->prefix_remove->setEnabled(false);
        ui->prefix_rename->setEnabled(false);
        int ipcindex = ui->ipclist->currentRow();
        int ipccount = ui->ipclist->count();
        if (ipcindex == -1) {
            return;
        }
        else if (ui->prefixlist->count() < 2) {
            int response = dsm_gui_lib::warning_message("Must have at least one prefix declared!");
            if (response == QMessageBox::Cancel) {
                return;
            }
            else if (response == QMessageBox::Ok) {
                return;
            }
        }
        else {

            int prefix_index = ipc_name_index.at(ipcindex)+8;
            int remove_index  = prefix_index+removeitem+1;

            ipc_name_size[ipcindex] += -1;
            ipc_name_prefixes[ipcindex] += -1;

            QString data_inp;
            int prefix_num = ipc_name_prefixes[ipcindex];

            data_inp = QString::number(prefix_num);
            ipc_update.replace(prefix_index,dsm_gui_lib::whitespace(data_inp)+" ! Number of TX Prefixes\n");
            ipc_update.removeAt(remove_index);

            for (int i = ipcindex+1; i < ipccount; i++){
                ipc_name_index[i] += -1;
            }

            delete ui->prefixlist->takeItem(removeitem);
            ui->prefixnum->display(prefix_num);

            write_data();

            receive_data();
            apply_data();
        }
    }
}

void IPC_Menu::on_prefix_add_clicked()
{
    int ipcindex = ui->ipclist->currentRow();
    int ipccount = ui->ipclist->count();
    if (ipcindex == -1) {
        return;
    }
    else {
        ui->prefixlist->setCurrentRow(-1);
        ui->prefix_remove->setEnabled(false);
        ui->prefix_rename->setEnabled(false);

        QString newprefix = "\"SC[0].AC\"";

        ui->prefixlist->addItem(newprefix);

        ipc_name_size[ipcindex] += 1;
        ipc_name_prefixes[ipcindex] += 1;

        int prefix_num = ipc_name_prefixes[ipcindex];
        int prefix_index = ipc_name_index.at(ipcindex)+8;
        int addprefix_index  = prefix_index+prefix_num;

        QString data_inp;

        data_inp = QString::number(prefix_num);
        ipc_update.replace(prefix_index,dsm_gui_lib::whitespace(data_inp)+" ! Number of TX Prefixes\n");
        ipc_update.insert(addprefix_index,dsm_gui_lib::whitespace(newprefix) + " ! Prefix " + QString::number(prefix_num-1) + "\n");

        for (int i = ipcindex+1; i < ipccount; i++){
            ipc_name_index[i] += 1;
        }

        ui->prefixnum->display(prefix_num);

        write_data();
        receive_data();
        apply_data();
    }
}

void IPC_Menu::on_prefix_rename_clicked()
{
    int renameitem = ui->prefixlist->currentRow();
    if (renameitem == -1) {
        return;
    }
    else {
        //ui->prefixlist->setCurrentRow(-1);
        int ipcindex = ui->ipclist->currentRow();
        if (ipcindex == -1) {
            return;
        }
        else {
            bool ok;
            QString text = QInputDialog::getText(this, tr("Input Prefix Name"), tr("Prefix:"), QLineEdit::Normal, ui->prefixlist->currentItem()->text(), &ok);
            if (ok && !text.isEmpty()) {

                int prefix_index = ipc_name_index.at(ipcindex)+8;
                int rename_index  = prefix_index+renameitem+1;

                ipc_update.replace(rename_index,dsm_gui_lib::whitespace(text)+" ! Prefix " + QString::number(renameitem) + "\n");
                ui->prefixlist->item(renameitem)->setText(text);

                write_data();

                receive_data();
                apply_data();

                //ui->prefixlist->repaint();
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
