TARGET = CoolScroll
TEMPLATE = lib

DEFINES += COOLSCROLL_LIBRARY

QTC_PLUGIN_NAME = CoolScroll
QTC_PLUGIN_DEPENDS = \
    texteditor \
    coreplugin

# CoolScroll files

SOURCES += coolscrollplugin.cpp \
    coolscrollbar.cpp \
    coolscrollbarsettings.cpp \
    settingspage.cpp \
    settingsdialog.cpp

HEADERS += coolscrollplugin.h\
        coolscroll_global.h\
        coolscrollconstants.h \
    coolscrollbar.h \
    coolscrollbarsettings.h \
    settingspage.h \
    settingsdialog.h

OTHER_FILES = CoolScroll.pluginspec

# Qt Creator linking

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/tmp/qtc

PROVIDER = Zhuk

LIBS += -L$$(LIBSROOT) \
-L$$(LIBSROOT)/qtcreator \
-L$$(LIBSROOT)/qtcreator/plugins

###
# Set the QTC_SOURCE environment variable to set the setting here

include($$(QTC_SOURCE)/src/qtcreatorplugin.pri)
#include($$(QTC_SOURCE)/src/plugins/coreplugin/coreplugin.pri)
#include($$(QTC_SOURCE)/src/plugins/coreplugin/coreplugin.pri)
#include($$(QTC_SOURCE)/src/plugins/texteditor/texteditor.pri)

FORMS += \
    settingsdialog.ui

message(QTC_SOURCE = $$(QTC_SOURCE))
message(LIBSROOT = $$(LIBSROOT))
message(QTC_BUILD = $$(QTC_BUILD))
message(Good luck with make...)
