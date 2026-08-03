#include "registerwidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
const QColor kErrorColor(185, 28, 28);
const QColor kSuccessColor(22, 101, 52);
}

RegisterWidget::RegisterWidget(QWidget* parent)
    : QWidget(parent),
      m_panel(new QFrame(this)),
      m_titleLabel(new QLabel("Create your account", this)),
      m_subtitleLabel(new QLabel("It only takes a few seconds to get started.", this)),
      m_messageLabel(new QLabel(this)),
      m_usernameLabel(new QLabel("Username", this)),
      m_emailLabel(new QLabel("Email", this)),
      m_passwordLabel(new QLabel("Password", this)),
      m_confirmPasswordLabel(new QLabel("Confirm password", this)),
      m_usernameEdit(new QLineEdit(this)),
      m_emailEdit(new QLineEdit(this)),
      m_passwordEdit(new QLineEdit(this)),
      m_confirmPasswordEdit(new QLineEdit(this)),
            m_passwordToggleButton(new QToolButton(this)),
            m_confirmPasswordToggleButton(new QToolButton(this)),
      m_registerButton(new QPushButton("Register", this)),
      m_loginButton(new QPushButton("Already have an account? Login", this)) {
    buildUi();
}

void RegisterWidget::buildUi() {
    applyBasePalette();

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(24, 24, 24, 24);
    rootLayout->setSpacing(0);

    rootLayout->addStretch();

    auto* centerRow = new QHBoxLayout();
    centerRow->addStretch();
    centerRow->addWidget(m_panel, 0, Qt::AlignCenter);
    centerRow->addStretch();
    rootLayout->addLayout(centerRow);

    rootLayout->addStretch();

    stylePanel(m_panel);

    auto* panelLayout = new QVBoxLayout(m_panel);
    panelLayout->setContentsMargins(24, 24, 24, 24);
    panelLayout->setSpacing(12);

    styleLabel(m_titleLabel, true);
    styleLabel(m_subtitleLabel, false);
    styleLabel(m_usernameLabel, false);
    styleLabel(m_emailLabel, false);
    styleLabel(m_passwordLabel, false);
    styleLabel(m_confirmPasswordLabel, false);
    styleLabel(m_messageLabel, false);

    m_messageLabel->setWordWrap(true);
    m_messageLabel->setVisible(false);

    styleLineEdit(m_usernameEdit, false);
    styleLineEdit(m_emailEdit, false);
    styleLineEdit(m_passwordEdit, true);
    styleLineEdit(m_confirmPasswordEdit, true);
    styleToggleButton(m_passwordToggleButton);
    styleToggleButton(m_confirmPasswordToggleButton);

    m_usernameEdit->setPlaceholderText("Choose a username");
    m_emailEdit->setPlaceholderText("Enter your email");
    m_passwordEdit->setPlaceholderText("Create a password");
    m_confirmPasswordEdit->setPlaceholderText("Repeat your password");
    m_passwordToggleButton->setText(QStringLiteral("👁"));
    m_confirmPasswordToggleButton->setText(QStringLiteral("👁"));
    m_passwordToggleButton->setToolTip("Show or hide password");
    m_confirmPasswordToggleButton->setToolTip("Show or hide password");

    stylePrimaryButton(m_registerButton);
    styleLinkButton(m_loginButton);

    auto* usernameLayout = new QVBoxLayout();
    usernameLayout->setSpacing(6);
    usernameLayout->addWidget(m_usernameLabel);
    usernameLayout->addWidget(m_usernameEdit);

    auto* emailLayout = new QVBoxLayout();
    emailLayout->setSpacing(6);
    emailLayout->addWidget(m_emailLabel);
    emailLayout->addWidget(m_emailEdit);

    auto* passwordLayout = new QVBoxLayout();
    passwordLayout->setSpacing(6);
    passwordLayout->addWidget(m_passwordLabel);
    auto* passwordRow = new QHBoxLayout();
    passwordRow->setSpacing(8);
    passwordRow->addWidget(m_passwordEdit, 1);
    passwordRow->addWidget(m_passwordToggleButton, 0, Qt::AlignVCenter);
    passwordLayout->addLayout(passwordRow);

    auto* confirmPasswordLayout = new QVBoxLayout();
    confirmPasswordLayout->setSpacing(6);
    confirmPasswordLayout->addWidget(m_confirmPasswordLabel);
    auto* confirmPasswordRow = new QHBoxLayout();
    confirmPasswordRow->setSpacing(8);
    confirmPasswordRow->addWidget(m_confirmPasswordEdit, 1);
    confirmPasswordRow->addWidget(m_confirmPasswordToggleButton, 0, Qt::AlignVCenter);
    confirmPasswordLayout->addLayout(confirmPasswordRow);

    auto* buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(12);
    buttonRow->addWidget(m_registerButton);
    buttonRow->addStretch();

    panelLayout->addWidget(m_titleLabel);
    panelLayout->addWidget(m_subtitleLabel);
    panelLayout->addSpacing(4);
    panelLayout->addLayout(usernameLayout);
    panelLayout->addLayout(emailLayout);
    panelLayout->addLayout(passwordLayout);
    panelLayout->addLayout(confirmPasswordLayout);
    panelLayout->addWidget(m_messageLabel);
    panelLayout->addLayout(buttonRow);
    panelLayout->addWidget(m_loginButton, 0, Qt::AlignCenter);

    connect(m_registerButton, &QPushButton::clicked, this, [this]() {
        emit registerRequested(m_usernameEdit->text(), m_emailEdit->text(), m_passwordEdit->text(), m_confirmPasswordEdit->text());
    });
    connect(m_loginButton, &QPushButton::clicked, this, &RegisterWidget::loginRequested);
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, [this]() {
        emit registerRequested(m_usernameEdit->text(), m_emailEdit->text(), m_passwordEdit->text(), m_confirmPasswordEdit->text());
    });
    connect(m_emailEdit, &QLineEdit::returnPressed, this, [this]() {
        emit registerRequested(m_usernameEdit->text(), m_emailEdit->text(), m_passwordEdit->text(), m_confirmPasswordEdit->text());
    });
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, [this]() {
        emit registerRequested(m_usernameEdit->text(), m_emailEdit->text(), m_passwordEdit->text(), m_confirmPasswordEdit->text());
    });
    connect(m_confirmPasswordEdit, &QLineEdit::returnPressed, this, [this]() {
        emit registerRequested(m_usernameEdit->text(), m_emailEdit->text(), m_passwordEdit->text(), m_confirmPasswordEdit->text());
    });
    connect(m_passwordToggleButton, &QToolButton::clicked, this, [this]() {
        setPasswordVisibility(m_passwordEdit->echoMode() == QLineEdit::Password);
    });
    connect(m_confirmPasswordToggleButton, &QToolButton::clicked, this, [this]() {
        setConfirmPasswordVisibility(m_confirmPasswordEdit->echoMode() == QLineEdit::Password);
    });
}

void RegisterWidget::applyBasePalette() {
    QFont font(QStringLiteral("Segoe UI"), 10);
    setFont(font);
    setAutoFillBackground(false);
}

void RegisterWidget::stylePanel(QFrame* frame) {
    frame->setAutoFillBackground(false);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLineWidth(1);
    frame->setMinimumWidth(440);
    frame->setMaximumWidth(520);
}

void RegisterWidget::styleLabel(QLabel* label, bool title) {
    QFont font = label->font();
    font.setBold(title);
    font.setPointSize(title ? 17 : 10);
    label->setFont(font);
    label->setAlignment(Qt::AlignLeft);
}

void RegisterWidget::styleLineEdit(QLineEdit* edit, bool secret) {
    edit->setAutoFillBackground(false);
    edit->setMinimumHeight(32);
    edit->setClearButtonEnabled(true);
    if (secret) {
        edit->setEchoMode(QLineEdit::Password);
    }
}

void RegisterWidget::stylePrimaryButton(QPushButton* button) {
    button->setAutoFillBackground(false);
    button->setMinimumHeight(36);
    button->setMinimumWidth(120);
}

void RegisterWidget::styleLinkButton(QPushButton* button) {
    button->setFlat(true);
}

void RegisterWidget::styleToggleButton(QToolButton* button) {
    button->setAutoFillBackground(false);
    button->setCheckable(true);
    button->setAutoRaise(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(34, 34);
    QFont font(QStringLiteral("Segoe UI Symbol"), 13);
    font.setBold(true);
    button->setFont(font);
}

void RegisterWidget::setPasswordVisibility(bool visible) {
    m_passwordEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
    m_passwordToggleButton->setText(visible ? QStringLiteral("🙈") : QStringLiteral("👁"));
}

void RegisterWidget::setConfirmPasswordVisibility(bool visible) {
    m_confirmPasswordEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
    m_confirmPasswordToggleButton->setText(visible ? QStringLiteral("🙈") : QStringLiteral("👁"));
}

void RegisterWidget::updateMessageAppearance(bool success) {
    QPalette palette = m_messageLabel->palette();
    palette.setColor(QPalette::WindowText, success ? kSuccessColor : kErrorColor);
    m_messageLabel->setPalette(palette);
}

void RegisterWidget::setMessage(const QString& text, bool success) {
    m_messageLabel->setText(text);
    m_messageLabel->setVisible(!text.trimmed().isEmpty());
    updateMessageAppearance(success);
}

void RegisterWidget::clearMessage() {
    setMessage(QString(), false);
}

void RegisterWidget::clearFields() {
    m_usernameEdit->clear();
    m_emailEdit->clear();
    m_passwordEdit->clear();
    m_confirmPasswordEdit->clear();
}

void RegisterWidget::clearPasswordFields() {
    m_passwordEdit->clear();
    m_confirmPasswordEdit->clear();
}

void RegisterWidget::setFormEnabled(bool enabled) {
    m_usernameEdit->setEnabled(enabled);
    m_emailEdit->setEnabled(enabled);
    m_passwordEdit->setEnabled(enabled);
    m_confirmPasswordEdit->setEnabled(enabled);
    m_registerButton->setEnabled(enabled);
    m_loginButton->setEnabled(enabled);
}