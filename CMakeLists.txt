cmake_minimum_required(VERSION 3.26)
project(Cache_Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(Cache_Project main.cpp
        LRUCache.h
        LRUCache.cpp)
