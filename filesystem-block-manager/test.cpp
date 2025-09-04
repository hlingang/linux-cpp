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
#define MUTEX_SIZE ( 8 )
#define MUTEX_BIT ( 3 )

std::mutex g_mutex[ MUTEX_SIZE ];

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
#define init_global_data( ptr, val, sz ) memset( ptr, val, sizeof( *ptr ) * sz )
block_data_t*       g_blocks;
struct page_data_t* g_page;
struct page_t*      g_page_map;
unsigned long       page_bitmap[ 1024 ];
struct inode_t*     bdev;
//===============================================================

unsigned long last_bytes( unsigned long from, unsigned long len )
{
    unsigned long end = from + len;  // END 节点不会包含在内
    if ( end >= PAGE_SIZE )
        end = PAGE_SIZE;
    return end;
}

static unsigned long hash_ptr( const void* ptr, unsigned long bits )
{
    unsigned long val  = ( unsigned long )ptr;
    unsigned long mask = ( 1UL << bits ) - 1;
    val ^= ( val >> bits );
    return val & mask;
}
void set_bit( unsigned long nr, unsigned long* addr )
{
    std::mutex* mtx = &g_mutex[ hash_ptr( addr, MUTEX_BIT ) ];
    mtx->lock();
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    *ptr |= ( 1UL << offset );
    mtx->unlock();
}

void clear_bit( unsigned long nr, unsigned long* addr )
{
    std::mutex* mtx = &g_mutex[ hash_ptr( addr, MUTEX_BIT ) ];
    mtx->lock();
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    *ptr &= ~( 1UL << offset );
    mtx->unlock();
}

int test_bit( unsigned long nr, unsigned long* addr )
{
    unsigned long* ptr    = addr + NR_BIT_WORD( nr );
    unsigned long  offset = NR_BIT_OFFSET( nr );
    unsigned long  val    = ( *ptr ) & ( 1UL << offset );
    return val != 0;
}
int test_and_set_bit( unsigned long nr, unsigned long* addr )
{
    std::mutex* mtx = &g_mutex[ hash_ptr( addr, MUTEX_BIT ) ];
    mtx->lock();
    unsigned long* ptr       = addr + NR_BIT_WORD( nr );
    unsigned long  offset    = NR_BIT_OFFSET( nr );
    int            old_value = ( ( *ptr ) & ( 1UL << offset ) ) != 0;
    *ptr |= ( 1UL << offset );
    mtx->unlock();
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

struct buffer_head* create_page_buffers( space_mapping* mapping, struct page_t* page, unsigned long index,
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
        create_page_buffers( ( struct space_mapping* )&inode->i_mapping, page, index, block );
    }
    return page->bh;
}
static void init_page_buffers( struct buffer_head* bh, unsigned long iblock )
{
    while ( bh )
    {
        if ( !bh->mapped )
            map_bh( bh, iblock );
        iblock++;
        bh = bh->next;
    }
}
struct buffer_head* sb_bread( struct super_block_t* sb, unsigned long iblock )
{
    unsigned long  index = iblock >> ( PAGE_SHIFT - BLOCK_SHIFT );
    struct page_t* page  = find_get_page( bdev, index );
    page->index          = index;
    unsigned block       = index << ( PAGE_SHIFT - BLOCK_SHIFT );
    if ( !page )
        return NULL;
    if ( !page->bh )
        create_page_buffers( ( struct space_mapping* )&bdev->i_mapping, page, index, block );
    init_page_buffers( page->bh, block );
    struct buffer_head* bh = page->bh;
    while ( bh != NULL )
    {
        if ( block == iblock )
            break;
        block++;
        bh = bh->next;
    }
    if ( !bh )
        return NULL;
    if ( !bh->uptodate )
        read_block( bh );
    return bh;
}
static inline unsigned long group_start_block( unsigned long ngp )
{
    return ngp * NR_BLOCKS_PER_GROUP + START_BLOCK;
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

static inline unsigned long find_next_bit( void* addr, unsigned long size, unsigned long start_bit )
{
    unsigned long* pos = ( unsigned long* )addr;
    unsigned long  value;
    unsigned long  index        = 0;
    unsigned long  last_offset  = ( size - 1 ) % NR_BITS_PER_LONG;
    unsigned long  last_index   = ( size - 1 ) / NR_BITS_PER_LONG;
    unsigned long  start_offset = start_bit % NR_BITS_PER_LONG;
    unsigned long  start_index  = start_bit / NR_BITS_PER_LONG;
    unsigned long  search_len;
    unsigned long  mask;
    unsigned long  result;
    pos += start_index;
    value = ~*pos;
    mask  = ~0UL << start_offset;
    while ( !( value = ( ~*pos ) & mask ) )
    {
        ++pos;
        ++index;
        mask = ~0UL;
        if ( index > last_index )
            goto out;
    }
    search_len = NR_BITS_PER_LONG;
    if ( index == last_index )
    {
        value &= ( ( 1UL << ( last_offset + 1 ) ) - 1 );
        search_len = last_offset + 1;
    }
    result = ~value;
    for ( unsigned long i = 0; i < search_len; i++ )
    {
        if ( !test_bit( i, &result ) )
            return i + index * NR_BITS_PER_LONG;
    }
out:
    return size;
}
//******* 这里通过参数和返回值进行默认的类型转换(避免显式的类型转换) *********//
static int find_next_block( void* addr, int size, unsigned long start_blk )
{
    int target_blk = find_next_bit( addr, size, start_blk );
    if ( target_blk < size )
        return target_blk;
    return -1;
}
int __get_block( super_block_t* sb, unsigned long ngp )
{
    if ( ngp >= sb->nr_groups )
        ngp = 0;
    int ino;
    for ( unsigned long i = 0; i < sb->nr_groups; i++ )
    {
        group_desc_t* desc = group_desc( sb, ngp );
        buffer_head*  bh   = sb_bread( sb, desc->bitmap_block );
    repeat:
        ino = find_next_block( bh->data, MAX_BLOCK_BIT, 22 );
        if ( ino >= 0 )
        {
            if ( !test_and_set_bit( ino, ( unsigned long* )bh->data ) )
                goto found;
            else
                goto repeat;
        }
        ++ngp;
        if ( ngp == sb->nr_block )
            ngp = 0;
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
        {
            inode->i_data[ DIRECT_BLOCK ] = get_block( inode->sb );
            printf( "[Alloc Index Block:%lu]\n", inode->i_data[ DIRECT_BLOCK ] );
        }
        bmd = ( unsigned long* )sb_bread( inode->sb, inode->i_data[ DIRECT_BLOCK ] )->data + bmd_offset;
        if ( !*bmd )
        {
            *bmd = get_block( inode->sb );
            printf( "[Alloc Indirect-Index Block:%lu]\n", *bmd );
        }
        bte = ( unsigned long* )( sb_bread( inode->sb, *bmd )->data ) + bte_offset;
    }
    if ( !*bte )
    {
        *bte = get_block( inode->sb );
        printf( "[Alloc User Block:%lu]\n", *bte );
    }
    unsigned long target = *bte;
    map_bh( bh, target );
    return target;
}
int write_inode( struct inode_t* inode, const char* buf, unsigned long len )
{
    unsigned long offset = 0;
    unsigned long loop   = 0;
    do
    {
        struct buffer_head* bh;
        unsigned long       pos    = inode->size;
        unsigned long       index  = pos >> PAGE_SHIFT;
        unsigned long       iblock = index << ( PAGE_SHIFT - BLOCK_SHIFT );
        struct page_t*      page   = find_get_page( inode, index );
        if ( !page )
            return -1;
        if ( !page->bh )
        {
            create_page_buffers( &inode->i_mapping, page, index, iblock );  // 传入的 iblock 是起始逻辑block
        }
        bh = page->bh;
        while ( bh )
        {
            if ( !bh->mapped )
                get_blocks( bh, inode, iblock );
            iblock++;
            bh = bh->next;
        }
        unsigned long from   = pos & PAGE_MASK;
        unsigned long to     = last_bytes( from, len );
        unsigned long nbytes = to - from;
        len -= nbytes;
        void*         addr = ( char* )page->vir + from;
        unsigned long block_start, block_end;
        // prepare to write
        for ( block_start = 0, bh = page->bh; block_start < PAGE_SIZE; )
        {
            block_end = block_start + BLOCK_SIZE;
            if ( block_start < to && block_end > from )
            {
                // 交集区域(不包含[=]区域)
                if ( block_end > to || block_start < from )
                {
                    read_block( bh );
                }
            }
            block_start = block_end;
            bh          = bh->next;
        }
        //========= 以 -[page]- 为单位进行拷贝操作 ===========//
        memcpy( addr, buf + offset, nbytes );
        offset += nbytes;
        modify_inode_size( inode, nbytes );
        // 磁盘 I/O 操作
        unsigned long blk[ PAGE_SIZE / BLOCK_SIZE ];
        char          blk_cache[ BLOCK_SIZE + 1 ];
        memset( blk, 0x00, sizeof( blk ) );
        for ( block_start = 0, bh = page->bh; block_start < PAGE_SIZE; )
        {
            block_end = block_start + BLOCK_SIZE;
            if ( block_start < to && block_end > from )
            {
                // 位于读写区域的 [target] block
                write_block( bh );
                printf( "[Block I/O]: bh->nr_block(%lu)\n", bh->nr_block );
                blk[ block_start / BLOCK_SIZE ] = bh->nr_block;
            }
            block_start = block_end;
            bh          = bh->next;
        }
        for ( unsigned idx = 0; idx < PAGE_SIZE / BLOCK_SIZE; idx++ )
        {
            if ( !blk[ idx ] )
                continue;
            block_data_t* block_data = g_blocks + blk[ idx ];
            memcpy( blk_cache, block_data, BLOCK_SIZE );
            blk_cache[ BLOCK_SIZE ] = 0;
            printf( "[Block:%lu][Page:%lu] data: \n%s\n", blk[ idx ], index, ( const char* )blk_cache );
        }
        printf( "----------------- Write-Loop-%lu End ----------------------\n", loop++ );
    } while ( len > 0 );
    printf( "============================ Write-End ==============================\n" );
    return offset;  // 实际写入的数据量
}
//===============================================================

int main()
{
    // ============== 创建 bdev ==============
    bdev = new struct inode_t;
    memset( bdev, 0x00, sizeof( struct inode_t ) );
    // ============== 创建 block ==============
    g_blocks = new block_data_t[ 1024 ];
    init_global_data( g_blocks, 0x00, 1024 );
    // ============== 创建 page map ==============
    g_page_map = new struct page_t[ 1024 ];
    init_global_data( g_page_map, 0x00, 1024 );
    // ============== 创建 page data ==============
    g_page = new page_data_t[ 1024 ];
    init_global_data( g_page, 0x00, 1024 );
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
        sb->nr_groups           = NR_GROUPS;
        sb->nr_blocks_per_group = NR_BLOCKS_PER_GROUP;
        sb->nr_start_block      = START_BLOCK;
        sb->nr_block            = group_start_block( ngp ) + START_BLOCK;  // 静态预留block(启动过程已经静态写入数据)
        sb->sbh                 = sb_bread( sb, sb->nr_block );
        sb->sbi                 = ( char* )sb->sbh->data + sb->sbh->offset;
        sb->group_desc          = new struct buffer_head*;
        unsigned long group_desc_block =
            group_start_block( ngp ) + GROUP_DESC_BLOCK;  // 静态预留block(启动过程中已经静态写入数据)
        sb->group_desc[ 0 ] = sb_bread( sb, group_desc_block );
        init_group_desc( sb, sb->group_desc[ 0 ] );  // 模拟 [group-desc-block] 的初始化
        stats_desc_block_data( group_desc_block, ngp );
        group_desc_t*       desc         = group_desc( sb, ngp );
        unsigned long       bitmap_block = desc->bitmap_block;
        struct buffer_head* bh_bitmap    = sb_bread( sb, bitmap_block );
        init_block_bitmap( sb, bh_bitmap );
        if ( !g_sb )
            g_sb = sb;
    }
    struct inode_t* inode = alloc_inode( g_sb );
    char            buf[ PAGE_SIZE * 5 / 3 ];
    char            tmp[ BLOCK_SIZE ];
    memset( buf, 0x00, sizeof( buf ) );
    unsigned long sz    = 0;
    char*         x_ptr = buf;
    for ( int i = 0; i < 512; i++ )
    {
        snprintf( tmp, sizeof( tmp ), "I am a hero-%d\n", i );
        memcpy( x_ptr, tmp, strlen( tmp ) );
        x_ptr += strlen( tmp );
        sz += strlen( tmp );
        if ( sz >= sizeof( buf ) )
        {
            buf[ sizeof( buf ) - 2 ] = '\n';  //** 最后一行 换行 **//
            buf[ sizeof( buf ) - 1 ] = 0;
            break;
        }
    }
    printf( "/////////////////// Write Large Buffer ///////////////////////\n" );
    write_inode( inode, buf, strlen( buf ) );
    printf( "/////////////////// Write Small Buffer //////////////////////\n" );
    unsigned nstep  = 618;
    unsigned offset = 0;
    while ( offset + nstep < sizeof( buf ) )
    {
        write_inode( inode, buf + offset, nstep );
        offset += nstep;
    }
    if ( offset < sizeof( buf ) )
        write_inode( inode, buf + offset, sizeof( buf ) - offset );
    return 0;
}