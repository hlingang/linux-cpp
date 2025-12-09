#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>
#include <unistd.h>
#include <tuple>
#include <utility>

using namespace std;

struct BaseInvoker
{
    // virtual interface define
    virtual void operator()() = 0;
};
// 萃取 tuple size
template < typename... Args > struct _S_tuple_size
{
    static const size_t value = sizeof...( Args );
};
// 模板特例进行类型萃取 //
template < typename... Args > struct _S_tuple_size< tuple< Args... > >
{
    static const size_t value = sizeof...( Args );
};
// index 序列保存 //
template < size_t... Is > struct _S_tuple_index
{
};
// 递归获取序列 //

// 通用模板
template < size_t... Is > struct _S_tuple_seq
{
    using type = __undefined;
};
// 递归特例模板
template < size_t N, size_t... M > struct _S_tuple_seq< N, M... > : _S_tuple_seq< N - 1, N - 1, M... >
{
};
// 特例模板
template < size_t... Is > struct _S_tuple_seq< 0, 0, Is... >
{
    // 保存类型信息
    using type = _S_tuple_index< 0, Is... >;
};
template < size_t N > typename _S_tuple_seq< N >::type _S_make_tuple_index()
{
    // 通过返回实例对象传递类型信息
    return typename _S_tuple_seq< N >::type();
}
// 将 tuple 打包成 可调用对象
template < typename _Tuple > struct Invoker : public BaseInvoker
{
    _Tuple _M_tuple;  // 保存对象实例 传递信息
    Invoker( _Tuple&& _t ) : _M_tuple( std::move( _t ) ) {}
    // 接口的单一化分离 //
    template < size_t... Is > void _M_invoke( _S_tuple_index< Is... > )
    {
        return std::__invoke( get< Is >( _M_tuple )... );
    }
    void operator()()
    {
        return _M_invoke( _S_make_tuple_index< _S_tuple_size< decltype( _M_tuple ) >::value >() );
    }
};
class InvokerInterface
{
public:
    template < typename Callable, typename... Args > InvokerInterface( Callable&& _f, Args... args )
    {
        using _Tuple = tuple< Callable, Args... >;
        // 通过创建对象实例 传递信息 [入参] -> [tuple对象] -> [invoker对象]
        _S_invoker =
            _S_make_invoker( _S_make_tuple( std::forward< Callable >( _f ), std::forward< Args >( args )... ) );
        _S_invoker->operator()();
    }
    template < typename Callable, typename... Args >
    tuple< Callable, Args... > _S_make_tuple( Callable&& _f, Args... args )
    {
        using _Tuple = tuple< Callable, Args... >;
        return _Tuple{ std::forward< Callable >( _f ), std::forward< Args >( args )... };
    }
    template < typename _Tuple > Invoker< _Tuple >* _S_make_invoker( _Tuple&& _t )
    {
        return new Invoker< _Tuple >( std::forward< _Tuple >( _t ) );
    }
    ~InvokerInterface()
    {
        delete _S_invoker;
        _S_invoker = nullptr;
    }

private:
    // 基类指针保存 [具体实现对象指针]
    BaseInvoker* _S_invoker;
};