#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "MemberLogin.h"
#include <vector>

struct Node {
    Member data;
    Node* next;
    Node(const Member& m) : data(m), next(nullptr) {}
};

class LinkedList {
public:
    LinkedList() : head(nullptr) {}
    ~LinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void addMember(const Member& member) {
        Node* newNode = new Node(member);
        newNode->next = head;
        head = newNode;
    }
    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
    }
    std::vector<Member> toVector() const {
        std::vector<Member> result;
        Node* current = head;
        while (current) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }

private:
    Node* head;
};

#endif // LINKEDLIST_H