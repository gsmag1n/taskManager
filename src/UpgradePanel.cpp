#include "UpgradePanel.h"

#include "Game.h"
#include "UpgradeCard.h"

#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

namespace cookie {

namespace {
QLabel* makeSectionHeader(const QString& text) {
    auto* l = new QLabel(text);
    l->setObjectName("sectionHeader");
    QFont f = l->font();
    f.setPointSize(11);
    f.setBold(true);
    f.setCapitalization(QFont::AllUppercase);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 1.2);
    l->setFont(f);
    return l;
}
} // namespace

UpgradePanel::UpgradePanel(Game* game, QWidget* parent)
    : QWidget(parent), m_game(game) {
    setObjectName("upgradePanel");

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->verticalScrollBar()->setSingleStep(24);
    outer->addWidget(scroll);

    auto* container = new QWidget;
    container->setObjectName("upgradePanelContainer");
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(8);

    layout->addWidget(makeSectionHeader("Click upgrades"));
    for (const auto& u : Game::clickUpgrades()) {
        auto* card = new UpgradeCard(UpgradeCard::Kind::ClickUpgrade,
                                     u.key, u.glyph, u.name, u.description);
        connect(card, &UpgradeCard::buyRequested, this, [this, key = u.key, card]() {
            for (const auto& def : Game::clickUpgrades()) {
                if (def.key == key) {
                    if (m_game->buyClickUpgrade(def)) card->playPurchaseFlash();
                    break;
                }
            }
        });
        m_cards.insert(u.key, card);
        layout->addWidget(card);
    }

    layout->addSpacing(10);
    layout->addWidget(makeSectionHeader("Buildings"));
    for (const auto& p : Game::producers()) {
        auto* card = new UpgradeCard(UpgradeCard::Kind::Producer,
                                     p.key, p.glyph, p.name, p.description);
        connect(card, &UpgradeCard::buyRequested, this, [this, key = p.key, card]() {
            for (const auto& def : Game::producers()) {
                if (def.key == key) {
                    if (m_game->buyProducer(def)) card->playPurchaseFlash();
                    break;
                }
            }
        });
        m_cards.insert(p.key, card);
        layout->addWidget(card);
    }

    layout->addStretch(1);
    scroll->setWidget(container);

    connect(m_game, &Game::stateChanged, this, &UpgradePanel::refresh);
    refresh();
}

void UpgradePanel::refresh() {
    for (const auto& u : Game::clickUpgrades()) {
        auto* card = m_cards.value(u.key);
        if (!card) continue;
        const bool owned = m_game->hasClickUpgrade(u.key);
        card->setStatus(u.cost, /*count=*/0,
                        /*affordable=*/m_game->canAfford(u.cost) && !owned,
                        /*ownedOneShot=*/owned);
    }
    for (const auto& p : Game::producers()) {
        auto* card = m_cards.value(p.key);
        if (!card) continue;
        const double cost = m_game->producerCost(p);
        card->setStatus(cost,
                        m_game->producerCount(p.key),
                        m_game->canAfford(cost),
                        /*ownedOneShot=*/false);
    }
}

} // namespace cookie
