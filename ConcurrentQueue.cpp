#include "ConcurrentQueue.h"

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
typename ConcurrentQueue<ValueType>::Node* ConcurrentQueue<ValueType>::pop() {
    if (head == tail) {
        return nullptr;
    }

    Node* old_head = head.load();
    while (!head.compare_exchange_weak(old_head, old_head->next)) {
        old_head = head.load();
    }
    --_size;

    return old_head;
}