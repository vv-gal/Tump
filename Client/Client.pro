QT += core gui network widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG -= app_bundle
CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginwindow.cpp \
    qcustomplot.cpp \
    registerwindow.cpp \
    tcpclient.cpp \
    taskwindow.cpp \
    graphwindow.cpp

HEADERS += \
    mainwindow.h \
    loginwindow.h \
    qcustomplot.h \
    registerwindow.h \
    tcpclient.h \
    taskwindow.h \
    graphwindow.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
