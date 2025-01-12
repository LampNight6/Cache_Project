//#ifndef CACHE_PROJECT_KLRUCACHE_TEST_H
//#define CACHE_PROJECT_KLRUCACHE_TEST_H
//
//#include<unordered_map>
//#include<memory>
//#include<mutex>
//
//template<typename  Key, typename Value> class  KLruCache;   //前向声明，向LruNode声明存在该类
//
//template<typename Key, typename Value>  //模板类声明
//class LruNode
//{
//private:
//    Key key_;   //模板参数，在实例化时由用户指定
//    Value value_;
//    size_t accessCount_;    //size_t是非负整型，与数组大小、容器大小的返回值相同
//    std::shared_ptr<LruNode<Key, Value>> prev_;     //shared_ptr是一个智能指针，共享节点所有权，可被多处引用，所有引用销毁后自动释放
//    std::shared_ptr<LruNode<Key, Value>> next_;
//
//public:
//    LruNode(Key key, Value value)
//            :key_(key)
//            ,value_(value)
//            ,accessCount_(1)
//            ,prev_(nullptr)
//            ,next_(nullptr)
//    {}      //这是构造函数的函数体，表示构造函数除了初始化变量后没有其他操作
//    Key getKey() const{return key_;}        //访问器，提供访问类中私有成员变量的公共方法
//    Value getValue() const{return value_;}
//    void setValue(const Value& value) {value_ = value;}
//    size_t getAccessCount() const{ return accessCount_;}
//    void incrementAccessCount(){ ++ accessCount_;}
//
//    friend class KLruCache<Key, Value>;     //定义友元类，友元类可以直接访问本类的private和protected成员
//};
//
//template<typename Key, typename Value>
//class KLruCache
//{
//public:
//    using LruNodeType = LruNode<Key, Value>;        //定义类型别名
//    using NodePtr = std::shared_ptr<LruNodeType>;
//    using NodeMap = std::unordered_map<Key, NodePtr>;
//
//    KLruCache(int capacity)     //构造函数，初始化实例对象，定义缓存容量
//            :capacity_(capacity)
//    {
//        initializeList();
//    }
//
//    ~KLruCache() = default;     //生成默认析构函数，自动释放资源
//
//    void put(Key key, Value value)
//    {
//        if(capacity_ <=0)
//            return;
//
//        std::lock_guard<std::mutex> lock(mutex_);       //加锁保护多线程访问，作用域结束时自动解锁
//        auto it = nodeMap_.find(key);
//        if(it != nodeMap_.end())
//        {
//            updateExistingNode(it->second, value);
//            return;
//        }
//
//        addNewNode(key, value);
//    }
//
//    bool get(Key key, Value& value){
//        std::lock_guard<std::mutex> lock(mutex_);
//        auto it = nodeMap_.find(key);
//        if(it != nodeMap_.end()){
//            moveToMostRecent(it->second);
//            value = it -> second->getValue();
//            return true;
//        }
//        return false;
//    }
//
//    Value get(Key key){
//        Value value{};
//        get(key, value);
//        return value;
//    }
//
//    void remove(Key key){
//        std::lock_guard<std::mutex> lock(mutex_);
//        auto it = nodeMap_.find(key);
//        if(it != nodeMap_.end()){
//            removeNode(it->second);
//            nodeMap_.erase(it);
//        }
//    }
//
//private:
//    void initializeList(){
//        dummyHead_ = std::make_shared<LruNodeType>(Key(), Value());
//        dummyTail_ = std::make_shared<LruNodeType>(Key(), Value());
//        dummyHead_->next_ = dummyTail_;
//        dummyTail_->prev_ = dummyHead_;
//    }
//
//    void updateExistingNode(NodePtr node, const Value& value){
//        node->setValue(value);
//        moveToMostRecent(node);
//    }
//
//    void addNewNode(const Key& key, const Value& value){
//        if(nodeMap_.size() >= capacity_){
//            evictLeastRecent();
//        }
//        NodePtr newNode = std::make_shared<LruNodeType>(key,value);
//        insertNode(newNode);
//        nodeMap_[key] = newNode;
//    }
//
//
//    void moveToMostRecent(NodePtr node){
//        removeNode(node);
//        insertNode(node);
//    }
//
//    void removeNode(NodePtr node){
//        node->prev_->next_ = node->next_;
//        node->next_->prev_ = node->prev_;
//    }
//
//    void insertNode(NodePtr node){
//        node->next_ = dummyTail_;
//        node->prev_ = dummyTail_->prev_;
//        dummyTail_->prev_->next_ = node;
//        dummyTail_->prev_ = node;
//    }
//
//    void evictLeastRecent(){
//        NodePtr leastRecent = dummyHead_->next_;
//        removeNode(leastRecent);
//        nodeMap_.erase(leastRecent->getKey());
//    }
//
//private:
//    int          capacity_; // 缓存容量
//    NodeMap      nodeMap_; // key -> Node
//    std::mutex   mutex_;
//    NodePtr       dummyHead_; // 虚拟头结点
//    NodePtr       dummyTail_;
//
//};
//
//
//#endif //CACHE_PROJECT_KLRUCACHE_TEST_H
