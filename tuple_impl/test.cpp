#include <unistd.h>
#include <tuple_impl.h>
#include <iostream>
#include <string>
#include <typeinfo>

/**

类型转换关系图示:
子类和任意父类间可以发生自动隐式转换
tuple_impl<0, int, double, std::string, char>           // (t1)
    |                                                        |
    +-- tuple_impl<1, double, std::string, char>             |
    |       |                                                |
    |       +-- tuple_impl<2, std::string, char>        // get<2>(t1)
    |       |       |                                        |
    |       |        +-- tuple_impl<3, char>                 |
    |       |                |                               |
    |       |                +-- BaseHead<3, char>     // BaseHead::_Get_M_head()
    |       |
    |       |        +-- BaseHead<2, std::string>
    |       |
    |       +-- BaseHead<1, double>     
    |
    +-- BaseHead<0, int>
 */

int main()
{
    ns_tuple_impl::tuple_impl<0, int, double, std::string, char> t1(42, 3.14, "Hello, World!", 'A');
    std::cout << ns_tuple_impl::get<2>(t1) << std::endl;
    return 0;
}