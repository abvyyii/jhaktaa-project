#include "registerwidget.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QGraphicsBlurEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPainter>

namespace {
const QColor kWindowColor(240, 240, 240);
const QColor kPanelColor(240, 240, 240);
const QColor kErrorColor(185, 28, 28);
const QColor kSuccessColor(22, 101, 52);
const QColor kBrandColor(37, 99, 235);
const QColor kSparkColor(79, 70, 229);
const QColor kTextColor(28, 28, 28);
const QColor kAccentColor(59, 130, 246);
constexpr int kBackgroundBlurRadius = 24;

QString findBackgroundGifPath() {
    if (QFile::exists(QStringLiteral(":/bg_ref.gif"))) {
        return QStringLiteral(":/bg_ref.gif");
    }

    const QStringList searchRoots = {
        QDir::currentPath(),
        QCoreApplication::applicationDirPath(),
        QDir::cleanPath(QCoreApplication::applicationDirPath() + "/.."),
        QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../.."),
        QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../..")
    };

    QStringList candidates;
    for (const QString& root : searchRoots) {
        if (root.isEmpty()) {
            continue;
        }
        candidates << QDir::cleanPath(root + "/bg_ref.gif");
        candidates << QDir::cleanPath(root + "/bg_ref.GIF");
    }
    candidates << QStringLiteral("bg_ref.gif");
    candidates << QStringLiteral("bg_ref.GIF");

    for (const QString& candidate : candidates) {
        if (!candidate.isEmpty() && QFileInfo::exists(candidate) && QFileInfo(candidate).isFile()) {
            return candidate;
        }
    }

    return QString();
}

void applyRoundedBackdropMask(QWidget* widget, const QRect& rect) {
    if (!widget || rect.isEmpty()) {
        return;
    }

    QPixmap maskPixmap(rect.size());
    maskPixmap.fill(Qt::transparent);

    QPainter painter(&maskPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(0, 0, rect.width(), rect.height()), 16, 16);
    painter.end();

    widget->setMask(maskPixmap.mask());
}
}

RegisterWidget::RegisterWidget(QWidget* parent)
    : QWidget(parent),
      m_backgroundLabel(new QLabel(this)),
      m_backgroundMovie(new QMovie(this)),
      m_panel(new QFrame(this)),
      m_panelBackdrop(new QFrame(this)),
      m_accentBar(new QFrame(this)),
      m_brandBadge(new QLabel(this)),
      m_bannerTitle(new QLabel(QStringLiteral("Jhatkaa"), this)),
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
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral("background: transparent;"));

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

    rootLayout->addWidget(contentWidget, 0, 0);
    contentWidget->raise();

    stylePanel(m_panel);
    stylePanelBackdrop(m_panelBackdrop, m_panel);
    m_panel->raise();
    m_accentBar->setFixedHeight(8);
    QPalette accentPalette = m_accentBar->palette();
    accentPalette.setColor(QPalette::Window, kAccentColor);
    m_accentBar->setPalette(accentPalette);
    m_accentBar->setAutoFillBackground(true);

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
    topBannerRow->setSpacing(0);
    topBannerRow->addStretch();
    topBannerRow->addWidget(m_bannerTitle, 0, Qt::AlignCenter);
    topBannerRow->addStretch();

    QPixmap logoPixmap(QStringLiteral(":/logo.png"));
    if (!logoPixmap.isNull()) {
        m_brandBadge->setPixmap(logoPixmap.scaled(42, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_brandBadge->setText("J");
        QFont badgeFont = m_brandBadge->font();
        badgeFont.setBold(true);
        badgeFont.setPointSize(18);
        m_brandBadge->setFont(badgeFont);
        QPalette badgePalette = m_brandBadge->palette();
        badgePalette.setColor(QPalette::Window, kBrandColor);
        badgePalette.setColor(QPalette::WindowText, Qt::white);
        m_brandBadge->setPalette(badgePalette);
        m_brandBadge->setAutoFillBackground(true);
    }
    m_brandBadge->setAlignment(Qt::AlignCenter);
    m_brandBadge->setMinimumSize(42, 42);
    m_brandBadge->setMaximumSize(42, 42);

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
    m_passwordToggleButton->setText(QStringLiteral("�"));
    m_confirmPasswordToggleButton->setText(QStringLiteral("🙈"));
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

    panelLayout->addWidget(m_accentBar);
    panelLayout->addLayout(topBannerRow);
    panelLayout->addLayout(headerRow);
    panelLayout->addSpacing(8);
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
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    setPalette(palette);
    setAutoFillBackground(true);
    setStyleSheet(QStringLiteral("background: #f0f0f0;"));
}

void RegisterWidget::setupAnimatedBackground() {
    m_backgroundLabel->hide();
    m_backgroundMovie->stop();
    m_backgroundLabel->clear();
}

void RegisterWidget::stylePanel(QFrame* frame) {
    QPalette palette = frame->palette();
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Dark, QColor(127, 127, 127));
    palette.setColor(QPalette::Shadow, QColor(127, 127, 127));
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);
    frame->setAttribute(Qt::WA_TranslucentBackground, false);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setLineWidth(1);
    frame->setStyleSheet(QStringLiteral("background: white; border: 1px solid #7f7f7f;"));
    frame->setMinimumWidth(460);
    frame->setMaximumWidth(560);
}

void RegisterWidget::stylePanelBackdrop(QFrame* backdrop, QFrame* host) {
    Q_UNUSED(host);
    backdrop->hide();
}

void RegisterWidget::styleLabel(QLabel* label, bool title) {
    QFont font = label->font();
    font.setBold(title);
    font.setPointSize(title ? 17 : 10);
    label->setFont(font);
    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet(QStringLiteral("border: none; background: transparent;"));
    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, kTextColor);
    label->setPalette(palette);
}

void RegisterWidget::styleLineEdit(QLineEdit* edit, bool secret) {
    QPalette palette = edit->palette();
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::Text, kTextColor);
    palette.setColor(QPalette::PlaceholderText, QColor(110, 110, 110));
    edit->setPalette(palette);
    edit->setAutoFillBackground(true);
    edit->setStyleSheet(QStringLiteral("border: 1px solid #7f7f7f; background: white; padding: 6px 8px;"));
    edit->setMinimumHeight(32);
    edit->setClearButtonEnabled(true);
    if (secret) {
        edit->setEchoMode(QLineEdit::Password);
    }
}

void RegisterWidget::stylePrimaryButton(QPushButton* button) {
    QPalette palette = button->palette();
    palette.setColor(QPalette::Button, kAccentColor);
    palette.setColor(QPalette::ButtonText, Qt::white);
    button->setPalette(palette);
    button->setAutoFillBackground(true);
    button->setStyleSheet(QStringLiteral("QPushButton { background-color: #f0f0f0; color: black; border: 1px solid #7f7f7f; padding: 6px 10px; } QPushButton:hover { background-color: #e5e5e5; }"));
    button->setMinimumHeight(36);
    button->setMinimumWidth(120);
}

void RegisterWidget::styleLinkButton(QPushButton* button) {
    QPalette palette = button->palette();
    palette.setColor(QPalette::ButtonText, kAccentColor);
    palette.setColor(QPalette::WindowText, kAccentColor);
    button->setPalette(palette);
    button->setFlat(true);
    button->setAutoFillBackground(false);
    button->setStyleSheet(QStringLiteral("background: transparent; border: none; color: #003399; text-decoration: underline;"));
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

void RegisterWidget::updateAnimatedBackgroundSize() {
    if (m_backgroundMovie->isValid()) {
        m_backgroundMovie->setScaledSize(size());
    }

    const QPixmap currentPixmap = m_backgroundLabel->pixmap(Qt::ReturnByValue);
    if (!currentPixmap.isNull()) {
        m_backgroundLabel->setPixmap(currentPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}

void RegisterWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_backgroundLabel->setGeometry(rect());
    updateAnimatedBackgroundSize();
    if (m_panelBackdrop && m_panel) {
        m_panelBackdrop->setGeometry(m_panel->geometry());
        applyRoundedBackdropMask(m_panelBackdrop, m_panel->rect());
    }
}

void RegisterWidget::setPasswordVisibility(bool visible) {
    m_passwordEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
    m_passwordToggleButton->setText(visible ? QStringLiteral("👁") : QStringLiteral(""));
}

void RegisterWidget::setConfirmPasswordVisibility(bool visible) {
    m_confirmPasswordEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
    m_confirmPasswordToggleButton->setText(visible ? QStringLiteral("👁") : QStringLiteral(""));
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