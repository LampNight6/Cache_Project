#ifndef KLRU_K_CACHE_H
#define KLRU_K_CACHE_H

#include <unordered_map>
#include <memory>

// 基本的 KLruCache 实现
template <typename Key, typename Value>
class KLruCache {
public:
    KLruCache(int capacity)
            : capacity_(capacity) {
        L = new Node(-1, Value());
        R = new Node(-1, Value());
        L->next = R;
        R->prev = L;
    }

    virtual ~KLruCache() {
        delete L;
        delete R;
    }

    Value get(Key key) {
        if (hash_.find(key) != hash_.end()) {
            Node* node = hash_[key];
            remove(node);
            insert(node->key, node->value);
            return node->value;
        }
        return Value();
    }

    void put(Key key, Value value) {
        if (hash_.find(key) != hash_.end()) {
            Node* node = hash_[key];
            remove(node);
            insert(key, value);
        } else {
            if (hash_.size() == capacity_) {
                Node* node = L->next;
                remove(node);
            }
            insert(key, value);
        }
    }
    void removeKey(const Key& key) {
        if (hash_.find(key) != hash_.end()) {
            remove(hash_[key]);
        }
    }

protected:
    struct Node {
        Key key;
        Value value;
        Node* prev;
        Node* next;
        Node(Key k, Value v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity_;
    std::unordered_map<Key, Node*> hash_;
    Node* L; // 头哨兵
    Node* R; // 尾哨兵

    void remove(Node* node) {
        Node* prev = node->prev;
        Node* next = node->next;
        prev->next = next;
        next->prev = prev;
        hash_.erase(node->key);
        delete node;
    }

    void insert(Key key, Value value) {
        Node* node = new Node(key, value);
        Node* prev = R->prev;
        Node* next = R;
        prev->next = node;
        node->prev = prev;
        node->next = next;
        next->prev = node;
        hash_[key] = node;
    }
};

// KLruKCache 实现
template <typename Key, typename Value>
class KLruKCache : public KLruCache<Key, Value> {
public:
    KLruKCache(int capacity, int historyCapacity, int k)
            : KLruCache<Key, Value>(capacity),
              historyList_(std::make_unique<KLruCache<Key, size_t>>(historyCapacity)),
              k_(k) {}

    Value get(Key key) {
        int historyCount = historyList_->get(key);
        historyList_->put(key, ++historyCount);
        return KLruCache<Key, Value>::get(key);
    }

    void put(Key key, Value value) {
        if (KLruCache<Key, Value>::get(key) != Value()) {
            KLruCache<Key, Value>::put(key, value);
        }

        int historyCount = historyList_->get(key);
        historyList_->put(key, ++historyCount);

        if (historyCount >= k_) {
            historyList_->removeKey(key); // 使用 removeKey 替代 remove
            KLruCache<Key, Value>::put(key, value);
        }
    }


private:
    int k_; // 进入缓存的访问次数阈值
    std::unique_ptr<KLruCache<Key, size_t>> historyList_; // 记录访问历史
};

#endif // KLRU_K_CACHE_H
