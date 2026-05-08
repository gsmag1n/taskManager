#include "Game.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include <cmath>

namespace cookie {

// -----------------------------------------------------------------------------
// Static catalogs
// -----------------------------------------------------------------------------
const QVector<ProducerDef>& Game::producers() {
    static const QVector<ProducerDef> kProducers = {
        {"cursor",   "Cursor",        "Auto-clicks the cookie for you.",          "👆", 15.0,         0.1},
        {"grandma",  "Grandma",       "A nice grandma to bake more cookies.",     "👵", 100.0,        1.0},
        {"bakery",   "Bakery",        "A whole bakery devoted to cookies.",       "🥐", 1'100.0,      8.0},
        {"factory",  "Factory",       "Industrial-scale cookie production.",      "🏭", 12'000.0,     47.0},
        {"mine",     "Cookie Mine",   "Mines deep for ancient cookie ore.",       "⛏️", 130'000.0,    260.0},
        {"shipment", "Shipment",      "Imports cookies from cookie planets.",     "🚀", 1'400'000.0,  1'400.0},
        {"alchemy",  "Alchemy Lab",   "Transmutes gold into cookies.",            "⚗️", 20'000'000.0, 7'800.0},
        {"portal",   "Portal",        "Opens a rift to the Cookieverse.",         "🌀", 330'000'000.0,44'000.0},
        {"timemachine","Time Machine","Brings back cookies from the past.",       "⏳", 5'100'000'000.0,260'000.0},
    };
    return kProducers;
}

const QVector<ClickUpgradeDef>& Game::clickUpgrades() {
    static const QVector<ClickUpgradeDef> kUpgrades = {
        {"reinforced_mouse","Reinforced Mouse","Each click is worth +1 cookie.",    "🖱️", 100.0,       1.0, 1.0},
        {"steel_finger",    "Steel Finger",   "Each click is worth +5 cookies.",    "🤛", 2'500.0,     5.0, 1.0},
        {"sugar_rush",      "Sugar Rush",     "Doubles click power.",               "🍬", 15'000.0,    0.0, 2.0},
        {"golden_whisk",    "Golden Whisk",   "Each click is worth +50 cookies.",   "🥄", 120'000.0,   50.0, 1.0},
        {"cosmic_finger",   "Cosmic Finger",  "Doubles click power again.",         "🌌", 1'500'000.0, 0.0, 2.0},
        {"midas_touch",     "Midas Touch",    "Doubles click power once more.",     "✨", 50'000'000.0, 0.0, 2.0},
    };
    return kUpgrades;
}

namespace {
bool ownsAtLeastOneOfEach(const Game& g) {
    for (const auto& p : Game::producers()) {
        if (g.producerCount(p.key) <= 0) return false;
    }
    return true;
}
}

const QVector<Achievement>& Game::achievements() {
    static const QVector<Achievement> kAchievements = {
        {"first_click",  "First Crumb",    "Click the cookie for the first time.",                     "🍪",
            [](const Game& g){ return g.clicks() >= 1; }},
        {"hundred",      "Snack Time",     "Bake 100 cookies in total.",                               "🍩",
            [](const Game& g){ return g.totalCookies() >= 100; }},
        {"ten_k",        "Bakery Boss",    "Bake 10,000 cookies in total.",                            "👑",
            [](const Game& g){ return g.totalCookies() >= 10'000; }},
        {"million",      "Cookie Tycoon",  "Bake 1,000,000 cookies in total.",                         "💰",
            [](const Game& g){ return g.totalCookies() >= 1'000'000; }},
        {"billion",      "Cookie Magnate", "Bake 1,000,000,000 cookies in total.",                     "💎",
            [](const Game& g){ return g.totalCookies() >= 1'000'000'000; }},
        {"clicker",      "Clicky Fingers", "Click the cookie 250 times.",                              "👆",
            [](const Game& g){ return g.clicks() >= 250; }},
        {"idle",         "Hands Off",      "Reach 100 cookies per second.",                            "🛋️",
            [](const Game& g){ return g.cps() >= 100; }},
        {"speedy",       "Cookie Storm",   "Reach 10,000 cookies per second.",                         "🌪️",
            [](const Game& g){ return g.cps() >= 10'000; }},
        {"diversified",  "Diversified",    "Own at least one of every building.",                      "🏆",
            [](const Game& g){ return ownsAtLeastOneOfEach(g); }},
    };
    return kAchievements;
}

// -----------------------------------------------------------------------------
// Game
// -----------------------------------------------------------------------------
Game::Game(QObject* parent) : QObject(parent) {}

double Game::clickPower() const {
    double additive = 1.0;
    double multi    = 1.0;
    for (const auto& u : clickUpgrades()) {
        if (m_clickUpgrades.contains(u.key)) {
            additive += u.add;
            multi    *= u.multiply;
        }
    }
    // A small fraction of CPS contributes to click power, à la Cookie Clicker.
    return additive * multi + cps() * 0.01;
}

double Game::cps() const {
    double total = 0.0;
    for (const auto& p : producers()) {
        total += p.baseCps * m_producerCounts.value(p.key, 0);
    }
    return total;
}

double Game::producerCost(const ProducerDef& p) const {
    const int owned = m_producerCounts.value(p.key, 0);
    return std::floor(p.baseCost * std::pow(kCostGrowth, owned));
}

double Game::click() {
    const double gain = clickPower();
    m_cookies      += gain;
    m_totalCookies += gain;
    ++m_clicks;
    emit clicked(gain);
    emit stateChanged();
    evaluateAchievements();
    return gain;
}

double Game::tick(double dtSeconds) {
    if (dtSeconds <= 0.0) return 0.0;
    const double gain = cps() * dtSeconds;
    if (gain > 0.0) {
        m_cookies      += gain;
        m_totalCookies += gain;
        emit stateChanged();
        evaluateAchievements();
    }
    return gain;
}

bool Game::buyProducer(const ProducerDef& p) {
    const double cost = producerCost(p);
    if (!canAfford(cost)) return false;
    m_cookies -= cost;
    m_producerCounts[p.key] = m_producerCounts.value(p.key, 0) + 1;
    emit purchased(p.key);
    emit stateChanged();
    evaluateAchievements();
    return true;
}

bool Game::buyClickUpgrade(const ClickUpgradeDef& u) {
    if (m_clickUpgrades.contains(u.key)) return false;
    if (!canAfford(u.cost)) return false;
    m_cookies -= u.cost;
    m_clickUpgrades.insert(u.key);
    emit purchased(u.key);
    emit stateChanged();
    evaluateAchievements();
    return true;
}

void Game::evaluateAchievements() {
    for (const auto& a : achievements()) {
        if (m_achievementsUnlocked.contains(a.key)) continue;
        if (a.test && a.test(*this)) {
            m_achievementsUnlocked.insert(a.key);
            emit achievementUnlocked(a);
        }
    }
}

void Game::resetForNewGame() {
    m_cookies = 0.0;
    m_totalCookies = 0.0;
    m_clicks = 0;
    m_producerCounts.clear();
    m_clickUpgrades.clear();
    m_achievementsUnlocked.clear();
    emit stateChanged();
}

// -----------------------------------------------------------------------------
// Persistence
// -----------------------------------------------------------------------------
QString Game::defaultSavePath() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(dir.isEmpty() ? QDir::homePath() + "/.cookie_clicker" : dir)
        .filePath("save.json");
}

bool Game::save(const QString& path) const {
    const QString p = path.isEmpty() ? defaultSavePath() : path;
    QFileInfo info(p);
    QDir().mkpath(info.absolutePath());

    QJsonObject root;
    root.insert("cookies",       m_cookies);
    root.insert("total_cookies", m_totalCookies);
    root.insert("clicks",        static_cast<qint64>(m_clicks));

    QJsonObject prod;
    for (auto it = m_producerCounts.cbegin(); it != m_producerCounts.cend(); ++it) {
        prod.insert(it.key(), it.value());
    }
    root.insert("producer_counts", prod);

    QJsonArray click;
    for (const auto& key : m_clickUpgrades) click.append(key);
    root.insert("click_upgrades", click);

    QJsonArray ach;
    for (const auto& key : m_achievementsUnlocked) ach.append(key);
    root.insert("achievements", ach);

    QFile f(p);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool Game::load(const QString& path) {
    const QString p = path.isEmpty() ? defaultSavePath() : path;
    QFile f(p);
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) return false;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) return false;
    const auto root = doc.object();

    m_cookies      = root.value("cookies").toDouble(0.0);
    m_totalCookies = root.value("total_cookies").toDouble(0.0);
    m_clicks       = static_cast<qint64>(root.value("clicks").toDouble(0.0));

    m_producerCounts.clear();
    const auto prod = root.value("producer_counts").toObject();
    for (auto it = prod.begin(); it != prod.end(); ++it) {
        m_producerCounts.insert(it.key(), it.value().toInt(0));
    }

    m_clickUpgrades.clear();
    for (const auto& v : root.value("click_upgrades").toArray()) {
        m_clickUpgrades.insert(v.toString());
    }

    m_achievementsUnlocked.clear();
    for (const auto& v : root.value("achievements").toArray()) {
        m_achievementsUnlocked.insert(v.toString());
    }

    emit stateChanged();
    return true;
}

} // namespace cookie
