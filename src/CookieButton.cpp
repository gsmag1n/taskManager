#include "CookieButton.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QTimer>
#include <QtMath>

#include <algorithm>
#include <random>

namespace cookie {

namespace {
constexpr int    kFps             = 60;
constexpr double kIdlePeriodSec   = 2.6;
constexpr double kIdleAmplitude   = 0.018;
constexpr double kPulseDecaySec   = 0.45;   // how fast the click pulse fades
constexpr double kHoverScale      = 1.04;

double easeOutCubic(double t) {
    t = std::clamp(t, 0.0, 1.0);
    const double inv = 1.0 - t;
    return 1.0 - inv * inv * inv;
}
} // namespace

CookieButton::CookieButton(QWidget* parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMinimumSize(minimumSizeHint());

    m_clock.start();
    m_timer = new QTimer(this);
    m_timer->setInterval(1000 / kFps);
    connect(m_timer, &QTimer::timeout, this, &CookieButton::onTick);
    m_timer->start();
}

void CookieButton::onTick() {
    const qint64 nowMs = m_clock.elapsed();
    const double dt = (m_lastTickMs == 0) ? 1.0 / kFps
                                          : (nowMs - m_lastTickMs) / 1000.0;
    m_lastTickMs = nowMs;

    // Decay click pulse.
    if (m_pulse > 0.0) {
        m_pulse = std::max(0.0, m_pulse - dt / kPulseDecaySec);
    }

    // Update particles.
    for (auto& p : m_particles) {
        p.vel.ry() += 600.0 * dt;          // gravity (px/s^2)
        p.pos += p.vel * dt;
        p.rotation += p.rotSpeed * dt;
        p.life -= dt;
    }
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                       [](const Particle& p) { return p.life <= 0.0; }),
        m_particles.end());

    // Update shockwaves.
    for (auto& s : m_shockwaves) s.age += dt;
    m_shockwaves.erase(
        std::remove_if(m_shockwaves.begin(), m_shockwaves.end(),
                       [](const Shockwave& s) { return s.age >= s.maxAge; }),
        m_shockwaves.end());

    update();
}

void CookieButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        m_pressed = true;
        m_pressedAtMs = m_clock.elapsed();
        startClickPulse();
        emit clicked();
    }
    QWidget::mousePressEvent(e);
}

void CookieButton::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) m_pressed = false;
    QWidget::mouseReleaseEvent(e);
}

void CookieButton::enterEvent(QEnterEvent* e)  { m_hover = true;  update(); QWidget::enterEvent(e); }
void CookieButton::leaveEvent(QEvent* e)       { m_hover = false; update(); QWidget::leaveEvent(e); }

void CookieButton::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Space || e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        startClickPulse();
        emit clicked();
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}

void CookieButton::startClickPulse() {
    m_clickAtMs = m_clock.elapsed();
    m_pulse = 1.0;
    spawnShockwave();
    spawnParticles(8);
}

void CookieButton::spawnShockwave() {
    m_shockwaves.push_back({0.0, 0.55});
}

void CookieButton::spawnParticles(int count) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> ang(0.0, 2 * M_PI);
    std::uniform_real_distribution<double> speed(160.0, 280.0);
    std::uniform_real_distribution<double> life(0.55, 0.9);
    std::uniform_real_distribution<double> sz(3.5, 6.5);
    std::uniform_real_distribution<double> rot(-6.0, 6.0);

    const QPointF center(width() * 0.5, height() * 0.5);
    const double  r = std::min(width(), height()) * 0.36;

    for (int i = 0; i < count; ++i) {
        const double a = ang(rng);
        const double s = speed(rng);
        Particle p;
        p.pos = center + QPointF(std::cos(a) * r, std::sin(a) * r);
        p.vel = QPointF(std::cos(a) * s, std::sin(a) * s - 60.0);
        p.lifeMax = p.life = life(rng);
        p.size = sz(rng);
        p.rotation = ang(rng);
        p.rotSpeed = rot(rng);
        m_particles.push_back(p);
    }
}

// -----------------------------------------------------------------------------
// Painting
// -----------------------------------------------------------------------------
namespace {

// Paint a single cookie with chocolate chips into a rect (assumed square-ish).
void paintCookie(QPainter& p, const QRectF& rect) {
    const QPointF c = rect.center();
    const double  r = std::min(rect.width(), rect.height()) * 0.5;

    // Soft warm shadow under the cookie.
    {
        QRadialGradient g(c + QPointF(0, r * 0.18), r * 1.05, c + QPointF(0, r * 0.18));
        g.setColorAt(0.0, QColor(0, 0, 0, 130));
        g.setColorAt(0.7, QColor(0, 0, 0, 60));
        g.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.setBrush(g);
        p.setPen(Qt::NoPen);
        p.drawEllipse(c + QPointF(0, r * 0.10), r * 1.04, r * 0.30);
    }

    // Cookie body — radial gradient (highlight top-left, dark bottom-right).
    {
        QRadialGradient g(c + QPointF(-r * 0.30, -r * 0.30), r * 1.4, c + QPointF(-r * 0.30, -r * 0.30));
        g.setColorAt(0.00, QColor(0xE9, 0xB1, 0x6A));
        g.setColorAt(0.45, QColor(0xC6, 0x86, 0x44));
        g.setColorAt(0.80, QColor(0x8E, 0x55, 0x2A));
        g.setColorAt(1.00, QColor(0x60, 0x37, 0x1A));
        p.setBrush(g);
        p.setPen(QPen(QColor(0x4A, 0x2A, 0x12), std::max(1.5, r * 0.02)));
        p.drawEllipse(c, r, r);
    }

    // A few baked speckles / texture dots.
    {
        // Use a deterministic "noise" so the cookie looks consistent each frame.
        static const struct { double a, d, s; QColor color; } speckles[] = {
            {0.40,  0.20, 1.2, QColor(0xA0, 0x5E, 0x2D)},
            {1.10,  0.45, 1.6, QColor(0x86, 0x4A, 0x22)},
            {2.05,  0.65, 1.0, QColor(0xB6, 0x70, 0x38)},
            {2.85,  0.30, 1.4, QColor(0x70, 0x3D, 0x18)},
            {3.55,  0.55, 1.1, QColor(0x9C, 0x5A, 0x2A)},
            {4.30,  0.75, 1.3, QColor(0x5E, 0x32, 0x14)},
            {4.95,  0.25, 1.0, QColor(0xA8, 0x66, 0x32)},
            {5.55,  0.50, 1.5, QColor(0x88, 0x4C, 0x24)},
            {6.05,  0.70, 1.2, QColor(0x6C, 0x3A, 0x18)},
        };
        p.setPen(Qt::NoPen);
        for (const auto& s : speckles) {
            const QPointF pt = c + QPointF(std::cos(s.a) * r * s.d, std::sin(s.a) * r * s.d);
            p.setBrush(s.color);
            p.drawEllipse(pt, s.s, s.s);
        }
    }

    // Chocolate chips: positioned in a fixed pleasant pattern.
    struct Chip { double angle; double dist; double size; };
    static const Chip chips[] = {
        {-1.20, 0.10, 0.16},
        { 0.30, 0.30, 0.14},
        { 1.55, 0.55, 0.13},
        { 2.70, 0.30, 0.12},
        { 3.65, 0.60, 0.15},
        { 4.65, 0.20, 0.14},
        { 5.55, 0.65, 0.12},
        { 0.95, 0.70, 0.11},
    };
    for (const auto& chip : chips) {
        const QPointF center =
            c + QPointF(std::cos(chip.angle) * r * chip.dist,
                        std::sin(chip.angle) * r * chip.dist);
        const double cs = r * chip.size;

        // Drop shadow.
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 90));
        p.drawEllipse(center + QPointF(cs * 0.18, cs * 0.22), cs, cs * 0.85);

        // Chip body.
        QRadialGradient cg(center + QPointF(-cs * 0.35, -cs * 0.35), cs * 1.6,
                            center + QPointF(-cs * 0.35, -cs * 0.35));
        cg.setColorAt(0.0, QColor(0x6A, 0x3A, 0x1E));
        cg.setColorAt(0.6, QColor(0x3A, 0x1C, 0x0C));
        cg.setColorAt(1.0, QColor(0x18, 0x0A, 0x04));
        p.setBrush(cg);
        p.drawEllipse(center, cs, cs * 0.92);

        // Glossy highlight.
        QRadialGradient hg(center + QPointF(-cs * 0.30, -cs * 0.40), cs * 0.7,
                            center + QPointF(-cs * 0.30, -cs * 0.40));
        hg.setColorAt(0.0, QColor(255, 255, 255, 170));
        hg.setColorAt(1.0, QColor(255, 255, 255, 0));
        p.setBrush(hg);
        p.drawEllipse(center + QPointF(-cs * 0.20, -cs * 0.30), cs * 0.55, cs * 0.35);
    }

    // Top glossy sheen across the whole cookie.
    {
        QRadialGradient g(c + QPointF(-r * 0.45, -r * 0.55), r * 0.9,
                          c + QPointF(-r * 0.45, -r * 0.55));
        g.setColorAt(0.0, QColor(255, 255, 255, 70));
        g.setColorAt(1.0, QColor(255, 255, 255, 0));
        p.setBrush(g);
        p.setPen(Qt::NoPen);
        p.drawEllipse(c + QPointF(-r * 0.10, -r * 0.20), r * 0.85, r * 0.55);
    }
}

void paintCrumb(QPainter& p, const QPointF& center, double size, double rotation, double alpha) {
    p.save();
    p.translate(center);
    p.rotate(qRadiansToDegrees(rotation));
    QColor body(0x8E, 0x55, 0x2A);
    body.setAlphaF(std::clamp(alpha, 0.0, 1.0));
    QColor chip(0x2A, 0x16, 0x08);
    chip.setAlphaF(std::clamp(alpha, 0.0, 1.0));
    p.setPen(Qt::NoPen);
    p.setBrush(body);
    p.drawRoundedRect(QRectF(-size, -size * 0.7, size * 2, size * 1.4),
                      size * 0.4, size * 0.4);
    p.setBrush(chip);
    p.drawEllipse(QPointF(-size * 0.25, 0.0), size * 0.30, size * 0.30);
    p.drawEllipse(QPointF(size * 0.40, -size * 0.20), size * 0.22, size * 0.22);
    p.restore();
}

} // namespace

void CookieButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    const double t = m_clock.elapsed() / 1000.0;
    const double idle = std::sin(t * 2.0 * M_PI / kIdlePeriodSec) * kIdleAmplitude;
    const double pulseEase = easeOutCubic(m_pulse);
    const double pressed   = m_pressed ? 0.05 : 0.0;
    const double hoverBoost = m_hover ? (kHoverScale - 1.0) : 0.0;
    const double scale = 1.0 + idle + hoverBoost - pulseEase * 0.10 - pressed;

    const QSize sz = size();
    const QPointF center(sz.width() / 2.0, sz.height() / 2.0);

    // Shockwaves (drawn behind the cookie).
    for (const auto& s : m_shockwaves) {
        const double k = s.age / s.maxAge;
        const double radius = std::min(sz.width(), sz.height()) * (0.42 + k * 0.40);
        const double alpha  = std::max(0.0, 1.0 - k);
        QColor c1(0xFF, 0xD7, 0x80);
        c1.setAlphaF(0.55 * alpha);
        QPen pen(c1, 4.0 * (1.0 - k * 0.5));
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(center, radius, radius);
    }

    // Cookie itself.
    p.save();
    p.translate(center);
    p.scale(scale, scale);
    const double cookieR = std::min(sz.width(), sz.height()) * 0.42;
    paintCookie(p, QRectF(-cookieR, -cookieR, cookieR * 2, cookieR * 2));
    p.restore();

    // Particles in front.
    for (const auto& particle : m_particles) {
        const double alpha = std::clamp(particle.life / particle.lifeMax, 0.0, 1.0);
        paintCrumb(p, particle.pos, particle.size, particle.rotation, alpha);
    }

    // Subtle focus ring when keyboard-focused.
    if (hasFocus()) {
        QPen pen(QColor(0xFF, 0xC8, 0x6B, 140), 2, Qt::DashLine);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        const double r = std::min(sz.width(), sz.height()) * 0.46;
        p.drawEllipse(center, r, r);
    }
}

} // namespace cookie
