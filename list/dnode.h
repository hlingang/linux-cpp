#ifndef __DNODE_H_
#define __DNODE_H_

#include <cstddef>
#include <cstdlib>
#include <cstdio>

struct node
{
    int   val;
    node* pre;
    node* next;
};

inline void print_node( node** head )
{
    int __idx = 0;
    for ( node** p = head; *p; )
    {
        node* entry = *p;
        printf( "idx:%d, val: %d\n", __idx++, ( *p )->val );
        p = &entry->next;
    }
}

inline void remove_node( int __val, node** head )
{
    for ( node** p = head; *p; )
    {
        node* entry = *p;
        if ( entry->val == __val )
        {
            *p               = entry->next;
            entry->next->pre = *p;
            free( entry );
        }
        else
        {
            p = &entry->next;
        }
    }
}

inline node** __find_node( int __val, node** head )
{
    if ( !head )
    {
        return nullptr;
    }
    node** p = head;
    for ( ; *p; p = &( *p )->next )
    {
        if ( ( *p )->val == __val )
        {
            break;
        }
    }
    return p;
}

inline node* find_node( int __val, node** head )
{
    if ( !head )
    {
        return nullptr;
    }
    node** p = __find_node( __val, head );
    return p ? *p : nullptr;
}

inline node* find_tail( node** head )
{
    node** p = head;
    if ( !*p )
    {
        return nullptr;
    }
    for ( ; ( *p )->next; p = &( *p )->next )
    {
    }
    return *p;
}

inline void insert_node( int __val, node** head )
{
    if ( !head )
    {
        return;
    }
    node** p = __find_node( __val, head );
    if ( !p )
    {
        return;
    }
    if ( *p )
    {
        printf( "find node[%d] exists and return.\n", __val );
        return;
    }
    node* new_node = new node{ __val, nullptr };
    new_node->pre  = *p;
    *p             = new_node;
}

inline node* reverse_node( node** head )
{
    if ( !head )
    {
        return nullptr;
    }
    node** p       = head;
    node*  next    = nullptr;
    node*  back_up = nullptr;
    for ( ; *p; )
    {
        back_up      = ( *p )->next;  // 备份后一个位置
        ( *p )->next = ( *p )->pre;
        ( *p )->pre  = back_up;
        next         = *p;  // 备份前一个位置
        *p           = back_up;
    }
    return next;
}

#endif