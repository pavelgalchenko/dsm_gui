#include "ipc_menu.h"
#include "ui_ipc_menu.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>

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

void IPC_Menu::set_validators()
{
    ui->ipcmode->addItems(ipcmodeinputs);
    ui->socketrole->addItems(socketrole_inputs);
    ui->acs_id->setValidator(new QIntValidator);
    ui->portnum->setValidator(new QIntValidator);
}

void IPC_Menu::receive_ipcpath(QString path)
{
    inout_path = path;
    file_path = path+"Inp_IPC.txt";
    receive_data();
    apply_data();
    populate_list();
}

void IPC_Menu::receive_data()
{
    ipc_data.clear();
    ipc_string.clear();
    ipc_update.clear();
    static QRegularExpression rx1("(.*?)*!");
    static QRegularExpression rx2("\"(.*?)\"");

    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match1 = rx1.match(line);
        if (match1.hasMatch()) {
            QString capture = match1.captured(0);
            ipc_data.append(capture);
        }
        else {
            ipc_data.append("blankline");
        }
        QRegularExpressionMatch match2 = rx2.match(line);
        if (match2.hasMatch()) {
            QString capture = match2.captured(0);
            ipc_string.append(capture);
        }
        else {
            ipc_string.append("blankline");
        }
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


void IPC_Menu::apply_data()
{
    QStringList line_items;
    QString line_item;
    int num_ipc = 0;
    int num_prefixes = 0;
    int current_index = 2;

    ipc_names.clear();
    ipc_name_index.clear();
    ipc_name_prefixes.clear();
    ipc_name_size.clear();

    line_items = ipc_data[1].split(QRegExp("\\s"), QString::SkipEmptyParts);
    line_item = line_items[0];
    num_ipc = line_item.toInt();

    for (int i=0; i < num_ipc; i++) {

        line_items = ipc_data[current_index+8].split(QRegExp("\\s"), QString::SkipEmptyParts);
        line_item = line_items[0];
        num_prefixes = line_item.toInt();

        ipc_names.append("IPC "+QString::number(i));
        ipc_update.replace(current_index,"********************************  "+ipc_names.at(i)+"  ********************************\n");
        ipc_name_index.append(current_index);
        ipc_name_size.append(9+num_prefixes);
        ipc_name_prefixes.append(num_prefixes);
        current_index = current_index+9+num_prefixes;
    }
    write_data();
}

void IPC_Menu::populate_list()
{
    ui->ipclist->clear();
    ui->ipclist->addItems(ipc_names);
}

void IPC_Menu::on_ipc_remove_clicked()
{
    int removeitem = ui->ipclist->currentRow();
    if (removeitem == -1) {
        return;
    }
    else {

        int remove_amount = ipc_name_size.at(removeitem);
        int remove_index  = ipc_name_index.at(removeitem);

        for (int i = 0; i < remove_amount; i++) {
            ipc_update.removeAt(remove_index);
        }

        delete ui->ipclist->takeItem(removeitem);

        QString data_inp;
        int ipc_num = ui->ipclist->count();

        data_inp = QString::number(ipc_num);
        ipc_update[1] = whitespace(data_inp) + " ! Number of Sockets\n";

        write_data();

        receive_data();
        apply_data();
        populate_list();

        global_ipc_index = -1;
    }
}

void IPC_Menu::on_ipc_add_clicked()
{
    int ipc_count = ui->ipclist->count();

    QString data_inp = QString::number(ipc_count+1);
    ipc_update[1] = whitespace(data_inp) + " ! Number of Sockets\n";

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
    populate_list();

    global_ipc_index = -1;
}

void IPC_Menu::on_ipclist_itemClicked(QListWidgetItem *item)
{
    int index = ui->ipclist->row(item);

    if (index == global_ipc_index) {
        return;
    }
    else {
        if ( (global_ipc_index != -1) && (global_ipc_ignore == 0) ) {
            int response = warning_message("Note that changes to the previous selected IPC are lost unless you first select \"Apply\"! This is your only warning.");
            if (response == QMessageBox::Cancel) {
                ui->ipclist->setCurrentRow(global_ipc_index);
                global_ipc_ignore = 1;
                return;
            }
            else if (response == QMessageBox::Ok) {
                global_ipc_ignore = 1;
            }
        }

        global_ipc_index = index;
        ui->prefix_remove->setEnabled(false);

        int data_index = ipc_name_index.at(index);
        int line_num;
        int prefix_num;

        QStringList line_items;
        QString line_item;

        line_num = data_index+1;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        for( int i=0; i<ipcmodeinputs.count(); ++i )
        {
            if (line_items[0] == ipcmodeinputs[i]) ui->ipcmode->setCurrentIndex(i);
        }

        line_num = data_index+2;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->acs_id->setText(line_items[0]);

        line_item = ipc_string[data_index+3];
        ui->filename->setText(line_item);

        line_num = data_index+4;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        for( int i=0; i<socketrole_inputs.count(); ++i )
        {
            if (line_items[0] == socketrole_inputs[i]) ui->socketrole->setCurrentIndex(i);
        }

        line_num = data_index+5;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->servername->setText(line_items[0]);
        ui->portnum->setText(line_items[1]);

        line_num = data_index+6;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        if (line_items[0] == "TRUE") ui->blocking_on->setChecked(true);
        else ui->blocking_off->setChecked(true);

        line_num = data_index+7;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        if (line_items[0] == "TRUE") ui->echo_on->setChecked(true);
        else ui->echo_off->setChecked(true);

        line_num = data_index+8;
        line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
        ui->prefixnum->display(line_items[0]);

        QStringList prefixes;
        ui->prefixlist->clear();
        prefix_num = ui->prefixnum->intValue();
        for (int i = 0; i < prefix_num; i++){
            line_num = data_index+9+i;
            line_items = ipc_data[line_num].split(QRegExp("\\s"), QString::SkipEmptyParts);
            prefixes.append(line_items[0]);
        }
        ui->prefixlist->addItems(prefixes);

    }
}

int IPC_Menu::warning_message(QString warningText)
{
    QMessageBox warningMsg;
    warningMsg.setIcon(QMessageBox::Warning);
    warningMsg.setText(warningText);
    warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warningMsg.exec();
    return ret;
}

void IPC_Menu::on_loaddefaultButton_clicked()
{
    int response = warning_message("Overwrite IPC file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"Inp_IPC.txt");
        QFile::copy(inout_path+"__default__/Inp_IPC.txt", inout_path+"Inp_IPC.txt");
        receive_data();
        apply_data();
        ui->ipclist->setCurrentRow(-1);
        global_ipc_index = -1;
        populate_list();
    }
    else {
        return;
    }
}

void IPC_Menu::on_savedefaultButton_clicked()
{
    int response = warning_message("Overwrite Default IPC file?");
    if (response == QMessageBox::Ok) {
        QFile::remove(inout_path+"__default__/Inp_IPC.txt");
        QFile::copy(inout_path+"Inp_IPC.txt",inout_path+"__default__/Inp_IPC.txt");
        receive_data();
        apply_data();
        ui->ipclist->setCurrentRow(-1);
        global_ipc_index = -1;
        populate_list();
    }
    else {
        return;
    }
}

void IPC_Menu::on_closeButton_clicked()
{
    IPC_Menu::close();
}

void IPC_Menu::on_applyButton_clicked()
{
    QString data_inp;

    int index = ui->ipclist->currentRow();

    if (index == -1) {
        return;
    }

    int ipc_num = ui->ipclist->count();
    int data_index = ipc_name_index.at(index);

    ipc_update[0] = "<<<<<<<<<<<<<<< 42: InterProcess Comm Configuration File >>>>>>>>>>>>>>>>\n";

    data_inp = QString::number(ipc_num);
    ipc_update[1] = whitespace(data_inp) + " ! Number of Sockets\n";

    ipc_update[data_index+0] = "********************************  IPC "+QString::number(index)+"  ********************************\n";

    data_inp = ui->ipcmode->currentText();
    ipc_update[data_index+1] = whitespace(data_inp) + " ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n";

    data_inp = ui->acs_id->text();
    ipc_update[data_index+2] = whitespace(data_inp) + " ! AC.ID for ACS mode\n";

    data_inp = ui->filename->text();
    ipc_update[data_index+3] = whitespace(data_inp) + " ! File name for WRITE or READ\n";

    data_inp = ui->socketrole->currentText();
    ipc_update[data_index+4] = whitespace(data_inp) + " ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n";

    data_inp = ui->servername->text() + "  " + ui->portnum->text();
    ipc_update[data_index+5] = whitespace(data_inp) + " ! Server Host Name, Port\n";

    if (ui->blocking_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    ipc_update[data_index+6] = whitespace(data_inp) + " ! Allow Blocking (i.e. wait on RX)\n";

    if (ui->echo_on->isChecked()) data_inp = "TRUE";
    else data_inp = "FALSE";
    ipc_update[data_index+7] = whitespace(data_inp) + " ! Echo to stdout\n";

    data_inp = QString::number(ui->prefixnum->value());
    ipc_update[data_index+8] = whitespace(data_inp) + " ! Number of TX prefixes\n";

    for (int i = 0; i < ipc_name_prefixes[index]; i++) {
        data_inp = ui->prefixlist->item(i)->text();
        ipc_update[data_index+9+i] = whitespace(data_inp) + " ! Prefix "+QString::number(i)+" \n";
    }

    write_data();
}

QString IPC_Menu::whitespace(QString data)
{
    QString empty_space = "                              ";
    int data_len = empty_space.count()-data.count();
    if (data_len < 1) data_len = 1;
    for (int i = 0; i < data_len; i++){
        data.append(" ");
    }
    return data;
}

void IPC_Menu::on_prefixlist_itemClicked(QListWidgetItem *item)
{
    ui->prefix_remove->setEnabled(true);
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
        int ipcindex = ui->ipclist->currentRow();
        int ipccount = ui->ipclist->count();
        if (ipcindex == -1) {
            return;
        }
        else if (ui->prefixlist->count() < 2) {
            int response = warning_message("Must have at least one prefix declared!");
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
            ipc_update.replace(prefix_index,whitespace(data_inp)+" ! Number of TX Prefixes\n");
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

        QString newprefix = "\"SC[0].AC\"";

        ui->prefixlist->addItem(newprefix);

        ipc_name_size[ipcindex] += 1;
        ipc_name_prefixes[ipcindex] += 1;

        int prefix_num = ipc_name_prefixes[ipcindex];
        int prefix_index = ipc_name_index.at(ipcindex)+8;
        int addprefix_index  = prefix_index+prefix_num;

        QString data_inp;

        data_inp = QString::number(prefix_num);
        ipc_update.replace(prefix_index,whitespace(data_inp)+" ! Number of TX Prefixes\n");
        ipc_update.insert(addprefix_index,whitespace(newprefix) + " ! Prefix " + QString::number(prefix_num-1) + "\n");

        for (int i = ipcindex+1; i < ipccount; i++){
            ipc_name_index[i] += 1;
        }

        ui->prefixnum->display(prefix_num);

        write_data();
        receive_data();
        apply_data();
    }
}
