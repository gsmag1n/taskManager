#include "UpgradeCard.h"

#include "AnimationEffects.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

UpgradeCard::UpgradeCard(int upgradeIndex, QWidget *parent)
    : QFrame(parent)
    , m_upgradeIndex(upgradeIndex)
{
    setObjectName("UpgradeCard");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(8);

    auto *headerLayout = new QHBoxLayout;
    m_nameLabel = new QLabel;
    m_nameLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #ffe3a0;");

    m_levelLabel = new QLabel;
    m_levelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_levelLabel->setObjectName("StatCaption");

    headerLayout->addWidget(m_nameLabel, 1);
    headerLayout->addWidget(m_levelLabel);

    m_descriptionLabel = new QLabel;
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setObjectName("StatCaption");

    auto *buyLayout = new QHBoxLayout;
    m_costLabel = new QLabel;
    m_costLabel->setObjectName("StatCaption");

    m_buyButton = new QPushButton("Buy");
    connect(m_buyButton, &QPushButton::clicked, this, [this] {
        emit buyRequested(m_upgradeIndex);
    });

    buyLayout->addWidget(m_costLabel, 1);
    buyLayout->addWidget(m_buyButton);

    layout->addLayout(headerLayout);
    layout->addWidget(m_descriptionLabel);
    layout->addLayout(buyLayout);
}

int UpgradeCard::upgradeIndex() const
{
    return m_upgradeIndex;
}

void UpgradeCard::setUpgrade(const UpgradeState &upgrade, bool canBuy, const QString &formattedCost)
{
    m_nameLabel->setText(upgrade.definition.name);
    m_descriptionLabel->setText(upgrade.definition.description);
    m_levelLabel->setText("Level " + QString::number(upgrade.level));
    m_costLabel->setText("Cost: " + formattedCost);
    m_buyButton->setEnabled(canBuy);
}

void UpgradeCard::pulseBuyButton()
{
    AnimationEffects::pulseButton(m_buyButton);
}
