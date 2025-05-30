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
#include <list>
#include <string>
#include <typeinfo>

using namespace std;

class __Undefined;

// type trait //
/*
1. 通过函数传递的实参对象进行类型萃取
2. 通过显示的类型参数进行类型萃取
*/
template < typename Tp > void display_type( Tp&& __t )
{
    cout << "type: " << typeid( __t ).name() << endl;
    cout << "value: " << __t << endl;
}
////////////////// 参数对象进行类型萃取 //////////////////////
///////////////// 萃取 [一级] 类型信息
template < typename Tp > void __params_type_traits_1( const vector< Tp >& __t )
{
    display_type( __t[ 0 ] );  // 提取一级类型信息 //
}
///////////////// 萃取 [二级] 类型信息
template < template < typename... > class TempClass, typename Tp >
void __params_type_traits_2( const TempClass< Tp >& __t )
{
    for ( const auto& item : __t )
    {
        display_type( item );
    }
}
///////////////////// 偏特化实现 /////////////////////////////

// 偏特化实现[一级]类型萃取 //
template < typename Tp > struct __S_template_type_traits_1
{
    using type = __Undefined;  // 显式的类型参数进行类型萃取 //
};
template < typename Tp > struct __S_template_type_traits_1< vector< Tp > >
{
    using type = Tp;  // 显式的类型参数进行类型萃取 //
};
////////////////////////////////////////////////////////////////

////////////////// 显示模板参数进行类型萃取, 通过特化模板进行萃取 //////////////////////
// 偏特化实现[二级] 类型萃取 //
template < typename Tp > struct __S_template_type_traits_2
{
    using type = __Undefined;  // 默认类型为未定义 //
};
// 偏特化实现 二级嵌套 类型萃取 //
template < template < typename, typename... > class TempClass, typename Tp >
struct __S_template_type_traits_2< TempClass< Tp > >
{
    using type = Tp;  // 显式的类型参数进行类型萃取 //
};

int main()
{
    __params_type_traits_1( vector< int >{ 1, 2, 3 } );            // 函数实现 一级类型萃取
    __params_type_traits_2( vector< int >{ 10, 20, 30 } );         // 函数实现 二级类型萃取
    __params_type_traits_2( list< string >{ "HELLO", "WORLD" } );  // 函数实现 二级类型萃取
    display_type(
        typename __S_template_type_traits_1< vector< string > >::type( "VECTOR-STRING" ) );  // 偏特化实现 一级类型萃取
    display_type(
        typename __S_template_type_traits_2< list< string > >::type( "LIST-STRING" ) );  // 偏特化实现 二级类型萃取
    display_type(
        typename __S_template_type_traits_2< vector< double > >::type( 3.1415926 ) );  // 偏特化实现 二级类型萃取
}