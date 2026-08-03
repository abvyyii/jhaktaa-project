#pragma once

#include <QString>
#include <QWidget>

class QFrame;
class QLabel;
class QLineEdit;
class QMovie;
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QPushButton;
class QResizeEvent;

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget* parent = nullptr);

    void setMessage(const QString& text, bool success);
    void clearMessage();
    void clearFields();
    void setFormEnabled(bool enabled);

signals:
    void loginRequested(const QString& identifier, const QString& password);
    void registerRequested();

private:
    void buildUi();
    void applyBasePalette();
    void setupAnimatedBackground();
    void updateAnimatedBackgroundSize();
    void stylePanel(QFrame* frame);
    void styleLabel(QLabel* label, bool title = false);
    void styleLineEdit(QLineEdit* edit, bool secret = false);
    void stylePrimaryButton(QPushButton* button);
    void styleLinkButton(QPushButton* button);
    void updateMessageAppearance(bool success);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel* m_backgroundLabel;
    QMovie* m_backgroundMovie;
    QGraphicsOpacityEffect* m_backgroundOpacityEffect;
    QPropertyAnimation* m_backgroundFadeAnimation;
    QFrame* m_panel;
    QFrame* m_accentBar;
    QLabel* m_brandBadge;
    QLabel* m_bannerLeftBolt;
    QLabel* m_bannerTitle;
    QLabel* m_bannerRightBolt;
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    QLabel* m_messageLabel;
    QLabel* m_identifierLabel;
    QLabel* m_passwordLabel;
    QLineEdit* m_identifierEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
};