
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
    printf( "test.cpp::main(), pid: %d\n", getpid() );
#ifdef WIN
    printf( "**WINDOWS ENBALE**" );
    char c;
    std::cin >> c;
#endif
}