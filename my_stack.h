//
// Created by HP on 2020/6/16.
//

#ifndef STL_MY_ALLOCATOR_MY_STACK_H
#define STL_MY_ALLOCATOR_MY_STACK_H

#include "my_deque.h"

// 利用deque实现stack
// 这个stack就像是容器的adapter，适配器
// 充分利用底层容器实现其他的功能
// 书上所说的“修改某物接口，形成另一种风貌”，称为适配器
template <class T, class Sequence = deque<T> >
class stack {

    friend bool operator==(const stack<T, Sequence>&, const stack<T, Sequence>&);
    friend bool operator<(const stack<T, Sequence>&, const stack<T, Sequence>&);

public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

protected:
    Sequence c;


public:
    // 利用底层Sequence的操作来完成stack的操作
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference top() { return c.back(); }
    const_reference top() const { return c.back(); }

    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_back(); }

};

template <class T, class Sequence>
bool operator==(const stack<T, Sequence>& x, const stack<T, Sequence>& y) {
    return x.c == y.c;
}

template <class T, class Sequence>
bool operator<(const stack<T, Sequence>& x, const stack<T, Sequence>& y) {
    return x.c < y.c;
}

#endif //STL_MY_ALLOCATOR_MY_STACK_H
