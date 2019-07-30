TARGET = bin/screenshot
TEMPLATE = app

QT += gui core widgets

INCLUDEPATH += . \
    include

SOURCES += \
    src/main.cpp \
    src/screenshot.cpp \
    src/widgetwindow.cpp

HEADERS += \
    include/screenshot.h \
    include/widgetwindow.h

RESOURCES += res.qrc

MOC_DIR += build
OBJECTS_DIR += build
RCC_DIR += build