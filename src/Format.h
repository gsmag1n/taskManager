#pragma once

#include <QString>

namespace cookie {

/// Compact numeric formatting: 1234 -> "1.23K", 1.5e9 -> "1.50B", etc.
QString formatNumber(double n);

/// Like formatNumber but shows at least 1 decimal even for small numbers.
QString formatRate(double n);

} // namespace cookie
