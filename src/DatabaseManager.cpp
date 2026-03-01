#include "DatabaseManager.h"
#include <QDebug>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QODBC");
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::connect() {
    db.setDatabaseName("DRIVER={ODBC Driver 17 for SQL Server};SERVER=DESKTOP-F7H8IR1\\SQLEXPRESS;DATABASE=GymManagementSystem;Trusted_Connection=Yes;");
    
    if (!db.open()) {
        qDebug() << "Database connection failed:" << db.lastError().text();
        return false;
    }
    qDebug() << "Database connected successfully";
    return true;
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password, QString& role) {
    QSqlQuery query(db);
    query.prepare("SELECT Role FROM Users WHERE Username = ? AND Password = ?");
    query.bindValue(0, username);
    query.bindValue(1, password);
    if (!query.exec()) {
        qDebug() << "Authentication query failed:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        role = query.value("Role").toString();
        return true;
    }
    return false;
}

bool DatabaseManager::registerMember(const QString& name, const QString& username, const QString& phone,
                                     const QString& password, const QString& deadline,
                                     bool monday, bool tuesday, bool wednesday, bool thursday,
                                     bool friday, bool saturday, bool sunday) {
    if (!db.isOpen()) {
        qDebug() << "Database not open for registration";
        return false;
    }

    QSqlQuery query(db);
    
    // Validate inputs
    if (name.isEmpty() || username.isEmpty() || phone.isEmpty() || password.isEmpty() || deadline.isEmpty()) {
        qDebug() << "Invalid input: Name, Username, Phone, Password, or Deadline is empty";
        return false;
    }

    // Validate input lengths to prevent truncation
    if (name.length() > 100 || username.length() > 50 || phone.length() > 15 || password.length() > 255) {
        qDebug() << "Input exceeds database column limits";
        return false;
    }

    // Call stored procedure
    query.prepare("EXEC RegisterMember @Name = ?, @Username = ?, @Phone = ?, @Password = ?, @Deadline = ?, "
                  "@Monday = ?, @Tuesday = ?, @Wednesday = ?, @Thursday = ?, @Friday = ?, @Saturday = ?, @Sunday = ?");
    query.bindValue(0, name);
    query.bindValue(1, username);
    query.bindValue(2, phone);
    query.bindValue(3, password);
    query.bindValue(4, deadline);
    query.bindValue(5, monday ? 1 : 0);
    query.bindValue(6, tuesday ? 1 : 0);
    query.bindValue(7, wednesday ? 1 : 0);
    query.bindValue(8, thursday ? 1 : 0);
    query.bindValue(9, friday ? 1 : 0);
    query.bindValue(10, saturday ? 1 : 0);
    query.bindValue(11, sunday ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "RegisterMember stored procedure failed:" << query.lastError().text()
                 << "Name=" << name << ", Username=" << username << ", Phone=" << phone;
        return false;
    }

    return true;
}

bool DatabaseManager::updateSubscription(int userId, const QString& newDeadline) {
    if (!db.isOpen()) {
        qDebug() << "Database not open for subscription update";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("EXEC UpdateSubscription @UserID = ?, @NewDeadline = ?, @Amount = ?");
    query.bindValue(0, userId);
    query.bindValue(1, newDeadline);
    query.bindValue(2, 50.00); // Default amount, adjust as needed

    if (!query.exec()) {
        qDebug() << "UpdateSubscription stored procedure failed:" << query.lastError().text();
        return false;
    }
    return true;
}
