#include "orb_menu.h"
#include "ui_orb_menu.h"

ORB_Menu::ORB_Menu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ORB_Menu)
{
    ui->setupUi(this);
}

ORB_Menu::~ORB_Menu()
{
    delete ui;
}
