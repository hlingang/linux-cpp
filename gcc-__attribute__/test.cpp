#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <iostream>

inline void                                    test_inline() __attribute__( ( always_inline ) );
inline void __attribute__( ( always_inline ) ) test_inline()
{
    printf( "This is an inline function.\n" );
}

void                             test_unused() __attribute__( ( unused ) );
void __attribute__( ( unused ) ) test_unused()
{
    printf( "This is an unused function.\n" );
}

void                           test_weak() __attribute__( ( weak ) );
void __attribute__( ( weak ) ) test_weak()
{
    printf( "This is an unused function.\n" );
}

void                               test_noreturn() __attribute__( ( noreturn ) );
void __attribute__( ( noreturn ) ) test_noreturn()
{
    printf( "This is an unused function.\n" );
    exit( 0 );
}

void                                  test_constructor() __attribute__( ( constructor ) );
void __attribute__( ( constructor ) ) test_constructor()
{
    printf( "This is an construct function.\n" );
}

void                                 test_destructor() __attribute__( ( destructor ) );
void __attribute__( ( destructor ) ) test_destructor()
{
    printf( "This is an destroy function.\n" );
}

void test_unused_params( int a __attribute__( ( unused ) ) = 10 );
void test_unused_params( int a __attribute__( ( unused ) ) )
{
    printf( "This is an unused params function.\n" );
}

void                                   test_nonnull_params( int* a = nullptr ) __attribute__( ( nonnull( 1 ) ) );
void __attribute__( ( nonnull( 1 ) ) ) test_nonnull_params( int* a )
{
    printf( "This is an non-null params function.\n" );
}

static int g_i __attribute__( ( used ) ) = 9999;

#pragma pack( 2 )
struct aliged_struct
{
    int    a;
    char   b;
    double c;
} __attribute__( ( aligned( 16 ) ) );
#pragma pack()

int main()
{
    test_inline();  // 调用内联函数
    test_unused();  // 调用未使用的函数
    test_unused_params();
    // test_nonnull_params(); // warn
    test_nonnull_params( &g_i );
    aliged_struct s_a;
    printf( "aligend_struct p_a :%p\n", &s_a.a );
    printf( "aligend_struct p_b :%p\n", &s_a.b );
    printf( "aligend_struct p_c :%p\n", &s_a.c );
    printf( "aligend_struct size:%zu\n", sizeof( s_a ) );
    return 0;
}