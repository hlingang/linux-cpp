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

int main()
{

    auto f         = []( int a, const std::string& str ) -> void { printf( "Invoke test[%d]: %s\n", a, str.c_str() ); };
    char buf[ 32 ] = { 0 };
    for ( int i = 0; i < 10; i++ )
    {
        snprintf( buf, sizeof( buf ), "I am Invoker-%d", i );
        InvokerInterface( f, i + 10, buf );
    }
    return 0;

    shared_ptr< int > ss;
}