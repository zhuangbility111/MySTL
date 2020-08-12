//
// Created by 54969 on 2020/3/13.
//

#ifndef STL_MY_ALLOCATOR_MY_UNINITIALIZED_H
#define STL_MY_ALLOCATOR_MY_UNINITIALIZED_H

#include "my_allocator.h"
#include <stdio.h>
#include <string.h>
#include <type_traits>
#include <bits/valarray_after.h>


/*
 * 主要是实现uninitialized_copy()、uninitialized_fill()、uninitialized_fill_n()
 * 它们都是作用于未初始化（但已经申请）的内存空间上
 * 所以需要使用到my_allocator.h中定义的construct()构造以及destroy()析构
 */


// uninitialized_fill函数，负责将x填充到first的内存空间上
// 首先萃取出迭代器first的value_type，也就是first的模板参数类型，然后判断该类型是否为POD类型
// POD意为Plain Old Data，也就是标量类型，或者是最传统的基本数据类型
// POD里欸行必然拥有trivial ctor/dtor/copy/assignment函数
// 因此针对POD类型才有最有效率的初值填写手法（类似于直接将3付给一个int变量），而对non-POD型则使用拷贝构造函数
template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

// 通过模板参数来传递迭代器中的元素类型，然后判断它是否为POD类型
template <class ForwardIterator, class Size, class T, class T1>
ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 如果拷贝构造函数等同于直接赋值，而且destructor是trivial的，则调用以下函数
// 也就是如果是POD类型（__true_type），则会调用以下函数
// 借用function template的参数推导机制实现的
template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
    return fill_n(first, n, x);     // 如果是POD类型，则直接借助高阶函数实现。
}

// 如果不是POD类型，则执行以下函数。借助function template的参数推导机制实现的
// 也就是如果不是POD类型（__false_type），则会调用以下函数
template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
    ForwardIterator cur = first;
    // 循环调用construct()在已申请的内存空间上进行拷贝构造
    // &*cur为指向已申请内存空间的指针，x为需要拷贝的值
    for(; n > 0; n--, cur++) {
        construct(&*cur, x);
    }
    return cur;
}

// --------------------------------------------------------------------------------------------------------
// uninitialized_fill函数，负责将x填充到从first开始连续n个位置的内存空间上
// 这是在已经分配的内存空间上进行的操作
// 具体实现与fill_n()是一样的，先判断迭代器保存的元素类型是否为POD类型，
// POD类型则直接赋值，非POD类型就需要使用底层的construct()构造对象
template <class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
    return __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T, class T1>
void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_aux(first, n, x, is_POD());
}

// POD版本
template <class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
    fill(first, last, x);
}

// 非POD版本
template <class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
    ForwardIterator cur = first;
    for (; cur != last; cur++) {
        construct(&*cur, x);
    }
}


// ---------------------------------------------------------------------------------------------------
// uninitialized_copy函数，它是负责将first至last范围内的对象拷贝到result指定的位置上
// 这是在已经分配的内存空间上进行的操作，需要用到在已分配的内存空间上构造对象的construct()
// 具体实现与fill_n()是一样的，先判断迭代器保存的元素类型是否为POD类型，
// POD类型则直接赋值，非POD类型就需要使用底层的construct()构造对象
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return __uninitialized_copy(first, last, result, value_type(result));
}

template <class InputIterator, class ForwardIterator, class T>
ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator, class T>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
    return copy(first, last, result);       // POD类型直接调用高阶函数实现
}

template <class InputIterator, class ForwardIterator, class T>
ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
    // 借用construct()实现
    ForwardIterator cur = result;
    for(; first != last; first++, cur++) {
        construct(&*cur, *first);        // 一个一个地调用construct()，result指向已申请但未构造的内存空间，first为需要拷贝的值
    }
    return cur;
}

// 针对char*和wchar_t*两种类型，可以使用最具效率的做法memmove()（直接移动内存内容）来执行复制行为
// 所以特别为这两种类型设计特例化版本
inline char* uninitialized_copy(const char* first, const char* last, char* result) {
    // 直接使用memmove()，第一个参数为指向目的地址的指针，第二个参数为指向源地址的指针，第三个参数为拷贝的字节数
    memmove(result, first, last-first);
    return result + (last-first);
}

inline wchar_t *uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    // 直接使用memmove()
    memmove(result, first, sizeof(wchar_t) * (last-first));
    return result + (last-first);
}




#endif //STL_MY_ALLOCATOR_MY_UNINITIALIZED_H
