#pragma once

#include <QMainWindow>
#include <QPoint>
#include <QString>
#include <QSettings>

class QStackedWidget;
class QLabel;
class QToolButton;
class QWidget;
class LoginWidget;
class RegisterWidget;
class DashboardWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void showLoginScreen();
    void minimizeWindow();
    void toggleMaximize();
    void closeWindow();
    void showRegisterScreen();
    void handleLoginRequested(const QString& identifier, const QString& password);
    void handleRegisterRequested(const QString& username, const QString& email, const QString& password, const QString& confirmPassword);
    void handleLogoutRequested();

private:
    void applyWindowPalette();
    void createTitleBar();
    void styleCanvasView();
    void setCurrentScreen(QWidget* screen);
    bool eventFilter(QObject* watched, QEvent* event) override;
    void saveSession();
    void restoreSession();
    void clearSession();

private:
    QWidget* m_windowContainer;
    QWidget* m_titleBar;
    QLabel* m_titleLabel;
    QLabel* m_titleIconLabel;
    QToolButton* m_minimizeButton;
    QToolButton* m_maximizeButton;
    QToolButton* m_closeButton;
    QStackedWidget* m_stack;
    QPoint m_dragPosition;
    bool m_dragging;
    LoginWidget* m_loginWidget;
    RegisterWidget* m_registerWidget;
    DashboardWidget* m_dashboardWidget;
    QString m_loggedInUsername;
    bool m_databaseReady;
};