#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "grh_menu.h"
#include "tdr_menu.h"
#include "fov_menu.h"
#include "nos_menu.h"
#include "rgn_menu.h"
#include "ipc_menu.h"

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void send_data(QString);

private slots:

    int warning_message(QString);

    void on_new_mission_clicked();

    void on_GRH_Menu_clicked();
    void on_TDR_Menu_clicked();
    void on_FOV_Menu_clicked();
    void on_NOS_Menu_clicked();
    void on_RGN_Menu_clicked();
    void on_IPC_Menu_clicked();

private:
    Ui::MainWindow *ui;

    GRH_Menu *grh_menu;
    TDR_Menu *tdr_menu;
    FOV_Menu *fov_menu;
    NOS_Menu *nos_menu;
    RGN_Menu *rgn_menu;
    IPC_Menu *ipc_menu;
    QString path;
};
#endif // MAINWINDOW_H
