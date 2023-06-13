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

private:
    Ui::SPC_submenu *ui;
};

#endif // SPC_SUBMENU_H
