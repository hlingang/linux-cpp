#ifndef __NODE_H_
#define __NODE_H_

#include <cstddef>
#include <cstdlib>
#include <cstdio>

struct node_data
{
    int val;
};

struct node
{
    void* data;
    node* next;
};

inline void print_node( node** head )
{
    if ( !head )
    {
        return;
    }
    int __idx = 0;
    for ( node** p = head; *p; )
    {
        node* entry = *p;
        printf( "idx:%d, val: %d\n", __idx++, static_cast< node_data* >( ( *p )->data )->val );
        p = &entry->next;
    }
}

inline void __free_node( node* __node )
{
    if ( !__node )
    {
        return;
    }
    free( __node->data );  // free data
    free( __node );        // free node
    __node = nullptr;
}

inline void free_node( node** head )
{
    if ( !head )
    {
        return;
    }
    for ( node** p = head; *p; )
    {
        node* entry = *p;
        *p          = entry->next;
        __free_node( entry );
    }
}

inline node* create_node( int __val )
{
    auto* new_data = ( node_data* )malloc( sizeof( node_data ) );
    new_data->val  = __val;
    node* new_node = ( node* )malloc( sizeof( node ) );
    new_node->data = new_data;
    new_node->next = nullptr;
    return new_node;
}

inline node* create_node( node* __node )
{
    node* new_node = ( node* )malloc( sizeof( node ) );
    new_node->data = __node->data;
    new_node->next = nullptr;
    return new_node;
}

inline void remove_node( int __val, node** head )
{
    if ( !head )
    {
        return;
    }
    for ( node** p = head; *p; )
    {
        node* entry = *p;
        if ( ( ( node_data* )entry->data )->val == __val )
        {
            *p = entry->next;
            __free_node( entry );
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
        if ( ( ( node_data* )( *p )->data )->val == __val )
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
    if ( !head )
    {
        return nullptr;
    }
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

inline int node_size( node** head )
{
    if ( !head )
    {
        return 0;
    }
    int __sz = 0;
    for ( node** p = head; *p; p = &( *p )->next, ++__sz )
    {
    };
    return __sz;
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
    node* new_node = create_node( __val );
    *p             = new_node;
}

inline int comp( node* n1, node* n2 )
{
    return ( static_cast< node_data* >( ( n1 )->data )->val - static_cast< node_data* >( ( n2 )->data )->val );
}

inline void insert_node( node** head, node* __node, void* fn )
{
    if ( !head )
    {
        return;
    }
    node** p    = head;
    auto*  comp = ( int ( * )( node*, node* ) )fn;
    for ( ; *p; )
    {
        if ( comp )
        {
            if ( comp( *p, __node ) == 0 )
            {
                ( *p )->data = __node->data;
                return;
            }
            if ( comp( *p, __node ) < 0 )
            {
                p = &( *p )->next;
                continue;
            }
            break;
        }
        p = &( *p )->next;
    }
    node* new_node = create_node( __node );
    new_node->next = *p;
    *p             = new_node;
}

inline void reverse_node( node** head )
{
    if ( !head )
    {
        return;
    }
    node** p       = head;
    node*  next    = nullptr;
    node*  back_up = nullptr;
    for ( ; *p; )
    {
        back_up      = ( *p )->next;
        ( *p )->next = next;
        next         = *p;
        *p           = back_up;
    }
    *p = next;
}

inline void copy_node( node** tar, node** src, void* fn )
{
    for ( node** p = src; *p; p = &( *p )->next )
    {
        insert_node( tar, *p, fn );
    }
}

inline int node_less( node* n1, node* n2 )
{
    return static_cast< node_data* >( n1->data )->val - static_cast< node_data* >( n2->data )->val;
}

inline int node_greater( node* n1, node* n2 )
{
    return static_cast< node_data* >( n2->data )->val - static_cast< node_data* >( n1->data )->val;
}

#endif
