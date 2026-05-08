#pragma once

#include "Upgrade.h"

#include <QFrame>

class QLabel;
class QPushButton;

class UpgradeCard : public QFrame
{
    Q_OBJECT

public:
    explicit UpgradeCard(int upgradeIndex, QWidget *parent = nullptr);

    int upgradeIndex() const;
    void setUpgrade(const UpgradeState &upgrade, bool canBuy, const QString &formattedCost);
    void pulseBuyButton();

signals:
    void buyRequested(int upgradeIndex);

private:
    int m_upgradeIndex = -1;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_descriptionLabel = nullptr;
    QLabel *m_levelLabel = nullptr;
    QLabel *m_costLabel = nullptr;
    QPushButton *m_buyButton = nullptr;
};
