
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <numeric>
using namespace std;

void thread_func()
{
    printf( "test.cpp::thread_func(), pid: %d\n", getpid() );
    this_thread::sleep_for( std::chrono::seconds( 5 ) );
    printf( "thread end!\n" );
}

int main()
{
    printf( "test.cpp::main(), pid: %d\n", getpid() );
    thread __t( thread_func );
    __t.join();
#ifdef WIN32
    printf( "**WINDOWS ENBALE(32)**" );
#elif defined( WIN64 )
    printf( "**WINDOWS ENBALE(64)**" );
#endif
#if defined( WIN32 ) || defined( WIN64 )
    char c;
    std::cin >> c;
#endif
}