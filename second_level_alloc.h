//
// Created by 54969 on 2020/3/12.
//

#ifndef STL_MY_ALLOCATOR_SECOND_LEVEL_ALLOC_H
#define STL_MY_ALLOCATOR_SECOND_LEVEL_ALLOC_H

#include <new>
#include "first_level_alloc.h"

namespace my_std {
    /*
 * 第二级分配器，主要是负责管理小于128Bytes的内存块
 * 主要是由一个链表和一个内存池构成
 * 链表上主要是各种规格的内存块（8B，16B……），而内存池就是一大块
 * 一般的分配和释放，都只是在链表上进行操作。分配则将内存块（本来就已经申请好的）从链表上脱离下来
 * 释放则将内存块重新连接到链表上。所以如果链表上的内存块足以进行分配时，这些分配和释放的操作都不涉及到malloc和free
 * 也就是不与操作系统打交道，只是将内存块放到链表上，或者将内存块从链表上脱离下来。这些内存块是已经malloc到内存池，然后从内存池上拿下来的
 * 只有内存池负责使用malloc和free，与操作系统打交道，申请堆区的内存空间
 * 申请到的内存块会放在内存池上。如果链表上的内存块不够时，则将内存池上的内存块连接到链表上
 * 所以可以理解为：链表上的内存块与用户的申请和释放内存打交道，而内存池上的内存块则是实打实地与操作系统打交道
 * 另一种说法：自由链表负责和用户打交道（块大小的调整，块的分配和回收），内存池负责和操作系统打交道（包括在内存不足时调用上一级空间配置器和使用OOM机制），这样就实现了职责分离
 * 链表只是一种对内存池里的内存块的管理方式
 */

// 匿名枚举，等价于静态常量const static int __ALIGN = 8;
    const static int __ALIGN = 8;     // 小型内存块的上调边界，也就是每一个内存块必须是__ALIGN的倍数
    const static int __MAX_BYTES = 128;       // 小型内存块的上限，也就是内存块的最大尺寸
    const static int __NFREELISTS = __MAX_BYTES / __ALIGN;        // free-lists的个数，也就是总共有多少种不同大小的内存块，每一种内存块都有一个链表free-list负责管理

// 第二级分配器，模板没有类型参数
// 两个都是变量参数。第一参数用于多线程环境，第二参数没有派上用场
    template <bool threads, int inst>
    class __default_alloc_template {

    private:
        // 主要是负责将bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes) {
            return (((bytes) + __ALIGN-1) & ~(__ALIGN-1));      // 这个处理很巧妙，加7再与7取反后的结果&，相当于将7对应的二进制位置为0
        }

        // 这个结构体是free-lists的节点结构，其中free_list_link指向下一个obj节点
        // 每一个内存块上都有一个obj结构，用于保存下一个内存块的起始地址
        // 如果这个内存块分配给用户使用，也就是从free-list上脱离，那么obj这个结构就不需要使用，用户直接覆盖
        // 如果这个内存块释放，也就是重新连接到free-list上，则从内存块上开辟一部分来保存obj结构，也就是保存下一节点的地址
        // 原来的实现版本
//    union obj {
//        union obj * free_list_link;
//        char client_data[1];
//    };
        struct obj {
            struct obj * free_list_link;
        };

        // 因为有16种大小不同的内存块，所以有16个free-lists。1个free-lists保存一种大小的内存块
        static obj * volatile free_list[__NFREELISTS];

        // 根据内存块的大小，选择使用第n号free-list。也就是根据内存块的大小决定哪一条链表
        static size_t FREELIST_INDEX(size_t bytes) {
            return (((bytes) + __ALIGN-1) / __ALIGN - 1);   // 这个处理也是很巧妙，加7除以8再减1，可以得到内存块对应的链表的标号
        }

        // 返回一个大小为n的内存块，并且有可能将大小为n的其他内存块加入到free-list中
        static void *refill(size_t n);

        // 主要负责从内存池中申请一大块内存块到free-list上。这个内存块的大小为size*n_objs
        // 如果无法成功申请n_objs个内存块，可能会减少申请内存块的个数
        // 如果内存池中空间大小不够，则需要使用malloc向系统申请更多的内存块，增加内存池的大小
        static char *chunk_alloc(size_t size, int &n_objs);

        // 内存池状态
        static char *start_free;    // 内存池起始地址。只在chunk_alloc()中进行变化
        static char *end_free;      // 内存池结束地址。只在chunk_alloc()中进行变化
        static size_t heap_size;    // 堆的大小

    public:
        // 分配内存空间
        static void* allocate(size_t n) {
            obj *result;
            // 保存所需要的内存块的链表的起始指针
            obj * volatile * my_free_list;

            // 如果所需要申请的内存空间大小大于128Bytes，调用第一级分配器
            if (n > (size_t) __MAX_BYTES) {
                return __malloc_alloc_template<inst>::allocate(n);
            }

            // 获取这个大小的内存块对应的链表头
            my_free_list = free_list + FREELIST_INDEX(n);
            // 将result指向第一个链表的第一个内存块
            result = *my_free_list;
            // 如果指针不为空，说明有多余的内存块，直接返回这个内存块的地址，并调整链表
            if (result != NULL) {
                // 调整free-list
                // *my_free_list实际上就是得到数组中的指针，也就是指向链表第一个节点的指针
                // 将这个指针指向result的下一个节点，也就是下一个空闲的内存块
                // 同时返回result指向的内存块，让它直接从链表上脱离下来
                *my_free_list = result->free_list_link;
                return (void*)result;
            }
            // 如果指针为空，说明链表中没有空闲的内存块，则需要使用refill()向内存池中索取内存块，重新填充freelist
            void *r = refill(ROUND_UP(n));
            return r;
        }

        // 回收内存空间
        static void deallocate(void *p, size_t n) {
            obj *q = (obj*)p;
            // 指针的指针，通过他指向保存在数组中的free-list的头节点
            obj* volatile * my_free_list;

            // 如果大于128，就调用第一级内存分配器
            if (n > (size_t)__MAX_BYTES) {
                __malloc_alloc_template<inst>::deallocate(p, n);
                return;
            }

            // 获取指向free-list链表头的指针
            my_free_list = free_list + FREELIST_INDEX(n);
            // 调整free-list
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }

        // 重新分配内存
        static void* reallocate(void* p, size_t old_size, size_t new_size);
    };

// 对分配器的相关成员进行初始化
    template <bool threads, int inst>
    char *__default_alloc_template<threads, inst>::start_free = 0;

    template <bool threads, int inst>
    char *__default_alloc_template<threads, inst>::end_free = 0;

    template <bool threads, int inst>
    size_t __default_alloc_template<threads, inst>::heap_size = 0;

// 16种大小的内存块，所以有16个free-list。free-list[i]表示第i条链表的起始指针
    template <bool threads, int inst>
    typename __default_alloc_template<threads, inst>::obj * volatile
            __default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// 向内存池中索取内存块
// 返回一个大小为n的内存块，同时有可能会给free-list上添加数个大小为n的内存块
    template <bool threads, int inst>
    void* __default_alloc_template<threads, inst>::refill(size_t n) {
        // 默认向内存池申请20个大小为n的内存块
        int n_objs = 20;
        // chunk为向内存池申请的内存块的地址
        // n_objs为引用，所以函数返回后，n_objs为实际返回的内存块数量
        char* chunk = chunk_alloc(n, n_objs);

        // 指向free-list的头节点
        obj* volatile* my_free_list;
        obj* result;
        obj* cur_obj, *next_obj;

        // 如果返回的内存块数量为1，说明内存池中没有多余的块可以返回了，则直接将这个返回的内存块给用户
        if (n_objs == 1) {
            result = (obj*) chunk;
            return result;
        }
        // 否则准备调整free list
        my_free_list = free_list + FREELIST_INDEX(n);

        // 如果返回的内存块数量大于1，说明内存池中有多余的块，这些多余的内存块可以添加到free-list中
        // 将第一块内存块作为result返回
        result = (obj*) chunk;

        // 然后将剩下的内存块添加到free-list中
        // 实际上一个块的大小为n Bytes，而obj的大小为4 Bytes（32位系统的地址有32位，也就是4 Bytes）
        // 这是一个很巧妙的设计。需要链接到链表时，从内存块上抠出前4 Bytes作为连接指针和连接对象
        // 不需要时，则整一个块返回给用户使用，前4 Bytes同样是普通的内存空间
        // 所以实际上是在一个块上划分了前4 Bytes作为obj对象，或者说将前4 Bytes解释为obj对象
        // 但是实际上需要定位到下一个块的起始地址，还是需要将指针转换成char*（移动一次代表向后移动一个Bytes单位）
        // 然后移动n次，相当于移动n个Bytes
        // chunk原来就是char*，所以可以直接+n，定位到下一个块的起始地址
        // obj中的指针指向的是下一个块的起始地址，也就是指向下一个块中前4 Bytes组成的obj对象的起始地址
        // 也就是在内存池返回的这一大块内存中，除去第一块需要返回给用户，剩下的一大块需要组织成链表free-list
        *my_free_list = (obj*)(chunk + n);
        next_obj = (obj*)(chunk + n);
        for (int i = 1; i < n_objs; i++) {
            cur_obj = next_obj;
            next_obj = (obj*)((char*)cur_obj + n);
            // 此时cur_obj指向最后一块
            if (i+1 == n_objs) {
                cur_obj->free_list_link = NULL;
            }
                // 否则，将当前块的指针指向下一个块，链接起来
            else {
                cur_obj->free_list_link = next_obj;
            }
        }
        return (void*)result;

    }

// 从内存池中取内存块到free-list中
// 如果内存池中的内存不够了，则需要使用malloc向操作系统申请
// 内存池的总大小是通过起始地址和终止地址的差值end_free - start_free得到的
    template <bool threads, int inst>
    char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &n_objs) {
        size_t total_bytes = size * n_objs;     // free-list申请的内存块的总大小
        size_t bytes_left = end_free - start_free;      // 内存池剩余的内存空间
        char *result;
        // 如果内存池中剩余的内存空间大于free-list申请的内存块，也就是内存池还有剩余
        // 则将相应的内存块返回给free-list，并更新内存池的大小
        if (bytes_left >= total_bytes) {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
            // 否则如果内存池中剩余的内存大小无法完全满足需求量，但是足够供应一个或一个以上的内存块
            // 则返回尽可能多的内存块，更新n_objs的值
            // 因为n_objs是引用，所以在free-list可以得到具体返回的内存块数
        else if (bytes_left >= size) {
            n_objs = bytes_left / size;
            result = start_free;
            start_free += n_objs*size;
            return result;
        }
            // 如果内存池中连一个内存块的大小都无法提供，需要使用malloc向操作系统申请内存空间
        else {
            // 首先如果内存池中还有一些残余的内存，将他们分配到适当的free-list中
            // 这些内存是对齐8Bytes的，对内存池的大小进行改动时，也是以8的倍数进行改动
            // 所以剩余的内存空间大小必然是8Bytes的倍数，它也就必然是一个大小刚好为(n*8Bytes)的内存块
            // 不需要担心出现例如10Bytes这样的内存空间被当作16Bytes的内存块放入free-list中
            if (bytes_left > 0) {
                obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                // 将其插入对应的free-list中
                ((obj*) start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*) start_free;
            }

            // 分配完后，内存池的大小为0。需要使用malloc向操作系统申请更多的内存空间放入到内存池中
            // 这里的heap_size是一个动态值，随着申请内存次数的增加，heap_size也不断增加
            // 满足越来越大的内存需求
            size_t bytes_to_get = 2*total_bytes + ROUND_UP(heap_size>>4);
            start_free = (char*) malloc(bytes_to_get);
            // 如果堆空间不足，malloc()失败，无法申请内存空间
            if (start_free == nullptr) {
                obj* volatile *my_free_list, *p;
                // 则遍历free-lists，对free-lists进行调整
                // 如果数组链表中有比size更大的内存块，说明这些内存块可以拆分之后返回给用户
                // 那么先将这些内存块回收到内存池中
                // 然后递归调用chunk_alloc()让内存池重新分配新增加的内存，分配合适的内存块给free-list
                // 并将其中一块内存块返回给用户
                for (int i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != NULL) {
                        // 将free-list中的第一块内存块还给内存池
                        // 将下一块内存块作为free-list新的头节点
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        // 递归调用chunk_alloc()让内存池重新分配新增加的内存，分配合适的内存块给free-list和用户
                        return chunk_alloc(size, n_objs);
                    }
                }
                // 如果遍历完free-list后，发现真的一块内存块都不剩了
                // 那么调用第一级分配器，看看 out of memory 机制是否能尽点力
                // 如果第一级分配器也无能为力，会抛出异常
                end_free = nullptr;
                start_free = (char*)__malloc_alloc_template<inst>::allocate(size);
            }
            // 如果执行到这里，说明第一级分配器的分配奏效了，或者malloc申请内存成功了，更新内存池的大小
            // 更新动态值heap_size
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            // 递归调用chunk_alloc()，重新调整内存池，并分配内存块给用户
            return chunk_alloc(size, n_objs);
        }
    }
}


#endif //STL_MY_ALLOCATOR_SECOND_LEVEL_ALLOC_H
