#pragma once

#include <QString>

class QPushButton;
class QWidget;

namespace AnimationEffects
{
void showFloatingText(QWidget *parent, QWidget *anchor, const QString &text);
void pulseButton(QPushButton *button);
}
