#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include "invoker.h"

using namespace std;

int ( MemberCls::*member_invoker )( int );

int main()
{

    MemberCls  obj;
    MemberCls* pObj = &obj;
    member_invoker  = &MemberCls::memberFunction;
    for ( int i = 0; i < 30; i += 6 )
    {
        ( obj.*member_invoker )( i );
        ( pObj->*member_invoker )( i + 1 );

        ( obj.*( obj._M_ops.__call ) )( i + 2 );
        ( pObj->*( obj._M_ops.__call ) )( i + 3 );

        ( obj.*( pObj->_M_ops.__call ) )( i + 4 );
        ( pObj->*( pObj->_M_ops.__call ) )( i + 5 );
    }
}
