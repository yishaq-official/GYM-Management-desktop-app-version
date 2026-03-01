#include "AdminDashboard.h"
#include "ui_AdminDashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

AdminDashboard::AdminDashboard(int userId, DatabaseManager* dbManager, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AdminDashboard),
    dbManager(dbManager),
    userId(userId),
    registrationDialog(nullptr),
    viewMembersDialog(nullptr),
    paymentQueue(new PaymentQueue(dbManager, this)),
    paymentHistory(new PaymentHistory(this))
{
    ui->setupUi(this);
    qDebug() << "AdminDashboard initialized for UserID=" << userId;

    // Initialize navigation components
    stackedWidget = new QStackedWidget(this);
    backButton = new QPushButton("Back", this);
    backButton->setEnabled(false);
    backButton->setStyleSheet("QPushButton { background-color: #1E3A8A; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold; } "
                              "QPushButton:hover { background-color: #3B82F6; } "
                              "QPushButton:disabled { background-color: #A9A9A9; }");

    // Use the existing UI widget as the dashboard page
    QWidget* dashboardWidget = new QWidget(this);
    QVBoxLayout* dashboardLayout = new QVBoxLayout(dashboardWidget);
    dashboardLayout->addWidget(ui->titleLabel);
    dashboardLayout->addWidget(ui->actionsGroupBox);
    dashboardLayout->addWidget(paymentHistory); // Add payment history display
    dashboardLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    dashboardWidget->setLayout(dashboardLayout);

    // Create dialog pages
    registrationDialog = new AdminDialog(userId, dbManager, this);
    viewMembersDialog = new ViewMembersDialog(userId, dbManager, this);

    // Add pages to stacked widget
    stackedWidget->addWidget(dashboardWidget); // Index 0: Dashboard
    stackedWidget->addWidget(registrationDialog); // Index 1: Registration
    stackedWidget->addWidget(viewMembersDialog); // Index 2: View Members

    // Replace the stackedWidgetPlaceholder with the actual stackedWidget
    ui->verticalLayout->replaceWidget(ui->stackedWidgetPlaceholder, stackedWidget);
    delete ui->stackedWidgetPlaceholder; // Clean up placeholder

    // Add back button in a top-right fixed position
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch(); // Push back button to the right
    topLayout->addWidget(backButton);
    topLayout->setContentsMargins(0, 0, 10, 0); // Small right margin
    ui->verticalLayout->insertLayout(0, topLayout); // Insert at the top

    // Connect existing buttons to new slots
    connect(ui->registerButton, &QPushButton::clicked, this, &AdminDashboard::onRegisterClicked);
    connect(ui->viewButton, &QPushButton::clicked, this, &AdminDashboard::onViewClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &AdminDashboard::onRemoveClicked);
    connect(backButton, &QPushButton::clicked, this, &AdminDashboard::onBackClicked);

    // Add refresh payment button functionality (assuming a refresh button exists or can be added)
    QPushButton* refreshButton = new QPushButton("Refresh Payments", this);
    refreshButton->setStyleSheet("QPushButton { background-color: #1E3A8A; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold; } "
                                 "QPushButton:hover { background-color: #3B82F6; }");
    ui->verticalLayout_2->insertWidget(3, refreshButton); // Add after removeButton
    connect(refreshButton, &QPushButton::clicked, this, &AdminDashboard::onRefreshPayments);

    // Set initial page
    stackedWidget->setCurrentWidget(dashboardWidget);
}

AdminDashboard::~AdminDashboard()
{
    delete ui;
}

void AdminDashboard::onRegisterClicked()
{
    navigationStack.push(stackedWidget->currentIndex());
    stackedWidget->setCurrentWidget(registrationDialog);
    backButton->setEnabled(true);
}

void AdminDashboard::onViewClicked()
{
    navigationStack.push(stackedWidget->currentIndex());
    stackedWidget->setCurrentWidget(viewMembersDialog);
    backButton->setEnabled(true);
}

void AdminDashboard::onRemoveClicked()
{
    navigationStack.push(stackedWidget->currentIndex());
    stackedWidget->setCurrentWidget(viewMembersDialog); // Reuse view dialog for remove
    backButton->setEnabled(true);
}

void AdminDashboard::onBackClicked()
{
    if (!navigationStack.isEmpty()) {
        int prevIndex = navigationStack.pop();
        stackedWidget->setCurrentIndex(prevIndex);
        backButton->setEnabled(!navigationStack.isEmpty());
    }
}

void AdminDashboard::onRefreshPayments()
{
    // Example: Refresh payment status for all members (modify as needed)
    QSqlQuery query(dbManager->getDatabase());
    query.prepare("SELECT UserID FROM Members");
    if (query.exec()) {
        while (query.next()) {
            int userId = query.value("UserID").toInt();
            paymentQueue->enqueuePayment(userId, "Processed"); // Example status
        }
    }
    paymentHistory->addPayment(userId, "Payment Refreshed"); // Log the action
}

