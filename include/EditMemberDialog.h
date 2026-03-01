#ifndef EDITMEMBERDIALOG_H
#define EDITMEMBERDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class UpdateSubscriptionDialog;
}

class EditMemberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMemberDialog(int userId, DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~EditMemberDialog();

signals:
    void memberUpdated(); // Added signal to notify of successful updates

private slots:
    void on_updateButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::UpdateSubscriptionDialog *ui;
    DatabaseManager* dbManager;
    int userId;

    void loadMemberData();
};

#endif // EDITMEMBERDIALOG_H