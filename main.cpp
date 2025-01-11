#include <iostream>
#include "KLruCache_Test.h"

int main() {
    KLruCache<int, std::string> cache(3);

    // 添加元素到缓存
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    // 访问缓存中的元素
    std::string value;
    if (cache.get(2, value)) {
        std::cout << "Key 2: " << value << std::endl;
    }

    // 添加新元素并驱逐最久未使用的元素
    cache.put(4, "four");

    // 检查驱逐的元素是否存在
    if (!cache.get(1, value)) {
        std::cout << "Key 1 has been evicted." << std::endl;
    }

    // 当前缓存中的元素
    if (cache.get(2, value)) {
        std::cout << "Key 2: " << value << std::endl;
    }
    if (cache.get(3, value)) {
        std::cout << "Key 3: " << value << std::endl;
    }
    if (cache.get(4, value)) {
        std::cout << "Key 4: " << value << std::endl;
    }


    return 0;
}