#include "ipc_menu.h"
#include "ui_ipc_menu.h"

IPC_Menu::IPC_Menu(QWidget *parent) : QDialog(parent), ui(new Ui::IPC_Menu) {
   ui->setupUi(this);
   set_validators();
}

IPC_Menu::~IPC_Menu() {
   delete ui;
}

void IPC_Menu::set_validators() {
   QRegularExpression rx("[^\"]*");

   ui->ipclist->setSortingEnabled(true);
   ui->ipclist->sortItems(Qt::AscendingOrder);

   ui->prefixlist->setSortingEnabled(true);
   ui->prefixlist->sortItems(Qt::AscendingOrder);

   ui->ipcmode->addItems(dsm_gui_lib::sortStringList(ipcmodeinputs.values()));
   ui->socketrole->addItems(
       dsm_gui_lib::sortStringList(socketrole_inputs.values()));
   ui->filename->setValidator(new QRegularExpressionValidator(rx));
   ui->acs_id->setValidator(new QIntValidator(0, INFINITY));
   ui->portnum->setValidator(new QIntValidator(0, INFINITY));

   connect(ui->servername, &QLineEdit::textChanged, this,
           &IPC_Menu::server_changed);
   connect(ui->portnum, &QLineEdit::textChanged, this,
           &IPC_Menu::server_changed);
}

void IPC_Menu::receive_ipcpath(QString path) {
   inout_path = path;
   file_path  = path + "Inp_IPC.yaml";
   apply_data();
}

void IPC_Menu::apply_data() {
   ipc_list_hash.clear();
   ui->ipclist->clear();

   YAML::Node ipc_file_yaml = YAML::LoadFile(file_path.toStdString());
   YAML::Node ipcs          = ipc_file_yaml["IPCs"];

   const QString ipcName = "IPC ";
   for (YAML::iterator it = ipcs.begin(); it != ipcs.end(); ++it) {
      IPC new_ipc    = (*it).as<IPC>();
      const size_t i = std::distance(ipcs.begin(), it);
      QListWidgetItem *newIPC =
          new QListWidgetItem(ipcName + QString::number(i), ui->ipclist);
      ipc_list_hash.insert(newIPC, new_ipc);
   }
}

void IPC_Menu::on_ipc_remove_clicked() {
   const int removeitem = ui->ipclist->currentRow();
   if (removeitem == -1)
      return;
   else {
      QListWidgetItem *cur_item = ui->ipclist->currentItem();
      ipc_list_hash.remove(cur_item);
      ui->ipclist->takeItem(removeitem);
      ui->ipclist->setCurrentRow(-1);
      clear_fields();
   }

   if (ui->ipclist->count() != 0) {
      QList<QListWidgetItem *> itemList =
          ui->ipclist->findItems("*", Qt::MatchWildcard);
      const int count = ui->ipclist->count();
      for (int i = 0; i < count; i++) {
         itemList[i]->setText("IPC " + QString::number(i));
      }
   }
}

void IPC_Menu::on_ipc_add_clicked() {

   QString newName      = "IPC ";
   QStringList curNames = dsm_gui_lib::getTextFromList(ui->ipclist);
   int ipcNum           = 0;
   for (int i = 0; i <= 50; i++) {
      ipcNum              = i;
      QString newNameTest = newName + QString::number(ipcNum);
      if (!curNames.contains(newNameTest)) {
         newName = newNameTest;
         break;
      }
      if (i == 50)
         return; // Nothing happens if too many
   }

   QListWidgetItem *newIPC = new QListWidgetItem(newName, ui->ipclist);
   IPC new_ipc             = IPC();
   ipc_list_hash.insert(newIPC, new_ipc);

   ui->ipclist->setCurrentRow(-1);
   clear_fields();
}

void IPC_Menu::on_ipc_duplicate_clicked() {
   int index                = ui->ipclist->currentRow();
   QListWidgetItem *curItem = ui->ipclist->currentItem();
   QStringList curNames     = dsm_gui_lib::getTextFromList(ui->ipclist);

   if (index == -1)
      return;
   QString newName = "IPC ";
   int ipcNum;
   if (ui->ipclist->count() != 0) {
      for (int i = 0; i <= 50; i++) {
         ipcNum              = i;
         QString newNameTest = newName + QString::number(ipcNum);
         if (!curNames.contains(newNameTest)) {
            newName = newNameTest;
            break;
         }
         if (i == 50)
            return; // Nothing happens if too many
      }
   }

   QListWidgetItem *newItem = curItem->clone();
   newItem->setText(newName);
   ui->ipclist->addItem(newItem);

   IPC new_ipc = ipc_list_hash[curItem];
   ipc_list_hash.insert(newItem, new_ipc);
}

void IPC_Menu::on_ipclist_itemClicked(QListWidgetItem *item) {
   const IPC ipc       = ipc_list_hash[item];
   const Socket socket = ipc.socket();
   const Host host     = socket.host();

   dsm_gui_lib::setQComboBox(ui->ipcmode, ipcmodeinputs[ipc.mode()]);
   ui->acs_id->setText(QString::number(ipc.ac_id()));
   ui->filename->setText(ipc.file_name());
   dsm_gui_lib::setQComboBox(ui->socketrole, socketrole_inputs[socket.role()]);
   ui->servername->setText(host.name());
   ui->portnum->setText(QString::number(host.port()));
   ui->blocking->setChecked(socket.blocking());
   ui->echo->setChecked(ipc.echo());
   ui->prefixlist->clear();
   ui->prefixlist->addItems(ipc.prefixes());
}

void IPC_Menu::on_loaddefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite IPC file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "Inp_IPC.yaml");
      QFile::copy(inout_path + "__default__/Inp_IPC.yaml",
                  inout_path + "Inp_IPC.yaml");
      apply_data();
      ui->ipclist->setCurrentRow(-1);
   } else {
      return;
   }
}

void IPC_Menu::on_savedefaultButton_clicked() {
   int response = dsm_gui_lib::warning_message("Overwrite Default IPC file?");
   if (response == QMessageBox::Ok) {
      QFile::remove(inout_path + "__default__/Inp_IPC.yaml");
      QFile::copy(inout_path + "Inp_IPC.yaml",
                  inout_path + "__default__/Inp_IPC.yaml");
      apply_data();
      ui->ipclist->setCurrentRow(-1);
   } else {
      return;
   }
}

void IPC_Menu::on_closeButton_clicked() {
   IPC_Menu::close();
}

void IPC_Menu::on_applyButton_clicked() {
   QList<IPC> ipc_list =
       dsm_gui_lib::getOrderedListFromHash(ui->ipclist, ipc_list_hash);
   YAML::Node ipc_yaml(YAML::NodeType::Map);
   ipc_yaml["IPCs"] = ipc_list;
   dsm_gui_lib::write_data(file_path, ipc_yaml);
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
   } else {
      ui->prefixlist->setCurrentRow(-1);
      int ipcindex             = ui->ipclist->currentRow();
      QListWidgetItem *curItem = ui->ipclist->currentItem();

      if (ipcindex == -1)
         return;
      else if (ui->prefixlist->count() < 2) {
         int response = dsm_gui_lib::error_message(
             "Must have at least one prefix declared!");
         if (response == QMessageBox::Ok)
            return;
      } else {
         delete ui->prefixlist->takeItem(removeitem);

         update_prefixes(curItem);
      }
   }
}

void IPC_Menu::on_prefix_add_clicked() {
   int ipcindex = ui->ipclist->currentRow();

   if (ipcindex == -1)
      return;
   else {
      ui->prefixlist->setCurrentRow(-1);
      QListWidgetItem *curItem = ui->ipclist->currentItem();

      QString newprefix = "SC[0].AC";

      ui->prefixlist->addItem(newprefix);
      update_prefixes(curItem);
   }
}

void IPC_Menu::on_prefix_rename_clicked() {
   int renameitem = ui->prefixlist->currentRow();
   if (renameitem == -1)
      return;
   else {
      int ipcindex = ui->ipclist->currentRow();
      if (ipcindex == -1)
         return;
      else {
         QListWidgetItem *curIPC = ui->ipclist->currentItem();
         bool ok;
         QString text = QInputDialog::getText(
             this, tr("Input Prefix Name"), tr("Prefix:"), QLineEdit::Normal,
             ui->prefixlist->currentItem()->text(), &ok);
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
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];
   ipc->setMode(ipcmodeinputs.key(arg1));
}

void IPC_Menu::on_acs_id_textChanged(const QString &arg1) {
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];

   ipc->setACID(arg1.toInt());
}

void IPC_Menu::on_filename_textChanged(const QString &arg1) {
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];
   ipc->setFileName(arg1);
}

void IPC_Menu::on_socketrole_currentTextChanged(const QString &arg1) {
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];
   ipc->setSocketRole(socketrole_inputs.key(arg1));
}

void IPC_Menu::on_blocking_toggled(bool checked) {
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];
   ipc->setSocketBlocking(checked);
}

void IPC_Menu::on_echo_toggled(bool checked) {
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];
   ipc->setEcho(checked);
}

void IPC_Menu::server_changed() {
   QListWidgetItem *curItem = ui->ipclist->currentItem();

   if (curItem == NULL)
      return;
   IPC *ipc = &ipc_list_hash[curItem];
   Host new_host(ui->servername->text(), ui->portnum->text().toInt());
   ipc->setSocketHost(new_host);
}

void IPC_Menu::update_prefixes(QListWidgetItem *item) {
   if (item == NULL)
      return;
   IPC *ipc = &ipc_list_hash[item];

   QStringList prefixes = dsm_gui_lib::getTextFromList(ui->prefixlist);
   int prefix_num       = prefixes.count();
   ipc->setPrefixes(prefixes);
   ui->prefixnum->display(prefix_num);
}
