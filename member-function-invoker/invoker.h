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
    using type = int ( MemberCls::* )( int );
    type __call;
    MemberOps_t() : __call( nullptr )
    {
        printf( "MemberOps_t<0> constructed\n" );
    }
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
    MemberOps_t< 0 >  _M_ops;
    MemberOps_t< 1 >* _M_p_ops_1;
    MemberOps_t< 2 >* _M_p_ops_2;
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
