#pragma once

#include <QWidget>

class QEasingCurve;
class QEvent;
class QMouseEvent;
class QPaintEvent;
class QVariantAnimation;

class CookieButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal pressScale READ pressScale WRITE setPressScale)
    Q_PROPERTY(qreal glow READ glow WRITE setGlow)

public:
    explicit CookieButton(QWidget *parent = nullptr);

    qreal pressScale() const;
    void setPressScale(qreal scale);

    qreal glow() const;
    void setGlow(qreal glow);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void animateProperty(const char *propertyName,
                         qreal start,
                         qreal end,
                         int duration,
                         const QEasingCurve &curve);

    qreal m_pressScale = 1.0;
    qreal m_glow = 0.0;
    qreal m_pulse = 0.0;
    bool m_pressed = false;
    QVariantAnimation *m_pulseAnimation = nullptr;
};
