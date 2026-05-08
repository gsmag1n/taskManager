#include <QtWidgets>

#include <cmath>
#include <functional>
#include <vector>

class CookieButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal pressScale READ pressScale WRITE setPressScale)
    Q_PROPERTY(qreal glow READ glow WRITE setGlow)

public:
    explicit CookieButton(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setCursor(Qt::PointingHandCursor);
        setMinimumSize(260, 260);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        m_pulseAnimation = new QVariantAnimation(this);
        m_pulseAnimation->setStartValue(0.0);
        m_pulseAnimation->setEndValue(1.0);
        m_pulseAnimation->setDuration(2200);
        m_pulseAnimation->setLoopCount(-1);
        m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
        connect(m_pulseAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            m_pulse = value.toReal();
            update();
        });
        m_pulseAnimation->start();
    }

    qreal pressScale() const { return m_pressScale; }

    void setPressScale(qreal scale)
    {
        m_pressScale = scale;
        update();
    }

    qreal glow() const { return m_glow; }

    void setGlow(qreal glow)
    {
        m_glow = glow;
        update();
    }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton) {
            QWidget::mousePressEvent(event);
            return;
        }

        m_pressed = true;
        animateProperty("pressScale", m_pressScale, 0.92, 90, QEasingCurve::OutQuad);
        animateProperty("glow", 0.0, 1.0, 140, QEasingCurve::OutCubic);
        event->accept();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton || !m_pressed) {
            QWidget::mouseReleaseEvent(event);
            return;
        }

        m_pressed = false;
        animateProperty("pressScale", m_pressScale, 1.0, 240, QEasingCurve::OutBack);
        animateProperty("glow", m_glow, 0.0, 360, QEasingCurve::OutQuad);

        if (rect().contains(event->pos())) {
            emit clicked();
        }

        event->accept();
    }

    void leaveEvent(QEvent *event) override
    {
        if (m_pressed) {
            m_pressed = false;
            animateProperty("pressScale", m_pressScale, 1.0, 180, QEasingCurve::OutBack);
            animateProperty("glow", m_glow, 0.0, 220, QEasingCurve::OutQuad);
        }
        QWidget::leaveEvent(event);
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int side = qMin(width(), height());
        const QPointF center(width() / 2.0, height() / 2.0);
        const qreal radius = side * 0.39 * m_pressScale;
        const qreal pulseGlow = (std::sin(m_pulse * 2.0 * M_PI) + 1.0) * 0.5;

        painter.save();
        painter.translate(center);

        QRadialGradient halo(QPointF(0, 0), radius * (1.35 + 0.12 * pulseGlow));
        halo.setColorAt(0.0, QColor(255, 220, 120, 90 + static_cast<int>(70 * m_glow)));
        halo.setColorAt(0.6, QColor(255, 180, 80, 35 + static_cast<int>(80 * pulseGlow)));
        halo.setColorAt(1.0, QColor(255, 180, 80, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(halo);
        painter.drawEllipse(QPointF(0, 0), radius * 1.45, radius * 1.45);

        QRadialGradient dough(QPointF(-radius * 0.28, -radius * 0.32), radius * 1.35);
        dough.setColorAt(0.0, QColor(255, 222, 153));
        dough.setColorAt(0.55, QColor(212, 139, 58));
        dough.setColorAt(1.0, QColor(125, 70, 28));
        painter.setBrush(dough);
        painter.setPen(QPen(QColor(94, 51, 22), side * 0.012));
        painter.drawEllipse(QPointF(0, 0), radius, radius);

        painter.setPen(Qt::NoPen);
        const std::vector<QPointF> chips = {
            {-0.46, -0.18}, {-0.30, 0.36}, {-0.12, -0.48}, {0.06, 0.18},
            {0.24, -0.22}, {0.38, 0.34}, {0.50, -0.02}, {-0.02, 0.52},
            {-0.55, 0.12}, {0.18, -0.56}, {-0.22, -0.02}, {0.44, -0.42}
        };

        for (std::size_t i = 0; i < chips.size(); ++i) {
            const QPointF chip = chips.at(i) * radius;
            const qreal chipRadius = radius * (0.075 + 0.018 * (i % 3));
            painter.setBrush(QColor(73, 36, 18));
            painter.drawEllipse(chip, chipRadius, chipRadius * 0.78);
            painter.setBrush(QColor(45, 21, 10, 110));
            painter.drawEllipse(chip + QPointF(chipRadius * 0.18, chipRadius * 0.2),
                                chipRadius * 0.52, chipRadius * 0.4);
        }

        painter.setPen(QPen(QColor(255, 241, 205, 160), side * 0.011, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRectF(-radius * 0.64, -radius * 0.68, radius * 1.04, radius * 0.92),
                        28 * 16, 74 * 16);
        painter.restore();
    }

private:
    void animateProperty(const char *propertyName, qreal start, qreal end, int duration, const QEasingCurve &curve)
    {
        auto *animation = new QPropertyAnimation(this, propertyName, this);
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->setDuration(duration);
        animation->setEasingCurve(curve);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    qreal m_pressScale = 1.0;
    qreal m_glow = 0.0;
    qreal m_pulse = 0.0;
    bool m_pressed = false;
    QVariantAnimation *m_pulseAnimation = nullptr;
};

struct Upgrade
{
    QString name;
    QString description;
    double cost = 0.0;
    double growth = 1.0;
    int level = 0;
    std::function<void()> apply;
    QLabel *levelLabel = nullptr;
    QLabel *costLabel = nullptr;
    QPushButton *button = nullptr;
};

class CookieClickerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CookieClickerWindow(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setWindowTitle("Cookie Clicker Qt6");
        setMinimumSize(980, 660);
        setStyleSheet(R"(
            QWidget {
                background: #2f1b12;
                color: #fff1d4;
                font-family: "Segoe UI", "Inter", sans-serif;
                font-size: 15px;
            }
            QLabel#Title {
                color: #ffe3a0;
                font-size: 38px;
                font-weight: 800;
            }
            QLabel#StatNumber {
                color: #ffffff;
                font-size: 32px;
                font-weight: 800;
            }
            QLabel#StatCaption {
                color: #d6b486;
                font-size: 14px;
            }
            QFrame#Panel {
                background: #4d2b1c;
                border: 2px solid #7a4a29;
                border-radius: 22px;
            }
            QFrame#UpgradeCard {
                background: #603721;
                border: 1px solid #9b6539;
                border-radius: 16px;
            }
            QPushButton {
                background: #f1a742;
                border: 0;
                border-radius: 12px;
                color: #40210d;
                font-weight: 800;
                padding: 10px 14px;
            }
            QPushButton:hover {
                background: #ffc15d;
            }
            QPushButton:pressed {
                background: #d88928;
            }
            QPushButton:disabled {
                background: #7b6550;
                color: #c8b29c;
            }
        )");

        auto *root = new QHBoxLayout(this);
        root->setContentsMargins(24, 24, 24, 24);
        root->setSpacing(24);

        auto *bakeryPanel = createPanel();
        auto *bakeryLayout = new QVBoxLayout(bakeryPanel);
        bakeryLayout->setContentsMargins(28, 24, 28, 24);
        bakeryLayout->setSpacing(18);

        auto *title = new QLabel("Cookie Bakery");
        title->setObjectName("Title");
        title->setAlignment(Qt::AlignCenter);

        m_cookieLabel = new QLabel;
        m_cookieLabel->setObjectName("StatNumber");
        m_cookieLabel->setAlignment(Qt::AlignCenter);

        m_rateLabel = new QLabel;
        m_rateLabel->setObjectName("StatCaption");
        m_rateLabel->setAlignment(Qt::AlignCenter);

        m_cookieButton = new CookieButton;
        connect(m_cookieButton, &CookieButton::clicked, this, &CookieClickerWindow::bakeCookie);

        m_tipLabel = new QLabel("Click the cookie, buy upgrades, and let the bakery warm up.");
        m_tipLabel->setObjectName("StatCaption");
        m_tipLabel->setAlignment(Qt::AlignCenter);
        m_tipLabel->setWordWrap(true);

        bakeryLayout->addWidget(title);
        bakeryLayout->addSpacing(4);
        bakeryLayout->addWidget(m_cookieLabel);
        bakeryLayout->addWidget(m_rateLabel);
        bakeryLayout->addWidget(m_cookieButton, 1);
        bakeryLayout->addWidget(m_tipLabel);

        auto *upgradePanel = createPanel();
        auto *upgradeLayout = new QVBoxLayout(upgradePanel);
        upgradeLayout->setContentsMargins(24, 22, 24, 22);
        upgradeLayout->setSpacing(16);

        auto *upgradeTitle = new QLabel("Bakery Upgrades");
        upgradeTitle->setObjectName("Title");
        upgradeTitle->setStyleSheet("font-size: 30px;");
        upgradeLayout->addWidget(upgradeTitle);

        auto *subtitle = new QLabel("Spend cookies on tools that increase clicks and passive baking.");
        subtitle->setObjectName("StatCaption");
        subtitle->setWordWrap(true);
        upgradeLayout->addWidget(subtitle);

        m_upgradeList = new QVBoxLayout;
        m_upgradeList->setSpacing(12);
        upgradeLayout->addLayout(m_upgradeList);
        upgradeLayout->addStretch();

        m_statusLabel = new QLabel;
        m_statusLabel->setObjectName("StatCaption");
        m_statusLabel->setWordWrap(true);
        upgradeLayout->addWidget(m_statusLabel);

        root->addWidget(bakeryPanel, 3);
        root->addWidget(upgradePanel, 2);

        setupUpgrades();

        auto *passiveTimer = new QTimer(this);
        passiveTimer->setInterval(100);
        connect(passiveTimer, &QTimer::timeout, this, &CookieClickerWindow::tickBakery);
        passiveTimer->start();

        updateUi();
    }

private slots:
    void bakeCookie()
    {
        addCookies(m_clickPower);
        showFloatingText(QString("+%1").arg(formatNumber(m_clickPower)), m_cookieButton);
        setStatus("Fresh crumbs! Each click now bakes " + formatNumber(m_clickPower) + " cookies.");
    }

    void tickBakery()
    {
        if (m_cookiesPerSecond <= 0.0) {
            return;
        }

        addCookies(m_cookiesPerSecond / 10.0);
    }

private:
    QFrame *createPanel()
    {
        auto *panel = new QFrame;
        panel->setObjectName("Panel");
        return panel;
    }

    void setupUpgrades()
    {
        addUpgrade("Rolling Pin",
                   "Adds +1 cookie to every click.",
                   25.0,
                   1.18,
                   [this] { m_clickPower += 1.0; });

        addUpgrade("Chocolate Chips",
                   "Adds +3 cookies to every click.",
                   85.0,
                   1.22,
                   [this] { m_clickPower += 3.0; });

        addUpgrade("Grandma's Oven",
                   "Bakes +4 cookies per second.",
                   120.0,
                   1.25,
                   [this] { m_cookiesPerSecond += 4.0; });

        addUpgrade("Sprinkle Storm",
                   "Adds +10 cookies per click and +8 per second.",
                   430.0,
                   1.28,
                   [this] {
                       m_clickPower += 10.0;
                       m_cookiesPerSecond += 8.0;
                   });

        addUpgrade("Golden Mixer",
                   "Boosts click power by 35% and bakes +20 per second.",
                   1400.0,
                   1.35,
                   [this] {
                       m_clickPower = std::ceil(m_clickPower * 1.35);
                       m_cookiesPerSecond += 20.0;
                   });
    }

    void addUpgrade(const QString &name,
                    const QString &description,
                    double cost,
                    double growth,
                    std::function<void()> apply)
    {
        Upgrade upgrade;
        upgrade.name = name;
        upgrade.description = description;
        upgrade.cost = cost;
        upgrade.growth = growth;
        upgrade.apply = std::move(apply);

        auto *card = new QFrame;
        card->setObjectName("UpgradeCard");
        auto *layout = new QVBoxLayout(card);
        layout->setContentsMargins(16, 14, 16, 14);
        layout->setSpacing(8);

        auto *headerLayout = new QHBoxLayout;
        auto *nameLabel = new QLabel(name);
        nameLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #ffe3a0;");
        upgrade.levelLabel = new QLabel;
        upgrade.levelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        upgrade.levelLabel->setObjectName("StatCaption");
        headerLayout->addWidget(nameLabel, 1);
        headerLayout->addWidget(upgrade.levelLabel);

        auto *descriptionLabel = new QLabel(description);
        descriptionLabel->setWordWrap(true);
        descriptionLabel->setObjectName("StatCaption");

        auto *buyLayout = new QHBoxLayout;
        upgrade.costLabel = new QLabel;
        upgrade.costLabel->setObjectName("StatCaption");
        upgrade.button = new QPushButton("Buy");
        connect(upgrade.button, &QPushButton::clicked, this, [this, index = static_cast<int>(m_upgrades.size())] {
            buyUpgrade(index);
        });

        buyLayout->addWidget(upgrade.costLabel, 1);
        buyLayout->addWidget(upgrade.button);

        layout->addLayout(headerLayout);
        layout->addWidget(descriptionLabel);
        layout->addLayout(buyLayout);

        m_upgradeList->addWidget(card);
        m_upgrades.push_back(std::move(upgrade));
    }

    void buyUpgrade(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_upgrades.size())) {
            return;
        }

        Upgrade &upgrade = m_upgrades[index];
        if (m_cookies < upgrade.cost) {
            setStatus("Need " + formatNumber(upgrade.cost - m_cookies) + " more cookies for " + upgrade.name + ".");
            pulseButton(upgrade.button);
            return;
        }

        m_cookies -= upgrade.cost;
        upgrade.apply();
        upgrade.level += 1;
        upgrade.cost = std::ceil(upgrade.cost * upgrade.growth);

        setStatus(upgrade.name + " upgraded! The bakery smells even better.");
        showFloatingText("Upgrade!", upgrade.button);
        updateUi();
    }

    void addCookies(double amount)
    {
        m_cookies += amount;
        m_totalCookies += amount;
        updateUi();
    }

    void updateUi()
    {
        m_cookieLabel->setText(formatNumber(m_cookies) + " cookies");
        m_rateLabel->setText(formatNumber(m_clickPower) + " per click  |  " +
                             formatNumber(m_cookiesPerSecond) + " per second  |  " +
                             formatNumber(m_totalCookies) + " all-time");

        for (Upgrade &upgrade : m_upgrades) {
            upgrade.levelLabel->setText("Level " + QString::number(upgrade.level));
            upgrade.costLabel->setText("Cost: " + formatNumber(upgrade.cost));
            upgrade.button->setEnabled(m_cookies >= upgrade.cost);
        }
    }

    void setStatus(const QString &message)
    {
        m_statusLabel->setText(message);
    }

    void showFloatingText(const QString &text, QWidget *anchor)
    {
        auto *label = new QLabel(text, this);
        label->setAttribute(Qt::WA_TransparentForMouseEvents);
        label->setStyleSheet("color: #fff7c7; font-size: 26px; font-weight: 900; background: transparent;");
        label->adjustSize();

        const QPoint anchorCenter = anchor->mapTo(this, anchor->rect().center());
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
        connect(group, &QParallelAnimationGroup::finished, label, &QLabel::deleteLater);
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
        connect(animation, &QPropertyAnimation::finished, effect, &QGraphicsColorizeEffect::deleteLater);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QString formatNumber(double value) const
    {
        if (value < 1000.0) {
            const double rounded = std::round(value * 10.0) / 10.0;
            return QString::number(rounded, 'f', rounded == std::floor(rounded) ? 0 : 1);
        }

        static const QStringList suffixes = {"", "K", "M", "B", "T", "Qa"};
        int suffixIndex = 0;
        double scaled = value;
        while (scaled >= 1000.0 && suffixIndex < suffixes.size() - 1) {
            scaled /= 1000.0;
            ++suffixIndex;
        }

        return QString::number(scaled, 'f', scaled >= 100.0 ? 0 : 1) + suffixes.at(suffixIndex);
    }

    double m_cookies = 0.0;
    double m_totalCookies = 0.0;
    double m_clickPower = 1.0;
    double m_cookiesPerSecond = 0.0;

    CookieButton *m_cookieButton = nullptr;
    QLabel *m_cookieLabel = nullptr;
    QLabel *m_rateLabel = nullptr;
    QLabel *m_tipLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QVBoxLayout *m_upgradeList = nullptr;
    std::vector<Upgrade> m_upgrades;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CookieClickerWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
