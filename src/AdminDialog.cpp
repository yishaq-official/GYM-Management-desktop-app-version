#include "AdminDialog.h"
#include "ui_AdminDialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QDate>

AdminDialog::AdminDialog(int userId, DatabaseManager* dbManager, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AdminDialog),
    dbManager(dbManager),
    userId(userId)
{
    ui->setupUi(this);
    qDebug() << "AdminDialog initialized for Admin UserID=" << userId;

    ui->deadlineEdit->setMinimumDate(QDate::currentDate());

    connect(ui->registerButton, &QPushButton::clicked, this, &AdminDialog::on_registerButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AdminDialog::on_cancelButton_clicked);
}

AdminDialog::~AdminDialog() {
    delete ui;
}

void AdminDialog::on_registerButton_clicked() {
    QString name = ui->nameEdit->text().trimmed();
    QString username = ui->usernameEdit->text().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    QDate deadline = ui->deadlineEdit->date();
    bool ok;
    double amount = ui->amountEdit->text().toDouble(&ok);
    QString schedule = ui->scheduleCombo->currentText();

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

    bool monday = schedule.contains("M");
    bool tuesday = schedule.contains("T") && !schedule.contains("Th");
    bool wednesday = schedule.contains("W");
    bool thursday = schedule.contains("Th");
    bool friday = schedule.contains("F");
    bool saturday = schedule.contains("Sa");
    bool sunday = schedule.contains("Su");

    if (!dbManager->registerMember(name, username, phone, password, deadline.toString("yyyy-MM-dd"),
                                   monday, tuesday, wednesday, thursday, friday, saturday, sunday)) {
        qDebug() << "Failed to register member";
        QMessageBox::critical(this, "Error", "Failed to register member. Check database logs.");
        return;
    }

    QMessageBox::information(this, "Success", "Member registered successfully.");
    accept();
}

void AdminDialog::on_cancelButton_clicked() {
    reject();
}

