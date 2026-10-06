#include "LanguageManager.h"
#include <QSettings>
#include <QLocale>

LanguageManager &LanguageManager::instance()
{
    static LanguageManager s_instance;
    return s_instance;
}

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
    initDictionary();
    m_currentLang = detectInitialLanguage();
}

QList<LanguageManager::LanguageInfo> LanguageManager::supportedLanguages() const
{
    return {
        {QStringLiteral("en"), QStringLiteral("English"), QStringLiteral("🇬🇧")},
        {QStringLiteral("it"), QStringLiteral("Italiano"), QStringLiteral("🇮🇹")},
        {QStringLiteral("fr"), QStringLiteral("Français"), QStringLiteral("🇫🇷")},
        {QStringLiteral("de"), QStringLiteral("Deutsch"), QStringLiteral("🇩🇪")},
        {QStringLiteral("es"), QStringLiteral("Español"), QStringLiteral("🇪🇸")}
    };
}

QString LanguageManager::detectInitialLanguage()
{
    QSettings settings(QStringLiteral("QShutProject"), QStringLiteral("qshutdown"));
    QString saved = settings.value(QStringLiteral("language")).toString().trimmed().toLower();
    if (saved == QLatin1String("en") || saved == QLatin1String("it") ||
        saved == QLatin1String("fr") || saved == QLatin1String("de") ||
        saved == QLatin1String("es")) {
        return saved;
    }

    // Rileva lingua del sistema operativo
    QString sysLang = QLocale::system().name().left(2).toLower();
    if (sysLang == QLatin1String("it")) return QStringLiteral("it");
    if (sysLang == QLatin1String("fr")) return QStringLiteral("fr");
    if (sysLang == QLatin1String("de")) return QStringLiteral("de");
    if (sysLang == QLatin1String("es")) return QStringLiteral("es");

    // Default: Inglese in assenza o lingua non supportata
    return QStringLiteral("en");
}

QString LanguageManager::currentLanguage() const
{
    return m_currentLang;
}

void LanguageManager::setLanguage(const QString &code)
{
    QString clean = code.trimmed().toLower();
    if (clean != QLatin1String("en") && clean != QLatin1String("it") &&
        clean != QLatin1String("fr") && clean != QLatin1String("de") &&
        clean != QLatin1String("es")) {
        clean = QStringLiteral("en");
    }

    if (m_currentLang != clean) {
        m_currentLang = clean;
        QSettings settings(QStringLiteral("QShutProject"), QStringLiteral("qshutdown"));
        settings.setValue(QStringLiteral("language"), m_currentLang);
        emit languageChanged(m_currentLang);
    }
}

QString LanguageManager::text(const QString &key) const
{
    if (m_dict.contains(key)) {
        const auto &translations = m_dict[key];
        if (translations.contains(m_currentLang)) {
            return translations[m_currentLang];
        }
        if (translations.contains(QStringLiteral("en"))) {
            return translations[QStringLiteral("en")];
        }
    }
    return key;
}

QString LanguageManager::guideHtmlPath() const
{
    return QStringLiteral(":/help/guide_%1.html").arg(m_currentLang);
}

void LanguageManager::initDictionary()
{
    auto add = [this](const QString &key,
                      const QString &en,
                      const QString &it,
                      const QString &fr,
                      const QString &de,
                      const QString &es) {
        m_dict[key][QStringLiteral("en")] = en;
        m_dict[key][QStringLiteral("it")] = it;
        m_dict[key][QStringLiteral("fr")] = fr;
        m_dict[key][QStringLiteral("de")] = de;
        m_dict[key][QStringLiteral("es")] = es;
    };

    add(QStringLiteral("app_title"),
        QStringLiteral("QShut - Shutdown Timer"),
        QStringLiteral("QShut - Spegnimento Programmato"),
        QStringLiteral("QShut - Programmateur d'extinction"),
        QStringLiteral("QShut - Herunterfahren-Planer"),
        QStringLiteral("QShut - Temporizador de Apagado"));

    add(QStringLiteral("header_title"),
        QStringLiteral("Schedule Shutdown"),
        QStringLiteral("Pianifica Spegnimento"),
        QStringLiteral("Programmer l'extinction"),
        QStringLiteral("Herunterfahren planen"),
        QStringLiteral("Programar Apagado"));

    add(QStringLiteral("action_label"),
        QStringLiteral("Action:"),
        QStringLiteral("Azione:"),
        QStringLiteral("Action :"),
        QStringLiteral("Aktion:"),
        QStringLiteral("Acción:"));

    add(QStringLiteral("action_poweroff"),
        QStringLiteral("Shut down computer"),
        QStringLiteral("Spegni il computer"),
        QStringLiteral("Éteindre l'ordinateur"),
        QStringLiteral("Computer herunterfahren"),
        QStringLiteral("Apagar el equipo"));

    add(QStringLiteral("action_reboot"),
        QStringLiteral("Restart computer"),
        QStringLiteral("Riavvia il computer"),
        QStringLiteral("Redémarrer l'ordinateur"),
        QStringLiteral("Computer neu starten"),
        QStringLiteral("Reiniciar el equipo"));

    add(QStringLiteral("action_suspend"),
        QStringLiteral("Suspend (Sleep)"),
        QStringLiteral("Sospendi (Sleep)"),
        QStringLiteral("Mettre en veille"),
        QStringLiteral("Energie sparen (Standby)"),
        QStringLiteral("Suspender (Reposo)"));

    add(QStringLiteral("action_poweroff_name"),
        QStringLiteral("System shutdown"),
        QStringLiteral("Spegnimento del sistema"),
        QStringLiteral("Extinction du système"),
        QStringLiteral("System-Herunterfahren"),
        QStringLiteral("Apagado del sistema"));

    add(QStringLiteral("action_reboot_name"),
        QStringLiteral("System restart"),
        QStringLiteral("Riavvio del sistema"),
        QStringLiteral("Redémarrage du système"),
        QStringLiteral("System-Neustart"),
        QStringLiteral("Reinicio del sistema"));

    add(QStringLiteral("action_suspend_name"),
        QStringLiteral("System suspend"),
        QStringLiteral("Sospensione del sistema"),
        QStringLiteral("Mise en veille"),
        QStringLiteral("System-Ruhezustand"),
        QStringLiteral("Suspensión del sistema"));

    add(QStringLiteral("tab_countdown"),
        QStringLiteral("Countdown"),
        QStringLiteral("Conto alla Rovescia"),
        QStringLiteral("Compte à rebours"),
        QStringLiteral("Countdown"),
        QStringLiteral("Cuenta Regresiva"));

    add(QStringLiteral("tab_exact_time"),
        QStringLiteral("Specific Time"),
        QStringLiteral("Orario Specifico"),
        QStringLiteral("Heure précise"),
        QStringLiteral("Bestimmte Uhrzeit"),
        QStringLiteral("Hora Específica"));

    add(QStringLiteral("time_minutes_label"),
        QStringLiteral("Time in minutes:"),
        QStringLiteral("Tempo in minuti:"),
        QStringLiteral("Temps en minutes :"),
        QStringLiteral("Zeit in Minuten:"),
        QStringLiteral("Tiempo en minutos:"));

    add(QStringLiteral("minutes_suffix"),
        QStringLiteral(" min"),
        QStringLiteral(" min"),
        QStringLiteral(" min"),
        QStringLiteral(" Min"),
        QStringLiteral(" min"));

    add(QStringLiteral("time_exact_label"),
        QStringLiteral("Shut down at:"),
        QStringLiteral("Spegni alle ore:"),
        QStringLiteral("Éteindre à :"),
        QStringLiteral("Ausschalten um:"),
        QStringLiteral("Apagar a las:"));

    add(QStringLiteral("scheduled_for"),
        QStringLiteral("Scheduled for %1 at %2 (in %3h %4m)"),
        QStringLiteral("Pianificato per %1 alle %2 (tra %3h %4m)"),
        QStringLiteral("Prévu pour %1 à %2 (dans %3h %4m)"),
        QStringLiteral("Geplant für %1 um %2 (in %3h %4m)"),
        QStringLiteral("Programado para %1 a las %2 (en %3h %4m)"));

    add(QStringLiteral("today"),
        QStringLiteral("today"),
        QStringLiteral("oggi"),
        QStringLiteral("aujourd'hui"),
        QStringLiteral("heute"),
        QStringLiteral("hoy"));

    add(QStringLiteral("tomorrow"),
        QStringLiteral("tomorrow"),
        QStringLiteral("domani"),
        QStringLiteral("demain"),
        QStringLiteral("morgen"),
        QStringLiteral("mañana"));

    add(QStringLiteral("status_waiting"),
        QStringLiteral("Waiting..."),
        QStringLiteral("In attesa..."),
        QStringLiteral("En attente..."),
        QStringLiteral("Wartend..."),
        QStringLiteral("En espera..."));

    add(QStringLiteral("status_scheduled"),
        QStringLiteral("%1 scheduled for %2."),
        QStringLiteral("%1 programmato alle %2."),
        QStringLiteral("%1 programmé pour %2."),
        QStringLiteral("%1 geplant für %2."),
        QStringLiteral("%1 programado para las %2."));

    add(QStringLiteral("status_canceled"),
        QStringLiteral("Timer canceled."),
        QStringLiteral("Timer annullato."),
        QStringLiteral("Minuterie annulée."),
        QStringLiteral("Timer abgebrochen."),
        QStringLiteral("Temporizador cancelado."));

    add(QStringLiteral("status_executing"),
        QStringLiteral("Executing: %1..."),
        QStringLiteral("Esecuzione in corso: %1..."),
        QStringLiteral("Exécution en cours : %1..."),
        QStringLiteral("Wird ausgeführt: %1..."),
        QStringLiteral("Ejecutando: %1..."));

    add(QStringLiteral("opt_minimize_tray"),
        QStringLiteral("Minimize to system tray"),
        QStringLiteral("Riduci a icona nella barra di sistema"),
        QStringLiteral("Réduire dans la zone de notification"),
        QStringLiteral("In den Infobereich minimieren"),
        QStringLiteral("Minimizar a la bandeja del sistema"));

    add(QStringLiteral("opt_test_mode"),
        QStringLiteral("Simulation mode (Test)"),
        QStringLiteral("Modalità simulazione (Test)"),
        QStringLiteral("Mode simulation (Test)"),
        QStringLiteral("Simulationsmodus (Test)"),
        QStringLiteral("Modo simulación (Prueba)"));

    add(QStringLiteral("opt_test_tooltip"),
        QStringLiteral("Simulate timer and warning without actually shutting down PC"),
        QStringLiteral("Simula il timer e l'avviso senza spegnere realmente il PC"),
        QStringLiteral("Simule la minuterie sans éteindre le PC"),
        QStringLiteral("Simuliert den Timer, ohne den PC herunterzufahren"),
        QStringLiteral("Simula el temporizador sin apagar el equipo"));

    add(QStringLiteral("btn_start"),
        QStringLiteral("Start Timer"),
        QStringLiteral("Avvia Timer"),
        QStringLiteral("Démarrer"),
        QStringLiteral("Timer starten"),
        QStringLiteral("Iniciar Timer"));

    add(QStringLiteral("btn_cancel"),
        QStringLiteral("Cancel"),
        QStringLiteral("Annulla"),
        QStringLiteral("Annuler"),
        QStringLiteral("Abbrechen"),
        QStringLiteral("Cancelar"));

    add(QStringLiteral("btn_guide"),
        QStringLiteral("Guide"),
        QStringLiteral("Guida"),
        QStringLiteral("Guide"),
        QStringLiteral("Anleitung"),
        QStringLiteral("Guía"));

    add(QStringLiteral("btn_language"),
        QStringLiteral("Language"),
        QStringLiteral("Lingua"),
        QStringLiteral("Langue"),
        QStringLiteral("Sprache"),
        QStringLiteral("Idioma"));

    add(QStringLiteral("tray_show"),
        QStringLiteral("Show Window"),
        QStringLiteral("Mostra Finestra"),
        QStringLiteral("Afficher la fenêtre"),
        QStringLiteral("Fenster anzeigen"),
        QStringLiteral("Mostrar ventana"));

    add(QStringLiteral("tray_cancel"),
        QStringLiteral("Cancel Shutdown"),
        QStringLiteral("Annulla Spegnimento"),
        QStringLiteral("Annuler l'extinction"),
        QStringLiteral("Ausschalten abbrechen"),
        QStringLiteral("Cancelar apagado"));

    add(QStringLiteral("tray_quit"),
        QStringLiteral("Quit"),
        QStringLiteral("Esci"),
        QStringLiteral("Quitter"),
        QStringLiteral("Beenden"),
        QStringLiteral("Salir"));

    add(QStringLiteral("tray_active"),
        QStringLiteral("QShut Active"),
        QStringLiteral("QShut Attivo"),
        QStringLiteral("QShut Actif"),
        QStringLiteral("QShut Aktiv"),
        QStringLiteral("QShut Activo"));

    add(QStringLiteral("tray_idle"),
        QStringLiteral("QShut - No operation in progress"),
        QStringLiteral("QShut - Nessuna operazione in corso"),
        QStringLiteral("QShut - Aucune opération en cours"),
        QStringLiteral("QShut - Kein Vorgang aktiv"),
        QStringLiteral("QShut - Sin operaciones en curso"));

    add(QStringLiteral("warn_title"),
        QStringLiteral("Shutdown Warning"),
        QStringLiteral("Avviso Spegnimento"),
        QStringLiteral("Avertissement"),
        QStringLiteral("Warnung"),
        QStringLiteral("Aviso de Apagado"));

    add(QStringLiteral("warn_msg"),
        QStringLiteral("WARNING: %1 in 60 seconds!\nOpen QShut to cancel if needed."),
        QStringLiteral("ATTENZIONE: %1 tra 60 secondi!\nApri QShut per annullare se necessario."),
        QStringLiteral("ATTENTION : %1 dans 60 secondes !\nOuvrez QShut pour annuler si nécessaire."),
        QStringLiteral("ACHTUNG: %1 in 60 Sekunden!\nÖffnen Sie QShut zum Abbrechen."),
        QStringLiteral("ATENCIÓN: ¡%1 en 60 segundos!\nAbra QShut para cancelar si es necesario."));

    add(QStringLiteral("error_title"),
        QStringLiteral("Shutdown Error"),
        QStringLiteral("Errore Spegnimento"),
        QStringLiteral("Erreur d'extinction"),
        QStringLiteral("Fehler"),
        QStringLiteral("Error de Apagado"));

    add(QStringLiteral("error_zero_time"),
        QStringLiteral("The set time must be greater than zero!"),
        QStringLiteral("Il tempo impostato deve essere maggiore di zero!"),
        QStringLiteral("Le temps défini doit être supérieur à zéro !"),
        QStringLiteral("Die Zeit muss größer als Null sein!"),
        QStringLiteral("¡El tiempo debe ser mayor que cero!"));

    add(QStringLiteral("confirm_close_title"),
        QStringLiteral("QShut - Timer Running"),
        QStringLiteral("QShut - Timer in corso"),
        QStringLiteral("QShut - Minuterie en cours"),
        QStringLiteral("QShut - Timer läuft"),
        QStringLiteral("QShut - Temporizador en marcha"));

    add(QStringLiteral("confirm_close_msg"),
        QStringLiteral("The timer is currently running.\n\nDo you want to minimize to system tray to keep it running?"),
        QStringLiteral("Il timer è attualmente in funzione.\n\nVuoi minimizzare nella barra delle applicazioni per mantenerlo attivo?"),
        QStringLiteral("La minuterie est en cours.\n\nVoulez-vous la réduire dans la zone de notification ?"),
        QStringLiteral("Der Timer läuft.\n\nMöchten Sie ihn in den Infobereich minimieren?"),
        QStringLiteral("El temporizador está en marcha.\n\n¿Desea minimizar a la bandeja del sistema?"));

    add(QStringLiteral("test_completed_title"),
        QStringLiteral("Simulation Completed"),
        QStringLiteral("Simulazione Completata"),
        QStringLiteral("Simulation terminée"),
        QStringLiteral("Simulation abgeschlossen"),
        QStringLiteral("Simulación completada"));

    add(QStringLiteral("test_completed_msg"),
        QStringLiteral("[Test Mode] Action simulated successfully: %1"),
        QStringLiteral("[Modalità Test] Azione simulata con successo: %1"),
        QStringLiteral("[Mode Test] Action simulée avec succès : %1"),
        QStringLiteral("[Testmodus] Aktion erfolgreich simuliert: %1"),
        QStringLiteral("[Modo Prueba] Acción simulada con éxito: %1"));

    add(QStringLiteral("guide_window_title"),
        QStringLiteral("QShut - User Guide"),
        QStringLiteral("QShut - Guida Utente"),
        QStringLiteral("QShut - Guide de l'utilisateur"),
        QStringLiteral("QShut - Benutzerhandbuch"),
        QStringLiteral("QShut - Guía del Usuario"));

    add(QStringLiteral("guide_lang_label"),
        QStringLiteral("Language:"),
        QStringLiteral("Lingua:"),
        QStringLiteral("Langue :"),
        QStringLiteral("Sprache:"),
        QStringLiteral("Idioma:"));

    add(QStringLiteral("guide_close_btn"),
        QStringLiteral("Close"),
        QStringLiteral("Chiudi"),
        QStringLiteral("Fermer"),
        QStringLiteral("Schließen"),
        QStringLiteral("Cerrar"));
}
