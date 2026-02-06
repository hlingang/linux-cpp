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

    auto x = min( 10, 20 );
    auto y = max( 50, 20 );
    type_check( int, 100 );
    build_bug_on( sizeof( int ) != 4 );
    printf( "min: %d, max: %d\n", x, y );
    return 0;
}
