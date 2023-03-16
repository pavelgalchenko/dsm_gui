QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
ICON = dsm_gui.icns

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    color_select.cpp \
    fov_menu.cpp \
    grh_menu.cpp \
    main.cpp \
    mainwindow.cpp \
    nos_menu.cpp \
    rgn_menu.cpp \
    tdr_menu.cpp

HEADERS += \
    color_select.h \
    fov_menu.h \
    grh_menu.h \
    mainwindow.h \
    nos_menu.h \
    rgn_menu.h \
    tdr_menu.h

FORMS += \
    color_select.ui \
    fov_menu.ui \
    grh_menu.ui \
    mainwindow.ui \
    nos_menu.ui \
    rgn_menu.ui \
    tdr_menu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
