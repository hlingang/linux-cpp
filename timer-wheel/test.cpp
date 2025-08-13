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

using namespace std;
///////////////////// C 语言实现 /////////////////////////////////

struct list_head
{
    list_head* next;
    list_head* prev;
};
static inline int list_size( struct list_head* head )
{
    int        n;
    list_head* pos;
    for ( pos = head->next, n = 0; pos != head; pos = pos->next, n++ )
    {
    }
    return n;
}
inline void INIT_LIST_HEAD( list_head* list )
{
    list->next = list;
    list->prev = list;
}

inline bool list_empty( const list_head* list )
{
    return list->next == list && list->prev == list;
}
inline void list_add( list_head* new_node, list_head* head )
{
    new_node->next   = head->next;
    new_node->prev   = head;
    head->next->prev = new_node;
    head->next       = new_node;
}
inline void list_del( list_head* entry )
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
}
inline void list_del_init( list_head* entry )
{
    list_del( entry );
    INIT_LIST_HEAD( entry );
}
inline void list_move( list_head* new_node, list_head* head )
{
    list_del( new_node );
    list_add( new_node, head );
}
inline void list_switch_head( struct list_head* old_head, struct list_head* new_head )
{
    if ( list_empty( old_head ) )
        return;
    list_add( new_head, old_head );
    list_del_init( old_head );
}
#define list_entry( ptr, type, member ) ( ( type* )( ( char* )( ptr ) - offsetof( type, member ) ) )
#define list_for_each( pos, head ) for ( list_head* pos = ( head )->next; pos != ( head ); pos = pos->next )
#define list_for_each_entry( pos, head, member )                                                    \
    for ( pos = list_entry( ( head )->next, __typeof__( *pos ), member ); &pos->member != ( head ); \
          pos = list_entry( pos->member.next, __typeof__( *pos ), member ) )
#define list_for_each_entry_safe( pos, tmp, head, member )                                            \
    for ( pos = list_entry( ( head )->next, __typeof__( *pos ), member ), tmp = ( pos )->member.next; \
          &pos->member != ( head ); pos = list_entry( tmp, __typeof__( *pos ), member ), tmp = ( tmp )->next )

#define LIST_HEAD( name ) list_head name = ( struct list_head ){ &name, &name };
#define INIT_LIST_ARRAY( _list, _size ) \
    for ( int i = 0; i < _size; i++ )   \
    {                                   \
        INIT_LIST_HEAD( &_list[ i ] );  \
    }
#define STATS_LIST_ARRAY( name, _list, _size )                               \
    {                                                                        \
        printf( "%s STATS\n", name );                                        \
        for ( int i = 0; i < _size; i++ )                                    \
        {                                                                    \
            printf( "%s[%d] size=%d\n", name, i, list_size( &_list[ i ] ) ); \
        }                                                                    \
    }
#define STATS_TV_BASE( tv_base )                        \
    {                                                   \
        STATS_LIST_ARRAY( "tv", tv_base->tv, N_TV );    \
        STATS_LIST_ARRAY( "tv1", tv_base->tv1, N_TV1 ); \
        STATS_LIST_ARRAY( "tv2", tv_base->tv2, N_TV2 ); \
    }

#define TV_BIT 3
#define TV1_BIT 3
#define TV2_BIT 2
#define N_TV ( 1 << TV_BIT )
#define N_TV1 ( 1 << TV1_BIT )
#define N_TV2 ( 1 << TV2_BIT )

#define TV_SIZE ( 1 << TV_BIT )
#define TV1_SIZE ( 1 << ( TV_BIT + TV1_BIT ) )
#define TV2_SIZE ( TV1_SIZE * N_TV2 )
#define MAX_TV_SIZE ( TV2_SIZE )

#define TV_MASK ( N_TV - 1 )
#define TV1_MASK ( N_TV1 - 1 )
#define TV2_MASK ( N_TV2 - 1 )

struct timer_base
{
    struct list_head tv[ N_TV ];
    struct list_head tv1[ N_TV1 ];
    struct list_head tv2[ N_TV2 ];
    std::mutex       lock;
};

struct timer_list
{
    list_head     node;
    unsigned long expires;            // 过期时间
    unsigned long data;               // 附加数据
    void ( *func )( unsigned long );  // 回调函数
    struct timer_base* base;
};

static inline void default_timer_func( unsigned long data )
{
    printf( "Deal-with timer data:(%lu)\n", data );
}

inline void init_tv_base( struct timer_base* tv_base )
{
    INIT_LIST_ARRAY( tv_base->tv, N_TV );
    INIT_LIST_ARRAY( tv_base->tv1, N_TV1 );
    INIT_LIST_ARRAY( tv_base->tv2, N_TV2 );
}

inline void add_timer( struct timer_list* timer, struct timer_base* tv_base, unsigned long jeffies )
{
    timer->base         = tv_base;
    unsigned long index = timer->expires - jeffies;
    tv_base->lock.lock();
    if ( index < TV_SIZE )
    {
        list_add( &timer->node, &tv_base->tv[ index & TV_MASK ] );
    }
    else if ( index < TV1_SIZE )
    {
        list_add( &timer->node, &tv_base->tv1[ ( index >> TV_BIT ) & TV1_MASK ] );
    }
    else
    {
        list_add( &timer->node, &tv_base->tv2[ ( index >> ( TV_BIT + TV1_BIT ) ) & TV2_MASK ] );
    }
    tv_base->lock.unlock();
}
inline void init_timer( struct timer_list* timer, unsigned long expires, unsigned long data )
{
    timer->expires = expires;
    timer->data    = data;
    timer->func    = default_timer_func;
}

inline int cascade( const char* name, struct list_head* tv, unsigned long index, unsigned jeffies,
                    struct timer_base* tv_base )
{
    tv_base->lock.lock();
    struct list_head* head = &tv[ index ];
    if ( list_empty( head ) )
    {
        tv_base->lock.unlock();
        return index;
    }
    printf( "Cascade %s, index=%lu, array size=%d\n", name, index, list_size( head ) );
    printf( "=================== BEFORE-CASCADE ====================\n" );
    STATS_TV_BASE( tv_base );
    struct timer_list* timer;
    struct list_head*  tmp;
    struct list_head   cascade_list;
    INIT_LIST_HEAD( &cascade_list );
    list_switch_head( head, &cascade_list );
    list_for_each_entry_safe( timer, tmp, &cascade_list, node )
    {
        list_del_init( &timer->node );
        unsigned long offset = timer->expires - jeffies;
        if ( offset < TV_SIZE )
        {
            list_add( &timer->node, &tv_base->tv[ timer->expires & TV_MASK ] );
        }
        else if ( offset < TV1_SIZE )
        {
            list_add( &timer->node, &tv_base->tv1[ ( timer->expires >> TV_BIT ) & TV1_MASK ] );
        }
        else
        {
            list_add( &timer->node, &tv_base->tv2[ ( timer->expires >> ( TV_BIT + TV1_BIT ) ) & TV2_MASK ] );
        }
    }
    tv_base->lock.unlock();
    printf( "=================== AFTER-CASCADE ====================\n" );
    STATS_TV_BASE( tv_base );
    return index;
}

int main()
{
    unsigned long seed = time( nullptr );
    srand( seed );
    struct timer_base  tv_base_s;
    struct timer_base* tv_base = &tv_base_s;
    init_tv_base( tv_base );
    for ( int i = 0; i < 1024; i++ )
    {
        int value = rand() % ( 2 * MAX_TV_SIZE );
        printf( "i = %04d, value=%d\n", i, value );
        struct timer_list* timer = ( struct timer_list* )malloc( sizeof( struct timer_list ) );
        memset( timer, 0x00, sizeof( struct timer_list ) );
        init_timer( timer, value, i );
        add_timer( timer, tv_base, 0 );
    }
    STATS_TV_BASE( tv_base );
    unsigned long start = rand() % N_TV;
    printf( "Start Process: %lu\n", start );
    for ( int i = 0; i < 2 * MAX_TV_SIZE; i++ )
    {
        unsigned long jeffies = start + i;
        int           index   = jeffies & TV_MASK;
        if ( !index && !cascade( "tv1", tv_base->tv1, ( jeffies >> TV_BIT ) & TV1_MASK, jeffies, tv_base ) )
            cascade( "tv2", tv_base->tv2, ( jeffies >> ( TV_BIT + TV1_BIT ) ) & TV2_MASK, jeffies, tv_base );
        tv_base->lock.lock();
        struct list_head work_list;
        INIT_LIST_HEAD( &work_list );
        struct list_head* head = &work_list;
        printf( "Process tv[%d], size=%d\n", index, list_size( head ) );
        list_switch_head( tv_base->tv + index, &work_list );
        while ( !list_empty( head ) )
        {
            struct list_head*  pos   = head->next;
            struct timer_list* entry = list_entry( pos, struct timer_list, node );
            list_del_init( pos );
            void ( *func )( unsigned long ) = entry->func;
            entry->base                     = NULL;
            unsigned data                   = entry->data;
            tv_base->lock.unlock();
            func( data );
            tv_base->lock.lock();
            entry->base = NULL;
            printf( "Process-timer: %04lu End.\n", entry->data );
            free( entry );
        }
        tv_base->lock.unlock();
    }
    STATS_TV_BASE( tv_base );
    return 0;
}