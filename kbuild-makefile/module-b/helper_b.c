#include <stdio.h>
#include <get_arch.h>
void helper_b()
{
    char arch[ 32 ] = { 0 };
    get_arch( arch, sizeof( arch ) );
    printf( "lib_b call from helper-b[%s]\n", arch );
}