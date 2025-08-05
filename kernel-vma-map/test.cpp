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

struct vm_struct_area
{
    unsigned long   addr;
    unsigned long   size;
    vm_struct_area* next;
    // 单链表用于 从[VMA搜索点]启动的遍历
};

#define ALIGN_SIZE 8
#define VMA_ALIGN( addr ) ( ( addr + ( ALIGN_SIZE - 1 ) ) & ~( ALIGN_SIZE - 1 ) )
#define RB_ENTRY( ptr, type, member ) ( ( type* )( ( char* )( ptr ) - offsetof( type, member ) ) )

struct vm_struct_area* g_vm_head = nullptr;
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
// 分配虚拟地址空间 并且 返回一个新的 vm_struct_area 结构体
struct vm_struct_area* alloc_vma( unsigned long size )
{
    unsigned long           addr = VM_START;
    unsigned long           end  = VM_END;
    struct vm_struct_area*  temp;
    struct vm_struct_area*  vma;
    struct vm_struct_area** p;
    size = VMA_ALIGN( size );
    if ( !size )
    {
        return NULL;
    }
    g_vm_mutex.lock();  // 锁定全局虚拟内存区域链表
    for ( p = &g_vm_head; ( temp = *p ); p = &temp->next )
    {
        if ( addr + size < addr )
            return NULL;
        if ( addr + size > end )
            return NULL;
        if ( temp->addr + temp->size > addr )  // 两个VMA 存在交集的时候需要更新 查找地址[ADDR]
        {
            if ( addr < temp->addr && addr + size <= temp->addr )
            {
                break;  // 找到一个合适的地址
            }
            addr = VMA_ALIGN( temp->addr + temp->size );  // 更新地址到下一个可用位置
        }
    }
    vma = ( struct vm_struct_area* )malloc( sizeof( struct vm_struct_area ) );
    memset( vma, 0x00, sizeof( struct vm_struct_area ) );
    vma->addr = addr;
    vma->size = size;
    vma->next = *p;
    *p        = vma;      // 插入到链表中
    g_vm_mutex.unlock();  // 解锁全局虚拟内存区域链表
    return vma;
}

int main()
{
    srand( time( nullptr ) );
    for ( int i = 0;; ++i )
    {
        unsigned long          size = random() % ( 0x40 - 2 ) + 2;
        struct vm_struct_area* vma  = get_unmaped_area( size );
        if ( !vma )
            break;
        printf( "[%d], size:%lu[%lu], VMA: %lu - %lu\n", i, size, vma->size, vma->addr, vma->addr + vma->size );
    }
    printf( "==========================================\n" );
    unsigned long idx = 0;
    for ( struct vm_struct_area* vma = g_vm_head; vma; vma = vma->next )
    {
        printf( "VMA[%lu]: %lu - %lu, size: [%lu]\n", idx++, vma->addr, vma->addr + vma->size, vma->size );
    }
    return 0;
}