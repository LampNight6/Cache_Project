#ifndef CACHE_PROJECT_KLFUCACHE_TEST_H
#define CACHE_PROJECT_KLFUCACHE_TEST_H

#include "cmath"
#include "memory"
#include "mutex"
#include "thread"
#include "unordered_map"
#include "vector"
#include "KICachePolicy.h"

template<typename Key, typename Value> class  KLfuCache;

template<typename Key, typename Value>
class FreqList
{
private:
    struct Node
    {
        int freq;
        Key key;
        Value value;
        std::shared_ptr<Node> pre;
        std::shared_ptr<Node> next;

        Node()
        : freq(1), pre(NULL), next(NULL){}
        Node(Key key, Value value)
        : freq(1), key(key), value(value), pre(NULL), next(NULL){}
    };

    using NodePtr = std::shared_ptr<Node>;
    int freq_;
    NodePtr head_;
    NodePtr tail_;

public:
    explicit FreqList(int n)    //保护该构造函数不被隐式转换
        :freq_(n)
    {
        head_ = std::make_shared<Node>();
        tail_ = std::make_shared<Node>();
        head_->next = tail_;
        tail_->pre = head_;
    }

    bool  isEmpty() const
    {
        return head_->next == tail_;
    }

    void addNode(NodePtr node)
    {
        if(!node || !head_ || !tail_)
            return;

        node->pre = tail_->pre;
        node->next = tail_;
        tail_->pre->next = node;
        tail_->pre = node;
    }

    void removeNode(NodePtr node)
    {
        if(!node || !head_ || !tail_)
            return;
        if(!node->pre || !node->next)
            return;

        node->pre->next = node->next;
        node->next->pre = node->pre;
        node->pre = nullptr;
        node->next = nullptr;
    }

    NodePtr getFirstNode() const{
        return head_->next;
    }

    friend class KLfuCache<Key, Value>;
};

template<typename Key, typename Value>
class KLfuCache:public Cache_Project::KICachePolicy<Key, Value>
{
public:
    using Node = typename FreqList<Key, Value>::Node;
    using NodePtr = std::shared_ptr<Node>;
    using NodeMap = std::unordered_map<Key, NodePtr>;

    KLfuCache(int capacity, int maxAverageNum = 10)
        :capacity_(capacity), minFreq_(INT8_MAX), maxAverageNum_(maxAverageNum), curAverageNum_(0), curTotalNum_(0)
    {}

    ~KLfuCache() override = default;

    void put(Key key, Value value) override
    {
        if(capacity_ == 0)
            return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it =nodeMap_.find(key);
        if(it != nodeMap_.end())
        {
            it->second->value = value;
            getInternal(it->second, value);
            return;
        }

        putInternal(key, value);
    }

    bool get(Key key, Value& value) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = nodeMap_.find(key);
        if(it != nodeMap_.end())
        {
            getInternal(it->second, value);
            return true;
        }
        return false;
    }

    Value get(Key key) override{
        Value value;
        get(key, value);
        return value;
    }

    void purge()
    {
        nodeMap_.clear();
        freqToFreqList_.clear();
    }


private:
    void putInternal(Key key, Value value);
    void getInternal(NodePtr node, Value& value);
    void kickOut();
    void removeFromFreqList(NodePtr node);
    void addToFreqList(NodePtr node);
    void addFreqNum();
    void decreaseFreqNum(int num);
    void handleOverMaxAverageNum();
    void updateMinFreq();

private:
    int capacity_;
    int minFreq_;
    int maxAverageNum_;
    int curAverageNum_;
    int curTotalNum_;
    std::mutex mutex_;
    NodeMap nodeMap_;
    std::unordered_map<int, FreqList<Key, Value>*> freqToFreqList_;
};

template<typename Key, typename Value>
void KLfuCache<Key, Value>::getInternal(KLfuCache::NodePtr node, Value &value) {
    value = node->value;
    removeFromFreqList(node);
    if(node->freq == minFreq_ && freqToFreqList_[node->freq]-> isEmpty())
        minFreq_++;
    node->freq++;
    addToFreqList(node);
    addFreqNum();
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::putInternal(Key key, Value value) {

    if(nodeMap_.size() == capacity_)
    {
        kickOut();
    }
    NodePtr node = std::make_shared<Node>(key,value);
    nodeMap_[key] = node;
    addToFreqList(node);
    addFreqNum();
    minFreq_ = std::min(minFreq_, 1);
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::kickOut() {
    NodePtr node = freqToFreqList_[minFreq_]->getFirstNode();
    removeFromFreqList(node);
    nodeMap_.erase(node->key);
    decreaseFreqNum(node->freq);
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::removeFromFreqList(NodePtr node) {
    if(!node)
        return;
    auto freq = node->freq;
    freqToFreqList_[freq]->removeNode(node);
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::addToFreqList(KLfuCache::NodePtr node) {
    if(!node)
        return;
    auto freq = node->freq;
    if(freqToFreqList_.find(node->freq) == freqToFreqList_.end()){
        freqToFreqList_[node->freq] = new FreqList<Key, Value>(node->freq);
    }
    freqToFreqList_[freq]->addNode(node);
}
template<typename Key, typename Value>
void KLfuCache<Key, Value>::addFreqNum() {
    curTotalNum_++;
    if(nodeMap_.empty())
        curAverageNum_ = 0;
    else
        curAverageNum_ = curTotalNum_ / nodeMap_.size();
    if(curAverageNum_ > maxAverageNum_)
    {
        handleOverMaxAverageNum();
    }
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::decreaseFreqNum(int num) {
    curTotalNum_ -= num;
    if(nodeMap_.empty())
        curAverageNum_ = 0;
    else
        curAverageNum_ = curTotalNum_ / nodeMap_.size();
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::handleOverMaxAverageNum() {
    if(nodeMap_.empty())
        return;

    for(auto it = nodeMap_.begin(); it != nodeMap_.end(); it++){
        if(!it->second)
            continue;
        NodePtr node = it->second;
        removeFromFreqList(node);
        node->freq -= maxAverageNum_ / 2;
        if(node->freq < 1)
            node->freq =1;
        addToFreqList(node);
    }
    updateMinFreq();
}

template<typename Key, typename Value>
void KLfuCache<Key, Value>::updateMinFreq() {
    minFreq_ = INT8_MAX;
    for(const auto & pair : freqToFreqList_){
        if(pair.second && !pair.second->isEmpty()){
            minFreq_ = std::min(minFreq_, pair.first);
        }
    }
    if(minFreq_ == INT8_MAX)
        minFreq_ = 1;
}

#endif //CACHE_PROJECT_KLFUCACHE_TEST_H
