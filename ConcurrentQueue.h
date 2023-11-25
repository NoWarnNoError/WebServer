#include <atomic>
#include <vector>

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

    void Init();
    bool empty();
    int size();
    Node* push(ValueType val);
    Node* pop();
};