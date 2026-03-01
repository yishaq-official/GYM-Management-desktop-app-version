#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include "AdminDashboard.h"
#include "MembersDashboard.h"  // Added include for MembersDashboard
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>  // For database queries

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow),
    dbManager(new DatabaseManager())
{
    ui->setupUi(this);
    qDebug() << "LoginWindow initialized";

    if (!dbManager->connect()) {
        qDebug() << "Database connection failed in LoginWindow";
        QMessageBox::critical(this, "Error", "Failed to connect to the database.");
    } else {
        qDebug() << "Database connected successfully in LoginWindow";
    }

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::on_loginButton_clicked);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::on_loginButton_clicked() {
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter both username and password.");
        return;
    }

    QString role;
    if (!dbManager->authenticateUser(username, password, role)) {
        qDebug() << "Authentication failed for Username:" << username;
        QMessageBox::critical(this, "Error", "Invalid username or password.");
        return;
    }

    qDebug() << "Login successful: Username =" << username << ", Role =" << role;

    if (role == "Admin") {
        AdminDashboard* dashboard = new AdminDashboard(1, dbManager); // Assuming admin ID is 1
        dashboard->show();
        this->close();
    } else {
        // Fetch userId for the member
        QSqlQuery query(dbManager->getDatabase());
        query.prepare("SELECT ID FROM Users WHERE Username = ? AND Password = ?");
        query.bindValue(0, username);
        query.bindValue(1, password);

        if (!query.exec() || !query.next()) {
            qDebug() << "Failed to fetch user ID:" << query.lastError().text();
            QMessageBox::critical(this, "Error", "Failed to load user data.");
            return;
        }

        int userId = query.value("ID").toInt();
        MembersDashboard* dashboard = new MembersDashboard(userId, dbManager);
        dashboard->show();
        this->close();
    }
}
