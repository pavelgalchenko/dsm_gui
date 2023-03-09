#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_new_mission_clicked()
{
    QString homedir = getenv("HOME");
    QString dir_name= QFileDialog::getExistingDirectory(this, tr("Choose Folder"), homedir);

    if (dir_name.isEmpty())
        return;

    path = dir_name+"/InOut/";

    QDir dir(path);

    if (dir.exists()) {
        dir.removeRecursively();
        dir.mkpath(".");
    }
    else {
        dir.mkpath(".");
    }

    ui->mission_path->setText(path);

    QFile::copy(":/data/__default__/Flex_Simple.txt", path+"Flex_Simple.txt");
    QFile::copy(":/data/__default__/Inp_Cmd.txt", path+"Inp_Cmd.txt");
    QFile::copy(":/data/__default__/Inp_FOV.txt", path+"Inp_FOV.txt");
    QFile::copy(":/data/__default__/Inp_Graphics.txt", path+"Inp_Graphics.txt");
    QFile::copy(":/data/__default__/Inp_IPC.txt", path+"Inp_IPC.txt");
    QFile::copy(":/data/__default__/Inp_NOS3.txt", path+"Inp_NOS3.txt");
    QFile::copy(":/data/__default__/Inp_Region.txt", path+"Inp_Region.txt");
    QFile::copy(":/data/__default__/Inp_Sim.txt", path+"Inp_Sim.txt");
    QFile::copy(":/data/__default__/Inp_TDRS.txt", path+"Inp_TDRS.txt");
    QFile::copy(":/data/__default__/Nodes_Simple.txt", path+"Nodes_Simple.txt");
    QFile::copy(":/data/__default__/Orb_LEO.txt", path+"Orb_LEO.txt");
    QFile::copy(":/data/__default__/SC_Simple.txt", path+"SC_Simple.txt");
    QFile::copy(":/data/__default__/Shaker_Simple.txt", path+"Shaker_Simple.txt");
    QFile::copy(":/data/__default__/TRV.txt", path+"TRV.txt");
    QFile::copy(":/data/__default__/Whl_Simple.txt", path+"Whl_Simple.txt");

    ui->GRH_Menu->setEnabled(true);
}

void MainWindow::on_GRH_Menu_clicked()
{
    grh_menu = new GRH_Menu(this);
    grh_menu->setModal(true);
    grh_menu->show();

    connect(this, SIGNAL(send_data(QString)), grh_menu, SLOT(receive_path(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

