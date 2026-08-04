#pragma once

#include <QString>
#include <QWidget>

class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QToolButton;
class QResizeEvent;

class RegisterWidget : public QWidget {
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget* parent = nullptr);

    void setMessage(const QString& text, bool success);
    void clearMessage();
    void clearFields();
    void clearPasswordFields();
    void setFormEnabled(bool enabled);

signals:
    void registerRequested(const QString& username, const QString& email, const QString& password, const QString& confirmPassword);
    void loginRequested();

private:
    void buildUi();
    void applyBasePalette();
    void setupAnimatedBackground();
    void updateAnimatedBackgroundSize();
    void stylePanel(QFrame* frame);
    void stylePanelBackdrop(QFrame* backdrop, QFrame* host);
    void styleLabel(QLabel* label, bool title = false);
    void styleLineEdit(QLineEdit* edit, bool secret = false);
    void stylePrimaryButton(QPushButton* button);
    void styleLinkButton(QPushButton* button);
    void styleToggleButton(QToolButton* button);
    void setPasswordVisibility(bool visible);
    void setConfirmPasswordVisibility(bool visible);
    void updateMessageAppearance(bool success);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel* m_backgroundLabel;
    QFrame* m_panel;
    QFrame* m_panelBackdrop;
    QFrame* m_accentBar;
    QLabel* m_brandBadge;
    QLabel* m_bannerLeftBolt;
    QLabel* m_bannerTitle;
    QLabel* m_bannerRightBolt;
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    QLabel* m_messageLabel;
    QLabel* m_usernameLabel;
    QLabel* m_emailLabel;
    QLabel* m_passwordLabel;
    QLabel* m_confirmPasswordLabel;
    QLineEdit* m_usernameEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_confirmPasswordEdit;
    QToolButton* m_passwordToggleButton;
    QToolButton* m_confirmPasswordToggleButton;
    QPushButton* m_registerButton;
    QPushButton* m_loginButton;
};