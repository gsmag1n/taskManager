#pragma once

#include <QWidget>
#include <QElapsedTimer>
#include <QPointF>
#include <QVector>

class QTimer;

namespace cookie {

/// A big animated, clickable cookie. Emits `clicked()` on press.
/// Drawn entirely with QPainter — no image assets required.
class CookieButton : public QWidget {
    Q_OBJECT
public:
    explicit CookieButton(QWidget* parent = nullptr);

    QSize sizeHint() const override     { return {360, 360}; }
    QSize minimumSizeHint() const override { return {220, 220}; }

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private slots:
    void onTick();

private:
    struct Particle {
        QPointF pos;
        QPointF vel;
        double  life;     // remaining seconds
        double  lifeMax;
        double  rotation;
        double  rotSpeed;
        double  size;
    };
    struct Shockwave {
        double age;       // seconds since spawn
        double maxAge;
    };

    void startClickPulse();
    void spawnParticles(int count);
    void spawnShockwave();

    QTimer*        m_timer        = nullptr;
    QElapsedTimer  m_clock;
    qint64         m_lastTickMs   = 0;
    qint64         m_pressedAtMs  = -1;
    qint64         m_clickAtMs    = -1;
    bool           m_hover        = false;
    bool           m_pressed      = false;

    QVector<Particle>  m_particles;
    QVector<Shockwave> m_shockwaves;

    double m_pulse = 0.0;     // current click-pulse strength (0..1, decays)
};

} // namespace cookie
