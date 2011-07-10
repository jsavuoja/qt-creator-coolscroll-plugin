#ifndef COOLSCROLL_H
#define COOLSCROLL_H

#include "coolscroll_global.h"

#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditor.h>

#include <QSharedPointer>

class CoolScrollbarSettings;

namespace CoolScroll {
namespace Internal {

class CoolScrollPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    CoolScrollPlugin();
    ~CoolScrollPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:

    QSharedPointer<CoolScrollbarSettings> m_settings;

private slots:
    void triggerAction();
    void on_editorCreated(Core::IEditor *editor, const QString &fileName);
};

} // namespace Internal
} // namespace CoolScroll

#endif // COOLSCROLL_H

