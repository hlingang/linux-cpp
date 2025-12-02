#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include <async.h>

using namespace std;

void func( void* args, void* ret )
{
    int a             = *( ( int* )args );
    *( ( int* )ret )  = ++a;
    *( ( int* )args ) = a;
    return;
};

int main()
{
    int           input[ 8 ]{ 9, 2, 3, 4, 5, 6, 7, 8 };
    int           output[ 8 ]{ 0, 0, 0, 0, 0, 0, 0, 0 };
    int           sz = 8;
    ParallelWork* p  = GetParallelWork( sz );
    for ( int i = 0; i < sz; ++i )
    {
        p->Operation( i, ( void* )func, ( void* )&input[ i ], ( void* )&output[ i ] );
    }
    for ( int id = 0; id < 1000; id++ )
    {
        p->Start();
        p->Wait();
        for ( int i = 0; i < sz; i++ )
        {
            printf( "[%d] Output[%d] = %d\n", id, i, output[ i ] );
        }
        printf( "----------------------------------------------------\n" );
    }
    p->Stop();
    return 0;
}