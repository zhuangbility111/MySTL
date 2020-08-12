//
// Created by HP on 2020/6/17.
//

#ifndef STL_MY_ALLOCATOR_MY_HEAP_AND_PRIORITY_QUEUE_H
#define STL_MY_ALLOCATOR_MY_HEAP_AND_PRIORITY_QUEUE_H

#include <iterator>
#include "my_vector.h"

// 实际上heap只是提供一些抽象的算法，实际底层容器还是vector（或者array，但是array不能动态改变大小）
// 这些算法对底层容器进行操作，使其变成一个合格的heap
class heap {
public:
    // 添加元素到堆中，并动态调整堆结构
    // 此函数被调用时，保证新元素已经被置于底层容器的最尾端
    // 对这个元素执行“上浮”操作，找到它在堆中最合适的位置
    template <class RandomAccessIterator>
    inline static void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
        // 这里使用了iterator_traits特性萃取机将迭代器中的相关类型(distance_type, value_type)萃取出来
        __push_heap_aux(first, last, distance_type(first), value_type(first));
    }

    // 萃取函数
    // 返回difference_type*类型的值
    // 相当于将iterator中的difference_type类型萃取出来
    template <class Iterator>
    inline static typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 萃取函数
    // 返回value_type*类型的值
    // 相当于将iterator中的value_type类型萃取出来
    template <class Iterator>
    inline static typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    // Distance 和 T 实际上都是通过上面的两个萃取函数得到的类型
    // 实际上只是为了获得迭代器中的difference_type和value_type
    // 上一层函数萃取到类型之后，这一层函数给底层函数提供相关的值
    template <class RandomAccessIterator, class Distance, class T>
    inline static void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
        __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
    }

    // 实际执行上浮操作的底层函数
    // first参数为指向容器首元素的迭代器，hole_index为heap中最后一个元素的索引，也就是新元素的索引
    // top_index为heap中第一个元素的索引，也就是根节点的索引
    // value为新元素的值
    // 迭代器（或者说是容器）中的数据类型value_type和指针差值类型difference_type通过萃取剂萃取出来
    // 然后通过参数类型来传递
    // 也就是使用了这样的一种机制来获取迭代器（容器）中的细节信息
    // 如果不使用萃取剂，则heap类中无法获取容器与迭代器中的一些具体信息
    // 这里也体现出了萃取剂的抽象作用
    // 同时，函数是通过迭代器来访问底层容器的，相当于底层容器封闭起来，提供了抽象的方法
    // 无需知道底层容器是什么，也无论底层容器是什么，只要迭代器能够访问底层容器，并且这个迭代器符合stl的标准
    // 那么就可以使用迭代器操作容器，从而实现heap的特性
    // 所以函数参数只有迭代器和索引值，而没有具体的容器
    template <class RandomAccessIterator, class Distance, class T>
    static void __push_heap(RandomAccessIterator first, Distance hole_index, Distance top_index, T value) {
        // 先找到父节点的索引
        Distance parent = (hole_index - 1) / 2;
        // 不断上浮，直到根节点或者父节点的值大于value
        while (hole_index < top_index && *(first + parent) < value) {
            *(first + hole_index) = *(first + parent);
            hole_index = parent;
            parent = (hole_index - 1) / 2;
        }
        // 最后将value复制到最终上浮的位置
        *(first + hole_index) = value;
    }


    // 移除根部节点，需要将最底部的元素移动至根节点，然后执行下沉操作
    // 参数为容器的两个首尾迭代器，依然是隐藏了底层容器的细节，通过迭代器来操作容器
    template <class RandomAccessIterator>
    inline static void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
        // 萃取value_type
        __pop_heap_aux(first, last, value_type(first));
    }

    // 萃取到value_type后，为底层函数提供初值
    // 并且萃取出distance_type
    template <class RandomAccessIterator, class T>
    inline static void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
        // 萃取distance_type
        // 这里的将下一层的last设置为last-1，是因为last-1的位置需要存放弹出的元素，不需要将其纳入调整的范围
        // 所以操作的范围应该是[first, last-1)
        __pop_heap(first, last-1, last-1, T(*(last-1)), distance_type(first));
    }

    // 萃取到distance_type后，为底层函数提供初值
    // 参数result为保存需要弹出的元素的迭代器
    // value为最后一个节点的值
    template <class RandomAccessIterator, class T, class Distance>
    inline static void __pop_heap(RandomAccessIterator first, RandomAccessIterator last,
            RandomAccessIterator result, T value, Distance*) {
        // 将根结点的值（当前最大值）移动到序列的最后，保存起来
        // 也可以通过pop_back()将其弹出
        *result = *first;
        // 接着对堆进行调整（将最后一个节点移到根节点，然后对其执行下沉操作）
        // last-first实则为last-1-first = vec.size()-1
        __adjust_heap(first, Distance(0), Distance(last - first), value);
    }

    // 调整堆的函数（下沉）
    // value为最后一个节点的值
    // hole_index为缺口的索引（也就是根节点的索引，因为根节点被移除）
    // len为last-1到first的距离，也可以理解为容器的长度-1，也就是最后一个节点的索引
    // 因为调整的范围是[first, last-1)，last-1的位置保存弹出的结果
    template <class RandomAccessIterator, class T, class Distance>
    static void __adjust_heap(RandomAccessIterator first, Distance hole_index, Distance len, T value) {
        // 父节点的索引
        Distance top_index = hole_index;
        // 右子节点的索引
        Distance second_child_index = 2 * hole_index + 2;
        while (second_child_index < len) {
            // 获取左右子节点的最大值
            if (*(first + second_child_index) < *(first + second_child_index - 1))
                second_child_index--;
            // 如果子节点大于最后一个节点的值，则子节点往上移动，最后一个节点继续往下沉
            if (value < *(first + second_child_index)) {
                *(first + hole_index) = *(first + second_child_index);
            }
            else
                break;
            // 更新索引，往下一层
            hole_index = second_child_index;
            second_child_index = 2 * hole_index + 2;
        }
        // 相当于最后一个节点只有左子节点
        if (second_child_index == len) {
            if (value < *(first + second_child_index - 1)) {
                *(first + hole_index) = *(first + second_child_index - 1);
                hole_index = second_child_index - 1;
            }
        }
        // 最后将最后那个节点的值放到它最终的位置上
        *(first + hole_index) = value;
    }


    template <class RandomAccessIterator>
    static void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
        // 每执行一次pop，就会有一个当前的最大值移动到当前范围内的最后
        // 所以执行n次，相当于对容器做了排序
        while (last - first > 1) {
            pop_heap(first, last--);
        }
    }

    // 将[first, last)排列为一个heap
    template <class RandomAccessIterator>
    inline static void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
        // 萃取出value_type和distance_type
        __make_heap(first, last, value_type(first), distance_type(first));
    }

    template <class RandomAccessIterator, class T, class Distance>
    static void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
        // 从最小的子树开始，对每一棵子树进行调整，使其变为堆，也就是对每一棵子树的根节点执行下沉操作
        // 然后一直变大，直到最后对最大的子树做堆调整
        if (last - first < 2)
            return;
        Distance len = last - first;
        Distance parent = (len - 2) / 2;
        while (true) {
            __adjust_heap(first, parent, len, T(*(first + parent)));
            if (parent == 0)
                return;
            --parent;
        }
    }
};

template <class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type> >
class priority_queue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

protected:
    Sequence c;     // 底层容器
    Compare comp;   // 元素大小比较的函数对象

public:
    // 默认构造函数
    priority_queue() : c() {  }

    explicit priority_queue(const Compare& x) : c(), comp(x) {  }

    // 用到的make_heap()、push_heap()、pop_heap()都是泛型算法
    // 实际上在构造优先队列时，就会构造底层容器，而且会使底层容器拥有heap的特性
    // 这样才满足优先队列的定义
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x) :
                c (first, last), comp(x) {
        // 调用make_heap()使底层容器拥有堆的性质
        heap::make_heap(first, last);
    }

    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last) :
                c (first, last) {
        // 调用make_heap()使底层容器拥有堆的性质
        heap::make_heap(first, last);
    }

    bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    const_reference top() const {
        return c.front();
    }

    // 先将元素压入容器的尾部，然后使用堆函数来调整容器满足堆的特性
    void push(const value_type& x) {
        c.push_back(x);
        heap::push_heap(c.begin(), c.end());
    }

    // 先使用堆函数将元素从堆中去除，然后在容器上真正地去除
    void pop() {
        heap::pop_heap(c.begin(), c.end());
        c.pop_back();
    }

};

#endif //STL_MY_ALLOCATOR_MY_HEAP_AND_PRIORITY_QUEUE_H
