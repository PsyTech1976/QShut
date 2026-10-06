#pragma once

#include <QString>
#include <QObject>

class ShutdownManager : public QObject {
    Q_OBJECT
public:
    enum class Action {
        PowerOff,
        Reboot,
        Suspend
    };
    Q_ENUM(Action)

    explicit ShutdownManager(QObject *parent = nullptr);

    bool executeAction(Action action);
    void setDryRun(bool enable);
    bool isDryRun() const;

    static QString actionName(Action action);

signals:
    void actionExecuted(Action action, bool success, const QString &message);

private:
    bool executeViaDBus(Action action);
    bool executeViaSystemctl(Action action);
    bool executeViaShutdownCmd(Action action);

    bool m_dryRun = false;
};
