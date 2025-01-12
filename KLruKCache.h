#ifndef KLRU_K_CACHE_H
#define KLRU_K_CACHE_H

#include"KLruCache.h"
#include <unordered_map>
#include <memory>

// LRU优化：Lru-k版本。 通过继承的方式进行再优化
template<typename Key, typename Value>
class KLruKCache : public KLruCache<Key, Value>
{
public:
    KLruKCache(int capacity, int historyCapacity, int k)
            : KLruCache<Key, Value>(capacity) // 调用基类构造
            , historyList_(std::make_unique<KLruCache<Key, size_t>>(historyCapacity))
            , k_(k)
            {}

    Value get(Key key)
    {
        // 获取该数据访问次数
        int historyCount = historyList_->get(key);
        // 如果访问到数据，则更新历史访问记录节点值count++
        historyList_->put(key, ++historyCount);


        // 从缓存中获取数据，不一定能获取到，因为可能不在缓存中
        return KLruCache<Key, Value>::get(key);
    }

    void put(Key key, Value value)
    {
        // 先判断是否存在于缓存中，如果存在于则直接覆盖，如果不存在则不直接添加到缓存
        if (KLruCache<Key, Value>::get(key) != "")
            KLruCache<Key, Value>::put(key, value);


        // 如果数据历史访问次数达到上限，则添加入缓存
        int historyCount = historyList_->get(key);
        historyList_->put(key, ++historyCount);

        if (historyCount >= k_)
        {
            // 移除历史访问记录
            historyList_->remove(key);
            // 添加入缓存中
            KLruCache<Key, Value>::put(key, value);
        }
    }
private:
    int k_; // 进入缓存队列的评判标准
    std::unique_ptr<KLruCache<Key, size_t>> historyList_; // 访问数据历史记录(value为访问次数)
};

#endif // KLRU_K_CACHE_H
