#include "ConcurrentQueue.h"

template <typename ValueType>
ConcurrentQueue<ValueType>::~ConcurrentQueue() {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

template <typename ValueType>
void ConcurrentQueue<ValueType>::Init() {
    _size = 0;
    Node* dummy = new Node();
    head = dummy;
    tail = dummy;
}

template <typename ValueType>
bool ConcurrentQueue<ValueType>::empty() {
    return head == tail;
}

template <typename ValueType>
int ConcurrentQueue<ValueType>::size() {
    return _size.load();
}

template <typename ValueType>
typename ConcurrentQueue<ValueType>::Node* ConcurrentQueue<ValueType>::push(
    ValueType val) {
    std::atomic<Node*> new_node = new Node(val);
    Node* old_tail = tail.load();

    while (!old_tail->next.compare_exchange_weak(nullptr, new_node)) {
        Node* old_tail = tail.load();
    }
    tail.store(new_node);
    ++_size;

    return nullptr;
}

template <typename ValueType>
ValueType ConcurrentQueue<ValueType>::pop() {
    for (;;) {
        Node* head_node = head.load();
        Node* tail_node = tail.load();
        Node* next_node = head_node->next.load();
        if (head_node == head.load()) {
            if (head_node == tail_node) {
                if (next_node == nullptr) {
                    return nullptr;
                }
                tail.compare_exchange_strong(tail_node, next_node);
            } else {
                if (next_node == nullptr) {
                    continue;
                }
                ValueType r = head_node->value;
                if (head.compare_exchange_weak(head_node, next_node)) {
                    delete head_node;
                    return r;
                }
            }
        }
    }
}