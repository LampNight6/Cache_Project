//
// Created by admin on 2025/1/9.
//

#ifndef CACHE_PROJECT_LRUCACHE_H
#define CACHE_PROJECT_LRUCACHE_H
#include <unordered_map>
// LRUCache 类定义
class LRUCache {
private:
    // 双向链表节点定义
    struct Node {
        int key;
        int value;
        Node *pre, *next;
        Node(int k, int v) : key(k), value(v), pre(nullptr), next(nullptr) {}
    };

    int n; // 缓存容量
    std::unordered_map<int, Node *> hash; // 哈希表：key 映射到链表节点
    Node *L, *R; // 双向链表的哨兵节点（头部 L 和尾部 R）

    // 从双向链表中删除节点
    void remove(Node *node);

    // 将新节点插入到链表尾部
    void insert(int key, int value);

public:
    // 构造函数，初始化缓存容量
    LRUCache(int capacity);

    // 获取缓存中键对应的值
    int get(int key);

    // 插入或更新键值对
    void put(int key, int value);
};

#endif //CACHE_PROJECT_LRUCACHE_H







