#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QStack>
#include "DatabaseManager.h"
#include "AdminDialog.h"
#include "ViewMembersDialog.h"
#include "PaymentQueue.h"
#include "PaymentHistory.h"

namespace Ui {
class AdminDashboard;
}

class AdminDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDashboard(int userId, DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~AdminDashboard();

private slots:
    void onRegisterClicked();
    void onViewClicked();
    void onRemoveClicked();
    void onBackClicked();
    void onRefreshPayments();

private:
    Ui::AdminDashboard *ui;
    DatabaseManager* dbManager;
    int userId;
    QStackedWidget* stackedWidget;
    QPushButton* backButton;
    QStack<int> navigationStack;
    AdminDialog* registrationDialog;
    ViewMembersDialog* viewMembersDialog;
    PaymentQueue* paymentQueue;
    PaymentHistory* paymentHistory;
};

#endif // ADMINDASHBOARD_H