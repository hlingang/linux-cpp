
#include <iostream>
#include <invoker.h>
#include <random>
#include <numeric>

using namespace std;

extern char __start_security[];
extern char __stop_security[];
extern char __start_security_text[];
extern char __stop_security_text[];

extern char __security_text_start[];
extern char __security_text_stop[];

extern char __start_security_data[];
extern char __stop_security_data[];

void __attribute__( ( constructor ) ) __init()
{

    mt19937                  mt{};
    uniform_int_distribution __rand( 0, numeric_limits< int >::max() );
    int                      offset = __rand( mt );
    for ( Security_data* data = ( Security_data* )__start_security_data; ( char* )data < __stop_security_data;
          offset++, data++ )
        data->magic_code = offset;
}

int main()
{

    void ( *func1 )() = ( void ( * )() )__start_security_text;
    void ( *func2 )() = ( void ( * )() )__security_text_start;
    func1();
    func2();
    for ( Security_data* data = ( Security_data* )__start_security_data; ( char* )data < __stop_security_data; data++ )
        printf( "magic_code=%d\n", data->magic_code );
    return 0;
}
