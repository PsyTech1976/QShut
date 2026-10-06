#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QSystemTrayIcon>
#include "ShutdownManager.h"

class QTabWidget;
class QSpinBox;
class QTimeEdit;
class QComboBox;
class QLabel;
class QPushButton;
class QToolButton;
class QProgressBar;
class QCheckBox;
class QCloseEvent;
class QMenu;
class QAction;
class HelpDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setDryRunDefault(bool dryRun);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onStartClicked();
    void onCancelClicked();
    void onTimerTick();
    void onTabChanged(int index);
    void onTimeEditChanged(const QTime &time);
    void addMinutesToTimer(int mins);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onLanguageActionTriggered(const QString &langCode);
    void onOpenHelpClicked();

public slots:
    void retranslateUi();

private:
    void setupUi();
    void setupTrayIcon();
    void setupLanguageMenu();
    void updateCountdownDisplay();
    void updateScheduledTimeInfo();
    void lockControls(bool locked);
    void triggerAction();

    // UI Widgets
    QLabel *m_titleLabel = nullptr;
    QLabel *m_actionLabel = nullptr;
    QComboBox *m_actionCombo = nullptr;
    QTabWidget *m_tabWidget = nullptr;
    QLabel *m_minsLabel = nullptr;
    QSpinBox *m_minutesSpinBox = nullptr;
    QLabel *m_exactTimeLabel = nullptr;
    QTimeEdit *m_timeEdit = nullptr;
    QLabel *m_targetTimeInfoLabel = nullptr;
    QLabel *m_countdownLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QPushButton *m_startBtn = nullptr;
    QPushButton *m_cancelBtn = nullptr;
    QPushButton *m_helpBtn = nullptr;
    QPushButton *m_langBtn = nullptr;
    QMenu *m_langMenu = nullptr;
    QCheckBox *m_dryRunCheck = nullptr;
    QCheckBox *m_minimizeToTrayCheck = nullptr;

    // Tray
    QSystemTrayIcon *m_trayIcon = nullptr;
    QAction *m_trayShowAction = nullptr;
    QAction *m_trayCancelAction = nullptr;
    QAction *m_trayQuitAction = nullptr;

    // Help Dialog
    HelpDialog *m_helpDialog = nullptr;

    // Logic
    ShutdownManager m_shutdownManager;
    QTimer m_countdownTimer;
    QDateTime m_targetDateTime;
    qint64 m_totalSeconds = 0;
    qint64 m_remainingSeconds = 0;
    bool m_isRunning = false;
    bool m_warned60s = false;
};
