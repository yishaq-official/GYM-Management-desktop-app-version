#ifndef PAYMENTQUEUE_H
#define PAYMENTQUEUE_H

#include <QQueue>
#include <QObject>
#include <QDateTime> // Added to resolve QDateTime incomplete type
#include "DatabaseManager.h"

struct PaymentRequest {
    int userId;
    QString status;
    QDateTime timestamp;
};

class PaymentQueue : public QObject
{
    Q_OBJECT
public:
    explicit PaymentQueue(DatabaseManager* dbManager, QObject* parent = nullptr);
    void enqueuePayment(int userId, const QString& status);
    void processQueue();

private:
    QQueue<PaymentRequest> queue;
    DatabaseManager* dbManager;
};

#endif // PAYMENTQUEUE_H