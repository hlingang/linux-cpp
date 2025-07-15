#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
//-------------------------------------------修饰函数---------------------------------------------------------//

inline void                                    test_inline() __attribute__( ( always_inline ) );
inline void __attribute__( ( always_inline ) ) test_inline()
{
    printf( "This is an inline function.\n" );
}

void                               test_noinline() __attribute__( ( noinline ) );
void __attribute__( ( noinline ) ) test_noinline()  // 函数不内联: 禁止编译期的内联优化动作
{
    printf( "This is an noinline function.\n" );
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
void __attribute__( ( noreturn ) ) test_noreturn()  // 函数不返回: 禁止编译期的返回动作
{
    printf( "This is an unused function.\n" );
    exit( 0 );
}

int                                         test_warn_unused_result() __attribute__( ( warn_unused_result ) );
int __attribute__( ( warn_unused_result ) ) test_warn_unused_result()
{
    printf( "This is an warn_unused_result function.\n" );
    return 1;
}

void                                 test_deprecated() __attribute__( ( deprecated ) );
void __attribute__( ( deprecated ) ) test_deprecated()
{
    printf( "This is an deprecated function.\n" );
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

//-------------------------------------------修饰函数形参---------------------------------------------------------//

void test_unused_params( int a __attribute__( ( unused ) ) = 10 );
void test_unused_params( int a __attribute__( ( unused ) ) )
{
    printf( "This is an unused params function.\n" );
}

void                                   test_nonnull_params( int* a = nullptr ) __attribute__( ( nonnull( 1 ) ) );
void __attribute__( ( nonnull( 1 ) ) ) test_nonnull_params( int* a )  // 增加编译期的参数非空属性检查
{
    printf( "This is an non-null params function.\n" );
}

//--------------------------------------------修饰变量--------------------------------------------------------//

static int g_i __attribute__( ( used ) ) = 9999;

//--------------------------------------------修饰结构体--------------------------------------------------------//

#pragma pack( 2 )
struct aliged_struct
{
    int    a;
    char   b;
    double c;
} __attribute__( ( aligned( 16 ) ) );
#pragma pack()
//---------------------------------------------THE-END-------------------------------------------------------//

int main()
{
    test_inline();    // 调用内联函数
    test_noinline();  // 调用非内联函数
    test_unused();    // 调用未使用的函数
    test_unused_params();
    test_nonnull_params( &g_i );
    // test_nonnull_params();      // must warn within compile
    // test_deprecated();          // must warn within compile
    // test_warn_unused_result();  // must warn within compile
    aliged_struct s_a;
    printf( "aligend_struct p_a :%p\n", &s_a.a );
    printf( "aligend_struct p_b :%p\n", &s_a.b );
    printf( "aligend_struct p_c :%p\n", &s_a.c );
    printf( "aligend_struct size:%zu\n", sizeof( s_a ) );
    return 0;
}