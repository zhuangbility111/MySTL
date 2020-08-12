//
// Created by HP on 2020/7/31.
//

#ifndef STL_MY_ALLOCATOR_MY_HASHMAP_H
#define STL_MY_ALLOCATOR_MY_HASHMAP_H

#include "my_hashtable.h"


template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>, class Alloc = alloc>
class hash_map {

private:
    // value变成了pair对，key还是原来的key
//    typedef hashtable<pair<const Key, T>, Key, HashFcn, selectlst<pair<const Key, T> >, EqualKey, Alloc> hash_table;
    typedef hashtable<pair<const Key, T>, Key, HashFcn,  T, EqualKey, Alloc> hash_table;
    hash_table rep;


public:
    typedef typename hash_table::key_type key_type;
    typedef typename hash_table::value_type value_type;
    typedef typename hash_table::hasher hasher;
    typedef typename hash_table::key_equal key_equal;

    typedef typename hash_table::size_type size_type;

    // hash_map的迭代器实际上用的就是hash_table中的迭代器
    typedef typename hash_table::iterator iterator;
    typedef typename hash_table::const_iterator const_iterator;

    // 返回相应的函数对象
    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    // 构造函数
    // 缺省使用大小为100的表格，将由hash table 自动调整为最接近且较大的质数
    // 默认构造函数
    hash_map() : rep(100, hasher(), key_equal()) {}
    // 指定n大小的构造函数
    explicit hash_map(size_type n) : rep(n, hasher(), key_equal()) {}
    // 指定n大小和hash函数对象的构造函数
    hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    // 指定n大小、hash函数对象和key_equal对象的构造函数
    hash_map(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}

    // 指定迭代器范围来构造hash_map
    template <class InputIterator>
    hash_map(InputIterator first, InputIterator last) : rep(100, hasher(), key_equal()){
        // 键值不允许重复，因此使用insert_unique
        rep.insert_unique(first, last);
    }

    // 指定迭代器范围，以及初始大小n
    template <class InputIterator>
    hash_map(InputIterator first, InputIterator last, size_type n) : rep(n, hasher(), key_equal()){
        // 键值不允许重复，因此使用insert_unique
        rep.insert_unique(first, last);
    }

    // 指定迭代器范围、初始大小n，以及hash函数对象
    template <class InputIterator>
    hash_map(InputIterator first, InputIterator last, size_type n, const hasher& hf) : rep(n, hf, key_equal()){
        // 键值不允许重复，因此使用insert_unique
        rep.insert_unique(first, last);
    }

    // 指定迭代器范围、初始大小n，以及hash函数对象、key_equal对象
    template <class InputIterator>
    hash_map(InputIterator first, InputIterator last, size_type n, const hasher& hf, const key_equal& eql)
        :rep(n, hf, eql){
        // 键值不允许重复，因此使用insert_unique
        rep.insert_unique(first, last);
    }

public:
    // 操作接口，基本都依靠底层hash_table的操作实现
    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(hash_map& hs) { rep.swap(hs.rep); }

    friend bool operator==(const hash_map&, const hash_map&);

    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }

    // 返回const_iterator 通过const成员函数实现
    // 因为指针指向的是const对象，内部的成员变量无法修改，从而保证返回的iterator无法修改对象
    const_iterator begin() const { return rep.begin(); }
    const_iterator end() const { return rep.end(); }

public:
    // 各种版本的插入函数
    // 其实这个value_type就是pair类型，也就是一个pair的键值对
    pair<iterator, bool> insert(const value_type& obj) {
        return rep.insert_unique(obj);
    }

    // 将迭代器范围内的元素插入hash_mapz中
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        rep.insert_unique(first, last);
    }

    pair<iterator, bool> insert_noresize(const value_type& obj) {
        return rep.insert_unique_noresize(obj);
    }

    // find函数，返回迭代器
    iterator find(const key_type& key) { return rep.find(key); }
    // 通过指定指针指向常量对象来保证返回的迭代器是const的
    const_iterator find(const key_type& key) const { return rep.find(key); }

    // hash_map独特的[]运算符，如果index存在，则返回该index对应的T value
    // 同时，map[index]可以作为左值，也就是可以通过map[index] = 1这种形式赋值
    // 要实现这种方式的赋值，则这个函数需要返回引用。通过返回引用使得返回值可以作为左值使用
    // 如果key在map中不存在，那么应该是插入一个value（也就是pair对）进去，然后返回pair对的第二个值，也就是T value的引用
    // 所以内部实现是返回pair.second
    T& operator[](const key_type& key) {
        return rep.find_or_insert(value_type(key, T())).second;
    }


    // 返回key对应的元素个数
    // 但是因为map中一个key只能对应一个元素，所以实际上返回值只能是0、1
    // 一个key只能对应一个元素是通过插入时调用的insert_unique来保证的
    size_type count(const key_type& key) {
        return rep.count(key);
    }

    // 返回等于key的迭代器范围
    // 实际上这个范围最多只能有一个迭代器
    pair<iterator, iterator> equal_range(const key_type& key) {
        return rep.equal_range(key);
    }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return rep.equal_range(key);
    }

    // erase函数
    size_type erase(const key_type& key) {
        return rep.erase(key);
    }

    void erase(iterator it) {
        rep.erase(it);
    }

    void erase(iterator first, iterator last) {
        rep.erase(first, last);
    }

    void clear() {
        rep.clear();
    }

public:
    void resize(size_type n) { rep.resize(n); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_size(); }
    // 返回某个桶（这个桶的索引为n）中元素的个数
    size_type elems_in_bucket(size_type n) const {
        return rep.elems_in_bucket(n);
    }

};

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>, class Alloc = alloc>
inline bool operator==(const hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm1,
                        const hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm2) {
    return hm1.rep == hm2.rep;
}




#endif //STL_MY_ALLOCATOR_MY_HASHMAP_H
