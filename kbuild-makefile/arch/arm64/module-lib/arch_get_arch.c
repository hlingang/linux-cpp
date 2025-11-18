#include <asm/get_arch.h>
void arch_get_arch( char* ptr, int size )
{
    snprintf( ptr, size, "arm64" );
}