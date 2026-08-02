#include "mainwindow.h"

#include <QPalette>
#include <QStackedWidget>

#include "dashboardwidget.h"
#include "database.h"
#include "loginwidget.h"
#include "registerwidget.h"

namespace {
const QColor kWindowColor(240, 240, 240);

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

    applyWindowPalette();
    setCentralWidget(m_stack);

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

    showLoginScreen();
}

void MainWindow::applyWindowPalette() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, kWindowColor);
    setPalette(palette);
    setAutoFillBackground(true);
}

void MainWindow::setCurrentScreen(QWidget* screen) {
    if (screen) {
        m_stack->setCurrentWidget(screen);
    }
}

void MainWindow::showLoginScreen() {
    setCurrentScreen(m_loginWidget);
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
    showLoginScreen();
}