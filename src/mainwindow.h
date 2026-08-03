#pragma once

#include <QMainWindow>
#include <QString>
#include <QSettings>

class QStackedWidget;
class LoginWidget;
class RegisterWidget;
class DashboardWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void showLoginScreen();
    void showRegisterScreen();
    void handleLoginRequested(const QString& identifier, const QString& password);
    void handleRegisterRequested(const QString& username, const QString& email, const QString& password, const QString& confirmPassword);
    void handleLogoutRequested();

private:
    void applyWindowPalette();
    void setCurrentScreen(QWidget* screen);
    void saveSession();
    void restoreSession();
    void clearSession();

private:
    QStackedWidget* m_stack;
    LoginWidget* m_loginWidget;
    RegisterWidget* m_registerWidget;
    DashboardWidget* m_dashboardWidget;
    QString m_loggedInUsername;
    bool m_databaseReady;
};