#include "mainwindow.h"
#include "qglobal.h"
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
    ui->Warning->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_new_mission_clicked()
{
    QString homedir = getenv("HOME");
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Choose Folder"), homedir, QFileDialog::DontUseNativeDialog);

    if (dir_name.isEmpty())
        return;

    path = dir_name+"/InOut/";

    QDir dir(path);

    if (dir.exists()) {
        int response = dsm_gui_lib::warning_message("Overwrite InOut in Selected Directory?");
        if (response == QMessageBox::Ok) {
            dir.removeRecursively();
            dir.mkpath(".");
            dir.mkpath("./__default__/");
        }
        else {
            return;
        }
    }
    else {
        dir.mkpath(".");
        dir.mkpath("./__default__/");
    }

    ui->mission_path->setText(path);

    QFile::copy(":/data/__default__/Flex_Simple.txt", path+"Flex_Simple.txt");
    QFile::copy(":/data/__default__/Inp_Cmd.txt", path+"Inp_Cmd.txt");
    QFile::copy(":/data/__default__/Inp_FOV.txt", path+"Inp_FOV.txt");
    QFile::copy(":/data/__default__/Inp_Graphics.txt", path+"Inp_Graphics.txt");
    QFile::copy(":/data/__default__/Inp_IPC.txt", path+"Inp_IPC.txt");
    QFile::copy(":/data/__default__/Inp_DSM.txt", path+"Inp_DSM.txt");
    QFile::copy(":/data/__default__/Inp_NOS3.txt", path+"Inp_NOS3.txt");
    QFile::copy(":/data/__default__/Inp_Region.txt", path+"Inp_Region.txt");
    QFile::copy(":/data/__default__/Inp_Sim.txt", path+"Inp_Sim.txt");
    QFile::copy(":/data/__default__/Inp_TDRS.txt", path+"Inp_TDRS.txt");
    QFile::copy(":/data/__default__/Nodes_Simple.txt", path+"Nodes_Simple.txt");
    QFile::copy(":/data/__default__/Orb_Default.txt", path+"Orb_Default.txt");
    QFile::copy(":/data/__default__/SC_Simple.txt", path+"SC_Simple.txt");
    QFile::copy(":/data/__default__/Shaker_Simple.txt", path+"Shaker_Simple.txt");
    QFile::copy(":/data/__default__/TRV.txt", path+"TRV.txt");
    QFile::copy(":/data/__default__/Whl_Simple.txt", path+"Whl_Simple.txt");

    QFile::copy(":/data/__default__/Flex_Simple.txt", path+"__default__/Flex_Simple.txt");
    QFile::copy(":/data/__default__/Inp_Cmd.txt", path+"__default__/Inp_Cmd.txt");
    QFile::copy(":/data/__default__/Inp_FOV.txt", path+"__default__/Inp_FOV.txt");
    QFile::copy(":/data/__default__/Inp_Graphics.txt", path+"__default__/Inp_Graphics.txt");
    QFile::copy(":/data/__default__/Inp_IPC.txt", path+"__default__/Inp_IPC.txt");
    QFile::copy(":/data/__default__/Inp_DSM.txt", path+"__default__/Inp_DSM.txt");
    QFile::copy(":/data/__default__/Inp_NOS3.txt", path+"__default__/Inp_NOS3.txt");
    QFile::copy(":/data/__default__/Inp_Region.txt", path+"__default__/Inp_Region.txt");
    QFile::copy(":/data/__default__/Inp_Sim.txt", path+"__default__/Inp_Sim.txt");
    QFile::copy(":/data/__default__/Inp_TDRS.txt", path+"__default__/Inp_TDRS.txt");
    QFile::copy(":/data/__default__/Nodes_Simple.txt", path+"__default__/Nodes_Simple.txt");
    QFile::copy(":/data/__default__/Orb_Default.txt", path+"__default__/Orb_Default.txt");
    QFile::copy(":/data/__default__/SC_Simple.txt", path+"__default__/SC_Simple.txt");
    QFile::copy(":/data/__default__/Shaker_Simple.txt", path+"__default__/Shaker_Simple.txt");
    QFile::copy(":/data/__default__/TRV.txt", path+"__default__/TRV.txt");
    QFile::copy(":/data/__default__/Whl_Simple.txt", path+"__default__/Whl_Simple.txt");

    QStringList newDefaultFiles = QDir(":/data/__default__/").entryList();

    foreach (QString neededFile, newDefaultFiles) {
        QFile::copy(":/data/__default__/"+neededFile,path+neededFile);
        QFile::copy(":/data/__default__/"+neededFile,path+"__default__/"+neededFile);
    }

    ui->GRH_Menu->setEnabled(true);
    ui->TDR_Menu->setEnabled(true);
    ui->FOV_Menu->setEnabled(true);
    ui->NOS_Menu->setEnabled(true);
    ui->RGN_Menu->setEnabled(true);
    ui->IPC_Menu->setEnabled(true);
    ui->SPC_Menu->setEnabled(true);
    ui->ORB_Menu->setEnabled(true);
    ui->SIM_Menu->setEnabled(true);
    ui->DSM_Menu->setEnabled(true);
}

void MainWindow::on_load_mission_clicked() {
    QString homedir = getenv("HOME");
    QString oldPath = path;
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Choose Folder"), homedir, QFileDialog::DontUseNativeDialog);
    bool missingSim = false;
    int response;

    if (dir_name.isEmpty())
        return;

    QString defaultPath = dir_name+"/InOut/__default__/";
    path = dir_name+"/InOut/";

    QDir defaultDir(defaultPath);
    QDir dir(path);

    if (dir.exists()) {
        response = dsm_gui_lib::warning_message("Open "+dir_name+"?");
        if (response != QMessageBox::Ok) {
            path= oldPath;
            return;
        }
    }

    ui->mission_path->setText(path);

    if (!dir.exists()) {
        response = dsm_gui_lib::warning_message("Missing "+path+"\nMake directory and continue?");
        if (response == QMessageBox::Ok) {
        }
        else {
            path = oldPath;
            ui->mission_path->setText(path);
            return;
        }
        dir.mkpath(".");
    }
    if (!defaultDir.exists()) {
        response = dsm_gui_lib::warning_message("Missing "+defaultPath+"\nMake directory and continue?");
        if (response == QMessageBox::Ok) {
        }
        else {
            path = oldPath;
            ui->mission_path->setText(path);
            return;
        }
        defaultDir.mkpath(".");
        QStringList newDefaultFiles = QDir(path).entryList();
        foreach (QString neededFile, newDefaultFiles) {
            QFile::copy(path+neededFile,defaultPath+neededFile);
        }

    }

    QStringList curFiles = QDir(path).entryList();
    QStringList defaultFiles = QDir(defaultPath).entryList();

    // Make sure that the "Inp_*" files exist in dir_name+"/__default__/".
    // If they don't, copy from the resources into default.
    // Then, make sure that "Inp_*" file exists in dir_name_"/InOut/",
    // copying from dir_name+"/__default__/" if it doesn't.
    foreach (QString neededFile, neededFiles) {

        if (!defaultFiles.contains(neededFile)) {
            response = dsm_gui_lib::warning_message("Missing "+neededFile+" in "+defaultPath+"\nLoad from resources and continue?");
            if (response == QMessageBox::Ok) {
            }
            else {
                path = oldPath;
                ui->mission_path->setText(path);
                return;
            }
            QFile::copy(":/data/__default__/"+neededFile, defaultPath+neededFile);
        }

        if (!curFiles.contains(neededFile)) {
            response = dsm_gui_lib::warning_message("Missing "+neededFile+" in "+path+"\nLoad from resources and continue?");
            if (response == QMessageBox::Ok) {
            }
            else {
                path = oldPath;
                ui->mission_path->setText(path);
                return;
            }
            QFile::copy(defaultPath+neededFile,path+neededFile);
            if (neededFile.compare("Inp_Sim.txt") == 0)
                missingSim = true;
        }
    }

    ui->SPC_Menu->setEnabled(true);
    ui->ORB_Menu->setEnabled(true);
    ui->SIM_Menu->setEnabled(true);
    if (!missingSim)
        enable_sub_menus();
    else
        disable_sub_menus();
}

void MainWindow::on_GRH_Menu_clicked()
{
    grh_menu = new GRH_Menu(this);
    grh_menu->setModal(true);
    grh_menu->show();

    connect(this, SIGNAL(send_data(QString)), grh_menu, SLOT(receive_grhpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_TDR_Menu_clicked()
{
    tdr_menu = new TDR_Menu(this);
    tdr_menu->setModal(true);
    tdr_menu->show();

    connect(this, SIGNAL(send_data(QString)), tdr_menu, SLOT(receive_tdrpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}


void MainWindow::on_FOV_Menu_clicked()
{
    QStringList simFiles = QDir(path).entryList({"Inp_Sim**"});
    QStringList scFiles = QDir(path).entryList({"SC_*"});
    if (scFiles.isEmpty() || simFiles.isEmpty()){
        dsm_gui_lib::error_message("There must be both a Inp_Sim file and a Spacecraft file before editing the FOV file.");
        return;
    }

    fov_menu = new FOV_Menu(this);
    fov_menu->setModal(true);
    fov_menu->show();

    connect(this, SIGNAL(send_data(QString)), fov_menu, SLOT(receive_fovpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}


void MainWindow::on_NOS_Menu_clicked()
{
    nos_menu = new NOS_Menu(this);
    nos_menu->setModal(true);
    nos_menu->show();

    connect(this, SIGNAL(send_data(QString)), nos_menu, SLOT(receive_nospath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_RGN_Menu_clicked()
{
    rgn_menu = new RGN_Menu(this);
    rgn_menu->setModal(true);
    rgn_menu->show();

    connect(this, SIGNAL(send_data(QString)), rgn_menu, SLOT(receive_rgnpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_IPC_Menu_clicked()
{
    ipc_menu = new IPC_Menu(this);
    ipc_menu->setModal(true);
    ipc_menu->show();

    connect(this, SIGNAL(send_data(QString)), ipc_menu, SLOT(receive_ipcpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_SPC_Menu_clicked()
{
    spc_menu = new SPC_Menu(this);
    spc_menu->setModal(true);
    spc_menu->show();

    connect(this, SIGNAL(send_data(QString)), spc_menu, SLOT(receive_spcpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_ORB_Menu_clicked()
{
    orb_menu = new ORB_Menu(this);
    orb_menu->setModal(true);
    orb_menu->show();

    connect(this, SIGNAL(send_data(QString)), orb_menu, SLOT(receive_orbpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_SIM_Menu_clicked()
{
    QStringList orbFiles = QDir(path).entryList({"Orb_*"});
    QStringList scFiles = QDir(path).entryList({"SC_*"});
    if (scFiles.isEmpty() || orbFiles.isEmpty()){
        // is this true???
        dsm_gui_lib::error_message("There must be both a Orbit file and a Spacecraft file before editing the Simulation file.");
        return;
    }

    sim_menu = new SIM_Menu(this);
    sim_menu->setModal(true);
    sim_menu->show();

    connect(this, SIGNAL(send_data(QString)), sim_menu, SLOT(receive_simpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::on_DSM_Menu_clicked() {
    dsm_menu = new DSM_Menu(this);
    dsm_menu->setModal(true);
    dsm_menu->show();

    connect(this, SIGNAL(send_data(QString)), dsm_menu, SLOT(receive_dsmpath(QString)));
    emit send_data(path);
    disconnect(this, SIGNAL(send_data(QString)), 0, 0);
}

void MainWindow::enable_sub_menus() {
    ui->Warning->setVisible(false);
    ui->GRH_Menu->setEnabled(true);
    ui->FOV_Menu->setEnabled(true);
    ui->TDR_Menu->setEnabled(true);
    ui->NOS_Menu->setEnabled(true);
    ui->RGN_Menu->setEnabled(true);
    ui->IPC_Menu->setEnabled(true);
}

void MainWindow::disable_sub_menus() {
    ui->Warning->setVisible(true);
    ui->GRH_Menu->setEnabled(false);
    ui->FOV_Menu->setEnabled(false);
    ui->TDR_Menu->setEnabled(false);
    ui->NOS_Menu->setEnabled(false);
    ui->RGN_Menu->setEnabled(false);
    ui->IPC_Menu->setEnabled(false);
}

void MainWindow::set_menu_buttons(bool enabled) {
    ui->SIM_Menu->setEnabled(enabled);
    ui->ORB_Menu->setEnabled(enabled);
    ui->SPC_Menu->setEnabled(enabled);
    ui->GRH_Menu->setEnabled(enabled);
    ui->FOV_Menu->setEnabled(enabled);
    ui->TDR_Menu->setEnabled(enabled);
    ui->NOS_Menu->setEnabled(enabled);
    ui->RGN_Menu->setEnabled(enabled);
    ui->IPC_Menu->setEnabled(enabled);
}
