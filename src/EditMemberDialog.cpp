#include "EditMemberDialog.h"
#include "ui_UpdateSubscriptionDialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>

EditMemberDialog::EditMemberDialog(int userId, DatabaseManager* dbManager, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::UpdateSubscriptionDialog),
    dbManager(dbManager),
    userId(userId)
{
    ui->setupUi(this);
    qDebug() << "EditMemberDialog initialized for UserID=" << userId;

    ui->deadlineEdit->setMinimumDate(QDate::currentDate());

    connect(ui->updateButton, &QPushButton::clicked, this, &EditMemberDialog::on_updateButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EditMemberDialog::on_cancelButton_clicked);

    loadMemberData();
}

EditMemberDialog::~EditMemberDialog() {
    delete ui;
}

void EditMemberDialog::loadMemberData() {
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT u.Name, u.Username, u.Phone, u.Password, s.Deadline, s.Amount, sch.DayOfWeek, p.Status "
                  "FROM Users u "
                  "LEFT JOIN Subscriptions s ON u.ID = s.UserID "
                  "LEFT JOIN Schedules sch ON u.ID = sch.UserID "
                  "LEFT JOIN Payments p ON u.ID = p.UserID "
                  "WHERE u.ID = ?");
    query.bindValue(0, userId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to load member data:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to load member data: " + query.lastError().text());
        close();
        return;
    }

    ui->nameEdit->setText(query.value("Name").toString());
    ui->usernameEdit->setText(query.value("Username").toString());
    ui->phoneEdit->setText(query.value("Phone").toString());
    ui->passwordEdit->setText(query.value("Password").toString());
    QDate deadline = query.value("Deadline").toDate();
    if (deadline.isValid()) {
        ui->deadlineEdit->setDate(deadline);
    } else {
        ui->deadlineEdit->setDate(QDate::currentDate());
    }
    ui->amountEdit->setText(query.value("Amount").toString());

    QString dayOfWeek = query.value("DayOfWeek").toString();
    int scheduleIndex = ui->scheduleCombo->findText(dayOfWeek, Qt::MatchContains);
    ui->scheduleCombo->setCurrentIndex(scheduleIndex != -1 ? scheduleIndex : 0);

    QString paymentStatus = query.value("Status").toString();
    int paymentIndex = ui->paymentStatusCombo->findText(paymentStatus);
    ui->paymentStatusCombo->setCurrentIndex(paymentIndex != -1 ? paymentIndex : 0);
}

void EditMemberDialog::on_updateButton_clicked() {
    QString name = ui->nameEdit->text().trimmed();
    QString username = ui->usernameEdit->text().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    QDate deadline = ui->deadlineEdit->date();
    bool ok;
    double amount = ui->amountEdit->text().toDouble(&ok);
    QString dayOfWeek = ui->scheduleCombo->currentText().split(" - ")[0]; // e.g., "MWF - 8AM" -> "MWF"
    QString paymentStatus = ui->paymentStatusCombo->currentText();

    if (name.isEmpty() || username.isEmpty() || phone.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields must be filled.");
        return;
    }

    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid amount.");
        return;
    }

    if (deadline < QDate::currentDate()) {
        QMessageBox::warning(this, "Input Error", "Deadline cannot be in the past.");
        return;
    }

    QSqlQuery query(dbManager->getDatabase());
    // Delete existing schedules for the user
    query.prepare("DELETE FROM Schedules WHERE UserID = ?");
    query.bindValue(0, userId);
    if (!query.exec()) {
        qDebug() << "Failed to delete existing schedules:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to update schedules: " + query.lastError().text());
        return;
    }

    // Insert new schedule days based on the selected dayOfWeek
    QStringList days = dayOfWeek.split("", Qt::SkipEmptyParts); // Split "MWF" into ["M", "W", "F"]
    for (const QString& day : days) {
        query.prepare("INSERT INTO Schedules (UserID, DayOfWeek, IsActive) VALUES (?, ?, 1)");
        query.bindValue(0, userId);
        query.bindValue(1, day);
        if (!query.exec()) {
            qDebug() << "Failed to insert schedule:" << query.lastError().text();
            QMessageBox::critical(this, "Error", "Failed to update schedule: " + query.lastError().text());
            return;
        }
    }

    // Update other member details
    query.prepare("EXEC UpdateMemberDetails @UserID = ?, @Name = ?, @Username = ?, @Phone = ?, @Password = ?, "
                  "@Deadline = ?, @Amount = ?, @DayOfWeek = ?, @PaymentStatus = ?");
    query.bindValue(0, userId);
    query.bindValue(1, name);
    query.bindValue(2, username);
    query.bindValue(3, phone);
    query.bindValue(4, password);
    query.bindValue(5, deadline.toString("yyyy-MM-dd"));
    query.bindValue(6, amount);
    query.bindValue(7, dayOfWeek); // Pass the full dayOfWeek string
    query.bindValue(8, paymentStatus);

    if (!query.exec()) {
        qDebug() << "UpdateMemberDetails failed:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to update member: " + query.lastError().text());
        return;
    }

    qDebug() << "Successfully updated UserID=" << userId;
    QMessageBox::information(this, "Success", "Member details updated successfully.");
    emit memberUpdated(); // Emit signal on successful update
    accept();
}

void EditMemberDialog::on_cancelButton_clicked() {
    reject();
}