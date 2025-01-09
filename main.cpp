#include <iostream>
#include "LRUCache.h"

int main() {
    // 初始化缓存容量为 3
    LRUCache cache(3);

    // 插入数据
    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);

    // 获取数据
    std::cout << "Key 1 value: " << cache.get(1) << std::endl; // 输出 10
    std::cout << "Key 2 value: " << cache.get(2) << std::endl; // 输出 20

    // 插入新数据，触发淘汰
    cache.put(4, 40); // 淘汰最久未使用的键值对 (3, 30)

    // 获取数据
    std::cout << "Key 3 value: " << cache.get(3) << std::endl; // 输出 -1
    std::cout << "Key 4 value: " << cache.get(4) << std::endl; // 输出 40

    // 插入新数据，触发淘汰
    cache.put(5, 50); // 淘汰最久未使用的键值对 (1, 10)

    // 获取数据
    std::cout << "Key 1 value: " << cache.get(1) << std::endl; // 输出 -1
    std::cout << "Key 2 value: " << cache.get(2) << std::endl; // 输出 20

    return 0;
}
