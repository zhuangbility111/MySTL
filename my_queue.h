//
// Created by HP on 2020/6/16.
//

#ifndef STL_MY_ALLOCATOR_MY_QUEUE_H
#define STL_MY_ALLOCATOR_MY_QUEUE_H

#include "my_deque.h"

// 利用deque实现queue
// 这个queue就像是容器的adapter，适配器
// 充分利用底层容器实现其他的功能
// 书上所说的“修改某物接口，形成另一种风貌”，称为适配器

template <class T, class Sequence = deque<T> >
class queue {
    friend bool operator==(const queue<T, Sequence>&, const queue<T, Sequence>&);
    friend bool operator<(const queue<T, Sequence>&, const queue<T, Sequence>&);


public:
    // 利用原有的deque中的类型
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

protected:
    Sequence c;

public:
    // 利用deque的操作实现queue的操作
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference front() { return c.front(); }
    const_reference front() const { return c.front(); }
    reference back() { return c.back(); }
    const_reference back() const { return c.back(); }
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }

};

// 满足对称性，所以使用非成员函数
template <class T, class Sequence>
bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.c == y.c;
}

template <class T, class Sequence>
bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
    return x.c < y.c;
}

#endif //STL_MY_ALLOCATOR_MY_QUEUE_H
