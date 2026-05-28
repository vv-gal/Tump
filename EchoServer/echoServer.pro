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

# Пути к PostgreSQL (MSYS2)
INCLUDEPATH += C:/msys64/mingw64/include
LIBS += -LC:/msys64/mingw64/lib -lpq

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
