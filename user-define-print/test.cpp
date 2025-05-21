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

using namespace std;
///////////////////// C 语言实现 /////////////////////////////////

void c_debug( const char* fmt, ... )
{
    char buf[ 1024 ];
    memset( buf, 0x00, sizeof( buf ) );
    va_list args;  // 负责传递不定长参数 //
    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    printf( "[C][Info] %s\n", buf );
}

#define __macro_debug( fmt, args... )                \
    {                                                \
        printf( "[MACRO][Info] " fmt "\n", ##args ); \
    }

#define __macro_debug_1( fmt, ... )                           \
    {                                                         \
        printf( "[MACRO-1][Info] " fmt "\n", ##__VA_ARGS__ ); \
    }

///////////////////////////////////////////////////////////////////

// C 语言不支持在调用的时候动态展开 [参数列表],
// 必须在申明的时候 申明为 参数列表 //

template < typename... Args > struct __S_fmt_aux
{
    int operator()( char* buf, size_t size, const char* fmt, Args&&... args )
    {
        snprintf( buf, size, fmt, std::forward< Args >( args )... );
        return 0;
    }
};
template <> struct __S_fmt_aux<>
{
    int operator()( char* buf, size_t size, const char* fmt )
    {
        snprintf( buf, size, "%s", fmt );
        return 0;
    }
};
template < typename... Args > __S_fmt_aux< Args... > fmt_aux( Args&&... args )
{
    return __S_fmt_aux< Args... >();
}

template < typename... Args > void debug( const char* fmt, Args&&... args )
{
    char buf[ 1024 ];
    memset( buf, 0x00, sizeof( buf ) );
    // 偏特化模板结构体 实现动态代码的最小化分离 //
    fmt_aux( std::forward< Args >( args )... )( buf, sizeof( buf ), fmt, std::forward< Args >( args )... );
    /////////////////////////////////////////
    std::printf( "[C++][Info] %s\n", buf );
}

int main()

{
    debug( "Hello!" );
    debug( "HELLO-WORLD %d:%s", 2025, "abc" );

    c_debug( "Hello!" );
    c_debug( "HELLO-WORLD %d:%s", 2025, "abc" );

    __macro_debug( "Hello!" );
    __macro_debug( "HELLO-WORLD %d:%s", 2025, "abc" );

    __macro_debug_1( "Hello!" );
    __macro_debug_1( "HELLO-WORLD %d:%s", 2025, "abc" );
}