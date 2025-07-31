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

// C 语言构建初始化结构体
//----------------------------//
// 1. 初始化列表自动构建对象机制
// 2. 通过函数返回自动构建对象机制

using namespace std;
///////////////////// C 语言实现 /////////////////////////////////
struct page_t
{
    char data[ 64 ];
};
#define PTE_BITS 3
#define PMD_BITS 3
#define PGD_BITS 3
#define PMD_SHIFT 3
#define PGD_SHIFT 6
#define PTE_MASK ( ( 1U << PTE_BITS ) - 1 )
#define PMD_MASK ( ( 1U << PMD_BITS ) - 1 )
#define PGD_MASK ( ( 1U << PGD_BITS ) - 1 )

using pgd_t = struct _S_pgd
{
    unsigned long pgd;  // 存储地址
};
using pmd_t = struct _S_pmd
{
    unsigned long pmd;  // 存储地址
};
using pte_t = struct _S_pte
{
    unsigned long pte;  // 存储地址
};

#define pgd_val( val ) ( val ).pgd
#define pmd_val( val ) ( val ).pmd
#define pte_val( val ) ( val ).pte

#define pgd_none( val ) ( !pgd_val( val ) )
#define pmd_none( val ) ( !pmd_val( val ) )
#define pte_none( val ) ( !pte_val( val ) )

#define make_pte( val )                 \
    ( pte_t )                           \
    {                                   \
        .pte = ( unsigned long )( val ) \
    }
#define make_pmd( val )                 \
    ( pmd_t )                           \
    {                                   \
        .pmd = ( unsigned long )( val ) \
    }
#define make_pgd( val )                 \
    ( pgd_t )                           \
    {                                   \
        .pgd = ( unsigned long )( val ) \
    }

pgd_t* g_pgd;
///////////////////////////////////////////////////////////////////

pgd_t* pgd_offset( pgd_t* pgd, unsigned long addr, unsigned long* pos = nullptr )
{
    if ( !pgd )
    {
        return nullptr;
    }
    unsigned long index = ( addr >> PGD_SHIFT ) & PGD_MASK;
    if ( pos )
        *pos = index;
    return g_pgd + index;
}

pmd_t* pmd_offset( pgd_t* pgd, unsigned long addr, unsigned long* pos = nullptr )
{
    if ( pgd_none( *pgd ) )
    {
        pmd_t* __pmd = ( pmd_t* )malloc( sizeof( page_t ) );
        memset( __pmd, 0x00, sizeof( page_t ) );
        *pgd = make_pgd( __pmd );
        // C 语言构建初始化结构体
        //----------------------------//
        // 1. 初始化列表自动构建对象机制
        // 2. 通过函数返回自动构建对象机制
    }
    unsigned long index = ( addr >> PMD_SHIFT ) & PMD_MASK;
    if ( pos )
        *pos = index;
    return ( pmd_t* )pgd_val( *pgd ) + index;
}

pte_t* pte_offset( pmd_t* pmd, unsigned long addr, unsigned long* pos = nullptr )
{
    if ( pmd_none( *pmd ) )
    {
        pte_t* __pte = ( pte_t* )malloc( sizeof( page_t ) );
        memset( __pte, 0x00, sizeof( page_t ) );
        *pmd = make_pmd( __pte );  // C 语言构建初始化结构体
    }
    unsigned long index = addr & PTE_MASK;
    if ( pos )
        *pos = index;
    return ( pte_t* )pmd_val( *pmd ) + index;
}

int main()
{
    g_pgd = ( pgd_t* )malloc( sizeof( page_t ) );
    memset( g_pgd, 0x00, sizeof( page_t ) );
    unsigned long i_pgd, i_pmd, i_pte;
    for ( int i = 0; i < 512; i++ )
    {
        pgd_t* pgd  = pgd_offset( g_pgd, i, &i_pgd );
        pmd_t* pmd  = pmd_offset( pgd, i, &i_pmd );
        pte_t* pte  = pte_offset( pmd, i, &i_pte );
        void*  page = ( void* )malloc( sizeof( page_t ) );
        memset( page, 0x00, sizeof( page_t ) );
        unsigned long* ptr = ( unsigned long* )page;
        *ptr               = i + 1;
        *( ptr + 1 )       = i + 512;
        *pte               = ( pte_t ){ ( unsigned long )page };
        printf( "VAL:%d, PGD[%lu]: %lx, PMD[%lu]: %lx, PTE[%lu]: %lx\n", i, i_pgd, pgd_val( *pgd ), i_pmd,
                pmd_val( *pmd ), i_pte, pte_val( *pte ) );
    }
    for ( int i = 0; i < 512; i++ )
    {
        pgd_t*         pgd = pgd_offset( g_pgd, i );
        pmd_t*         pmd = pmd_offset( pgd, i );
        pte_t*         pte = pte_offset( pmd, i );
        unsigned long* ptr = ( unsigned long* )pte_val( *pte );
        printf( "i:%d, pte=%p, val=%lu/%lu(delta:%lu)\n", i, ( void* )pte_val( *pte ), *ptr, *( ptr + 1 ),
                *( ptr + 1 ) - *ptr );
    }
    return 0;
}