#include <stdio.h>
#include <unistd.h>
void get_arch( char* ptr, int size );
void* malloc(size_t size);
void* memset(void*p, int, size_t);
void  free(void*p);
char* void_to_char_p(void* p);
void* char_to_void_p(char* p);