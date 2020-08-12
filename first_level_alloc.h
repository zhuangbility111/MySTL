//
// Created by 54969 on 2020/3/12.
//

#ifndef STL_MY_ALLOCATOR_FIRST_LEVEL_ALLOC_H
#define STL_MY_ALLOCATOR_FIRST_LEVEL_ALLOC_H

#include <new>
#include <stdlib.h>

#define __THROW_BAD_ALLOC throw std::bad_alloc()

/*
 * 实现第一级分配器，针对大于128Bytes内存块的管理
 * 主要是直接使用malloc()和free()来实现
 */

// 没有template类型参数，只有非类型参数inst，在类中没有使用。但是可以通过传入不同的值来生成不同的类
template <int inst>
class __malloc_alloc_template {
private:
    // 第一第二个都是返回void指针的分配内存空间函数，主要是在oom时执行内存分配的操作
    // 第三个是函数指针（指向错误处理函数的指针），所代表的函数将用来处理内存不足的情况
    // oom : out of memory.
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    // 处理内存分配出错（也就是oom）的情况，由用户指定内存分配出错后执行的操作
    // 与c++的set_new_handler()功能相似，只不过这个是我们自己定义的
    // 只有使用::operator new()和::operator delete()才可以使用set_new_handler()来指定操作
    // malloc和free()都不能使用，所以需要自定义出错后（oom）执行的操作
    // 一般都是尝试释放一部分内存空间，使得有足够的空间进行分配
    static void (* __malloc_alloc_oom_handler)();

public:

    // 首先是分配内存空间函数allocate()，n为字节数
    // 借助malloc(n)来申请内存空间
    static void *allocate(size_t n) {
        void *result = malloc(n);       // 直接使用malloc()实现
        // 如果malloc()无法申请，则改用oom_malloc()
        if (result == 0)
            result = oom_malloc(n);
        return result;
    }

    // 释放内存空间函数deallocate()，p为需要释放的内存空间位置
    // 直接使用free(p)
    static void deallocate(void *p, size_t n) {
        free(p);
    }

    // 重分配内存空间函数reallocate()，p为需要重分配的内存空间位置
    // 也是借助reallocate(p, new_size)来实现
    static void *reallocate(void *p, size_t old_size, size_t new_size) {
        void *result = realloc(p, new_size);
        // 如果realloc()无法分配，则改用oom_realloc()
        if (result == 0)
            result = oom_realloc(p, new_size);
        return result;
    }
};


// 这个指向错误处理函数的函数指针暂时设置为空，让用户自己定义
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

// 接着是oom_malloc()，负责在oom时执行分配内存空间的操作
template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
    void (* my_alloc_handler)();
    void* result;

    // 循环，不断尝试释放、分配、再释放、再分配
    while (true) {
        // 首先使用函数指针指向错误处理函数
        my_alloc_handler = __malloc_alloc_oom_handler;
        // 如果错误处理函数没有定义，则直接使用错误处理宏定义（抛出错误异常）
        if (my_alloc_handler == 0)
            __THROW_BAD_ALLOC;
        // 如果错误处理函数不为空，调用错误处理函数，在函数中企图释放内存
        my_alloc_handler();
        // 接着重新尝试分配内存空间
        result = malloc(n);
        // 如果分配成功，则返回；如果失败，则重新释放后再分配
        if (result != 0)
            return result;
    }

}

// 最后是oom_realloc()，负责在oom时执行内存空间重分配的操作
template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void *p, size_t new_size) {
    void* result;
    void (* my_alloc_handler)();

    // 循环，不断尝试释放、分配、再释放、再分配
    while (true) {
        // 首先使用函数指针指向错误处理函数
        my_alloc_handler = __malloc_alloc_oom_handler;
        // 如果错误处理函数没有定义，则直接使用错误处理宏定义（抛出错误异常）
        if (my_alloc_handler == 0)
            __THROW_BAD_ALLOC;
        // 如果错误处理函数不为空，调用错误处理函数，在函数中企图释放内存
        my_alloc_handler();
        // 接着重新尝试分配内存空间
        result = realloc(p, new_size);
        // 如果分配成功，则返回；如果失败，则重新释放后再分配
        if (result != 0)
            return result;

    }
}






#endif //STL_MY_ALLOCATOR_FIRST_LEVEL_ALLOC_H
