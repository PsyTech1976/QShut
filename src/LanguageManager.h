#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QPair>

class LanguageManager : public QObject {
    Q_OBJECT

public:
    static LanguageManager &instance();

    struct LanguageInfo {
        QString code;
        QString name;
        QString flag;
    };

    QList<LanguageInfo> supportedLanguages() const;
    QString currentLanguage() const;
    void setLanguage(const QString &code);

    QString text(const QString &key) const;
    QString guideHtmlPath() const;

    // Helper per rilevazione iniziale
    static QString detectInitialLanguage();

signals:
    void languageChanged(const QString &code);

private:
    explicit LanguageManager(QObject *parent = nullptr);
    void initDictionary();

    QString m_currentLang;
    // Dizionario: [key][langCode] -> traduzione
    QMap<QString, QMap<QString, QString>> m_dict;
};
