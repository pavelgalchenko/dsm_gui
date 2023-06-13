#ifndef SPC_MENU_H
#define SPC_MENU_H

#include <QDialog>

namespace Ui {
class SPC_menu;
}

class SPC_menu : public QDialog
{
    Q_OBJECT

public:
    explicit SPC_menu(QWidget *parent = nullptr);
    ~SPC_menu();

private:
    Ui::SPC_menu *ui;
};

#endif // SPC_MENU_H
