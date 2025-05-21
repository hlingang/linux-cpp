#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>

using namespace std;

template < typename... Args > using valid_void_t = void;

template < typename T, typename = void >
/* 通配公式的默认参数， 用于匹配偏特化参数，同时完成参数的降维*输入参数只要一个就OK */
/* 默认参数的存在是为了实现参数的降维 */
struct __S_is_valid
{
    static const bool value = false;
};

template < typename T > struct __S_is_valid< T, valid_void_t< decltype( std::declval< T >().valid() ) > >
/*非独立模板参数* 以偏特化参数形式存在*/
/*偏特化参数 可以覆盖通用模板里面的默认参数*/
{
    static const bool value = true;
};

template < typename T > bool is_valid( const T& )
{
    return __S_is_valid< T >::value;
}

class Valid_clss
{
public:
    int valid()
    {
        return 0;
    }
};

int main()
{
    std::cout << __S_is_valid< int >::value << std::endl;
    std::cout << __S_is_valid< int, double >::value << std::endl;
    std::cout << __S_is_valid< decltype( std::declval< Valid_clss >().valid() ) >::value << std::endl;
    std::cout << is_valid( Valid_clss() ) << std::endl;
}