//
// Created by 54969 on 2020/10/8.
//

#ifndef MY_STL_ALLOCATOR_MY_TYPE_TRAITS_H
#define MY_STL_ALLOCATOR_MY_TYPE_TRAITS_H

/*
 * 自制的__type_traits，通过__type_traits可以确定一个类型的ctor/dtor/copy/assignment函数是否是trivial的
 * 先通过模板结构体将所有类都设置成ctor/dtor/copy/assignment函数都不是trivial的
 * 接着使用模板特例化，将所有POD类型都设置成ctor/dtor/copy/assignment函数都是trivial的
 * 调用函数时通过__true_type还是__false_type来决定调用的底层函数
 */

namespace my_std {
    struct __true_type {};      // 无意义的构造函数
    struct __false_type {};     // 有意义的构造函数

    // 所有类都默认不是trivial的
    template <class T>
    struct __type_traits {
        typedef __false_type has_trivial_default_constructor;       //默认构造函数是否有意义？
        typedef __false_type has_trivial_copy_constructor;          //拷贝构造函数是否有意义?
        typedef __false_type has_trivial_assgignment_constructor;   //拷贝赋值操作是否有意义?
        typedef __false_type has_trivial_destructor;                //析构函数是否有意义?
        /*POD意指Plain Old Data,也就是标量型别或传统的C struct(传统的C struct只能
        包含数据成员，不能包含函数成员。也就是所谓的聚合类。POD型别必然包含无意义
        的ctor/dtor/copy/assignment函数。
        */
        typedef __false_type is_POD_type;                           //是否为Plain Old Data?

    };

    // 模板特例化，将所有POD类型都设置成ctor/dtor/copy/assignment函数都是trivial的

    #ifndef _STL_TEMPLATE_NULL
    #define _STL_TEMPLATE_NULL template<>
    #endif

    _STL_TEMPLATE_NULL
    struct __type_traits<char> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<unsigned char> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<short> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<unsigned short> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<int> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<unsigned int> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<float> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<double> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    _STL_TEMPLATE_NULL
    struct __type_traits<long double> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };

    // 所有类的指针类型都属于POD类型，所以也需要针对指针类型进行模板特例化
    template <class T>
    struct __type_traits<T*> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assgignment_constructor;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;

    };


}


#endif //MY_STL_ALLOCATOR_MY_TYPE_TRAITS_H
