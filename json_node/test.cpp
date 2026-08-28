#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <node_list.h>

using namespace std;

int main()
{
    node js;
    js[ 2 ] = "hello-word";
    cout << js.to_string() << endl;
    node ks;
    ks[ "i" ] = 99;
    ks[ "c" ] = "str";
    cout << ks.to_string() << endl;
    ks[ "o" ] = js;
    cout << ks.to_string() << endl;
    ks.remove( "o" );
    cout << ks.to_string() << endl;
    ks.remove( "c" );
    cout << ks.to_string() << endl;
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
}