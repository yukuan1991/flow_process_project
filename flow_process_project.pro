#-------------------------------------------------
#
# Project created by QtCreator 2017-07-06T09:51:55
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG -= c++11
QMAKE_CXXFLAGS += -std=c++1z

RC_ICONS = png/flow_process.ico

TARGET = flow_process_project
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cc\
        flow_process.cc \
    interface_control/ribbon.cc \
    interface_control/flow_process_ribbon.cc \
    item/carry_item.cc \
    item/checkout_item.cc \
    item/process_item.cc \
    item/symbol_item.cc \
    item/temporary_storage_item.cc \
    item/wait_item.cc \
    interface_control/canvas_scene.cc \
    interface_control/canvas_view.cc \
    interface_control/about_us_dlg.cc \
    interface_control/time_unit_dlg.cc \
    interface_control/distance_unit_dlg.cc

HEADERS  += flow_process.h \
    interface_control/ribbon.h \
    interface_control/flow_process_ribbon.h \
    item/carry_item.h \
    item/checkout_item.h \
    item/process_item.h \
    item/symbol_item.h \
    item/temporary_storage_item.h \
    item/wait_item.h \
    interface_control/canvas_scene.h \
    interface_control/canvas_view.h \
    json.hpp \
    interface_control/about_us_dlg.h \
    interface_control/time_unit_dlg.h \
    interface_control/distance_unit_dlg.h

FORMS    += flow_process.ui

LIBS += -lboost_locale
LIBS += -liconv
LIBS += -lboost_filesystem
LIBS += -lboost_system
