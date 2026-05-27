
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
1. 通用模板可以继承偏特化模板(递推)，也可以继承全特化模板。继承全特化版本的前，必须要有全特化模板的实现。
2. 通用模板和偏特化模板独立互斥，但是全特化模板不能单独存在，全特化模板在编译期会被视为具体类(具有完整实现的类)。
*/

/** ====================== inherite partial template  ====================== **/
template < typename Tp, typename Up > struct _S_inherite_partial : public _S_inherite_partial< Tp, int >
{
};

template < typename Tp > struct _S_inherite_partial< Tp, int >
{
    bool validate()
    {
        return false;
    }
    int _M_value;
};

/** ====================== inherite full template  ====================== **/

template < typename Tp, typename Up > struct _S_inherite_full;
template <> struct _S_inherite_full< int, int >;

template <> struct _S_inherite_full< int, int >
{
    bool validate()
    {
        return true;
    }
    int _M_value;
};
template < typename Tp, typename Up > struct _S_inherite_full : public _S_inherite_full< int, int >
{
};

int main()
{
    _S_inherite_partial< double, char > _s_partial;
    _S_inherite_full< int, int >        _s_full;
    cout << "validate: " << _s_partial.validate() << endl;
    cout << "validate: " << _s_full.validate() << endl;
    return 0;
}
