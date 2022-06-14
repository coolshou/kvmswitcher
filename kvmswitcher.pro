
TARGET=kvmswitcher

QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += lrelease
CONFIG += embed_translations
# static link
CONFIG += static

#LIBS += -L$$_PRO_FILE_PWD_/hidapi/  -lhidapi
CONFIG += link_pkgconfig
# require libhidapi-dev, libhidapi-libusb,  libusb-1.0-0-dev
PKGCONFIG += hidapi-libusb libusb-1.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/main.cpp \
        src/kvmswitcher.cpp

HEADERS += \
        src/kvmswitcher.h

TRANSLATIONS += \
    trans/kvmswitcher_en_US.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

