#include "cjson.hpp"
#include <iostream>
using namespace std;

int main()
{
    CJson array_json;
    CJson object_json;
    char  name[ 20 ] = "abcdefgh";
    object_json.add( "a", 60 );
    object_json.add( name, string( name ) );
    object_json.toFile( "test001.json" );
    CJson tmp_json;
    tmp_json.add( "123", 123 );
    object_json.add( "obj", tmp_json );
    array_json.append( object_json );
    array_json.append( "qwert" );
    array_json.append( 123 );
    cout << array_json.toString() << endl;
    array_json.toFile( "test002.json" );
}
