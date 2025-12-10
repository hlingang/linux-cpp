#include <utility>
namespace ns_tuple
{
template < size_t N, typename Tp > struct _Tuple_base
{
    Tp value;  // 存储具体类型的值
    // 具体类型存储值的初始化
    _Tuple_base( Tp&& __value ) : value( std::forward< Tp >( __value ) ) {}
    // 获取具体类型存储的值，同时完成入参向基类的自动类型转换
    static Tp get( const _Tuple_base& _tb )
    {
        return _tb.value;
    }
};
// 通用模板
template < size_t N, typename... Args > struct _Tuple_impl
{
};
// 特例模板
template < size_t N, typename Tp > struct _Tuple_impl< N, Tp > : _Tuple_base< N, Tp >
{
    using _Base = _Tuple_base< N, Tp >;  // 声明 _Base 类型
    _Tuple_impl( Tp&& _t ) : _Base( std::forward< Tp >( _t ) ) {}
};
// 实现递推
template < size_t N, typename Tp, typename... Args >
struct _Tuple_impl< N, Tp, Args... > : _Tuple_base< N, Tp >, _Tuple_impl< N + 1, Args... >
{
    using _Base      = _Tuple_base< N, Tp >;           // 声明 _Base 类型
    using _Inherited = _Tuple_impl< N + 1, Args... >;  // 声明 _Inherited 类型
    _Tuple_impl( Tp&& _t, Args&&... args )
        : _Base( std::forward< Tp >( _t ) ), _Inherited( std::forward< Args >( args )... )
    {
    }
    static auto get( const _Tuple_impl& _tl ) -> decltype( _Base::value )
    {
        return _Base::get( _tl );
    }
};
// 接口类定义、接口类构造函数定义
template < typename... Args > struct tuple : _Tuple_impl< 0, Args... >
{
    tuple( Args&&... args ) : _Tuple_impl< 0, Args... >( std::forward< Args >( args )... ) {}
};
// 接口函数定义
template < size_t N, typename... Args >  // 这里进行自动类型转换 //
auto get( const _Tuple_impl< N, Args... >& _t ) -> decltype( _Tuple_impl< N, Args... >::_Base::value )
{
    using _Base = typename _Tuple_impl< N, Args... >::_Base;
    return _Base::get( _t );
}
}  // namespace ns_tuple