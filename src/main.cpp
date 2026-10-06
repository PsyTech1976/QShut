#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("qshutdown"));
    app.setApplicationDisplayName(QStringLiteral("QShut"));
    app.setApplicationVersion(QStringLiteral("1.1.0"));
    app.setOrganizationName(QStringLiteral("QShutProject"));

    // Apparenza moderna
    if (QStyleFactory::keys().contains(QStringLiteral("Fusion"), Qt::CaseInsensitive)) {
        app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("QShut - Programmatore di spegnimento per Linux"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption testOption(QStringList() << QStringLiteral("t") << QStringLiteral("test"),
                                 QObject::tr("Avvia con modalità simulazione pre-abilitata (non esegue spegnimenti reali)."));
    parser.addOption(testOption);

    parser.process(app);

    MainWindow window;
    if (parser.isSet(testOption)) {
        window.setDryRunDefault(true);
    }

    window.show();
    return app.exec();
}
