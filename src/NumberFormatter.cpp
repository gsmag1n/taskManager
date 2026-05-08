#include "NumberFormatter.h"

#include <QStringList>

#include <cmath>

namespace NumberFormatter
{
QString format(double value)
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
}
