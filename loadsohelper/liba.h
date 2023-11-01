#ifndef __LIBA_H_
#define __LIBA_H_
#include "load_so_helper.h"

typedef int ( *test01_t )( int );
typedef void ( *test02_t )( const char* );
struct S_LIBASO
{
    FuncSymbol< test01_t > test01{ NULL, "test01" };  ///< function address
    FuncSymbol< test02_t > test02{ NULL, "test02" };  ///< function address
    void*                  handler{ NULL };           ///< so handler
    void*                  so = ( void* )"liba.so";   ///< so name
};
#endif
