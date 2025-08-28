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
#include <random>
#include <unistd.h>

using namespace std;
///////////////////// C 语言实现 /////////////////////////////////
struct list_head
{
    struct list_head* next;
    struct list_head* prev;
};

#define INIT_LIST_HEAD( ptr )                    \
    do                                           \
    {                                            \
        ( ptr )->next = ( ptr )->prev = ( ptr ); \
    } while ( 0 )

#define LIST_HEAD ( name ) struct list_head name = { &( name ), &( name ) }

#define list_entry( ptr, type, member ) ( ( type* )( ( char* )( ptr ) - ( unsigned long )&( ( ( type* )0 )->member ) ) )
#define list_for_each_entry( pos, head, member )                                                \
    for ( pos = list_entry( ( head )->next, typeof( *pos ), member ); &pos->member != ( head ); \
          pos = list_entry( pos->member.next, typeof( *pos ), member ) )

#define LIST_ENTRY( ptr ) list_entry( ptr, struct dentry, sibling )
#define fetch_entry( entry, ptr )                          \
    ( {                                                    \
        entry = list_entry( ptr, struct dentry, sibling ); \
        1;                                                 \
    } )
static void list_add( struct list_head* new_list, struct list_head* head )
{
    new_list->next       = head;
    new_list->prev       = head->prev;
    new_list->prev->next = new_list;
    new_list->next->prev = new_list;
}

static void list_del( struct list_head* entry )
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    INIT_LIST_HEAD( entry );
}
static bool list_empty( const struct list_head* head )
{
    return head->next == head && head->prev == head;
}

struct dentry
{
    char             name[ 128 ];
    unsigned long    id;
    struct dentry*   parent;
    struct list_head subdirs;
    struct list_head sibling;
};

static void init_dentry( struct dentry* dentry, const char* __name, unsigned long id, struct dentry* parent )
{
    dentry->id     = id;
    dentry->parent = parent;
    INIT_LIST_HEAD( &dentry->subdirs );
    INIT_LIST_HEAD( &dentry->sibling );
    memset( dentry->name, 0x00, sizeof( dentry->name ) );
    if ( __name )
        memcpy( dentry->name, __name, sizeof( dentry->name ) - 1 );
}
struct dentry dentry_root;

static void stats_dentry( struct dentry* root )
{
    printf( "\n==================== travel dentry tree ====================\n" );
    struct list_head* pos;
    struct dentry*    entry;
    struct dentry*    parent = root;
repeat:
    pos = parent->subdirs.next;
resume:
    while ( pos != &parent->subdirs )
    {
        entry = list_entry( pos, struct dentry, sibling );
        printf( "== entry[%p]:%s(%lu)\n", entry, entry->name, entry->id );
        if ( !list_empty( &entry->subdirs ) )  // 往下遍历
        {
            parent = entry;
            goto repeat;
        }
        pos = pos->next;
    }
    if ( parent != root )
    {
        pos    = parent->sibling.next;  // 回到上一级
        parent = parent->parent;        // 更新 parent
        goto resume;
    }
}

static void remove_dentry( struct dentry* dir )
{
    printf( "\n==================== remove dentry tree ====================\n" );
    struct dentry* entry;
    struct dentry* parent = dir;
    while ( !list_empty( &parent->subdirs ) )
    {
    repeat:
        while ( !list_empty( &parent->subdirs ) )
        {
            entry  = list_entry( parent->subdirs.next, struct dentry, sibling );
            parent = entry;
        }
        entry  = parent;
        parent = parent->parent;
        struct list_head* next;
        do
        {
            //*****  通过连续删除进行优化 **** //
            struct dentry* tmp = entry;
            next               = entry->sibling.next;
            list_del( &tmp->sibling );
            printf( "Free[%p]:%s(%lu)\n", tmp, tmp->name, tmp->id );
            free( tmp );
        } while ( next != &parent->subdirs && fetch_entry( entry, next ) && list_empty( &entry->subdirs ) );
        if ( parent != dir )
            goto repeat;
    }
}
//===============================================================

int main()
{
    init_dentry( &dentry_root, "root", 0, &dentry_root );
    for ( int i = 1; i < 5; i++ )
    {
        dentry* dir = new dentry;
        init_dentry( dir, ( "dir_" + to_string( i ) ).c_str(), i, &dentry_root );
        list_add( &dir->sibling, &dentry_root.subdirs );
        // printf( "Malloc[%p](pre:%p, next:%p)\n", dir, dir->sibling.prev, dir->sibling.next );
        for ( int j = 0; j < 3; j++ )
        {
            dentry* subdir = new dentry;
            init_dentry( subdir, ( "subdir_" + to_string( i ) + "_" + to_string( j ) ).c_str(), i * 10 + j, dir );
            list_add( &subdir->sibling, &dir->subdirs );
            // printf( "Malloc[%p](pre:%p, next:%p)\n", subdir, subdir->sibling.prev, subdir->sibling.next );

            for ( int k = 0; k < 2; k++ )
            {
                dentry* subsubdir = new dentry;

                init_dentry( subsubdir,
                             ( "sub_subdir_" + to_string( i ) + "_" + to_string( j ) + "_" + to_string( k ) ).c_str(),
                             100 * i + j * 10 + k, subdir );
                list_add( &subsubdir->sibling, &subdir->subdirs );
                // printf( "Malloc[%p](pre:%p, next:%p)\n", subsubdir, subsubdir->sibling.prev, subsubdir->sibling.next
                // );
            }
        }
    }
    printf( "================= First Level =================\n" );
    struct dentry* entry;
    list_for_each_entry( entry, &dentry_root.subdirs, sibling )
    {
        printf( "== First Level:[%p] %s(%lu)(pre:%p, next:%p)\n", entry, entry->name, entry->id, entry->sibling.prev,
                entry->sibling.next );
        struct dentry* sub_entry;
        list_for_each_entry( sub_entry, &entry->subdirs, sibling )
        {
            printf( "==== Second Level:[%p] %s(%lu)(pre:%p[%p], next:%p[%p])\n", sub_entry, sub_entry->name,
                    sub_entry->id, sub_entry->sibling.prev, LIST_ENTRY( sub_entry->sibling.prev ),
                    sub_entry->sibling.next, LIST_ENTRY( sub_entry->sibling.next ) );
            struct dentry* sub_sub_entry;
            list_for_each_entry( sub_sub_entry, &sub_entry->subdirs, sibling )
            {
                printf( "======== Third Level:[%p] %s(%lu)(pre:%p[%p], next:%p[%p])\n", sub_sub_entry,
                        sub_sub_entry->name, sub_sub_entry->id, sub_sub_entry->sibling.prev,
                        LIST_ENTRY( sub_sub_entry->sibling.prev ), sub_sub_entry->sibling.next,
                        LIST_ENTRY( sub_sub_entry->sibling.next ) );
            }
        }
    }
    stats_dentry( &dentry_root );
    remove_dentry( &dentry_root );
    return 0;
}