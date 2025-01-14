#ifndef CACHE_PROJECT_LFUCACHE_H
#define CACHE_PROJECT_LFUCACHE_H


class LFUCahe{
private:
    struct Node{
        int key;
        int value;
        int freq;
        Node* pre, * next;
        Node(int key,int value, int freq){
            this->key = key;
            this->value = value;
            this->freq = freq;
            pre = NULL;
            next = NULL;
        }
    };
    struct FreqList{
        int freq;
        Node* L,* R;
        FreqList(int freq){
            this->freq=freq;
            L = new Node(-1,-1,1);
            R = new Node(-1,-1,1);
            L->next = R;
            R->pre = L;
        }
    };
    int n;  //缓存空间大小
    int minFreq;
    std::unordered_map<int, Node*> hashNode;
    std::unordered_map<int,FreqList*> hashFreq;

    void deleteFromList(Node* node){
        Node* pre = node->pre;
        Node* next = node->next;
        pre->next = next;
        next->pre = pre;
    }
    void append(Node* node){
        int freq = node->freq;
        if(hashFreq.find(freq) == hashFreq.end())
            hashFreq[freq] = new FreqList(freq);
        FreqList* curList = hashFreq[freq];

        Node* pre = curList->R->pre;
        Node* next = curList->R;
        pre->next = node;
        node->next = next;
        next->pre = node;
        node->pre = pre;
    }
public:
    LFUCahe(int capacity){
        n = capacity;
        minFreq = 0;
    }
    int get(int key){
        if(hashNode.find(key) != hashNode.end()){
            Node* node = hashNode[key];
            deleteFromList(node);
            node->freq++;
            if(hashFreq[minFreq]->L->next == hashFreq[minFreq]->R)
                minFreq++;
            append(node);
            return  node->value;
        } else{
            return -1;
        }
    }
    void put(int key, int value){
        if(n ==0)
            return;
        if(get(key) != -1)
            hashNode[key]->value = value;
        else{
            if(hashNode.size() == n){
                Node* node = hashFreq[minFreq]->L->next;
                deleteFromList(node);
                hashNode.erase(node->key);
            }
            Node* node = new Node(key, value, 1);
            hashNode[key] = node;
            minFreq = 1;
            append(node);
        }
    }
};

#endif //CACHE_PROJECT_LFUCACHE_H
