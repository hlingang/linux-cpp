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

template < typename... Args > struct __S_params_saver
{
    using index_helper_t = typename __S_index_helper< sizeof...( Args ) >::type;
    tuple< Args... > m_tu;
    // 保存 动态参数 //
    __S_params_saver( Args&&... args ) : m_tu( std::forward< Args >( args )... ) {}
    int operator()( const char* fmt )  // 类型萃取 //
    {
        __display_params( fmt, typename __S_index_helper< sizeof...( Args ) >::type() );
        return 0;
    }
    // 此处通过显示的参数传递 实现 对类型 的萃取 //
    template < size_t... Is > void __display_params( const char* fmt, const __S_index_seq< Is... >& )
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
template < typename... Args > __S_params_saver< Args... > params_saver( Args&&... args )
{
    return __S_params_saver< Args... >( std::forward< Args >( args )... );
}

int main()
{
    auto s = params_saver( 1, "ab", 'c', 3.14 );
    s( "display saved parameters: $1=%d, $2=%s, $3=%c, $4=%.2f\n" );
}