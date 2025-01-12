//#ifndef CACHE_PROJECT_KHASHLRUCACHES_TEST_H
//#define CACHE_PROJECT_KHASHLRUCACHES_TEST_H
//
//#include<iostream>
//#include "KLruCache.h"
//
//template<typename Key, typename  Value>
//class  KHashLruCaches{
//public:
//    KHashLruCaches(size_t capacity, int sliceNum)
//        :capacity_(capacity)
//        ,sliceNum_(sliceNum > 0 ? sliceNum : std::thread::hardware_concurrency())
//        {
//            size_t sliceSize = std::ceil(capacity / static_cast<double>(sliceNum));
//            for(int i = 0; i < sliceNum_; i++){
//                lruSliceCaches_.emplace_back(new KLruCache<Key, Value>(sliceSize));
//            }
//        }
//
//        void put(Key key, Value value){
//            size_t sliceIndex = Hash(key) % sliceNum_;
//            return lruSliceCaches_[sliceIndex]->put(key, value);
//        }
//
//        bool get(Key key, Value& value){
//            size_t sliceIndex = Hash(key) % sliceNum_;
//            return lruSliceCaches_[sliceIndex]->get(key, value);
//        }
//
//        Value get(Key key){
//            Value value;
//            memset(&value, 0, sizeof(value));
//            get(key, value);
//            return value;
//        }
//
//
//private:
//
//    size_t Hash(Key key){
//        std::hash<Key> hashFunc;
//        return hashFunc(key);
//    }
//
//    size_t capacity_;
//    int sliceNum_;
//    std::vector<std::unique_ptr<KLruCache<Key,Value>>> lruSliceCaches_;
//};
//
//
//#endif //CACHE_PROJECT_KHASHLRUCACHES_TEST_H
//
//
//
//
