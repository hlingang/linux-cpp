#include <stdio.h>
#include "include/get_arch.h"

void get_arch( char* ptr, int size )
{
    arch_get_arch( ptr, size );
}

__attribute__( ( weak ) ) void arch_get_arch( char* ptr, int size )
{
    snprintf( ptr, size, "unknown-arch" );
}
