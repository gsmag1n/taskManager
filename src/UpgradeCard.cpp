#include "UpgradeCard.h"
#include "Format.h"

#include <QFont>
#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

namespace cookie {

namespace {
constexpr int kHeight       = 76;
constexpr int kCornerRadius = 12;
constexpr int kGlyphSize    = 44;
constexpr int kPaddingX     = 12;
}

UpgradeCard::UpgradeCard(Kind kind, QString id, QString glyph,
                          QString name, QString description, QWidget* parent)
    : QWidget(parent),
      m_kind(kind),
      m_id(std::move(id)),
      m_glyph(std::move(glyph)),
      m_name(std::move(name)),
      m_description(std::move(description)) {
    setFixedHeight(kHeight);
    setMinimumWidth(280);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);

    m_shadow = new QGraphicsDropShadowEffect(this);
    m_shadow->setBlurRadius(0);
    m_shadow->setOffset(0, 2);
    m_shadow->setColor(QColor(0, 0, 0, 110));
    setGraphicsEffect(m_shadow);
}

void UpgradeCard::setStatus(double cost, int count, bool affordable, bool ownedOneShot) {
    m_cost          = cost;
    m_count         = count;
    m_affordable    = affordable;
    m_ownedOneShot  = ownedOneShot;
    setCursor((ownedOneShot || !affordable) ? Qt::ArrowCursor : Qt::PointingHandCursor);
    update();
}

void UpgradeCard::setFlash(double v) {
    m_flash = v;
    update();
}

void UpgradeCard::playPurchaseFlash() {
    auto* a = new QPropertyAnimation(this, "flash", this);
    a->setDuration(420);
    a->setStartValue(1.0);
    a->setEndValue(0.0);
    a->setEasingCurve(QEasingCurve::OutCubic);
    a->start(QAbstractAnimation::DeleteWhenStopped);
}

void UpgradeCard::enterEvent(QEnterEvent* e) {
    m_hover = true;
    m_shadow->setBlurRadius(18);
    m_shadow->setOffset(0, 4);
    update();
    QWidget::enterEvent(e);
}

void UpgradeCard::leaveEvent(QEvent* e) {
    m_hover = false;
    m_pressed = false;
    m_shadow->setBlurRadius(0);
    m_shadow->setOffset(0, 2);
    update();
    QWidget::leaveEvent(e);
}

void UpgradeCard::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton && !m_ownedOneShot && m_affordable) {
        m_pressed = true;
        emit buyRequested();
        update();
    }
    QWidget::mousePressEvent(e);
}

void UpgradeCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    const QRectF r = rect().adjusted(1, 1, -1, -1);

    QPainterPath path;
    path.addRoundedRect(r, kCornerRadius, kCornerRadius);

    // Background gradient that depends on state.
    QColor top, bot, border;
    if (m_ownedOneShot) {
        top    = QColor(0x3B, 0x55, 0x2A);
        bot    = QColor(0x29, 0x3C, 0x1E);
        border = QColor(0x86, 0xC4, 0x5A);
    } else if (!m_affordable) {
        top    = QColor(0x29, 0x1E, 0x16);
        bot    = QColor(0x1E, 0x16, 0x10);
        border = QColor(0x55, 0x3A, 0x22);
    } else {
        top    = QColor(0x4B, 0x32, 0x1E);
        bot    = QColor(0x33, 0x21, 0x12);
        border = QColor(0xFF, 0xC8, 0x6B);
    }
    if (m_hover && !m_ownedOneShot && m_affordable) {
        top = top.lighter(115);
        bot = bot.lighter(115);
    }
    if (m_pressed) {
        top = top.darker(115);
        bot = bot.darker(115);
    }

    QLinearGradient g(r.topLeft(), r.bottomLeft());
    g.setColorAt(0, top);
    g.setColorAt(1, bot);
    p.fillPath(path, g);

    // Optional gold flash overlay (purchase confirmation).
    if (m_flash > 0.0) {
        QColor flashC(0xFF, 0xE6, 0xA8);
        flashC.setAlphaF(0.55 * m_flash);
        p.fillPath(path, flashC);
    }

    p.setPen(QPen(border, m_hover ? 2.0 : 1.2));
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    // Glyph circle.
    const QPointF glyphCenter(r.left() + kPaddingX + kGlyphSize / 2.0, r.center().y());
    {
        QRadialGradient rg(glyphCenter - QPointF(8, 8), kGlyphSize, glyphCenter - QPointF(8, 8));
        rg.setColorAt(0.0, QColor(0xE9, 0xB1, 0x6A));
        rg.setColorAt(0.6, QColor(0x8E, 0x55, 0x2A));
        rg.setColorAt(1.0, QColor(0x4A, 0x2A, 0x12));
        p.setBrush(rg);
        p.setPen(QPen(QColor(0x2A, 0x16, 0x08), 1.2));
        p.drawEllipse(glyphCenter, kGlyphSize / 2.0, kGlyphSize / 2.0);
    }
    {
        QFont f;
        f.setPointSize(20);
        p.setFont(f);
        p.setPen(QColor(0xFF, 0xE6, 0xA8));
        QRectF glyphRect(glyphCenter.x() - kGlyphSize / 2.0,
                         glyphCenter.y() - kGlyphSize / 2.0,
                         kGlyphSize, kGlyphSize);
        p.drawText(glyphRect, Qt::AlignCenter, m_glyph);
    }

    // Text block.
    const int textX = static_cast<int>(glyphCenter.x() + kGlyphSize / 2.0 + 12);
    const int textRight = static_cast<int>(r.right() - kPaddingX);

    QFont titleF; titleF.setBold(true); titleF.setPointSize(11);
    QFont subF;   subF.setPointSize(9);
    QFontMetrics tfm(titleF), sfm(subF);

    p.setFont(titleF);
    p.setPen(QColor(0xFF, 0xE6, 0xA8));
    QRectF nameRect(textX, r.top() + 8, textRight - textX, tfm.height());
    p.drawText(nameRect, Qt::AlignLeft | Qt::AlignTop, m_name);

    p.setFont(subF);
    p.setPen(QColor(0xCB, 0xA8, 0x7F));
    QRectF descRect(textX, nameRect.bottom() + 1, textRight - textX, sfm.height() * 2 + 2);
    p.drawText(descRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_description);

    // Right side — cost or "Owned" / count.
    QFont rightTopF; rightTopF.setBold(true); rightTopF.setPointSize(11);
    QFont rightBotF; rightBotF.setPointSize(9);

    QString rightTop;
    QString rightBot;
    QColor  rightTopColor;

    if (m_ownedOneShot) {
        rightTop = "Owned";
        rightTopColor = QColor(0xC8, 0xE8, 0xA0);
        rightBot = "✓";
    } else {
        rightTop = formatNumber(m_cost);
        rightTopColor = m_affordable ? QColor(0xFF, 0xE6, 0xA8) : QColor(0x99, 0x6E, 0x52);
        if (m_kind == Kind::Producer) {
            rightBot = QStringLiteral("owned: %1").arg(m_count);
        } else {
            rightBot = QStringLiteral("one-time");
        }
    }

    QFontMetrics rfm(rightTopF), rbfm(rightBotF);
    const int costW = std::max(rfm.horizontalAdvance(rightTop),
                               rbfm.horizontalAdvance(rightBot));
    QRectF costTopRect(textRight - costW, r.top() + 10, costW, rfm.height());
    QRectF costBotRect(textRight - costW, costTopRect.bottom() + 2, costW, rbfm.height());

    p.setFont(rightTopF);
    p.setPen(rightTopColor);
    p.drawText(costTopRect, Qt::AlignRight | Qt::AlignTop, rightTop);

    p.setFont(rightBotF);
    p.setPen(QColor(0xCB, 0xA8, 0x7F));
    p.drawText(costBotRect, Qt::AlignRight | Qt::AlignTop, rightBot);
}

} // namespace cookie
