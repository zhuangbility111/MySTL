//
// Created by 54969 on 2020/3/19.
//

#ifndef STL_MY_ALLOCATOR_MY_LIST_H
#define STL_MY_ALLOCATOR_MY_LIST_H

#include "my_allocator.h"

/*
 * list的源代码
 */

// 首先是节点结构体，T为节点中的元素的类型
template <class T>
struct __list_node {
    typedef void* void_pointer;     // 指针类型为void*。为什么是void指针呢，因为方便向各种类型的指针进行转换
    void_pointer prev;      // 指向前一个节点的指针
    void_pointer next;      // 指向后一个节点的指针
    T data;     // 元素
};

// 接着是list迭代器的设计
template <class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*>      iterator;   // 迭代器类型
    typedef __list_iterator<T, Ref, Ptr>    self;       //

    typedef bidirectional_iterator_tag iterator_category;   // 迭代器的类型为双向迭代器。tag用于在调用一些底层函数时识别这个迭代器的类型
    typedef T value_type;               // 元素的类型
    typedef Ptr pointer;                // 元素的指针类型
    typedef Ref reference;              // 元素的引用类型
    typedef __list_node<T>* link_type;  // 指向节点的指针类型
    typedef size_t size_type;           // 元素数目的类型
    typedef ptrdiff_t difference_type;  // 迭代器差值的类型

    link_type node;     // 迭代器内部的指向节点的指针

    // 构造函数
    __list_iterator() = default;
    __list_iterator(link_type x) : node(x) {}
    __list_iterator(const iterator& x) : node(x.node) {}


    // 接着重载运算符
    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }

    // 解引用，取数据值
    reference operator*() const { return node->data; }
    // 成员存取运算子的标准做法
    pointer operator->() const { return &(operator*()); }

    // 最后是自增自减
    // 前缀自增
    // 返回的是当前对象(*this)，所以可以返回引用
    self& operator++() {
        // node指向下一个节点
        node = (link_type)(node->next);
        return *this;
    }
    // 后缀自增
    // 返回的是临时对象，所以不能返回引用
    self operator++(int) {
        self tmp = *this;
//        this->node = (link_type)(node->next);
        ++*this;
        return tmp;

    }

    // 前缀自减
    self& operator--() {
        node = (link_type)(node->prev);
        return *this;
    }

    // 后缀自减
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};


// 最后实现list结构。T为元素类型，Alloc为内存分配器类型
template <class T, class Alloc = alloc>
class list {
protected:
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;     // 专属内存分配器，以节点作为分配单位，每次分配一个节点大小的内存空间
public:
    typedef list_node* link_type;
    // list的迭代器类型
    typedef __list_iterator<T, T&, T*> iterator;
    typedef size_t size_type;
    typedef T& reference;              // 元素的引用类型

protected:
    // 指向起始节点的指针，哨兵节点
    // 将这个起始节点置为空节点，可以理解为是头节点与尾节点的一个连接节点，相当于是list的end()（list的结尾）
    link_type node;

    // 分配一个节点并返回
    link_type get_node() { return list_node_allocator::allocate(); }
    // 释放一个节点的内存空间
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    // 申请一个节点的内存空间，并进行构造，带有元素值，返回指向这个节点的指针
    link_type create_node(const T& x) {
        // 申请内存空间
        link_type p = get_node();
        // 构造对象
        construct(&p->data, x);
        return p;
    }

    // 销毁一个节点，析构并释放内存空间
    void destroy_node(link_type p) {
        // 析构对象
        destroy(&p->data);
        // 释放内存空间
        put_node(p);
    }

    // 产生一个空链表
    // 实际上就是新建一个空节点，node指向这个空节点。这个节点的next和prev都是指向自己，data不需要进行构造
    void empty_initialize() {
        // 分配一个节点大小的内存空间，并使用node指向它
        node = get_node();
        // 将next和prev都指向自己
        // 数据data不需要进行构造
        node->next = node;
        node->prev = node;
    }

    // 将[first, last)内的所有元素移动到position之前
    // 这是后面splice()、merge()、reverse()实现的基础
    void transfer(iterator position, iterator first, iterator last) {
        if (position != first) {
            link_type(last.node->prev)->next = position.node;
            link_type(first.node->prev)->next = last.node;
            link_type(position.node->prev)->next = first.node;
            link_type tmp = link_type(position.node->prev);
            position.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = tmp;
        }
    }


public:
    // 默认构造函数，产生一个空链表
    list() { empty_initialize(); }

    // begin()，返回node指向的空节点的下一个节点
    // 空节点是尾节点与头节点的连接节点
    // 所以list的头节点是node->next
    iterator begin() { return (link_type)((*node).next); }

    // 直接返回node指向的空节点
    // list的尾节点就是node
    iterator end() { return node; }

    // 如果node与node->next都指向node，说明list中只有一个node指向的空节点，list为空
    // 否则不为空
    bool empty() const { return (begin() == end()); }
    size_type size() const {
        return (size_type) distance(begin(), end());
    }

    // 取头节点的元素值
    reference front() { return *begin(); }
    // 取尾节点的元素值
    reference back() { return *(--end()); }

    // 指定位置插入节点（位置之前插入）
    iterator insert(iterator position, const T& x) {
        // 申请节点的内存空间，并初始化节点内部的data
        link_type p = create_node(x);
        // 将节点p插入list中position节点之前
        p->next = position.node;
        p->prev = position.node->prev;
        position.node->prev = p;
        ((link_type)p->prev)->next = p;
//        return iterator(p);     // 构造一个新的迭代器返回
        return p;       // 或者执行隐式类型转换，转换成迭代器类型
    }

    // 插入一个节点作为头节点
    void push_front(const T& x) { insert(begin(), x); }

    // 最后位置插入节点
    void push_back(const T& x) { insert(end(), x); }

    // 移除迭代器position所指向的节点，返回指向下一个节点的迭代器
    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        // 销毁节点，包括析构和回收内存空间
        destroy_node(position.node);
        return iterator(next_node);     // 通过指针构造一个新的迭代器返回
    }

    // 移除头节点
    void pop_front() { erase(begin()); }
    // 移除尾节点
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }

    // 清除所有节点
    void clear() {
        // 指向头节点，从头节点开始依次销毁节点
        link_type cur = begin().node;
        // node指向空节点，也就是尾节点与头节点之间的哨兵节点。清除时只需留下一个空节点就行
        while (cur != node) {
            link_type tmp = cur;
            cur = link_type(cur->next);
            // 销毁当前节点
            destroy_node(tmp);
        }
        // 接着恢复空节点的原始状态
        node->next = node;
        node->prev = node;
    }

    // 将数值为value的所有元素移除
    void remove(const T& value) {
        iterator first = begin();
        iterator last = end();
        while (first != last) {
            if (*first == value) {
                first = erase(first);
            }
            else
                ++first;
        }
    }

    // 移除数值相同的连续元素。只有“连续而且相同的元素”，才会被移除到只剩下一个
    void unique() {
        iterator first = begin();
        iterator last = end();
        // 空链表则什么都不做
        if (first == last)
            return;
        iterator next = first;
        while (++next != last) {
            // 如果相等，则将next指向的节点销毁
            if (*first == *next)
                erase(next);
            // 不相等，则调整first，指向next所指向的节点
            else
                first = next;
            next = first;
        }
    }

    // 将list x接合于position所指的位置之前。x必须不同于*this
    // 实际上就是将list.begin()到list.end()之间的元素插入到position位置之前，与transfer()的功能相同
    // 可以借助transfer实现
    void splice(iterator position, list& x) {
        // x不为空，也就是x中确实拥有元素，才执行转移
        if (!x.empty()) {
            transfer(position, x.begin(), x.end());
        }
    }

    // 将迭代器i所指的元素接合于position所指的位置之前。position和i可以指向同一个list上的元素
    // 也是可以借助transfer实现，只要将区间设置为[i,i+1)，然后使用transfer(position, i, i+1)
    void splice(iterator position, list&, iterator i) {
        iterator j = i;
        ++j;
        // 如果position本来就与i指向同一个元素，或者本来i就在position前面，不需要移动
        if (position == i || position == j)
            return;
        transfer(position, i, j);
    }

    // 将[first, last)内的所有元素接合于position所指的位置之前
    // position和[first, last)可以指向同一个list
    // 但position不能再[first, last)之内
    void splice(iterator position, list&, iterator first, iterator last) {
        // 如果first不等于last，也就是first到last之间确实拥有元素，才执行转移
        if (first != last)
            transfer(position, first, last);
    }


    // 将x合并到*this身上。两个lists的内容都必须先经过递增排序
    void merge(list<T, Alloc>& x) {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        // 注意！！！前提是两个lists都已经过自增排序
        while (first1 != last1 && first2 != last2) {
            // 如果first2指向的元素小于first1指向的元素
            // 也就是this的当前元素大于x的当前元素
            // 将x的当前元素插入到this的当前元素前面
            if (*first2 < *first1) {
                iterator next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            }
            // 否则如果first2指向的元素大于等于first1指向的元素
            // 将first1往后移动，直到找到了比first2大的元素，或者直接移动到first1的结尾
            else {
                ++first1;
            }
        }

        // 如果最终first2不等于last2，说明在上一步中first1已经移动到结尾
        // 那么直接将first2到last2之间的元素插入到*this的后面，也就是last1的后面
        if (first2 != last2)
            transfer(last1, first2, last2);
    }


    // 将*this中的内容进行逆置
    void reverse() {
        // 如果*this的长度为0或者1，直接返回
        if (node->next == node || link_type(node->next)->next == node)
            return;
        iterator cur = begin();
        ++cur;
        // 循环将后面的节点依次插入到头节点的前面
        while (cur != end()) {
            iterator old = cur;
            ++cur;
            transfer(begin(), old, cur);
        }

    }

    // list不能使用STL算法sort()，必须使用自己的sort()
    // 因为STL算法sort()只接受RandomAccessIterator，而list的迭代器为BidirectionalIterator
    // 使用的是quick sort()
    // 其实应该是归并排序，方法很巧妙
    // https://blog.csdn.net/qq_31720329/article/details/85535787
    void sort() {
        // 如果*this的长度为0或者1，直接返回
        if (node->next == node || link_type(node->next)->next == node)
            return;

        // 设置临时数据存放区
        // 调用默认构造函数产生一个空链表和一个空链表数组
        // counter[0]存放size为2^0=1个的链表（有序）
        // counter[1]存放size为2^1=2个的链表（有序）
        // counter[2]存放size为2^2=4个的链表（有序）
        // ····
        // counter[n]存放size为2^n个的链表（有序）
        // 将相同size的链表进行merge，将merge的结果放置到carry中，然后再从carry转移到存放2*size的list中
        // 所以就是1+1=2;2+2=4;4+4=8······
        // 这就是典型的归并排序
        list<T, Alloc> carry;
        list<T, Alloc> counter[64];

        int fill = 0;   // fill可以理解为当前有序子链表的最长长度的对数log，也就是2^fill=size(当前最长有序子链表)
        while (!empty()) {
            // 将*this的头节点插入到carry中，作为carry的新的头节点
            carry.splice(carry.begin(), *this, begin());
            int i = 0;  // i可以理解为当前正在处理（merge）的子链表的长度的对数log，也就是2^i=size(当前正在处理的子链表)
            while (i < fill && !counter[i].empty()) {
                // 将carry中的子链表与counter[i]中的子链表进行merge
                counter[i].merge(carry);
                // 然后将counter[i]中merge后的子链表放入到carry中，准备与counter[i+1]中的子链表进行merge
                carry.swap(counter[i++]);
            }
            // 经过上面的merge之后，此时最长有序子链表的长度为2^fill，所以需要将其放入到counter[i]
            // 而如果当前merge后的子链表还没有达到最长有序子链表的长度，则保存到counter[i]中，等到接下来的合并
            // 所以不管有没有merge，这里都是将carry中的子链表放入到counter[i]中
            carry.swap(counter[i]);
            // 如果此时i等于fill，说明刚完成两段最长有序子链表的merge，所以接下来需要merge得到2^(fill+1)长度的子链表
            // 因此fill++
            if (i == fill)
                fill++;
        }

        // *this空了以后，说明所有的子链表都在counter中，且所有子链表都是有序的
        // 那么遍历counter，将这些子链表进行merge，就可以得到有序的结果
        for (int i = 1; i < fill; i++) {
            counter[i].merge(counter[i-1]);
        }
        // 将merge后的有序链表换回到*this中
        swap(counter[fill-1]);

    }
};


#endif //STL_MY_ALLOCATOR_MY_LIST_H
