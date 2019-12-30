#-------------------------------------------------
#
# Project created by QtCreator 2015-02-18T17:32:34
#
#-------------------------------------------------
INCLUDEPATH += /usr/local/include

INCLUDEPATH += /usr/local/Cellar/opencv@2/2.4.13.7_5/include

LIBS += -L/usr/local/lib

LIBS += -L/usr/local/Cellar/opencv@2/2.4.13.7_5/lib

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_features2d

LIBS += -L/Users/giuseppe/flandmark-master/bin/libflandmark/ -lflandmark_shared -lflandmark_static

INCLUDEPATH += /Users/giuseppe/flandmark-master/libflandmark/
DEPENDPATH += /Users/giuseppe/flandmark-master/libflandmark/

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicaHumana
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    selezionaformavolto.cpp \
    dialogsettings.cpp \
    dialogusadefaultpath.cpp

HEADERS  += mainwindow.h \
    CVImageWidget.h \
    selezionaformavolto.h \
    dialogsettings.h \
    dialogusadefaultpath.h

FORMS    += mainwindow.ui \
    selezionaformavolto.ui \
    dialogsettings.ui \
    dialogusadefaultpath.ui

RESOURCES = resources.qrc

