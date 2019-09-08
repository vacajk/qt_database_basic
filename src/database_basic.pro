#-------------------------------------------------
#
# Project created by QtCreator 2019-09-08T21:01:22
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = database_basic
TEMPLATE = app

CONFIG  += c++11

SOURCES += main.cpp\
        database_basic.cpp \
    database_samples_db.cpp

HEADERS  += database_basic.h \
    database_samples_db.h

FORMS    += database_basic.ui
