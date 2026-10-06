#include "MainWindow.h"
#include "LanguageManager.h"
#include "HelpDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QSpinBox>
#include <QTimeEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QCheckBox>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QIcon>
#include <QFont>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(QStringLiteral(":/icons/qshutdown.svg")));
    setMinimumSize(480, 520);
    resize(500, 540);

    setupUi();
    setupTrayIcon();
    setupLanguageMenu();

    connect(&m_countdownTimer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(&m_shutdownManager, &ShutdownManager::actionExecuted, this, [this](ShutdownManager::Action, bool success, const QString &msg) {
        auto &lm = LanguageManager::instance();
        if (!success) {
            QMessageBox::critical(this, lm.text(QStringLiteral("error_title")), msg);
        } else if (m_shutdownManager.isDryRun()) {
            QMessageBox::information(this, lm.text(QStringLiteral("test_completed_title")), msg);
        }
    });

    connect(&LanguageManager::instance(), &LanguageManager::languageChanged, this, &MainWindow::retranslateUi);

    retranslateUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 18, 20, 18);
    mainLayout->setSpacing(14);

    // Header Layout: Icona + Titolo + Language + Help
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);

    auto *titleIconLabel = new QLabel(this);
    titleIconLabel->setPixmap(QIcon(QStringLiteral(":/icons/qshutdown.svg")).pixmap(36, 36));
    titleIconLabel->setFixedSize(36, 36);

    m_titleLabel = new QLabel(this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(15);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    headerLayout->addWidget(titleIconLabel);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    // Pulsante Menu Lingua (Popup)
    m_langBtn = new QPushButton(this);
    m_langBtn->setCursor(Qt::PointingHandCursor);
    m_langBtn->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-locale")));
    m_langBtn->setStyleSheet(QStringLiteral(
        "QPushButton { padding: 6px 10px; border-radius: 6px; border: 1px solid palette(mid); background-color: palette(button); }"
        "QPushButton:hover { background-color: palette(light); }"
    ));
    headerLayout->addWidget(m_langBtn);

    // Pulsante Guida HTML
    m_helpBtn = new QPushButton(this);
    m_helpBtn->setCursor(Qt::PointingHandCursor);
    m_helpBtn->setIcon(QIcon::fromTheme(QStringLiteral("help-browser")));
    m_helpBtn->setStyleSheet(QStringLiteral(
        "QPushButton { padding: 6px 10px; border-radius: 6px; border: 1px solid palette(mid); background-color: palette(button); }"
        "QPushButton:hover { background-color: palette(light); }"
    ));
    connect(m_helpBtn, &QPushButton::clicked, this, &MainWindow::onOpenHelpClicked);
    headerLayout->addWidget(m_helpBtn);

    mainLayout->addLayout(headerLayout);

    // Selezione Azione (Spegni, Riavvia, Sospendi)
    auto *actionLayout = new QHBoxLayout();
    m_actionLabel = new QLabel(this);
    QFont boldFont = m_actionLabel->font();
    boldFont.setBold(true);
    m_actionLabel->setFont(boldFont);

    m_actionCombo = new QComboBox(this);
    m_actionCombo->setMinimumHeight(32);
    actionLayout->addWidget(m_actionLabel);
    actionLayout->addWidget(m_actionCombo, 1);
    mainLayout->addLayout(actionLayout);

    // Tab Widget
    m_tabWidget = new QTabWidget(this);

    // TAB 1: Conto alla Rovescia
    auto *countdownTab = new QWidget();
    auto *countdownTabLayout = new QVBoxLayout(countdownTab);
    countdownTabLayout->setContentsMargins(12, 14, 12, 14);
    countdownTabLayout->setSpacing(10);

    auto *spinLayout = new QHBoxLayout();
    m_minsLabel = new QLabel(countdownTab);
    m_minsLabel->setFont(boldFont);

    m_minutesSpinBox = new QSpinBox(countdownTab);
    m_minutesSpinBox->setRange(1, 10080);
    m_minutesSpinBox->setValue(30);
    m_minutesSpinBox->setMinimumHeight(32);
    m_minutesSpinBox->setAlignment(Qt::AlignCenter);
    QFont spinFont = m_minutesSpinBox->font();
    spinFont.setPointSize(12);
    m_minutesSpinBox->setFont(spinFont);

    spinLayout->addWidget(m_minsLabel);
    spinLayout->addWidget(m_minutesSpinBox, 1);
    countdownTabLayout->addLayout(spinLayout);

    // Pulsanti rapidi (+15m, +30m, +1h, +2h)
    auto *quickButtonsLayout = new QHBoxLayout();
    auto addQuickBtn = [this, quickButtonsLayout](const QString &text, int minutes) {
        auto *btn = new QPushButton(text, this);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, minutes]() {
            addMinutesToTimer(minutes);
        });
        quickButtonsLayout->addWidget(btn);
    };
    addQuickBtn(QStringLiteral("+15m"), 15);
    addQuickBtn(QStringLiteral("+30m"), 30);
    addQuickBtn(QStringLiteral("+1h"), 60);
    addQuickBtn(QStringLiteral("+2h"), 120);
    countdownTabLayout->addLayout(quickButtonsLayout);

    m_tabWidget->addTab(countdownTab, QString());

    // TAB 2: Orario Specifico
    auto *timeTab = new QWidget();
    auto *timeTabLayout = new QVBoxLayout(timeTab);
    timeTabLayout->setContentsMargins(12, 14, 12, 14);
    timeTabLayout->setSpacing(10);

    auto *exactTimeLayout = new QHBoxLayout();
    m_exactTimeLabel = new QLabel(timeTab);
    m_exactTimeLabel->setFont(boldFont);

    m_timeEdit = new QTimeEdit(timeTab);
    m_timeEdit->setDisplayFormat(QStringLiteral("HH:mm"));
    m_timeEdit->setTime(QTime::currentTime().addSecs(1800));
    m_timeEdit->setMinimumHeight(32);
    m_timeEdit->setAlignment(Qt::AlignCenter);
    QFont timeFont = m_timeEdit->font();
    timeFont.setPointSize(12);
    m_timeEdit->setFont(timeFont);

    exactTimeLayout->addWidget(m_exactTimeLabel);
    exactTimeLayout->addWidget(m_timeEdit, 1);
    timeTabLayout->addLayout(exactTimeLayout);

    m_targetTimeInfoLabel = new QLabel(timeTab);
    m_targetTimeInfoLabel->setStyleSheet(QStringLiteral("color: #718096; font-style: italic;"));
    m_targetTimeInfoLabel->setAlignment(Qt::AlignCenter);
    timeTabLayout->addWidget(m_targetTimeInfoLabel);

    connect(m_timeEdit, &QTimeEdit::timeChanged, this, &MainWindow::onTimeEditChanged);

    m_tabWidget->addTab(timeTab, QString());
    mainLayout->addWidget(m_tabWidget);

    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // Box Visualizzazione Countdown
    auto *displayFrame = new QFrame(this);
    displayFrame->setFrameShape(QFrame::StyledPanel);
    displayFrame->setStyleSheet(QStringLiteral(
        "QFrame { background-color: palette(alternate-base); border-radius: 8px; padding: 12px; }"
    ));
    auto *displayLayout = new QVBoxLayout(displayFrame);
    displayLayout->setSpacing(8);

    m_statusLabel = new QLabel(displayFrame);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(10);
    m_statusLabel->setFont(statusFont);
    displayLayout->addWidget(m_statusLabel);

    m_countdownLabel = new QLabel(QStringLiteral("00:00:00"), displayFrame);
    m_countdownLabel->setAlignment(Qt::AlignCenter);
    QFont timerFont = m_countdownLabel->font();
    timerFont.setPointSize(28);
    timerFont.setBold(true);
    m_countdownLabel->setFont(timerFont);
    displayLayout->addWidget(m_countdownLabel);

    m_progressBar = new QProgressBar(displayFrame);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(10);
    displayLayout->addWidget(m_progressBar);

    mainLayout->addWidget(displayFrame);

    // Opzioni Checkbox
    auto *optionsLayout = new QHBoxLayout();
    m_minimizeToTrayCheck = new QCheckBox(this);
    m_minimizeToTrayCheck->setChecked(true);
    optionsLayout->addWidget(m_minimizeToTrayCheck);

    m_dryRunCheck = new QCheckBox(this);
    optionsLayout->addWidget(m_dryRunCheck);
    mainLayout->addLayout(optionsLayout);

    // Pulsanti Principali: Avvia e Annulla
    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(12);

    m_startBtn = new QPushButton(this);
    m_startBtn->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    m_startBtn->setMinimumHeight(42);
    QFont btnFont = m_startBtn->font();
    btnFont.setPointSize(11);
    btnFont.setBold(true);
    m_startBtn->setFont(btnFont);
    m_startBtn->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #2b8a3e; color: white; border-radius: 6px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #2f9e44; }"
        "QPushButton:pressed { background-color: #237032; }"
        "QPushButton:disabled { background-color: #8ce99a; color: #f1f3f5; }"
    ));
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    m_cancelBtn = new QPushButton(this);
    m_cancelBtn->setIcon(QIcon::fromTheme(QStringLiteral("process-stop")));
    m_cancelBtn->setMinimumHeight(42);
    m_cancelBtn->setFont(btnFont);
    m_cancelBtn->setEnabled(false);
    m_cancelBtn->setStyleSheet(QStringLiteral(
        "QPushButton { background-color: #c92a2a; color: white; border-radius: 6px; padding: 8px 16px; }"
        "QPushButton:hover { background-color: #e03131; }"
        "QPushButton:pressed { background-color: #a51d24; }"
        "QPushButton:disabled { background-color: #ffa8a8; color: #f1f3f5; }"
    ));
    connect(m_cancelBtn, &QPushButton::clicked, this, &MainWindow::onCancelClicked);

    buttonsLayout->addWidget(m_startBtn, 2);
    buttonsLayout->addWidget(m_cancelBtn, 1);
    mainLayout->addLayout(buttonsLayout);
}

void MainWindow::setupTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(QStringLiteral(":/icons/qshutdown.svg")));

    auto *trayMenu = new QMenu(this);
    m_trayShowAction = trayMenu->addAction(QString());
    connect(m_trayShowAction, &QAction::triggered, this, [this]() {
        showNormal();
        activateWindow();
    });

    m_trayCancelAction = trayMenu->addAction(QString());
    connect(m_trayCancelAction, &QAction::triggered, this, &MainWindow::onCancelClicked);

    trayMenu->addSeparator();

    m_trayQuitAction = trayMenu->addAction(QString());
    connect(m_trayQuitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(trayMenu);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    m_trayIcon->show();
}

void MainWindow::setupLanguageMenu()
{
    m_langMenu = new QMenu(this);
    auto *group = new QActionGroup(this);
    group->setExclusive(true);

    for (const auto &info : LanguageManager::instance().supportedLanguages()) {
        auto *action = m_langMenu->addAction(QStringLiteral("%1  %2").arg(info.flag, info.name));
        action->setCheckable(true);
        action->setData(info.code);
        group->addAction(action);

        connect(action, &QAction::triggered, this, [this, action]() {
            onLanguageActionTriggered(action->data().toString());
        });
    }

    m_langBtn->setMenu(m_langMenu);
}

void MainWindow::onLanguageActionTriggered(const QString &langCode)
{
    LanguageManager::instance().setLanguage(langCode);
}

void MainWindow::onOpenHelpClicked()
{
    if (!m_helpDialog) {
        m_helpDialog = new HelpDialog(this);
    }
    m_helpDialog->show();
    m_helpDialog->raise();
    m_helpDialog->activateWindow();
}

void MainWindow::retranslateUi()
{
    auto &lm = LanguageManager::instance();

    setWindowTitle(lm.text(QStringLiteral("app_title")));
    m_titleLabel->setText(lm.text(QStringLiteral("header_title")));
    m_actionLabel->setText(lm.text(QStringLiteral("action_label")));

    // Aggiorna voci combobox azione preservando selezione
    int currentActionIdx = m_actionCombo->currentIndex();
    if (currentActionIdx < 0) currentActionIdx = 0;
    m_actionCombo->blockSignals(true);
    m_actionCombo->clear();
    m_actionCombo->addItem(QIcon::fromTheme(QStringLiteral("system-shutdown")),
                           lm.text(QStringLiteral("action_poweroff")),
                           static_cast<int>(ShutdownManager::Action::PowerOff));
    m_actionCombo->addItem(QIcon::fromTheme(QStringLiteral("system-reboot")),
                           lm.text(QStringLiteral("action_reboot")),
                           static_cast<int>(ShutdownManager::Action::Reboot));
    m_actionCombo->addItem(QIcon::fromTheme(QStringLiteral("system-suspend")),
                           lm.text(QStringLiteral("action_suspend")),
                           static_cast<int>(ShutdownManager::Action::Suspend));
    m_actionCombo->setCurrentIndex(currentActionIdx);
    m_actionCombo->blockSignals(false);

    // Tab e label
    m_tabWidget->setTabText(0, lm.text(QStringLiteral("tab_countdown")));
    m_tabWidget->setTabText(1, lm.text(QStringLiteral("tab_exact_time")));
    m_minsLabel->setText(lm.text(QStringLiteral("time_minutes_label")));
    m_minutesSpinBox->setSuffix(lm.text(QStringLiteral("minutes_suffix")));
    m_exactTimeLabel->setText(lm.text(QStringLiteral("time_exact_label")));

    // Opzioni e bottoni
    m_minimizeToTrayCheck->setText(lm.text(QStringLiteral("opt_minimize_tray")));
    m_dryRunCheck->setText(lm.text(QStringLiteral("opt_test_mode")));
    m_dryRunCheck->setToolTip(lm.text(QStringLiteral("opt_test_tooltip")));
    m_startBtn->setText(lm.text(QStringLiteral("btn_start")));
    m_cancelBtn->setText(lm.text(QStringLiteral("btn_cancel")));
    m_helpBtn->setText(QStringLiteral(" %1").arg(lm.text(QStringLiteral("btn_guide"))));

    // Lingua corrente nel pulsante
    QString currentCode = lm.currentLanguage();
    for (const auto &info : lm.supportedLanguages()) {
        if (info.code == currentCode) {
            m_langBtn->setText(QStringLiteral(" %1 %2").arg(info.flag, info.name));
            break;
        }
    }

    // Sincronizza spunta menu lingua
    for (auto *act : m_langMenu->actions()) {
        if (act->data().toString() == currentCode) {
            act->setChecked(true);
        }
    }

    // Tray Menu
    if (m_trayShowAction) m_trayShowAction->setText(lm.text(QStringLiteral("tray_show")));
    if (m_trayCancelAction) m_trayCancelAction->setText(lm.text(QStringLiteral("tray_cancel")));
    if (m_trayQuitAction) m_trayQuitAction->setText(lm.text(QStringLiteral("tray_quit")));

    if (!m_isRunning) {
        m_statusLabel->setText(lm.text(QStringLiteral("status_waiting")));
        if (m_trayIcon) m_trayIcon->setToolTip(lm.text(QStringLiteral("tray_idle")));
    } else {
        auto action = static_cast<ShutdownManager::Action>(m_actionCombo->currentData().toInt());
        m_statusLabel->setText(lm.text(QStringLiteral("status_scheduled"))
                                   .arg(ShutdownManager::actionName(action),
                                        m_targetDateTime.toString(QStringLiteral("HH:mm:ss (dd/MM)"))));
    }

    updateScheduledTimeInfo();
    updateCountdownDisplay();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible()) {
            hide();
        } else {
            showNormal();
            activateWindow();
        }
    }
}

void MainWindow::addMinutesToTimer(int mins)
{
    m_minutesSpinBox->setValue(m_minutesSpinBox->value() + mins);
}

void MainWindow::onTabChanged(int)
{
    updateScheduledTimeInfo();
}

void MainWindow::onTimeEditChanged(const QTime &)
{
    updateScheduledTimeInfo();
}

void MainWindow::updateScheduledTimeInfo()
{
    auto &lm = LanguageManager::instance();
    QDateTime now = QDateTime::currentDateTime();
    QTime pickedTime = m_timeEdit->time();
    QDateTime scheduled(now.date(), pickedTime);

    if (scheduled <= now) {
        scheduled = scheduled.addDays(1);
    }

    qint64 diffSecs = now.secsTo(scheduled);
    int hours = static_cast<int>(diffSecs / 3600);
    int minutes = static_cast<int>((diffSecs % 3600) / 60);

    QString dayStr = (scheduled.date() == now.date()) ? lm.text(QStringLiteral("today")) : lm.text(QStringLiteral("tomorrow"));
    m_targetTimeInfoLabel->setText(
        lm.text(QStringLiteral("scheduled_for"))
            .arg(dayStr)
            .arg(pickedTime.toString(QStringLiteral("HH:mm")))
            .arg(hours)
            .arg(minutes)
    );
}

void MainWindow::onStartClicked()
{
    auto &lm = LanguageManager::instance();
    QDateTime now = QDateTime::currentDateTime();

    if (m_tabWidget->currentIndex() == 0) {
        int mins = m_minutesSpinBox->value();
        m_totalSeconds = static_cast<qint64>(mins) * 60;
        m_targetDateTime = now.addSecs(m_totalSeconds);
    } else {
        QTime pickedTime = m_timeEdit->time();
        m_targetDateTime = QDateTime(now.date(), pickedTime);
        if (m_targetDateTime <= now) {
            m_targetDateTime = m_targetDateTime.addDays(1);
        }
        m_totalSeconds = now.secsTo(m_targetDateTime);
    }

    if (m_totalSeconds <= 0) {
        QMessageBox::warning(this, lm.text(QStringLiteral("error_title")), lm.text(QStringLiteral("error_zero_time")));
        return;
    }

    m_remainingSeconds = m_totalSeconds;
    m_isRunning = true;
    m_warned60s = false;
    m_shutdownManager.setDryRun(m_dryRunCheck->isChecked());

    lockControls(true);
    updateCountdownDisplay();

    m_countdownTimer.start(1000);

    auto action = static_cast<ShutdownManager::Action>(m_actionCombo->currentData().toInt());
    QString actionName = ShutdownManager::actionName(action);

    QString msg = lm.text(QStringLiteral("status_scheduled"))
                      .arg(actionName, m_targetDateTime.toString(QStringLiteral("HH:mm:ss (dd/MM)")));

    m_statusLabel->setText(msg);

    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(lm.text(QStringLiteral("tray_active")), msg, QSystemTrayIcon::Information, 3000);
    }

    if (m_minimizeToTrayCheck->isChecked()) {
        hide();
    }
}

void MainWindow::onCancelClicked()
{
    if (!m_isRunning) return;

    auto &lm = LanguageManager::instance();
    m_countdownTimer.stop();
    m_isRunning = false;
    lockControls(false);

    m_statusLabel->setText(lm.text(QStringLiteral("status_canceled")));
    m_progressBar->setValue(0);
    updateCountdownDisplay();

    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->setToolTip(lm.text(QStringLiteral("tray_idle")));
        m_trayIcon->showMessage(QStringLiteral("QShut"), lm.text(QStringLiteral("status_canceled")), QSystemTrayIcon::Warning, 3000);
    }
}

void MainWindow::onTimerTick()
{
    if (!m_isRunning) return;

    QDateTime now = QDateTime::currentDateTime();
    m_remainingSeconds = now.secsTo(m_targetDateTime);

    if (m_remainingSeconds <= 0) {
        m_remainingSeconds = 0;
        updateCountdownDisplay();
        m_countdownTimer.stop();
        triggerAction();
        return;
    }

    updateCountdownDisplay();

    // Avviso di sicurezza quando mancano 60 secondi
    if (m_remainingSeconds <= 60 && !m_warned60s) {
        m_warned60s = true;
        auto &lm = LanguageManager::instance();
        auto action = static_cast<ShutdownManager::Action>(m_actionCombo->currentData().toInt());
        QString warnMsg = lm.text(QStringLiteral("warn_msg")).arg(ShutdownManager::actionName(action));
        if (m_trayIcon && m_trayIcon->isVisible()) {
            m_trayIcon->showMessage(lm.text(QStringLiteral("warn_title")), warnMsg, QSystemTrayIcon::Warning, 10000);
        }
        showNormal();
        activateWindow();
    }
}

void MainWindow::updateCountdownDisplay()
{
    auto &lm = LanguageManager::instance();
    if (!m_isRunning) {
        m_countdownLabel->setText(QStringLiteral("--:--:--"));
        m_progressBar->setValue(0);
        if (m_trayIcon) {
            m_trayIcon->setToolTip(lm.text(QStringLiteral("tray_idle")));
        }
        return;
    }

    qint64 secs = qMax<qint64>(0, m_remainingSeconds);
    qint64 hours = secs / 3600;
    qint64 minutes = (secs % 3600) / 60;
    qint64 seconds = secs % 60;

    QString timeStr = QStringLiteral("%1:%2:%3")
                          .arg(hours, 2, 10, QLatin1Char('0'))
                          .arg(minutes, 2, 10, QLatin1Char('0'))
                          .arg(seconds, 2, 10, QLatin1Char('0'));

    m_countdownLabel->setText(timeStr);

    if (m_totalSeconds > 0) {
        int progress = static_cast<int>(100 - (secs * 100 / m_totalSeconds));
        m_progressBar->setValue(qBound(0, progress, 100));
    }

    if (m_trayIcon) {
        auto action = static_cast<ShutdownManager::Action>(m_actionCombo->currentData().toInt());
        m_trayIcon->setToolTip(QStringLiteral("%1: %2").arg(ShutdownManager::actionName(action), timeStr));
    }
}

void MainWindow::lockControls(bool locked)
{
    m_tabWidget->setEnabled(!locked);
    m_actionCombo->setEnabled(!locked);
    m_dryRunCheck->setEnabled(!locked);
    m_startBtn->setEnabled(!locked);
    m_cancelBtn->setEnabled(locked);
}

void MainWindow::triggerAction()
{
    auto &lm = LanguageManager::instance();
    m_isRunning = false;
    lockControls(false);

    auto action = static_cast<ShutdownManager::Action>(m_actionCombo->currentData().toInt());
    m_statusLabel->setText(lm.text(QStringLiteral("status_executing")).arg(ShutdownManager::actionName(action)));

    m_shutdownManager.executeAction(action);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_isRunning) {
        auto &lm = LanguageManager::instance();
        QMessageBox::StandardButton resBtn = QMessageBox::question(
            this,
            lm.text(QStringLiteral("confirm_close_title")),
            lm.text(QStringLiteral("confirm_close_msg")),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
            QMessageBox::Yes
        );

        if (resBtn == QMessageBox::Yes) {
            event->ignore();
            hide();
            return;
        } else if (resBtn == QMessageBox::No) {
            onCancelClicked();
            event->accept();
            return;
        } else {
            event->ignore();
            return;
        }
    }

    event->accept();
}

void MainWindow::setDryRunDefault(bool dryRun)
{
    if (m_dryRunCheck) {
        m_dryRunCheck->setChecked(dryRun);
    }
}
