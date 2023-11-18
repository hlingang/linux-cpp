#include <iostream>
#include "json.h"

using namespace std;

int main()
{
    std::cout << "start..." << endl;
    json::Json j;
    j[ "a" ] = 1;
    j[ "b" ] = "hello";
    json::Json q( "abc" );
    j[ "q" ] = q;
    json::Json p( 1 );
    j[ "p" ] = p;
    json::Json l;
    l.append( 1 );
    l.append( "abc" );
    j[ "L" ] = l;
    json::Json o;
    o[ "a1" ]  = 100;
    o[ "a2" ]  = "123";
    o[ "a3" ]  = l;
    j[ "obj" ] = o;
    cout << j.toString() << endl;
    for ( auto& key : j )
    {
        std::cout << "key=" << key.first.toString() << " value=" << key.second.toString() << endl;
    }
}
