#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include <stdarg.h>
#include <mutex>

using namespace std;
///////////////////// C 语言实现 /////////////////////////////////

#define VM_START 0
#define VM_END 0x400

struct rb_root
{
    struct rb_node* rb_node;
};

struct rb_node
{
    struct rb_node* rb_right;
    struct rb_node* rb_left;
};
struct vm_struct_area
{
    unsigned long  start;
    unsigned long  end;
    struct rb_node rb_node;
    // 二叉树用于 [给定地址]->[VMA搜索点] 的快速定位
    // 在没有指定起始地址的条件下，二叉树无法优化搜索
    vm_struct_area* next;
    // 单链表用于 从[VMA搜索点]启动的遍历
};

#define RB_ROOT        \
    ( struct rb_root ) \
    {                  \
        NULL           \
    }
#define ALIGN_SIZE 8
#define VMA_ALIGN( addr ) ( ( addr + ( ALIGN_SIZE - 1 ) ) & ~( ALIGN_SIZE - 1 ) )
#define RB_EMPTY_ROOT( root ) ( ( root )->rb_node == NULL )
#define RB_EMPTY_NODE( node ) ( ( node )->rb_left == NULL && ( node )->rb_right == NULL )
#define RB_ENTRY( ptr, type, member ) ( ( type* )( ( char* )( ptr ) - offsetof( type, member ) ) )

struct vm_struct_area* g_vm_head = nullptr;
struct rb_root         g_vm_root = RB_ROOT;
std::mutex             g_vm_mutex;  // 用于保护全局虚拟内存区域链表的互斥锁

unsigned long vma_size( struct vm_struct_area* vma )
{
    if ( !vma )
    {
        return 0;
    }
    unsigned long size = 1;
    while ( vma->next )
    {
        vma = vma->next;
        size++;
    }
    return size;
}

void __rb_insert_node( struct rb_node** link, struct rb_node* node )
{
    node->rb_left  = NULL;
    node->rb_right = NULL;
    *link          = node;
}
void __rb_insert_list( struct vm_struct_area* vma, struct rb_node* rb_pre, struct rb_node* rb_parent )
{
    if ( rb_pre )
    {
        struct vm_struct_area* vma_pre = RB_ENTRY( rb_pre, vm_struct_area, rb_node );
        vma->next                      = vma_pre->next;
        vma_pre->next                  = vma;
    }
    else
    {
        // 新插入的节点将成为 HEAD 节点
        // 父节点 一定是 叶子节点的 前节点[pre] 或者 [后节点 next]
        if ( rb_parent )
        {
            struct vm_struct_area* vma_parent = RB_ENTRY( rb_parent, vm_struct_area, rb_node );
            vma->next                         = vma_parent;
        }
        else
            vma->next = nullptr;
        g_vm_head = vma;  // 如果是第一个节点，则更新全局头指针
    }
}
// insert 前必须保证 vma 范围的有效性(由 get_unmaped_area 来保证)
void rb_insert_vm( struct rb_root* root, struct vm_struct_area* vma )
{
    struct rb_node** rb_link;
    struct rb_node*  rb_parent = NULL;
    struct rb_node*  rb_pre    = NULL;
    unsigned long    addr      = vma->start;
    g_vm_mutex.lock();
    rb_link = &root->rb_node;
    while ( *rb_link )
    {
        rb_parent                      = *rb_link;
        struct vm_struct_area* vma_tmp = RB_ENTRY( rb_parent, vm_struct_area, rb_node );
        if ( addr >= vma_tmp->end )  // 包含相等
        {
            rb_pre  = rb_parent;
            rb_link = &rb_parent->rb_right;
        }
        else
        {
            rb_link = &rb_parent->rb_left;
        }
    }
    __rb_insert_node( rb_link, &vma->rb_node );
    __rb_insert_list( vma, rb_pre, rb_parent );
    g_vm_mutex.unlock();
}
// 分配虚拟地址空间 并且 返回一个新的 vm_struct_area 结构体
struct vm_struct_area* get_unmaped_area( unsigned long size )
{
    unsigned long          addr = VM_START;
    unsigned long          end  = VM_END;
    struct vm_struct_area* vma  = nullptr;
    size                        = VMA_ALIGN( size );
    g_vm_mutex.lock();  // 锁定全局虚拟内存区域链表
    rb_node** rb_link = &g_vm_root.rb_node;
    while ( *rb_link )
    {
        struct vm_struct_area* vma_tmp = RB_ENTRY( *rb_link, vm_struct_area, rb_node );
        if ( vma_tmp->end > addr )
        {
            vma = vma_tmp;
            if ( vma->start >= addr )  // 快速查找满足条件的起始 vma(仅仅负责查找起始搜索点)
                break;
            rb_link = &( *rb_link )->rb_left;
        }
        else
        {
            rb_link = &( *rb_link )->rb_right;
        }
    }
    for ( ; vma; vma = vma->next )  // 进行单链表遍历(负责分配虚拟地址空间)
    {
        if ( addr + size <= vma->start )
            break;
        addr = VMA_ALIGN( vma->end );
    }
    g_vm_mutex.unlock();  // 解锁全局虚拟内存区域链表
    if ( addr + size > end )
        return NULL;           // 没有足够的空间
    if ( addr + size < addr )  // 地址/长度 溢出检查
        return NULL;
    vma = ( struct vm_struct_area* )malloc( sizeof( struct vm_struct_area ) );
    memset( vma, 0x00, sizeof( struct vm_struct_area ) );
    vma->start = addr;
    vma->end   = addr + size;
    return vma;
}

int main()
{
    srand( time( nullptr ) );
    for ( int i = 0;; ++i )
    {
        unsigned long          size = random() % 0x40;
        struct vm_struct_area* vma  = get_unmaped_area( size );
        if ( !vma )
            break;
        rb_insert_vm( &g_vm_root, vma );
        printf( "[%d], size:%lu[%lu], VMA: %lu - %lu\n", i, size, vma->end - vma->start, vma->start, vma->end );
    }
    printf( "==========================================\n" );
    unsigned long idx = 0;
    for ( struct vm_struct_area* vma = g_vm_head; vma; vma = vma->next )
    {
        printf( "VMA[%lu]: %lu - %lu, size: [%lu]\n", idx++, vma->start, vma->end, vma->end - vma->start );
    }
    return 0;
}