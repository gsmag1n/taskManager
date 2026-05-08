#include "CookieClickerWindow.h"

#include "AnimationEffects.h"
#include "CookieButton.h"
#include "NumberFormatter.h"
#include "UpgradeCard.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

CookieClickerWindow::CookieClickerWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Cookie Clicker Qt6");
    setMinimumSize(980, 660);

    applyTheme();
    buildUi();
    createUpgradeCards();

    connect(&m_game, &CookieGame::changed, this, &CookieClickerWindow::refreshUi);
    connect(m_cookieButton, &CookieButton::clicked, this, &CookieClickerWindow::handleCookieClicked);

    auto *passiveTimer = new QTimer(this);
    passiveTimer->setInterval(100);
    connect(passiveTimer, &QTimer::timeout, this, [this] {
        m_game.tick(0.1);
    });
    passiveTimer->start();

    refreshUi();
}

void CookieClickerWindow::buildUi()
{
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

    auto *tipLabel = new QLabel("Click the cookie, buy upgrades, and let the bakery warm up.");
    tipLabel->setObjectName("StatCaption");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setWordWrap(true);

    bakeryLayout->addWidget(title);
    bakeryLayout->addSpacing(4);
    bakeryLayout->addWidget(m_cookieLabel);
    bakeryLayout->addWidget(m_rateLabel);
    bakeryLayout->addWidget(m_cookieButton, 1);
    bakeryLayout->addWidget(tipLabel);

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
}

void CookieClickerWindow::applyTheme()
{
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
}

void CookieClickerWindow::createUpgradeCards()
{
    for (int index = 0; index < m_game.upgradeCount(); ++index) {
        auto *card = new UpgradeCard(index);
        connect(card, &UpgradeCard::buyRequested, this, &CookieClickerWindow::handleUpgradeRequested);
        m_upgradeList->addWidget(card);
        m_upgradeCards.push_back(card);
    }
}

void CookieClickerWindow::refreshUi()
{
    m_cookieLabel->setText(NumberFormatter::format(m_game.cookies()) + " cookies");
    m_rateLabel->setText(NumberFormatter::format(m_game.clickPower()) + " per click  |  " +
                         NumberFormatter::format(m_game.cookiesPerSecond()) + " per second  |  " +
                         NumberFormatter::format(m_game.totalCookies()) + " all-time");

    for (UpgradeCard *card : m_upgradeCards) {
        const int index = card->upgradeIndex();
        const UpgradeState &upgrade = m_game.upgrade(index);
        card->setUpgrade(upgrade,
                         m_game.canBuyUpgrade(index),
                         NumberFormatter::format(upgrade.currentCost));
    }
}

void CookieClickerWindow::handleCookieClicked()
{
    const double bakedCookies = m_game.bakeClick();
    AnimationEffects::showFloatingText(this,
                                       m_cookieButton,
                                       "+" + NumberFormatter::format(bakedCookies));
    setStatus("Fresh crumbs! Each click now bakes " +
              NumberFormatter::format(m_game.clickPower()) + " cookies.");
}

void CookieClickerWindow::handleUpgradeRequested(int index)
{
    const PurchaseResult result = m_game.buyUpgrade(index);
    if (result.status == PurchaseStatus::InvalidUpgrade) {
        return;
    }

    UpgradeCard *card = m_upgradeCards.value(index, nullptr);
    if (result.status == PurchaseStatus::NotEnoughCookies) {
        const UpgradeState &upgrade = m_game.upgrade(index);
        setStatus("Need " + NumberFormatter::format(result.cookiesMissing) +
                  " more cookies for " + upgrade.definition.name + ".");
        if (card) {
            card->pulseBuyButton();
        }
        return;
    }

    const UpgradeState &upgrade = m_game.upgrade(index);
    setStatus(upgrade.definition.name + " upgraded! The bakery smells even better.");
    if (card) {
        AnimationEffects::showFloatingText(this, card, "Upgrade!");
    }
}

void CookieClickerWindow::setStatus(const QString &message)
{
    m_statusLabel->setText(message);
}

QFrame *CookieClickerWindow::createPanel()
{
    auto *panel = new QFrame;
    panel->setObjectName("Panel");
    return panel;
}
