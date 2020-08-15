//
// Created by 54969 on 2020/3/17.
//
#include "first_level_alloc.h"
#include "second_level_alloc.h"
#include "my_uninitialized.h"


#ifndef STL_MY_ALLOCATOR_MY_VECTOR_H
#define STL_MY_ALLOCATOR_MY_VECTOR_H


typedef __malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;     // 令alloc为第一级分配器


template <class T, class Alloc = alloc>
class vector {
public:
    // vector的嵌套类型定义
    typedef T           value_type;         // 元素类型
    typedef value_type* pointer;            // 元素指针类型
    typedef value_type* iterator;           // 迭代器类型
    typedef value_type& reference;          // 元素引用类型
    typedef size_t      size_type;          // 元素数量的类型
    typedef ptrdiff_t   difference_type;    // 元素指针差值的类型

protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;     // 内存分配器，底层是由第一级分配器和第二级分配器构成

    iterator start;             // 迭代器，表示目前使用空间的头部。实际上这三个迭代器都是指向元素的指针
    iterator finish;            // 迭代器，表示目前使用空间的尾部
    iterator end_of_storage;    // 迭代器，表示目前可用空间的尾部

    // 插入时内存空间不够用，需要调整内存空间的辅助函数
    void insert_aux(iterator position, const T& x);

    // 在position开始的位置连续插入n个X的拷贝
    void insert(iterator position, size_type n, const T& x);

    // 回收vector使用的内存空间，主要是借用内存分配器提供的回收接口实现
    void deallocate() {
        if (start != NULL)
            data_allocator::deallocate(start, (size_type)(end_of_storage - start));
    }

    // 在vector中初始化n个元素
    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    // 返回指向头元素的迭代器
    iterator begin() { return start; }
    // 指向尾元素的下一个位置的迭代器
    iterator end() { return finish; }
    size_type size() { return (size_type)(end() - begin()); }
    // 返回vector所申请的内存空间大小（以元素的大小为单位）
    size_type capacity() const { return (size_type)(end_of_storage - begin()); }
    bool empty() const { return begin() == end(); }
    reference operator[](size_type n) { return *(begin() + n); }

    // 构造函数
    // 默认构造函数
    vector() : start(0), finish(0), end_of_storage(0) {}
    // 构造n个value(size_type)
    vector(size_type n, const T& value) {
        fill_initialize(n, value);
    }
    // int
    vector(int n, const T& value) {
        fill_initialize(n, value);
    }
    // long
    vector(long n, const T& value) {
        fill_initialize(n, value);
    }
    // 构造n个默认初始化的元素，调用T的默认构造函数
    // 只有一个参数的构造函数需要加explicit，避免隐式类型转换
    explicit vector(size_type n) {
        fill_initialize(n, T());
    }


    // 析构函数
    // 借助allocator头文件中的destroy()对所有元素进行析构
    // 调用的destroy()版本是以两个迭代器区间为参数的版本
    // 然后再将已申请的内存空间进行回收，调用allocator中的deallocate()
    // 析构所有元素和释放内存空间是两个分开的步骤
    ~vector() {
        destroy(start, finish);
        deallocate();
    }

    // 返回首元素的引用
    reference front() { return *begin(); }
    // 返回尾元素的引用
    reference back() { return *(end()-1); }
    // 将元素插入至最尾端
    // 如果vector中还有内存空间，使用construct，借助复制构造函数对X进行复制，并构造一个新的对象，放在已经申请的内存空间上
    // 如果申请的内存空间已用完，则需要重新申请内存空间进行扩容，然后将原来的元素复制到新的内存空间中。这部分操作放在insert_aux()中实现
    void push_back(const T& X) {
        if (finish != end_of_storage) {
            // 借助construct()函数在已分配的内存空间上构造对象
            construct(finish, X);
            finish++;
        }
        // 如果没有多余的内存空间，需要重新申请新的内存空间，并进行元素的转移
        else
            insert_aux(end(), X);
    }

    // 弹出尾部元素。实际上就是对尾部元素进行析构，但是不回收内存空间，并对finish的值进行更新
    void pop_back() {
        if (start != finish) {
            --finish;
            // 借助destroy()对finish指向的内存空间进行对象的析构
            destroy(finish);
        }
    }

    // 清除[first, last)中的所有元素
    // 先将后面的一部分元素往前覆盖，然后删除最后的那几个元素
    // 函数返回指向删除起始位置的迭代器
    iterator erase(iterator first, iterator last) {
        // copy()为全局函数，具体实现在第6章
        // 将last到finish的元素依次拷贝到first开始的内存空间
        iterator i = copy(last, finish, first);
        // 然后析构i到finish的元素对象
        destroy(i, finish);
        // 更新finish迭代器
        finish = finish - (last - first);
        return first;
    }

    // 清除某个位置上的元素
    // 需要分清该位置的元素是否为最后一个元素，如果是最后一个元素直接删除就好
    // 如果不是，还需要将后面的元素往前移动
    // 函数返回指向删除位置的迭代器
    iterator erase(iterator position) {
        // 如果当前元素不是最后一个元素，往前覆盖
        // 最终都是直接删除最后一个位置的元素
        if (position+1 != finish) {
            // 使用copy()将后面的元素往前移动
            // 将position+1到finish的元素一次拷贝到position开始的内存空间
            copy(position+1, finish, position);
        }
        --finish;
        // 析构最后一个元素对象
        destroy(finish);
        // 返回删除的位置
        return position;
    }

    // 重设vector的size（不涉及到内存空间的释放，只是对元素进行添加或者抹除erase）
    // 如果new_size大于原来的size，则多出来的位置复制X的值
    void resize(size_type new_size, const T& X) {
        // 如果小于，则需要抹除多余的元素
        if (new_size < size()) {
            erase(begin() + new_size, end());
        }
        // 如果大于，则需要添加new_size-size()个X（复制）
        else
            insert(end(), new_size-size(), X);
    }

    // 这个版本只指定new_size的大小，如果需要添加元素，使用默认的构造函数
    void resize(size_type new_size) {
        // 传入默认构造的对象作为复制的对象
        resize(new_size, T());
    }

    // 清除所有元素，直接使用erase()迭代器区间
    void clear() { erase(begin(), end()); }

protected:
    // 分配内存空间，并构造n个value对象，并返回指向内存空间首地址的迭代器（指向首元素的指针）
    // 这个需要使用分配器的allocate()进行内存空间的分配，以及uninitialized_fill_n()实现n个元素的拷贝初始化
    iterator allocate_and_fill(size_type n, const T& value) {
        iterator result = data_allocator::allocate(n);
        ::uninitialized_fill_n(result, n, value);
        return result;
    }
};

// 其实并不是在原有的空间上接续新空间，因为无法保证原有空间之后是否还有可供分配的内存空间
// 所以是以原空间大小的2倍重新申请内存空间，并将原有的元素拷贝至新的内存空间，最后释放原来的内存空间
// 所以原有的迭代器都会失效，都必须更新
//
template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(vector<T, Alloc>::iterator position, const T &x) {
    const size_type old_size = size();
    // 如果old_size为0，则new_size为1；否则如果old_size不为0，则new_size为2*old_size
    const size_type new_size = (old_size == 0 ? 1 : 2 * old_size);

    // 使用分配器的allocate()申请新的内存空间
    iterator new_start = data_allocator::allocate(new_size);
    iterator new_finish = new_start;


    try {
        // 接着将原vector的元素拷贝至新的内存空间中
        new_finish = uninitialized_copy(start, position, new_start);
        // 然后将需要添加的x拷贝到新的结尾中
        construct(new_finish, x);
        // 调整new_finish
        ++new_finish;
    }
    // 如果拷贝的过程中发生了异常，需要析构对象，然后回收新申请的内存空间
    // 接着抛出异常
    catch (...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, new_size);
        throw;
    }

    // 如果拷贝成功之后，先析构元素对象，然后回收原来的内存空间
    destroy(begin(), end());
    deallocate();

    // 注意！！！最后需要更新迭代器的值
    start = new_start;
    finish = new_finish;
    end_of_storage = start + new_size;
}

// 从position开始的位置，连续插入n个x的拷贝
template <class T, class Alloc>
void vector<T, Alloc>::insert(vector::iterator position, vector::size_type n, const T &x) {
    if (n != 0) {
        // 如果备用空间的大小满足插入n个x的拷贝，则直接插入
        if (size_type(end_of_storage - finish) >= n) {
            T x_copy = x;       // 使用T的拷贝构造函数
            iterator old_finish = finish;
            const size_type elems_after = finish - position;    // 计算position之后还有多少个元素，这些元素需要往后移动
            // 如果插入点之后的现有元素个数大于新增元素个数
            // 先将finish-n到finish之间的元素移动到finish之后
            // 然后再将position到old_finish-n的元素移动到以old_finish为结尾的内存单元，也就是往后移
            // 为什么需要分成这么多步骤呢？我认为是针对操作的内存空间是否已经初始化来区分
            // finish之后的内存空间都是未初始化的，所以这一部分的内存操作需要调用未初始化版本的函数
            // 而finish之前的内存空间是已经初始化的，所以调用初始化版本的函数
            if (elems_after > n) {
                // 因为finish之后的内存都是未初始化的，所以调用的是未初始化版本的copy
                uninitialized_copy(finish-n, finish, finish);   // 将finish-n到finish之间的元素移动到finish之后
                finish += n;
                // 因为old_finish之前的内存都是已经初始化的，所以使用的是初始化版本的copy
                // fill也是一个道理
                copy_backward(position, old_finish - n, old_finish);  // 以从后往前的顺序来拷贝
                fill(position, position + n, x_copy);   // 最后将x的拷贝填充到position至position+n的区间
            }
            // 如果插入点之后的现有元素个数小于等于新增元素个数
            else {
                // 先将需要增加的元素中最后面的那一部分放入到finish之后，调用未初始化版本的函数
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += (n - elems_after);
                // 然后将position至old_finish之前的那部分原有的元素拷贝到新的finish之后
                uninitialized_copy(position, old_finish, finish);
                // 最后将需要增加的元素中前面的那一部分赋值到position至old_finish之间
                // 因为这部分的内存空间已经初始化过，所以调用已初始化版本的函数
                fill(position, old_finish, x_copy);
            }
        }
        // 如果备用空间的大小不满足插入n个x的拷贝，则需要分配新的内存空间，并将原来的元素拷贝到新的内存空间中
        // 新的内存空间大小可以是旧长度的两倍，或是旧长度+新增元素个数，取决于哪一个更大
        else {
            const size_type old_size = size();
            const size_type new_size = old_size + max(old_size, n);

            // 分配新的内存空间，借助内存分配器
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;

            try {
                // 因为新的内存空间上都是没有初始化的，所以调用的都是未初始化版本的函数
                // 先将旧vector的插入点之前的元素复制到新的空间上
                new_finish = uninitialized_copy(start, position, new_finish);
                // 再将需要添加的元素填到后面
                new_finish = uninitialized_fill_n(new_finish, n, x);
                // 最后将旧vector上position至finish上的元素复制到新空间上
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
            catch (...) {
                // 如果发生了异常，实现“commit or rollback”
                // 这里是直接rollback，析构对象，并回收新申请的内存空间
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw;
            }

            // 注意！！！如果没有发生异常，说明拷贝成功，需要回收原来的内存空间，并更新迭代器
            // 先析构对象
            destroy(start, finish);
            // 释放空间
            deallocate();
            // 更新迭代器
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }
}




#endif //STL_MY_ALLOCATOR_MY_VECTOR_H
