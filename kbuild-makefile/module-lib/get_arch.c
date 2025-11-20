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
char* void_to_char_p(void *p){return (char*)p;}
void* char_to_void_p(char* p){return (void*)p;}
void test()
{
    printf("swig-test-end\n");
}
