//#ifndef CACHE_PROJECT_KLRUKCACHE_TEST_H
//#define CACHE_PROJECT_KLRUKCACHE_TEST_H
//
//#include"KLruCache.h"
//#include<unordered_map>
//#include <memory>
//
//template<typename Key, typename Value>
//class KLruKCache : public KLruCache<Key, Value>
//{
//public:
//    KLruKCache(int capacity, int historyCapacity, int k)
//            : KLruCache<Key, Value>(capacity)
//            , historyList_(std::make_unique<KLruCache<Key, size_t>>(historyCapacity))
//            , k_(k)
//            {}
//    Value get(Key key){
//        int historyCount = historyList_->get(key);
//        historyList_->put(key, ++historyCount);
//        return KLruCache<Key, Value>::get(key);
//    }
//
//    void put(Key key, Value value){
//        if(KLruCache<Key, Value>::get(key) != "")
//            KLruCache<Key, Value>::put(key, value);
//
//        int historyCount = historyList_->get(key);
//        historyList_->put(key, ++historyCount);
//
//        if(historyCount >= k_){
//            historyList_->remove(key);
//            KLruCache<Key, Value>::put(key, value);
//        }
//    }
//private:
//    int k_;
//    std::unique_ptr<KLruCache<Key, size_t>> historyList_;
//};
//
//
//
//#endif //CACHE_PROJECT_KLRUKCACHE_TEST_H
