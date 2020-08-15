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




// lower_bound，应用于有序区间
// 返回序列中第一个不小于目标元素的元素的迭代器i，也就是说[first, i)的元素均小于目标元素
// i指向的元素可能是与目标元素相等，或者大于
// 两个版本，operator< 和 二元仿函数compare


// 因为使用二分查找，所以不同类型的迭代器对于二分查找的过程会有影响
// 所以需要分成两种底层函数
// 底层函数1，ForwardIterator的版本，每次只能往后移动一个单位，并且不能比较迭代器的大小
template <class ForwardIterator, class T>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, const T& value, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
    // 使用len来表示正在判断的区间大小
    Distance len = 0;
    // 求两个迭代器之间的距离
    distance(first, last, len);
    Distance half;
    // 指向中间元素
    ForwardIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first;

        // 移动指向中间元素
        advance(middle, half);
        // 取右半边，因为中间元素小于value，所以这个中间元素不可能是结果，丢弃
        if (*middle < value) {
            len = len - half - 1;
            first = middle;
            ++first;
        }
        // 如果中间元素大于等于value，则这个中间元素有可能是结果
        // 取左半边，并且这个中间元素需要包括进来
        else {
            len = half;
        }
    }
    return first;
}

// 底层函数1，ForwardIterator的版本，每次只能往后移动一个单位，并且不能比较迭代器的大小
template <class ForwardIterator, class T, class Compare>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
    // 使用len来表示正在判断的区间大小
    Distance len = 0;
    // 求两个迭代器之间的距离
    distance(first, last, len);
    Distance half;
    // 指向中间元素
    ForwardIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first;
        // 移动指向中间元素
        advance(middle, half);
        // 取右半边，因为中间元素小于value，所以这个中间元素不可能是结果，丢弃
        if (comp(*middle, value)) {
            len = len - half - 1;
            first = middle;
            ++first;
        }
        // 如果中间元素大于等于value，则这个中间元素有可能是结果
        // 取左半边，并且这个中间元素需要包括进来
        else {
            len = half;
        }
    }
    return first;
}


// 底层函数2，RandomAccessIterator的版本
template <class RandomAccessIterator, class T>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    // 使用len来表示正在判断的区间大小
    Distance len = last - first;
    Distance half;
    // 指向中间元素
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        // 取右半边，因为中间元素小于value，所以这个中间元素不可能是结果，丢弃
        if (*middle < value) {
            len = len - half - 1;
            first = middle + 1;
        }
        // 如果中间元素大于等于value，则这个中间元素有可能是结果
        // 取左半边，并且这个中间元素需要包括进来
        else {
            len = half;
        }
    }
    return first;
}

// 底层函数2，RandomAccessIterator的版本
template <class RandomAccessIterator, class T, class Compare>
RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare comp, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    // 使用len来表示正在判断的区间大小
    Distance len = last - first;
    Distance half;
    // 指向中间元素
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        // 取右半边，因为中间元素小于value，所以这个中间元素不可能是结果，丢弃
        if (comp(*middle, value)) {
            len = len - half - 1;
            first = middle + 1;
        }
            // 如果中间元素大于等于value，则这个中间元素有可能是结果
            // 取左半边，并且这个中间元素需要包括进来
        else {
            len = half;
        }
    }
    return first;
}


// 版本1，使用operator<
template <class ForwardIterator, class T>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    // 不同迭代器类型，实现方法不一样
    typedef typename iterator_traits<ForwardIterator>::iterator_category category;
    return __lower_bound(first, last, value, category());
}

// 版本2，使用仿函数
template <class ForwardIterator, class T, class Compare>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    // 不同迭代器类型，实现方法不一样
    typedef typename iterator_traits<ForwardIterator>::iterator_category category;
    return __lower_bound(first, last, value, comp, category());
}


// upper_bound函数，用于有序序列，返回“在不破坏顺序的情况下，可插入value的最后一个合适的位置”
// 也就是返回一个迭代器i，使得[first, i)均小于等于value，而i必然大于value

// 迭代器类型不同会影响底层实现，这是ForwardIterator的版本
template<class ForwardIterator, class T>
ForwardIterator __upper_bound(ForwardIterator first, ForwardIterator last, const T& value, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
    // 使用len保存当前正在判断的区间的长度
    Distance len = 0;
    distance(first, last, len);
    // 使用half保存半个区间的长度
    Distance half = 0;
    // 指向区间的中间迭代器
    ForwardIterator middle;
    // 只要判断区间的长度(last - first)不为0，也就是还没有确定某个元素，则一直二分查找
    while (len > 0) {
        half = len >> 1;
        middle = first;
        // 移动middle到中间位置
        advance(middle, half);
        // 如果中间位置的元素大于value，则取左半边，并且中间元素有可能作为结果，因此需要包含中间元素
        if (value < *middle) {
            len = half;
        }
        // 如果中间位置的元素小于等于value，则取右半边，并且中间元素不可能作为结果，将中间元素丢弃
        else {
            len = len - half - 1;
            first = middle;
            ++first;
        }
    }
    return first;
}


// RandomAccessIterator版本
template<class RandomAccessIterator, class T>
RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last, const T& value, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    // 使用len保存当前正在判断的区间的长度
    Distance len = last - first;
    // 使用half保存半个区间的长度
    Distance half = 0;
    // 指向区间的中间迭代器
    RandomAccessIterator middle;
    // 只要判断区间的长度(last - first)不为0，也就是还没有确定某个元素，则一直二分查找
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        // 如果中间位置的元素大于value，则取左半边，并且中间元素有可能作为结果，因此需要包含中间元素
        if (value < *middle) {
            len = half;
        }
            // 如果中间位置的元素小于等于value，则取右半边，并且中间元素不可能作为结果，将中间元素丢弃
        else {
            len = len - half - 1;
            first = middle + 1;
        }
    }
    return first;

}

// upper_bound对外接口
template <class ForwardIterator, class T>
ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    // 不同迭代器类型，实现方法不一样
    typedef typename iterator_traits<ForwardIterator>::iterator_category category;
    return __upper_bound(first, last, value, category());
}

// binary_search，二分查找某个元素是否存在
// 方法是通过lower_bound来实现，lower_bound会返回第一个不小于value的元素，此时直接判断这个元素是否与value相等
// 就可以判断是否找得到value
// 版本1，使用operator<
template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
    ForwardIterator res = lower_bound(first, last, value);
    // 没有到last，并且res不大于value，则res必然等于value
    return (res != last && !(value < *res));
}

// 版本2，使用comp仿函数
template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    ForwardIterator res = lower_bound(first, last, value, comp);
    // 没有到last，并且res不大于value，则res必然等于value
    return (res != last && !comp(value, *res));
}

// next_permutation，找到当前序列的下一个排列组合，使得下一个排列组合比前面的所有组合都大（字典序）
// 初始状态是一个有序序列，前一个元素必然比后一个元素大
// 下一个排列组合应该是尽可能后面的元素发生变化，而前面的元素尽量不发生变化
// 就像abcde，下一个排列组合是abced，再下一个排列组合是abecd
// 这样才能满足下一个排列组合比前面的所有组合都大（字典序）
// 因此从序列的后面开始寻找，找到一对相邻的元素i和j，使得前面的i小于j
// 此时他们俩后面的元素都是逆序，说明后面的元素组成的子序列已经是这个子序列的最后一个排列组合（逆序一定是序列的最后一个排列组合）
// 那么只能从前面的元素i做文章
// 从结尾往前找到一个最接近于i，并且大于i的元素，两者互换位置（互换后j后面的子序列依然是逆序）
// 因为i已经换成了下一个大于它的元素，相当于以i为起点开启一个新的序列
// 所以i后面的这个子序列从逆序重新变成顺序（顺序一定是序列的第一个排列组合）
// 然后又重复上面的过程，直到i成为第一个元素
template <class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    if (first == last)
        return false;
    BidirectionalIterator pre = first;
    ++pre;
    if (pre == last)
        return false;
    pre = last;
    --pre;
    BidirectionalIterator rear = pre;
    --pre;
    // 找到pre小于rear的位置
    while (rear != first && !(*pre < *rear)) {
        rear = pre;
        --pre;
    }
    // 整个序列都是逆序，说明已经是最后一个排列组合，没有下一个了，直接返回false
    if (rear == first)
        return false;
    // 从后往前寻找第一个大于pre的元素，也就是pre后面的元素中最接近于pre，并且大于pre的元素
    BidirectionalIterator j = last;
    --j;
    while (!(*j < *pre))
        --j;
    // 交换两者的值
    swap(*pre, *j);
    // 对pre后面的序列进行反转，也就是让它从逆序变为顺序，此时成为pre变成新元素之后的第一个排列组合
    reverse(rear, last);
    return true;
}

// prev_permutation，寻找当前序列的前一个排列组合，与上面那个函数相对应
// 实际上实现方法也与上面的相似
// 从后往前找到相邻元素i和j，使得i < j
// 因为顺序是第一个排列组合，所以遇到顺序序列时，需要对第一个不满足顺序的元素，也就是上面说的i
// 对其进行降级（也就是找个比它小的元素替换），然后将它后面的序列反转为逆序
// 就是当前序列的前一个排列组合
template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    if (first == last)
        return false;
    BidirectionalIterator pre = first;
    ++pre;
    if (pre == last)
        return false;
    pre = last;
    --pre;
    // 从后往前找到相邻元素i和j，使得i < j，也就是找到第一个破坏后面序列逆序的元素
    // 将其替换为比它小的元素，然后将它后面的序列反转为顺序，就是前一个排列组合
    BidirectionalIterator rear = pre;
    --pre;
    while (rear != first && !(*rear < *pre)) {
        --pre;
        --rear;
    }
    if (rear == first)
        return false;
    BidirectionalIterator j = last;
    --last;
    while (!(*j < *pre))
        --j;
    swap(*j, *pre);
    reverse(rear, last);
    return true;
}

// random_shuffle()，将序列随机重排

// 版本1，使用内部随机数生成器
template <class RandomAccessIterator, class Distance>
void __random_shuffle(RandomAccessIterator first, RandomAccessIterator last, Distance*) {
    if (first == last)
        return;
    for (RandomAccessIterator i = first + 1; i != last; ++i) {
#ifdef __STL_NO_DRAND48
        swap(*i, *(first + Distance(rand() % ((i - first) + 1))));
#else
        swap(*i, *(first + Distance(lrand48() % ((i - first) + 1))));
#endif
    }
}

// 版本1
template <class RandomAccessIterator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last) {
    __random_shuffle(first, last, distance_type(first));
}


// 版本2，使用一个会产生随机数的仿函数
template <class RandomAccessIterator, class RandomNumberGenerator>
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator& rand) {
    if (first == last)
        return;
    for (RandomAccessIterator i = first + 1; i != last; ++i)
        swap(*i, *(first + rand((i - first) + 1)));
}

// partial_sort，部分排序，也就是接受一个middle迭代器，重新安排[first, last)，使序列中的middle - first个最小元素以递增顺序排序
// 置于[first, middle)内，其余last - middle个元素安置于[middle, last)中，不保证有任何特定顺序
// 思路是将前面[first, middle)的元素维护成一个大根堆，然后不断遍历后半部分的元素
// 如果遇到比大根堆的堆顶（也就是前半部分的最大值）小的元素，则将大根堆的堆顶出堆，插入这个小的元素
// 这个大根堆就一直维护着[first, cur)中前 middle - first个小的元素
// 最后将这个大根堆一个一个出堆，放入目标位置中，就是一个有序的序列

// 版本1，使用 operator<
template <class RandomAccessIterator, class T>
void __partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T*) {
    make_heap(first, middle);
    for (RandomAccessIterator cur = middle; cur < last; ++cur) {
        // 当前元素与堆顶比较
        if (*cur < *first)
            // 将堆顶弹出（移动至middle的位置）
            // 然后将middle的值移动至堆顶
            // 接着调整堆，做下沉操作
            __pop_heap(first, middle, cur, T(*cur), distance_type(first));
    }
    // 最后逐步将堆顶弹出，形成一个有序的序列
    sort_heap(first, middle);
}

template <class RandomAccessIterator>
void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
    __partial_sort(first, middle, last, value_type(first));
}

// partial_sort_copy，在partial_sort的基础上，将(first - first)个最小元素拷贝到另一个序列中
template <class InputIterator, class RandomAccessIterator>
void partial_sort_copy(InputIterator first, InputIterator last,
                        RandomAccessIterator result_first, RandomAccessIterator result_last) {
    if (first == last || result_first == result_last)
        return;
    copy(first, last, result_first);
    partial_sort(first, last, last);
}


// 重头戏之sort函数

// __unguarded_linear_insert函数，插入排序和__linear_insert的辅助函数
// 主要负责从有序区间的结尾往前寻找，在有序区间中找到合适的插入位置来插入当前元素
// 无边界检查的版本
template <class RandomAccessIterator, class T>
void __unguarded_linear_insert(RandomAccessIterator last, T value) {
    RandomAccessIterator next = last - 1;
    while (value < *next) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

// __linear_insert函数，插入排序的辅助函数，负责将last元素插入到[first, last)这个有序区间中
// 有边界检查的版本
template <class RandomAccessIterator, class T>
void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*) {
    T value = *last;
    // 如果小于左边有序区间的第一个元素（最小的元素）
    // 则不需要比较，直接将其放到有序区间的第一个元素中
    // 这样在后面从后往前寻找合适的插入位置时就不再需要进行边界检查，提高寻找的效率
    if (value < *first) {
        // [first, last)按照从后往前的顺序拷贝到[first+1, last+1)
        copy_backward(first, last, last + 1);
        *first = value;
    }
    // 因为前面已经保证了value的插入位置不是在首元素
    // 所以后续寻找不需要边界检查，提高寻找效率
    // 否则，从后往前寻找合适的插入位置
    else {
        __unguarded_linear_insert(last, value);
    }
}


// 有边界检查的插入排序
template <class RandomAccessIterator>
void __insert_sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first == last)
        return;
    // 每次插入一个元素进入左边的有序区间，使用辅助函数__linear_insert实现每次插入的过程
    for (RandomAccessIterator i = first + 1; i != last; ++i) {
        __linear_insert(first, i, value_type(first));
    }
}

// 快排中的分割操作，根据标兵pivot将序列切分成左右两部分，左半部分小于等于pivot，右半部分大于pivot
// 返回右半部分的第一个位置
// 没有边界检查，因为在选取pivot时已经保证原序列中必然存在小于pivot的元素和大于pivot的元素
template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, T pivot) {
    if (first == last)
        return last;
    --last;
    while (true) {
        // 在左半边中寻找大于pivot的元素
        while (*first < pivot)
            ++first;
        // 在右半边中寻找小于等于pivot的元素
        while (pivot < *last)
            --last;
        if (first < last)
            swap(*first, *last);
        else
            return first;
        --last;
        ++first;
    }
}

// 使用三点中值的方法来选取pivot
// 具体方法是选取整个序列的头、尾、中央三个位置的元素，以他们三个中中间大小的作为pivot
// 防止在取pivot时取到极端大或者极端小的元素
// 除了防止取pivot取到极端值，提高快排的效率之外，还可以保证在partition的时候，
// 左右两边都有一个兜底的边界值，因此partition不需要判断是否到了边界
// 每次partition时都可以省略很多次边界检查，提高了partition的效率
template <class T>
const T& __median(const T& a, const T& b, const T& c) {
    if (a < b) {
        if (b < c)
            return b;
        else if (c < a)
            return a;
        else
            return c;
    }
    else if (b < a) {
        if (a < c)
            return a;
        else if (c < b)
            return b;
        else
            return c;
    }
}

// 控制递归的深度，防止分割恶化导致递归过深
// 找出 2^k <= n 的最大值k
template <class Size>
inline Size __lg(Size n) {
    Size k;
    for (k = 0; n > 1; n >>= 1)
        ++k;
    return k;
}

// 无边界检查版本的插入排序的底层函数
template <class RandomAccessIterator, class T>
void __unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
    // 遍历所有元素，每个元素从后往前寻找合适的插入位置
    // 调用的是无边界检查的寻找
    for (RandomAccessIterator i = first; i < last; ++i) {
        __unguarded_linear_insert(i, T(*i));
    }
}

// 无边界检查版本的插入排序
template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    __unguarded_insertion_sort_aux(first, last, value_type(first));
}


const int __stl_threshold = 16;

// 最终版本的插入排序
template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
    // last - first > 16，也就是序列长度大于16
    // 则先对前16个元素做有边界检查的插入排序
    // 然后对后面的剩余元素做无边界检查的插入排序
    if (last - first > __stl_threshold) {
        __insert_sort(first, first + __stl_threshold);
        __unguarded_insertion_sort(first + __stl_threshold, last);
    }
    // last - first < 16
    // 则直接对其进行有边界检查的插入排序
    else {
        __insert_sort(first, last);
    }
}

// 快排的最终版本，当序列足够小时，返回，调用插入排序
template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*, Size depth_limit) {
    while (last - first > __stl_threshold) {
        // 超过递归调用的深度，此时考虑使用堆排序来代替
        if (depth_limit == 0) {
            partial_sort(first, last, last);
            return;
        }

        RandomAccessIterator cut = __unguarded_partition(first, last, __median(*first, *(first + (last - first) / 2), *(last - 1)));
        // 对右半部分进行递归切分
        __introsort_loop(cut, last, value_type(cut), --depth_limit);

        // 将last设为左半边的结尾，从而利用当前函数来进行左半部分的切分
        // 减少一次递归函数调用
        last = cut;
    }
}

// 最终的sort，对外接口
template <class RandomAccessIterator>
void sort(RandomAccessIterator first, RandomAccessIterator last) {
    if (first != last) {
        // 先快排，直到切分的序列足够小时
        __introsort_loop(first, last, value_type(first), __lg(last-first) * 2);
        // 使用插入排序
        // 因为插入排序在面对“几近排序”的序列时，有很好的表现
        __final_insertion_sort(first, last);
    }
}


// equal_range，应用于有序区间，返回等于value的一个连续区间的起始迭代器和结尾迭代器
// lower_bound刚好返回这个连续区间的起始迭代器，而upper_bound刚好返回这个连续区间的结尾迭代器
// 先使用二分查找找到中间元素等于value，然后对左半边调用lower_bound，对右半边调用upper_bound
// 这样做的原因是，本身lower_bound 和 upper_bound 都会执行二分查找，在还没有找到中间元素等于value时
// 两者会对相同的区间做二分查找，直到找到中间元素等于value，lower_bound 和 upper_bound 才会针对不同的区间做二分查找
// 因此先做二分查找，可以减去前期两者重复的二分查找
// 版本1，使用operator <
// RandomAccessIterator版本
template <class RandomAccessIterator, class T>
pair<RandomAccessIterator, RandomAccessIterator> __equal_range(RandomAccessIterator first,
                            RandomAccessIterator last, const T& value, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    Distance len = last - first;
    Distance half;
    RandomAccessIterator middle;
    while (len > 0) {
        half = len >> 1;
        middle = first + half;
        if (*middle < value) {
            first = middle + 1;
            len = len - half - 1;
        }
        else if (value < *middle) {
            len = half;
        }
        else {
            RandomAccessIterator left = lower_bound(first, middle, value);
            RandomAccessIterator right = upper_bound(++middle, first + len, value);
            return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
        }
    }
    // 如果没有匹配的，返回一对迭代器，指向第一个大于value的元素
    return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}

// ForwardIterator版本
template <class ForwardIterator, class T>
pair<ForwardIterator, ForwardIterator> __equal_range(ForwardIterator first,
                ForwardIterator last, const T& value, forward_iterator_tag) {
    typedef typename iterator_traits<ForwardIterator>::difference_type Distance;
    Distance len = 0;
    Distance half;
    ForwardIterator middle;
    distance(first, last, len);
    while (len > 0) {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < value) {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else if (value < *middle) {
            len = half;
        }
        else {
            ForwardIterator left = lower_bound(first, middle, value);
            ++middle;
            advance(first, len);
            ForwardIterator right = upper_bound(middle, first, value);
            return pair<ForwardIterator, ForwardIterator>(left, right);
        }
    }
    // 如果没有匹配的，返回一对迭代器，指向第一个大于value的元素
    return pair<ForwardIterator, ForwardIterator>(first, first);
}
template <class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator first,
                                                          ForwardIterator last, const T& value) {
    typedef typename iterator_traits<ForwardIterator>::iterator_category category;
    __equal_range(first, last, value, category());
}

// 版本2，使用仿函数comp


// nth_element，使第n个迭代器的值为整个序列排序后第n个迭代器的值
// 也就是将排序后第n个元素归位，并且这个元素左半边序列中的所有元素均不大于这个元素
// 这个元素右半边序列中的所有元素均大于这个元素
// 思路有点像做快速排序的partition，每次partition都可以确定某个元素在排序后的序列中的位置
// 不过这里每次partition并不能确定pivot的位置，只是做了切分而已，确定小于pivot和大于pivot元素的个数
// 通过这个方法可以确定某个范围内的元素个数
// 如果某个范围包含了nth，那么对这个范围内的元素进行排序，就可以确定nth的元素
// 因此我们只能不断切分，确定nth所在的序列，然后不断缩小这个序列的大小，直到这个序列大小为3
// 然后对这个序列进行排序，则nth的元素就可以确定了
// 故解决办法就是每次都使用三点中值为pivot的方法选取pivot，然后对序列进行partition
// 如果pivot的位置位于nth的前面，则继续对pivot后面的序列做partition
// 如果pivot的位置位于nth的后面，则继续对pivot前面的序列做partition
// 重复直到剩余序列的长度为3，使用插入排序对他们进行排序，此时这三个元素都已经归位
// nth必然存在于他们三个之中，所以目标达成
template <class RandomAccessIterator, class T>
void __nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, T*) {
    T pivot;
    while (last - first > 3) {
        pivot = __median(*first, *(first + (last - first) / 2), *(last - 1));
        // 根据pivot进行切分
        RandomAccessIterator cut = __unguarded_partition(first, last, pivot);
        // nth在右边，则对右边进行切分
        if (cut <= nth)
            first = cut;
        // nth在左边，则对左边进行切分
        else
            last = cut;
    }
    // 最后对包含nth的小序列排序
    __insert_sort(first, last);
}

template <class RandomAccessIterator>
inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last) {
    __nth_element(first, nth, last, value_type(first));
}

// inplace_merge，将两个有序区间合并为一个
// 因为是将两个区间原地合并，所以需要申请缓冲区


// 有缓冲区的底层inplace_merge函数
template <class BidirectionalIterator, class T, class Distance, class Pointer>
void __merge_adaptive(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last,
                        Distance len1, Distance len2, Pointer buf, Distance buf_size) {
    // 如果序列1比较短，并且缓冲区足够装下序列1
    // 那么序列1拷贝到缓冲区中，然后使用merge将缓冲区1中的序列1和序列2合并
    if (len1 < len2 && len1 <= buf_size) {
        Pointer end_buf = copy(first, middle, buf);
        merge(buf, end_buf, middle, last, first);
    }
    // 如果序列2比较短，并且缓冲区足够装下序列2
    // 将序列2拷贝到缓冲区中，然后使用__merge_backward将两个序列从后往前合并
    else if (len2 <= len1 && len2 <= buf_size){
        Pointer end_buf = copy(middle, last, buf);
        __merge_backward(first, middle, buf, end_buf, last);
    }
    // 如果缓冲区比两个序列都小，就要换一种思路来处理
    // 将序列拆分成两个子序列，前面子序列的所有元素比后面子序列的所有元素小
    // 然后递归处理这两个子序列，这样就可以一直缩小到缓冲区大小足够为止
    else {
        BidirectionalIterator first_cut = first;
        BidirectionalIterator last_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        if (len1 < len2) {
            len22 = len2 / 2;
            advance(last_cut, len22);
            first_cut = upper_bound(first, middle, *last_cut);
            distance(first, first_cut, len11);
        }
        else {
            len11 = len1 / 2;
            advance(first_cut, len11);
            last_cut = lower_bound(middle, last, *first_cut);
            distance(middle, last_cut, len22);
        }
        // 新的中点
        // 前面的序列小于后面的序列
        BidirectionalIterator new_middle = __rotate_adaptive();
        // 递归对前面的序列做排序
        __merge_adaptive(first, first_cut, new_middle, len11, len22, buf, buf_size);
        // 递归对后面的序列做排序
        __merge_adaptive(new_middle, last_cut, last, len1 - len11, len2 - len22, buf, buf_size);
    }
}

// 旋转函数，主要是将后面序列的前几个小的移动到前面序列，而将后面序列的后面几个大的移动到后面序列
// 其实就是集体向左移动，使得后面的元素保持顺序在前面，前面的元素保持顺序在后面
// 从而使得移动后前面序列的所有元素均小于后面序列的所有元素
// 所以才可以递归使用小缓冲区分别对两个子序列做合并
// 先用缓冲区来实现旋转，如果不行再借用rotate方法
template <class BidirectionalIterator1, class BidirectionalIterator2, class Distance>
BidirectionalIterator1 __rotate_adaptive(BidirectionalIterator1 first, BidirectionalIterator1 middle, BidirectionalIterator1 last,
                                            Distance len1, Distance len2, BidirectionalIterator2 buf, Distance buf_size) {
    if (len1 < len2 && len1 <= buf_size) {
        // 将前半序列拷贝到缓冲区
        BidirectionalIterator1 buf_end = copy(first, middle, buf);
        // 后半序列拷贝到前面
        copy(middle, last, first);
        // 前半序列拷贝到后面，返回新的中间点
        return copy_backward(buf, buf_end, last);
    }
    else if (len2 <= len1 && len2 <= buf_size) {
        // 将后半序列拷贝到缓冲区
        BidirectionalIterator1 buf_end = copy(middle, last, buf);
        // 将前半序列从后往前拷贝到后面
        copy_backward(first, middle, last);
        return copy(buf, buf_end, first);
    }
    else {
        // 缓冲区不够，则直接使用原地旋转算法
        rotate(first, middle, last);
        advance(first, len2);
        return first;
    }
}


// 辅助函数
template <class BidirectionalIterator, class T, class Distance>
void __inplace_merge_aux(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, T*, Distance*) {
    Distance distance1 = 0; // 序列1的长度
    Distance distance2 = 0; // 序列2的长度
    distance(first, middle, distance1);
    distance(middle, last, distance2);
    // 申请缓冲区
    // 这个缓冲区实际上就是n个sizeof(T)大小的组成的连续内存空间
    // 迭代器就是这个连续空间上的指向T的指针
    // 外部使用迭代器来操作这个序列
    std::_Temporary_buffer<BidirectionalIterator, T> buf(first, last);
    // 如果没有申请到缓冲区，调用没有缓冲区的版本
    // 过程较复杂
    if (buf.begin() == nullptr)
        __merge_without_buffer(first, middle, last, distance1, distance2);
    // 如果有缓冲区，调用有缓冲区的版本
    else
        __merge_adaptive(first, middle, last, distance1, distance2, buf.begin(), Distance(buf.size()));
}

template <class BidirectionalIterator>
inline void inplace_merge(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last) {
    if (first == middle || middle == last)
        return;
    __inplace_merge_aux(first, middle, last);
}

// 归并排序。利用inplace_merge实现
template <class BidirectionalIterator>
void merge_sort(BidirectionalIterator first, BidirectionalIterator last) {
    typename iterator_traits<BidirectionalIterator>::difference_type n = distance(first, last);
    if (n == 0 || n == 1)
        return;
    BidirectionalIterator middle = first;
    advance(middle, n / 2);
    // 递归使左右序列有序
    merge_sort(first, middle);
    merge_sort(middle, last);
    // 然后使用inplace_merge进行归并
    inplace_merge(first, middle, last);
}

#endif //STL_MY_ALLOCATOR_MY_STL_ALGO_H
