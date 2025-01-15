#ifndef CACHE_PROJECT_KHASHLFUCACHE_H
#define CACHE_PROJECT_KHASHLFUCACHE_H


#include "KLfuCache.h"
// 并没有牺牲空间换时间，他是把原有缓存大小进行了分片。
template<typename Key, typename Value>
class KHashLfuCache
{
public:
    KHashLfuCache(size_t capacity, int sliceNum, int maxAverageNum = 10)
            : sliceNum_(sliceNum > 0 ? sliceNum : std::thread::hardware_concurrency())
            , capacity_(capacity)
    {
        size_t sliceSize = std::ceil(capacity_ / static_cast<double>(sliceNum_)); // 每个lfu分片的容量
        for (int i = 0; i < sliceNum_; ++i)
        {
            lfuSliceCaches_.emplace_back(new KLfuCache<Key, Value>(sliceSize, maxAverageNum));
        }
    }

    void put(Key key, Value value)
    {
        // 根据key找出对应的lfu分片
        size_t sliceIndex = Hash(key) % sliceNum_;
        return lfuSliceCaches_[sliceIndex]->put(key, value);
    }

    bool get(Key key, Value& value)
    {
        // 根据key找出对应的lfu分片
        size_t sliceIndex = Hash(key) % sliceNum_;
        return lfuSliceCaches_[sliceIndex]->get(key, value);
    }

    Value get(Key key)
    {
        Value value;
        get(key, value);
        return value;
    }

    // 清除缓存
    void purge()
    {
        for (auto& lfuSliceCache : lfuSliceCaches_)
        {
            lfuSliceCache->purge();
        }
    }

private:
    // 将key计算成对应哈希值
    size_t Hash(Key key)
    {
        std::hash<Key> hashFunc;
        return hashFunc(key);
    }

private:
    size_t capacity_; // 缓存总容量
    int sliceNum_; // 缓存分片数量
    std::vector<std::unique_ptr<KLfuCache<Key, Value>>> lfuSliceCaches_; // 缓存lfu分片容器
};

#endif //CACHE_PROJECT_KHASHLFUCACHE_H
