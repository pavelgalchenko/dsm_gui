#include "spc_submenu.h"
#include "ui_spc_submenu.h"

SPC_submenu::SPC_submenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SPC_submenu)
{
    ui->setupUi(this);
}

SPC_submenu::~SPC_submenu()
{
    delete ui;
}
