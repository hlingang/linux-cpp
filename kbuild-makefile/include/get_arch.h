#include <stdio.h>
#include "include/asm/get_arch.h"
void                           test();
void                           get_arch(  char* ptr, int size );
__attribute__( ( weak ) ) void arch_get_arch(  char* ptr, int size );
char* void_to_char_p(void *p);
void* char_to_void_p(char* p);
