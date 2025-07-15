#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <iostream>

#define __i_always_inline __attribute__( ( always_inline ) )
#define __i_noinline __attribute__( ( noinline ) )
#define __i_noreturn __attribute__( ( noreturn ) )
#define __i_nonnull( n ) __attribute__( ( nonnull( n ) ) )
#define __i_used __attribute__( ( used ) )
#define __i_unused __attribute__( ( unused ) )
#define __i_weak __attribute__( ( weak ) )
#define __i_warn_unused_result __attribute__( ( warn_unused_result ) )
#define __i_deprecated __attribute__( ( deprecated ) )
#define __i_constructor __attribute__( ( constructor ) )
#define __i_destructor __attribute__( ( destructor ) )
#define __i_aligned( n ) __attribute__( ( aligned( n ) ) )

//-------------------------------------------修饰函数---------------------------------------------------------//

inline void                   test_inline() __i_always_inline;
inline void __i_always_inline test_inline()
{
    printf( "This is an inline function.\n" );
}

void              test_noinline() __i_noinline;
void __i_noinline test_noinline()  // 函数不内联: 禁止编译期的内联优化动作
{
    printf( "This is an noinline function.\n" );
}

void            test_unused() __i_unused;
void __i_unused test_unused()
{
    printf( "This is an unused function.\n" );
}

void          test_weak() __i_weak;
void __i_weak test_weak()
{
    printf( "This is an weak function.\n" );
}

void              test_noreturn() __i_noreturn;
void __i_noreturn test_noreturn()  // 函数不返回: 禁止编译期的返回动作
{
    printf( "This is an noreturn function.\n" );
    exit( 0 );
}

int                        test_warn_unused_result() __i_warn_unused_result;
int __i_warn_unused_result test_warn_unused_result()
{
    printf( "This is an warn_unused_result function.\n" );
    return 1;
}

void                test_deprecated() __i_deprecated;
void __i_deprecated test_deprecated()
{
    printf( "This is an deprecated function.\n" );
}

void                 test_constructor() __i_constructor;
void __i_constructor test_constructor()
{
    printf( "This is an construct function.\n" );
}

void                test_destructor() __i_destructor;
void __i_destructor test_destructor()
{
    printf( "This is an destroy function.\n" );
}

//-------------------------------------------修饰函数形参---------------------------------------------------------//

void test_unused_params( int a __i_unused = 10 );
void test_unused_params( int a __i_unused )
{
    printf( "This is an unused params function.\n" );
}

void test_nonnull_params( int* a = nullptr ) __i_nonnull( 1 );
void __i_nonnull( 1 ) test_nonnull_params( int* a )  // 增加编译期的参数非空属性检查
{
    printf( "This is an non-null params function.\n" );
}

//--------------------------------------------修饰变量--------------------------------------------------------//

static int g_i __i_used = 9999;

//--------------------------------------------修饰结构体--------------------------------------------------------//

#pragma pack( 2 )
struct aliged_struct
{
    int    a;
    char   b;
    double c;
} __i_aligned( 16 );
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