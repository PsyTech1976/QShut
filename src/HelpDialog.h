#pragma once

#include <QDialog>
#include <QString>

class QTextBrowser;
class QComboBox;
class QLabel;
class QPushButton;

class HelpDialog : public QDialog {
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog() override = default;

public slots:
    void loadGuide(const QString &langCode);
    void retranslateUi();

private slots:
    void onLangComboChanged(int index);

private:
    void setupUi();

    QTextBrowser *m_textBrowser = nullptr;
    QComboBox *m_langCombo = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_langLabel = nullptr;
    QPushButton *m_closeBtn = nullptr;
};
