#ifndef ORB_MENU_H
#define ORB_MENU_H

#include <QMainWindow>

namespace Ui {
class ORB_Menu;
}

class ORB_Menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit ORB_Menu(QWidget *parent = nullptr);
    ~ORB_Menu();

private:
    Ui::ORB_Menu *ui;
};

#endif // ORB_MENU_H
