#ifndef __LIBA_H_
#define __LIBA_H_
#include "load_so_helper.h"

typedef int ( *T_test01 )( int );
typedef void ( *T_test02 )( const char* );
struct S_LIBASO
{
    FuncSymbol< T_test01 > test01{ NULL, "test01" };  ///< function address
    FuncSymbol< T_test02 > test02{ NULL, "test02" };  ///< function address
    void*                  handler{ NULL };           ///< so handler
    void*                  so = ( void* )"liba.so";   ///< so name
};
#endif
