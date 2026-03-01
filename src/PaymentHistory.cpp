#include "PaymentHistory.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDebug>

PaymentHistory::PaymentHistory(QWidget* parent) : QWidget(parent), head(nullptr) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addStretch();
    setLayout(layout);
}

void PaymentHistory::addPayment(int userId, const QString& status) {
    PaymentNode* newNode = new PaymentNode{userId, status, QDateTime::currentDateTime(), nullptr};
    if (!head) {
        head = newNode;
    } else {
        PaymentNode* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    qDebug() << "Added payment history for UserID" << userId;
    showHistory(); // Trigger slideshow update
}

void PaymentHistory::showHistory() {
    PaymentNode* current = head;
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
    layout->removeWidget(layout->itemAt(0)->widget()); // Clear previous label
    while (current) {
        QLabel* label = new QLabel(QString("UserID: %1, Status: %2, Time: %3")
                                   .arg(current->userId).arg(current->status).arg(current->timestamp.toString()));
        layout->insertWidget(0, label);
        current = current->next;
    }
    // Add basic slideshow logic with QTimer (simplified)
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, layout]() {
        if (layout->count() > 1) {
            layout->removeWidget(layout->itemAt(1)->widget());
        }
    });
    timer->start(2000); // Update every 2 seconds
}