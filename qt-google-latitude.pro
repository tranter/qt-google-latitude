#-------------------------------------------------
#
# Project created by QtCreator 2012-02-01T23:40:41
#
#-------------------------------------------------

QT       += core gui webkit network

TARGET = qtlatitude
TEMPLATE = app

linux* {
LIBS += ../qjson/build/lib/libqjson.so
INCLUDEPATH += ../qjson/include
}

win* {
LIBS += ../qjson/build/lib/qjson0.lib
INCLUDEPATH += ../qjson/include
}

macx* {
LIBS += -F../qjson/build/lib -framework qjson
INCLUDEPATH += ../qjson/include
}

SOURCES += main.cpp\
        mainwindow.cpp \
    form.cpp \
    oauth2.cpp \
    logindialog.cpp \
    latitude_data_manager.cpp \
    selectaddressdlg.cpp

HEADERS  += mainwindow.h \
    form.h \
    oauth2.h \
    logindialog.h \
    latitude_data_manager.h \
    selectaddressdlg.h

FORMS    += mainwindow.ui \
    form.ui \
    logindialog.ui \
    selectaddressdlg.ui

RESOURCES += \
    resource.qrc











