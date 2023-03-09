#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "grh_menu.h"

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

    void on_new_mission_clicked();

    void on_GRH_Menu_clicked();

private:
    Ui::MainWindow *ui;

    GRH_Menu *grh_menu;
    QString path;
};
#endif // MAINWINDOW_H
