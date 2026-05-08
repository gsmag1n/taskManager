#include "Toast.h"

#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>

namespace cookie {

namespace {
constexpr int kDisplayMs    = 2400;
constexpr int kFadeInMs     = 220;
constexpr int kFadeOutMs    = 360;
constexpr int kPaddingX     = 16;
constexpr int kPaddingY     = 12;
constexpr int kGlyphTextGap = 12;
constexpr int kCornerRadius = 14;
} // namespace

Toast::Toast(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::NoFocus);
    hide();

    m_fx = new QGraphicsOpacityEffect(this);
    m_fx->setOpacity(0.0);
    setGraphicsEffect(m_fx);

    m_holdTimer = new QTimer(this);
    m_holdTimer->setSingleShot(true);
    connect(m_holdTimer, &QTimer::timeout, this, &Toast::hideAndAdvance);

    if (parent) parent->installEventFilter(this);
}

bool Toast::eventFilter(QObject* obj, QEvent* ev) {
    if (obj == parent() && ev->type() == QEvent::Resize) reanchor();
    return QWidget::eventFilter(obj, ev);
}

void Toast::enqueue(const QString& glyph, const QString& title, const QString& subtitle) {
    m_queue.enqueue({glyph, title, subtitle});
    if (!isVisible() && !m_holdTimer->isActive()) showNext();
}

void Toast::showNext() {
    if (m_queue.isEmpty()) return;
    const Message msg = m_queue.dequeue();
    m_glyph    = msg.glyph;
    m_title    = msg.title;
    m_subtitle = msg.subtitle;

    QFont titleF; titleF.setBold(true); titleF.setPointSize(11);
    QFont subF;   subF.setPointSize(10);
    QFont glyphF; glyphF.setPointSize(20);

    QFontMetrics tfm(titleF), sfm(subF), gfm(glyphF);
    const int glyphW = gfm.horizontalAdvance(m_glyph);
    const int textW  = std::max(tfm.horizontalAdvance(m_title),
                                sfm.horizontalAdvance(m_subtitle));
    const int textH  = tfm.height() + 2 + sfm.height();

    const int w = kPaddingX + glyphW + kGlyphTextGap + textW + kPaddingX;
    const int h = kPaddingY + std::max<int>(gfm.height(), textH) + kPaddingY;
    resize(w, h);
    reanchor();
    show();
    raise();

    auto* fadeIn = new QPropertyAnimation(m_fx, "opacity", this);
    fadeIn->setDuration(kFadeInMs);
    fadeIn->setStartValue(m_fx->opacity());
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutCubic);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    m_holdTimer->start(kDisplayMs);
    update();
}

void Toast::hideAndAdvance() {
    auto* fadeOut = new QPropertyAnimation(m_fx, "opacity", this);
    fadeOut->setDuration(kFadeOutMs);
    fadeOut->setStartValue(m_fx->opacity());
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InCubic);
    connect(fadeOut, &QPropertyAnimation::finished, this, [this]{
        hide();
        if (!m_queue.isEmpty()) showNext();
    });
    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void Toast::reanchor() {
    auto* p = parentWidget();
    if (!p) return;
    const int x = (p->width() - width()) / 2;
    const int y = 18;
    move(std::max(8, x), y);
}

void Toast::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    QPainterPath rounded;
    rounded.addRoundedRect(rect().adjusted(0, 0, -1, -1), kCornerRadius, kCornerRadius);

    p.fillPath(rounded, QColor(0x33, 0x1F, 0x10, 235));
    p.setPen(QPen(QColor(0xFF, 0xC8, 0x6B, 200), 1.5));
    p.setBrush(Qt::NoBrush);
    p.drawPath(rounded);

    QFont glyphF; glyphF.setPointSize(20);
    QFont titleF; titleF.setBold(true); titleF.setPointSize(11);
    QFont subF;   subF.setPointSize(10);

    QFontMetrics gfm(glyphF), tfm(titleF), sfm(subF);

    const int gx = kPaddingX;
    const int gy = (height() - gfm.height()) / 2 + gfm.ascent();

    p.setFont(glyphF);
    p.setPen(QColor(0xFF, 0xE6, 0xA8));
    p.drawText(QPoint(gx, gy), m_glyph);

    const int textX = kPaddingX + gfm.horizontalAdvance(m_glyph) + kGlyphTextGap;
    int textY = (height() - (tfm.height() + 2 + sfm.height())) / 2 + tfm.ascent();

    p.setFont(titleF);
    p.setPen(QColor(0xFF, 0xE6, 0xA8));
    p.drawText(QPoint(textX, textY), m_title);

    textY += 2 + sfm.height();
    p.setFont(subF);
    p.setPen(QColor(0xE0, 0xC4, 0x9A));
    p.drawText(QPoint(textX, textY - sfm.descent()), m_subtitle);
}

} // namespace cookie
