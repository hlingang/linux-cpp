#ifndef __LOAD_SO_HELPER_
#define __LOAD_SO_HELPER_
#include <dlfcn.h>
#include <iostream>

template < typename T > struct FuncSymbol
{
    T           __call;
    const char* __sym;
    FuncSymbol() : __call( nullptr ), __sym( nullptr ) {}
    FuncSymbol( void* call, const char* sym ) : __call( ( T )call ), __sym( sym ) {}
} __attribute__( ( packed ) );
template < typename T > void* LoadSOHelper( T& sso, const char* _so = nullptr )
{
    int         N      = sizeof( T ) / sizeof( FuncSymbol< void* > );
    void**      pcAddr = ( void** )&sso;
    const char* so     = _so ? _so : ( const char* )sso.so;
    if ( so == nullptr )
    {
        printf( "so name to load is null\n" );
        return nullptr;
    }
    void* handler = dlopen( so, RTLD_LAZY );
    if ( handler == nullptr )
    {
        printf( "[ERROR]: %s\n", dlerror() );
        return nullptr;
    }
    for ( int i = 0; i < N - 1; i++ )
    {
        void** func   = pcAddr;                    // function address
        char** symbol = ( char** )( pcAddr + 1 );  // funcsymbol address
        if ( *symbol == nullptr )
        {
            continue;
        }
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
    sso.handler = handler;
    return handler;
}
template < typename T > void CloseSOHelper( T& sso )
{
    if ( sso.handler )
    {
        dlclose( sso.handler );
    }
}
#endif
