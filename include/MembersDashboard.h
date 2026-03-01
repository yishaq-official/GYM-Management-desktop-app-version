#ifndef MEMBERSDASHBOARD_H
#define MEMBERSDASHBOARD_H

#include <QWidget>
#include "DatabaseManager.h"

namespace Ui {
class MembersDashboard;
}

class MembersDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit MembersDashboard(int userId, DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~MembersDashboard();

private slots:
    void on_refreshPaymentButton_clicked();
    void on_changePasswordButton_clicked();

private:
    Ui::MembersDashboard *ui;
    DatabaseManager* dbManager;
    int userId;

    void loadData();
    void updateSubscriptionDeadline();
    void updatePaymentStatus();
    void updateSchedule();
};

#endif // MEMBERSDASHBOARD_H