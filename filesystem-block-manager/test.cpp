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
#define NR_BITS_PER_LONG ( sizeof( unsigned long ) * 8 )
#define NR_BIT_WORD( nr ) ( nr / NR_BITS_PER_LONG )
#define NR_BIT_MASK( nr ) ( 1 << NR_BIT_OFFSET( nr ) )
#define NR_BIT_OFFSET( nr ) ( nr % NR_BLOCKS_PER_GROUP )

#define PAGE_SIZE ( 4096 )
#define BLOCK_SIZE ( 1024 )
#define NR_MAX_BLOCKS ( 1024 )
#define NR_BLOCKS_PER_GROUP ( 256 )
#define NR_GROUPS ( NR_MAX_BLOCKS / NR_BLOCKS_PER_GROUP )
#define SUPER_BLOCK_ID ( 0 )
#define START_BLOCK ( 0 )
#define PAGE_SHIFT ( 12 )
#define BLOCK_SHIFT ( 10 )

#define SUPER_BLOCK ( 0 )
#define GROUP_DESC_BLOCK ( 1 )
#define BITMAP_BLOCK ( 2 )
#define RESV_BLOCK ( 3 )

struct inode_t
{
    struct page_t*      i_mapping[ 1024 ];
    struct buffer_head* i_buffer_head[ 1024 ][ 4 ];
    unsigned            i_data[ 5 ];
};
struct group_desc_t
{
    unsigned long bitmap_block;
    unsigned long start_block;
    unsigned long super_block;
    unsigned long group_desc_block;
};

struct super_block_t
{
    unsigned long        nr_groups;
    unsigned long        nr_blocks_per_group;
    unsigned long        nr_block;
    unsigned long        nr_start_block;
    struct buffer_head** group_desc;
    struct buffer_head*  sbh;
    void*                sbi;
};
struct block_data_t
{
    char data[ 1024 ];
} __attribute__( ( aligned( 1024 ) ) );

struct page_data_t
{
    char data[ 4096 ];
} __attribute__( ( aligned( 4096 ) ) );

struct page_t
{
    void*               vir;
    unsigned            index;
    struct buffer_head* bh;
};

struct buffer_head
{
    struct page_t* page;
    unsigned long  nr_block;
    buffer_head*   next;
    void*          data;
    unsigned long  offset;
};

#define buf_ctl( type, x ) ( ( type )( x ) )
#define init_global_block( x, s )          \
    for ( int i = 0; i < s; i++, ++x )     \
    {                                      \
        *buf_ctl( unsigned long*, x ) = i; \
    }
#define stats_global_block( x, s )                               \
    for ( int i = 0; i < s; i++, ++x )                           \
    {                                                            \
        printf( "%i: %lu\n", i, *buf_ctl( unsigned long*, x ) ); \
    }

#define init_global_page( x, map, s )                \
    for ( int i = 0; i < s; i++, ++x )               \
    {                                                \
        struct page_t* __page = map + i;             \
        __page->vir           = buf_ctl( void*, x ); \
    }
block_data_t*       g_blocks;
struct page_data_t* g_page;
struct page_t*      g_page_map;
unsigned long       page_bitmap[ 1024 ];
struct inode_t*     bdev;
//===============================================================

void set_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    *ptr |= ( 1U << offset );
}

void clear_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    *ptr &= ~( 1U << offset );
}

int test_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    return ( *ptr & ( 1U << offset ) ) != 0;
}

struct page_t* get_one_page()
{
    for ( int i = 0; i < 1024; i++ )
    {
        if ( !page_bitmap[ i ] )
        {
            page_bitmap[ i ] = 1;
            return g_page_map + i;
        }
    }
    return NULL;
}
void free_one_page( struct page_t* page )
{
    unsigned long index  = page - g_page_map;
    page_bitmap[ index ] = 0;
}

struct buffer_head* create_buffer_head( struct buffer_head** pbh, struct page_t* page, unsigned long index,
                                        unsigned iblock )
{
    unsigned long offset = PAGE_SIZE;
    offset -= BLOCK_SIZE;
    buffer_head* head = NULL;
    while ( offset > 0 )
    {
        struct buffer_head* bh     = new struct buffer_head;
        bh->next                   = NULL;
        bh->page                   = page;
        bh->offset                 = offset;
        bh->next                   = head;
        bh->nr_block               = iblock;
        bh->data                   = ( char* )page->vir + offset;
        head                       = bh;
        pbh[ offset / BLOCK_SIZE ] = bh;
        offset -= BLOCK_SIZE;
        ++iblock;
    }
    page->bh = head;
    return page->bh;
}

struct buffer_head* read_block( struct buffer_head* bh )
{
    unsigned      nr_block = bh->nr_block;
    block_data_t* ptr      = g_blocks + nr_block;
    memcpy( bh->data, ptr, BLOCK_SIZE );
    unsigned long* ptr_v = ( unsigned long* )bh->data;
    cout << "*Read block = " << *ptr_v << " , offset = " << bh->offset << endl;
    return bh;
}

struct buffer_head* write_block( struct buffer_head* bh )
{
    unsigned nr_block = bh->nr_block;
    cout << "Write Block: " << bh->nr_block << endl;
    block_data_t* ptr = g_blocks + nr_block;
    memcpy( ptr, bh->data, BLOCK_SIZE );
    return bh;
}

struct buffer_head* sb_bread( struct super_block_t* sb, unsigned long iblock )
{
    unsigned long index = iblock >> ( PAGE_SHIFT - BLOCK_SHIFT );
    if ( !bdev->i_mapping[ index ] )
    {
        bdev->i_mapping[ index ] = get_one_page();
    }
    struct page_t* page = bdev->i_mapping[ index ];
    page->index         = index;
    unsigned block      = index << ( PAGE_SHIFT - BLOCK_SHIFT );
    if ( !page->bh )
    {
        create_buffer_head( ( struct buffer_head** )bdev->i_buffer_head[ index ], page, index, block );
    }
    struct buffer_head* bh = page->bh;
    while ( bh != NULL )
    {
        if ( bh->nr_block == iblock )
        {
            break;
        }
        bh = bh->next;
    }
    read_block( bh );
    return bh;
}
static inline unsigned long group_first_block( super_block_t* sb, unsigned long ngp )
{
    return ngp * NR_BLOCKS_PER_GROUP + sb->nr_start_block;
}
int init_group_desc( super_block_t* sb, buffer_head* bh )
{
    group_desc_t* desc = ( group_desc_t* )bh->data;
    for ( unsigned long ngp = 0; ngp < sb->nr_groups; ngp++ )
    {
        desc->start_block      = group_first_block( sb, ngp ) + START_BLOCK;
        desc->bitmap_block     = group_first_block( sb, ngp ) + BITMAP_BLOCK;
        desc->group_desc_block = group_first_block( sb, ngp ) + GROUP_DESC_BLOCK;
        desc->super_block      = group_first_block( sb, ngp ) + SUPER_BLOCK;
    }
    write_block( bh );
    return 1;
}

void stats_desc_block_data( unsigned long block, unsigned long ngp )
{
    group_desc_t* desc = ( group_desc_t* )( g_blocks + block );
    printf( "group %lu: start block: %lu\n", ngp, desc->start_block );
    printf( "group %lu: super block: %lu\n", ngp, desc->super_block );
    printf( "group %lu: group desc block: %lu\n", ngp, desc->group_desc_block );
    printf( "group %lu: bitmap block: %lu\n", ngp, desc->bitmap_block );
}

group_desc_t* group_desc( super_block_t* sb, unsigned long ngp )
{
    return ( group_desc_t* )sb->group_desc[ 0 ]->data + ngp;
}

void init_block_bitmap( super_block_t* sb, struct buffer_head* bh )
{
    unsigned long* addr = ( unsigned long* )bh->data;
    set_bit( START_BLOCK, addr );
    set_bit( SUPER_BLOCK, addr );
    set_bit( GROUP_DESC_BLOCK, addr );
    set_bit( BITMAP_BLOCK, addr );
    set_bit( RESV_BLOCK, addr );
}

//===============================================================

int main()
{
    // ============== 创建 bdev ==============
    bdev = new struct inode_t;
    memset( bdev, 0x00, sizeof( struct inode_t ) );
    // ============== 创建 block ==============
    block_data_t* ptr;
    g_blocks = new block_data_t[ 1024 ];
    memset( g_blocks, 0x00, sizeof( struct block_data_t ) * 1024 );
    ptr = g_blocks;
    init_global_block( ptr, 1024 );
    ptr = g_blocks;
    stats_global_block( ptr, 1024 );

    // ============== 创建 page ==============
    g_page_map = new struct page_t[ 1024 ];
    memset( g_page_map, 0x00, sizeof( struct page_t ) * 1024 );
    g_page = new page_data_t[ 1024 ];
    memset( g_page, 0x00, sizeof( page_data_t ) * 1024 );
    struct page_data_t* page_addr = g_page;
    struct page_t*      map_addr  = g_page_map;
    init_global_page( page_addr, map_addr, 1024 );
    memset( page_bitmap, 0x00, sizeof( page_bitmap ) );

    // ============== 创建 super-block ============
    for ( int ngp = 0; ngp < NR_GROUPS; ngp++ )
    {
        super_block_t* sb = new super_block_t;
        memset( sb, 0x00, sizeof( struct super_block_t ) );
        sb->nr_groups                  = NR_GROUPS;
        sb->nr_blocks_per_group        = NR_BLOCKS_PER_GROUP;
        sb->nr_start_block             = START_BLOCK;
        sb->nr_block                   = ngp * NR_BLOCKS_PER_GROUP + START_BLOCK;
        sb->sbh                        = sb_bread( sb, sb->nr_block );
        sb->sbi                        = ( char* )sb->sbh->data + sb->sbh->offset;
        sb->group_desc                 = new struct buffer_head*;
        unsigned long group_desc_block = ngp * NR_BLOCKS_PER_GROUP + GROUP_DESC_BLOCK;
        sb->group_desc[ 0 ]            = sb_bread( sb, group_desc_block );
        init_group_desc( sb, sb->group_desc[ 0 ] );
        stats_desc_block_data( group_desc_block, ngp );
        group_desc_t*       desc         = group_desc( sb, ngp );
        unsigned long       bitmap_block = desc->bitmap_block;
        struct buffer_head* bh_bitmap    = sb_bread( sb, bitmap_block );
        init_block_bitmap( sb, bh_bitmap );
    }
    return 0;
}