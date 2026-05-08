#include "FloatingText.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QEasingCurve>

#include <random>

namespace cookie {

namespace {
constexpr int kDurationMs = 900;
constexpr int kRise       = 70;     // pixels the text rises over its life
constexpr int kJitter     = 18;     // horizontal random offset

QFont labelFont() {
    QFont f;
    f.setPointSize(18);
    f.setBold(true);
    return f;
}
} // namespace

FloatingText::FloatingText(QWidget* parent, const QString& text, const QPoint& origin)
    : QWidget(parent), m_text(text), m_origin(origin) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::NoFocus);

    QFontMetrics fm(labelFont());
    const QSize sz = fm.size(0, m_text) + QSize(20, 14);
    resize(sz);

    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dx(-kJitter, kJitter);
    m_origin += QPoint(dx(rng), 0);

    move(m_origin.x() - sz.width() / 2,
         m_origin.y() - sz.height() / 2);
}

FloatingText* FloatingText::spawn(QWidget* container,
                                  const QPoint& originInContainer,
                                  const QString& text) {
    auto* ft = new FloatingText(container, text, originInContainer);
    ft->show();
    ft->raise();

    auto* anim = new QPropertyAnimation(ft, "progress", ft);
    anim->setDuration(kDurationMs);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    QObject::connect(anim, &QPropertyAnimation::finished, ft, &QObject::deleteLater);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    return ft;
}

void FloatingText::setProgress(double p) {
    m_progress = p;
    move(m_origin.x() - width() / 2,
         m_origin.y() - height() / 2 - static_cast<int>(kRise * p));
    update();
}

void FloatingText::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setFont(labelFont());

    const double alpha = std::max(0.0, 1.0 - m_progress);
    QColor fill(0xFF, 0xE6, 0xA8);
    QColor outline(0x4A, 0x2A, 0x10);
    fill.setAlphaF(alpha);
    outline.setAlphaF(alpha);

    QPainterPath path;
    path.addText(QPointF(10, height() - 10), labelFont(), m_text);
    p.setPen(QPen(outline, 3));
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);
    p.setPen(Qt::NoPen);
    p.setBrush(fill);
    p.drawPath(path);
}

} // namespace cookie
