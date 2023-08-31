#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setDesktopSettingsAware(false);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/data/dsm_gui.icns"));
    MainWindow w;
    w.show();
    return a.exec();
}
