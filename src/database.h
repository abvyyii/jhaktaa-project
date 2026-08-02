#pragma once

#include <QByteArray>
#include <QString>

class Database {
public:
    static bool initialize(QString* errorMessage = nullptr);
    static bool usernameExists(const QString& username, QString* errorMessage = nullptr);
    static bool emailExists(const QString& email, QString* errorMessage = nullptr);
    static bool createUser(const QString& username, const QString& email, const QString& password, QString* errorMessage = nullptr);
    static bool authenticate(const QString& identifier, const QString& password, QString* usernameOut = nullptr, QString* errorMessage = nullptr);

private:
    static QString connectionName();
    static QString databasePath();
    static QByteArray randomSalt();
    static QByteArray hashPassword(const QByteArray& salt, const QString& password);
    static bool ensureInitialized(QString* errorMessage);
};