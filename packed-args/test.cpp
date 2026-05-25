
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <numeric>
using namespace std;

/*
1. 万能引用传参会保留参数的原始类型，不会进行退化，数组会以数组引用的方式传递
2. remove_cv 只能移除顶层的const，不能移除底层的const
3. 折叠参数的展开的时候，可以对参数进行多重处理
*/

template < bool cond > struct __m_cond_type
{
    static const bool value = cond;
};
using __m_false_type = __m_cond_type< false >;
using __m_true_type  = __m_cond_type< true >;
template < bool... > struct __And_ops__ : __m_false_type
{
};

template < bool cond > struct __And_ops__< cond > : __m_cond_type< cond >
{
};
template < bool cond1, bool cond2 > struct __And_ops__< cond1, cond2 > : __m_cond_type< cond1 && cond2 >
{
};
template < bool cond1, bool... cond2 >
struct __And_ops__< cond1, cond2... > : __And_ops__< cond1, __And_ops__< cond2... >::value >
{
};
template < bool... > struct __Or_ops__ : __m_false_type
{
};
template < bool cond > struct __Or_ops__< cond > : __m_cond_type< cond >
{
};

template < bool cond1, bool cond2 > struct __Or_ops__< cond1, cond2 > : __m_cond_type< cond1 || cond2 >
{
};
template < bool cond1, bool... cond2 >
struct __Or_ops__< cond1, cond2... > : __Or_ops__< cond1, __Or_ops__< cond2... >::value >
{
};

template < typename T > void convertible_check( T&& )
{
    cout << "convert string      : " << std::is_convertible< typename std::decay< T >::type, std::string >::value
         << endl;
    cout << "convert char*       : " << std::is_convertible< typename std::decay< T >::type, char* >::value << endl;
    cout << "convert const char* : " << std::is_convertible< typename std::decay< T >::type, const char* >::value
         << endl;
    cout << "convert char[]      : " << std::is_convertible< typename std::decay< T >::type, char ( & )[] >::value
         << endl;
    cout << "-----------------------------" << endl;
}
template < typename... Args > void packed_args_test( Args&&... args )
{
    static_assert( sizeof...( args ) > 0, "Expected at least one argument" );
    static_assert(
        __And_ops__< __Or_ops__<
            std::is_convertible< typename std::decay< Args >::type, std::string >::value,
            std::is_convertible< typename std::decay< Args >::type, char* >::value,
            std::is_convertible< typename std::decay< Args >::type, const char* >::value >::value... >::value,
        "Expected all arguments to be convertible to std::string or char*" );
    printf( "input args count: %zu\n", sizeof...( args ) );
    initializer_list< int > __attribute__( ( unused ) ) dummy{ ( convertible_check( args ), 0 )... };
}

int main()
{
    std::string s( "iklueiofu" );
    char        c[] = "ioeu8eu";
    packed_args_test( c );  // Example usage of the variadic function
#ifdef WIN32
    printf( "**WINDOWS ENBALE(32)**" );
#elif defined( WIN64 )
    printf( "**WINDOWS ENBALE(64)**" );
#endif
#if defined( WIN32 ) || defined( WIN64 )
    char c;
    std::cin >> c;
#endif
}