
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
平凡类型重载: 值优先级 > 引用优先级
非平凡类型的重载: 值优先级 == 引用优先级(两者同时出现编译器会报错)
*/
struct S
{
};

template < typename T > void test_str( T&& t )
{
    printf( "test_str(T t) : %s\n", typeid( T ).name() );
}

// void test_str( string& s )
// {
//     printf( "test_str( string& s) : %s\n", typeid( string& ).name() );
// }

void test_str( string s )
{
    printf( "test_str( string s) : %s\n", typeid( string& ).name() );
}

void test_int( int a )
{
    printf( "test_int(int a) : %s\n", typeid( int ).name() );
}

void test_int( int& a )
{
    printf( "test_int(int& a) : %s\n", typeid( int ).name() );
}

// void test_struct( S a )
// {
//     printf( "test_struct(S a) : %s\n", typeid( S ).name() );
// }

void test_struct( S& a )
{
    printf( "test_struct(S& a) : %s\n", typeid( S ).name() );
}

int main()
{

    std::string str( "iklueiofu" );
    test_str( str );  // Calls the template function, not the string overload
    test_int( 10 );
    S _s;
    test_struct( _s );
    return 0;
}