//
// Created by HP on 2020/8/1.
//

#ifndef STL_MY_ALLOCATOR_MY_STL_ALGOBASE_H
#define STL_MY_ALLOCATOR_MY_STL_ALGOBASE_H

#include <type_traits>

/*
 * 每一个函数基本都有两个版本，一个是operator<的版本
 * 一个是自定义比较对象的版本
 */


// 判断两个区间范围内的元素是否相等
// 版本1，指定区间1的范围和区间2的起点
template <class InputIterator1, class InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2))
            return false;
    }
    return true;
}

// 版本2，指定区间1的范围和区间2的起点，以及判断相等的仿函数
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate p) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!p(*first1, *first2))
            return false;
    }
    return true;
}

// 版本3，指定区间1的范围和区间2的范围
template <class InputIterator1, class InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    if (distance(first1, last1) != distance(first2, last2))
        return false;
    for (; first1 != last1, first2 != last2; ++first1, ++first2) {
        if (!(*first1 == *first2))
            return false;
    }
    return true;
}

// 版本4，指定区间1的范围和区间2的范围，以及判断相等的仿函数
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate p) {
    if (distance(first1, last1) != distance(first2, last2))
        return false;
    for (; first1 != last1, first2 != last2; ++first1, ++first2) {
        if (!p(*first1, *first2))
            return false;
    }
    return true;
}


// 将[first, last)内的所有元素改填新值
template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value) {
    for (; first != last; first++) {
        *first = value;
    }
}

// 将[first, last)内的前n个元素改填新值，返回的迭代器指向被填入的最后一个元素的下一个位置
template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    for (; n > 0; --n, ++first) {
        *first = value;
    }
    return first;
}

// 将两个ForwardIterator所指的对象对调
template <class ForwardIterator1, class ForwardIterator2>
void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
    // 直接借助std::swap()实现（C++20）
    std::swap(a, b);
}

// 检查第一个范围 [first1, last1) 是否按字典序小于第二个范围 [first2, last2) 。
// 逐元素比较二个范围。
// 首个不匹配元素定义范围是否按字典序小于或大于另一个。
// 若一个范围是另一个的前缀，则较短的范围小于另一个。
// 若二个范围拥有等价元素和相同长度，则范围按字典序相等。
// 空范围按字典序小于任何非空范围。
// 二个空范围按字典序相等。
// 以上规则可以变为四种可能：
// 1. 如果第一序列的元素较小，返回true；否则返回false
// 2. 如果到达last1而尚未到达last2，返回true
// 3. 如果到达last2而尚未到达last1，返回false
// 4. 如果同时到达last1和last2（也就是所有元素都匹配），返回true
// 版本1
template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2)
            return true;
        else if (*first1 > *first2)
            return false;
    }
    // 只有first1到达last1，而first2没有到达last2，才可能返回true
    // 其他情况均返回false
    return (first1 == last1) && (first2 != last2);
}

// 版本2，指定比较对象
template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2, Compare compare) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (compare(*first1, *first2))
            return true;
        else if (compare(*first2, *first1))
            return false;
    }
    // 只有first1到达last1，而first2没有到达last2，才可能返回true
    // 其他情况均返回false
    return (first1 == last1) && (first2 != last2);
}

// 版本3，特化版本，针对参数为const unsigned char*，也就是char指针
// 实际上保存的是字符串，因此可以使用memcmp()提高效率
bool lexicographical_compare(const unsigned char* first1, const unsigned char* last1,
                                const unsigned char* first2, const unsigned char* last2) {
    // 两个指针范围，实际上是两个字符串在比较，直接使用memcmp提高效率
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = memcmp(first1, first2, min(len1, len2));
    // 如果相等，则len1较小则返回true
    return result != 0 ? result < 0 : len1 < len2;
}

// 返回两者中较大的那个
// 版本1
template <class T>
const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

// 版本2，使用自定义compare函数
template <class T, class Compare>
const T& max(const T& a, const T& b, Compare compare) {
    return compare(a, b) ? b : a;
}

// 返回两者中较小的那个
// 版本1
template <class T>
const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}

// 版本2，使用自定义compare函数
template <class T, class Compare>
const T& min(const T& a, const T& b, Compare compare) {
    return compare(b, a) ? b : a;
}

// p平行比较两个序列，指出两者之间的第一个不匹配点
// 返回一对迭代器，分别指向两序列中的不匹配点
// 如果两个序列相等，则返回两者的last迭代器
// 版本1，序列1的两个迭代器和序列2的起始迭代器
template <class InputIterator1, class InputIterator2>
std::pair<InputIterator1, InputIterator2>
        mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    while (first1 != last1 && *(first1) == *(first2)) {
        ++first1;
        ++first2;
    }
    return make_pair(first1, first2);
}

// 版本2，序列1的两个迭代器和序列2的起始迭代器以及equality对象
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
std::pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate p) {
    while (first1 != last1 && p(*(first1), *(first2))) {
        ++first1;
        ++first2;
    }
    return make_pair(first1, first2);
}

// 版本3，序列1的两个迭代器和序列2的两个迭代器
template <class InputIterator1, class InputIterator2>
std::pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && first2 != last2 && *(first1) == *(first2)) {
        ++first1;
        ++first2;
    }
    return make_pair(first1, first2);
}

// 版本4，序列1的两个迭代器和序列2的两个迭代器以及equality对象
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
std::pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate p) {
    while (first1 != last1 && first2 != last2 && p(*(first1), *(first2))) {
        ++first1;
        ++first2;
    }
    return make_pair(first1, first2);
}


// -----------------------------------------------------------------------------------
// 接下来是copy函数，负责将[first,last)内的元素复制到输出区间[result, result + (last - first))
// 这是一个比较复杂的函数，因为涉及到多种优化方法
// 使用到函数重载、type_traits、偏特化等技巧
// 主要有几个可优化的点是：
// 1. 如果迭代器指向的序列是字符串，那么直接使用字符串拷贝，效率会提高
// 2. 如果迭代器本身是指针，那么这个序列在内存上是连续存放的，所以可能可以使用memmove复制底层内存来加速
// 如果指针指向的类型拥有trivial operator=（例如int类型），那么直接使用memmove复制底层内存是一种更快的方法
// 3. 如果迭代器本身就是迭代器，那么所指向的元素在内存上可能不是连续存放的，所以不能使用memmove
// 但是迭代器有分InputIterator和RandomIterator，RandomIterator可以加速复制过程


// 只要迭代器类型为RandomAccessIterator（包括指针），底层都是使用这个函数进行复制
// 因为指针和RandomAccessIterator存在共同的复制操作
// 所以将这个共同的复制操作抽出来，合成一个函数
template <class RandomAccessIterator, class OutputIterator>
OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result) {
    typedef typename std::iterator_traits<RandomAccessIterator>::difference_type Distance;
    for (Distance n = last - first; n > 0; --n, ++first, ++result) {
        *result = *first;
    }
    return result;
}

// 指针指向的类型有trivial_assignment_operator，也就是可以使用memmove加速复制过程
// 而不需要调用拷贝赋值运算符
template <class T>
T* __copy_t(const T* first,const T* last, T* result, __true_type) {
    memmove(result, first, sizeof(T)*(last - first));
    return result + (last - first);
}

// 指针指向的类型没有trivial_assignment_operator，只能使用拷贝赋值运算符
template <class T>
T* __copy_t(const T* first,const T* last, T* result, __false_type) {
    // 指针本身也是RandomAccessIterator，所以也可以调用这个函数来实现复制
    __copy_d(first, last, result);
}

// InputIterator类型迭代器调用的版本，最低效的复制方法
template <class InputIterator, class OutputIterator>
InputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, std::input_iterator_tag) {
    for(; first != last; ++first, ++result) {
        *result = *first;
    }
    return result;
}

// RandomAccessIterator类型迭代器调用的版本，可以对复制过程进行优化
template <class RandomAccessIterator, class OutputIterator>
OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last,
                        OutputIterator result, std::random_access_iterator_tag) {
    __copy_d(first, last, result);
}

// 负责copy函数分发的结构体，主要就是应对真的迭代器
// 迭代器指向的元素一般在内存上不是连续的，所以没有机会像指针迭代器那样，使用memmove复制底层元素
template <class InputIterator, class OutputIterator>
struct __copy_dispatch {
    // 使结构体变为函数对象，函数参数为作为函数对象时传入的函数参数
    OutputIterator operator() (InputIterator first, InputIterator last, OutputIterator result) {
        typedef typename std::iterator_traits<InputIterator>::iterator_category iterator_category;
        // 根据迭代器类型调用指定版本的底层copy函数
        // 这里主要是如果迭代器类型为RandomAccessIterator时，可以对复制过程进行加速
        // 所以需要区分不同迭代器类型的实现版本
        __copy(first, last, result, iterator_category());
    }
};

// 偏特化的copy函数分发的结构体，主要应对迭代器为指针类型
template <class T>
struct __copy_dispatch<T*, T*> {
    // 使结构体变为函数对象，函数参数为作为函数对象时传入的函数参数
    T* operator() (T* first, T* last, T* result) {
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        // 根据指针指向的类型是否有trivial_assignment_operator来决定是否使用memmove加速复制
        __copy_t(first, last, result, t());
    }
};

// 偏特化的copy函数分发的结构体，主要应对迭代器为const指针类型
template <class T>
struct __copy_dispatch<const T*, T*> {
    // 使结构体变为函数对象，函数参数为作为函数对象时传入的函数参数
    T* operator() (T* first, T* last, T* result) {
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        // 根据指针指向的类型是否有trivial_assignment_operator来决定是否使用memmove加速复制
        __copy_t(first, last, result, t());
    }
};

// 最上层接口，泛化版本
template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
    return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

// 同样是最上层接口，但是是重载版本，主要应对迭代器为const char*
// 这时序列为字符串，可以考虑直接使用memmove加速复制
template<>
char* copy(const char* first, const char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

// 同样是最上层接口，但是是重载版本，主要应对迭代器为const wchar_t*
// 这时序列为字符串，可以考虑直接使用memmove加速复制
template<>
wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    memmove(result, first, sizeof(wchar_t)*(last - first));
    return result + (last - first);
}

// -----------------------------------------------------------------------------------------------
// copy_backward
// 泛化版本
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                        BidirectionalIterator2  result) {
    if (first == last)
        return result;
    __copy_backward_dispatch(first, last, result);
}

// 特化版本，针对char*
template <>
char* copy_backward(const char* first, const char* last, char* result) {
    size_t n = last - first;
    char* des_begin = result - n;
    memmove(des_begin, first, n * sizeof(char));
    return result;
}

// 特化版本，针对wchar_t*
template <>
wchar_t* copy_backward(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    size_t n = last - first;
    wchar_t* des_begin = result - n;
    memmove(des_begin, first, n * sizeof(wchar_t));
    return result;
}

// 分发结构体函数，泛化版本
// 根据迭代器类型调用不同版本的底层函数
template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __copy_backward_dispatch {
    BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                        BidirectionalIterator2 result) {
        typedef typename std::iterator_traits<BidirectionalIterator1>::iterator_category iterator_category;
        __copy_backward(first, last, result, iterator_category());
    }
};

// 分发结构体函数的偏特化版本
template <class T>
struct __copy_backward_dispatch<T*, T*> {
    T* operator()(T* first, T* last, T* result) {
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        __copy_backward_t(first, last, result, t());
    }
};

template <class T>
struct __copy_backward_dispatch<const T*, T*> {
    T* operator()(const T* first, const T* last, T* result) {
        typedef typename __type_traits<T>::has_trivial_assignment_operator t;
        __copy_backward_t(first, last, result, t());
    }
};

// 针对迭代器为指针的特化版本底层函数，有trivial的copy assignment
template <class T>
T* __copy_backward_t(const T* first, const T* last, T* result, __true_type) {
    size_t n = last - first;
    T* des_begin = result - n;
    memmove(des_begin, first, n * sizeof(T));
    return result;
}

// 有 non-trivial的copy assignment
// 那么必须使用随机访问迭代器的版本
template <class T>
T* __copy_backward_t(const T* first, const T* last, T* result, __false_type) {
    __copy_backward_d(first, last, result);
}

// 根据迭代器的不同类型，执行不同的底层函数
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                        BidirectionalIterator2 result) {
    typedef typename iterator_traits<BidirectionalIterator1>::iterator_category category;
    __copy_backward(first, last, result, category());
}

// 双向单步迭代器的版本
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
                                       BidirectionalIterator2 result, bidirectional_iterator_tag) {
    while (first != last) {
        *(--result) = *(--last);
    }
    return result;
}

// 随机访问迭代器的版本
template <class RandomAccessIterator, class BidirectionalIterator2>
BidirectionalIterator2 __copy_backward(RandomAccessIterator first, RandomAccessIterator last,
                                       BidirectionalIterator2 result, random_access_iterator_tag) {
    __copy_backward_d(first, last, result);
}

template <class RandomAccessIterator, class BidirectionalIterator2>
BidirectionalIterator2 __copy_backward_d(RandomAccessIterator first, RandomAccessIterator last,
                                        BidirectionalIterator2 result) {
    typedef typename iterator_traits<RandomAccessIterator>::difference_type Distance;
    Distance n = last - first;
    while (n > 0) {
        *(--result) = *(--last);
        --n;
    }
    return result;
}

#endif //STL_MY_ALLOCATOR_MY_STL_ALGOBASE_H
