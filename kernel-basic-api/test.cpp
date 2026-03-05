#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include "kernel-api.h"

using namespace std;
using namespace ns_kapi;

int main()
{
    const char*   s1     = "abc";
    const char*   s2     = "abcdef";
    int           result = ns_kapi::strcmp( s1, s2 );
    unsigned long len1   = ns_kapi::strlen( s1 );
    unsigned long len2   = ns_kapi::strlen( s2 );
    cout << "strcmp result: " << result << endl;
    cout << "strlen result: " << len1 << ", " << len2 << endl;

    int         ret1  = ns_kapi::memcmp( ( void* )s2, ( void* )s1, strlen( s1 ) );
    char        buf[] = " asuroesik  ";  // save on stack
    const char* ret2  = ns_kapi::strstrip( buf );
    char*       ret3  = ns_kapi::strpbrk( "suyfireuoiu", "abrcde" );
    cout << "memcmp result: " << ret1 << endl;
    cout << "strstrip result: " << ret2 << endl;
    cout << "strpbrk result: " << ret3 << endl;
    return 0;
}
