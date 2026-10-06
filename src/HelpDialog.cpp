#include "HelpDialog.h"
#include "LanguageManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QIcon>
#include <QFont>

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowIcon(QIcon(QStringLiteral(":/icons/qshutdown.svg")));
    resize(680, 560);
    setMinimumSize(540, 420);

    setupUi();
    retranslateUi();

    connect(&LanguageManager::instance(), &LanguageManager::languageChanged, this, [this](const QString &code) {
        retranslateUi();
        loadGuide(code);
    });

    loadGuide(LanguageManager::instance().currentLanguage());
}

void HelpDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Barra superiore con titolo e selettore lingua
    auto *topLayout = new QHBoxLayout();

    auto *iconLabel = new QLabel(this);
    iconLabel->setPixmap(QIcon(QStringLiteral(":/icons/qshutdown.svg")).pixmap(28, 28));
    topLayout->addWidget(iconLabel);

    m_titleLabel = new QLabel(this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    topLayout->addWidget(m_titleLabel);

    topLayout->addStretch();

    m_langLabel = new QLabel(this);
    topLayout->addWidget(m_langLabel);

    m_langCombo = new QComboBox(this);
    for (const auto &info : LanguageManager::instance().supportedLanguages()) {
        m_langCombo->addItem(QStringLiteral("%1  %2").arg(info.flag, info.name), info.code);
    }
    m_langCombo->setMinimumWidth(140);
    topLayout->addWidget(m_langCombo);

    connect(m_langCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HelpDialog::onLangComboChanged);
    mainLayout->addLayout(topLayout);

    // Visualizzatore HTML
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(true);
    m_textBrowser->setStyleSheet(QStringLiteral(
        "QTextBrowser { border: 1px solid palette(mid); border-radius: 6px; padding: 10px; background-color: palette(base); }"
    ));
    mainLayout->addWidget(m_textBrowser, 1);

    // Barra inferiore con pulsante Chiudi
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    m_closeBtn = new QPushButton(this);
    m_closeBtn->setMinimumWidth(100);
    m_closeBtn->setMinimumHeight(32);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    bottomLayout->addWidget(m_closeBtn);

    mainLayout->addLayout(bottomLayout);
}

void HelpDialog::retranslateUi()
{
    auto &lm = LanguageManager::instance();
    setWindowTitle(lm.text(QStringLiteral("guide_window_title")));
    m_titleLabel->setText(lm.text(QStringLiteral("guide_window_title")));
    m_langLabel->setText(lm.text(QStringLiteral("guide_lang_label")));
    m_closeBtn->setText(lm.text(QStringLiteral("guide_close_btn")));

    // Sincronizza combobox
    QString current = lm.currentLanguage();
    for (int i = 0; i < m_langCombo->count(); ++i) {
        if (m_langCombo->itemData(i).toString() == current) {
            m_langCombo->blockSignals(true);
            m_langCombo->setCurrentIndex(i);
            m_langCombo->blockSignals(false);
            break;
        }
    }
}

void HelpDialog::onLangComboChanged(int index)
{
    QString langCode = m_langCombo->itemData(index).toString();
    loadGuide(langCode);
}

void HelpDialog::loadGuide(const QString &langCode)
{
    QString path = QStringLiteral(":/help/guide_%1.html").arg(langCode);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_textBrowser->setHtml(QString::fromUtf8(file.readAll()));
        file.close();
    } else {
        m_textBrowser->setPlainText(QStringLiteral("Guida non disponibile per la lingua selezionata."));
    }
}
