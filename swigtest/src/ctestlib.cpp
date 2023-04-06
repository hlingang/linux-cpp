#include "ctestlib.h"
#include <cstdio>

int gValue = 0xFF;

void A::A_test( int n )
{
    printf( "A_test value = %d\n", n );
}

void swig_test( int n )
{
    for ( int i = 0; i < n; i++ )
    {
        printf( "swig test - %d\n", i );
    }
}

void swig_test( float fn )
{
    printf( "swig test - 000: %f\n", fn );
}
