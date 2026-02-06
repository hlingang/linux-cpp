#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>
#include <unistd.h>
#include <tuple>
#include <utility>

using namespace std;

#define min( x, y )                                               \
    ( {                                                           \
        /* 定义局部变量， 避免宏表达式操作带来副作用(比如++/--)*/ \
        __typeof__( x ) __x = ( x );                              \
        __typeof__( y ) __y = ( y );                              \
        ( void )( &__x == &__y ); /* 类型检查 */                  \
        __x < __y ? __x : __y;                                    \
    } )

#define max( x, y )                                               \
    ( {                                                           \
        /* 定义局部变量， 避免宏表达式操作带来副作用(比如++/--)*/ \
        __typeof__( x ) __x = ( x );                              \
        __typeof__( y ) __y = ( y );                              \
        ( void )( &__x == &__y ); /* 类型检查 */                  \
        __x > __y ? __x : __y;                                    \
    } )

#define type_check( T, v )                          \
    {                                               \
        __typeof__( T ) __dummy1;                   \
        __typeof__( v ) __dummy2;                   \
        /*显式丢弃返回值*类型检查失败，会编译失败*/ \
        ( void )( __dummy1 == __dummy2 );           \
    }

#define build_bug_on( cond ) ( void )sizeof( char[ 1 - 2 * !!( cond ) ] )  // 编译时断言