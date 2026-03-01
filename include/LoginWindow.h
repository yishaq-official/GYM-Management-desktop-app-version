#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "DatabaseManager.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_loginButton_clicked();

private:
    Ui::LoginWindow *ui;
    DatabaseManager* dbManager;
};

#endif // LOGINWINDOW_H