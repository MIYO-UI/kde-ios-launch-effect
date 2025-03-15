#include "ioslauncheffect.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(IOSLaunchEffectFactory,
                           "ioslauncheffect.json",
                           registerPlugin<KWin::IOSLaunchEffect>();)

#include "main.moc"