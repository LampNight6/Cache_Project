//#include <iostream>
//#include "KLruCache_Test.h"
//
//int main() {
//    KLruCache<int, std::string> cache(3);
//
//    // 添加元素到缓存
//    cache.put(1, "one");
//    cache.put(2, "two");
//    cache.put(3, "three");
//
//    // 访问缓存中的元素
//    std::string value;
//    if (cache.get(2, value)) {
//        std::cout << "Key 2: " << value << std::endl;
//    }
//
//    // 添加新元素并驱逐最久未使用的元素
//    cache.put(4, "four");
//
//    // 检查驱逐的元素是否存在
//    if (!cache.get(1, value)) {
//        std::cout << "Key 1 has been evicted." << std::endl;
//    }
//
//    // 当前缓存中的元素
//    if (cache.get(2, value)) {
//        std::cout << "Key 2: " << value << std::endl;
//    }
//    if (cache.get(3, value)) {
//        std::cout << "Key 3: " << value << std::endl;
//    }
//    if (cache.get(4, value)) {
//        std::cout << "Key 4: " << value << std::endl;
//    }
//
//
//    return 0;
//}

//#include <iostream>
//#include "KLruKCache.h"
//
//int main()
//{
//    // 创建一个KLruKCache实例，容量为5，历史容量为10，k值为3
//    KLruKCache<std::string, std::string> cache(5, 10, 3);
//
//    // 添加数据
//    cache.put("A", "Apple");
//    cache.put("B", "Banana");
//    cache.put("C", "Cherry");
//
//    // 模拟访问 "A" 3次，达到阈值将其加入缓存
//    std::cout << "Accessing 'A' (should print empty as it's not yet in cache): " << cache.get("A") << std::endl;
//    cache.put("A", "Apple"); // 再次更新
//    std::cout << "Accessing 'A' again: " << cache.get("A") << std::endl;
//    cache.put("A", "Apple"); // 第三次更新，应该满足条件加入缓存
//
//    cache.put("B", "Banana");
//    cache.put("B", "Banana");
//
//    // 获取缓存数据并验证
//    std::cout << "Final access to 'A' (should now return value 'Apple'): " << cache.get("A") << std::endl;
//    std::cout << "Accessing 'B': " << cache.get("B") << std::endl;
//    std::cout << "Accessing 'C': " << cache.get("C") << std::endl;
//
//
//
//
//    // 添加更多数据超出容量，验证LRU淘汰
//    cache.put("D", "Date");
//    cache.put("E", "Elderberry");
//    cache.put("F", "Fig"); // "A" 或 "B" 应该被淘汰（取决于访问情况）
//    cache.put("F", "Fig");
//    cache.put("F", "Fig");
//    std::cout << "Accessing 'A' after potential eviction: " << cache.get("A") << std::endl;
//    std::cout << "Accessing 'B': " << cache.get("B") << std::endl;
//    std::cout << "Accessing 'C': " << cache.get("C") << std::endl;
//    std::cout << "Accessing 'D': " << cache.get("D") << std::endl;
//    std::cout << "Accessing 'E': " << cache.get("E") << std::endl;
//    std::cout << "Accessing 'F': " << cache.get("F") << std::endl;
//
//    return 0;
//}

//#include "iostream"
//#include "cmath"
//#include "thread"
//#include "vector"
//#include "KHashLruCaches.h"
//int main() {
//    // 创建一个 KHashLruCaches 实例
//    size_t totalCapacity = 10;
//    int sliceNum = 3; // 分为 3 个分片
//    KHashLruCaches<int, std::string> cache(totalCapacity, sliceNum);
//
//    // 测试 put 和 get 功能
//    std::cout << "=== Testing KHashLruCaches ===" << std::endl;
//
//    // 插入测试数据
//    cache.put(1, "One");
//    cache.put(2, "Two");
//    cache.put(3, "Three");
//    cache.put(11, "Eleven"); // 会被映射到与 1 相同的分片
//
//    // 检查插入数据是否正确
//    std::string value;
//    if (cache.get(1, value)) {
//        std::cout << "Key 1 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 1 not found!" << std::endl;
//    }
//
//    if (cache.get(2, value)) {
//        std::cout << "Key 2 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 2 not found!" << std::endl;
//    }
//
//    if (cache.get(3, value)) {
//        std::cout << "Key 3 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 3 not found!" << std::endl;
//    }
//
//    if (cache.get(11, value)) {
//        std::cout << "Key 11 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 11 not found!" << std::endl;
//    }
//
//    // 测试 LRU 行为：插入更多数据，超出容量
//    cache.put(4, "Four");
//    cache.put(5, "Five");
//    cache.put(6, "Six");
//    cache.put(7, "Seven");
//    cache.put(8, "Eight");
//    cache.put(9, "Nine");
//    cache.put(10, "Ten");
//
//    // 尝试访问可能被移除的键
//    if (cache.get(1, value)) {
//        std::cout << "Key 1 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 1 evicted (expected due to LRU)." << std::endl;
//    }
//
//    if (cache.get(4, value)) {
//        std::cout << "Key 4 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 4 not found!" << std::endl;
//    }
//
//    // 测试未找到的键
//    if (cache.get(100, value)) {
//        std::cout << "Key 100 found with value: " << value << std::endl;
//    } else {
//        std::cout << "Key 100 not found (expected)." << std::endl;
//    }
//
//    std::cout << "=== Test Completed ===" << std::endl;
//
//    return 0;
//}

#include <iostream>
#include <unordered_map>
#include "LFUCache.h"

using namespace std;

int main() {
    // 创建一个容量为 2 的 LFU 缓存
    LFUCahe cache(2);

    // 测试 put 和 get 方法
    cache.put(1, 1); // 缓存存储键值对 (1, 1)
    cache.put(2, 2); // 缓存存储键值对 (2, 2)

    cout << "Get key 1: " << cache.get(1) << endl; // 返回 1

    cache.put(3, 3); // 淘汰键 2（访问频率最低），存储键值对 (3, 3)

    cout << "Get key 2: " << cache.get(2) << endl; // 返回 -1（键 2 被淘汰）
    cout << "Get key 3: " << cache.get(3) << endl; // 返回 3

    cache.put(4, 4); // 淘汰键 1（访问频率最低），存储键值对 (4, 4)

    cout << "Get key 1: " << cache.get(1) << endl; // 返回 -1（键 1 被淘汰）
    cout << "Get key 3: " << cache.get(3) << endl; // 返回 3
    cout << "Get key 4: " << cache.get(4) << endl; // 返回 4

    return 0;
}

