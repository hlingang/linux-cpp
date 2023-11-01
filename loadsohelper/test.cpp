#include "liba.h"
#include "load_so_helper.h"
#include <dlfcn.h>
#include <iostream>

using namespace std;

int main()
{
    S_LIBASO liba_so;
    if ( LoadSOHelper( liba_so ) == nullptr )
    {
        cout << "[ERROR]: load so fail" << endl;
        return -1;
    }
    liba_so.test01.__call( 10 );
    liba_so.test02.__call( "welcome!" );
    CloseSOHelper( liba_so );
    return 0;
}