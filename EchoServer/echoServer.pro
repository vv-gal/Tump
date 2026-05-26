QT -= gui
QT += core network sql

CONFIG += c++17 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mytcpserver.cpp \
    functionsforserver.cpp \
    database.cpp \
    emailsender.cpp

HEADERS += \
    mytcpserver.h \
    functionsforserver.h \
    database.h \
    emailsender.h

TARGET = EchoServer

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
