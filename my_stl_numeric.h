//
// Created by HP on 2020/8/1.
//

#ifndef STL_MY_ALLOCATOR_MY_STL_NUMERIC_H
#define STL_MY_ALLOCATOR_MY_STL_NUMERIC_H

// accumulate函数，对迭代器范围内的元素求和
// 版本1，使用operator+
template <class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init) {
    for(; first != last; first++) {
        init = std::move(init) + *first;
    }
    return init;
}

// 版本2，使用指定的BinaryOperation
template <class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation op) {
    for(; first != last; first++) {
        init = op(std::move(init), *first);
    }
    return init;
}

// 计算 [first, last) 范围中每对相邻元素的第二个和第一个的差，
// 并写入它们到始于 d_first + 1 的范围。写入不修改的 *first 副本到 *d_first 。
// 返回指向最后被写入元素后一位置的迭代器。
// 版本1，使用operator-
template <class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator d_first) {
    if (first == last)
        return d_first;
    // 先将*first保存到d_first上，然后d_first后面的迭代器才保存差值
    // 这样做是为了有足够的信息来重建输入区间的原始内容
    typedef typename std::iterator_traits<InputIterator>::value_type value_t;
    *d_first = *first;
    // 保存前一个元素
    value_t acc = *first;
    while (++first != last) {
        // 保存后一个元素
        value_t val = *first;
        *++d_first = val - std::move(acc);
        acc = std::move(val);
    }
    return ++d_first;
}

// 版本2，使用指定的BinaryOperation
template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator d_first, BinaryOperation op) {
    if (first == last)
        return d_first;
    // 先将*first保存到d_first上，然后d_first后面的迭代器才保存差值
    // 这样做是为了有足够的信息来重建输入区间的原始内容
    typedef typename std::iterator_traits<InputIterator>::value_type value_t;
    *d_first = *first;
    // 保存前一个元素
    value_t acc = *first;
    while (++first != last) {
        // 保存后一个元素
        value_t val = *first;
        *++d_first = op(val, std::move(acc));
        acc = std::move(val);
    }
    return ++d_first;
}

// 计算内积（即积之和）或在范围 [first1, last1) 和始于 first2 的范围上进行有序映射/规约操作。
// 版本1，使用operator+和operator*
template <class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
    while (first1 != last1) {
        init = std::move(init) + *first1 * *first2;
        ++first1;
        ++first2;
    }
    return init;
}

// 版本2，使用二元操作
template <class InputIterator1, class InputIterator2, class T, class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
                BinaryOperation1 op1, BinaryOperation2 op2) {
    while (first1 != last1) {
        init = op1(std::move(init) + op2(*first1, *first2));
        ++first1;
        ++first2;
    }
    return init;
}

// 计算范围 [first, last) 的子范围中元素的部分和，并写入到始于 d_first 的范围。
// 第一版本用 operator+ ，第二版本用给定的二元函数 op 对元素求和
// 返回指向最后被写入元素后一元素的迭代器。
template <class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator d_first) {
    if (first == last)
        return d_first;
    typename std::iterator_traits<InputIterator>::value_type sum = *first;
    // 将第一个first的值保存到第一个d_first中
    *d_first = sum;
    while (++first != last) {
        sum = std::move(sum) + *first;
        *++d_first = sum;
    }
    // 返回指向最后被写入元素后一元素的迭代器
    return ++d_first;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator d_first, BinaryOperation op) {
    if (first == last)
        return d_first;
    typename std::iterator_traits<InputIterator>::value_type sum = *first;
    // 将第一个first的值保存到第一个d_first中
    *d_first = sum;
    while (++first != last) {
        sum = op(std::move(sum), *first);
        *++d_first = sum;
    }
    // 返回指向最后被写入元素后一元素的迭代器
    return ++d_first;
}

// 用来设定某个区间的内容，使其内的每一个元素从指定的value值开始，呈现递增状态
template <class ForwardIterator, class T>
void iota(ForwardIterator first, ForwardIterator last, T value) {
    while (first != last) {
        *first = value;
        ++first;
        ++value;
    }
}





#endif //STL_MY_ALLOCATOR_MY_STL_NUMERIC_H
