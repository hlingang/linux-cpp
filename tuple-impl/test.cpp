#include <iostream>
#include <string>
#include "tuple-impl.h"

using namespace std;

/**

类型转换关系图示:
子类和任意父类间可以发生自动隐式转换
_Tuple_impl<0, int, double, std::string, char>           // get<0>(t) 函数传参发生自动隐式转化
    |                                                        |
    +-- _Tuple_impl<1, double, std::string, char>        // get<1>(t) 函数传参发生自动隐式转化
    |       |                                                |
    |       +-- _Tuple_impl<2, std::string, char>        // get<2>(t) 函数传参发生自动隐式转化
    |       |       |                                        |
    |       |        +-- _Tuple_impl<3, char>            // get<3>(t) 函数传参发生自动隐式转化
    |       |                |                               |
    |       |                +-- _Tuple_base<3, char>    // _Tuple_base::get()
    |       |
    |       |        +-- _Tuple_base<2, std::string>
    |       |
    |       +-- _Tuple_base<1, double>
    |
    +-- _Tuple_base<0, int>
 */

int main()
{
    ns_tuple::tuple< int, float, string, double, char > t( 99, 3.14, "abc", 100.01, 'A' );
    cout << ns_tuple::get< 0 >( t ) << endl;
    cout << ns_tuple::get< 1 >( t ) << endl;
    cout << ns_tuple::get< 2 >( t ) << endl;
    cout << ns_tuple::get< 3 >( t ) << endl;
    cout << ns_tuple::get< 4 >( t ) << endl;
}