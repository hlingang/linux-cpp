
#include <iostream>
#include <invoker.h>
#include <random>
#include <numeric>
#include <unistd.h>
#include <tuple>
#include <sstream>
#include <vector>

using namespace std;

unsigned long val = 1000;

void main_loop()
{
    printf( "main-loop...\n" );
    sleep( 2 );
}

template < int... Is > struct __seq
{
};

template < int N, int... M > struct __seq_index : __seq_index< N - 1, N - 1, M... >
{
};

template < int... M > struct __seq_index< 0, M... >
{
    using type = struct __seq< M... >;
};
template < typename Tp > int __sput( stringstream& __ss, Tp&& __v )
{

    __ss << __v << ", ";
    return 0;
}

template < int... Is, typename... Args > void __print( const __seq< Is... >&, const tuple< Args... >& __t )
{
    stringstream  __ss;
    vector< int > __vec = { __sput( __ss, get< Is >( __t ) )... };
    cout << __ss.str() << endl;
}

template < typename... Args > void printTuple( tuple< Args... >& __t )
{

    __print( typename __seq_index< sizeof...( Args ) >::type(), __t );
}

int main()
{
    atexit( exit_1 );  // 反序注册
    atexit( exit_0 );
    main_loop();
    tuple< int, string, double > __t( 99, "hello-world", 3.14 );
    printTuple( __t );
    return 0;
}
