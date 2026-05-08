#pragma once

#include <QMainWindow>
#include <QElapsedTimer>

class QLabel;
class QTimer;

namespace cookie {

class Game;
class CookieButton;
class UpgradePanel;
class Toast;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private slots:
    void onTick();
    void refreshStats();
    void onCookieClicked();

private:
    void buildUi();
    void applyTheme();

    Game*         m_game        = nullptr;
    CookieButton* m_cookie      = nullptr;
    UpgradePanel* m_upgrades    = nullptr;
    Toast*        m_toast       = nullptr;

    QLabel* m_cookiesLabel      = nullptr;
    QLabel* m_cookiesLabelSmall = nullptr;
    QLabel* m_cpsLabel          = nullptr;
    QLabel* m_clickPowerLabel   = nullptr;
    QLabel* m_clicksLabel       = nullptr;
    QLabel* m_totalLabel        = nullptr;

    QTimer*        m_tickTimer  = nullptr;
    QTimer*        m_saveTimer  = nullptr;
    QElapsedTimer  m_tickClock;
    qint64         m_lastTickMs = 0;
};

} // namespace cookie
