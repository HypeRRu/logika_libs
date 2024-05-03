QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    logika_backend.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    logika_backend.h \
    logika_defs.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

DEFINES += "LOGIKA_USE_CONNECTIONS="
DEFINES += "LOGIKA_USE_CONNECTIONS_SERIAL="
DEFINES += "LOGIKA_USE_CONNECTIONS_NETWORK="
DEFINES += "LOGIKA_USE_LOGGER="
DEFINES += "LOGIKA_USE_METERS="
DEFINES += "LOGIKA_USE_METERS4="
DEFINES += "LOGIKA_USE_METERS4L="
DEFINES += "LOGIKA_USE_METERS4M="
DEFINES += "LOGIKA_USE_PROTOCOL_M4="
DEFINES += "LOGIKA_USE_RESOURCE_CONVERTERS="
DEFINES += "LOGIKA_RESOURCES_EXPORT="

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../logika_build/lib/ \
    -llogika_common \
    -llogika_connections_common \
    -llogika_connections_network \
    -llogika_connections_serial \
    -llogika_meters_common \
    -llogika_meters4 \
    -llogika_meters4l \
    -llogika_meters4m \
    -llogika_protocols \
    -llogika_logger \
    -llogika_resources \
    -llogika_resource_converters

unix:!macx: LIBS += -L/usr/local/lib/ -lprotobuf

INCLUDEPATH += $$PWD/../logika_build/include
INCLUDEPATH += /usr/local/include
DEPENDPATH += $$PWD/../logika_build/include
