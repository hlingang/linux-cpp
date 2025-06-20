#include <stdio.h>
/*
1. 元素间默认以最大方式对齐(由相邻的2个基本单元中的最大单元决定)
2. 结构体整体默认以最小方式对齐
*/

#pragma pack( 4 )
// 影响元素间的对齐最大对齐，
// 同时受到相邻元素间的最大对齐规则的限制
typedef struct
{
    char   b1;       // 1
    char   b2;       // 3
    int    a;        // 4
    char   b3;       // 1
    char   b4;       // 3
    double c;        // 8
    char   d[ 17 ];  // 20

} __attribute__( ( aligned( 2 ) ) ) Align_4_t;  // 影响结构体整体最小对齐， 不影响内部元素间的对齐
#pragma pack()

#pragma pack( 8 )  // 影响元素间的对齐，
// 同时受到相邻的最大基本单元对齐规则的限制 //
typedef struct
{
    char   b1;       // 1
    char   b2;       // 3
    int    a;        // 4
    char   b3;       // 1
    char   b4;       // 7
    double c;        // 8
    char   d[ 17 ];  // 24

} __attribute__( ( aligned( 2 ) ) ) Align_8_t;  // 影响结构体整体最小对齐， 不影响内部元素间的对齐
#pragma pack()

#define display_align( n, x )                                                \
    {                                                                        \
        printf( "======================ALIGN(%d)===================\n", n ); \
        printf( "sizeof(%s) = %zu\n", #x, sizeof( x ) );                     \
        printf( "&b1 = %p\n", &x.b1 );                                       \
        printf( "&b2 = %p\n", &x.b2 );                                       \
        printf( "&b3 = %p\n", &x.b3 );                                       \
        printf( "&b4 = %p\n", &x.b4 );                                       \
        printf( " &c = %p\n", &x.c );                                        \
    }

int main()
{
    Align_4_t align_4;
    Align_8_t align_8;
    display_align( 4, align_4 );
    display_align( 8, align_8 );
}