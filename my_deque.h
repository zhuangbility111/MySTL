//
// Created by HP on 2020/6/11.
//

#ifndef STL_MY_ALLOCATOR_MY_DEQUE_H
#define STL_MY_ALLOCATOR_MY_DEQUE_H

#include <iterator>
#include "my_allocator.h"
#include "my_vector.h"

/*
 * 双向队列deque
 * 一个核心结构map（不是stl中的map），实际上是一个链表
 * map上的每一个节点指向一个缓冲区（类似于一个数组组成的内存块）
 * 所以实际上map的每一个节点是一个指向缓冲区的指针
 * 所有节点指向的缓冲区组成了deque的数据区域
 * |__|__|__|__|__| <--- map
 *         |
 *         |
 *         |
 *        |__|__|__|__|__|__|__|__|__|__| <--- 缓冲区
 */


// 如果n不为0，说明用户指定了缓冲区的大小，那么直接返回n
// 如果n为0，表示buffer size为默认值
// 那么如果sz（元素大小）小于512，返回512 / sz
// 如果sz不小于512，返回1
inline size_t __deque_buf_size(size_t n, size_t sz) {
    if (n != 0)
        return n;
    else {
        if (sz < 512)
            return static_cast<size_t>(512 / sz);
        else
            return static_cast<size_t>(1);
    }
}

// 迭代器，抽象成指向deque中其中一个节点的迭代器
// 实则内部是一个指向某个缓冲区上某个节点的指针
// 但是需要保存一些额外的信息来使得它具有抽象成deque中一个节点的迭代器的功能
template <class T, class Ref, class Ptr, size_t Buf_size>
struct __deque_iterator {

    typedef __deque_iterator<T, T&, T*, Buf_size> * iterator;
    typedef __deque_iterator<T, const T&, const T*, Buf_size> * const_iterator;

    typedef __deque_iterator self;

    // 定义一些迭代器的常规属性，为了提供统一接口给外面的调用者使用
    // 迭代器类型
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef ptrdiff_t difference_type;
    // T**为指向map结构中某个节点的指针，方便选定前一个缓冲区或者下一个缓冲区
    typedef value_type ** map_pointer;


    // 指向缓冲区的起始节点，用于移动时进行判断
    value_type* first;
    // 指向缓冲区上的一个节点，对外表示为指向deque上的一个节点
    value_type* cur;
    // 指向缓冲区的终点，功能与first相同，实际是指向结尾节点的下一个位置，也就是最后一个节点的下个位置
    value_type* last;
    // 指向map上的某个节点
    map_pointer map_node;

    // 获取当前缓冲区的大小，这也是一个有用的信息
    static size_t buffer_size() { return __deque_buf_size(Buf_size, sizeof(T)); }

    // 根据map_node_offset更新当前迭代器的对应信息
    void set_node(map_pointer cur_map_node) {
        map_node = cur_map_node;
        first = *cur_map_node;
        last = *cur_map_node + difference_type (buffer_size());
    }

    // 接着就是重载各种迭代器运算符+,-,++,--等
    reference operator*() const { return *cur; }

    pointer operator->() const { return &(operator*()); }

    // 迭代器相减，实际上就是指针相减
    difference_type operator-(self& iter) {
        return static_cast<difference_type>(buffer_size()*(this->map_node - iter.map_node - 1)
                                                + this->cur - this->first
                                                + iter.last - iter.cur);
    }

    // 前缀++，直接返回引用
    self& operator++() {
        cur++;
        if (cur == last) {
            set_node(map_node+1);
            cur = first;
        }
        return *this;
    }

    // 后缀++，因为要先返回值，再修改当前的对象，所以不可能返回临时变量的引用
    // 那么只能返回值而不是引用
    self operator++(int ) {
        self temp = *this;
        ++*this;
        return temp;
    }

    // 前缀--
    self& operator--() {
        // 如果指向第一个节点，则往回退一个map_node，并且cur指向last，因为last本身指向最后一个节点之后的空节点
        // 所以cur还需要进行调整
        if (cur == first) {
            set_node(map_node-1);
            cur = last;
        }
        --cur;
        return *this;
    }

    // 后缀--
    // 同理是返回值，因为不可能返回临时变量的引用
    self operator--(int) {
        self temp = *this;
        --*this;
        return temp;
    }


    // 先实现+=，它是很多符号的基础
    // 迭代器加上一个数，实际上就是指针加上一个数，需要判断相加之后指针是否还留在当前缓冲区上
    // 如果超出当前缓冲区，则需要移动到下一个缓冲区
    // n可能为正数或者负数
    self& operator+=(difference_type n) {
        // 保存map_node的偏移量
        difference_type map_node_offset;
//        if (n >= 0) {
//            if (cur + n <= last)
//                cur += n;
//            else {
//                map_node_offset = (n - (last - cur)) / difference_type (buffer_size());
//                set_node(map_node_offset);
//                cur = first + (n - (last - cur) - (map_node_offset) * difference_type (buffer_size()));
//            }
//        }
//        else {
//            n = -n;
//            if (cur - n >= first)
//                cur -= n;
//            else {
//                map_node_offset = (n - (cur - first)) / difference_type (buffer_size());
//                set_node(map_node_offset);
//                cur = last - (n - (cur - first) - (map_node_offset) * difference_type (buffer_size()));
//            }
//        }
        // 重构代码
        // 先计算移动后元素对应当前first的偏移量
        difference_type offset = cur + n - first;
        // 如果偏移量在0-buffer_size-1，说明移动后迭代器还留在当前缓冲器中，直接移动就行
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            cur += n;
        }
        // 如果偏移量是负数，说明要往前面的map_node移动
        else if (offset < 0){

            map_node_offset = -difference_type ((-offset - 1) / buffer_size()) - 1;
            set_node(map_node + map_node_offset);
            // 这里是同余数的想法，first(原来的) - offset = (map_node_offset)*buffer_size + first(更新后的) - offset
            cur = first + (offset - map_node_offset*difference_type(buffer_size()));
        }
        // 如果偏移量大于等于buffer_size，说明需要往后面的map_node移动
        else if (offset >= difference_type(buffer_size())) {
            map_node_offset = offset / difference_type (buffer_size());
            set_node(map_node + map_node_offset);
            cur = first + (offset - map_node_offset*difference_type(buffer_size()));
        }
        return *this;
    }

    // 迭代器减去一个数，也可以利用+=来实现
    self& operator-=(difference_type n) {
        this->operator+=(-n);
        return *this;
    }

    // 迭代器加上一个数，实际上就是指针加上一个数，需要判断相加之后指针是否还留在当前缓冲区上
    // 如果超出当前缓冲区，则需要移动到下一个缓冲区上
    // 直接利用+=来实现
    self operator+(difference_type n) {
        // 先将当前迭代器拷贝下来
        self temp = *this;
        // 然后利用+=来计算
        temp.operator+=(n);
        // 返回临时变量
        return temp;
    }

    // 迭代器减去一个数，实际上就是指针减去一个数，需要判断相减之后指针是否还留在当前缓冲区上
    // 利用+=来实现
    self operator-(difference_type n) {
        // 临时变量拷贝。然后更新临时变量
        self temp = *this;
        temp.operator-=(n);
        return temp;
    }

    // []运算符，返回跳跃n个距离的值
    // 返回 值的引用
    reference operator[](difference_type n) {
        return *(*this + n);
    }

    // 关系运算符
    bool operator==(const self& x) const {
        return this->cur == x.cur;
    }

    bool operator!=(const self& x) const {
        return !(*this == x);
    }

    bool operator<(const self& x) const {
        if (this->map_node != x.map_node)
            return this->map_node < x.map_node;
        else
            return this->cur < x.cur;
    }

};

// T为保存的数据类型，Alloc为内存分配器类型，Buf_size为变量参数，保存每块缓冲区的大小
template <class T, class Alloc = alloc, size_t Buf_size = 0>
class deque {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef pointer* map_pointer;
    typedef size_t size_type;
    typedef value_type& reference;
    typedef ptrdiff_t difference_type;

    typedef __deque_iterator<T, T&, T*, Buf_size> iterator;

    // 返回指向第一个元素的迭代器
    iterator begin() { return start; }
    // 返回指向最后一个元素的下一个位置的迭代器
    iterator end() { return finish; }
    // 访问deque中的元素，索引为n
    // 利用迭代器的operator[]函数实现
    reference operator[] (size_type n) { return start[difference_type(n)]; }
    // 返回第一个元素
    reference front() { return *start; }
    // 返回最后一个元素
    reference back() { return *(finish-1); }

    // 实际存放元素的总大小
    // 借助迭代器的operator-(iterator)函数实现
    size_type size() const { return finish - start; }
    // 总的容量
    // 因为size_type本身是无符号整型，所以-1转换为无符号整型可以存的最大数值
    size_type max_size() const { return size_type (-1); }
    // 判断是否为空
    bool empty() const { return finish == start; }

    // 根据n，创建空的map数据结构以及缓冲区
    // 其实只要申请内存空间就行，不需要构造对象
    // 构造对象由fill_initialize来负责
    void create_map_and_buffer(size_type n);

    // 填充和初始化
    void fill_initialize(size_type n, const value_type& value);

    // 构造函数，负责生成一个deque，由n个value组成
    // 主要分成两个步骤，申请map和缓冲区的内存，以及构造对象
    deque(int n, const value_type& value) {
        fill_initialize(n, value);
    }

    // 返回缓冲区中的元素个数
    size_type buffer_size() {
        return Buf_size / sizeof(value_type);
    }

    void push_back(const value_type& t) {
        // 如果缓冲区还有剩余空间，则直接在已申请的空间上构造对象
        if (finish.cur != finish.last - 1) {
            // 构造对象
            construct(finish.cur, t);
            // 调整迭代器
            ++finish.cur;
        }
        // 如果缓冲区不够，则需要申请额外的缓冲区
        else {
            push_back_aux(t);
        }
    }

    // 申请额外的缓冲区
    // 如果map节点不够了，则需要先申请map结构，接着拷贝，然后再申请map节点对应缓冲区
    void push_back_aux(const value_type& t) {
        value_type t_copy = t;
        // 调整map结构，从而满足添加新元素进去的空间要求
        // 判断是否需要重新申请一个map结构（如果map结构中节点不够了）
        reserve_map_at_back();
        // 假设不需要，或者申请完毕后
        // 申请新的缓冲区，并将地址保存到map_节点中
        *(finish.map_node + 1) = buffer_allocator::allocate(Buf_size);
        // 先构造对象
        construct(finish.cur, t_copy);
        // 然后更新finish迭代器
        // 因为finish迭代器是指向最后一个元素的下一个位置
        finish.set_node(finish.map_node + 1);
        finish.cur = finish.first;
    }

    void push_front(const value_type& t) {
        // 如果当前缓冲区的前面还剩余空间，直接构造
        if (start.first != start.cur) {
            // 先更新迭代器
            --start.cur;
            // 再构造对象
            construct(start.cur, t);
        }
        // 否则也是需要申请额外的缓冲区
        else {
            push_front_aux(t);
        }
    }

    // 申请额外的缓冲区
    // 如果map节点不够了，则需要先申请map结构，接着拷贝，然后再申请map节点对应缓冲区
    void push_front_aux(const value_type& t) {
        value_type t_copy = t;
        // 调整map结构，从而满足添加新元素进去的空间要求
        // 判断是否需要重新申请一个map结构（如果map结构中节点不够了）
        reserve_map_at_front();
        // 申请完毕，为map节点申请缓冲区内存，然后构造对象，更新迭代器
        *(start.map_node - 1) = buffer_allocator::allocate(Buf_size);
        start.set_node(start.map_node - 1);
        start.cur = start.last-1;
        construct(start.cur, t_copy);
    }

    // 调整map结构，从而满足添加新元素进去的空间要求
    // 从map数据结构的尾部来判断是否需要扩充map数据结构，如果如果需要则扩充，并将原来的数据拷贝到新的内存位置
    // map_nodes_to_add为需要添加的map_node个数
    void reserve_map_at_back(size_type map_nodes_to_add = 1) {
        // 如果map结构后面剩余的节点个数小于某个值，则需要重新申请内存空间，扩容
        if (map_nodes_to_add + 1 > map_size - (finish.map_node - map)) {
            reallocate_map(map_nodes_to_add, false);
        }
    }

    // 调整map结构，从而满足添加新元素进去的空间要求
    // 从map数据结构的头部来判断是否需要扩充map数据结构，如果需要则扩充，并将原来的数据拷贝到新的内存位置
    // map_nodes_to_add为需要添加的map_node个数
    void reserve_map_at_front(size_type map_nodes_to_add = 1) {
        // 如果map结构前面剩余的节点个数小于某个值，则需要重新申请内存空间，扩容
        if (map_nodes_to_add > start.map_node - map) {
            reallocate_map(map_nodes_to_add, true);
        }

    }

    // 这个函数是reserve_at_back和reserve_at_front的底层实现
    // 用于扩充map数据结构
    // 并完成一些附带的操作，比如将原来的数据拷贝到新的内存空间
    // map_nodes_to_add为需要添加的map_node个数
    // add_at_front则判断是往前扩充还是往后扩充map结构
    // 扩充的原理如下：
    // 如果当前map结构的大小大于新的map结构的2倍，那么说明还有很多空闲的map节点
    // 只是因为前面或者后面没有空余的节点了
    // 那么这种情况不需要重新申请更大的内存空间，而是对map结构进行调整，将真正拥有缓冲区的map节点移动到map结构的中间
    // 而如果当前map结构的大小小于新的map结构的2倍，那么则说明不够空间的map节点
    // 因此需要重新申请更大的内存空间，并将map结构进行拷贝
    void reallocate_map(size_type map_nodes_to_add, bool add_at_front) {
        size_type old_map_nodes_num = finish.map_node - start.map_node + 1;
        size_type new_map_nodes_num = old_map_nodes_num + map_nodes_to_add;

        map_pointer new_start;
        // 如果当前map结构的大小大于新的map结构的2倍，说明map结构中还有很多空闲节点，不需要重新申请
        // 直接移动一下
        // 如果前面节点不足，则往后移动；如果后面节点不足，则往前移动
        if (map_size > 2 * new_map_nodes_num) {
            new_start = map + (map_size - new_map_nodes_num) / 2;
            if (add_at_front)
                new_start += map_nodes_to_add;
            // 确定新的起点之后，开始拷贝
            // 通过判断新起点与旧起点的相对位置，来决定是从后往前拷贝，还是从前往后拷贝
            if (new_start < start.map_node)
                copy(start.map_node, finish.map_node + 1, new_start);
            else
                copy_backward(start.map_node, finish.map_node + 1, new_start + old_map_nodes_num);
        }
        // map结构中没有太多的空闲节点，则重新申请内存空间，并将原来的map结构拷贝到新的内存位置
        else {
            // 如果所需扩充的节点数比原map的节点个数还要大，那就按前者来扩充，否则就两倍map节点个数
            size_type new_map_size = map_size + max(map_size, map_nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_start = new_map + (new_map_size - new_map_nodes_num) / 2;
            if (add_at_front)
                new_start += map_nodes_to_add;
            // 将旧的map结构的数据拷贝到新的map结构
            copy(start.map_node, finish.map_node + 1, new_start);
            // 然后回收旧的map结构的内存空间
            map_allocator::deallocate(map, map_size);
            map = new_map;
            map_size = new_map_size;
        }
        // 更新起始和结尾迭代器
        start.set_node(new_start);
        finish.set_node(new_start + old_map_nodes_num - 1);

    }

    void pop_back() {
        // 如果不是缓冲区的第一个节点，则直接析构
        if (finish.cur != finish.first) {
            --finish.cur;
            destroy(finish.cur);
        }
        // 如果是缓冲区的第一个节点，则需要释放这个缓冲区，调整整个结构
        // 调用辅助函数来帮忙
        else {
            pop_back_aux();
        }
    }

    // pop_back的辅助函数
    // 主要对应删除的点为所在缓冲区的第一个节点，需要释放缓冲区
    void pop_back_aux() {
        // 释放缓冲区，并移动finish迭代器
        buffer_allocator::deallocate(finish.first, buffer_size());
        finish.set_node(finish.map_node-1);
        finish.cur = finish.last - 1;
        destroy(finish.cur);
    }

    void pop_front() {
        // 如果不是缓冲区的最后一个节点，则直接析构
        if (start.cur != start.last - 1) {
            destroy(start.cur);
            start.cur++;
        }
        // 如果是缓冲区的最后一个节点，与pop_back相同，需要借助辅助函数来调整内存空间，回收缓冲区
        else {
            pop_front_aux();
        }
    }

    void pop_front_aux() {
        // 释放缓冲区，并移动start迭代器
        destroy(start.cur);
        buffer_allocator::deallocate(start.first, buffer_size());
        start.set_node(start.map_node+1);
        start.cur = start.first;
    }

    // 请空deque，保留一个缓冲区
    void clear() {
        map_pointer cur;
        // 头缓冲区和尾缓冲区不是完全使用的，所以需要挑出来特殊处理
        for (cur = start.map_node + 1; cur < finish.map_node; cur++) {
            // 析构
            destroy(*cur, *cur + Buf_size);
            // 释放空间
            buffer_allocator::deallocate(*cur, buffer_size());
        }
        // 如果头缓冲区和尾缓冲区不是同一个
        // 只保留头缓冲区
        if (start.map_node != finish.map_node) {
            // 先分别析构
            destroy(start.cur, start.last);
            destroy(finish.first, finish.cur);
            // 释放尾缓冲区的内存空间
            buffer_allocator::deallocate(finish.first, buffer_size());
        }
        // 如果是同一个，那么直接析构就可以了
        else {
            destroy(start.cur, finish.cur);
        }
        // 更新迭代器，调用拷贝赋值运算符
        finish = start;
    }

    // 清除pos指向的元素，pos为清除点
    // 返回新位置的迭代器
    // 如果当前位置前面的节点比较少，则将前面的节点往后移
    // 如果当前位置后面的节点比较少，则将后面的节点往后移
    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        // 前面的节点比较少
        if (index < (size() >> 1)) {
            // 因为旧的范围在前面，所以使用后向拷贝
            // 为什么能用copy呢？其实copy的参数就是符合stl规则的迭代器
            // 也就是只要是符合stl规则的迭代器，都可以作为参数使用copy
            // copy内部实现实际就是迭代器的+、-等操作
            // 所以只要重载了相应的运算符，符合stl规则
            // 就可以将自定义的迭代器作为参数使用copy
            // 迭代器隐藏了内部实现的不同，只是给外层提供了重载的运算符，所以不同的迭代器使用起来是一样的
            // 所以才有这种万能的stl算法，只要参数符合规则，就可以使用
            copy_backward(start, pos, next);
            // 删除第一个节点
            pop_front();
        }
        // 后面的节点比较少
        else {
            // 从前往后拷贝
            copy(next, finish, pos);
            // 删除最后一个节点
            pop_back();
        }
        return start + index;
    }

    // 删除[first, last)区间内的元素
    // 依然是判断区间前面的元素多还是后面的元素多
    iterator erase(iterator first, iterator last) {
        // 如果范围是整个deque，直接调用clear
        if (first == start && last == finish) {
            clear();
            return finish;
        }

        else {
            difference_type front_num = first - start;
            difference_type back_num = finish - last;
            difference_type n = last - first;
            if (front_num > back_num) {
                // 从后往前拷贝
                copy_backward(start, first, last);
                iterator new_start = start + n;
                // 析构
                destroy(start, new_start);
                // 释放前面的内存空间
                map_pointer cur = start.map_node;
                for (; cur < new_start.map_node; cur++) {
                    buffer_allocator::deallocate(*cur, buffer_size());
                }
                // 更新迭代器
                start = new_start;
            }
            else {
                // 从前往后拷贝
                copy(last, finish, first);
                iterator new_finish = finish - n;
                // 析构
                destroy(new_finish, finish);
                // 释放后面的内存空间
                map_pointer cur = new_finish.map_node + 1;
                for(; cur <= finish.map_node; cur++) {
                    buffer_allocator::deallocate(*cur, buffer_size());
                }
                // 更新迭代器
                finish = new_finish;
            }
            return start + front_num;
        }

    }

    // 在position指向的元素之前插入一个x
    // 返回插入之后位置的迭代器
    iterator insert(iterator position, const value_type& x) {
        // 如果插入位置是start，直接调用push_front()
        if (position == start) {
            push_front(x);
            return start;
        }
        // 如果插入位置是finish，直接调用push_back()
        else if (position == finish) {
            push_back(x);
            return (finish - 1);
        }
        // 否则如果插入位置是中间的话，调用辅助函数
        else {
            return insert_aux(position, x);
        }

    }

    // insert的辅助函数
    // 判断插入的点是靠前的点还是靠后的点，靠前的点则将前面部分往前移动，靠后的点则将后面部分wanghouyidong
    iterator insert_aux(iterator pos, const value_type& x) {
        difference_type index = pos - start;
        value_type x_copy = x;
        // 靠前的位置
        if (index < size() / 2) {
            // 先在前面插入一个节点，然后往前覆盖
            push_front(front());
            // 其实下面这一部分代码可以使用start+1， pos+1等实现
            // 使用前缀++的好处是可以减少一次拷贝，提高效率
            // 使用+，然后再拷贝，相当于执行了两次拷贝，中间多了一个临时变量temp
            // 新拷贝区间的起点
            iterator front1 = start;
            ++front1;
            // 旧拷贝区间的起点
            iterator front2 = front1;
            ++front2;
            pos = start + index;
            // 旧拷贝区间的终点
            iterator pos1 = pos;
            ++pos1;
            copy(front2, pos1, front1);
        }
        // 靠后的位置
        else {
            // 先在后面插入一个节点，然后往后覆盖
            push_back(back());
            iterator back1 = finish;
            // 新拷贝区间的终点
            --back1;
            // 旧拷贝区间的终点
            iterator back2 = back1;
            --back2;
            // 旧拷贝区间的起点
            pos = start + index;
            copy_backward(pos, back2, back1);

        }
        *pos = x_copy;
        return pos;
    }


protected:
    // 指向起始元素的迭代器
    iterator start;
    // 指向最后一个元素的下一个位置的迭代器
    iterator finish;

    // 指向map数据结构的指针，也就是指向map数据结构中第一个含缓冲区的map_node
    map_pointer map;
    size_type map_size;     // map数据结构有多少个节点（包括含缓冲区和不含缓冲区的节点）

    // 内存分配器，主要负责对缓冲区的内存进行分配，一次分配一个元素大小
    typedef simple_alloc<value_type, Alloc> buffer_allocator;
    // 内存分配器，主要负责对map进行分配，一次分配一个map_node
    typedef simple_alloc<pointer, Alloc> map_allocator;





};

// 负责申请内存空间
template <class T, class Alloc, size_t Buf_size>
void deque<T, Alloc, Buf_size>::create_map_and_buffer(size_type n) {
    size_type buffer_num = n / Buf_size + 1;
    // 如果buffer_num+2 小于 8，则默认的缓冲区个数为8
    // 如果不小于，则缓冲区的个数为buffer_num+2
    map_size = max(buffer_num + 2, size_type (8));
    // 根据buffer_num申请map的内存空间
    map = map_allocator::allocate(map_size);
    // 接着为每个缓冲区申请内存空间
    // 要让含有缓冲区的map_node在map中间，这样才适合前插和后插
    map_pointer cur = map + (map_size - buffer_num) / 2;
    // 指向最后一个包含缓冲区的map_node
    map_pointer map_finish = cur + buffer_num - 1;
    for (; cur <= map_finish; cur++) {
        // 使用缓冲区对应的内存分配器来分配缓冲区的内存
        *cur = buffer_allocator::allocate(Buf_size);
    }
    map_pointer map_start = map_finish + 1 - buffer_num;
    // 接着设置deque的起始和结尾迭代器，为它们设置相关的初始信息，缓冲区的头和尾、指向当前缓冲区的哪个元素
    start.set_node(map_start);
    start.cur = start.first;
    finish.set_node(map_finish);
    finish.cur = finish.first + (n % Buf_size);

}

// 负责构造对象
template <class T, class Alloc, size_t Buf_size>
void deque<T, Alloc, Buf_size>::fill_initialize(size_type n, const value_type &value) {
    // 创建map结构和buffer结构，并根据填入的元素个数将start迭代器和finish迭代器设置好
    create_map_and_buffer(n);
    // 接着就是使用未初始化函数填充map和buffer
    map_pointer cur;
    for (cur = start.map_node; cur != finish.map_node; cur++) {
        uninitialized_fill(*cur, *cur + Buf_size, value);
    }
    // 最后一个map_node指向的缓冲区中，元素可能未填满，所以需要挑出来特殊处理
    // finish.first为finish迭代器指向的元素所在的缓冲区的首地址，而cur为finish迭代器指向的元素的地址
    uninitialized_fill(finish.first, finish.cur, value);
}


#endif //STL_MY_ALLOCATOR_MY_DEQUE_H
