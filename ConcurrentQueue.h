#include <assert.h>
#include <atomic>
#include <iostream>

template <typename ValueType>
class ConcurrentQueue {
   private:
    struct Node {
        ValueType val;
        std::atomic<Node*> next;

        Node() : val(0), next(nullptr) {}
        Node(ValueType _val) : val(_val), next(nullptr) {}
    };

   private:
    std::atomic<int> _size;
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

   public:
    ConcurrentQueue() { Init(); }
    ~ConcurrentQueue();

    void Init();
    const bool empty() const;
    const int size() const;
    const ValueType front() const;
    void push(const ValueType& val);
    void pop();
};

template <typename ValueType>
ConcurrentQueue<ValueType>::~ConcurrentQueue() {
    while (Node* now_node = head.load()) {
        head.store(now_node->next);
        // std::cout << now_node->val << ' ';
        delete now_node;
    }
    // std::cout << std::endl;
}

template <typename ValueType>
void ConcurrentQueue<ValueType>::Init() {
    _size = 0;
    Node* dummy = new Node();
    head = dummy;
    tail = dummy;
}

template <typename ValueType>
const bool ConcurrentQueue<ValueType>::empty() const {
    return head == tail;
}

template <typename ValueType>
const int ConcurrentQueue<ValueType>::size() const {
    return _size.load();
}

template <typename ValueType>
const ValueType ConcurrentQueue<ValueType>::front() const {
    assert(!empty());

    return head.load()->next.load()->val;
}

template <typename ValueType>
void ConcurrentQueue<ValueType>::push(const ValueType& val) {
    std::atomic<Node*> now(new Node(val));

    for (;;) {
        Node* tail_node = tail.load();
        std::atomic<Node*> next(tail_node->next.load());
        Node* next_node = next.load();

        if (tail_node == tail.load()) {
            if (next_node == nullptr) {
                if (tail_node->next.compare_exchange_weak(next_node, now)) {
                    tail.compare_exchange_strong(tail_node, now);
                    ++_size;
                    return;
                }
            } else {
                tail.compare_exchange_strong(tail_node, next);
            }
        }
    }

    return;
}

template <typename ValueType>
void ConcurrentQueue<ValueType>::pop() {
    for (;;) {
        Node* head_node = head.load();
        Node* tail_node = tail.load();
        Node* next_node = head_node->next.load();
        if (head_node == head.load()) {
            if (head_node == tail_node) {
                if (next_node == nullptr) {
                    return;
                }
                tail.compare_exchange_strong(tail_node, next_node);
            } else {
                if (next_node == nullptr) {
                    continue;
                }
                ValueType r = head_node->val;
                if (head.compare_exchange_weak(head_node, next_node)) {
                    delete head_node;
                    --_size;
                    return;
                }
            }
        }
    }
}