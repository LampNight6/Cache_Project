#ifndef CACHE_PROJECT_LFUCACHE_H
#define CACHE_PROJECT_LFUCACHE_H

#include <unordered_map>
#include <iostream>

//三级缓存LFU代码
class LFUCahe {
private:
    struct Node {
        int key;
        int value;
        int freq;
        Node* pre, * next;
        Node(int key, int value, int freq) {
            this->key = key;
            this->value = value;
            this->freq = freq;
            pre = NULL;
            next = NULL;
        }
    };

    struct FreqList { // 使用双向链表便于维护
        int freq;
        Node* L, * R;
        FreqList(int freq) {
            this->freq = freq;
            L = new Node(-1, -1, 1); // 哨兵节点
            R = new Node(-1, -1, 1); // 哨兵节点
            L->next = R;
            R->pre = L;
        }
    };

    int n;  // 总缓存容量
    int minFreqL1, minFreqL2, minFreqL3;  // 三个缓存层的最小频次
    std::unordered_map<int, Node*> hashNodeL1, hashNodeL2, hashNodeL3;
    std::unordered_map<int, FreqList*> hashFreqL1, hashFreqL2, hashFreqL3;

    void deleteFromList(Node* node, int level) {
        Node* pre = node->pre;
        Node* next = node->next;
        pre->next = next;
        next->pre = pre;
    }

    void append(Node* node, int level) {
        FreqList* curList;
        if (level == 1) {
            if (hashFreqL1.find(node->freq) == hashFreqL1.end())
                hashFreqL1[node->freq] = new FreqList(node->freq);
            curList = hashFreqL1[node->freq];
        } else if (level == 2) {
            if (hashFreqL2.find(node->freq) == hashFreqL2.end())
                hashFreqL2[node->freq] = new FreqList(node->freq);
            curList = hashFreqL2[node->freq];
        } else { // level 3
            if (hashFreqL3.find(node->freq) == hashFreqL3.end())
                hashFreqL3[node->freq] = new FreqList(node->freq);
            curList = hashFreqL3[node->freq];
        }

        Node* pre = curList->R->pre;
        Node* next = curList->R;
        pre->next = node;
        node->next = next;
        next->pre = node;
        node->pre = pre;
    }

    void moveToHigherCache(Node* node) {
        // 如果在L1缓存中, 那么尝试移动到L2, 否则移动到L3
        if (hashNodeL1.find(node->key) != hashNodeL1.end()) {
            if (hashNodeL2.size() < n) {
                hashNodeL2[node->key] = node;
                hashNodeL1.erase(node->key);
                deleteFromList(node, 1);
                append(node, 2);
            } else {
                if (hashNodeL3.size() < n) {
                    hashNodeL3[node->key] = node;
                    hashNodeL2.erase(node->key);
                    deleteFromList(node, 2);
                    append(node, 3);
                }
            }
        } else if (hashNodeL2.find(node->key) != hashNodeL2.end()) {
            if (hashNodeL3.size() < n) {
                hashNodeL3[node->key] = node;
                hashNodeL2.erase(node->key);
                deleteFromList(node, 2);
                append(node, 3);
            }
        }
    }

    void evict(int level) {
        // 根据缓存级别逐级淘汰
        if (level == 1) {
            Node* node = hashFreqL1[minFreqL1]->L->next;
            deleteFromList(node, 1);
            hashNodeL1.erase(node->key);
        } else if (level == 2) {
            Node* node = hashFreqL2[minFreqL2]->L->next;
            deleteFromList(node, 2);
            hashNodeL2.erase(node->key);
        } else { // level 3
            Node* node = hashFreqL3[minFreqL3]->L->next;
            deleteFromList(node, 3);
            hashNodeL3.erase(node->key);
        }
    }

public:
    LFUCahe(int capacity) {
        n = capacity;
        minFreqL1 = 0;
        minFreqL2 = 0;
        minFreqL3 = 0;
    }

    int get(int key) {
        if (hashNodeL1.find(key) != hashNodeL1.end()) {
            Node* node = hashNodeL1[key];
            deleteFromList(node, 1);
            node->freq++;
            if (hashFreqL1[minFreqL1]->L->next == hashFreqL1[minFreqL1]->R)
                minFreqL1++;
            append(node, 1);
            return node->value;
        } else if (hashNodeL2.find(key) != hashNodeL2.end()) {
            Node* node = hashNodeL2[key];
            deleteFromList(node, 2);
            node->freq++;
            if (hashFreqL2[minFreqL2]->L->next == hashFreqL2[minFreqL2]->R)
                minFreqL2++;
            append(node, 2);
            return node->value;
        } else if (hashNodeL3.find(key) != hashNodeL3.end()) {
            Node* node = hashNodeL3[key];
            deleteFromList(node, 3);
            node->freq++;
            if (hashFreqL3[minFreqL3]->L->next == hashFreqL3[minFreqL3]->R)
                minFreqL3++;
            append(node, 3);
            return node->value;
        } else {
            return -1;
        }
    }

    void put(int key, int value) {
        if (n == 0)
            return;

        // 先尝试从L1、L2、L3中获取数据
        if (get(key) != -1) {
            hashNodeL1[key]->value = value;
            return;
        }

        // 超过容量时移除最少使用的节点
        if (hashNodeL1.size() + hashNodeL2.size() + hashNodeL3.size() == n) {
            if (hashFreqL1[minFreqL1]->L->next != hashFreqL1[minFreqL1]->R)
                evict(1);
            else if (hashFreqL2[minFreqL2]->L->next != hashFreqL2[minFreqL2]->R)
                evict(2);
            else
                evict(3);
        }

        Node* node = new Node(key, value, 1);
        hashNodeL1[key] = node;
        minFreqL1 = 1;
        append(node, 1);
    }
};

#endif //CACHE_PROJECT_LFUCACHE_H
