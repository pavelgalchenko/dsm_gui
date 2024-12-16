Dependencies:
    Qt5
    yaml-cpp
    pkgconfig (if not using Conda)
This GUI is NOT compatible with Qt6

Instructions:
From top level directory
```
mkdir qt_build
cd qt_build
qmake ../src/dsm_gui.pro
make
```
