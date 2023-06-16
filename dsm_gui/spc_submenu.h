#ifndef SPC_SUBMENU_H
#define SPC_SUBMENU_H

#include <QDialog>

namespace Ui {
class SPC_submenu;
}

class SPC_submenu : public QDialog
{
    Q_OBJECT

public:
    explicit SPC_submenu(QWidget *parent = nullptr);
    ~SPC_submenu();

private slots:
    void receive_spc_sm_path(QString);

private:
    Ui::SPC_submenu *ui;
    QString spc_name;
};

#endif // SPC_SUBMENU_H
