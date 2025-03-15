#include "ioslauncheffect_config.h"

// KDE
#include <KConfigGroup>
#include <KPluginFactory>
#include <KLocalizedString>

// Qt
#include <QDebug>

K_PLUGIN_FACTORY_WITH_JSON(IOSLaunchEffectConfigFactory,
                           "ioslauncheffect_config.json",
                           registerPlugin<KWin::IOSLaunchEffectConfig>();)

namespace KWin
{

IOSLaunchEffectConfig::IOSLaunchEffectConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    ui.setupUi(this);

    addConfig(KSharedConfig::openConfig("kwinrc")->group("Effect-IOSLaunch"), &ui);

    connect(ui.durationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &IOSLaunchEffectConfig::markAsChanged);
    connect(ui.enabledCheckBox, &QCheckBox::stateChanged, this, &IOSLaunchEffectConfig::markAsChanged);
}

IOSLaunchEffectConfig::~IOSLaunchEffectConfig()
{
}

void IOSLaunchEffectConfig::save()
{
    KCModule::save();

    // Sync configuration
    KSharedConfig::openConfig("kwinrc")->sync();
    
    // Tell KWin to reload the effect
    QDBusConnection dbus = QDBusConnection::sessionBus();
    QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
    dbus.send(message);
}

void IOSLaunchEffectConfig::defaults()
{
    KCModule::defaults();
}

} // namespace KWin

#include "ioslauncheffect_config.moc"