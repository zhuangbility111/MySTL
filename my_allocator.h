//
// Created by 54969 on 2020/3/12.
//

#ifndef STL_MY_ALLOCATOR_MY_ALLOCATOR_H
#define STL_MY_ALLOCATOR_MY_ALLOCATOR_H

#include <new>          // 为了使用placement new，在已申请的内存空间上对对象进行初始化
#include <type_traits>  // 为了使用__type_traits，它是用来萃取型别的特性，例如是否具备non-trivial default ctor
#include "second_level_alloc.h"

using namespace std;

namespace my_std {
    /*
 * 先实现在已申请的内存空间上进行构造函数和析构函数
 * construct()和destroy()
 */

// 最基础的construct函数实现，直接在已经申请的内存空间上对对象进行初始化
// p为指向已申请的内存空间的指针，T2为需要拷贝的源对象
    template <class T1, class T2>
    inline void construct(T1* p, const T2& value) {
        new(p) T1(value);       // 使用placement new，调用的构造函数是T1::T1(value)
    }

// 最基础的destroy函数，接受一个指向需要析构的对象的内存空间的指针p
    template <class T>
    inline void destroy(T* p) {
        p->~T();        // 直接调用对象的析构函数
    }

// 第二个版本的destroy函数，接受两个迭代器参数，析构两个迭代器范围内的对象
// 首先找元素的数据类型，然后用_type_traits<>寻找最合适的析构方法
    template <class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        _destroy(first, last, value_type(first));       // value_type()返回迭代器的模板参数类型，也就是迭代器所对应的容器的元素类型
    }

// 这个_destroy函数是实现迭代器版本destroy()的辅助函数
// 主要是根据迭代器中元素的类型来判断是否对象是否有trivial destructor
// 如果有trivial destructor，就不需要显式调用它的析构函数
// 如果有non-trivial destructor，就需要进一步显式调用它的析构函数
// 这里很巧妙的使用了模板和指针来传递value_type()返回的元素类型，通过参数来隐式指定T的类型，也方便下面根据T的类型来做进一步的操作
    template <class ForwardIterator, class T>
    inline void _destroy(ForwardIterator first, ForwardIterator last, T*) {
        // __type_traits就是所谓的萃取器，可以使用它的一个类has_trivial_destructor来判断某个类型是否有trivial destructor
        // 需要加typename，表示typedef的东西是一个类型，而不是一个普通成员
//    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
//    _destroy_aux(first, last, trivial_destructor());    // 进一步调用辅助函数，根据T是否有trivial destructor来做进一步的处理
    }

// 如果T拥有non-trivial destructor，则trivial_destructor()会返回一个__false_type的值，所以会调用这个函数
// 这个函数主要是对有non-trivial的T进行进一步的处理，显式调用它的析构函数来析构对象
    template <class ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
        // first到last之间的每一个迭代器，都是用第一个版本的destroy进行析构
        // 也就是将指向对象的指针传入，destroy直接对这个指针使用它的析构函数
        for( ; first < last; first++) {
            // 因为first是一个迭代器类型，而不是一个指针，所以要先使用*first解引用它实际保存的对象，再使用&引用这个对象的地址
            destroy(&*first);
        }
    }

// 如果T拥有trivial destructor，则trivial_destructor()会返回一个__true_type的值，所以会调用这个函数
// 不需要显式调用析构函数进行对象析构，所以不做任何处理，只是为了提供重载的_destroy函数
    template <class ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}


// 第三个destroy版本，主要是应对参数为两个char*的情况
    inline void destroy(char*, char*) {}

// 第四个destroy版本，主要是应对参数为两个wchar_t*的情况
    inline void destroy(wchar_t*, wchar_t*) {}


/*
 * --------------------------------------------------------------------------------------------------
 * 接着就是实现分配内存空间和回收内存空间的函数
 * allocate()和deallocate()
 * 这个分配器分成两个分配器，第一级分配器和第二级分配器
 * 第一级分配器主要负责管理大于128字节的内存块，而第二级分配器主要负责管理小于128字节的小内存块，基于memory pool整理
 */

// 首先使用一个类，内部封装了底层的两级分配器，相当于提供一个外层接口
// 通过模板参数Alloc来决定使用的是哪一个底层的分配器
    template <class T, class Alloc = __default_alloc_template<false, 1>>
    class simple_alloc {
    public:
        // 用于分配内存空间，分配n个T大小的空间
        // 内部调用底层的allocate()，也就是两级分配器
        static T *allocate(size_t n) {
            if (n == 0)
                return 0;
            return (T*) Alloc::allocate(n * sizeof(T));     // 将具体空间大小转换为字节
        }

        // 分配一个T大小的空间
        static T *allocate(void) {
            return (T*) Alloc::allocate(sizeof(T));
        }

        // 回收p指向的内存中n个T大小的空间
        static void deallocate(T *p, size_t n) {
            if (n != 0)
                Alloc::deallocate(p, n * sizeof(T));
        }

        // 回收一个T大小的空间
        static void deallocate(T *p) {
            Alloc::deallocate(p, sizeof(T));
        }


    };
}






#endif //STL_MY_ALLOCATOR_MY_ALLOCATOR_H

