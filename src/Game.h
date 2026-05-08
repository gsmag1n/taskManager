#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include <QSet>

#include <functional>

namespace cookie {

// Cost growth per purchase (classic idle-game multiplier).
constexpr double kCostGrowth = 1.15;

struct ProducerDef {
    QString key;
    QString name;
    QString description;
    QString glyph;          // emoji-like glyph rendered in the card
    double  baseCost;
    double  baseCps;
};

struct ClickUpgradeDef {
    QString key;
    QString name;
    QString description;
    QString glyph;
    double  cost;
    double  add;            // additive bonus to click power
    double  multiply;       // multiplicative bonus to click power
};

struct Achievement {
    QString key;
    QString name;
    QString description;
    QString glyph;
    std::function<bool(const class Game&)> test;
};

/// Pure game state and rules. Emits change signals for the UI to react to.
class Game : public QObject {
    Q_OBJECT
public:
    explicit Game(QObject* parent = nullptr);

    static const QVector<ProducerDef>&     producers();
    static const QVector<ClickUpgradeDef>& clickUpgrades();
    static const QVector<Achievement>&     achievements();

    // ---- accessors ----------------------------------------------------------
    double cookies() const         { return m_cookies; }
    double totalCookies() const    { return m_totalCookies; }
    qint64 clicks() const          { return m_clicks; }
    int    producerCount(const QString& key) const { return m_producerCounts.value(key, 0); }
    bool   hasClickUpgrade(const QString& key) const { return m_clickUpgrades.contains(key); }
    bool   hasAchievement(const QString& key) const  { return m_achievementsUnlocked.contains(key); }

    double clickPower() const;
    double cps() const;
    double producerCost(const ProducerDef& p) const;
    bool   canAfford(double cost) const { return m_cookies + 1e-9 >= cost; }

    // ---- actions ------------------------------------------------------------
    /// Apply manual click. Returns the cookies gained.
    double click();

    /// Advance idle production by `dtSeconds`. Returns gained cookies.
    double tick(double dtSeconds);

    bool buyProducer(const ProducerDef& p);
    bool buyClickUpgrade(const ClickUpgradeDef& u);

    /// Re-evaluate achievement predicates and emit `achievementUnlocked`
    /// for any newly satisfied ones.
    void evaluateAchievements();

    // ---- persistence --------------------------------------------------------
    static QString defaultSavePath();
    bool save(const QString& path = {}) const;
    bool load(const QString& path = {});
    void resetForNewGame();

signals:
    void stateChanged();                    // cookies / cps / counts changed
    void purchased(const QString& key);     // a producer or click upgrade was bought
    void achievementUnlocked(const Achievement& a);
    void clicked(double gained);

private:
    double m_cookies      = 0.0;
    double m_totalCookies = 0.0;
    qint64 m_clicks       = 0;
    QHash<QString, int> m_producerCounts;
    QSet<QString>       m_clickUpgrades;
    QSet<QString>       m_achievementsUnlocked;
};

} // namespace cookie
