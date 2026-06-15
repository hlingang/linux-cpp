#ifndef _INVOKER_H
#define _INVOKER_H

#include <iostream>

void __attribute__( ( section( "security.text" ) ) ) security_func()
{
    printf( "security:1234\n" );
};

struct Security_data
{
    int magic_code;
};

Security_data security_data1 __attribute__( ( section( "security.data" ) ) );
Security_data security_data2 __attribute__( ( section( "security.data" ) ) );
Security_data security_data3 __attribute__( ( section( "security.data" ) ) );

#endif