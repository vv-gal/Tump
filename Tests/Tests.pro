QT += testlib sql network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    tst_funcforserver_test.cpp \
    ../EchoServer/functionsforserver.cpp \
    ../EchoServer/mytcpserver.cpp \
    ../EchoServer/database.cpp \
    ../EchoServer/emailsender.cpp

HEADERS += \
    ../EchoServer/database.h \
    ../EchoServer/functionsforserver.h \
    ../EchoServer/mytcpserver.h \
    ../EchoServer/emailsender.h
