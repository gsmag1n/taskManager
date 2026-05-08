#pragma once

#include <QWidget>
#include <QHash>

class QVBoxLayout;
class QScrollArea;

namespace cookie {

class Game;
class UpgradeCard;

/// Two stacked sections: "Click upgrades" (one-shot) and "Buildings" (repeatable).
/// Wires each card to the `Game` and keeps prices/counts in sync.
class UpgradePanel : public QWidget {
    Q_OBJECT
public:
    explicit UpgradePanel(Game* game, QWidget* parent = nullptr);

public slots:
    void refresh();

private:
    Game* m_game = nullptr;
    QHash<QString, UpgradeCard*> m_cards;   // keyed by upgrade key
};

} // namespace cookie
