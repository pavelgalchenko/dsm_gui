QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
ICON = dsm_gui.icns

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    grh_menu.cpp \
    main.cpp \
    mainwindow.cpp \
    tdr_menu.cpp

HEADERS += \
    grh_menu.h \
    mainwindow.h \
    tdr_menu.h

FORMS += \
    grh_menu.ui \
    mainwindow.ui \
    tdr_menu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
