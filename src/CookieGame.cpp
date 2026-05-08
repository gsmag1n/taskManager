#include "CookieGame.h"

#include "UpgradeCatalog.h"

#include <QtGlobal>

#include <cmath>

CookieGame::CookieGame(QObject *parent)
    : QObject(parent)
{
    for (const UpgradeDefinition &definition : UpgradeCatalog::createDefaultUpgrades()) {
        UpgradeState state;
        state.definition = definition;
        state.currentCost = definition.baseCost;
        m_upgrades.push_back(state);
    }
}

double CookieGame::cookies() const
{
    return m_cookies;
}

double CookieGame::totalCookies() const
{
    return m_totalCookies;
}

double CookieGame::clickPower() const
{
    return m_clickPower;
}

double CookieGame::cookiesPerSecond() const
{
    return m_cookiesPerSecond;
}

int CookieGame::upgradeCount() const
{
    return m_upgrades.size();
}

const UpgradeState &CookieGame::upgrade(int index) const
{
    Q_ASSERT(isValidUpgradeIndex(index));
    return m_upgrades.at(index);
}

bool CookieGame::canBuyUpgrade(int index) const
{
    return isValidUpgradeIndex(index) && m_cookies >= m_upgrades.at(index).currentCost;
}

double CookieGame::bakeClick()
{
    addCookies(m_clickPower);
    return m_clickPower;
}

void CookieGame::tick(double seconds)
{
    if (seconds <= 0.0 || m_cookiesPerSecond <= 0.0) {
        return;
    }

    addCookies(m_cookiesPerSecond * seconds);
}

PurchaseResult CookieGame::buyUpgrade(int index)
{
    PurchaseResult result;
    result.upgradeIndex = index;

    if (!isValidUpgradeIndex(index)) {
        result.status = PurchaseStatus::InvalidUpgrade;
        return result;
    }

    UpgradeState &state = m_upgrades[index];
    if (m_cookies < state.currentCost) {
        result.status = PurchaseStatus::NotEnoughCookies;
        result.cookiesMissing = state.currentCost - m_cookies;
        return result;
    }

    m_cookies -= state.currentCost;
    m_clickPower += state.definition.clickPowerBonus;
    m_cookiesPerSecond += state.definition.cookiesPerSecondBonus;

    if (state.definition.clickPowerMultiplier != 1.0) {
        m_clickPower = std::ceil(m_clickPower * state.definition.clickPowerMultiplier);
    }

    state.level += 1;
    state.currentCost = std::ceil(state.currentCost * state.definition.costGrowth);

    result.status = PurchaseStatus::Success;
    emit changed();
    return result;
}

void CookieGame::addCookies(double amount)
{
    if (amount <= 0.0) {
        return;
    }

    m_cookies += amount;
    m_totalCookies += amount;
    emit changed();
}

bool CookieGame::isValidUpgradeIndex(int index) const
{
    return index >= 0 && index < m_upgrades.size();
}
