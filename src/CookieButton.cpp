#include "CookieButton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QVariantAnimation>

#include <array>
#include <cmath>

namespace
{
constexpr qreal Pi = 3.14159265358979323846;
}

CookieButton::CookieButton(QWidget *parent)
    : QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
    setMinimumSize(260, 260);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_pulseAnimation = new QVariantAnimation(this);
    m_pulseAnimation->setStartValue(0.0);
    m_pulseAnimation->setEndValue(1.0);
    m_pulseAnimation->setDuration(2200);
    m_pulseAnimation->setLoopCount(-1);
    m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
    connect(m_pulseAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        m_pulse = value.toReal();
        update();
    });
    m_pulseAnimation->start();
}

qreal CookieButton::pressScale() const
{
    return m_pressScale;
}

void CookieButton::setPressScale(qreal scale)
{
    m_pressScale = scale;
    update();
}

qreal CookieButton::glow() const
{
    return m_glow;
}

void CookieButton::setGlow(qreal glow)
{
    m_glow = glow;
    update();
}

void CookieButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    m_pressed = true;
    animateProperty("pressScale", m_pressScale, 0.92, 90, QEasingCurve::OutQuad);
    animateProperty("glow", 0.0, 1.0, 140, QEasingCurve::OutCubic);
    event->accept();
}

void CookieButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_pressed) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    m_pressed = false;
    animateProperty("pressScale", m_pressScale, 1.0, 240, QEasingCurve::OutBack);
    animateProperty("glow", m_glow, 0.0, 360, QEasingCurve::OutQuad);

    if (rect().contains(event->pos())) {
        emit clicked();
    }

    event->accept();
}

void CookieButton::leaveEvent(QEvent *event)
{
    if (m_pressed) {
        m_pressed = false;
        animateProperty("pressScale", m_pressScale, 1.0, 180, QEasingCurve::OutBack);
        animateProperty("glow", m_glow, 0.0, 220, QEasingCurve::OutQuad);
    }
    QWidget::leaveEvent(event);
}

void CookieButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    const QPointF center(width() / 2.0, height() / 2.0);
    const qreal radius = side * 0.39 * m_pressScale;
    const qreal pulseGlow = (std::sin(m_pulse * 2.0 * Pi) + 1.0) * 0.5;

    painter.save();
    painter.translate(center);

    QRadialGradient halo(QPointF(0, 0), radius * (1.35 + 0.12 * pulseGlow));
    halo.setColorAt(0.0, QColor(255, 220, 120, 90 + static_cast<int>(70 * m_glow)));
    halo.setColorAt(0.6, QColor(255, 180, 80, 35 + static_cast<int>(80 * pulseGlow)));
    halo.setColorAt(1.0, QColor(255, 180, 80, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(halo);
    painter.drawEllipse(QPointF(0, 0), radius * 1.45, radius * 1.45);

    QRadialGradient dough(QPointF(-radius * 0.28, -radius * 0.32), radius * 1.35);
    dough.setColorAt(0.0, QColor(255, 222, 153));
    dough.setColorAt(0.55, QColor(212, 139, 58));
    dough.setColorAt(1.0, QColor(125, 70, 28));
    painter.setBrush(dough);
    painter.setPen(QPen(QColor(94, 51, 22), side * 0.012));
    painter.drawEllipse(QPointF(0, 0), radius, radius);

    painter.setPen(Qt::NoPen);
    const std::array<QPointF, 12> chips = {
        QPointF{-0.46, -0.18},
        QPointF{-0.30, 0.36},
        QPointF{-0.12, -0.48},
        QPointF{0.06, 0.18},
        QPointF{0.24, -0.22},
        QPointF{0.38, 0.34},
        QPointF{0.50, -0.02},
        QPointF{-0.02, 0.52},
        QPointF{-0.55, 0.12},
        QPointF{0.18, -0.56},
        QPointF{-0.22, -0.02},
        QPointF{0.44, -0.42},
    };

    for (std::size_t i = 0; i < chips.size(); ++i) {
        const QPointF chip = chips.at(i) * radius;
        const qreal chipRadius = radius * (0.075 + 0.018 * (i % 3));
        painter.setBrush(QColor(73, 36, 18));
        painter.drawEllipse(chip, chipRadius, chipRadius * 0.78);
        painter.setBrush(QColor(45, 21, 10, 110));
        painter.drawEllipse(chip + QPointF(chipRadius * 0.18, chipRadius * 0.2),
                            chipRadius * 0.52, chipRadius * 0.4);
    }

    painter.setPen(QPen(QColor(255, 241, 205, 160), side * 0.011, Qt::SolidLine, Qt::RoundCap));
    painter.drawArc(QRectF(-radius * 0.64, -radius * 0.68, radius * 1.04, radius * 0.92),
                    28 * 16, 74 * 16);
    painter.restore();
}

void CookieButton::animateProperty(const char *propertyName,
                                   qreal start,
                                   qreal end,
                                   int duration,
                                   const QEasingCurve &curve)
{
    auto *animation = new QPropertyAnimation(this, propertyName, this);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setDuration(duration);
    animation->setEasingCurve(curve);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
