QT += widgets
QT += svg
QT += network

CONFIG += c++17

SOURCES += main.cpp \
    diagramviewer.cpp \
    latexParser.cpp \
    particle.cpp \
    updates.cpp

HEADERS += \
    diagramviewer.h \
    latexParser.h \
    particle.h \
    updates.h \
    version.h \
    windowwithclosesignal.hpp

RESOURCES += \
    resource.qrc

win32:RC_FILE = resource.rc
