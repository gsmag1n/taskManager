#pragma once

#include <QWidget>
#include <QString>

class QPropertyAnimation;
class QGraphicsDropShadowEffect;

namespace cookie {

/// Visual model for one row in the upgrade panel.
/// `kind=Producer` shows a count and lets you buy repeatedly.
/// `kind=ClickUpgrade` is one-shot (shows "Owned" once purchased).
class UpgradeCard : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double flash READ flash WRITE setFlash)
public:
    enum class Kind { Producer, ClickUpgrade };

    UpgradeCard(Kind kind,
                QString id,
                QString glyph,
                QString name,
                QString description,
                QWidget* parent = nullptr);

    QString id() const          { return m_id; }
    Kind    kind() const        { return m_kind; }

    /// `cost` ignored when `owned` is true (for click upgrades).
    void setStatus(double cost, int count, bool affordable, bool ownedOneShot);

    double flash() const        { return m_flash; }
    void   setFlash(double v);

    /// Animate a brief golden flash to confirm a purchase.
    void playPurchaseFlash();

signals:
    void buyRequested();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    Kind    m_kind;
    QString m_id;
    QString m_glyph;
    QString m_name;
    QString m_description;

    double  m_cost          = 0.0;
    int     m_count         = 0;
    bool    m_affordable    = false;
    bool    m_ownedOneShot  = false;
    bool    m_hover         = false;
    bool    m_pressed       = false;

    double  m_flash         = 0.0;
    QGraphicsDropShadowEffect* m_shadow = nullptr;
};

} // namespace cookie
