#pragma once

#include <QString>

struct UpgradeDefinition
{
    QString name;
    QString description;
    double baseCost = 0.0;
    double costGrowth = 1.0;
    double clickPowerBonus = 0.0;
    double cookiesPerSecondBonus = 0.0;
    double clickPowerMultiplier = 1.0;
};

struct UpgradeState
{
    UpgradeDefinition definition;
    double currentCost = 0.0;
    int level = 0;
};
