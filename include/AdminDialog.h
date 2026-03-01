#ifndef ADMINDIALOG_H
#define ADMINDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class AdminDialog;
}

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(int userId, DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~AdminDialog();

private slots:
    void on_registerButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AdminDialog *ui;
    DatabaseManager* dbManager;
    int userId;
};

#endif // ADMINDIALOG_H