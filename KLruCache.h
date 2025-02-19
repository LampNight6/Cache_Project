#ifndef KLRUCACHE_H
#define KLRUCACHE_H

#include <cstring>
#include <list>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "KICachePolicy.h"

namespace Cache_Project
{
    // 前向声明
    template<typename Key, typename Value> class KLruCache;

    template<typename Key, typename Value>
    class LruNode
    {
    private:
        Key key_;
        Value value_;
        size_t accessCount_;  // 访问次数
        std::shared_ptr<LruNode<Key, Value>> prev_;
        std::shared_ptr<LruNode<Key, Value>> next_;

    public:
        LruNode(Key key, Value value)
                : key_(key)
                , value_(value)
                , accessCount_(1)
                , prev_(nullptr)
                , next_(nullptr)
        {}

        // 提供必要的访问器
        Key getKey() const { return key_; }
        Value getValue() const { return value_; }
        void setValue(const Value& value) { value_ = value; }
        size_t getAccessCount() const { return accessCount_; }
        void incrementAccessCount() { ++accessCount_; }

        friend class KLruCache<Key, Value>;
    };


    template<typename Key, typename Value>
    class KLruCache : public KICachePolicy<Key, Value>
    {
    public:
        using LruNodeType = LruNode<Key, Value>;
        using NodePtr = std::shared_ptr<LruNodeType>;
        using NodeMap = std::unordered_map<Key, NodePtr>;

        KLruCache(int capacity)
                : capacity_(capacity)
        {
            initializeList();
        }

        ~KLruCache() = default;

        // 添加缓存
        void put(Key key, Value value)
        {
            if (capacity_ <= 0)
                return;

            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end())
            {
                // 如果在当前容器中,则更新value,并调用get方法，代表该数据刚被访问
                updateExistingNode(it->second, value);
                return ;
            }

            addNewNode(key, value);
        }

        bool get(Key key, Value& value)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end())
            {
                moveToMostRecent(it->second);
                value = it->second->getValue();
                return true;
            }
            return false;
        }

        Value get(Key key)
        {
            Value value{};
            get(key, value);
            return value;
        }

        // 删除指定元素
        void remove(Key key)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end())
            {
                removeNode(it->second);
                nodeMap_.erase(it);
            }
        }

    private:
        void initializeList()
        {
            // 创建首尾虚拟节点
            dummyHead_ = std::make_shared<LruNodeType>(Key(), Value());
            dummyTail_ = std::make_shared<LruNodeType>(Key(), Value());
            dummyHead_->next_ = dummyTail_;
            dummyTail_->prev_ = dummyHead_;
        }

        void updateExistingNode(NodePtr node, const Value& value)
        {
            node->setValue(value);
            moveToMostRecent(node);
        }

        void addNewNode(const Key& key, const Value& value)
        {
            if (nodeMap_.size() >= capacity_)
            {
                evictLeastRecent();
            }

            NodePtr newNode = std::make_shared<LruNodeType>(key, value);
            insertNode(newNode);
            nodeMap_[key] = newNode;
        }

        // 将该节点移动到最新的位置
        void moveToMostRecent(NodePtr node)
        {
            removeNode(node);
            insertNode(node);
        }

        void removeNode(NodePtr node)
        {
            node->prev_->next_ = node->next_;
            node->next_->prev_ = node->prev_;
        }

        // 从尾部插入结点
        void insertNode(NodePtr node)
        {
            node->next_ = dummyTail_;
            node->prev_ = dummyTail_->prev_;
            dummyTail_->prev_->next_ = node;
            dummyTail_->prev_ = node;
        }

        // 驱逐最近最少访问
        void evictLeastRecent()
        {
            NodePtr leastRecent = dummyHead_->next_;
            removeNode(leastRecent);
            nodeMap_.erase(leastRecent->getKey());
        }

    private:
        int          capacity_; // 缓存容量
        NodeMap      nodeMap_; // key -> Node
        std::mutex   mutex_;
        NodePtr       dummyHead_; // 虚拟头结点
        NodePtr       dummyTail_;
    };

}


#endif // KLRUCACHE_H
