#-------------------------------------------------
#
# Project created by QtCreator 2013-05-03T11:13:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = SUZUKI_ALGO_PART2
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    bloblabel.cpp


INCLUDEPATH += C:\opencv\build\include\

LIBS += -LC:\opencv\build_vs\lib\Debug\
-lopencv_core244d\
-lopencv_highgui244d\
-lopencv_video244d\
-lopencv_imgproc244d\
-lopencv_nonfree244d\
-lopencv_features2d244d\
-lopencv_flann244d\
-lopencv_legacy244d\
-lopencv_calib3d244d\

HEADERS += \
    bloblabel.h
