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
/*
类的申明和实现的分离
- 更好的代码组织结构
- 提高代码的可读性和可维护性
- 减少编译时间
- 支持信息隐藏和封装
- 【有效的解决相互引用的依赖问题】

类的声明:
- 前置声明（只申明类名，不包含成员）
- 完整声明（包含成员变量和成员函数的声明）
- 完整声明和实现(包含成员变量声明和成员函数的定义)
*/

using namespace std;
enum MEMBER_OPS
{
    OPS_BASE,
    OPS_COND1,
    OPS_COND2,
    OPS_MAX,
};
class MemberCls;
template < int N > struct MemberOps_t
{
};
template <> struct MemberOps_t< 0 >
{
    // 通用成员定义 //
    using type = int ( MemberCls::* )( int );
    type __call;
    // 默认初始化 //
    MemberOps_t() : __call( nullptr )
    {
        printf( "MemberOps_t<0> constructed\n" );
    }
    // 子类和超类间的自定义初始化接口 //
    void init( type __call )
    {
        this->__call = __call;
    }
};
template <> struct MemberOps_t< OPS_COND1 > : public MemberOps_t< OPS_BASE >
{
    using super = MemberOps_t< OPS_BASE >;
    MemberOps_t();
};
template <> struct MemberOps_t< OPS_COND2 > : public MemberOps_t< OPS_BASE >
{
    using super = MemberOps_t< OPS_BASE >;
    MemberOps_t();
};
class MemberCls
{
public:
    int memberFunction( int a )
    {
        printf( "Member Function Invoke test[%d]\n", a );
        return 0;
    }
    int memberFunction_1( int a )
    {
        printf( "Member Function Invoke test-1[%d]\n", a );
        return 0;
    }
    int memberFunction_2( int a )
    {
        printf( "Member Function Invoke test-2[%d]\n", a );
        return 0;
    }
    MemberOps_t< OPS_BASE >  _M_ops;
    MemberOps_t< OPS_BASE >* _M_p_ops_1;
    MemberOps_t< OPS_BASE >* _M_p_ops_2;
    MemberCls()
    {
        _M_ops.__call = &MemberCls::memberFunction;
        _M_p_ops_1    = new MemberOps_t< OPS_COND1 >();
        _M_p_ops_2    = new MemberOps_t< OPS_COND2 >();
    }
    ~MemberCls()
    {
        delete _M_p_ops_1;
        delete _M_p_ops_2;
    }
    void invoker( int i )
    {
        ( this->*( _M_ops.__call ) )( i );
    }
};

MemberOps_t< OPS_COND1 >::MemberOps_t()
{
    super::init( &MemberCls::memberFunction_1 );
    printf( "MemberOps_t<OPS_COND1> constructed\n" );
}
MemberOps_t< OPS_COND2 >::MemberOps_t()
{
    super::init( &MemberCls::memberFunction_2 );
    printf( "MemberOps_t<OPS_COND2> constructed\n" );
}