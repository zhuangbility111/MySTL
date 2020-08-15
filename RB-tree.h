//
// Created by HP on 2020/6/27.
//

#ifndef STL_MY_ALLOCATOR_RB_TREE_H
#define STL_MY_ALLOCATOR_RB_TREE_H

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;       // 红色为0
const __rb_tree_color_type __rb_tree_black = true;      // 黑色为1

// 基类，树节点
// 主要是包含一些基础指针，以及红黑属性
struct __rb_tree_node_base {
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;        // 指向父节点
    base_ptr left;
    base_ptr right;

    // 从x节点的子树中找到最小值
    static base_ptr minimum(base_ptr x) {
        while (x->left != nullptr)
            x = x->left;
        return x;
    }

    // 从x节点的子树中找到最大值
    static base_ptr maximum(base_ptr x) {
        while (x->right != nullptr)
            x = x->right;

        return x;
    }
};

// 子类，继承node_base
// 加入类型模板
// 在子类的基础上加入值value_field
template <class Value>
struct __rb_tree_node : public __rb_tree_node_base {
    typedef __rb_tree_node<Value>* link_type;
    Value value_field;
};


// 底层迭代器，指向底层节点
struct __rb_tree_base_iterator {
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;       // 迭代器类型
    typedef ptrdiff_t difference_type;

    base_ptr node;      //指向底层节点的指针

    // 递增函数
    // 三种情况
    // 1，有右子节点，直接在右子节点的左子树中寻找
    // 2，无右子节点，往左上寻找，直到转折点
    // 2.1，在祖先节点中找到
    // 2.2，在祖先节点找不到，那么直接返回end()
    void increment() {
        // 如果有右子节点，那么下一个数一定在右子节点的左子树中，是该左子树的最小节点
        // 直接往该左子树的左下角移动
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != nullptr)
                node = node->left;
        }

        // 如果没有右子节点，说明只有当前节点被包含在某一个祖先节点的左子树时，这个祖先节点才会大于当前节点
        // 此时这个祖先节点就是下一个节点
        //     p
        //   |   |
        //   x   y
        // 考虑这种情况，设当前节点为x，那么下一个数必定是p
        // 也就是一直往上遍历，直到当前节点为父节点的左子节点时，父节点就是下一个节点
        else {
            base_ptr parent = node->parent;
            // 沿着左上角的方向移动，直到遇到第一个转折点
            // 此时说明下面的子树是转折点的左子树，而x是这棵左子树的最大值，那么下一个数就是转折点
            while (node == parent->right) {
                node = parent;
                parent = parent->parent;
            }
            // 当然如果一直遍历到根节点，都没有转折点，说明x是整棵树的最大值
            // 那么只能返回end()迭代器
            // end()迭代器指向的是header节点
            // header节点是root节点的parent，而root节点也是header节点的parent
            // header节点可以理解为容器结尾的下一个位置
            // header节点的左子节点为整棵树最左的节点
            // header节点的右子节点为整棵树最右的节点

            // 按道理来说应该是直接
            // node = parent;
            // 但是有一种特殊情况需要判断，就是如果root节点就是整棵树的最右节点
            // 那么header->right = root
            // 所以循环到最后，parent = root, node = header;
            // 因此需要加一个判断，当parent != node.right时，才可以node = parent
            if (node->right != parent)
                node = parent;
        }
    }

    // 递减函数
    // 同样三种情况
    // 1，如果本身指向end()，也就是header节点，那么直接返回整棵树的最大值，也就是header的右子节点
    // 2，如果有左子节点，在左子节点的右子树中寻找
    // 3，如果无左子节点，往右上寻找，直到转折点
    void decrement() {
        // header节点
        if (node->color == __rb_tree_red && node->parent->parent)
            node = node->right;
        else if (node->left != nullptr) {
            node = node->left;
            while (node->right != nullptr)
                node = node->right;
        }
        else {
            base_ptr parent = node->parent;
            while (parent->left == node) {
                node = parent;
                parent = parent->parent;
            }
            node = parent;
        }

    }
};

// 上层迭代器，RB_tree真正的迭代器，指向上层节点
// 模板类
template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator {
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;

    typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
    typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr> self;
    typedef __rb_tree_node<Value>* link_type;

    // 构造函数
    __rb_tree_iterator() = default;
    __rb_tree_iterator(link_type x) {
        node = x;
    }
    __rb_tree_iterator(const iterator& it) {
        node = it.node;
    }

    reference operator*() const {
        return link_type(node)->value_field;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        increment();
        return *this;
    }

    self operator++(int) {
        self temp = *this;
        increment();
        return temp;
    }

    self& operator--() {
        decrement();
        return *this;
    }

    self operator--(int) {
        self temp = *this;
        decrement();
        return temp;
    }
};


// RB_tree数据结构
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
class rb_tree {
protected:
    typedef __rb_tree_node_base* base_ptr;          // 底层节点指针
    typedef __rb_tree_node<Value> rb_tree_node;     // 上层节点
    typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;   // 节点专属内存分配器
    typedef __rb_tree_color_type color_type;        // 颜色类型

public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef rb_tree_node* link_type;        // 上层节点指针
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;      // 迭代器差值类型

protected:
    size_type node_count;       // 节点数量
    link_type header;           // header节点，指向根节点
    Compare key_compare;        // 函数对象，主要用来自定义键值比较

    // 分配一个节点的内存
    link_type get_node() { return rb_tree_node_allocator::allocate(); }

    // 释放一个节点的内存
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    // 根据初值构造一个节点
    link_type create_node(const value_type& x) {
        // 申请节点内存
        link_type temp = get_node();
        // 对节点上的value进行构造初始化
        construct(&temp->value_field, x);
        return temp;
    }

    // 拷贝一个节点，值和颜色
    link_type clone_node(link_type x) {
        link_type temp = create_node(x);
        temp->color = x->color;
        temp->left = nullptr;
        temp->right = nullptr;
        return temp;
    }

    // 销毁节点
    void destroy_node(link_type p) {
        // 先析构
        destroy(&p->value_field);
        // 然后再释放内存空间
        put_node(p);
    }

    // 以下三个函数负责获取header的相应指针
    link_type& root() const { return (link_type&) header->parent; }
    link_type& left_most() const { return (link_type&) header->left; }
    link_type& right_most() const { return (link_type&) header->right; }

    // 以下几个函数访问上层节点的相关属性
    // 把对节点的成员对象的访问封装成函数
    // 返回引用的目的是可以让返回值成为左值，通过这些返回值来修改节点上相应的值
    static link_type& left(link_type x) { return (link_type&)(x->left); }
    static link_type& right(link_type x) { return (link_type&)(x->right); }
    static link_type& parent(link_type x) { return (link_type&)(x->parent); }
    static reference value(link_type x) { return x->value_field; }
    static const Key& key(link_type x) {return KeyOfValue()(value(x)); }
    static color_type& color(link_type x) { return (color_type&)(x->color); }

    // 以下几个函数访问下层节点的相关属性
    // 返回引用的目的是可以让返回值成为左值，通过这些返回值来修改节点上相应的值
    static link_type& left(base_ptr x) { return (link_type&)(x->left); }
    static link_type& right(base_ptr x) { return (link_type&)(x->right); }
    static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
    static reference value(base_ptr x) { return ((link_type)x)->value_field; }
    static const Key& key(base_ptr x) {return KeyOfValue()(value(link_type(x))); }
    static color_type& color(base_ptr x) { return (color_type&)(link_type(x)->color); }


    // 求树的极大值和极小值，节点中已经自带了
    static link_type minimum(link_type x) {
        return (link_type) __rb_tree_node_base::minimum(x);
    }

    static link_type maximum(link_type x) {
        return (link_type) __rb_tree_node_base::maximum(x);
    }

public:
    // 迭代器
    typedef __rb_tree_iterator<value_type , reference, pointer > iterator;
    typedef __rb_tree_iterator<value_type , const_reference , const_pointer > const_iterator;

private:
    // 插入节点函数
    iterator __insert(base_ptr x, base_ptr y, const value_type& v);
    // 拷贝节点
    link_type __copy(link_type x, link_type p);
    // 删除某个节点
    void __erase(link_type x);

    // 初始化函数，其实就是初始化header节点
    void init() {
        // 分配内存空间
        header = get_node();

        // 因为没有value对象，所以不需要对value进行构造初始化
        // 只需要设置相关的属性值
        color(header) = __rb_tree_red;
        root() = nullptr;
        left_most() = header;
        right_most() = header;
    }

public:
    // 构造函数/析构函数
    rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) {
        // 对header节点进行初始化
        init();
    }

    ~rb_tree() {
        clear();
        put_node(header);
    }

    // 拷贝赋值运算符
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {

    }

    // 一些数据结构相关的成员函数
    Compare key_comp() const { return key_compare; }
    iterator begin() { return left_most(); }
    iterator end() { return header; }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

    // 将x插入到RB_tree中（保持节点值独一无二）
    pair<iterator, bool> insert_unique(const value_type& x);

    // 将x插入到RB_tree中（允许节点值重复）
    iterator insert_equal(const value_type& x);

    // 查找是否有键值为k的节点
    iterator find(const Key& k);

};

// 允许插入相同key的值
// 其中KeyOfValue是仿函数，可以通过value得到key值
// 这个方法对应map
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type &value) {
    link_type y = header;
    link_type x = root();
    while (x != nullptr) {
        y = x;
        // 将当前节点的key与value对应的key比较
        // 如果小就往右边走，如果大就往左边走
        x = key_compare(KeyOfValue()(value), key(x)) ? left(x) : right(x);
    }
    return __insert(x, y, value);
}

// 不允许插入相同key的值
// 如果插入了相同key的值，会返回包含false的pair对象
// 如果插入成功，会返回包含true的pair对象
// 这个方法对应set
// 因为涉及到相等，而且Compare对象只能判断是大于等于还是小于
// 所以判断value是否已经存在于树中有一定的难度
// 这个方法十分巧妙
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type &v) {
    link_type parent = header;
    link_type cur = root();
    bool comp = true;
    while (cur != nullptr) {
        parent = cur;
        // 如果v对应的key小于cur的key，则返回1
        // 如果v对应的key大于等于cur的key，则返回0
        comp = key_compare(KeyOfValue()(v), key(cur));
        // 如果v对应的key小于cur的key，走左边
        if (comp)
            cur = left(cur);
            // 如果v对应的key大于等于cur的key，走右边
        else
            cur = right(cur);
    }
    iterator j = iterator(parent);
    // 情况1
    // 如果最终v是父节点的左子节点
    if (comp) {
        // 如果v是整棵树的最小值，也就是父节点为begin()
        // 说明v不可能重复，直接插入
        if (j == begin())
            return pair<iterator, bool>(__insert(cur, j, v), true);
        // 如果v不是整棵树的最小值
        // 则需要获得parent的前一个节点（前指的是迭代器的前一个，也就是整棵树中所有小于parent的，并且最靠近parent的那个）
        else
            --j;
    }
    // 情况1：如果v是父节点parent的左子节点，并且v不是整棵树的最小值，那么只要证明 parent-- < v < parent
    // 那么从迭代器的角度来说，v是介于parent--到parent之间，那么在整棵树中必然是独一无二的
    // 所以如果parent-- < v，则直接插入

    // 情况2：另一种情况，如果v是父节点parent的右子节点，说明 v >= parent。j = parent
    // 如果下面证明了 parent < v，那么v必然是唯一的
    // 因为如果存在parent的某个父节点等于v，那么这个父节点必然大于parent
    // 在从上往下遍历的过程中，必然会遇到这个父节点，然后会沿着这个父节点的右边走，那么就肯定不可能遇到parent了（因为parent小于这个父节点）
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return pair<iterator, bool>(__insert(cur, parent, v));

    // 情况1：而如果parent-- >= v，必定有parent-- == v ，无法插入；
    // 因为如果parent-- > v，那么肯定不可能走到parent的下面，遇到parent--就往左边走了
    // 故这种情况是插入失败

    // 情况2：另一种情况，也就是上面无法证明 parent < v，也就是得到了 parent >= v的结论
    // 而本来的条件是 v >= parent，所以综合起来就是 v == parent，存在重复值，所以无法插入
    return pair<iterator, bool>(j, false);

}

// 全局函数
// 新节点必为红节点，如果父节点也为红节点，那么会违反红黑树规则，所以需要做旋转，x为旋转节点
// 这是左旋转
inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    __rb_tree_node_base* new_root = x->right;
    x->right = new_root->left;
    if (new_root->left != nullptr)
        new_root->left->parent = x;
    // 如果x是根节点，则需要更新new_root为根节点
    if (x == root)
        root = new_root;
    if (x == x->parent->left)
        x->parent->left = new_root;
    else if (x == x->parent->right)
        x->parent->right = new_root;
    new_root->parent = x->parent;
    x->parent = new_root;
    new_root->left = x;

}

// 全局函数
// 新节点必为红节点，如果父节点也为红节点，那么会违反红黑树规则，所以需要做旋转，x为旋转节点
// 这是右旋转
inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    __rb_tree_node_base* new_root = x->left;
    x->left = new_root->right;
    if (new_root->right != nullptr)
        new_root->right->parent = x;
    // 如果x是根节点，则需要更新new_root为根节点
    if (x == root)
        root = new_root;
    if (x->parent->left == x)
        x->parent->left = new_root;
    else if (x->parent->right == x)
        x->parent->right = new_root;
    new_root->parent = x->parent;
    x->parent = new_root;
    new_root->right = x;
}

// 全局函数
// 插入节点之后的重平衡函数
inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
    // 新节点颜色为红色
    x->color = __rb_tree_red;
    // 如果当前节点不为根节点，并且父节点也为红色，则违反两个红色节点不能相邻的规则
    // 需要继续往上进行调整，直到父节点不为红色，或者父节点为根节点为止
    while (x != root && x->parent->color == __rb_tree_red) {
        // 如果当前节点位于祖父节点的左子树中
        if (x->parent == x->parent->parent->left) {
            // 叔叔节点
            __rb_tree_node_base* uncle = x->parent->parent->right;
            // 如果叔叔节点为红色，父亲节点为红色
            // 那么直接让叔叔节点和父亲节点变为黑色，爷爷节点设置为红色
            // 然后如果爷爷节点的父亲为红色，继续往上调整；
            // 如果爷爷节点的父亲为黑色，则调整完毕
            if (uncle != nullptr && uncle->color == __rb_tree_red) {
                x->parent->color = __rb_tree_black;
                uncle->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                // 将x设置为爷爷节点，如果爷爷节点的父亲节点为红色，则继续调整
                x = x->parent->parent;
            }
            // 否则如果叔叔节点为空（空也可以理解为黑色），或者为黑色
            // 则需要进行右旋转，将一个红色的节点移动到爷爷节点的右子树中
            // 此时爷爷节点的左子树少了一个红色节点，右子树多了一个红色节点
            // 但是两棵子树的黑色节点数目没有变化，所以调整之后满足规则
            else {
                // 如果x是父亲节点的右子节点，则需要先左旋调整
                if (x == x->parent->right) {
                    // 旋转节点为父亲节点
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);
                }
                // 然后进行以爷爷节点为旋转节点进行右旋转
                // 对颜色先进行调整，父亲节点作为新的根节点，需要变为黑色；
                // 而爷爷节点作为新的根节点的右子节点，则需要变为红色
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_right(x->parent->parent, root);
            }
        }
        // 如果当前节点位于祖父节点的右子树中
        else if (x->parent == x->parent->parent->right) {
            // 父亲节点为红色，如果叔叔节点也为红色，那么直接将父亲节点和叔叔节点变为黑色，爷爷节点变为红色
            // 然后如果爷爷节点的父亲也为红色，则继续往上调整
            // 叔叔节点
            __rb_tree_node_base* uncle = x->parent->parent->left;
            if (uncle != nullptr && uncle->color == __rb_tree_red) {
                uncle->color = __rb_tree_black;
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                x = x->parent->parent;
            }
            // 否则如果叔叔节点为空（空也可以理解为黑色），或者为黑色
            // 则需要进行左旋转，将一个红色的节点移动到爷爷节点的左子树中
            // 此时爷爷节点的右子树少了一个红色节点，左子树多了一个红色节点
            // 但是两棵子树的黑色节点数目没有变化，所以调整之后满足规则
            else {
                // 如果x是父亲节点的左子节点，则需要先右旋调整
                if (x == x->parent->left) {
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }

    // 如果一直往上调整，直到x为根节点时，直接无脑将根节点设置为黑色，因为直接设置根节点为黑色不违反任何一个规则
    root->color = __rb_tree_black;
}


// 插入节点
// x为新值的插入点，y为插入点的父节点，v为新值
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::base_ptr x_,
                                                          rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::base_ptr y_,
                                                          const value_type &v) {
    link_type x = (link_type)x_;
    link_type y = (link_type)y_;
    link_type z = create_node(v);
    // 先插入节点，再调整平衡
    // 如果y是header，那么说明整棵树是空的
    // 那么根节点就为z
    if (y == header) {
        left_most() = z;
        right_most() = z;
        root() = z;
    }
    // 如果y不是header，并且v小于y，y是最左节点
    // 那么需要更新最左节点为待插入节点
    else if (y != header && key_compare(KeyOfValue()(v), key(y))) {
        left(y) = z;
        if (y == left_most()) {
            left_most() = z;
        }
    }
    // 否则如果y不是header，v大于等于y，则插入到y的右边
    else if (y != header && !key_compare(KeyOfValue()(v), key(y))) {
        right(y) = z;
        if (y == right_most()) {
            right_most() = z;
        }
    }
    parent(z) = y;
    left(z) = nullptr;
    right(z) = nullptr;

    __rb_tree_rebalance(z, root());
    ++node_count;
    return iterator(z);
}

// 查找结点
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key &k) {
    // 因为Compare只能判断key(cur) < k，还是 key(cur) >= k
    // 所以每次key(cur) >= k时，设置一个指针y保存目标位置，使得 key(y) >= k
    // 直到遍历完毕时，再判断 k 是否 >= key(y)
    // 如果满足，说明 k == key
    // 如果 k < key，说明 k != key

    // 首先如果树中确实存在节点，等于k，那么y必定指向这个key为k的节点
    // 因为到达这个节点后，key >= k满足，则会往这个节点的左边移动，那么在这个节点的左子树遍历时
    // 所有节点的值必然是小于key的，也就不存在key >= k的节点，所以y不会变化，所以y必定指向这个key为k的节点
    link_type y = header;
    link_type cur = root();
    while (cur != nullptr) {
        // key(cur) < k 时 key_compare返回true
        // 所以!key_compare(key(cur), k)也就是key(cur) >= k
        if (!key_compare(key(cur), k)) {
            y = cur;
            cur = left(cur);
        }
        // key(cur) < k 时
        else
            cur = right(cur);
        iterator j = iterator(y);
        // 如果y没有变化（也就是y = header = end()），或者 k < key(y)
        // y没有变化说明key(cur)一直 < k，也就是树中的所有节点均小于k，那么必定没有与k相等的节点
        // 说明找不到
        if (j == end() || key_compare(k, key(j)))
            return end();
        else
            return j;
    }
}


#endif //STL_MY_ALLOCATOR_RB_TREE_H
