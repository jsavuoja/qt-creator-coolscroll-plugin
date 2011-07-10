TARGET = CoolScroll
TEMPLATE = lib

DEFINES += COOLSCROLL_LIBRARY

# CoolScroll files

SOURCES += coolscrollplugin.cpp \
    coolscrollbar.cpp \
    coolscrollbarsettings.cpp

HEADERS += coolscrollplugin.h\
        coolscroll_global.h\
        coolscrollconstants.h \
    coolscrollbar.h \
    coolscrollbarsettings.h

OTHER_FILES = CoolScroll.pluginspec


# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/diver/work/qt-creator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/home/diver/work/qtcreator-build-desktop

PROVIDER = Zhuk

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/coreplugin/coreplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/texteditor/texteditor.pri)

LIBS += -L$$IDE_PLUGIN_PATH/Nokia
