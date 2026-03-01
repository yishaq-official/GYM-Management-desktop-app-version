#include "PaymentQueue.h"
#include <QSqlQuery>
#include <QDebug>

PaymentQueue::PaymentQueue(DatabaseManager* dbManager, QObject* parent) : QObject(parent), dbManager(dbManager) {}

void PaymentQueue::enqueuePayment(int userId, const QString& status) {
    PaymentRequest request = {userId, status, QDateTime::currentDateTime()};
    queue.enqueue(request);
    qDebug() << "Enqueued payment update for UserID" << userId;
    processQueue(); // Process immediately or use a timer for batching
}

void PaymentQueue::processQueue() {
    while (!queue.isEmpty()) {
        PaymentRequest request = queue.dequeue();
        QSqlQuery query(dbManager->getDatabase());
        query.prepare("UPDATE Payments SET Status = ?, PaymentDate = ? WHERE UserID = ?");
        query.bindValue(0, request.status);
        query.bindValue(1, request.timestamp);
        query.bindValue(2, request.userId);
        if (!query.exec()) {
            qDebug() << "Failed to update payment:" << query.lastError().text();
        } else {
            qDebug() << "Updated payment for UserID" << request.userId;
        }
    }
}