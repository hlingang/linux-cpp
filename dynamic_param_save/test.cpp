#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>

using namespace std;

// 如何接受动态参数并且进行保存 和 传递

template < size_t... Is > struct __S_index_seq
{
};
template < size_t M, size_t... N > struct __S_index_helper : __S_index_helper< M - 1, M - 1, N... >
{
};
template < size_t... N > struct __S_index_helper< 0, 0, N... >
{
    using type = __S_index_seq< 0, N... >;
};

template < typename... Args > struct __S_fmt_aux
{
    using index_helper_t = typename __S_index_helper< sizeof...( Args ) >::type;
    tuple< Args... > m_tu;
    __S_fmt_aux( Args&&... args ) : m_tu( std::forward< Args >( args )... ) {}
    int operator()( const char* fmt )  // 类型萃取 //
    {
        index_helper_t __instance;
        __display_params( fmt, __instance );
        return 0;
    }
    template < size_t... Is > void __display_params( const char* fmt, __S_index_seq< Is... >& t )
    {
        printf( fmt, get< Is >( m_tu )... );
    }
    typename __S_index_helper< sizeof...( Args ) >::type
    /*此处无法通过返回值进行类型萃取? */
    /* template<size_t...Is> __S_index_seq<Is...> */
    get_index_seq()
    {
        return typename __S_index_helper< sizeof...( Args ) >::type();
    }
};
template < typename... Args > __S_fmt_aux< Args... > fmt_aux( Args... args )
{
    return __S_fmt_aux< Args... >( std::forward< Args >( args )... );
}

int main()

{
    auto s = fmt_aux( 1, "ab", 'c', 3.14 );
    s( "display parameters: $1=%d, $2=%s, $3=%c, $4=%.2f\n" );
}