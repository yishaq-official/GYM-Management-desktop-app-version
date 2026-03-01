#ifndef VIEWMEMBERSDIALOG_H
#define VIEWMEMBERSDIALOG_H

#include <QWidget>
#include "DatabaseManager.h"
#include "EditMemberDialog.h"
#include <QList>
#include <QDate> // Added to resolve QDate incomplete type error

namespace Ui {
class ViewMembersDialog;
}

struct MemberData {
    int id;
    QString name;
    QString username;
    QString phone;
    QDate deadline;
    double amount;
    QString paymentStatus;
    QStringList scheduleDays;
};

class ViewMembersDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ViewMembersDialog(int userId, DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~ViewMembersDialog();

public slots:
    void refreshTable();

private slots:
    void on_searchEdit_textChanged();
    void on_refreshButton_clicked();
    void on_membersTable_cellDoubleClicked(int row, int column);
    void on_removeButton_clicked(int userId, const QString &name);

private:
    Ui::ViewMembersDialog *ui;
    DatabaseManager* dbManager;
    int userId;
    QList<MemberData> allMembers;

    void loadMembers(const QString &searchName = QString());
    void loadAllMembers();
};

#endif // VIEWMEMBERSDIALOG_H