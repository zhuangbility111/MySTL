//
// Created by 54969 on 2020/3/9.
//

#ifndef STL_MY_ALLOCATOR_JJALLOCATOR_H
#define STL_MY_ALLOCATOR_JJALLOCATOR_H

#include <new>      // placement new
#include <cstddef>  // ptrdiff_t, size_t
#include <cstdlib>  // exit()
#include <climits>  // UINT_MAX
#include <iostream>

using namespace std;

namespace JJ {

    /*
     * _allocate函数，主要是负责分配堆内存空间
     * 需要使用模板，因为是为不同的对象分配空间
     * size指定需要分配的内存空间大小，以对象的字节大小为单位
     */
    template <class T>
    inline T* _allocate(ptrdiff_t size, T*) {
        set_new_handler(0);     // 这句是负责设置分配内存出错时调用的函数，设为NULL表示不做处理，抛出bad_alloc异常
        T* tmp = (T*) ::operator new((size_t)(sizeof(T) * size));     // 调用的是::operator new()。这个函数只申请内存空间，不进行对象的初始化。它是new()的组成部分，具体可以看博客
        if (tmp == NULL) {
            cerr << "out of memory" << endl;
            exit(1);
        }
        return tmp;
    }

    /*
     * _deallocate函数，主要是负责回收堆内存空间
     * buffer为需要释放的空间的指针
     */
    template <class T>
    inline void _deallocate(T* buffer) {
        ::operator delete(buffer);      // ::表示使用全局的函数。operator delete()同样只是负责回收内存，不负责做对象的析构。它也是delete()的组成部分
    }

    /*
     * _construct函数，主要是负责在已分配的内存空间上构造对象
     * p为指向已分配内存空间的指针，value为用来拷贝的对象参数
     */
    template <class T1, class T2>
    inline void _construct(T1* p, const T2& value) {
        new(p) T1(value);       // 这句表示使用placement new。意思是在给定的内存空间上（也就是指针p指向的、已经申请的内存空间），使用T1的拷贝构造函数构造一个T1对象
    }

    /*
     * _destroy函数，主要是负责在已分配的内存空间上析构对象
     * p为指向已分配并已经构造了对象的内存空间的指针
     */
    template <class T>
    inline void _destroy(T* p) {
        p->~T();        // 直接通过调用T对象的析构函数来析构对象
    }


    /*
     * allocator类，主要是借助上面的函数来实现一个内存管理类
     */
    template <class T>
    class allocator {
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;

        template <class U>
        struct rebind {
            typedef allocator<U> other;
        };

        // 主要负责分配n个T对象大小的内存空间，借助_allocate()实现
        // hint用于局部性
        pointer allocate(size_type n, const void* hind = 0) {
            return _allocate((difference_type)n, (pointer)0);       // 不需要显式指定T类型，通过第二个指针参数来传递模板参数类型T
        }

        // 主要负责回收内存空间，借助_deallocate()实现
        // p为需要回收的内存空间的指针
        void deallocate(pointer p, size_type n) {
            _deallocate(p);     // 不需要显式指定T类型，通过参数p来传递模板参数类型T
        }

        // 主要负责对指定的内存空间进行对象的构造，借助_construct()实现
        // 使用的是拷贝构造函数，也就是需要传入一个对象的引用
        void construct(pointer p, const_reference value) {
            _construct(p, value);       // 同样是通过参数来传递模板参数T
        }

        // 主要负责对指定的内存空间进行对象的析构，借助_destroy()实现
        // p为指向需要析构的对象的内存空间的指针
        void destroy(pointer p) {
            _destroy(p);
        }

        // 返回传入对象的地址
        pointer address(reference x) {
            return (pointer)&x;
        }

        // 返回可申请的最大内存空间，以T对象的大小为单位
        size_type max_size() const {
            return size_type (UINT_MAX / sizeof(T));
        }

    };

}




#endif //STL_MY_ALLOCATOR_JJALLOCATOR_H
