#pragma once

#include "CookieGame.h"

#include <QWidget>
#include <QVector>

class CookieButton;
class QFrame;
class QLabel;
class QVBoxLayout;
class UpgradeCard;

class CookieClickerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CookieClickerWindow(QWidget *parent = nullptr);

private:
    void buildUi();
    void applyTheme();
    void createUpgradeCards();
    void refreshUi();
    void handleCookieClicked();
    void handleUpgradeRequested(int index);
    void setStatus(const QString &message);

    static QFrame *createPanel();

    CookieGame m_game;
    CookieButton *m_cookieButton = nullptr;
    QLabel *m_cookieLabel = nullptr;
    QLabel *m_rateLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QVBoxLayout *m_upgradeList = nullptr;
    QVector<UpgradeCard *> m_upgradeCards;
};
