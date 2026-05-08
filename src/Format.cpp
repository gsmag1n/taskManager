#include "Format.h"

#include <array>
#include <cmath>

namespace cookie {

namespace {
struct Suffix { double value; const char* name; };
constexpr std::array<Suffix, 11> kSuffixes = {{
    {1e3,  "K"}, {1e6,  "M"}, {1e9,  "B"}, {1e12, "T"},
    {1e15, "Qa"}, {1e18, "Qi"}, {1e21, "Sx"}, {1e24, "Sp"},
    {1e27, "Oc"}, {1e30, "No"}, {1e33, "Dc"},
}};
}

QString formatNumber(double n) {
    if (!std::isfinite(n)) return QStringLiteral("∞");
    const bool negative = n < 0;
    if (negative) n = -n;

    QString out;
    if (n < 1000.0) {
        if (std::floor(n) == n) out = QString::number(static_cast<qint64>(n));
        else                    out = QString::number(n, 'f', 1);
    } else {
        Suffix chosen{1.0, ""};
        for (const auto& s : kSuffixes) {
            if (n >= s.value) chosen = s;
            else break;
        }
        const double scaled = n / chosen.value;
        int decimals = scaled >= 100.0 ? 0 : (scaled >= 10.0 ? 1 : 2);
        out = QString::number(scaled, 'f', decimals) + QLatin1String(chosen.name);
    }
    return negative ? QStringLiteral("-") + out : out;
}

QString formatRate(double n) {
    if (n < 100.0) return QString::number(n, 'f', 1);
    return formatNumber(n);
}

} // namespace cookie
