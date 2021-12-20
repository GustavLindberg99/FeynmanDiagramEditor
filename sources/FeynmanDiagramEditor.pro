QT += widgets
QT += svg

CONFIG += c++17

SOURCES += main.cpp \
    diagramviewer.cpp \
    latexParser.cpp \
    particle.cpp

HEADERS += \
    diagramviewer.h \
    latexParser.h \
    particle.h \
    version.h \
    windowwithclosesignal.hpp

RESOURCES += \
    resource.qrc

win32:RC_FILE = resource.rc
