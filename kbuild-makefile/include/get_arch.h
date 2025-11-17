#include <stdio.h>
#include "include/asm/get_arch.h"
void                           get_arch( char* ptr, int size );
__attribute__( ( weak ) ) void arch_get_arch( char* ptr, int size );
