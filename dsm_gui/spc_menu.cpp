#include "spc_menu.h"
#include "ui_spc_menu.h"

SPC_menu::SPC_menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SPC_menu)
{
    ui->setupUi(this);
}

SPC_menu::~SPC_menu()
{
    delete ui;
}
