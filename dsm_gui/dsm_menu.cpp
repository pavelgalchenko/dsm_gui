#include "dsm_menu.h"
#include "ui_dsm_menu.h"

dsm_menu::dsm_menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dsm_menu)
{
    ui->setupUi(this);
}

dsm_menu::~dsm_menu()
{
    delete ui;
}
