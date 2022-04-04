VERSION = 0.0.1.0
QMAKE_TARGET_COMPANY = WIN-DESIGN
QMAKE_TARGET_PRODUCT = exportDigikam
QMAKE_TARGET_DESCRIPTION = exportDigikam
QMAKE_TARGET_COPYRIGHT = (c) 2022 WIN-DESIGN

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32-msvc* {
    contains(QT_ARCH, i386) {
        message("msvc 32-bit")
    } else {
        message("msvc 64-bit")
    }
}

win32-g++ {
    message("mingw")
    INCLUDEPATH += C:\dev\3rdParty\exiv2-0.27.5\include C:\dev\3rdParty\libraw C:\dev\3rdParty\libjpeg\include C:\dev\3rdParty\opencv\include dng
    LIBS += -LC:\dev\3rdParty\exiv2-0.27.5\lib -lexiv2.dll -LC:\dev\3rdParty\libraw\lib -LC:\dev\3rdParty\opencv\x64\mingw\lib -lraw -lws2_32 -lz -lopencv_core412.dll -lopencv_imgproc412.dll
}

unix {
    message("*nix")
    LIBS += -lraw -lexiv2
}

INCLUDEPATH += ./pgfutils/libpgf

QMAKE_CXXFLAGS += -DLIBRAW_NODLL -DLIBRAW_NOTHREADS

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    calbumroots.cpp \
    calbums.cpp \
    cexportdialog.cpp \
    cfoldersortfilterproxymodel.cpp \
    cimage.cpp \
    cimages.cpp \
    clogwindow.cpp \
    common.cpp \
    cthumbnailsortfilterproxymodel.cpp \
    main.cpp \
    cmainwindow.cpp \
    pgfutils/libpgf/Decoder.cpp \
    pgfutils/libpgf/Encoder.cpp \
    pgfutils/libpgf/PGFimage.cpp \
    pgfutils/libpgf/PGFstream.cpp \
    pgfutils/libpgf/Subband.cpp \
    pgfutils/libpgf/WaveletTransform.cpp \
    pgfutils/pgfutils.cpp

HEADERS += \
    calbumroots.h \
    calbums.h \
    cexportdialog.h \
    cfoldersortfilterproxymodel.h \
    cimage.h \
    cimages.h \
    clogwindow.h \
    cmainwindow.h \
    common.h \
    cthumbnailsortfilterproxymodel.h \
    pgfutils/libpgf/BitStream.h \
    pgfutils/libpgf/Decoder.h \
    pgfutils/libpgf/Encoder.h \
    pgfutils/libpgf/PGFimage.h \
    pgfutils/libpgf/PGFplatform.h \
    pgfutils/libpgf/PGFstream.h \
    pgfutils/libpgf/PGFtypes.h \
    pgfutils/libpgf/Subband.h \
    pgfutils/libpgf/WaveletTransform.h \
    pgfutils/pgfutils.h

FORMS += \
    cexportdialog.ui \
    clogwindow.ui \
    cmainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    pgfutils/libpgf/README

RESOURCES += \
    exportDigikam.qrc
