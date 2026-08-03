#include "mainwindow.h"

#include <QColor>
#include <QEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSettings>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include "dashboardwidget.h"
#include "database.h"
#include "loginwidget.h"
#include "registerwidget.h"

namespace {
QPixmap createTitlePixmap(int size) {
    QPixmap pixmap(QStringLiteral(":/logo.png"));
    if (!pixmap.isNull()) {
        return pixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QImage image(size, size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(QColor("#0078D7"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(2, 2, size - 4, size - 4);

    painter.setBrush(QColor("#FFFFFF"));
    painter.drawRect(size / 4, size / 4, size / 2, size / 2);

    painter.setPen(QPen(QColor("#FFFFFF"), 2));
    painter.drawLine(size / 4, size / 2, 3 * size / 4, size / 2);
    painter.drawLine(size / 2, size / 4, size / 2, 3 * size / 4);
    painter.end();

    return QPixmap::fromImage(image);
}

bool passwordMeetsPolicy(const QString& password) {
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (const QChar character : password) {
        if (character.isUpper()) {
            hasUpper = true;
        } else if (character.isLower()) {
            hasLower = true;
        } else if (character.isDigit()) {
            hasDigit = true;
        } else if (!character.isSpace()) {
            hasSymbol = true;
        }
    }

    return hasUpper && hasLower && hasDigit && hasSymbol;
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_stack(new QStackedWidget(this)),
      m_loginWidget(new LoginWidget(this)),
      m_registerWidget(new RegisterWidget(this)),
      m_dashboardWidget(new DashboardWidget(this)),
      m_databaseReady(false) {
    setWindowTitle("Jhatkaa");
    resize(980, 720);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    m_dragging = false;

    m_windowContainer = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(m_windowContainer);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    createTitleBar();
    rootLayout->addWidget(m_titleBar);
    rootLayout->addWidget(m_stack);

    applyWindowPalette();
    setCentralWidget(m_windowContainer);

    m_stack->addWidget(m_loginWidget);
    m_stack->addWidget(m_registerWidget);
    m_stack->addWidget(m_dashboardWidget);

    connect(m_loginWidget, &LoginWidget::loginRequested, this, &MainWindow::handleLoginRequested);
    connect(m_loginWidget, &LoginWidget::registerRequested, this, &MainWindow::showRegisterScreen);

    connect(m_registerWidget, &RegisterWidget::registerRequested, this, &MainWindow::handleRegisterRequested);
    connect(m_registerWidget, &RegisterWidget::loginRequested, this, &MainWindow::showLoginScreen);

    connect(m_dashboardWidget, &DashboardWidget::logoutRequested, this, &MainWindow::handleLogoutRequested);

    QString databaseError;
    m_databaseReady = Database::initialize(&databaseError);
    if (!m_databaseReady) {
        m_loginWidget->setMessage(databaseError, false);
        m_registerWidget->setMessage(databaseError, false);
        m_loginWidget->setFormEnabled(false);
        m_registerWidget->setFormEnabled(false);
    }

    restoreSession();
}

void MainWindow::createTitleBar() {
    m_titleBar = new QWidget(this);
    m_titleBar->setFixedHeight(34);
    m_titleBar->setObjectName("MainWindowTitleBar");
    m_titleBar->installEventFilter(this);

    auto* titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(10, 4, 8, 4);
    titleLayout->setSpacing(6);

    m_titleIconLabel = new QLabel(m_titleBar);
    m_titleIconLabel->setObjectName("MainWindowTitleIconLabel");
    m_titleIconLabel->setPixmap(createTitlePixmap(20));
    m_titleIconLabel->setAlignment(Qt::AlignVCenter);

    m_titleLabel = new QLabel("Jhatkaa", m_titleBar);
    m_titleLabel->setObjectName("MainWindowTitleLabel");
    m_titleLabel->setStyleSheet("color: #000000; font-weight: 600; background: transparent; border: none;");
    m_titleLabel->setAlignment(Qt::AlignVCenter);

    m_minimizeButton = new QToolButton(m_titleBar);
    m_minimizeButton->setObjectName("MinimizeButton");
    m_minimizeButton->setText("−");
    m_minimizeButton->setFixedSize(12, 12);
    m_minimizeButton->setToolTip("Minimize");

    m_maximizeButton = new QToolButton(m_titleBar);
    m_maximizeButton->setObjectName("MaximizeButton");
    m_maximizeButton->setText("□");
    m_maximizeButton->setFixedSize(12, 12);
    m_maximizeButton->setToolTip("Maximize");

    m_closeButton = new QToolButton(m_titleBar);
    m_closeButton->setObjectName("CloseButton");
    m_closeButton->setText("×");
    m_closeButton->setFixedSize(12, 12);
    m_closeButton->setToolTip("Close");

    titleLayout->addWidget(m_titleIconLabel);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_minimizeButton);
    titleLayout->addWidget(m_maximizeButton);
    titleLayout->addWidget(m_closeButton);

    connect(m_minimizeButton, &QToolButton::clicked, this, &MainWindow::minimizeWindow);
    connect(m_maximizeButton, &QToolButton::clicked, this, &MainWindow::toggleMaximize);
    connect(m_closeButton, &QToolButton::clicked, this, &MainWindow::closeWindow);
}

void MainWindow::styleCanvasView() {
    auto* view = m_dashboardWidget->findChild<QGraphicsView*>(QString(), Qt::FindDirectChildrenOnly);
    if (view) {
        view->setStyleSheet(R"(
            QGraphicsView {
                background-color: #FFFFFF;
                border: 1px solid #ADADAD;
            }
            QGraphicsView QScrollBar:vertical {
                background: transparent;
                border: none;
                width: 7px;
                padding: 0px;
            }
            QGraphicsView QScrollBar:horizontal {
                background: transparent;
                border: none;
                height: 7px;
                padding: 0px;
            }
            QGraphicsView QScrollBar::handle:vertical,
            QGraphicsView QScrollBar::handle:horizontal {
                background: #D0D0D0;
                border: none;
                border-radius: 9999px;
                min-height: 20px;
                min-width: 20px;
            }
            QGraphicsView QScrollBar::handle:vertical:hover,
            QGraphicsView QScrollBar::handle:horizontal:hover {
                background: #B5B5B5;
            }
            QGraphicsView QScrollBar::handle:vertical:pressed,
            QGraphicsView QScrollBar::handle:horizontal:pressed {
                background: #9E9E9E;
            }
            QGraphicsView QScrollBar::add-line,
            QGraphicsView QScrollBar::sub-line {
                height: 0px;
                width: 0px;
                background: transparent;
                border: none;
            }
            QGraphicsView QScrollBar::add-page,
            QGraphicsView QScrollBar::sub-page {
                background: transparent;
            }
            QGraphicsView QScrollBar::up-arrow,
            QGraphicsView QScrollBar::down-arrow,
            QGraphicsView QScrollBar::left-arrow,
            QGraphicsView QScrollBar::right-arrow {
                background: transparent;
                border: none;
            }
        )");
    }
}

void MainWindow::applyWindowPalette() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#F0F0F0"));
    palette.setColor(QPalette::WindowText, QColor("#000000"));
    palette.setColor(QPalette::Base, QColor("#FFFFFF"));
    palette.setColor(QPalette::AlternateBase, QColor("#F0F0F0"));
    palette.setColor(QPalette::ToolTipBase, QColor("#FFFFFF"));
    palette.setColor(QPalette::ToolTipText, QColor("#000000"));
    palette.setColor(QPalette::Text, QColor("#000000"));
    palette.setColor(QPalette::Button, QColor("#E1E1E1"));
    palette.setColor(QPalette::ButtonText, QColor("#000000"));
    palette.setColor(QPalette::BrightText, QColor("#FFFFFF"));
    palette.setColor(QPalette::Highlight, QColor("#0078D7"));
    palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    palette.setColor(QPalette::Light, QColor("#FFFFFF"));
    palette.setColor(QPalette::Midlight, QColor("#F0F0F0"));
    palette.setColor(QPalette::Dark, QColor("#ADADAD"));
    palette.setColor(QPalette::Mid, QColor("#D0D0D0"));
    palette.setColor(QPalette::Shadow, QColor("#ADADAD"));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#A0A0A0"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#A0A0A0"));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#A0A0A0"));
    palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#F0F0F0"));

    setAutoFillBackground(true);
    setPalette(palette);
    m_stack->setPalette(palette);
    m_stack->setAutoFillBackground(true);
    if (m_titleBar) {
        m_titleBar->setStyleSheet(R"(
            QWidget#MainWindowTitleBar {
                background-color: #F0F0F0;
                border-bottom: 1px solid #ADADAD;
            }
            QLabel#MainWindowTitleLabel {
                color: #000000;
                background: transparent;
            }
            QToolButton {
                background-color: #E1E1E1;
                color: transparent;
                border: 1px solid #ADADAD;
                border-radius: 6px;
                padding: 0;
            }
            QToolButton#MinimizeButton {
                background-color: #28C840;
                border-color: #1AAB29;
            }
            QToolButton#MaximizeButton {
                background-color: #FFBD2E;
                border-color: #DEA123;
            }
            QToolButton#CloseButton {
                background-color: #FF5F57;
                border-color: #E0443E;
            }
        )");
    }

    setStyleSheet(R"(
        QMainWindow, QDialog, QWidget {
            background-color: #F0F0F0;
            color: #000000;
        }
        QStackedWidget {
            background-color: #F0F0F0;
        }
        QListWidget, QTableWidget, QTreeWidget, QTextEdit, QLineEdit, QPlainTextEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background-color: #FFFFFF;
            color: #000000;
            border: 1px solid #ADADAD;
        }
        QPushButton {
            background-color: #E1E1E1;
            color: #000000;
            border: 1px solid #ADADAD;
            padding: 6px 10px;
        }
        QPushButton:hover {
            background-color: #E5F1FB;
        }
        QPushButton:pressed {
            background-color: #CCE4F7;
        }
        QPushButton:disabled {
            color: #A0A0A0;
            background-color: #F0F0F0;
        }
        QHeaderView::section, QGroupBox, QFrame {
            background-color: #F0F0F0;
            color: #000000;
            border: 1px solid #ADADAD;
        }
        QScrollBar:vertical, QScrollBar:horizontal {
            background-color: #F0F0F0;
            border: 1px solid #ADADAD;
        }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
            background-color: #E1E1E1;
            border: 1px solid #ADADAD;
        }
        QScrollBar::handle:vertical:hover, QScrollBar::handle:horizontal:hover {
            background-color: #E5F1FB;
        }
        QScrollBar::handle:vertical:pressed, QScrollBar::handle:horizontal:pressed {
            background-color: #CCE4F7;
        }
        QMenu {
            background-color: #FFFFFF;
            color: #000000;
            border: 1px solid #ADADAD;
        }
        QMenu::item:selected {
            background-color: #E5F1FB;
            color: #000000;
        }
    )");

    styleCanvasView();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_titleBar && event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragging = true;
            m_dragPosition = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
            return true;
        }
    } else if (watched == m_titleBar && event->type() == QEvent::MouseMove && m_dragging) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        move(mouseEvent->globalPosition().toPoint() - m_dragPosition);
        return true;
    } else if (watched == m_titleBar && event->type() == QEvent::MouseButtonRelease) {
        m_dragging = false;
        return true;
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::minimizeWindow() {
    showMinimized();
}

void MainWindow::toggleMaximize() {
    if (isMaximized()) {
        showNormal();
        m_maximizeButton->setText("□");
    } else {
        showMaximized();
        m_maximizeButton->setText("❐");
    }
}

void MainWindow::closeWindow() {
    close();
}

void MainWindow::setCurrentScreen(QWidget* screen) {
    if (screen) {
        m_stack->setCurrentWidget(screen);
    }
}

void MainWindow::showLoginScreen() {
    setCurrentScreen(m_loginWidget);
}

void MainWindow::saveSession() {
    if (!m_databaseReady) {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Jhatkaa", "session");
    settings.setValue("logged_in", !m_loggedInUsername.isEmpty());
    settings.setValue("username", m_loggedInUsername);
}

void MainWindow::restoreSession() {
    if (!m_databaseReady) {
        showLoginScreen();
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Jhatkaa", "session");
    const bool loggedIn = settings.value("logged_in", false).toBool();
    const QString savedUsername = settings.value("username", QString()).toString();

    if (loggedIn && !savedUsername.isEmpty()) {
        m_loggedInUsername = savedUsername;
        m_dashboardWidget->setUsername(m_loggedInUsername);
        setCurrentScreen(m_dashboardWidget);
        return;
    }

    showLoginScreen();
}

void MainWindow::clearSession() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Jhatkaa", "session");
    settings.remove("logged_in");
    settings.remove("username");
}

void MainWindow::showRegisterScreen() {
    setCurrentScreen(m_registerWidget);
}

void MainWindow::handleLoginRequested(const QString& identifier, const QString& password) {
    if (!m_databaseReady) {
        m_loginWidget->setMessage("Database is not available.", false);
        return;
    }

    QString loggedInUsername;
    QString errorMessage;
    if (!Database::authenticate(identifier, password, &loggedInUsername, &errorMessage)) {
        m_loginWidget->setMessage(errorMessage.isEmpty() ? "Invalid username or password." : errorMessage, false);
        return;
    }

    m_loggedInUsername = loggedInUsername;
    m_dashboardWidget->setUsername(m_loggedInUsername);
    m_loginWidget->clearFields();
    m_loginWidget->clearMessage();
    saveSession();
    setCurrentScreen(m_dashboardWidget);
}

void MainWindow::handleRegisterRequested(const QString& username, const QString& email, const QString& password, const QString& confirmPassword) {
    if (!m_databaseReady) {
        m_registerWidget->setMessage("Database is not available.", false);
        return;
    }

    const QString trimmedUsername = username.trimmed();
    const QString trimmedEmail = email.trimmed();

    if (trimmedUsername.isEmpty() || trimmedEmail.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        m_registerWidget->setMessage("Please fill in every field.", false);
        return;
    }

    if (password != confirmPassword) {
        m_registerWidget->setMessage("Passwords do not match.", false);
        return;
    }

    if (password.size() < 8) {
        m_registerWidget->setMessage("Password must be at least 8 characters long.", false);
        return;
    }

    if (!passwordMeetsPolicy(password)) {
        m_registerWidget->setMessage("Password must include an uppercase letter, a lowercase letter, a number, and a symbol.", false);
        return;
    }

    QString errorMessage;
    if (Database::usernameExists(trimmedUsername, &errorMessage)) {
        m_registerWidget->setMessage("That username is already taken.", false);
        return;
    }

    if (!errorMessage.isEmpty()) {
        m_registerWidget->setMessage(errorMessage, false);
        return;
    }

    if (Database::emailExists(trimmedEmail, &errorMessage)) {
        m_registerWidget->setMessage("That email address is already taken.", false);
        return;
    }

    if (!errorMessage.isEmpty()) {
        m_registerWidget->setMessage(errorMessage, false);
        return;
    }

    if (!Database::createUser(trimmedUsername, trimmedEmail, password, &errorMessage)) {
        m_registerWidget->setMessage(errorMessage.isEmpty() ? "Registration failed." : errorMessage, false);
        return;
    }

    m_registerWidget->setMessage("Registration successful. You can log in now.", true);
    m_registerWidget->clearPasswordFields();
}

void MainWindow::handleLogoutRequested() {
    m_loggedInUsername.clear();
    m_dashboardWidget->setUsername(QString());
    m_loginWidget->clearMessage();
    clearSession();
    showLoginScreen();
}