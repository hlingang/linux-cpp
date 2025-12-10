#include <string.h>
#include <iostream>
#include "tuple-impl.h"

using namespace std;

int main()
{
    ns_tuple::tuple< int, float, string, double, char > t( 99, 3.14, "abc", 100.01, 'A' );
    cout << ns_tuple::get< 0 >( t ) << endl;
    cout << ns_tuple::get< 1 >( t ) << endl;
    cout << ns_tuple::get< 2 >( t ) << endl;
    cout << ns_tuple::get< 3 >( t ) << endl;
    cout << ns_tuple::get< 4 >( t ) << endl;
}