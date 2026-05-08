#pragma once

#include <QWidget>
#include <QString>
#include <QQueue>

class QTimer;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

namespace cookie {

/// A small, transient notification widget anchored near the top of its parent.
/// Use `enqueue` to display a series of messages back-to-back.
class Toast : public QWidget {
    Q_OBJECT
public:
    explicit Toast(QWidget* parent);

    /// Queue a notification. The first call shows immediately, subsequent calls
    /// are shown one after another.
    void enqueue(const QString& glyph, const QString& title, const QString& subtitle);

    /// Re-anchor against the parent (call this on parent resize events).
    void reanchor();

protected:
    void paintEvent(QPaintEvent*) override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

private slots:
    void hideAndAdvance();

private:
    struct Message { QString glyph, title, subtitle; };
    void showNext();

    QQueue<Message>          m_queue;
    QString                  m_glyph, m_title, m_subtitle;
    QTimer*                  m_holdTimer = nullptr;
    QGraphicsOpacityEffect*  m_fx        = nullptr;
};

} // namespace cookie
