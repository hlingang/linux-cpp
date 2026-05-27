
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
1. 通用模板可以继承偏特化模板(递推)，但是不能继承全特化模板。
2. 通用模板和偏特化模板独立互斥，但是全特化模板不能单独存在，全特化模板在编译期会被视为具体类(具有完整实现的类)。
*/

// template < typename Tp, typename Up > struct _S_inherite : public _S_inherite< int, int >
// {
// };

template < typename Tp, typename Up > struct _S_inherite : public _S_inherite< Tp, int >
{
};

template < typename Tp > struct _S_inherite< Tp, int >
{
    bool validate()
    {
        return false;
    }
    int _M_value;
};

template <> struct _S_inherite< int, int >
{
    bool validate()
    {
        return false;
    }
    int _M_value;
};

int main()
{
    _S_inherite< double, char > _s;
    cout << "validate: " << _s.validate() << endl;
    return 0;
}
