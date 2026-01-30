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

class MemberCls
{
public:
    struct MemberOps_t
    {
        int ( MemberCls::*__call )( int a );
        MemberOps_t() : __call( nullptr ) {}
    };
    int memberFunction( int a )
    {
        printf( "Member Function Invoke test[%d]\n", a );
        return 0;
    }
    MemberOps_t _M_ops;
    MemberCls()
    {
        _M_ops.__call = &MemberCls::memberFunction;
    }
    void invoker( int i )
    {
        ( this->*( _M_ops.__call ) )( i );
    }
};