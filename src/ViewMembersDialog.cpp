#include "ViewMembersDialog.h"
#include "ui_ViewMembersDialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QDate>
#include "EditMemberDialog.h"

ViewMembersDialog::ViewMembersDialog(int userId, DatabaseManager* dbManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewMembersDialog),
    dbManager(dbManager),
    userId(userId)
{
    ui->setupUi(this);
    qDebug() << "ViewMembersDialog initialized for UserID=" << userId;

    // Connect search bar
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &ViewMembersDialog::on_searchEdit_textChanged);

    // Connect refresh button
    connect(ui->refreshButton, &QPushButton::clicked, this, &ViewMembersDialog::on_refreshButton_clicked);

    // Connect table double-click
    connect(ui->membersTable, &QTableWidget::cellDoubleClicked, this, &ViewMembersDialog::on_membersTable_cellDoubleClicked);

    // Initialize table
    ui->membersTable->setColumnCount(9);
    ui->membersTable->setHorizontalHeaderLabels({
        "ID", "Name", "Username", "Phone", "Deadline", "Amount", "Payment Status", "Schedule", "Action"
    });
    ui->membersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->membersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->membersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Load initial data
    loadAllMembers();
}

ViewMembersDialog::~ViewMembersDialog() {
    delete ui;
}

void ViewMembersDialog::loadAllMembers() {
    allMembers.clear(); // Clear previous data
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT u.ID, u.Name, u.Username, u.Phone, s.Deadline, s.Amount, p.Status, "
                  "STRING_AGG(sch.DayOfWeek, ',') AS ScheduleDays "
                  "FROM Users u "
                  "LEFT JOIN Subscriptions s ON u.ID = s.UserID "
                  "LEFT JOIN Payments p ON u.ID = p.UserID "
                  "LEFT JOIN Schedules sch ON u.ID = sch.UserID "
                  "WHERE u.Role = 'Member' "
                  "GROUP BY u.ID, u.Name, u.Username, u.Phone, s.Deadline, s.Amount, p.Status");
    if (!query.exec()) {
        qDebug() << "Failed to load all members:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to load members: " + query.lastError().text());
        return;
    }

    while (query.next()) {
        MemberData member;
        member.id = query.value("ID").toInt();
        member.name = query.value("Name").toString();
        member.username = query.value("Username").toString();
        member.phone = query.value("Phone").toString();
        member.deadline = query.value("Deadline").toDate();
        member.amount = query.value("Amount").toDouble();
        member.paymentStatus = query.value("Status").toString();
        member.scheduleDays = query.value("ScheduleDays").toString().split(",", Qt::SkipEmptyParts);
        allMembers.append(member);
    }
    refreshTable(); // Initial display
}

void ViewMembersDialog::refreshTable() {
    ui->membersTable->setRowCount(0);
    QString searchText = ui->searchEdit->text().toLower();
    int row = 0;

    for (const MemberData& member : allMembers) {
        if (searchText.isEmpty() || member.name.toLower().contains(searchText)) {
            ui->membersTable->insertRow(row);
            ui->membersTable->setItem(row, 0, new QTableWidgetItem(QString::number(member.id)));
            ui->membersTable->setItem(row, 1, new QTableWidgetItem(member.name));
            ui->membersTable->setItem(row, 2, new QTableWidgetItem(member.username));
            ui->membersTable->setItem(row, 3, new QTableWidgetItem(member.phone));
            ui->membersTable->setItem(row, 4, new QTableWidgetItem(member.deadline.toString("yyyy-MM-dd")));
            ui->membersTable->setItem(row, 5, new QTableWidgetItem(QString::number(member.amount)));
            ui->membersTable->setItem(row, 6, new QTableWidgetItem(member.paymentStatus));
            ui->membersTable->setItem(row, 7, new QTableWidgetItem(member.scheduleDays.join(""))); // Join without separator for "TTS" or "MWF"

            QPushButton* removeButton = new QPushButton("Remove", this);
            removeButton->setProperty("userId", member.id);
            removeButton->setProperty("name", member.name);
            connect(removeButton, &QPushButton::clicked, this, [this, removeButton]() {
                on_removeButton_clicked(removeButton->property("userId").toInt(),
                                      removeButton->property("name").toString());
            });
            ui->membersTable->setCellWidget(row, 8, removeButton);

            row++;
        }
    }
    qDebug() << "Displayed" << row << "members after search";
}

void ViewMembersDialog::on_searchEdit_textChanged() {
    refreshTable();
}

void ViewMembersDialog::on_refreshButton_clicked() {
    ui->searchEdit->clear();
    loadAllMembers(); // Reload all data from database
}

void ViewMembersDialog::on_membersTable_cellDoubleClicked(int row, int column) {
    Q_UNUSED(column); // Suppress unused parameter warning

    QTableWidgetItem* idItem = ui->membersTable->item(row, 0);
    if (!idItem) return;

    bool ok;
    int userId = idItem->text().toInt(&ok);
    if (!ok) {
        qDebug() << "Invalid UserID in table";
        return;
    }

    EditMemberDialog* dialog = new EditMemberDialog(userId, dbManager, this);
    connect(dialog, &EditMemberDialog::memberUpdated, this, &ViewMembersDialog::refreshTable);
    dialog->exec();
}

void ViewMembersDialog::on_removeButton_clicked(int userId, const QString &name) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deletion",
        QString("Are you sure you want to remove %1 (ID: %2)?").arg(name).arg(userId),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::No) return;

    QSqlQuery query(dbManager->getDatabase());
    query.prepare("DELETE FROM Payments WHERE UserID = ?");
    query.bindValue(0, userId);
    if (!query.exec()) {
        qDebug() << "Failed to delete payments:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete payments: " + query.lastError().text());
        return;
    }

    query.prepare("DELETE FROM Schedules WHERE UserID = ?");
    query.bindValue(0, userId);
    if (!query.exec()) {
        qDebug() << "Failed to delete schedules:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete schedules: " + query.lastError().text());
        return;
    }

    query.prepare("DELETE FROM Subscriptions WHERE UserID = ?");
    query.bindValue(0, userId);
    if (!query.exec()) {
        qDebug() << "Failed to delete subscriptions:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete subscriptions: " + query.lastError().text());
        return;
    }

    query.prepare("DELETE FROM Users WHERE ID = ?");
    query.bindValue(0, userId);
    if (!query.exec()) {
        qDebug() << "Failed to delete user:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete user: " + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Success", QString("Member %1 removed successfully.").arg(name));
    loadAllMembers(); // Reload all data after removal
}