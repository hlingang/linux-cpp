#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include <typeinfo>

using namespace std;

class __Undefined;

// type trait //
/*
1. 通过函数传递的实参对象进行类型萃取
2. 通过显示的类型参数进行类型萃取
*/
////////////////// 参数对象进行类型萃取 //////////////////////
template < typename Tp > void __S_params_type_traits( Tp&& __t )
{
    cout << "type: " << typeid( __t ).name() << endl;
    cout << "value: " << __t << endl;
}

////////////////// 显示模板参数进行类型萃取, 通过特化模板进行萃取 //////////////////////
template < typename Tp > struct __S_template_type_traits
{
    using type = __Undefined;  // 默认类型为未定义 //
};
template < template < typename, typename... > class TempClass, typename Tp >
struct __S_template_type_traits< TempClass< Tp > >
{
    using type = Tp;  // 显式的类型参数进行类型萃取 //
};

int main()
{
    int __t = 10;
    __S_params_type_traits( __t );  // 通过函数传递的实参对象进行类型萃取
    __S_params_type_traits(
        typename __S_template_type_traits< vector< double > >::type( 3.1415926 ) );  // 通过显式的类型参数进行类型萃取
}