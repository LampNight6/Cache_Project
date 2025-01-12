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

#include <iostream>
#include "KLruKCache.h"

int main()
{
    // 创建一个KLruKCache实例，容量为5，历史容量为10，k值为3
    KLruKCache<std::string, std::string> cache(5, 10, 3);

    // 添加数据
    cache.put("A", "Apple");
    cache.put("B", "Banana");
    cache.put("C", "Cherry");

    // 模拟访问 "A" 3次，达到阈值将其加入缓存
    std::cout << "Accessing 'A' (should print empty as it's not yet in cache): " << cache.get("A") << std::endl;
    cache.put("A", "Apple"); // 再次更新
    std::cout << "Accessing 'A' again: " << cache.get("A") << std::endl;
    cache.put("A", "Apple"); // 第三次更新，应该满足条件加入缓存

    cache.put("B", "Banana");
    cache.put("B", "Banana");

    // 获取缓存数据并验证
    std::cout << "Final access to 'A' (should now return value 'Apple'): " << cache.get("A") << std::endl;
    std::cout << "Accessing 'B': " << cache.get("B") << std::endl;
    std::cout << "Accessing 'C': " << cache.get("C") << std::endl;




    // 添加更多数据超出容量，验证LRU淘汰
    cache.put("D", "Date");
    cache.put("E", "Elderberry");
    cache.put("F", "Fig"); // "A" 或 "B" 应该被淘汰（取决于访问情况）
    cache.put("F", "Fig");
    cache.put("F", "Fig");
    std::cout << "Accessing 'A' after potential eviction: " << cache.get("A") << std::endl;
    std::cout << "Accessing 'B': " << cache.get("B") << std::endl;
    std::cout << "Accessing 'C': " << cache.get("C") << std::endl;
    std::cout << "Accessing 'D': " << cache.get("D") << std::endl;
    std::cout << "Accessing 'E': " << cache.get("E") << std::endl;
    std::cout << "Accessing 'F': " << cache.get("F") << std::endl;

    return 0;
}

