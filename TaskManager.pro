QT       += core gui widgets

CONFIG   += c++17

TARGET   = TaskManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS   += \
    mainwindow.ui

DEFINES += QT_DEPRECATED_WARNINGS
