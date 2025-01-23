cmake_minimum_required(VERSION 3.26)
project(Cache_Project)

set(CMAKE_CXX_STANDARD 17)
add_executable(Cache_Project
        KLruCache.h
        KLruKCache.h
        KHashLruCaches.h
        LFUCache.h
        KLfuCache.h
        KICachePolicy.h
        KHashLfuCache.h
        KArcCache/KArcCacheNode.h
        KArcCache/KArcCache.h
        KArcCache/KArcLruPart.h
        KArcCache/KArcLfuPart.h
        testAllCachePolicy.cpp
        LRUCache.h)
