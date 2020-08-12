//
// Created by HP on 2020/7/9.
//

#ifndef STL_MY_ALLOCATOR_MY_SET_H
#define STL_MY_ALLOCATOR_MY_SET_H

#include "RB-tree.h"

// 以红黑树为底层数据结构实现set
// 因为之前已经封装好红黑树，将其作为一个完整的容器
// 所以很多set的操作直接接用红黑树的操作来实现
// 大大降低了set的复杂性
// set中每一个元素的key和value是相同的
// Key为键值类型，Compare为比较对象，Alloc为内存分配对象
template <class Key, class Compare = less<Key>, class Alloc = alloc>
class set {
public:
    // typedefs
    typedef Key key_type;
    typedef Key value_type;

    typedef Compare key_compare;
    typedef Compare value_compare;

private:
    template <class T>
    struct identity : public unary_function<T, T> {
        const T& operator()(const T& x) const {
            return x;
        }
    };

    // typedef 红黑树类
    typedef rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc> rep_type;

    // 定义底层数据结构红黑树
    rep_type t;

public:
    // 因为如果通过迭代器或者指针可以修改底层红黑树的值，会导致set或者说是红黑树的组织混乱
    // 所以全部都设置成指向常量的，防止通过迭代器或者指针来修改底层红黑树的元素值
    // 要想修改只能通过红黑树提供的接口
    // 可以访问，只是不能修改
    typedef typename rep_type::const_pointer pointer;
    typedef typename rep_type::const_pointer const_pointer;
    typedef typename rep_type::const_reference reference;
    typedef typename rep_type::const_reference const_reference;
    typedef typename rep_type::const_iterator iterator;
    typedef typename rep_type::const_iterator const_iterator;

    typedef typename rep_type::size_type size_type;
    typedef typename rep_type::difference_type difference_type;

    // 构造函数
    set() : t(Compare()) {}

    explicit set(const Compare& comp) : t(comp) {}

    template <class InputIterator>
    set (InputIterator first, InputIterator last) : t(Compare()) {
        t.insert_unique(first, last);
    }

    template <class InputIterator>
    set(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
        t.insert_unique(first, last);
    }

    // 拷贝构造函数
    // 其实就是拷贝底层的红黑树
    set(const set<Key, Compare, Alloc>& x) : t(x.t) { }

    // 拷贝赋值运算符
    // 其实就是赋值底层的红黑树
    set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
        t = x.t;
        return *this;
    }

    // 定义完类型和构造函数之后，接下来定义set相关的操作
    // 最终都是调用底层红黑树的操作接口，所以操作逻辑都不复杂

    // 因为key和value相同，所以均返回红黑树的key_comp()
    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return t.key_comp(); }

    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    reverse_iterator rbegin() const { return t.rbegin(); }
    reverse_iterator rend() const { return t.rend(); }

    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }
    void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }

    // 插入和删除
    // 直接插入元素，在红黑树中会自动调整它的插入位置
    // 必须使用insert_unique，因为set中元素不重复
    pair<iterator, bool> insert(const value_type& x) {
        pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
        return pair<iterator, bool> (p.first, p.second);
    }

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        t.insert_unique(first, last);
    }

    void erase(iterator position) {
        t.erase((typename rep_type::iterator) position);
    }

    size_type erase(const key_type& x) {
        return t.erase(x);
    }

    void erase(iterator first, iterator last) {
        t.erase((typename rep_type::iterator) first, (typename rep_type::iterator) last);
    }

    void clear() {
        t.clear();
    }

    // set的一些操作函数，全都是借助红黑树t的操作实现
    iterator find(const key_type& x) const { return t.find(x); }
    size_type count(const key_type& x) const { return t.count(x); }

    // 返回一个迭代器，指向第一个关键字不小于k的元素
    iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }
    // 返回一个迭代器，指向第一个关键字大于k的元素
    iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }
    // 返回一个迭代器pair，表示关键字等于k的元素的范围
    pair<iterator, iterator> equal_range(const key_type& x) const { return t.equal_range(x); }


};

#endif //STL_MY_ALLOCATOR_MY_SET_H
