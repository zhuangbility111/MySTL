//
// Created by HP on 2020/8/2.
//

#ifndef STL_MY_ALLOCATOR_MY_STL_ALGO_H
#define STL_MY_ALLOCATOR_MY_STL_ALGO_H

#include <algorithm>

// ---------------------------------------------------------------------
// set操作，这个set的定义是数学上的定义，而不是STL中的定义
// 也就是只要是有序序列就可以使用这几个set操作，但是一定得是有序的

// set_union，也就是取两个集合的并集
// 思路是两个序列进行比较，如果相等则只保存一个，如果不相等则先保存小的那个
// 如果某个值在s1中出现了n次，在s2出现了m次，那么该值在输出区间中会出现max(m, n)次
// 版本1，使用operator <
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        // 将小的那个放入result中
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        }
        else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
        }
            // 相等则同样是只放一个
        else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    // 有一个区间已经到结尾了，所以这个操作是将没走完的区间中剩下的元素拷贝到result中
    return copy(first2, last2, copy(first1, last1, result));
}

// 版本2，使用Compare对象
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare compare) {
    while (first1 != last1 && first2 != last2) {
        // 将小的那个放入result中
        if (compare(*first1, *first2)) {
            *result = *first1;
            ++first1;
        }
        else if (compare(*first2, *first1)) {
            *result = *first2;
            ++first2;
        }
            // 相等则同样是只放一个
        else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    // 有一个区间已经到结尾了，所以这个操作是将没走完的区间中剩下的元素拷贝到result中
    return copy(first2, last2, copy(first1, last1, result));
}

// set_intersection，取两个序列的并集
// 如果某个值在s1出现n次，在s2出现m次，那么该值在输出区间中会出现min(m, n)次
// 思路是两个序列比较，较小的往后移，相等则将当前元素放入result中，然后两者均往后移，直到某一个序列走到结尾为止
// 版本1，使用operator<
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        // 较小的往后移动
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
            // 相等则将当前元素放入result中
        else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }

    }
    return result;

}

// 版本2，使用Compare对象
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare compare) {
    while (first1 != last1 && first2 != last2) {
        // 较小的往后移动
        if (compare(*first1, *first2))
            ++first1;
        else if (compare(*first2, *first1))
            ++first2;
            // 相等则将当前元素放入result中
        else {
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }

    }
    return result;
}

// set_difference，取两个序列的差集，s1-s2，出现于s1，但不出现于s2
// 如果某个值在s1出现n次，在s2出现m次，那么该值在输出区间中会出现max(n-m, 0)次
// 思路是如果s1 == s2，均往后移； s1 < s2, 插入s1，s1往后移；s1 > s2, s2往后移
// 如果s2到达结尾，将s1剩下的插入；如果s1到达结尾，直接返回
// 版本1，使用operator<
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    while (first1 != last1 && first2 != last2) {

        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1)
            ++first2;
        else {
            ++first1;
            ++first2;
        }

    }
    return copy(first1, last1, result);

}

// 版本2，使用Compare对象
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare compare) {
    while (first1 != last1 && first2 != last2) {

        if (compare(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (compare(*first2, *first1))
            ++first2;
        else {
            ++first1;
            ++first2;
        }

    }
    return copy(first1, last1, result);

}


// set_symmetric_difference，求两个有序序列的对称差集，也就是(s1-s2) U (s2-s1)
// 如果某个值在s1出现n次，在s2出现m次，那么该值在输出区间中会出现 |n-m| 次
// 思路是如果s1 == s2，均往后移；如果s1 < s2，将s1插入，s1往后移；如果s1 > s2，将s2插入，s2往后移
// 如果有一个序列到达结尾，则直接将另一个序列剩下的元素插入
// 版本1，使用operator<
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
            ++result;
        }
        else {
            ++first1;
            ++first2;
        }

    }
    return copy(first1, last1, copy(first2, last2, result));

}

// 版本2，使用Compare对象
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare compare) {
    while (first1 != last1 && first2 != last2) {
        if (compare(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (compare(*first2, *first1)) {
            *result = *first2;
            ++first2;
            ++result;
        }
        else {
            ++first1;
            ++first2;
        }

    }
    return copy(first1, last1, copy(first2, last2, result));
}

// adjacent_find，找出第一组满足条件的相邻元素，版本1是指两元素相等
template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
    if (first == last)
        return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (*first == *next)
            return first;
        first = next;
    }
    return last;
}

// 版本2，满足二元运算符的相邻元素
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred) {
    if (first == last)
        return last;
    ForwardIterator next = first;
    while (++next != last) {
        if (binary_pred(first, next))
            return first;
        first = next;
    }
    return last;
}

// count，计算满足条件的元素个数
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
    count(InputIterator first, InputIterator last, const T& value) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for (; first != last; ++first) {
        if (*first == value)
            ++n;
    }
    return n;
}

// count_if，将指定操作（一个仿函数）pred实施于[first, last)区间内的每一个元素上
// 并将“造成pred的结算结果为true”的所有元素的个数返回
template <class InputIterator, class UnaryPredicate>
typename iterator_traits<InputIterator>::difference_type
    count_if(InputIterator first, InputIterator last, UnaryPredicate pred) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    for(; first != last; ++first) {
        if (pred(*first))
            ++n;
    }
    return n;
}

// find，找到第一个匹配“equality条件者”
template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
    while (first != last && *first != value)
        ++first;
    return first;
}

// find_if，找到满足仿函数pred的第一个元素
template <class InputIterator, class UnaryPredicate>
typename iterator_traits<InputIterator>::difference_type
    find_if(InputIterator first, InputIterator last, UnaryPredicate pred) {
    while (first != last && pred(*first))
        ++first;
    return first;
}

// find_end，在范围 [first, last) 中搜索序列 [s_first, s_last) 的最后一次出现
// 如果找不到则返回last
// 因为最后一次出现可以理解为逆向的第一次出现，但是如果使用逆向，则迭代器必须支持双向移动
// 所以需要分发函数，如果是单向迭代器，只能正向寻找；
// 如果是双向迭代器，则从后往前逆向寻找

// find_end底层函数，这个对应迭代器类型为单向迭代器的情况
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 __find_end(ForwardIterator1 first, ForwardIterator1 last,
                            ForwardIterator2 s_first, ForwardIterator2 s_last,
                            std::forward_iterator_tag, std::forward_iterator_tag) {
    if (s_first == s_last)
        return last;
    ForwardIterator1 result = last;
    while(1) {
        // search的作用是在[first, last)之间找到[s_first, s_last)的首次出现
        // 如果找不到，返回last
        ForwardIterator1 new_result = std::search(first, last, s_first, s_last);
        if (new_result == last)
            return result;
        else {
            result = new_result;
            first = new_result;
            // 将起始点往后移动一格，也就是在后面的序列中重新寻找目标区间
            ++first;
        }
    }

}

// find_end底层函数，这个对应迭代器类型为双向迭代器的情况
// 直接从后往前寻找目标区间第一次出现的位置，这就是结果
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1 __find_end(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                    BidirectionalIterator2 s_first, BidirectionalIterator2 s_last,
                                    std::bidirectional_iterator_tag, std::bidirectional_iterator_tag) {
    typedef reverse_iterator<BidirectionalIterator1> reviter1;
    typedef reverse_iterator<BidirectionalIterator2> reviter2;

    reviter1 rfirst1(last);
    reviter1 rlast1(first);
    reviter2 rfirst2(s_last);
    reviter2 rlast2(s_first);
    // 从后往前寻找目标区间第一次出现的位置
    reviter1 rresult = search(rfirst1, rlast1, rfirst2, rlast2);
    // 没找到，直接返回last
    if (rresult == rlast1)
        return last;
    // 找到了，转换为正向迭代器，然后返回
    else {
        // 转换为正向迭代器
        BidirectionalIterator1 result = rresult.base();
        // 因为此时result指向的是结尾，需要将其移动至头部，所以使用advance来移动
        advance(result, -distance(s_first, s_last));
        return result;
    }

}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 find_end(ForwardIterator1 first, ForwardIterator1 last,
                            ForwardIterator2 s_first, ForwardIterator2 s_last) {
    typedef typename iterator_traits<ForwardIterator1>::iterator_category iterator_category1;
    typedef typename iterator_traits<ForwardIterator2>::iterator_category iterator_category2;

    // 根据迭代器类型选择合适的函数，也就是负责分发函数
    return __find_end(first, last, s_first, s_last, iterator_category1(), iterator_category2());
}


// find_first_of，在范围 [first, last) 中搜索范围 [s_first, s_last) 中的任何元素
// 指向范围 [first, last) 中等于来自范围 [s_first, s_last) 中一个元素的首个元素。若找不到这种元素，则返回 last
// 例如s1 = "abcdf"， s2 = "ahg"，那么函数返回指向a的迭代器，因为s2中存在元素与s1[0]相等
// 版本1，使用operator==
template <class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first, InputIterator last,
                            ForwardIterator s_first, ForwardIterator s_last) {
    // 遍历[first, last)上的每一个元素，看当前元素是否与[s_first, s_last)中的某个元素相等，相等则返回当前元素的迭代器
    for (; first != last; ++first) {
        for (ForwardIterator iter = s_first; iter != s_last; ++iter) {
            if (*first == *iter)
                return first;
        }
    }
    return last;
}

// 版本2，使用二元仿函数
template <class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first, InputIterator last,
                            ForwardIterator s_first, ForwardIterator s_last, BinaryPredicate pred) {
    // 遍历[first, last)上的每一个元素，看当前元素是否与[s_first, s_last)中的某个元素相等，相等则返回当前元素的迭代器
    for (; first != last; ++first) {
        for (ForwardIterator iter = s_first; iter != s_last; ++iter) {
            if (pred(*first, *iter))
                return first;
        }
    }
    return last;
}


// for_each，将仿函数f作用于[first, last) 区间内的每一个元素
// f不能改变元素内容，因为first 和 last 都是 InputIterator。如果改变需要使用函数transform()
template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
    for (; first != last; ++first)
        f(*first);
    return f;
}

// generate，将仿函数gen的运算结果填写在[first, last)区间内的所有元素上，调用元素的operator=
template <class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen) {
    for (; first != last; ++first)
        *first = gen();
}

// generate_n，将仿函数gen的运算结果填写在从first开始的n个元素上，调用元素的operator=
template <class OutputIterator, class Size, class Generator>
OutputIterator generate_n(OutputIterator first, Size n, Generator gen) {
    for(; n > 0; --n, ++first) {
        *first = gen();
    }
    return first;
}

// includes，只能应用于有序区间，判断区间二的每个元素值是否都存在于区间一，也就是区间二是否是区间一的子串
// 通过operator<来判断是否相等，而不使用operator==
// 也就是限定只能使用一种仿函数的操作（less（小于），greater（大于））来判断是否相等
// 如果s1 < s2, s2 < s1，那么就可以判断s1 == s2
template <class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2) {
    if (first2 == last2)
        return true;
    while (first1 != last1 && first2 != last2) {
        // 如果f1 < f2，说明f1后面的元素有可能等于f2，则移动f1
        if (*first1 < *first2)
            ++first1;
            // 如果f1 > f2，说明f1前面的元素和后面的元素都不可能等于f2，也就是return false
        else if (*first2 < *first1)
            return false;
            // 如果f1 == f2，那么f1, f2均往后移动，判断下一个元素
        else {
            ++first1;
            ++first2;
        }
    }
    // 只有f2移动到最后，才说明s2的所有元素已经访问过了，也就是说明s2是s1的子串
    // 因为只有f1 == f2，f2才会往后移动
    return (first2 == last2);
}

// 版本2，使用给定的二元运算仿函数comp，注意comp必须是做比较的操作，不然函数内部会出错
template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1,
              InputIterator2 first2, InputIterator2 last2, Compare comp) {
    if (first2 == last2)
        return true;
    while (first1 != last1 && first2 != last2) {
        // 如果f1 < f2，说明f1后面的元素有可能等于f2，则移动f1
        if (comp(*first1, *first2))
            ++first1;
            // 如果f1 > f2，说明f1前面的元素和后面的元素都不可能等于f2，也就是return false
        else if (comp(*first2, *first1))
            return false;
            // 如果f1 == f2，那么f1, f2均往后移动，判断下一个元素
        else {
            ++first1;
            ++first2;
        }
    }
    // 只有f2移动到最后，才说明s2的所有元素已经访问过了，也就是说明s2是s1的子串
    // 因为只有f1 == f2，f2才会往后移动
    return (first2 == last2);
}


// max_element，返回一个迭代器指向序列中数值最大的元素
// 版本1，使用operator<
template <class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
    if (first == last)
        return first;
    ForwardIterator max_iter = first;
    ++first;
    for (; first != last; ++first) {
        if (*max_iter < *first)
            max_iter = first;
    }
    return max_iter;
}

// 版本2，使用仿函数comp
template <class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last)
        return first;
    ForwardIterator max_iter = first;
    ++first;
    for (; first != last; ++first) {
        if (comp(*max_iter, *first))
            max_iter = first;
    }
    return max_iter;
}

// min_element，返回序列中数值最小的元素
// 版本1，使用operator<
template <class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
    if (first == last)
        return first;
    ForwardIterator min_iter = first;
    while (++first != last) {
        if (*first < *min_iter)
            min_iter = first;
    }
    return min_iter;
}

// 版本2，使用比较仿函数
template <class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last)
        return first;
    ForwardIterator min_iter = first;
    while (++first != last) {
        if (comp(*first, *min_iter))
            min_iter = first;
    }
    return min_iter;
}

// merge，将两个有序序列进行合并，返回结果序列最后一个元素的下一个位置
// 版本1，operator<
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result)  {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        }
        else {
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    // 将两个序列中剩余的元素拷贝到result中
    return copy(first1, last1, copy(first2, last2, result));
}

// 版本2，比较仿函数
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2,
                     OutputIterator result, Compare comp)  {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        }
        else {
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    // 将两个序列中剩余的元素拷贝到result中
    return copy(first1, last1, copy(first2, last2, result));
}

// partition，将序列重新排列，所有被一元条件运算pred判定为true的元素会被放在序列前端
// false放在后端
// 迭代器必须是支持双向移动的迭代器BidirectionalIterator
template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last,
                                Predicate pred) {
    // 思路是类似于快排的partition，从左边找false的元素，从右边找true的元素，然后交换
    --last;
    while (first != last) {
        // 从左边找到false的元素
        while (first != last && pred(*first))
            ++first;
        // 从右边找到true的元素
        while (first != last && !pred(*last))
            --last;
        if (first != last) {
            std::swap(*first, *last);
            ++first;
        }
    }
    return first;
}

// remove，移除区间内所有与value相等的元素
// 其实就是将区间内与value不相等的元素拷贝到区间的前面
template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
    // 慢指针，记录下一个可复制的位置
    ForwardIterator pre = first;
    // 快指针，判断元素是否需要移动到前面（是否与value不相等）
    ForwardIterator cur = first;
    for(; cur != last; ++cur) {
        if (*cur != value) {
            *pre = *cur;
            ++pre;
        }
    }
    return pre;
}


// remove_copy，移除区间内所有与value相等的元素
// 其实就是将区间内与value不相等的元素拷贝到新的区间中
template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator result, const T& value) {
    while (first != last) {
        if (*first != value) {
            *result = *first;
            ++result;
        }
        ++first;
    }
    return result;
}

// remove_if，与remove相似，只不过判定条件从equality变成了二元仿函数
template <class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate pred) {
    // 慢指针，记录下一个可复制的位置
    ForwardIterator pre = first;
    // 快指针，判断元素是否需要移动到前面（是否与value不相等）
    ForwardIterator cur = first;
    for(; cur != last; ++cur) {
        if (!pred(*cur)) {
            *pre = *cur;
            ++pre;
        }
    }
    return pre;
}

// remove_copy_if，与remove_copy相似，只不过判定条件从equality变成了二元仿函数
template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred) {
    while (first != last) {
        if (!pred(*first)) {
            *result = *first;
            ++result;
        }
        ++first;
    }
    return result;
}

// replace，旧值换成新值
template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value) {
    for (; first != last; ++first) {
        if (*first == old_value)
            *first = new_value;
    }
}

// replace_copy，与replace相似，只不过拷贝到新的地方
template <class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last, OutputIterator result,
                            const T& old_value, const T& new_value) {
    for(; first != last; ++first, ++result) {
        if (*first == old_value)
            *result = new_value;
        else
            *result = *first;
    }
    return result;
}

// replace_if，与replace相似，equality换成二元运算符
template <class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value, Predicate pred) {
    for (; first != last; ++first) {
        if (pred(old_value))
            *first = new_value;
    }
}

// replace_copy_if，与replace_if类似，只不过拷贝到新的地方
template <class InputIterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy_if(InputIterator first, InputIterator last, OutputIterator result,
                            Predicate pred, const T& new_value) {
    for(; first != last; ++first, ++result) {
        if (pred(*first))
            *result = new_value;
        else
            *result = *first;
    }
    return result;
}

// reverse，将序列反转
// 不同迭代器类型会影响性能，双向迭代器或者随机访问迭代器
// 因此需要设计两层函数，通过tag来区分不同的迭代器类型

// 底层函数，双向迭代器的版本，只能使用自增和自减
template <class BidirectionalIterator>
void __reverse(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag) {
    --last;
    while (first != last) {
        swap(*first, *last);
        if (++first == last)
            return;
        --last;
    }
}

// 底层函数，随机访问迭代器的版本
template <class RandomAccessIterator>
void __reverse(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    // 修正last
    --last;
    // 随机访问迭代器的好处就是可以直接比较大小
    while (first < last) {
        swap(*first, *last);
        ++first;
        --last;
    }
}

// reverse对外接口
template <class BidirectionalIterator>
void reverse(BidirectionalIterator first, BidirectionalIterator last) {
    // 萃取出迭代器的类型
    typedef typename iterator_traits<BidirectionalIterator>::iterator_category iterator_category;
    __reverse(first, last, iterator_category());
}

// reverse_copy，类似于reverse，但是结果存在result中
template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result) {
    // 修正last
    --last;
    for (; first != last; --last) {
        *result = *last;
        ++result;
    }
    return result;
}

// rotate，其实就是将序列循环左移，直至middle指向的元素移动到序列的最前面
// 具体实现与迭代器的移动能力有关，因此设置双层函数

// ForwardIterator版本，单项迭代器，只能往后移动来进行交换
template <class ForwardIterator>
void __rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last, forward_iterator_tag) {
    // middle将序列分成前半部分[first, middle)，和后半部分[middle, last)
    // 其实这个方法就是不断地交换前半部分和后半部分的元素
    // 当前半部分和后半部分一样长时，说明刚好旋转成功，直接退出
    // 如果前半部分比后半部分长，也就是后半部分的指针先到达last，说明前半部分还剩有元素需要移动到后半部分
    // 并且前半部分的剩余元素应该是放在已经移动到后半部分的元素的后面，所以将后半部分的指针重新移动到原来middle的地方，继续交换；
    // 如果后半部分比前半部分长，也就是前半部分的指针先到达middle，说明后半部分还剩有元素需要移动到前半部分
    // 并且后半部分的剩余元素应该是放在已经移动到后半部分的元素的前面，所以后半部分的指针不需要移动，更新后半部分的起点，直接从当前位置继续交换即可
    for (ForwardIterator i = middle; ;) {
        swap(*first, *i);
        ++first;
        ++i;
        if (first == middle) {
            // 说明前半部分与后半部分一样长，旋转结束
            if (i == last)
                return;
            // 后半部分比前半部分长，那么更新后半部分的起点middle为后半部分剩余的第一个元素
            middle = i;
        }
        // 前半部分比后半部分长，那么不需要更新后半部分的起点middle，将后半部分的指针重新移动到middle处
        else if (i == last) {
            i = middle;
        }
    }
}

// BidirectionalIterator版本，双向迭代器可以直接从起始处和结尾处开始，交换元素，直接使用reverse
// 前半部分使用reverse，后半部分使用reverse，最后总的序列使用reverse，就是结果
template <class BidirectionalIterator>
void __rotate(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, bidirectional_iterator_tag) {
    reverse(first, middle);
    reverse(middle, last);
    reverse(first, last);
}

// RandomAccessIterator版本，这个过程比较复杂，可以查看博客（不过我暂时还没看懂，直接拷贝代码了）

// 利用辗转相除法求最大公因子
template <class EuclideanRingElement>
EuclideanRingElement __gcd(EuclideanRingElement m, EuclideanRingElement n) {
    while (n != 0) {
        EuclideanRingElement t = m % n;
        m = n;
        n = t;
    }
    return m;
}

// 对属于同一组的元素做左移操作（同一组指的是之间相隔距离为shift的元素）
template <class RandomAccessIterator, class Distance, class T>
void __rotate_cycle(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator initial,
                        Distance shift, T*) {
    T value = *initial;
    RandomAccessIterator ptr1 = initial;
    RandomAccessIterator ptr2 = ptr1 + shift;
    while (ptr2 != initial) {
        *ptr1 = *ptr2;
        ptr1 = ptr2;
        if (last - ptr2 > shift)
            ptr2 += shift;
        else
            ptr2 = first + (shift - (last - ptr2));
    }
    *ptr1 = value;
}

template <class RandomAccessIterator>
void __rotate(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    // 求最大公因子
    Distance n = __gcd(last - first, middle - last);
    while (n--)
        __rotate_cycle(first, last, first + n, middle - first, value_type(first));
}

// 最外层接口
template <class ForwardIterator>
void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
    if (first == middle || middle == last)
        return;
    typedef typename iterator_traits<ForwardIterator>::iterator_category iterator_category;
    __rotate(first, middle, last, iterator_category());
}

// rotate_copy，这个比较简单，直接按照位置拷贝过去即可
// 先将后半部分元素拷贝到新序列的前半部分，将前半部分元素拷贝到新序列的后半部分
template <class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result) {
    return copy(first, middle, copy(middle, last, result));
}


// search，在序列一中查找序列二第一次出现的位置，返回第一次出现的位置
// 版本1，使用equality
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                        ForwardIterator2 first2, ForwardIterator2 last2) {
    typedef typename iterator_traits<ForwardIterator1>::difference_type Distance1;
    typedef typename iterator_traits<ForwardIterator2>::difference_type Distance2;
    Distance1 len1 = distance(first1, last1);
    Distance2 len2 = distance(first2, last2);
    ForwardIterator1 cur1 = first1;
    ForwardIterator2 cur2 = first2;
    // 如果序列一到达结尾，或者序列一剩下的长度小于序列二，找不到，返回last1
    while (cur1 != last1 && len1 >= len2) {
        // 如果序列二没有到结尾，并且序列一当前元素等于序列二当前元素
        // 两个序列的当前元素都往后移动
        while (cur2 != last2 && *cur1 == *cur2) {
            ++cur1;
            ++cur2;
        }
        // 如果序列二到结尾了，说明找到了，返回first1
        if (cur2 == last2)
            return first1;
        // 否则，没找到，将first1往后移动1位，并更新cur1
        // 序列一的剩余长度减一
        // 更新cur2，下一轮从头开始比
        else {
            --len1;
            cur1 = ++first1;
            cur2 = first2;
        }
    }
    return last1;

}

// 版本2，使用二元仿函数
// search，在序列一中查找序列二第一次出现的位置，返回第一次出现的位置
template <class ForwardIterator1, class ForwardIterator2, class Predicate>
ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                        ForwardIterator2 first2, ForwardIterator2 last2, Predicate pred) {
    typedef typename iterator_traits<ForwardIterator1>::difference_type Distance1;
    typedef typename iterator_traits<ForwardIterator2>::difference_type Distance2;
    Distance1 len1 = distance(first1, last1);
    Distance2 len2 = distance(first2, last2);
    ForwardIterator1 cur1 = first1;
    ForwardIterator2 cur2 = first2;
    // 如果序列一到达结尾，或者序列一剩下的长度小于序列二，找不到，返回last1
    while (cur1 != last1 && len1 >= len2) {
        // 如果序列二没有到结尾，并且序列一当前元素等于序列二当前元素
        // 两个序列的当前元素都往后移动
        while (cur2 != last2 && pred(*cur1, *cur2)) {
            ++cur1;
            ++cur2;
        }
        // 如果序列二到结尾了，说明找到了，返回first1
        if (cur2 == last2)
            return first1;
            // 否则，没找到，将first1往后移动1位，并更新cur1
            // 序列一的剩余长度减一
            // 更新cur2，下一轮从头开始比
        else {
            --len1;
            cur1 = ++first1;
            cur2 = first2;
        }
    }
    return last1;
}


// search_n，查找连续n个满足条件的元素，如果找到了，返回子序列起点；如果找不到，返回last
// 版本1，条件为相等
template <class ForwardIterator, class Integer, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Integer count, const T& value) {
    if (count <= 0)
        return first;
    first = find(first, last, value);
    while (first != last) {
        Integer n = count - 1;
        ForwardIterator i = first;
        // 循环判断子序列
        while (n != 0 && i != last && *i == value) {
            ++i;
            --n;
        }
        // n == 0，说明已经找到
        if (n == 0)
            return first;
        // 如果i == last，说明已经不可能找到了，直接返回last
        else if (i == last)
            return last;
        // 否则从不满足的元素开始重新找第一个满足条件的元素
        else
            first = find(i, last, value);
    }
    return last;
}

// 版本2，使用二元仿函数作为条件判断
template <class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Integer count, const T& value, BinaryPredicate pred) {
    if (count <= 0)
        return first;

    // 寻找第一个满足的元素
    while (first != last) {
        if (pred(*first, value))
            break;
        ++first;
    }

    while (first != last) {
        // 循环判断子序列是否满足
        Integer n = count - 1;
        ForwardIterator i = first;
        while (n > 0 && i != last && pred(*i, value)) {
            ++i;
            --n;
        }
        if (n == 0)
            return first;
        else if(i == last)
            return last;
        else if (!pred(*i, value)) {
            // 寻找第一个满足的元素
            while (i != last) {
                if (pred(*i, value))
                    break;
                ++i;
            }
            first = i;
        }

    }
    return last;


}


// transform，将仿函数作用于[first, last)中的每一个元素，并以其结果产生出要给新序列
// 版本1，一元仿函数，只作用于一个序列
template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op) {
    for(; first != last; ++first, ++result) {
        *result = op(*first);
    }
    return result;
}

// 版本2，二元仿函数，作用于两个序列
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
                            InputIterator2 first2, InputIterator2 last2,
                            OutputIterator result, BinaryOperation binary_op) {
    for(; first1 != last1 && first2 != last2; ++first1, ++first2, ++result)
        *result = binary_op(*first1, *first2);
    return result;
}

// swap_ranges，将[first1, last1)区间内的元素与“从 first2 开始、个数相同”的元素互换
// 如果两序列重叠，或者长度不同，结果不可预期
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator2 last1,
                                ForwardIterator2 first2) {
    while (first1 != last1) {
        swap(*first1, *first2);
        ++first1;
        ++first2;
    }
    return first2;
}

// unique，remove重复的元素，每当在 [first, last)内遇有重复元素群，他便移除改元素群中第一个以后的所有元素
// 注意，只移除相邻的重复元素。如果想要移除不相邻的重复元素，需要先排序
// remove与erase不同，erase会直接删除元素（回收内存空间），而remove只是用后面的元素覆盖前面需要删除的元素
// 所以remove之后序列的长度不变，而erase之后序列的长度会变化
// unique返回一个迭代器指向新区间的尾端，新区间之内不含相邻的重复元素
// 版本1，使用默认的equality

// 先实现unique_copy函数，将[first, last)区间内的元素拷贝到result中
// 相邻的相等元素群，则只拷贝一个
// 底层实现函数根据result迭代器的不同类型分为几种
// 一种是result迭代器为ForwardIterator，因此可以将结果序列的最后一个元素与原序列的当前元素进行比较（当前元素比最后一个元素快一步）
// 以实现相邻元素的比较
template <class InputIterator, class ForwardIterator>
ForwardIterator __unique_copy(InputIterator first, InputIterator last,
                                ForwardIterator result, forward_iterator_tag) {
    *result = *first;
    // first比result快，两者实际上是原序列上的相邻元素
    while (*++first != last) {
        // 不重复，拷贝到result
        if (*first != *result) {
            *result = *first;
            ++result;
        }
    }
    return ++result;
}

// result迭代器为OutputIterator
// 不能从result中读取前一个不重复的元素，因为OutputIterator是只写的
// 那就只能使用一个额外的变量保存前一个不重复的元素，遇到重复元素则值不变
// 只有遇到下一个不重复的元素，才更新这个变量
template <class InputIterator, class OutputIterator>
OutputIterator __unique_copy(InputIterator first, InputIterator last,
                                OutputIterator result, output_iterator_tag) {
    typedef typename iterator_traits<InputIterator>::value_type T;
    T value = *first;
    *result = value;
    while (*++first != last) {
        if (value != *first) {
            value = *first;
            *++result = value;
        }
    }
    return ++result;
}

// unique_copy对外接口，同时也是分发函数

template <class InputIterator, class OutputIterator>
OutputIterator unique_copy(InputIterator first, InputIterator last,
                           OutputIterator result) {
    // 根据不同的迭代器类型调用不同的底层函数
    typedef typename iterator_traits<InputIterator>::iterator_categoty iterator_category;
    return __unique_copy(first, last, result, iterator_category());
}

// unique函数，先找到第一个相邻元素相等的位置，再调用unique_copy
template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
    first = adjacent_find(first, last);
    return unique_copy(first, last, first);
}


// 底层实现函数
template <class ForwardIterator, class T>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, const T& value, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle >= value) {
            len = half;
        }
        else {
            len = len - half - 1;
            first = middle;
            ++first;
        }
    }
    return first;
}

template <class RandomAccessIterator, class T>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (*middle < value) {
            len = len - half - 1;
            first = middle + 1;
        }
        else {
            len = half;
        }
    }
    return first;
}

template <class ForwardIterator, class T>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    if (first == last)
        return last;
    typedef typename iterator_traits<ForwardIterator>::iterator_category iterator_category;
    return __lower_bound(first, last, value, iterator_category());
}


// ForwardIterator的版本
template <class ForwardIterator, class T>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last, const T& value, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
    Distance len = 0;
    distance(first, last, len);
    Distance half;
    ForwardIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle > value) {
            len = half;
        }
        else {
            first = middle;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}

// RandomAccessIterator的版本
template <class RandomAccessIterator, class T>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (*middle > value) {
            len = half;
        }
        else {
            len = len - half - 1;
            first = middle + 1;
        }
    }
    return first;

}

template <class ForwardIterator, class T>
ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    if (first == last)
        return last;
    typedef typename iterator_traits<ForwardIterator>::iterator_category iterator_category;
    return __upper_bound(first, last, value, iterator_category());
}



#endif //STL_MY_ALLOCATOR_MY_STL_ALGO_H
