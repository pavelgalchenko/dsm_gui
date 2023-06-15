#ifndef SPC_Menu_H
#define SPC_Menu_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>

namespace Ui {
class SPC_Menu;
}

class SPC_Menu : public QDialog
{
    Q_OBJECT

public:
    explicit SPC_Menu(QWidget *parent = nullptr);
    ~SPC_Menu();

private slots:

    void on_spc_add_clicked();
    void on_spc_remove_clicked();
    void on_spc_duplicate_clicked();

    void on_spc_load_clicked();
    void on_spc_save_clicked();
    void on_spc_close_clicked();
    void on_spc_apply_clicked();

    //void on_spc_conf_clicked();

    QString whitespace(QString);

    void on_spc_list_itemClicked(QListWidgetItem *item);

    void receive_data();
    void apply_data();
    void write_data();

    int warning_message(QString);

    void receive_spcpath(QString);

private:
    Ui::SPC_Menu *ui;

    int global_spc_index = -1;
    int global_spc_ignore = 0;

    QString inout_path;
    QString file_path;
    QStringList file_paths;



    QStringList spc_names;
    QStringList spc_string;
    QStringList spc_data;
    QStringList spc_update;
    int spc_name_index;

    QStringList spc_file_headers; // section headers in the file
    QStringList spc_file_descrip; // data descriptors in the file


};

#endif // SPC_Menu_H
