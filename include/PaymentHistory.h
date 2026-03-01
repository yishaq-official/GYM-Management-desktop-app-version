#ifndef PAYMENTHISTORY_H
#define PAYMENTHISTORY_H

#include <QObject>
#include <QWidget>
#include <QDateTime> // Added to resolve QDateTime incomplete type

struct PaymentNode {
    int userId;
    QString status;
    QDateTime timestamp;
    PaymentNode* next;
};

class PaymentHistory : public QWidget
{
    Q_OBJECT
public:
    explicit PaymentHistory(QWidget* parent = nullptr);
    void addPayment(int userId, const QString& status);
    void showHistory(); // Placeholder for slideshow logic

private:
    PaymentNode* head;
};

#endif // PAYMENTHISTORY_H