
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <numeric>
using namespace std;

template < int N, int M > struct __S_nest
{
    template < int K > struct __S_nest_inner
    {
        static const int value = K;
    };
    using type = __S_nest_inner< M + 1 >;  // 在嵌套的结构体中访问内层结构体的成员类型
};
template < int N, int M > using __type = typename __S_nest< N, M >::template __S_nest_inner< M + 2 >;

struct px
{
    int  x;
    void operator()() const  // 传入 const this 指针，表示该函数不会修改对象的成员变量
    {
        cout << "px operator()" << endl;
    }
    void operator()( int16_t )  // 非 const 成员函数，表示该函数可能会修改对象的成员变量
    {
        cout << "px operator() non-const" << endl;
    }
};

int main()
{
    __S_nest< 9, 99 >::type a;                         // 通过嵌套的方式获取到内层结构体的成员类型
    cout << "nest struct value: " << a.value << endl;  // 输出内层结构体的成员值
    __type< 9, 99 > _ta;
    cout << "nest struct value: " << _ta.value << endl;  // 输出内层结构体的成员值

    int  am = 100;
    int* p  = &am;
    // auto f  = [ am ]() -> void { am = 100; };
    px __px;
    __px();
    px* const __c_px = &__px;
    ( *__c_px )( 1 );
    int pi[ 2 ][ 3 ] = { { 1, 2, 3 }, { 4, 5, 6 } };
    cout << "pi[0][0]: " << pi[ 0 ][ 0 ] << endl;
    cout << "pi[1][2]: " << pi[ 1 ][ 2 ] << endl;
    int* pp = &pi[ 0 ][ 0 ];
    cout << "pp[0]: " << pp[ 0 ] << endl;
    cout << "pp[1]: " << pp[ 1 ] << endl;
    cout << "pp[2]: " << pp[ 2 ] << endl;
    return 0;
}
