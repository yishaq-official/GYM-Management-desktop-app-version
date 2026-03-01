#include "MembersDashboard.h"
#include "ui_MembersDashboard.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

MembersDashboard::MembersDashboard(int userId, DatabaseManager* dbManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MembersDashboard),
    dbManager(dbManager),
    userId(userId)
{
    ui->setupUi(this);
    // Set day names in the schedule table
    QString days[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for (int row = 0; row < 7; ++row) {
        QTableWidgetItem* dayItem = new QTableWidgetItem(days[row]);
        ui->scheduleTable->setItem(row, 0, dayItem);
    }
    loadData();
}

MembersDashboard::~MembersDashboard()
{
    delete ui;
}

void MembersDashboard::loadData()
{
    if (!dbManager || !dbManager->getDatabase().isOpen()) {
        QMessageBox::critical(this, "Error", "Database connection is not available.");
        return;
    }

    // Fetch and set the member's name for the welcome message
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT Name FROM Users WHERE ID = ?");
    query.bindValue(0, userId);
    if (query.exec() && query.next()) {
        QString name = query.value("Name").toString();
        ui->headerLabel->setText("Welcome, " + name);
    } else {
        ui->headerLabel->setText("Welcome, Member");
        qDebug() << "Failed to load member's name:" << query.lastError().text();
    }

    // Existing data loading functions
    updateSubscriptionDeadline();
    updatePaymentStatus();
    updateSchedule();
}

void MembersDashboard::updateSubscriptionDeadline()
{
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT Deadline FROM Subscriptions WHERE UserID = ?");
    query.bindValue(0, userId);
    if (query.exec() && query.next()) {
        QDate deadline = query.value("Deadline").toDate();
        ui->subscriptionLabel->setText("Subscription Deadline: " + deadline.toString("yyyy-MM-dd"));
        QDate today = QDate::currentDate();
        if (deadline < today) {
            ui->subscriptionLabel->setStyleSheet("color: red;");
        } else if (today.daysTo(deadline) <= 7) {
            ui->subscriptionLabel->setStyleSheet("color: orange;");
        }
    } else {
        ui->subscriptionLabel->setText("Subscription Deadline: Not available");
        qDebug() << "Failed to load subscription:" << query.lastError().text();
    }
}

void MembersDashboard::updatePaymentStatus()
{
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT TOP 1 Status FROM Payments WHERE UserID = ? ORDER BY PaymentDate DESC");
    query.bindValue(0, userId);
    if (query.exec() && query.next()) {
        ui->paymentStatusLabel->setText("Payment Status: " + query.value("Status").toString());
    } else {
        ui->paymentStatusLabel->setText("Payment Status: Not available");
    }
}

void MembersDashboard::updateSchedule()
{
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT DayOfWeek, IsActive FROM Schedules WHERE UserID = ?");
    query.bindValue(0, userId);
    if (query.exec()) {
        QMap<QString, int> scheduleMap;
        while (query.next()) {
            scheduleMap[query.value("DayOfWeek").toString()] = query.value("IsActive").toInt();
        }
        for (int row = 0; row < 7; ++row) {
            QTableWidgetItem* dayItem = ui->scheduleTable->item(row, 0);
            if (dayItem) {
                QString day = dayItem->text();
                int status = scheduleMap.value(day, 0);
                QTableWidgetItem* statusItem = new QTableWidgetItem(status ? "Active" : "Inactive");
                ui->scheduleTable->setItem(row, 1, statusItem);
            }
        }
    } else {
        qDebug() << "Failed to load schedule:" << query.lastError().text();
    }
}

void MembersDashboard::on_refreshPaymentButton_clicked()
{
    updatePaymentStatus();
}

void MembersDashboard::on_changePasswordButton_clicked()
{
    QString currentPassword = ui->currentPasswordEdit->text();
    QString newPassword = ui->newPasswordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "Error", "New passwords do not match.");
        return;
    }

    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT Password FROM Users WHERE ID = ?");
    query.bindValue(0, userId);
    if (query.exec() && query.next()) {
        if (query.value("Password").toString() == currentPassword) {
            query.prepare("UPDATE Users SET Password = ? WHERE ID = ?");
            query.bindValue(0, newPassword);
            query.bindValue(1, userId);
            if (query.exec()) {
                QMessageBox::information(this, "Success", "Password changed successfully.");
                ui->currentPasswordEdit->clear();
                ui->newPasswordEdit->clear();
                ui->confirmPasswordEdit->clear();
            } else {
                QMessageBox::critical(this, "Error", "Failed to change password: " + query.lastError().text());
            }
        } else {
            QMessageBox::warning(this, "Error", "Current password is incorrect.");
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to verify password: " + query.lastError().text());
    }
}
