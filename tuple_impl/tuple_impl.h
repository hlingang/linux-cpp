#include <iostream>
#include <typeinfo>
namespace ns_tuple_impl
{
template<size_t Idx, typename... Types> struct tuple_impl; // 1个或者多个参数(通用模板)

template<size_t Idx, typename Head> struct BaseHead   // 2个参数(特例模板)
{
    BaseHead(): _M_head() {}
    BaseHead(const Head& head): _M_head(head) {std::cout << "Constructing BaseHead index " << Idx << ", Head: " << head << std::endl; }
    static Head& _Get_M_Head( BaseHead& t) { return t._M_head; }
    Head _M_head;
};

template<size_t Idx, typename Head, typename... Tail> // 2个或者多个参数(递推模板)
struct tuple_impl<Idx, Head, Tail...>:public tuple_impl<Idx + 1, Tail...>, BaseHead<Idx, Head>
{
    typedef tuple_impl<Idx + 1, Tail...> _Inherited;
    typedef BaseHead<Idx, Head> _Base;

    tuple_impl( const Head& h, Tail... tail):  _Inherited(tail...), _Base(h) {}
    static Head& _Get_M_Head( tuple_impl& t) { return _Base::_Get_M_Head(t); }
};

template<size_t Idx, typename Head> struct tuple_impl<Idx, Head>:public BaseHead<Idx, Head>{
    typedef BaseHead<Idx, Head> _Base;
    tuple_impl(): _Base() {}
    tuple_impl(const Head& head): _Base(head) {}
    static Head& _Get_M_Head( tuple_impl& t) { return _Base::_Get_M_Head(t); }
};

template<size_t Idx, typename Head, typename... Types>
Head& get(tuple_impl<Idx, Head, Types...>& t)
{
    return t._Get_M_Head(t);
}
}

