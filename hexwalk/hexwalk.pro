QT += charts
CONFIG += static
RC_ICONS = images/hexwalk.ico
#Only for Windows:
#QMAKE_LFLAGS += -no-pie -lstdc++ -Bstatic -static-libgcc -static-libstdc++ -static
#Only for Mac:
ICON = images/hexwalk.icns
###############

HEADERS = \
    advancedsearchdialog.h \
    binanalysisdialog.h \
    converterdialog.h \
    entropychart.h \
    entropydialog.h \
    hashdialog.h \
    hexwalkmain.h \
    optionsdialog.h \
    ../src/qhexedit.h \
    ../src/chunks.h \
    ../src/commands.h \
    resultType.h \
    searchdialog.h \
    worditemdelegate.h


SOURCES = \
    advancedsearchdialog.cpp \
    binanalysisdialog.cpp \
    converterdialog.cpp \
    entropychart.cpp \
    entropydialog.cpp \
    hashdialog.cpp \
    hexwalkmain.cpp \
    main.cpp \
    optionsdialog.cpp \
    ../src/qhexedit.cpp \
    ../src/chunks.cpp \
    ../src/commands.cpp \
    searchdialog.cpp \
    worditemdelegate.cpp

RESOURCES = \
    hexwalk.qrc

FORMS += \
    advancedsearchdialog.ui \
    binanalysisdialog.ui \
    converterdialog.ui \
    entropydialog.ui \
    hashdialog.ui \
    hexwalkmain.ui \
    optionsdialog.ui \
    searchdialog.ui

DEFINES += QHEXEDIT_EXPORTS
