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

// C 语言不支持在调用的时候动态展开 [参数列表]

template <typename... Args>
void debug(const char *fmt, Args &&...args)
{
    std::printf(fmt, std::forward<Args>(args)...); // std::forward<Args>(args)...
}

template <>
void debug(const char *fmt)
{
    std::printf("%s", fmt);
}

template <typename... Args>
using valid_void_t = void;

template <typename T, typename = void>
/* 通配公式的默认参数， 用于匹配偏特化参数，同时完成参数的降维*输入参数只要一个就OK */
/* 默认参数的存在是为了实现参数的降维 */
struct __S_is_valid
{
    static const bool value = false;
};

template <typename T>
struct __S_is_valid<T,
                    valid_void_t<decltype(std::declval<T>().valid())>>
/*非独立模板参数* 以偏特化参数形式存在*/
/*偏特化参数 可以覆盖通用模板里面的默认参数*/
{
    static const bool value = true;
};

template <typename T>
bool is_valid(const T &)
{
    return __S_is_valid<T>::value;
}

class Valid_clss
{
public:
    int valid()
    {
        return 0;
    }
};

template <size_t... Is>
struct __S_index_seq
{
};

template <size_t M, size_t... N>
struct __S_index_helper : __S_index_helper<M - 1, M - 1, N...>
{
};
template <size_t... N>
struct __S_index_helper<0, 0, N...>
{
    using type = __S_index_seq<0, N...>;
};

template <typename... Args>
struct __S_fmt_aux
{
    tuple<Args...> m_tu;
    __S_fmt_aux(Args &&...args) : m_tu(std::forward<Args>(args)...) {}
    template <size_t... Is>
    int operator()(const __S_index_seq<Is...> &) // 类型萃取 //
    {
        printf("yes===%d, %s\n", get<Is>(m_tu)...);
        return 0;
    }
    typename __S_index_helper<sizeof...(Args)>::type
    /*此处无法通过返回值进行类型萃取? */
    /* template<size_t...Is> __S_index_seq<Is...> */
    get_index_seq()
    {
        return typename __S_index_helper<sizeof...(Args)>::type();
    }
};
template <typename... Args>
__S_fmt_aux<Args...>
fmt_aux(Args... args)
{
    return __S_fmt_aux<Args...>(std::forward<Args>(args)...);
}

int main()

{
    debug("Hello!\n");
    std::cout << __S_is_valid<int>::value << std::endl;
    std::cout << __S_is_valid<int, double>::value << std::endl;
    std::cout << __S_is_valid<decltype(std::declval<Valid_clss>().valid())>::value << std::endl;
    std::cout << is_valid(Valid_clss()) << std::endl;
    std::shared_ptr<char[]> am(new char[10], [](char *p)
                               { delete p; });
    std::shared_ptr<char[]> an = am;
    auto s = fmt_aux(1, "ab");
    s(s.get_index_seq());
}