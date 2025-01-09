#include "LRUCache.h"

// 构造函数，初始化容量和哨兵节点
LRUCache::LRUCache(int capacity) {
    n = capacity;
    L = new Node(-1, -1);
    R = new Node(-1, -1);
    L->next = R;
    R->pre = L;
}

// 从双向链表中删除节点
void LRUCache::remove(Node *node) {
    Node *pre = node->pre;
    Node *next = node->next;
    pre->next = next;
    next->pre = pre;
    hash.erase(node->key);
}

// 将新节点插入到链表尾部
void LRUCache::insert(int key, int value) {
    Node *pre = R->pre;
    Node *next = R;
    Node *newNode = new Node(key, value);

    pre->next = newNode;
    newNode->next = next;
    next->pre = newNode;
    newNode->pre = pre;

    hash[key] = newNode;
}

// 获取缓存中键对应的值
int LRUCache::get(int key) {
    if (hash.find(key) != hash.end()) {
        Node *node = hash[key];
        remove(node);
        insert(node->key, node->value);
        return node->value;
    }
    return -1;
}

// 插入或更新键值对
void LRUCache::put(int key, int value) {
    if (hash.find(key) != hash.end()) {
        Node *node = hash[key];
        remove(node);
        insert(key, value);
    } else {
        if (hash.size() == n) {
            Node *node = L->next;
            remove(node);
        }
        insert(key, value);
    }
}
