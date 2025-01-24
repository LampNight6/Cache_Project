#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <iomanip>
#include <random>
#include <algorithm>

#include "KLfuCache.h"
#include "KLruCache.h"
#include "KArcCache/KArcCache.h"
#include "LFUCahe.h"

class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    double elapsed() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

// 辅助函数：打印结果
void printResults(const std::string& testName, int capacity,
                  const std::vector<int>& get_operations,
                  const std::vector<int>& hits) {
    std::cout << "缓存大小: " << capacity << std::endl;
    std::cout << "LRU - 命中率: " << std::fixed << std::setprecision(2)
              << (100.0 * hits[0] / get_operations[0]) << "%" << std::endl;
    std::cout << "LFU - 命中率: " << std::fixed << std::setprecision(2)
              << (100.0 * hits[1] / get_operations[1]) << "%" << std::endl;
    std::cout << "ARC - 命中率: " << std::fixed << std::setprecision(2)
              << (100.0 * hits[2] / get_operations[2]) << "%" << std::endl;
}

void testHotDataAccess() {
    std::cout << "\n=== 测试场景1：热点数据访问测试 ===" << std::endl;

    const int CAPACITY = 5;
    const int OPERATIONS = 100000;
    const int HOT_KEYS = 3;
    const int COLD_KEYS = 5000;

    Cache_Project::KLruCache<int, std::string> lru(CAPACITY);
    Cache_Project::KLfuCache<int, std::string> lfu(CAPACITY);
    Cache_Project::KArcCache<int, std::string> arc(CAPACITY);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::array<Cache_Project::KICachePolicy<int, std::string>*, 3> caches = {&lru, &lfu, &arc};
    std::vector<int> hits(3, 0);
    std::vector<int> get_operations(3, 0);

    // 先进行一系列put操作
    for (int i = 0; i < caches.size(); ++i) {
        for (int op = 0; op < OPERATIONS; ++op) {
            int key;
            if (op % 100 < 40) {  // 40%热点数据
                key = gen() % HOT_KEYS;
            } else {  // 60%冷数据
                key = HOT_KEYS + (gen() % COLD_KEYS);
            }
            std::string value = "value" + std::to_string(key);
            caches[i]->put(key, value);
        }

        // 然后进行随机get操作
        for (int get_op = 0; get_op < OPERATIONS/2; ++get_op) {
            int key;
            if (get_op % 100 < 40) {  // 40%概率访问热点
                key = gen() % HOT_KEYS;
            } else {  // 60%概率访问冷数据
                key = HOT_KEYS + (gen() % COLD_KEYS);
            }

            std::string result;
            get_operations[i]++;
            if (caches[i]->get(key, result)) {
                hits[i]++;
            }
        }
    }

    printResults("热点数据访问测试", CAPACITY, get_operations, hits);
}

void testLoopPattern() {
    std::cout << "\n=== 测试场景2：循环扫描测试 ===" << std::endl;

    const int CAPACITY = 3;
    const int LOOP_SIZE = 200;
    const int OPERATIONS = 50000;

    Cache_Project::KLruCache<int, std::string> lru(CAPACITY);
    Cache_Project::KLfuCache<int, std::string> lfu(CAPACITY);
    Cache_Project::KArcCache<int, std::string> arc(CAPACITY);

    std::array<Cache_Project::KICachePolicy<int, std::string>*, 3> caches = {&lru, &lfu, &arc};
    std::vector<int> hits(3, 0);
    std::vector<int> get_operations(3, 0);

    std::random_device rd;
    std::mt19937 gen(rd());

    // 先填充数据
    for (int i = 0; i < caches.size(); ++i) {
        for (int key = 0; key < LOOP_SIZE * 2; ++key) {
            std::string value = "loop" + std::to_string(key);
            caches[i]->put(key, value);
        }

        // 然后进行访问测试
        int current_pos = 0;
        for (int op = 0; op < OPERATIONS; ++op) {
            int key;
            if (op % 100 < 70) {  // 70%顺序扫描
                key = current_pos;
                current_pos = (current_pos + 1) % LOOP_SIZE;
            } else if (op % 100 < 85) {  // 15%随机跳跃
                key = gen() % LOOP_SIZE;
            } else {  // 15%访问范围外数据
                key = LOOP_SIZE + (gen() % LOOP_SIZE);
            }

            std::string result;
            get_operations[i]++;
            if (caches[i]->get(key, result)) {
                hits[i]++;
            }
        }
    }

    printResults("循环扫描测试", CAPACITY, get_operations, hits);
}

void testWorkloadShift() {
    std::cout << "\n=== 测试场景3：工作负载剧烈变化测试 ===" << std::endl;

    const int CAPACITY = 4;
    const int OPERATIONS = 80000;
    const int PHASE_LENGTH = OPERATIONS / 5;

    Cache_Project::KLruCache<int, std::string> lru(CAPACITY);
    Cache_Project::KLfuCache<int, std::string> lfu(CAPACITY);
    Cache_Project::KArcCache<int, std::string> arc(CAPACITY);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::array<Cache_Project::KICachePolicy<int, std::string>*, 3> caches = {&lru, &lfu, &arc};
    std::vector<int> hits(3, 0);
    std::vector<int> get_operations(3, 0);

    // 先填充一些初始数据
    for (int i = 0; i < caches.size(); ++i) {
        for (int key = 0; key < 1000; ++key) {
            std::string value = "init" + std::to_string(key);
            caches[i]->put(key, value);
        }

        // 然后进行多阶段测试
        for (int op = 0; op < OPERATIONS; ++op) {
            int key;
            // 根据不同阶段选择不同的访问模式
            if (op < PHASE_LENGTH) {  // 热点访问
                key = gen() % 5;
            } else if (op < PHASE_LENGTH * 2) {  // 大范围随机
                key = gen() % 1000;
            } else if (op < PHASE_LENGTH * 3) {  // 顺序扫描
                key = (op - PHASE_LENGTH * 2) % 100;
            } else if (op < PHASE_LENGTH * 4) {  // 局部性随机
                int locality = (op / 1000) % 10;
                key = locality * 20 + (gen() % 20);
            } else {  // 混合访问
                int r = gen() % 100;
                if (r < 30) {
                    key = gen() % 5;
                } else if (r < 60) {
                    key = 5 + (gen() % 95);
                } else {
                    key = 100 + (gen() % 900);
                }
            }

            std::string result;
            get_operations[i]++;
            if (caches[i]->get(key, result)) {
                hits[i]++;
            }

            // 随机进行put操作，更新缓存内容
            if (gen() % 100 < 30) {  // 30%概率进行put
                std::string value = "new" + std::to_string(key);
                caches[i]->put(key, value);
            }
        }
    }

    printResults("工作负载剧烈变化测试", CAPACITY, get_operations, hits);
}
void testLFUCache() {
    // 创建一个容量为 20 的三级缓存
    LFUCahe cache(20);

    // 向缓存中插入键值对
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.put(4, 4);
    cache.put(5, 5);

    // 访问一些缓存项
    std::cout << "Get key 1: " << cache.get(1) << std::endl; // Should be hit in L1
    std::cout << "Get key 2: " << cache.get(2) << std::endl; // Should be hit in L1
    std::cout << "Get key 3: " << cache.get(3) << std::endl; // Should be hit in L1
    std::cout << "Get key 6 (not in cache): " << cache.get(6) << std::endl; // Should be missed

    // 插入更多元素，可能会导致缓存淘汰
    for (int i = 6; i <= 20; ++i) {
        cache.put(i, i);
    }

    // 访问一些缓存项，看看哪些缓存项仍然存在
    std::cout << "Get key 1: " << cache.get(1) << std::endl; // Should be found in L1
    std::cout << "Get key 5: " << cache.get(5) << std::endl; // Should be found in L1 or L2
    std::cout << "Get key 10: " << cache.get(10) << std::endl; // Should be found in L2 or L3
    std::cout << "Get key 15: " << cache.get(15) << std::endl; // Should be found in L2 or L3
    std::cout << "Get key 20: " << cache.get(20) << std::endl; // Should be found in L2 or L3

    // 插入更多元素，可能会导致缓存淘汰
    cache.put(21, 21);
    cache.put(22, 22);
    cache.put(23, 23);
    cache.put(24, 24);
    cache.put(25, 25);

    // 访问一些缓存项，看看哪些缓存项仍然存在
    for(int i = 1; i < 26; i++){
        std::cout << "Get key  " <<i<<" : "<< cache.get(i) << std::endl; // Might be evicted
    }

}



int main() {
    testHotDataAccess();
    testLoopPattern();
    testWorkloadShift();
    std::cout<<std::endl;std::cout<<std::endl;std::cout<<std::endl;
    testLFUCache();
    return 0;
}