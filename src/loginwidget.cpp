#include "loginwidget.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QMovie>
#include <QPalette>
#include <QPushButton>
#include <QResizeEvent>
#include <QDir>
#include <QVBoxLayout>

namespace {
const QColor kWindowColor(240, 240, 240);
const QColor kPanelColor(240, 240, 240);
const QColor kBrandColor(37, 99, 235);
const QColor kSparkColor(79, 70, 229);
const QColor kBorderColor(60, 60, 60);
const QColor kTextColor(28, 28, 28);
const QColor kAccentColor(59, 130, 246);
const QColor kSecondaryButtonColor(226, 232, 240);
const QColor kErrorColor(150, 56, 40);
const QColor kSuccessColor(76, 111, 63);
constexpr qreal kBackgroundOpacity = 0.3;

QString findBackgroundGifPath() {
    const QString appDirPath = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        QDir::cleanPath(appDirPath + "/../bg_ref.gif"),
        QDir::cleanPath(appDirPath + "/../bg_ref.GIF"),
        QDir::cleanPath(appDirPath + "/../../bg_ref.gif"),
        QDir::cleanPath(appDirPath + "/../../bg_ref.GIF"),
        appDirPath + "/bg_ref.gif",
        appDirPath + "/bg_ref.GIF"
    };

    for (const QString& candidate : candidates) {
        if (!candidate.isEmpty() && QFileInfo::exists(candidate)) {
            return candidate;
        }
    }

    return QString();
}
}

LoginWidget::LoginWidget(QWidget* parent)
    : QWidget(parent),
      m_backgroundLabel(new QLabel(this)),
      m_backgroundMovie(new QMovie(this)),
    m_backgroundOpacityEffect(nullptr),
    m_backgroundFadeAnimation(nullptr),
      m_panel(new QFrame(this)),
      m_accentBar(new QFrame(this)),
      m_brandBadge(new QLabel("J", this)),
      m_bannerLeftBolt(new QLabel(QStringLiteral("⚡"), this)),
      m_bannerTitle(new QLabel(QStringLiteral("Jhatkaa"), this)),
      m_bannerRightBolt(new QLabel(QStringLiteral("⚡"), this)),
      m_titleLabel(new QLabel("Welcome!!!", this)),
      m_subtitleLabel(new QLabel("Log in with your username or email.", this)),
      m_messageLabel(new QLabel(this)),
      m_identifierLabel(new QLabel("Username or email", this)),
      m_passwordLabel(new QLabel("Password", this)),
      m_identifierEdit(new QLineEdit(this)),
      m_passwordEdit(new QLineEdit(this)),
      m_loginButton(new QPushButton("Login", this)),
      m_registerButton(new QPushButton("Don't have an account? Register", this)) {
    buildUi();
}

void LoginWidget::buildUi() {
    applyBasePalette();

    auto* rootLayout = new QGridLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    setupAnimatedBackground();
    m_backgroundLabel->setGeometry(rect());

    auto* contentWidget = new QWidget(this);
    contentWidget->setAutoFillBackground(false);
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(24, 24, 24, 24);
    contentLayout->setSpacing(0);

    contentLayout->addStretch();

    auto* centerRow = new QHBoxLayout();
    centerRow->addStretch();
    centerRow->addWidget(m_panel, 0, Qt::AlignCenter);
    centerRow->addStretch();
    contentLayout->addLayout(centerRow);

    contentLayout->addStretch();

    rootLayout->addWidget(m_backgroundLabel, 0, 0);
    rootLayout->addWidget(contentWidget, 0, 0);
    contentWidget->raise();

    stylePanel(m_panel);
    m_accentBar->setFixedHeight(8);
    QPalette accentPalette = m_accentBar->palette();
    accentPalette.setColor(QPalette::Window, kAccentColor);
    m_accentBar->setPalette(accentPalette);
    m_accentBar->setAutoFillBackground(true);

    QFont badgeFont = m_brandBadge->font();
    badgeFont.setBold(true);
    badgeFont.setPointSize(18);
    m_brandBadge->setFont(badgeFont);
    m_brandBadge->setAlignment(Qt::AlignCenter);
    m_brandBadge->setMinimumSize(42, 42);
    m_brandBadge->setMaximumSize(42, 42);
    QPalette badgePalette = m_brandBadge->palette();
    badgePalette.setColor(QPalette::Window, kBrandColor);
    badgePalette.setColor(QPalette::WindowText, Qt::white);
    m_brandBadge->setPalette(badgePalette);
    m_brandBadge->setAutoFillBackground(true);

    auto styleBolt = [](QLabel* label) {
        QFont font = label->font();
        font.setBold(true);
        font.setPointSize(19);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, kSparkColor);
        label->setPalette(palette);
    };
    styleBolt(m_bannerLeftBolt);
    styleBolt(m_bannerRightBolt);

    QFont bannerFont = m_bannerTitle->font();
    bannerFont.setBold(true);
    bannerFont.setPointSize(22);
    m_bannerTitle->setFont(bannerFont);
    m_bannerTitle->setAlignment(Qt::AlignCenter);
    QPalette bannerPalette = m_bannerTitle->palette();
    bannerPalette.setColor(QPalette::WindowText, kBrandColor);
    m_bannerTitle->setPalette(bannerPalette);

    auto* panelLayout = new QVBoxLayout(m_panel);
    panelLayout->setContentsMargins(24, 20, 24, 24);
    panelLayout->setSpacing(12);

    auto* topBannerRow = new QHBoxLayout();
    topBannerRow->setSpacing(10);
    topBannerRow->addStretch();
    topBannerRow->addWidget(m_bannerLeftBolt);
    topBannerRow->addWidget(m_bannerTitle);
    topBannerRow->addWidget(m_bannerRightBolt);
    topBannerRow->addStretch();

    auto* headerRow = new QHBoxLayout();
    headerRow->setSpacing(12);
    auto* badgeBox = new QHBoxLayout();
    badgeBox->addWidget(m_brandBadge, 0, Qt::AlignTop);
    badgeBox->addSpacing(4);
    auto* brandColumn = new QVBoxLayout();
    brandColumn->setSpacing(4);
    brandColumn->addWidget(m_titleLabel);
    brandColumn->addWidget(m_subtitleLabel);
    badgeBox->addLayout(brandColumn);
    badgeBox->addStretch();
    headerRow->addLayout(badgeBox);
    headerRow->addStretch();

    styleLabel(m_titleLabel, true);
    styleLabel(m_subtitleLabel, false);
    styleLabel(m_identifierLabel, false);
    styleLabel(m_passwordLabel, false);
    styleLabel(m_messageLabel, false);

    m_messageLabel->setWordWrap(true);
    m_messageLabel->setVisible(false);

    styleLineEdit(m_identifierEdit, false);
    styleLineEdit(m_passwordEdit, true);

    m_identifierEdit->setPlaceholderText("Enter your username or email");
    m_passwordEdit->setPlaceholderText("Enter your password");

    stylePrimaryButton(m_loginButton);
    styleLinkButton(m_registerButton);

    auto* identifierLayout = new QVBoxLayout();
    identifierLayout->setSpacing(6);
    identifierLayout->addWidget(m_identifierLabel);
    identifierLayout->addWidget(m_identifierEdit);

    auto* passwordLayout = new QVBoxLayout();
    passwordLayout->setSpacing(6);
    passwordLayout->addWidget(m_passwordLabel);
    passwordLayout->addWidget(m_passwordEdit);

    auto* buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(12);
    buttonRow->addWidget(m_loginButton);
    buttonRow->addStretch();

    panelLayout->addWidget(m_accentBar);
    panelLayout->addLayout(topBannerRow);
    panelLayout->addLayout(headerRow);
    panelLayout->addSpacing(8);
    panelLayout->addLayout(identifierLayout);
    panelLayout->addLayout(passwordLayout);
    panelLayout->addWidget(m_messageLabel);
    panelLayout->addLayout(buttonRow);
    panelLayout->addWidget(m_registerButton, 0, Qt::AlignCenter);

    connect(m_loginButton, &QPushButton::clicked, this, [this]() {
        emit loginRequested(m_identifierEdit->text(), m_passwordEdit->text());
    });
    connect(m_registerButton, &QPushButton::clicked, this, &LoginWidget::registerRequested);
    connect(m_identifierEdit, &QLineEdit::returnPressed, this, [this]() {
        emit loginRequested(m_identifierEdit->text(), m_passwordEdit->text());
    });
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, [this]() {
        emit loginRequested(m_identifierEdit->text(), m_passwordEdit->text());
    });
}

void LoginWidget::setupAnimatedBackground() {
    m_backgroundLabel->setAlignment(Qt::AlignCenter);
    m_backgroundLabel->setScaledContents(true);
    QPalette backgroundPalette = m_backgroundLabel->palette();
    backgroundPalette.setColor(QPalette::Window, kWindowColor);
    m_backgroundLabel->setPalette(backgroundPalette);
    m_backgroundLabel->setAutoFillBackground(true);

    m_backgroundOpacityEffect = new QGraphicsOpacityEffect(m_backgroundLabel);
    m_backgroundOpacityEffect->setOpacity(kBackgroundOpacity);
    m_backgroundLabel->setGraphicsEffect(m_backgroundOpacityEffect);

    const QString backgroundPath = findBackgroundGifPath();
    if (!backgroundPath.isEmpty()) {
        m_backgroundMovie->setFileName(backgroundPath);
        m_backgroundMovie->setCacheMode(QMovie::CacheAll);
        m_backgroundMovie->setScaledSize(size());
        m_backgroundLabel->setMovie(m_backgroundMovie);
        m_backgroundMovie->start();

        m_backgroundFadeAnimation = new QPropertyAnimation(m_backgroundOpacityEffect, "opacity", this);
        m_backgroundFadeAnimation->setDuration(450);
        m_backgroundFadeAnimation->setStartValue(0.0);
        m_backgroundFadeAnimation->setEndValue(kBackgroundOpacity);
        m_backgroundFadeAnimation->start();
    } else {
        m_backgroundLabel->setText("Place bg_ref.gif on your Desktop to enable the animated background.");
        QPalette palette = m_backgroundLabel->palette();
        palette.setColor(QPalette::WindowText, QColor(122, 110, 95));
        m_backgroundLabel->setPalette(palette);
    }
}

void LoginWidget::updateAnimatedBackgroundSize() {
    if (m_backgroundMovie->isValid()) {
        m_backgroundMovie->setScaledSize(size());
    }
}

void LoginWidget::applyBasePalette() {
    QFont font(QStringLiteral("Segoe UI"), 10);
    setFont(font);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, kWindowColor);
    setPalette(palette);
    setAutoFillBackground(true);
}

void LoginWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_backgroundLabel->setGeometry(rect());
    updateAnimatedBackgroundSize();
}

void LoginWidget::stylePanel(QFrame* frame) {
    QPalette palette = frame->palette();
    palette.setColor(QPalette::Window, kPanelColor);
    palette.setColor(QPalette::WindowText, kPanelColor);
    palette.setColor(QPalette::Dark, kPanelColor);
    palette.setColor(QPalette::Shadow, kPanelColor);
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Plain);
    frame->setLineWidth(0);
    frame->setMinimumWidth(460);
    frame->setMaximumWidth(560);
}

void LoginWidget::styleLabel(QLabel* label, bool title) {
    QFont font = label->font();
    font.setBold(title);
    font.setPointSize(title ? 17 : 10);
    label->setFont(font);
    label->setAlignment(Qt::AlignLeft);
    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, kTextColor);
    label->setPalette(palette);
}

void LoginWidget::styleLineEdit(QLineEdit* edit, bool secret) {
    QPalette palette = edit->palette();
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::Text, kTextColor);
    palette.setColor(QPalette::PlaceholderText, QColor(110, 110, 110));
    edit->setPalette(palette);
    edit->setAutoFillBackground(true);
    edit->setMinimumHeight(32);
    edit->setClearButtonEnabled(true);
    if (secret) {
        edit->setEchoMode(QLineEdit::Password);
    }
}

void LoginWidget::stylePrimaryButton(QPushButton* button) {
    QPalette palette = button->palette();
    palette.setColor(QPalette::Button, kAccentColor);
    palette.setColor(QPalette::ButtonText, Qt::white);
    button->setPalette(palette);
    button->setAutoFillBackground(true);
    button->setMinimumHeight(36);
    button->setMinimumWidth(120);
}

void LoginWidget::styleLinkButton(QPushButton* button) {
    QPalette palette = button->palette();
    palette.setColor(QPalette::ButtonText, kAccentColor);
    button->setPalette(palette);
    button->setFlat(true);
}

void LoginWidget::updateMessageAppearance(bool success) {
    QPalette palette = m_messageLabel->palette();
    palette.setColor(QPalette::WindowText, success ? kSuccessColor : kErrorColor);
    m_messageLabel->setPalette(palette);
}

void LoginWidget::setMessage(const QString& text, bool success) {
    m_messageLabel->setText(text);
    m_messageLabel->setVisible(!text.trimmed().isEmpty());
    updateMessageAppearance(success);
}

void LoginWidget::clearMessage() {
    setMessage(QString(), false);
}

void LoginWidget::clearFields() {
    m_identifierEdit->clear();
    m_passwordEdit->clear();
}

void LoginWidget::setFormEnabled(bool enabled) {
    m_identifierEdit->setEnabled(enabled);
    m_passwordEdit->setEnabled(enabled);
    m_loginButton->setEnabled(enabled);
    m_registerButton->setEnabled(enabled);
}