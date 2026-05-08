#include "AnimationEffects.h"

#include <QGraphicsColorizeEffect>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QWidget>

namespace AnimationEffects
{
void showFloatingText(QWidget *parent, QWidget *anchor, const QString &text)
{
    if (!parent || !anchor) {
        return;
    }

    auto *label = new QLabel(text, parent);
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    label->setStyleSheet("color: #fff7c7; font-size: 26px; font-weight: 900; background: transparent;");
    label->adjustSize();

    const QPoint anchorCenter = anchor->mapTo(parent, anchor->rect().center());
    const int xOffset = QRandomGenerator::global()->bounded(-42, 43);
    const QPoint start(anchorCenter.x() + xOffset - label->width() / 2,
                       anchorCenter.y() - label->height() / 2);
    const QPoint end(start.x(), start.y() - 86);
    label->move(start);
    label->show();
    label->raise();

    auto *group = new QParallelAnimationGroup(label);
    auto *move = new QPropertyAnimation(label, "pos", group);
    move->setStartValue(start);
    move->setEndValue(end);
    move->setDuration(850);
    move->setEasingCurve(QEasingCurve::OutCubic);

    auto *fade = new QPropertyAnimation(label, "windowOpacity", group);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    fade->setDuration(850);
    fade->setEasingCurve(QEasingCurve::InQuad);

    group->addAnimation(move);
    group->addAnimation(fade);
    QObject::connect(group, &QParallelAnimationGroup::finished, label, &QLabel::deleteLater);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void pulseButton(QPushButton *button)
{
    if (!button) {
        return;
    }

    auto *effect = new QGraphicsColorizeEffect(button);
    effect->setColor(QColor("#ffef9b"));
    button->setGraphicsEffect(effect);

    auto *animation = new QPropertyAnimation(effect, "strength", effect);
    animation->setStartValue(0.0);
    animation->setKeyValueAt(0.45, 0.9);
    animation->setEndValue(0.0);
    animation->setDuration(420);
    animation->setEasingCurve(QEasingCurve::OutQuad);
    QObject::connect(animation, &QPropertyAnimation::finished, effect, &QGraphicsColorizeEffect::deleteLater);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
}
