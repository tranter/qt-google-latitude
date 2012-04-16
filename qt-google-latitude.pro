#-------------------------------------------------
#
# Project created by QtCreator 2012-02-01T23:40:41
#
#-------------------------------------------------

QT  += core gui webkit network

TARGET = qtlatitude
TEMPLATE = app

INCLUDEPATH += ../qjson/include

linux* {
    LIBS += ../qjson/build/lib/libqjson.so
}

win32-g++ {
    #MinGW compiler for Windows
    message(Configured for Windows (Qt Version: $$QT_VERSION; $$QMAKE_CC))

    LIBS += ../qjson/build/lib/libqjson0.a
}

win32-msvc* {
    #MSVC compiler for Windows
    LIBS += ../qjson/build/lib/qjson0.lib
    message(Configured for Windows (Qt Version: $$QT_VERSION; $$QMAKE_CC))
}

macx* {
    LIBS += -F../qjson/build/lib -framework qjson
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











