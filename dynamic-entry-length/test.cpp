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
#define PAGE_SIZE ( 1024 )
#define PAGE_MASK ( PAGE_SIZE - 1 )
#define PAGE_SHIFT ( 10 )
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
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
#pragma GCC diagnostic pop

struct entry
{
    unsigned long id;
    unsigned long entry_len;  // entry name length
    unsigned long name_len;
    char          name[ 128 ];
};
struct inode
{
    unsigned long size;
    void*         mapping[ 1024 ];
};
//===============================================================
static void* alloc_page()
{
    void* page = malloc( PAGE_SIZE );
    if ( page )
    {
        memset( page, 0, PAGE_SIZE );
    }
    return page;
}

static void write_inode( struct inode* inode, struct entry* entry )
{
repeat:
    unsigned long index  = inode->size >> PAGE_SHIFT;
    unsigned long offset = inode->size & PAGE_MASK;
    if ( index >= 1024 )
    {
        printf( "inode full\n" );
        return;
    }
    if ( !inode->mapping[ index ] )
    {
        inode->mapping[ index ] = alloc_page();
        if ( !inode->mapping[ index ] )
        {
            printf( "alloc page fail\n" );
            return;
        }
    }
    struct entry* tmp = ( struct entry* )( ( char* )inode->mapping[ index ] + offset );
    if ( offset + entry->entry_len < PAGE_SIZE )
    {
        tmp->id        = entry->id;
        tmp->entry_len = entry->entry_len;
        tmp->name_len  = entry->name_len;
        memcpy( tmp->name, entry->name, entry->name_len );
        inode->size += entry->entry_len;
    }
    else
    {
        inode->size += PAGE_SIZE - offset;  // 引入空洞 //
        cout << "page full, need next page: " << index << endl;
        goto repeat;
    }
}
#define last_bytes( size, nr )                 \
    ( {                                        \
        unsigned ret = PAGE_SIZE;              \
        if ( ( nr + 1 ) << PAGE_SHIFT > size ) \
            ret = ( size & PAGE_MASK );        \
        ret;                                   \
    } )
#define next_entry( e ) ( ( struct entry* )( ( char* )( e ) + ( e )->entry_len ) )

void* find_entry( struct inode* inode, struct entry* entry )
{
    unsigned long size   = inode->size;
    unsigned long npages = ( size + PAGE_SIZE - 1 ) >> PAGE_SHIFT;
    struct entry* e;
    for ( unsigned long i = 0; i < npages; i++ )
    {
        char* page     = ( char* )inode->mapping[ i ];
        char* end_addr = ( char* )page + last_bytes( size, i );
        end_addr -= entry->entry_len;  // 保证最后一个 e->entry_len 是有效的 //
        e = ( struct entry* )page;
        for ( ; ( char* )e <= end_addr; )
        {
            if ( !e || !e->entry_len )
                break;
            if ( e->name_len == entry->name_len && !strncmp( e->name, entry->name, e->name_len ) )
            {
                return e;
            }
            e = next_entry( e );
        }
    }
    return NULL;
}
void stats_inode( struct inode* inode )
{
    printf( "\n==================== travel entry ====================\n" );
    unsigned long size   = inode->size;
    unsigned long npages = ( size + PAGE_SIZE - 1 ) >> PAGE_SHIFT;
    struct entry* e;
    cout << "inode size:" << inode->size << ", npages:" << npages << endl;
    for ( unsigned long i = 0; i < npages; i++ )
    {
        char* page     = ( char* )inode->mapping[ i ];
        char* end_addr = ( char* )page + last_bytes( size, i );
        end_addr -= ( offsetof( struct entry, name ) + 1 );  // 保证最后一个 e->entry_len 是有效的 //
        printf( "---- page[%lu]:(%p -- %p) ----\n", i, page, ( char* )page + PAGE_SIZE );
        e = ( struct entry* )page;
        for ( ; ( char* )e <= end_addr; )
        {
            if ( !e || !e->entry_len )
                break;
            printf( "page[%lu] entry[%p vs find:%p]:%s(id:%lu, name_len:%lu, entry_len:%lu)\n", i, e,
                    find_entry( inode, e ), e->name, e->id, e->name_len, e->entry_len );
            e = next_entry( e );
        }
    }
}

int main()
{
    struct inode i_node;
    memset( &i_node, 0x00, sizeof( i_node ) );
    for ( int i = 0; i < 4096; i++ )
    {
        struct entry* e = new struct entry;
        memset( e, 0x00, sizeof( struct entry ) );
        e->id = i;
        snprintf( e->name, sizeof( e->name ), "entry_%d", i );
        e->name_len  = strlen( e->name );
        e->entry_len = offsetof( struct entry, name ) + e->name_len + 1;
        write_inode( &i_node, e );
        delete e;
    }
    stats_inode( &i_node );
    return 0;
}