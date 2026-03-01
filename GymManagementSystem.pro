QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    src/AdminDashboard.cpp \
    src/AdminDialog.cpp \
    src/DatabaseManager.cpp \
    src/EditMemberDialog.cpp \
    src/LoginWindow.cpp \
    src/ViewMembersDialog.cpp \
    src/main.cpp \
    src/MembersDashboard.cpp \
    src/PaymentQueue.cpp \
    src/PaymentHistory.cpp

HEADERS += \
    include/AdminDashboard.h \
    include/AdminDialog.h \
    include/DatabaseManager.h \
    include/EditMemberDialog.h \
    include/LoginWindow.h \
    include/ViewMembersDialog.h \
    include/MembersDashboard.h \
    include/PaymentQueue.h \
    include/PaymentHistory.h

FORMS += \
    forms/AdminDashboard.ui \
    forms/AdminDialog.ui \
    forms/LoginWindow.ui \
    forms/UpdateSubscriptionDialog.ui \
    forms/ViewMembersDialog.ui \
    forms/MembersDashboard.ui

INCLUDEPATH += include
DEPENDPATH += include src
