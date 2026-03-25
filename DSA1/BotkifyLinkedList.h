#ifndef BOTKIFY_LINKED_LIST_H
#define BOTKIFY_LINKED_LIST_H

#include "main.h"

template <class T> //class = typename
class BotkifyLinkedList
{
private:
    class Node
    {
    public:
        T data;
        Node* next;
        Node* extra;

        Node();
        Node(const T& data, Node* next = nullptr, Node* extra = nullptr);
    };

    Node* head;
    Node* tail;
    int count;

public:
    BotkifyLinkedList();
    ~BotkifyLinkedList();

    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item);

    bool empty() const;
    int size() const;
    void clear();

    T& get(int index) const;

    string toString() const;
};

//node
template <class T>
BotkifyLinkedList<T>::Node::Node() {
    next = nullptr;
    extra = nullptr;
}

template <class T>
BotkifyLinkedList<T>::Node::Node(const T& data, Node* next, Node* extra) {
    this->data = data;
    this->next = next;
    this->extra = extra;
}


// Botkify
template <typename T>
BotkifyLinkedList<T>::BotkifyLinkedList() {
    head = nullptr;
    count = 0;
}

template <typename T>
BotkifyLinkedList<T>::~BotkifyLinkedList(){
    clear();
}

template <typename T>
void BotkifyLinkedList<T>::add(T e){
    Node* n = new Node(e);
    if (!head) head = n;
    else {
        Node* cur = head;                     
        while (cur->next) cur = cur->next;
        cur->next = n;
    }
    count++;
}

template <class T>
void BotkifyLinkedList<T>::add(int index, T e) {
    if (index < 0 || index > count) 
    throw out_of_range("Index is invalid!");

    if (index == 0) {
        Node* n = new Node(e, head);
        head = n;
        if (count == 0) tail = n;
    } 
    else if (index == count) {
        add(e);
        return;
    } 
    else {
        Node* cur = head;
        for (int i = 0; i < index - 1; i++)
            cur = cur->next;
        Node* n = new Node(e, cur->next);
        cur->next = n;
    }
    count++;
}

template <class T>
T BotkifyLinkedList<T>::removeAt(int index) {
    if (index < 0 || index >= count) {
        throw out_of_range("Index is invalid!");
    }

    typename BotkifyLinkedList<T>::Node* removed = nullptr;
    T result;

    if (index == 0) {
        removed = head;
        head = head->next;
        if (removed == tail) {
            tail = nullptr;
        }
    } else {
        typename BotkifyLinkedList<T>::Node* cur = head;
        for (int i = 0; i < index - 1; ++i) {
            cur = cur->next;
        }
        removed = cur->next;
        cur->next = removed->next;
        if (removed == tail) {
            tail = cur;
        }
    }

    result = removed->data;
    delete removed;
    --count;
    return result;
}

template<class T>
bool BotkifyLinkedList<T>::removeItem(T item) {
    Node* cur = head;      
    Node* prev = nullptr; 

    while (cur != nullptr) {
        if (cur->data == item) {
            if (prev == nullptr) {
                head = cur->next;
            } else {
                prev->next = cur->next;
            }
            delete cur;
            count--;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }

    return false;
}

template <typename T>
bool BotkifyLinkedList<T>::empty() const {
    return count == 0;
}

template <typename T>
int BotkifyLinkedList<T>::size() const {
    return count;
}

template <typename T>
void BotkifyLinkedList<T>::clear() {
    while (head) {
        Node* tmp = head;
        head = head->next;
        delete tmp;
    }
    count = 0;
}


template <typename T>
T& BotkifyLinkedList<T>::get(int index) const {
    typename BotkifyLinkedList<T>::Node* cur = head;
    if (index < 0 || index >= count) {
        throw out_of_range("Index is invalid!");
    }
    for (int i = 0; i < index; i++) {
        cur = cur->next;
    }
    return cur->data;
}

template <class T>
string BotkifyLinkedList<T>::toString() const {
    stringstream ss;
    Node* cur = head;

    while (cur != nullptr) {
        ss << cur->data;
        if (cur->next) ss << ",";
        cur = cur->next;
    }
    return ss.str();
}

#endif // BOTKIFY_LINKED_LIST_H
