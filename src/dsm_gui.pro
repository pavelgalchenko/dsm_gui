QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
ICON = dsm_gui.icns

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONDA_DIR=$$system(echo $CONDA_PREFIX)
equals(CONDA_DIR,"") {
    message(No Conda)
} else {
    INCLUDEPATH += $$CONDA_DIR/include
    LIBS += -L$$CONDA_DIR/lib -lyaml-cpp
}

SOURCES += \
    dsm_gui_lib.cpp \
    dsm_menu.cpp \
    fov_menu.cpp \
    grh_menu.cpp \
    ipc_menu.cpp \
    main.cpp \
    mainwindow.cpp \
    nos_menu.cpp \
    orb_menu.cpp \
    rgn_menu.cpp \
    spc_menu.cpp \
    spc_submenu.cpp \
    sim_menu.cpp \
    tdr_menu.cpp

HEADERS += \
    dsm_gui_lib.h \
    dsm_menu.h \
    fov_menu.h \
    grh_menu.h \
    ipc_menu.h \
    mainwindow.h \
    nos_menu.h \
    orb_menu.h \
    rgn_menu.h \
    spc_menu.h \
    spc_submenu.h \
    sim_menu.h \
    tdr_menu.h \
    qtyaml.h

FORMS += \
    dsm_menu.ui \
    fov_menu.ui \
    grh_menu.ui \
    ipc_menu.ui \
    mainwindow.ui \
    nos_menu.ui \
    orb_menu.ui \
    rgn_menu.ui \
    spc_menu.ui \
    spc_submenu.ui \
    sim_menu.ui \
    tdr_menu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc


