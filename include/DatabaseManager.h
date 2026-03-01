#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariantList>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool connect();
    bool authenticateUser(const QString& username, const QString& password, QString& role);
    bool registerMember(const QString& name, const QString& username, const QString& phone,
                       const QString& password, const QString& deadline,
                       bool monday, bool tuesday, bool wednesday, bool thursday,
                       bool friday, bool saturday, bool sunday);
    bool updateSubscription(int userId, const QString& newDeadline);
    QSqlDatabase getDatabase() const { return db; }

private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H