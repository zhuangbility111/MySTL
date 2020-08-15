//
// Created by HP on 2020/7/20.
//

#ifndef STL_MY_ALLOCATOR_MY_HASHTABLE_H
#define STL_MY_ALLOCATOR_MY_HASHTABLE_H

// 定义hash表中的节点结构
template <class Value>
struct _hashtable_node {
    Value data;
    _hashtable_node* next;
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable {
public:
    typedef HashFcn hasher;
    typedef size_t size_type;
    typedef Value value_type;
    typedef Key key_type;
    typedef EqualKey key_equal;

    typedef _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;

private:
    // hash函数对象
    // 只能通过key来获取对象的hash值
    hasher hash;

    // 提取key函数对象
    ExtractKey get_key;

    // 判断key相等函数对象
    key_equal equals;

    typedef _hashtable_node<Value> Node;

    // vector保存所有的桶
    vector<Node*, Alloc> buckets;

    // 节点专属的内存配置器
    typedef simple_alloc<Node, Alloc> node_allocator;

    size_type elem_nums;

public:
    // 桶的个数
    size_type bucket_count() const { return buckets.size(); }

    // 质数组合，表格的size必须为下面28个质数中的一个，最接近并大于n的那个质数
    static const int __stl_num_primes = 28;
    static const unsigned long __stl_prime_list[__stl_num_primes] = {
        53, 97, 193, 389, 769,
        1543, 3079, 6151, 12289, 24593,
        49157, 98317, 196613, 393241, 786433,
        1572869, 3145739, 6291469, 12582917, 25165843,
        50331653, 100663319, 201326611, 402653189, 805306457,
        1610612742, 3221225473ul, 4294967291ul
    };

    inline unsigned long __stl_next_prime(unsigned long n) {
        const unsigned long* first = __stl_prime_list;
        const unsigned long* last = __stl_prime_list + __stl_num_primes;
        const unsigned long* pos = lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    // 最大的桶数量
    size_type max_bucket_size() const {
        return __stl_prime_list[__stl_num_primes - 1];
    }

    // 创建一个新节点
    Node* new_node (const value_type& data) {
        // 申请内存
        Node* node = node_allocator::allocate();
        construct(node->data, data);
        node->next = nullptr;
        return node;
    }

    void delete_node (Node* node) {
        destroy(node->data);
        node_allocator::deallocate(node);
    }


    // 构造函数，不提供默认构造函数
    // n 为桶的个数
    hashtable(size_type n, const HashFcn& hashFcn, const EqualKey& equalKey)
        : hash(hashFcn), equals(equalKey), get_key(ExtractKey()), elem_nums(0){
        initialize_buckets(n);
    }

    void initialize_buckets(size_type n) {
        size_type n_buckets = next_prime(n);
        buckets.reserve(n_buckets);
        // 将桶全部设置为空
        buckets.insert(buckets.end(), n_buckets, nullptr);
        elem_nums = 0;
    }

    // 返回大于n的下一个质数
    size_type next_prime(size_type n) {
        return __stl_next_prime(n);
    }

    hasher hash_funct() const {
        return this->hash;
    }

    key_equal key_eq() const {
        return this->equals;
    }

    size_type size() const {
        return this->elem_nums;
    }

    size_type max_size() const {
        return (size_type)(-1);
    }

    bool empty() const {
        return (elem_nums == 0);
    }

    // 重建表格
    void resize(const size_type&);

    // 不允许重复的插入
    pair<iterator, bool> insert_unique(const value_type& obj) {
        // 首先判断是否需要重建表格，如果需要则要扩充表格
        resize(elem_nums + 1);
        return insert_unique_noresize(obj);
    }

    template <class InputIterator>
    void insert_unique(const InputIterator& first, const InputIterator& last) {
        for (InputIterator cur = first; cur != last; cur++) {
            insert_unique(*cur);
        }
    }

    // 不允许重复的插入
    // 如果hashtable中已经有了这个值，则返回false
    // 否则，插入节点，并返回true
    pair<iterator, bool> insert_unique_noresize(const value_type&);

    // 允许重复的插入
    iterator insert_equal(const value_type& obj) {
        // 首先判断是否需要重建表格，如果需要则要扩充表格
        resize(elem_nums + 1);
        return insert_equal_noresize(obj);
    }

    iterator insert_equal_noresize(const value_type& data);

    // 使用hash函数来获取值所在的桶，总共有四个版本
    // 版本1，value_type 和 桶个数
    size_type bkt_num(const value_type& data, size_type n) {
        // 通过value获取key，然后借助版本3来实现
        return bkt_num(get_key(data), buckets.size());
    }

    // 版本2，只有一个value_type
    size_type bkt_num(const value_type& data) {
        // 通过value获取key，然后借助版本4来实现
        return bkt_num(get_key(data));
    }

    // 版本3，key_type 和 桶个数
    size_type bkt_num(const key_type& key, size_type n) {
        // 根据key值和hash函数获取对应的hash值，并取模，保证它一定属于某一个桶
        return hash(key) % n;
    }

    // 版本4，key_type
    size_type bkt_num(const key_type& key) {
        // 借用版本3来实现
        return bkt_num(key, buckets.size());
    }

    void clear();

    void copy_from(const hashtable&);

    // 根据键值在hashtable中寻找，返回迭代器
    iterator find(const key_type& key) {
        size_type bucket_index = bkt_num(key);
        for (Node* cur = buckets[bucket_index]; cur != nullptr; cur = cur->next) {
            if (equals(get_key(cur->data), key))
                return iterator(cur, this);
        }
        return iterator(nullptr, this);
    }

    // 根据键值计算这个键在hashtable中出现多少次
    size_type count(const key_type& key) {
        size_type bucket_index = bkt_num(key);
        size_type result = 0;
        for (Node* cur = buckets[bucket_index]; cur != nullptr; cur = cur->next) {
            if (equals(get_key(cur->data), key))
                ++result;
        }
        return result;
    }

};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::resize(const size_type& n) {
    // 表格重建与否的判断原则是拿元素个数（把新增元素计入后）和bucket vector的大小来比
    // 如果前者大于后者，则重建表格
    size_type old_n = buckets.size();
    // 重建表格
    if (n > old_n) {
        size_type new_size = next_prime(n);
        if (new_size > old_n) {
            vector<Node*, Alloc> new_buckets(new_size, nullptr);
            // 原来桶中的值需要重新映射，而不是直接拷贝
            for (size_type i = 0; i < old_n; i++) {
                Node* temp_node = buckets[i];
                while (temp_node != nullptr) {
                    // 计算他的哈希值，然后插入到新vector中对应的list上
                    size_type new_bucket = bkt_num(temp_node->data, n);
                    buckets[i] = temp_node->next;
                    temp_node->next = new_buckets[new_bucket];
                    new_buckets[new_bucket] = temp_node;
                    temp_node = buckets[i];
                }
            }
            buckets.swap(new_buckets);
        }

    }
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator, bool>
    hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_unique_noresize(const value_type & data) {
    size_type bucket_index = bkt_num(data);
    Node* cur = buckets[bucket_index];
    for(; cur != nullptr; cur = cur->next) {
        if (equals(get_key(data), get_key(cur->data)))
            return pair<iterator, bool>(iterator(cur, *this), false);
    }
    // 在桶里插入新节点
    cur = new_node(data);
    cur->next = buckets[bucket_index];
    buckets[bucket_index] = cur;
    ++elem_nums;
    return pair<iterator, bool>(iterator(cur, *this), true);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_equal_noresize(const value_type & data) {
    size_type bucket_index = bkt_num(data);
    Node* cur = buckets[bucket_index];
    // 先在桶里面找相同的，如果找到了，就在那个位置插入
    // 否则就在桶的最前面插入
    for (; cur != nullptr; cur = cur->next) {
        if (equals(get_key(data), get_key(cur->data))) {
            Node* node = new_node(data);
            node->next = cur->next;
            cur->next = node;
            ++elem_nums;
            return iterator(node, *this);
        }
    }
    // 没有找到，插到桶的最前面
    Node* node = new_node(data);
    node->next = buckets[bucket_index];
    buckets[bucket_index] = node;
    ++elem_nums;
    return iterator(node, *this);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::clear() {
    // 清空所有桶内的节点
    Node* cur;
    for (int i = 0; i < bucket_size(); i++) {
        cur = buckets[i];
        while (cur != nullptr) {
            buckets[i] = cur->next;
            delete_node(cur);
            cur = buckets[i];
        }
        buckets[i] = nullptr;
    }
    elem_nums = 0;
    // 只清空桶内的节点，不清除vector
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::copy_from(const hashtable &ht) {
    // 先清除原有的节点
    this->clear();
    // 然后清除vector中的内容
    buckets.clear();
    // 调整vector的容量为待拷贝hashtable中vector的大小
    buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), nullptr);
    // 将hashtable中所有桶的节点直接拷贝过来
    for (int i = 0; i < ht.buckets.size(); i++) {
        Node* cur = ht.buckets[i];
        if (cur != nullptr) {
            // 先拷贝第一个节点
            Node* copy = new_node(cur->data);
            buckets[i] = copy;
            // 然后就顺着list一直往下拷贝
            for (Node* cur_next = cur->next; cur_next != nullptr; cur_next = cur_next->next) {
                copy->next = new_node(cur_next->data);
                copy = copy->next;
            }
        }

    }
    // 更新节点数量
    elem_nums = ht.elem_nums;

}

// 定义hash表的迭代器
// HashFcn、ExtractKey、EqualKey均为函数对象
// 其中HashFcn为hash函数，ExtractKey为从value中提取Key的方法，EqualKey为判断key是否相等的方法
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct _hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef _hashtable_node<Value> Node;

    // 迭代器的category为单向迭代器
    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    // 指向当前节点
    Node* node;
    // 保存指向hashtable的指针
    hashtable* table;

    // 构造函数
    _hashtable_iterator(Node* n, hashtable* t): node(n), table(t) {

    }

    // 默认构造函数
    _hashtable_iterator() = default;

    // 重载运算符
    // 返回迭代器指向节点的值
    reference operator*() const { return node->data; }
    pointer operator->() const { return &(operator*()); }
    // 前缀
    iterator& operator++();
    // 后缀
    iterator operator++(int);

    // 关系元素符
    bool operator==(const iterator& it) const { return node == it.node; }
    bool operator!=(const iterator& it) const { return !(this->operator==(it)); }
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>&
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++() {
    // 如果当前节点不是list的最后一个节点，那么直接指向list中下一个节点
    if (node->next != nullptr) {
        node = node->next;
    }
    else {
        // 获取迭代器所在list的头节点（也就是在vector上的节点，也可以理解为桶编号，因为一个list可以看成一个桶）
        // bkt_num根据值计算节点的hash值
        size_type bucket_index = table->bkt_num(node->data);
        Node* cur = table->buckets[bucket_index];
        // 移植在vector上遍历，直到桶不为空，那么这个桶的第一个元素就是我们的下一个元素
        while (cur == nullptr && bucket_index < table->buckets.size()) {
            cur = table->buckets[++bucket_index];
        }
        node = cur;
    }
    return *this;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int) {
    iterator tmp = *this;
    operator++();
    return tmp;
}





#endif //STL_MY_ALLOCATOR_MY_HASHTABLE_H
