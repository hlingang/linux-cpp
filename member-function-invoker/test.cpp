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

    for ( int i = 0; i < 30; i += 12 )
    {
        obj.invoker( i + 0 );
        pObj->invoker( i + 1 );
        ( obj.*member_invoker )( i + 2 );
        ( pObj->*member_invoker )( i + 3 );

        ( obj.*( obj._M_ops.__call ) )( i + 4 );
        ( pObj->*( obj._M_ops.__call ) )( i + 5 );

        ( obj.*( pObj->_M_ops.__call ) )( i + 6 );
        ( pObj->*( pObj->_M_ops.__call ) )( i + 7 );

        ( obj.*( obj._M_p_ops_1->__call ) )( i + 8 );
        ( pObj->*( obj._M_p_ops_1->__call ) )( i + 9 );

        ( obj.*( obj._M_p_ops_2->__call ) )( i + 10 );
        ( pObj->*( obj._M_p_ops_2->__call ) )( i + 11 );
    }
}
