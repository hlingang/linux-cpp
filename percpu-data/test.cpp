#include <iostream>
#include <string>
#include <string.h>

using namespace std;

#define NR_CPUS 4

extern char                                                   __start_percpu[];
extern char                                                   __stop_percpu[];
static unsigned long                                          __cpu_shift[ NR_CPUS ];
char __attribute__( ( section( "percpu" ) ) )                 cpu_char   = 'A';
short __attribute__( ( section( "percpu" ) ) )                cpu_short  = 99;
int __attribute__( ( section( "percpu" ) ) )                  cpu_int    = 9999;
double __attribute__( ( section( "percpu" ), aligned( 4 ) ) ) cpu_double = 3.14;

#define per_cpu_var( var ) cpu_##var
#define per_cpu( var, cpuid ) \
    ( *( __typeof__( per_cpu_var( var ) )* )( ( char* )&per_cpu_var( var ) + __cpu_shift[ cpuid ] ) )

int main()
{
    printf( "start-percpu-addr = %p\n", __start_percpu );
    printf( "end-percpu-addr   = %p\n", __stop_percpu );
    printf( "percpu-size       = %ld\n", __stop_percpu - __start_percpu );
    char* __ptr = ( char* )malloc( NR_CPUS * ( __stop_percpu - __start_percpu ) );
    for ( int cpuid = 0; cpuid < NR_CPUS; cpuid++ )
    {
        memcpy( __ptr, __start_percpu, __stop_percpu - __start_percpu );
        __cpu_shift[ cpuid ] = __ptr - __start_percpu;
        __ptr += __stop_percpu - __start_percpu;
        printf( "cpu_shift[%d] = %ld\n", cpuid, __cpu_shift[ cpuid ] );
    }
    for ( int cpuid = 0; cpuid < NR_CPUS; cpuid++ )
    {
        cout << "cpu_char  : " << per_cpu( char, cpuid ) << endl;
        cout << "cpu_short : " << per_cpu( short, cpuid ) << endl;
        cout << "cpu_int   : " << per_cpu( int, cpuid ) << endl;
        cout << "cpu_double: " << per_cpu( double, cpuid ) << endl;
    }
}