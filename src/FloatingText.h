#pragma once

#include <QWidget>
#include <QString>

class QPropertyAnimation;

namespace cookie {

/// A short-lived widget that floats upward and fades out.
/// Used to show "+N" feedback when the cookie is clicked.
class FloatingText : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double progress READ progress WRITE setProgress)
public:
    /// Spawn a floating label inside `container` near `originInContainer`.
    /// The widget deletes itself when the animation finishes.
    static FloatingText* spawn(QWidget* container,
                               const QPoint& originInContainer,
                               const QString& text);

    double progress() const { return m_progress; }
    void   setProgress(double p);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    explicit FloatingText(QWidget* parent, const QString& text, const QPoint& origin);

    QString m_text;
    QPoint  m_origin;
    double  m_progress = 0.0;       // 0 .. 1
};

} // namespace cookie
