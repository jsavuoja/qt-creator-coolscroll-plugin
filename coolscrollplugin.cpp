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

#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QPushButton>
#include <QtGui/QTextBlock>

#include <QtCore/QtPlugin>

#include <QDebug>

#include "coolscrollbar.h"

using namespace CoolScroll::Internal;

CoolScrollPlugin::CoolScrollPlugin()
{
    // Create your members
}

CoolScrollPlugin::~CoolScrollPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool CoolScrollPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // connect to other plugins' signals
    // "In the initialize method, a plugin can be sure that the plugins it
    //  depends on have initialized their members."

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    Core::ActionManager *am = Core::ICore::instance()->actionManager();

    QAction *action = new QAction(tr("CoolScroll action"), this);
    Core::Command *cmd = am->registerAction(action, Constants::ACTION_ID,
                         Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));

    Core::ActionContainer *menu = am->createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("CoolScroll"));
    menu->addAction(cmd);
    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    Core::EditorManager* em = Core::ICore::instance()->editorManager();
    connect(em, SIGNAL(editorCreated(Core::IEditor*,QString)),
                SLOT(on_editorCreated(Core::IEditor*,QString)));

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

void CoolScrollPlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::instance()->mainWindow(),
                             tr("Action triggered"),
                             tr("This is an action from CoolScroll."));
}

void CoolScrollPlugin::on_editorCreated(Core::IEditor *editor, const QString &fileName)
{
    TextEditor::BaseTextEditorWidget* baseEditor = qobject_cast<TextEditor::BaseTextEditorWidget*>
                                                   (editor->widget());

    baseEditor->setVerticalScrollBar(new CoolScrollBar(baseEditor));

}

Q_EXPORT_PLUGIN2(CoolScroll, CoolScrollPlugin)


