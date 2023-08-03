#ifndef DSM_MENU_H
#define DSM_MENU_H

#include <QDialog>

namespace Ui {
class dsm_menu;
}

class dsm_menu : public QDialog
{
    Q_OBJECT

public:
    explicit dsm_menu(QWidget *parent = nullptr);
    ~dsm_menu();

private:
    Ui::dsm_menu *ui;
};

#endif // DSM_MENU_H
