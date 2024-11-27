#include "dsm_gui_lib.h"
#include <QComboBox>
#include <QDebug>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>

dsm_gui_lib::dsm_gui_lib() {}

int dsm_gui_lib::warning_message(QString warningText) {
   QMessageBox warningMsg;
   warningMsg.setIcon(QMessageBox::Warning);
   warningMsg.setText(warningText);
   warningMsg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
   int ret = warningMsg.exec();
   return ret;
}

int dsm_gui_lib::error_message(QString errorText) {
   QMessageBox errorMsg;
   errorMsg.setIcon(QMessageBox::Critical);
   errorMsg.setText(errorText);
   errorMsg.setStandardButtons(QMessageBox::Ok);
   int ret = errorMsg.exec();
   return ret;
}

QString dsm_gui_lib::whitespace(QString data) {
   QString empty_space = "                              ";
   int data_len        = empty_space.count() - data.count();
   if (data_len < 1)
      data_len = 1;
   for (int i = 0; i < data_len; i++) {
      data.append(" ");
   }
   return data;
}

QStringList dsm_gui_lib::sortStringList(QStringList unsorted) {
   QStringList sorted;
   unsorted.sort();
   sorted = unsorted;
   return sorted;
}

QStringList dsm_gui_lib::getTextFromList(QListWidget *list) {
   QStringList output                 = {};
   QList<QListWidgetItem *> listItems = list->findItems("*", Qt::MatchWildcard);
   for (QListWidgetItem *item : listItems)
      output.append(item->text());

   output.sort(Qt::CaseInsensitive);

   return output;
}

int dsm_gui_lib::get_sc_nitems(const QString inout_path, const QString sc_name,
                               const scSectionType type) {
   const QString scFileName = inout_path + "SC_" + sc_name + ".yaml";
   const YAML::Node sc_yaml = YAML::LoadFile(scFileName.toStdString());

   const QString searchStr = scSectionIdentifier(type);

   const YAML::Node query_node = sc_yaml[searchStr];
   const int nItems            = query_node.size();

   return std::max(nItems, 0);
}

void dsm_gui_lib::set_mult_cbox_validators(QList<QComboBox *> ui_elem,
                                           const QStringList string_list) {
   for (auto elem : ui_elem) {
      elem->addItems(string_list);
   }
}

QStringList dsm_gui_lib::apply_data_section_end(long cur_item,
                                                QListWidget *ui_elem,
                                                QStringList tmp_data,
                                                QString cur_item_name) {
   ui_elem->setCurrentRow(cur_item);
   ui_elem->currentItem()->setData(256, cur_item_name);
   ui_elem->currentItem()->setData(257, tmp_data);
   tmp_data.clear();
   return tmp_data;
}

std::tuple<long, int, QStringList>
dsm_gui_lib::item_entry_lineitems(QStringList spc_data, int line_num,
                                  long reset_ind, long entries, long headers) {
   QStringList line_items =
       spc_data[line_num - 1].split(QRegExp("\\s"), Qt::SkipEmptyParts);

   long section_line_num = line_num - reset_ind - headers;
   long cur_item         = floor(section_line_num / entries);
   long cur_entry        = section_line_num % entries;

   return {cur_item, cur_entry, line_items};
}

void dsm_gui_lib::mult_setText(QLineEdit *ui_elem[], int array_length,
                               QStringList current_data, int data_inds[]) {
   for (int i = 0; i < array_length; i++) {
      ui_elem[i]->setText(current_data[data_inds[i]]);
   }
}

bool dsm_gui_lib::fileExists(QString path) {
   QFileInfo check_file(path);
   // check if file exists and if yes: Is it really a file and not a directory?
   if (check_file.exists() && check_file.isFile()) {
      return true;
   } else {
      return false;
   }
}

QVector<QString> dsm_gui_lib::create_QVec2(QString arg1, QString arg2) {
   QVector<QString> tmp_data_vector = {arg1, arg2};
   return tmp_data_vector;
}

QVector<QString> dsm_gui_lib::create_QVec3(QString arg1, QString arg2,
                                           QString arg3) {
   QVector<QString> tmp_data_vector = {arg1, arg2, arg3};
   return tmp_data_vector;
}

QVector<QString> dsm_gui_lib::create_QVec4(QString arg1, QString arg2,
                                           QString arg3, QString arg4) {
   QVector<QString> tmp_data_vector = {arg1, arg2, arg3, arg4};
   return tmp_data_vector;
}

void dsm_gui_lib::write_data(const QString file_path, YAML::Node yaml) {
   QFile::remove(file_path);
   QFile file(file_path);
   if (!file.open(QFile::WriteOnly)) {
      QMessageBox::information(0, "error", file.errorString());
   } else {
      QTextStream in(&file);
      YAML::Emitter out;
      out.SetIndent(2);
      out.SetMapFormat(YAML::EMITTER_MANIP::Block);
      out << yaml;
      in << out.c_str();
   }

   file.close();
}

QString dsm_gui_lib::generate_comment(QString str_search, QString cur_line,
                                      YAML::Node comments) {
   QString string_to_append = "";
   if (!QString::compare(cur_line, str_search + ":")) {
      QString cur_comments      = comments[str_search].as<QString>();
      QStringList comments_list = cur_comments.split("\n");

      for (int j = 0; j < comments_list.size() - 1; j++) {
         string_to_append += "    # " + comments_list[j];
         if (j < comments_list.size() - 1)
            string_to_append += "\n";
      }
   }
   return string_to_append;
}
