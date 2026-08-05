#include "database.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace {
constexpr int kSaltSize = 16;
const QString kConnectionName = QStringLiteral("jhatkaa_auth_connection");
}

QString Database::connectionName() {
    return kConnectionName;
}

QString Database::databasePath() {
    return QCoreApplication::applicationDirPath() + "/users.db";
}

QByteArray Database::randomSalt() {
    QByteArray salt(kSaltSize, Qt::Uninitialized);
    for (int index = 0; index < salt.size(); ++index) {
        salt[index] = static_cast<char>(QRandomGenerator::global()->bounded(0, 256));
    }
    return salt;
}

QByteArray Database::hashPassword(const QByteArray& salt, const QString& password) {
    QByteArray data;
    data.reserve(salt.size() + password.toUtf8().size());
    data.append(salt);
    data.append(password.toUtf8());
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

bool Database::ensureInitialized(QString* errorMessage) {
    QString localError;
    if (!initialize(&localError)) {
        if (errorMessage) {
            *errorMessage = localError;
        }
        return false;
    }
    return true;
}

bool Database::initialize(QString* errorMessage) {
    QSqlDatabase db;
    if (QSqlDatabase::contains(connectionName())) {
        db = QSqlDatabase::database(connectionName());
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    }

    if (!db.isValid()) {
        if (errorMessage) {
            *errorMessage = "SQLite driver is not available.";
        }
        return false;
    }

    if (!db.isOpen()) {
        db.setDatabaseName(databasePath());
        if (!db.open()) {
            if (errorMessage) {
                *errorMessage = "Could not open the local users.db file: " + db.lastError().text();
            }
            return false;
        }
    }

    QSqlQuery query(db);
    const bool tableCreated = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE COLLATE NOCASE, "
        "email TEXT NOT NULL UNIQUE COLLATE NOCASE, "
        "password_hash TEXT NOT NULL, "
        "salt TEXT NOT NULL, "
        "created_at TEXT NOT NULL"
        ")");

    if (!tableCreated) {
        if (errorMessage) {
            *errorMessage = "Could not create the users table: " + query.lastError().text();
        }
        return false;
    }

    return true;
}

bool Database::usernameExists(const QString& username, QString* errorMessage) {
    if (!ensureInitialized(errorMessage)) {
        return false;
    }

    const QString trimmed = username.trimmed();
    QSqlDatabase db = QSqlDatabase::database(connectionName());
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM users WHERE username = :username COLLATE NOCASE LIMIT 1");
    query.bindValue(":username", trimmed);

    if (!query.exec()) {
        if (errorMessage) {
            *errorMessage = "Could not check the username: " + query.lastError().text();
        }
        return false;
    }

    return query.next();
}

bool Database::emailExists(const QString& email, QString* errorMessage) {
    if (!ensureInitialized(errorMessage)) {
        return false;
    }

    const QString trimmed = email.trimmed();
    QSqlDatabase db = QSqlDatabase::database(connectionName());
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM users WHERE email = :email COLLATE NOCASE LIMIT 1");
    query.bindValue(":email", trimmed);

    if (!query.exec()) {
        if (errorMessage) {
            *errorMessage = "Could not check the email address: " + query.lastError().text();
        }
        return false;
    }

    return query.next();
}

bool Database::createUser(const QString& username, const QString& email, const QString& password, QString* errorMessage) {
    if (!ensureInitialized(errorMessage)) {
        return false;
    }

    const QString trimmedUsername = username.trimmed();
    const QString trimmedEmail = email.trimmed();
    if (trimmedUsername.isEmpty() || trimmedEmail.isEmpty() || password.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Username, email, and password are required.";
        }
        return false;
    }

    if (usernameExists(trimmedUsername, errorMessage)) {
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = "That username is already taken.";
        }
        return false;
    }

    if (emailExists(trimmedEmail, errorMessage)) {
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = "That email address is already taken.";
        }
        return false;
    }

    const QByteArray salt = randomSalt();
    const QByteArray passwordHash = hashPassword(salt, password);

    QSqlDatabase db = QSqlDatabase::database(connectionName());
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO users (username, email, password_hash, salt, created_at) "
        "VALUES (:username, :email, :password_hash, :salt, :created_at)");
    query.bindValue(":username", trimmedUsername);
    query.bindValue(":email", trimmedEmail);
    query.bindValue(":password_hash", QString::fromLatin1(passwordHash));
    query.bindValue(":salt", QString::fromLatin1(salt.toHex()));
    query.bindValue(":created_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

    if (!query.exec()) {
        if (errorMessage) {
            *errorMessage = "Could not create the account: " + query.lastError().text();
        }
        return false;
    }

    return true;
}

bool Database::authenticate(const QString& identifier, const QString& password, QString* usernameOut, QString* errorMessage) {
    if (!ensureInitialized(errorMessage)) {
        return false;
    }

    const QString trimmedIdentifier = identifier.trimmed();
    if (trimmedIdentifier.isEmpty() || password.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Enter your username/email and password.";
        }
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName());
    QSqlQuery query(db);
    query.prepare(
        "SELECT username, password_hash, salt "
        "FROM users "
        "WHERE username = :identifier COLLATE NOCASE OR email = :identifier COLLATE NOCASE "
        "LIMIT 1");
    query.bindValue(":identifier", trimmedIdentifier);

    if (!query.exec()) {
        if (errorMessage) {
            *errorMessage = "Could not verify the login: " + query.lastError().text();
        }
        return false;
    }

    if (!query.next()) {
        if (errorMessage) {
            *errorMessage = "Invalid username or password.";
        }
        return false;
    }

    const QString storedUsername = query.value("username").toString();
    const QByteArray storedHash = query.value("password_hash").toByteArray();
    const QByteArray salt = QByteArray::fromHex(query.value("salt").toByteArray());
    const QByteArray calculatedHash = hashPassword(salt, password);

    if (storedHash != calculatedHash) {
        if (errorMessage) {
            *errorMessage = "Invalid username or password.";
        }
        return false;
    }

    if (usernameOut) {
        *usernameOut = storedUsername;
    }

    return true;
}