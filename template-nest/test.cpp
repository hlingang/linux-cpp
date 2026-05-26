
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

int main()
{
    __S_nest< 9, 99 >::type a;                         // 通过嵌套的方式获取到内层结构体的成员类型
    cout << "nest struct value: " << a.value << endl;  // 输出内层结构体的成员值
    __type< 9, 99 > _ta;
    cout << "nest struct value: " << _ta.value << endl;  // 输出内层结构体的成员值
    return 0;
}
