#include "MainWindow.h"

#include "CookieButton.h"
#include "FloatingText.h"
#include "Format.h"
#include "Game.h"
#include "Toast.h"
#include "UpgradePanel.h"

#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace cookie {

namespace {
constexpr int kTickIntervalMs = 100;     // 10 Hz idle accrual
constexpr int kSaveIntervalMs = 15'000;  // autosave every 15s
} // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Cookie Clicker");
    resize(1080, 720);
    setMinimumSize(880, 580);

    m_game = new Game(this);
    m_game->load();

    buildUi();
    applyTheme();
    refreshStats();

    m_tickClock.start();
    m_lastTickMs = m_tickClock.elapsed();

    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(kTickIntervalMs);
    connect(m_tickTimer, &QTimer::timeout, this, &MainWindow::onTick);
    m_tickTimer->start();

    m_saveTimer = new QTimer(this);
    m_saveTimer->setInterval(kSaveIntervalMs);
    connect(m_saveTimer, &QTimer::timeout, this, [this]{ m_game->save(); });
    m_saveTimer->start();

    connect(m_game, &Game::stateChanged, this, &MainWindow::refreshStats);
    connect(m_game, &Game::achievementUnlocked, this,
            [this](const Achievement& a) {
                if (m_toast) m_toast->enqueue(a.glyph,
                                              QStringLiteral("Achievement: %1").arg(a.name),
                                              a.description);
            });

    statusBar()->showMessage("Click the cookie to start. Progress autosaves.");
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi() {
    auto* central = new QWidget(this);
    central->setObjectName("centralPane");
    setCentralWidget(central);

    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // -------------------------------------------------------- Left stats column
    auto* leftCol = new QFrame;
    leftCol->setObjectName("leftPane");
    leftCol->setFrameShape(QFrame::NoFrame);
    leftCol->setFixedWidth(260);

    auto* leftLayout = new QVBoxLayout(leftCol);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(8);

    auto* title = new QLabel("Cookie Clicker");
    title->setObjectName("appTitle");
    QFont tFont = title->font();
    tFont.setPointSize(15);
    tFont.setBold(true);
    title->setFont(tFont);
    leftLayout->addWidget(title);

    auto* subtitle = new QLabel("A Qt6 idle game");
    subtitle->setObjectName("appSubtitle");
    leftLayout->addWidget(subtitle);
    leftLayout->addSpacing(18);

    m_cookiesLabel = new QLabel("0");
    m_cookiesLabel->setObjectName("cookiesBig");
    QFont bigFont = m_cookiesLabel->font();
    bigFont.setPointSize(36);
    bigFont.setBold(true);
    m_cookiesLabel->setFont(bigFont);
    m_cookiesLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftLayout->addWidget(m_cookiesLabel);

    m_cookiesLabelSmall = new QLabel("cookies");
    m_cookiesLabelSmall->setObjectName("cookiesSmall");
    leftLayout->addWidget(m_cookiesLabelSmall);
    leftLayout->addSpacing(14);

    auto addStat = [&](const QString& label, QLabel** out) {
        auto* row = new QFrame;
        row->setObjectName("statRow");
        auto* hl = new QHBoxLayout(row);
        hl->setContentsMargins(0, 0, 0, 0);
        auto* l = new QLabel(label);
        l->setObjectName("statLabel");
        auto* v = new QLabel("0");
        v->setObjectName("statValue");
        QFont vf = v->font();
        vf.setBold(true);
        v->setFont(vf);
        v->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        hl->addWidget(l);
        hl->addStretch(1);
        hl->addWidget(v);
        leftLayout->addWidget(row);
        *out = v;
    };

    addStat("per second",  &m_cpsLabel);
    addStat("per click",   &m_clickPowerLabel);
    addStat("clicks",      &m_clicksLabel);
    addStat("baked total", &m_totalLabel);

    leftLayout->addStretch(1);

    auto* hint = new QLabel("Tip: press space or enter to click.");
    hint->setObjectName("hint");
    hint->setWordWrap(true);
    leftLayout->addWidget(hint);

    root->addWidget(leftCol);

    // -------------------------------------------------- Center cookie + canvas
    auto* center = new QWidget;
    center->setObjectName("centerPane");
    auto* centerLayout = new QVBoxLayout(center);
    centerLayout->setContentsMargins(20, 20, 20, 20);
    centerLayout->setSpacing(0);
    centerLayout->addStretch(1);

    m_cookie = new CookieButton(center);
    auto* row = new QHBoxLayout;
    row->addStretch(1);
    row->addWidget(m_cookie);
    row->addStretch(1);
    centerLayout->addLayout(row);

    centerLayout->addStretch(1);

    connect(m_cookie, &CookieButton::clicked, this, &MainWindow::onCookieClicked);

    root->addWidget(center, 1);

    // ----------------------------------------------------- Right upgrade panel
    m_upgrades = new UpgradePanel(m_game);
    m_upgrades->setObjectName("rightPane");
    m_upgrades->setMinimumWidth(360);
    m_upgrades->setMaximumWidth(460);
    root->addWidget(m_upgrades);

    // Toast must be parented to the central pane so it overlays everything.
    m_toast = new Toast(central);
    m_toast->raise();
}

void MainWindow::applyTheme() {
    static const char* kQss = R"qss(
        QMainWindow, #centralPane { background: #1A100A; }
        #leftPane {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2A1A10, stop:1 #1A100A);
            border-right: 1px solid #3A2418;
        }
        #centerPane {
            background: qradialgradient(cx:0.5, cy:0.4, radius:0.9,
                fx:0.5, fy:0.4,
                stop:0 #3A2418, stop:1 #150C07);
        }
        #rightPane, #upgradePanelContainer {
            background: #1F140D;
            border-left: 1px solid #3A2418;
        }
        QScrollArea { background: transparent; border: none; }
        QScrollBar:vertical {
            background: transparent;
            width: 10px;
            margin: 4px;
        }
        QScrollBar::handle:vertical {
            background: #5A3A22;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #7A4F2E; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }

        QLabel { color: #E9CDA9; }
        #appTitle { color: #FFE6A8; }
        #appSubtitle { color: #B58E6E; font-size: 10pt; }
        #cookiesBig { color: #FFE6A8; }
        #cookiesSmall { color: #B58E6E; font-size: 10pt; }
        #statLabel { color: #B58E6E; }
        #statValue { color: #FFE6A8; }
        #sectionHeader { color: #FFC86B; padding: 6px 4px 2px 4px; }
        #hint { color: #8A6A50; font-size: 9pt; }

        QStatusBar { background: #150C07; color: #B58E6E; }
        QStatusBar::item { border: 0; }
    )qss";
    setStyleSheet(QString::fromLatin1(kQss));
}

void MainWindow::refreshStats() {
    if (!m_cookiesLabel) return;
    m_cookiesLabel->setText(formatNumber(m_game->cookies()));
    m_cpsLabel->setText(formatRate(m_game->cps()));
    m_clickPowerLabel->setText(formatNumber(m_game->clickPower()));
    m_clicksLabel->setText(QString::number(m_game->clicks()));
    m_totalLabel->setText(formatNumber(m_game->totalCookies()));
}

void MainWindow::onCookieClicked() {
    const double gained = m_game->click();

    // Spawn a "+N" floating label centered on the cookie, in the central pane's
    // coordinate space.
    QWidget* container = centralWidget();
    if (container && m_cookie) {
        const QPoint center = m_cookie->mapTo(container,
                                              QPoint(m_cookie->width() / 2,
                                                     m_cookie->height() / 2 - 20));
        FloatingText::spawn(container, center,
                            QStringLiteral("+%1").arg(formatNumber(gained)));
    }
}

void MainWindow::onTick() {
    const qint64 now = m_tickClock.elapsed();
    const double dt = (now - m_lastTickMs) / 1000.0;
    m_lastTickMs = now;
    m_game->tick(dt);
}

void MainWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (m_toast) m_toast->reanchor();
}

void MainWindow::closeEvent(QCloseEvent* e) {
    m_game->save();
    QMainWindow::closeEvent(e);
}

} // namespace cookie
