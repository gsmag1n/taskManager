#pragma once

#include "Upgrade.h"

#include <QObject>
#include <QVector>

enum class PurchaseStatus
{
    Success,
    NotEnoughCookies,
    InvalidUpgrade,
};

struct PurchaseResult
{
    PurchaseStatus status = PurchaseStatus::InvalidUpgrade;
    int upgradeIndex = -1;
    double cookiesMissing = 0.0;
};

class CookieGame : public QObject
{
    Q_OBJECT

public:
    explicit CookieGame(QObject *parent = nullptr);

    double cookies() const;
    double totalCookies() const;
    double clickPower() const;
    double cookiesPerSecond() const;

    int upgradeCount() const;
    const UpgradeState &upgrade(int index) const;
    bool canBuyUpgrade(int index) const;

    double bakeClick();
    void tick(double seconds);
    PurchaseResult buyUpgrade(int index);

signals:
    void changed();

private:
    void addCookies(double amount);
    bool isValidUpgradeIndex(int index) const;

    double m_cookies = 0.0;
    double m_totalCookies = 0.0;
    double m_clickPower = 1.0;
    double m_cookiesPerSecond = 0.0;
    QVector<UpgradeState> m_upgrades;
};
