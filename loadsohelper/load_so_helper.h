#ifndef __LOAD_SO_HELPER_
#define __LOAD_SO_HELPER_
#include <dlfcn.h>
#include <iostream>

template < typename T > struct FuncSymbol
{
    T           call;
    const char* sym;
    FuncSymbol() : call( nullptr ), sym( nullptr ) {}
    FuncSymbol( void* call_, const char* sym_ ) : call( ( T )call_ ), sym( sym_ ) {}
} __attribute__( ( packed ) );
template < typename T > void* LoadSOHelper( T& sso )
{
    int    N       = sizeof( T ) / sizeof( FuncSymbol< void* > );
    void** pcAddr  = ( void** )&sso;
    char*  so      = ( char* )sso._so;
    void*  handler = dlopen( so, RTLD_LAZY );
    if ( handler == nullptr )
    {
        return nullptr;
    }
    for ( int i = 0; i < N - 1; i++ )
    {
        void** func   = pcAddr;                    // function address
        char** symbol = ( char** )( pcAddr + 1 );  // funcsymbol address
        if ( ( *func = dlsym( handler, *symbol ) ) == nullptr )
        {
            if ( handler )
            {
                dlclose( handler );
            }
            return nullptr;
        }
        pcAddr += 2;
    }
    *( void** )pcAddr = handler;
    return handler;
}
template < typename T > void CloseSOHelper( T& sso )
{
    if ( sso._handler )
    {
        dlclose( sso._handler );
    }
}
#endif