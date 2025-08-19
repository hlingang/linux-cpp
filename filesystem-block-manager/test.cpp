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
#define NR_BIT_MASK( nr ) ( 1U << NR_BIT_OFFSET( nr ) )
#define NR_BIT_OFFSET( nr ) ( nr % NR_BITS_PER_LONG )

#define PAGE_SIZE ( 4096 )
#define PAGE_MASK ( PAGE_SIZE - 1 )
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
#define MAX_BLOCK_BIT ( NR_BLOCKS_PER_GROUP )
#define DIRECT_BLOCK ( 4 )
#define BLOCK_INDEX_SHIFT ( 7 )
#define BLOCK_INDEX_MASK ( ( 1 << BLOCK_INDEX_SHIFT ) - 1 )
#define BGD_MASK ( ( 1 << BLOCK_INDEX_SHIFT ) - 1 )
#define BMD_MASK ( ( 1 << BLOCK_INDEX_SHIFT ) - 1 )

struct space_mapping
{
    struct inode_t*     host;
    struct page_t*      page_mapping[ 1024 ];
    struct buffer_head* buffer_head_mapping[ 1024 ][ 4 ];
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

struct inode_t
{
    unsigned long        size;
    struct space_mapping i_mapping;
    unsigned long        i_data[ DIRECT_BLOCK + 1 ];
    super_block_t*       sb;
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
    int            uptodate;
    int            mapped;
};

#define buf_ctl( type, x ) ( ( type )( x ) )
#define init_global_block( x, s )          \
    for ( int i = 0; i < s; i++, ++x )     \
    {                                      \
        *buf_ctl( unsigned long*, x ) = 0; \
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
    *ptr |= ( 1UL << offset );
}

void clear_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    *ptr &= ~( 1UL << offset );
}

int test_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    return ( ( *ptr ) & ( 1UL << offset ) ) != 0;
}
int test_and_set_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr       = addr + NR_BIT_WORD( nr );
    unsigned long  offset    = NR_BIT_OFFSET( nr );
    int            old_value = ( ( *ptr ) & ( 1UL << offset ) ) != 0;
    set_bit( nr, addr );
    return old_value;
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

static inline void map_bh( struct buffer_head* bh, unsigned long block )
{
    if ( !bh )
        return;
    bh->nr_block = block;
    bh->mapped   = 1;
}

struct buffer_head* create_buffer_head( space_mapping* mapping, struct page_t* page, unsigned long index,
                                        unsigned iblock )
{
    unsigned long       offset = PAGE_SIZE;
    struct buffer_head* head   = NULL;
    do
    {
        offset -= BLOCK_SIZE;
        struct buffer_head* bh                                       = new struct buffer_head;
        bh->next                                                     = NULL;
        bh->page                                                     = page;
        bh->offset                                                   = offset;
        bh->next                                                     = head;
        bh->nr_block                                                 = -1;
        bh->mapped                                                   = 0;
        bh->data                                                     = ( char* )page->vir + offset;
        head                                                         = bh;
        mapping->buffer_head_mapping[ index ][ offset / BLOCK_SIZE ] = bh;
    } while ( offset > 0 );
    page->bh = head;
    return page->bh;
}
unsigned long inode_size( struct inode_t* inode )
{
    return inode->size;
}
struct buffer_head* read_block( struct buffer_head* bh )
{
    if ( bh->uptodate )
        return NULL;
    unsigned      nr_block = bh->nr_block;
    block_data_t* ptr      = g_blocks + nr_block;
    memcpy( bh->data, ptr, BLOCK_SIZE );
    bh->uptodate = 1;
    return bh;
}
struct buffer_head* write_block( struct buffer_head* bh )
{
    unsigned long nr_block = bh->nr_block;
    block_data_t* ptr      = g_blocks + nr_block;
    memcpy( ptr, bh->data, BLOCK_SIZE );
    printf( "copy data from (%p) to block(%p, %lu): %s\n", bh->data, ptr, nr_block, ( char* )ptr );
    bh->uptodate = 1;
    return bh;
}
struct page_t* find_get_page( struct inode_t* inode, unsigned long index )
{
    if ( !inode->i_mapping.page_mapping[ index ] )
    {
        inode->i_mapping.page_mapping[ index ] = get_one_page();
    }
    return inode->i_mapping.page_mapping[ index ];
}
struct buffer_head* get_blk_bh( struct inode_t* inode, unsigned long iblock )
{
    unsigned long  index = iblock >> ( PAGE_SHIFT - BLOCK_SHIFT );
    unsigned long  block = index << ( PAGE_SHIFT - BLOCK_SHIFT );
    struct page_t* page  = find_get_page( inode, index );
    if ( !page->bh )
    {
        create_buffer_head( ( struct space_mapping* )&inode->i_mapping, page, index, block );
    }
    return page->bh;
}
struct buffer_head* sb_bread( struct super_block_t* sb, unsigned long iblock )
{
    unsigned long  index = iblock >> ( PAGE_SHIFT - BLOCK_SHIFT );
    struct page_t* page  = find_get_page( bdev, index );
    page->index          = index;
    unsigned block       = index << ( PAGE_SHIFT - BLOCK_SHIFT );
    if ( !page->bh )
    {
        create_buffer_head( ( struct space_mapping* )&bdev->i_mapping, page, index, block );
    }
    struct buffer_head* bh = page->bh;
    while ( bh != NULL )
    {
        if ( block == iblock )
        {
            map_bh( bh, iblock );
            break;
        }
        block++;
        bh = bh->next;
    }
    if ( !bh )
        return NULL;
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
        desc++;
    }
    write_block( bh );
    return 1;
}
void stats_desc_block_data( unsigned long block, unsigned long ngp )
{
    group_desc_t* desc = ( group_desc_t* )( g_blocks + block ) + ngp;
    printf( "group %lu: start block: %lu\n", ngp, desc->start_block );
    printf( "group %lu: super block: %lu\n", ngp, desc->super_block );
    printf( "group %lu: group desc block: %lu\n", ngp, desc->group_desc_block );
    printf( "group %lu: bitmap block: %lu\n", ngp, desc->bitmap_block );
}
void clear_block( struct buffer_head* bh )
{
    memset( bh->data, 0x00, BLOCK_SIZE );
}
group_desc_t* group_desc( super_block_t* sb, unsigned long ngp )
{
    return ( group_desc_t* )sb->group_desc[ 0 ]->data + ngp;
}
void init_block_bitmap( super_block_t* sb, struct buffer_head* bh )
{
    clear_block( bh );
    unsigned long* addr = ( unsigned long* )bh->data;
    set_bit( START_BLOCK, addr );
    set_bit( SUPER_BLOCK, addr );
    set_bit( GROUP_DESC_BLOCK, addr );
    set_bit( BITMAP_BLOCK, addr );
    set_bit( RESV_BLOCK, addr );
    write_block( bh );
}
struct inode_t* alloc_inode( super_block_t* sb )
{
    struct inode_t* node = new struct inode_t;
    memset( node, 0x00, sizeof( struct inode_t ) );
    node->i_mapping.host = node;
    node->sb             = sb;
    return node;
}
static inline unsigned long find_next_bit( void* addr, unsigned long nr )
{
    unsigned long* pos = ( unsigned long* )addr;
    unsigned long  value;
    unsigned long  index     = 0;
    unsigned long  offset    = nr % NR_BITS_PER_LONG;
    unsigned long  max_index = nr / NR_BITS_PER_LONG;
    unsigned long  search_len;
    while ( !( value = ~*pos ) )
    {
        ++pos;
        ++index;
        if ( index > max_index )
            goto out;
    }
    search_len = NR_BITS_PER_LONG;
    if ( index == max_index && offset != 0 )
    {
        value &= ( ( 1UL << offset ) - 1 );
        search_len = offset;
    }
    for ( unsigned long i = 0; i < search_len; i++ )
    {
        if ( !test_bit( i, pos ) )
            return i + index * NR_BITS_PER_LONG;
    }
out:
    return nr;
}
int __get_block( super_block_t* sb, unsigned long ngp )
{
    cout << "get_block" << endl;
    if ( ngp >= sb->nr_groups )
        ngp = 0;
    unsigned long ino;
    for ( unsigned long i = 0; i < sb->nr_groups; i++ )
    {
        group_desc_t* desc = group_desc( sb, ngp );
        buffer_head*  bh   = sb_bread( sb, desc->bitmap_block );
    repeat:
        ino = find_next_bit( bh->data, MAX_BLOCK_BIT );
        if ( ino < MAX_BLOCK_BIT )
        {
            if ( !test_and_set_bit( ino, ( unsigned long* )bh->data ) )
                goto found;
            else
                goto repeat;
        }
        if ( ngp == sb->nr_block )
            ngp = 0;
        ++ngp;
    }
    return -1;
found:
    return NR_BLOCKS_PER_GROUP * ngp + ino;
}
int get_block( super_block_t* sb )
{
    return __get_block( sb, 0 );
}
unsigned long modify_inode_size( struct inode_t* inode, ssize_t sz )
{
    inode->size += sz;
    return inode->size;
}
unsigned long get_blocks( struct buffer_head* bh, struct inode_t* inode, unsigned long iblock )
{
    unsigned long* bmd;
    unsigned long* bte;
    cout << "get_blocks iblock = " << iblock << endl;
    if ( iblock < DIRECT_BLOCK )
    {
        bte = &inode->i_data[ iblock ];
    }
    else
    {
        iblock -= DIRECT_BLOCK;
        unsigned long bmd_offset = iblock >> BLOCK_INDEX_SHIFT;
        unsigned long bte_offset = iblock & BLOCK_INDEX_MASK;
        if ( !inode->i_data[ DIRECT_BLOCK ] )
            inode->i_data[ DIRECT_BLOCK ] = get_block( inode->sb );
        bmd = ( unsigned long* )sb_bread( inode->sb, inode->i_data[ DIRECT_BLOCK ] )->data + bmd_offset;
        if ( !*bmd )
        {
            *bmd = get_block( inode->sb );
        }
        printf( "bmd_block:%lu, bte_offset = %lu, addr = %p\n", *bmd, bte_offset, sb_bread( inode->sb, *bmd )->data );
        bte = ( unsigned long* )( sb_bread( inode->sb, *bmd )->data ) + bte_offset;
    }
    if ( !*bte )
    {
        *bte = get_block( inode->sb );
    }
    cout << "*bte = " << *bte << endl;
    unsigned long target = *bte;
    map_bh( bh, target );
    return target;
}
int write_inode( struct inode_t* inode, const char* buf, unsigned long len )
{
    struct buffer_head *head, *bh;
    unsigned long       pos = inode->size;

    unsigned long iblock = pos >> BLOCK_SHIFT;
    printf( "inode size = %lu,  write inode iblock=%lu\n", pos, iblock );
    head = get_blk_bh( inode, iblock );
    bh   = head;
    while ( bh )
    {
        if ( !bh->mapped )
            get_blocks( bh, inode, iblock );
        iblock++;
        printf( "bh->pblock(%lu)\n", bh->nr_block );
        bh = bh->next;
    }
    unsigned long start = pos & PAGE_MASK;
    unsigned long end   = start + len;
    if ( end >= PAGE_SIZE )
        end = PAGE_SIZE;
    len           = ( end - start ) & PAGE_MASK;
    void*    addr = ( char* )head->page->vir + start;
    unsigned block_start, block_end;
    for ( block_start = 0, block_end = 0, bh = head; block_start < PAGE_SIZE; )
    {
        block_end = block_start + BLOCK_SIZE;
        printf( "pblock(%lu), block_start:%lu-block_end:%lu\n", bh->nr_block, block_start, block_end );
        if ( block_start < end && block_end > start )
        {
            if ( block_end > end || block_start < start )
            {
                read_block( bh );
            }
        }
        block_start = block_end;
        bh          = bh->next;
    }
    memcpy( addr, buf, len );
    printf( "dump page(addr=%p): %s\n", addr, ( char* )addr );
    modify_inode_size( inode, len );
    unsigned long blk = 0;
    for ( block_start = 0, block_end = 0, bh = head; block_start < PAGE_SIZE; )
    {
        block_end = block_start + BLOCK_SIZE;
        printf( "block_start:%lu, block_end:%lu, start:%lu, end:%lu\n", block_start, block_end, start, end );
        if ( block_start < end && block_end > start )
        {
            write_block( bh );
            printf( "write block: bh->nr_block(%lu)\n", bh->nr_block );
            blk = bh->nr_block;
        }
        block_start = block_end;
        bh          = bh->next;
    }
    block_data_t* block_data = g_blocks + blk;
    printf( "blk:%lu data: %s\n", blk, ( const char* )block_data );
    printf( "============================================================\n" );
    return len;
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
    // ptr = g_blocks;
    // stats_global_block( ptr, 1024 );

    // ============== 创建 page ==============
    g_page_map = new struct page_t[ 1024 ];
    memset( g_page_map, 0x00, sizeof( struct page_t ) * 1024 );
    g_page = new page_data_t[ 1024 ];
    memset( g_page, 0x00, sizeof( page_data_t ) * 1024 );
    struct page_data_t* page_addr = g_page;
    struct page_t*      map_addr  = g_page_map;
    init_global_page( page_addr, map_addr, 1024 );
    memset( page_bitmap, 0x00, sizeof( page_bitmap ) );
    super_block_t* g_sb = NULL;
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
        if ( !g_sb )
            g_sb = sb;
    }

    struct inode_t* inode = alloc_inode( g_sb );
    char            buf[ BLOCK_SIZE ];
    char            tmp[ BLOCK_SIZE ];
    for ( int i = 0; i < 512; i++ )
    {
        snprintf( tmp, sizeof( tmp ), "I am a hero-%d\n", i );
        memcpy( buf, tmp, BLOCK_SIZE );
        write_inode( inode, buf, BLOCK_SIZE );
    }
    return 0;
}