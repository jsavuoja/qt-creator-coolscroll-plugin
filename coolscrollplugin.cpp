/*
*
* Copyright (C) 2011 EgorZhuk
*
* Authors: Egor Zhuk <egor.zhuk@gmail.com>
*
* This file is part of CoolScroll plugin for QtCreator.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "coolscrollplugin.h"
#include "coolscrollconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <texteditor/basetexteditor.h>

#include <QtGui/QMainWindow>
#include <QtGui/QScrollBar>
#include <QtGui/QPushButton>

#include <QtCore/QtPlugin>

#include "coolscrollbarsettings.h"
#include "coolscrollbar.h"
#include "settingspage.h"

namespace
{
    const QString l_nSettingsGroup("CoolScroll");
}

using namespace CoolScroll::Internal;

CoolScrollPlugin::CoolScrollPlugin() :
    m_settings(new CoolScrollbarSettings)
{
    readSettings();
}

CoolScrollPlugin::~CoolScrollPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool CoolScrollPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    SettingsPage* settingsPage = new SettingsPage(m_settings);
    connect(settingsPage, SIGNAL(settingsChanged()), SLOT(settingChanged()));
    addAutoReleasedObject(settingsPage);

    Core::EditorManager* em = Core::ICore::instance()->editorManager();
    connect(em, SIGNAL(editorCreated(Core::IEditor*,QString)),
                SLOT(editorCreated(Core::IEditor*,QString)));

    return true;
}

void CoolScrollPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
}

ExtensionSystem::IPlugin::ShutdownFlag CoolScrollPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void CoolScrollPlugin::editorCreated(Core::IEditor *editor, const QString &fileName)
{
    Q_UNUSED(fileName);
    TextEditor::BaseTextEditorWidget* baseEditor = qobject_cast<TextEditor::BaseTextEditorWidget*>
                                                   (editor->widget());

    baseEditor->setVerticalScrollBar(new CoolScrollBar(baseEditor, m_settings));

}

void CoolScroll::Internal::CoolScrollPlugin::readSettings()
{
    QSettings *settings = Core::ICore::instance()->settings();
    settings->beginGroup(l_nSettingsGroup);
    m_settings->read(settings);
    settings->endGroup();
}

void CoolScroll::Internal::CoolScrollPlugin::saveSettings()
{
    QSettings *settings = Core::ICore::instance()->settings();
    settings->beginGroup(l_nSettingsGroup);
    m_settings->save(settings);
    settings->endGroup();
    settings->sync();
}

void CoolScroll::Internal::CoolScrollPlugin::settingChanged()
{
    saveSettings();
    Core::EditorManager* em = Core::ICore::instance()->editorManager();

    QList<Core::IEditor*> editors = em->openedEditors();
    QList<Core::IEditor*>::iterator it = editors.begin();
    // editors will update settings after next opening
    for( ; it != editors.end(); ++it)
    {
        CoolScrollBar* bar = getEditorScrollBar(*it);
        Q_ASSERT(bar);
        bar->markStateDirty();
    }
    // update current editor right now
    CoolScrollBar* bar = getEditorScrollBar(em->currentEditor());
    Q_ASSERT(bar);
    bar->fullUpdateSettings();
}


CoolScrollBar * CoolScrollPlugin::getEditorScrollBar(Core::IEditor *editor)
{
    TextEditor::BaseTextEditorWidget* baseEditor = qobject_cast<TextEditor::BaseTextEditorWidget*>
                                                                (editor->widget());
    return qobject_cast<CoolScrollBar*>(baseEditor->verticalScrollBar());
}

Q_EXPORT_PLUGIN2(CoolScroll, CoolScrollPlugin)
