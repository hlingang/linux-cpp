#ifndef _INVOKER_H
#define _INVOKER_H

#include <iostream>

extern unsigned long val;

void exit_0()
{
    val++;
    printf( "exit_0\n" );
}
void exit_1()
{
    val++;
    printf( "exit_1\n" );
}
void __attribute__( ( destructor ) ) __destructor()
{
    printf( "destructor:val=%lu\n", val );
}
#endif