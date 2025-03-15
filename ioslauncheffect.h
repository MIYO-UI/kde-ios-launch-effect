#ifndef KWIN_IOSLAUNCHEFFECT_H
#define KWIN_IOSLAUNCHEFFECT_H

#include <kwineffects.h>
#include <QHash>
#include <QVariantAnimation>
#include <QPoint>

namespace KWin
{

class IOSLaunchEffect : public Effect
{
    Q_OBJECT

public:
    IOSLaunchEffect();
    ~IOSLaunchEffect() override;

    // Effect interface
    void reconfigure(ReconfigureFlags flags) override;
    void prePaintScreen(ScreenPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void prePaintWindow(EffectWindow *w, WindowPrePaintData &data, std::chrono::milliseconds presentTime) override;
    void paintWindow(EffectWindow *w, int mask, QRegion region, WindowPaintData &data) override;
    void postPaintScreen() override;
    bool isActive() const override;
    int requestedEffectChainPosition() const override;
    
    // For testing
    void animateWindow(EffectWindow *w);

public Q_SLOTS:
    void slotWindowAdded(EffectWindow *w);
    void slotWindowClosed(EffectWindow *w);
    void slotWindowDeleted(EffectWindow *w);
    
    // This is where we hook into the mouse click
    void slotMouseChanged(const QPoint &pos, const QPoint &oldpos, Qt::MouseButtons buttons,
                         Qt::MouseButtons oldbuttons, Qt::KeyboardModifiers modifiers,
                         Qt::KeyboardModifiers oldmodifiers);

private:
    void startAnimation(EffectWindow *w);
    void stopAnimation(EffectWindow *w);
    
    struct Data {
        QVariantAnimation *animation;
        float progress;
        QPointF origin;
        bool closing;
    };
    
    QHash<EffectWindow*, Data> m_animations;
    QPoint m_lastMousePos;
    
    // Configuration
    int m_duration;
    bool m_enabled;
};

} // namespace KWin

#endif // KWIN_IOSLAUNCHEFFECT_H