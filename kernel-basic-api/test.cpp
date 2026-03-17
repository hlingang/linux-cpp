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

int __undefined_fn() __attribute__( ( weak ) );  // 声明一个弱符号函数，允许链接器在链接阶段忽略未定义的符号

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
    char*       ret4  = ns_kapi::strstr( "hello, world, welcome!", "world" );
    cout << "memcmp result: " << ret1 << endl;
    cout << "strstrip result: " << ret2 << endl;
    cout << "strpbrk result: " << ret3 << endl;
    cout << "strstr result: " << ret4 << endl;
    if ( __undefined_fn )
        __undefined_fn();
    int is __attribute__( ( unused ) ) = 99;
    return 0;
}
