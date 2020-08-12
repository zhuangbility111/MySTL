//
// Created by HP on 2020/7/22.
//

#ifndef STL_MY_ALLOCATOR_MY_HASHSET_H
#define STL_MY_ALLOCATOR_MY_HASHSET_H

#include "my_hashtable.h"

template <class Value, class HashFcn = hash<Value>, class EqualKey = equal_to<Value>, class Alloc = alloc>
class hash_set {
private:
    typedef hashtable<Value, Value, HashFcn, Value, EqualKey, Alloc> hash_table;

    // 以hashtable作为底层容器
    hash_table rep;

public:
    typedef typename hash_table::size_type size_type;
    typedef typename hash_table::key_type key_type;
    typedef typename hash_table::value_type value_type;
    typedef typename hash_table::hasher hasher;
    typedef typename hash_table::key_equal key_equal;

    typedef typename hash_table::iterator iterator;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

    // 构造函数
    // 默认构造函数，默认设置hashtable大小为100，hash函数和key_equal函数都使用默认的
    hash_set() : rep(100, hasher(), key_equal()) { }

    // 指定n的构造函数，hash函数和key_equal函数都使用默认的
    explicit hash_set(size_type n) : rep(n, hasher(), key_equal()) { }

    // 指定n和hash函数的构造函数
    hash_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) { }

    // 指定n和hash、key_equal函数的构造函数
    hash_set(size_type n, const hasher& hf, const key_equal& ke) : rep(n, hf, ke) { }

    // 指定输入的迭代器范围，使用insert实现
    // hash_table使用默认的参数调用构造函数来初始化
    template <class InputIterator>
    hash_set(InputIterator first, InputIterator last): rep(100, hasher(), key_equal()) {
        rep.insert_unique(first, last);
    }

    // 指定n和hash函数的构造函数
    template <class InputIterator>
    hash_set(InputIterator first, InputIterator last, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_unique(first, last);
    }

    // 指定n和hash、key_equal函数的构造函数
    template <class InputIterator>
    hash_set(InputIterator first, InputIterator last, size_type n, const hasher& hf, const key_equal& ke) : rep(n, hf, ke) {
        rep.insert_unique(first, last);
    }

public:
    friend bool operator== (const hash_set&, const hash_set&);

    size_type size() const {
        return rep.size();
    }

    size_type max_size() const {
        return rep.max_size();
    }

    bool empty() const {
        return rep.empty();
    }

    iterator begin() const {
        return rep.begin();
    }

    iterator end() const {
        return rep.end();
    }

public:
    pair<iterator, bool> insert(const value_type& obj) {
        pair<typename hash_table::iterator, bool> p = rep.insert_unique(obj);
        return pair<iterator, bool>(p.first, p.second);
    }

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        rep.insert_unique(first, last);
    }

    pair<iterator, bool> insert_noresize(const value_type& obj) {
        pair<typename hash_table::iterator, bool> p = rep.insert_unique_noresize(obj);
        return pair<iterator, bool>(p.first, p.second);
    }

    iterator find(const key_type& key) const {
        return rep.find(key);
    }

    size_type count(const key_type& key) const {
        return rep.count(key);
    }

    size_type erase(const key_type& key) {
        return rep.erase(key);
    }

    void erase(iterator it) {
        rep.erase(it);
    }

    void erase(iterator first, iterator last) {
        return rep.erase(first, last);
    }

    void clear() {
        rep.clear();
    }

public:
    void resize(size_type n) {
        rep.resize(n);
    }

    size_type bucket_count() const {
        return rep.bucket_count();
    }

    size_type max_bucket_count() const {
        return rep.max_bucket_size();
    };

    size_type elems_in_bucket(size_type n) const {
        return rep.elems_in_bucket(n);
    }

};

template <class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator==(const hash_set<Value, HashFcn, EqualKey, Alloc>& hs1,
                        const hash_set<Value, HashFcn, EqualKey, Alloc>& hs2) {
    return hs1.rep == hs2.rep;
}

#endif //STL_MY_ALLOCATOR_MY_HASHSET_H
