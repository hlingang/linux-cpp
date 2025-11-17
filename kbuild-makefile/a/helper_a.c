#include <stdio.h>
#include <get_arch.h>
void helper_a()
{
    char arch[ 32 ] = { 0 };
    get_arch( arch, sizeof( arch ) );
    printf( "lib_a call from helper-a[%s]\n", arch );
}