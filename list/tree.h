#ifndef __TREE_H_
#define __TREE_H_

#include <cstddef>
#include <iostream>
#include <cstring>
#include <unistd.h>

enum e_dir
{
    e_left  = 0,
    e_right = 1,
};

struct tree
{
    const char* name;
    int         val;
    tree*       left;
    tree*       right;
};

int comp( const char* t1, const char* t2 )
{
    if ( t1 == nullptr && t2 == nullptr )
    {
        return 0;
    }
    if ( t1 == nullptr )
    {
        return 0 - strlen( t2 );
    }
    if ( t2 == nullptr )
    {
        return strlen( t1 );
    }
    if ( strlen( t1 ) == strlen( t2 ) )
    {
        return strcmp( t1, t2 );
    }
    return strlen( t1 ) - strlen( t2 );
}

inline tree* create_tree( const char* __name, int __val )
{
    tree* new_tree  = ( tree* )malloc( sizeof( tree ) );
    new_tree->name  = __name;
    new_tree->val   = __val;
    new_tree->left  = nullptr;
    new_tree->right = nullptr;
    return new_tree;
};

inline void print_tree( tree** root )
{
    tree** p = root;
    if ( *p )
    {
        printf( "name:%s, val=%d\n", ( *p )->name, ( *p )->val );
        print_tree( &( *p )->left );
        print_tree( &( *p )->right );
    }
}

inline tree** __find_tree( tree** root, const char* __name )
{
    if ( !root )
    {
        return nullptr;
    }
    tree** p = root;
    for ( ; *p; )
    {
        if ( comp( __name, ( *p )->name ) > 0 )
        {
            p = &( *p )->right;
        }
        else if ( comp( __name, ( *p )->name ) < 0 )
        {
            p = &( *p )->left;
        }
        else
        {
            break;
        }
    }
    return p;
}

inline tree* find_tree( tree** root, const char* __name )
{
    if ( !root )
    {
        return nullptr;
    }
    tree** p = __find_tree( root, __name );
    return p ? *p : nullptr;
}

inline void insert_tree( tree** root, const char* __name, int __val )
{
    if ( !root )
    {
        return;
    }
    tree** p = __find_tree( root, __name );
    if ( !p )
    {
        return;
    }
    if ( *p )
    {
        ( *p )->val = __val;
        return;
    }
    tree* new_tree = create_tree( __name, __val );
    *p             = new_tree;
}

inline void __insert_tree( tree** root, tree* __tree )
{
    return insert_tree( root, __tree->name, __tree->val );
}

inline tree* find_replace( tree** root, int __flag )
{
    if ( !root )
    {
        return nullptr;
    }
    tree** p = root;
    if ( __flag == e_left )
    {
        for ( ; ( *p )->right; p = &( *p )->right )
        {
        }
    }
    else if ( __flag == e_right )
    {
        for ( ; ( *p )->left; p = &( *p )->left )
        {
        }
    }
    return *p;
}

inline void remove_tree( tree** root, const char* __name )
{
    if ( !root )
    {
        return;
    }
    tree** p = __find_tree( root, __name );
    if ( !p )
    {
        return;
    }
    if ( !*p )
    {
        return;
    }
    if ( !( *p )->left && !( *p )->right )
    {
        free( *p );
        *p = nullptr;
    }
    else if ( ( *p )->left && !( *p )->right )
    {
        *p = ( *p )->left;
        free( *p );
    }
    else if ( !( *p )->left && ( *p )->right )
    {
        *p = ( *p )->right;
        free( *p );
    }
    else
    {
        tree* replace = find_replace( &( *p )->left, e_left );
        ( *p )->val   = replace->val;
        ( *p )->name  = replace->name;
        printf( "find replace tree: %s\n", replace->name );
        remove_tree( &( *p )->left, replace->name );
    }
}

inline void copy_tree( tree** tar, tree** src )
{
    if ( !tar || !src )
    {
        return;
    }
    tree** p = src;
    if ( !*p )
    {
        return;
    }
    __insert_tree( tar, *p );
    if ( ( *p )->left )
    {
        copy_tree( tar, &( *p )->left );
    }
    if ( ( *p )->right )
    {
        copy_tree( tar, &( *p )->right );
    }
}

inline int tree_size( tree** root )
{
    if ( !root )
    {
        return 0;
    }
    int    __sz = 0;
    tree** p    = root;
    if ( !*p )
    {
        return 0;
    }
    ++__sz;
    if ( ( *p )->left )
    {
        __sz += tree_size( &( *p )->left );
    }
    if ( ( *p )->right )
    {
        __sz += tree_size( &( *p )->right );
    }
    return __sz;
}

void free_tree( tree** root )
{
    if ( !root )
    {
        return;
    }
    tree** p = root;
    if ( !*p )
    {
        return;
    }
    // must free child node first!
    if ( ( *p )->left )
    {
        free_tree( &( *p )->left );
    }
    if ( ( *p )->right )
    {
        free_tree( &( *p )->right );
    }
    // then free current node!
    if ( nullptr == ( *p )->left && nullptr == ( *p )->right )
    {
        printf( "free tree: %s\n", ( *p )->name );
        free( *p );
        *p = nullptr;
    }
}

#endif