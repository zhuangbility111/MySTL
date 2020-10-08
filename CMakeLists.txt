cmake_minimum_required(VERSION 3.13)
project(STL_MY_ALLOCATOR)

set(CMAKE_CXX_STANDARD 14)

add_executable(STL_MY_ALLOCATOR main.cpp my_deque.h my_stack.h my_queue.h my_heap_and_priority_queue.cpp my_heap_and_priority_queue.h my_allocator.h)