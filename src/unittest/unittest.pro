#-------------------------------------------------
#
# Project created by QtCreator 2016-10-17T21:11:21
#
#-------------------------------------------------

QT       += testlib
QT       -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport


TARGET = tst_unittesttest
CONFIG	 += c++11
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ".."



SOURCES += tst_unittesttest.cpp \
    CSVReader.cpp \
    TestCSVReader.cpp \
    CustomSort.cpp \
    QSortListIO.cpp \
    mainwindow.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
