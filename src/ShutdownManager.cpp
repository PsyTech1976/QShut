#include "ShutdownManager.h"
#include "LanguageManager.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QProcess>
#include <QDebug>

ShutdownManager::ShutdownManager(QObject *parent)
    : QObject(parent)
{
}

void ShutdownManager::setDryRun(bool enable)
{
    m_dryRun = enable;
}

bool ShutdownManager::isDryRun() const
{
    return m_dryRun;
}

QString ShutdownManager::actionName(Action action)
{
    auto &lm = LanguageManager::instance();
    switch (action) {
    case Action::PowerOff:
        return lm.text(QStringLiteral("action_poweroff_name"));
    case Action::Reboot:
        return lm.text(QStringLiteral("action_reboot_name"));
    case Action::Suspend:
        return lm.text(QStringLiteral("action_suspend_name"));
    }
    return QStringLiteral("Action");
}

bool ShutdownManager::executeAction(Action action)
{
    auto &lm = LanguageManager::instance();
    if (m_dryRun) {
        qDebug() << "[DRY RUN] Would execute:" << actionName(action);
        QString msg = lm.text(QStringLiteral("test_completed_msg")).arg(actionName(action));
        emit actionExecuted(action, true, msg);
        return true;
    }

    // 1. D-Bus verso systemd-logind
    if (executeViaDBus(action)) {
        emit actionExecuted(action, true, QStringLiteral("D-Bus logind OK"));
        return true;
    }

    // 2. Fallback su systemctl
    if (executeViaSystemctl(action)) {
        emit actionExecuted(action, true, QStringLiteral("systemctl OK"));
        return true;
    }

    // 3. Fallback su shutdown command
    if (executeViaShutdownCmd(action)) {
        emit actionExecuted(action, true, QStringLiteral("shutdown command OK"));
        return true;
    }

    emit actionExecuted(action, false, lm.text(QStringLiteral("error_title")));
    return false;
}

bool ShutdownManager::executeViaDBus(Action action)
{
    QDBusInterface loginInterface(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1"),
        QStringLiteral("org.freedesktop.login1.Manager"),
        QDBusConnection::systemBus()
    );

    if (!loginInterface.isValid()) {
        qWarning() << "D-Bus login1 manager interface not valid";
        return false;
    }

    QString method;
    switch (action) {
    case Action::PowerOff:
        method = QStringLiteral("PowerOff");
        break;
    case Action::Reboot:
        method = QStringLiteral("Reboot");
        break;
    case Action::Suspend:
        method = QStringLiteral("Suspend");
        break;
    }

    QDBusReply<void> reply = loginInterface.call(method, true);
    if (!reply.isValid()) {
        qWarning() << "D-Bus call to" << method << "failed:" << reply.error().message();
        return false;
    }

    return true;
}

bool ShutdownManager::executeViaSystemctl(Action action)
{
    QString subcmd;
    switch (action) {
    case Action::PowerOff:
        subcmd = QStringLiteral("poweroff");
        break;
    case Action::Reboot:
        subcmd = QStringLiteral("reboot");
        break;
    case Action::Suspend:
        subcmd = QStringLiteral("suspend");
        break;
    }

    return QProcess::startDetached(QStringLiteral("systemctl"), {subcmd});
}

bool ShutdownManager::executeViaShutdownCmd(Action action)
{
    QStringList args;
    switch (action) {
    case Action::PowerOff:
        args << QStringLiteral("-h") << QStringLiteral("now");
        break;
    case Action::Reboot:
        args << QStringLiteral("-r") << QStringLiteral("now");
        break;
    case Action::Suspend:
        return false;
    }

    return QProcess::startDetached(QStringLiteral("shutdown"), args);
}
