#include "UpgradeCatalog.h"

namespace UpgradeCatalog
{
QVector<UpgradeDefinition> createDefaultUpgrades()
{
    return {
        {
            "Rolling Pin",
            "Adds +1 cookie to every click.",
            25.0,
            1.18,
            1.0,
            0.0,
            1.0,
        },
        {
            "Chocolate Chips",
            "Adds +3 cookies to every click.",
            85.0,
            1.22,
            3.0,
            0.0,
            1.0,
        },
        {
            "Grandma's Oven",
            "Bakes +4 cookies per second.",
            120.0,
            1.25,
            0.0,
            4.0,
            1.0,
        },
        {
            "Sprinkle Storm",
            "Adds +10 cookies per click and +8 per second.",
            430.0,
            1.28,
            10.0,
            8.0,
            1.0,
        },
        {
            "Golden Mixer",
            "Boosts click power by 35% and bakes +20 per second.",
            1400.0,
            1.35,
            0.0,
            20.0,
            1.35,
        },
    };
}
}
