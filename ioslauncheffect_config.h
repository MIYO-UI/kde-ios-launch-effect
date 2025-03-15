#ifndef KWIN_IOSLAUNCHEFFECT_CONFIG_H
#define KWIN_IOSLAUNCHEFFECT_CONFIG_H

#include <KCModule>
#include <KSharedConfig>

#include "ui_ioslauncheffect_config.h"

namespace KWin
{

class IOSLaunchEffectConfig : public KCModule
{
    Q_OBJECT

public:
    explicit IOSLaunchEffectConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    ~IOSLaunchEffectConfig() override;

    void save() override;
    void defaults() override;

private:
    Ui::IOSLaunchEffectConfig ui;
};

} // namespace KWin

#endif // KWIN_IOSLAUNCHEFFECT_CONFIG_H