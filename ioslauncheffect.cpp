#include "ioslauncheffect.h"

// KDE
#include <KConfigGroup>
#include <KSharedConfig>

// Qt
#include <QTimer>
#include <QtMath>

namespace KWin
{

IOSLaunchEffect::IOSLaunchEffect()
    : m_duration(300)
    , m_enabled(true)
{
    reconfigure(ReconfigureAll);
    
    // Connect to window management signals
    connect(effects, &EffectsHandler::windowAdded, this, &IOSLaunchEffect::slotWindowAdded);
    connect(effects, &EffectsHandler::windowClosed, this, &IOSLaunchEffect::slotWindowClosed);
    connect(effects, &EffectsHandler::windowDeleted, this, &IOSLaunchEffect::slotWindowDeleted);
    
    // Connect to mouse signals to track the last click position
    connect(effects, &EffectsHandler::mouseChanged, this, &IOSLaunchEffect::slotMouseChanged);
}

IOSLaunchEffect::~IOSLaunchEffect()
{
    // Clean up any running animations
    for (auto it = m_animations.begin(); it != m_animations.end(); ++it) {
        delete it.value().animation;
    }
}

void IOSLaunchEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)

    KSharedConfig::Ptr conf = KSharedConfig::openConfig(QStringLiteral("kwinrc"));
    KConfigGroup iosGroup(conf, QStringLiteral("Effect-IOSLaunch"));
    
    m_duration = iosGroup.readEntry("Duration", 300);
    m_enabled = iosGroup.readEntry("Enabled", true);
}

void IOSLaunchEffect::prePaintScreen(ScreenPrePaintData &data, std::chrono::milliseconds presentTime)
{
    // Make sure we keep getting repaints as long as animations are running
    if (isActive()) {
        data.mask |= PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS;
    }
    
    effects->prePaintScreen(data, presentTime);
}

void IOSLaunchEffect::prePaintWindow(EffectWindow *w, WindowPrePaintData &data, std::chrono::milliseconds presentTime)
{
    // Check if this window is being animated
    if (m_animations.contains(w)) {
        // Mark the window as being transformed
        data.setTransformed();
    }
    
    effects->prePaintWindow(w, data, presentTime);
}

void IOSLaunchEffect::paintWindow(EffectWindow *w, int mask, QRegion region, WindowPaintData &data)
{
    // If this window is being animated, apply the transformations
    if (m_animations.contains(w)) {
        const Data &animData = m_animations[w];
        
        if (!animData.closing) {
            // This is an opening animation (window appears from click point)
            
            // Get the progress of the animation (0.0 to 1.0)
            const float t = animData.progress;
            
            // Calculate the scale based on progress (start small, end at normal size)
            const float scale = 0.1f + 0.9f * t;
            
            // Calculate the opacity (fade in)
            const float opacity = t;
            
            // Compute the center point for the window 
            QRectF windowRect = w->geometry();
            QPointF windowCenter = windowRect.center();
            
            // Calculate the starting center (click position)
            QPointF startCenter = animData.origin;
            
            // Interpolate between start and end centers
            QPointF currentCenter = startCenter * (1-t) + windowCenter * t;
            
            // Calculate the translation needed to position the window correctly
            QPointF translation = currentCenter - windowCenter;
            
            // Apply transformations
            data.setScale(QVector3D(scale, scale, 1.0));
            data.translate(translation.x(), translation.y());
            data.multiplyOpacity(opacity);
        } else {
            // This is a closing animation (window shrinks to a point)
            
            // For closing, progress goes from 1.0 to 0.0
            const float t = 1.0f - animData.progress;
            
            // Calculate scale and opacity for the closing animation
            const float scale = 0.1f + 0.9f * (1.0f - t);
            const float opacity = 1.0f - t;
            
            // Compute center points
            QRectF windowRect = w->geometry();
            QPointF windowCenter = windowRect.center();
            QPointF endCenter = animData.origin;
            
            // Interpolate between window center and end center (usually the close button)
            QPointF currentCenter = windowCenter * (1-t) + endCenter * t;
            
            // Calculate the translation needed
            QPointF translation = currentCenter - windowCenter;
            
            // Apply transformations
            data.setScale(QVector3D(scale, scale, 1.0));
            data.translate(translation.x(), translation.y());
            data.multiplyOpacity(opacity);
        }
    }
    
    // Let KWin paint the window with our transformations
    effects->paintWindow(w, mask, region, data);
}

void IOSLaunchEffect::postPaintScreen()
{
    // Check if we need to continue animations
    bool repaint = false;
    
    for (auto it = m_animations.begin(); it != m_animations.end(); ) {
        EffectWindow *w = it.key();
        Data &data = it.value();
        
        if (data.animation->state() == QAbstractAnimation::Running) {
            repaint = true;
            ++it;
        } else {
            // Animation finished
            delete data.animation;
            it = m_animations.erase(it);
            
            // Make sure window repaints to show its final state
            effects->addRepaint(w->geometry());
        }
    }
    
    if (repaint) {
        effects->addRepaintFull();
    }
    
    effects->postPaintScreen();
}

bool IOSLaunchEffect::isActive() const
{
    return !m_animations.isEmpty();
}

int IOSLaunchEffect::requestedEffectChainPosition() const
{
    // Stack order for the effect (higher number = later processing)
    return 50;
}

void IOSLaunchEffect::animateWindow(EffectWindow *w)
{
    if (!m_enabled || m_animations.contains(w))
        return;
    
    startAnimation(w);
}

void IOSLaunchEffect::slotWindowAdded(EffectWindow *w)
{
    if (!m_enabled || m_animations.contains(w))
        return;
    
    // Skip certain window types
    if (w->isSpecialWindow() || w->isUtility() || w->isDesktop() || w->isDock())
        return;
    
    // Wait a bit before starting the animation to ensure the window is fully created
    QTimer::singleShot(50, this, [this, w]() {
        if (w && w->isNormalWindow()) {
            startAnimation(w);
        }
    });
}

void IOSLaunchEffect::slotWindowClosed(EffectWindow *w)
{
    // When a window is closed, we want to animate it shrinking back
    // to the close button position (or window center if we don't know)
    
    if (!m_enabled || m_animations.contains(w))
        return;
    
    // Skip certain window types
    if (w->isSpecialWindow() || w->isUtility() || w->isDesktop() || w->isDock())
        return;
    
    // Start with a point near the top-right of the window (close button area)
    QRectF geo = w->geometry();
    QPointF closePos = QPointF(geo.right() - 20, geo.top() + 20);
    
    // Create animation data
    Data animData;
    animData.closing = true;
    animData.progress = 1.0; // Start from fully visible
    animData.origin = closePos;
    animData.animation = new QVariantAnimation(this);
    animData.animation->setDuration(m_duration);
    animData.animation->setStartValue(1.0);
    animData.animation->setEndValue(0.0);
    animData.animation->setEasingCurve(QEasingCurve::InQuad);
    
    // Update progress value when animation advances
    connect(animData.animation, &QVariantAnimation::valueChanged, this, [this, w](const QVariant &value) {
        if (m_animations.contains(w)) {
            m_animations[w].progress = value.toFloat();
            effects->addRepaint(w->geometry());
        }
    });
    
    // Store and start the animation
    m_animations.insert(w, animData);
    animData.animation->start();
    
    // Make sure the window gets rendered with the effect
    effects->addRepaint(w->geometry());
}

void IOSLaunchEffect::slotWindowDeleted(EffectWindow *w)
{
    // Clean up when a window is deleted
    if (m_animations.contains(w)) {
        delete m_animations[w].animation;
        m_animations.remove(w);
    }
}

void IOSLaunchEffect::slotMouseChanged(const QPoint &pos, const QPoint &oldpos, Qt::MouseButtons buttons,
                                    Qt::MouseButtons oldbuttons, Qt::KeyboardModifiers modifiers,
                                    Qt::KeyboardModifiers oldmodifiers)
{
    Q_UNUSED(oldpos)
    Q_UNUSED(modifiers)
    Q_UNUSED(oldmodifiers)
    
    // Track mouse position on button press events
    if (buttons != oldbuttons && (buttons & Qt::LeftButton)) {
        m_lastMousePos = pos;
    }
}

void IOSLaunchEffect::startAnimation(EffectWindow *w)
{
    // Skip windows that are not normal
    if (!w || !w->isNormalWindow())
        return;

    // Create animation data
    Data animData;
    animData.closing = false;
    animData.progress = 0.0; // Start from invisible
    
    // Use the last mouse click position as the animation origin
    // If we don't have a valid position, use the center of the screen
    if (m_lastMousePos.isNull()) {
        QRect screenRect = effects->virtualScreenGeometry();
        animData.origin = screenRect.center();
    } else {
        animData.origin = m_lastMousePos;
    }
    
    // Create and set up the animation object
    animData.animation = new QVariantAnimation(this);
    animData.animation->setDuration(m_duration);
    animData.animation->setStartValue(0.0);
    animData.animation->setEndValue(1.0);
    animData.animation->setEasingCurve(QEasingCurve::OutQuad);
    
    // Update progress value when animation advances
    connect(animData.animation, &QVariantAnimation::valueChanged, this, [this, w](const QVariant &value) {
        if (m_animations.contains(w)) {
            m_animations[w].progress = value.toFloat();
            effects->addRepaint(w->geometry());
        }
    });
    
    // Store and start the animation
    m_animations.insert(w, animData);
    animData.animation->start();
    
    // Make sure the window gets rendered with the effect
    effects->addRepaint(w->geometry());
}

void IOSLaunchEffect::stopAnimation(EffectWindow *w)
{
    if (!m_animations.contains(w))
        return;
    
    // Stop the animation
    delete m_animations[w].animation;
    m_animations.remove(w);
    
    // Make sure the window gets redrawn
    effects->addRepaint(w->geometry());
}

} // namespace KWin