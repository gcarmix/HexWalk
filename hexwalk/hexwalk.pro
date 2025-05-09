QT += charts
CONFIG += static
RC_ICONS = images/hexwalk.ico
#Only for Windows:
win32:QMAKE_LFLAGS += -no-pie -lstdc++ -Bstatic -static-libgcc -static-libstdc++ -static
#Only for Mac:
macx:ICON = images/hexwalk.icns
###############
VERSION = "1.9.1"
QMAKE_TARGET_COPYRIGHT = "gcarmix"
QMAKE_TARGET_PRODUCT = "HexWalk"
HEADERS = \
    ../qhexedit/bytepattern.h \
    ../qhexedit/colortag.h \
    ../qhexedit/tagparser.hpp \
    advancedsearchdialog.h \
    binanalysisdialog.h \
    bytemap.h \
    bytemapdialog.h \
    converterwidget.h \
    diffdialog.h \
    disasmwidget.h \
    edittagdialog.h \
    entropychart.h \
    entropydialog.h \
    hashdialog.h \
    hexwalkmain.h \
    optionsdialog.h \
    ../qhexedit/qhexedit.h \
    ../qhexedit/chunks.h \
    ../qhexedit/commands.h \
    resultType.h \
    searchdialog.h \
    stringsdialog.h \
    tagsdialog.h \
    worditemdelegate.h


SOURCES = \
    ../qhexedit/bytepattern.cpp \
    ../qhexedit/colortag.cpp \
    ../qhexedit/tagparser.cpp \
    advancedsearchdialog.cpp \
    binanalysisdialog.cpp \
    bytemap.cpp \
    bytemapdialog.cpp \
    converterwidget.cpp \
    diffdialog.cpp \
    disasmwidget.cpp \
    edittagdialog.cpp \
    entropychart.cpp \
    entropydialog.cpp \
    hashdialog.cpp \
    hexwalkmain.cpp \
    main.cpp \
    optionsdialog.cpp \
    ../qhexedit/qhexedit.cpp \
    ../qhexedit/chunks.cpp \
    ../qhexedit/commands.cpp \
    searchdialog.cpp \
    stringsdialog.cpp \
    tagsdialog.cpp \
    worditemdelegate.cpp

RESOURCES = \
    hexwalk.qrc

FORMS += \
    advancedsearchdialog.ui \
    binanalysisdialog.ui \
    bytemapdialog.ui \
    converterwidget.ui \
    diffdialog.ui \
    disasmwidget.ui \
    edittagdialog.ui \
    entropydialog.ui \
    hashdialog.ui \
    hexwalkmain.ui \
    optionsdialog.ui \
    searchdialog.ui \
    stringsdialog.ui \
    tagsdialog.ui

DEFINES += QHEXEDIT_EXPORTS \
    APP_VERSION=\\\"$$VERSION\\\"

DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32
macx:OBJECTS_DIR = ../build/o/mac

win32:LIBS += -L$$PWD/../capstone/build/ -lcapstone
unix:LIBS += -L$$PWD/../capstone/ -lcapstone
macx:LIBS += -L$$PWD/../capstone/ -lcapstone

INCLUDEPATH += $$PWD/../capstone/include
DEPENDPATH += $$PWD/../capstone/include

win32: PRE_TARGETDEPS += $$PWD/../capstone/build/libcapstone.a
unix: PRE_TARGETDEPS += $$PWD/../capstone/libcapstone.a
macx: PRE_TARGETDEPS += $$PWD/../capstone/libcapstone.a
