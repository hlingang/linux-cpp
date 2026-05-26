
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <numeric>
using namespace std;

template < int N > struct __S_base
{
    const static int value = N;
    static void      test()
    {
        cout << "N: " << N << endl;
    }
};
template < int N > struct __S_recursive : public __S_base< N >, public __S_recursive< N - 1 >
{
    static void test()
    {
        __S_base< N >::test();
        __S_recursive< N - 1 >::test();
    }
};
template <> struct __S_recursive< 0 > : public __S_base< 0 >
{
};

template < int N > void recursive()
{
    if constexpr (
        N > 1 )  // 编译器条件分支，在编译阶段就会进行条件判断，满足条件的分支会被编译，不满足条件的分支会被丢弃
    {
        cout << "N: " << N << endl;
        recursive< N - 1 >();
    }
    else
    {
        cout << "N: " << N << endl;
    }
}

int main()
{
    recursive< 9 >();
    cout << "--------------I am split line---------------" << endl;
    __S_recursive< 9 >::test();
    return 0;
}
