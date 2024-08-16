#include "ringbuf.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    Ringbuf __m_buf( 33 );
    auto    __f_write = [ & ]() {
        for ( int i = 0; i < 100; i++ )
        {
            while ( __m_buf.write( &i, sizeof( int ) ) != sizeof( int ) )
            {
                this_thread::sleep_for( chrono::milliseconds( 10 ) );
            }
        }
    };
    auto __f_read = [ & ]() {
        char __buf[ 4 ];
        for ( int i = 0; i < 100; i++ )
        {
            while ( __m_buf.read( &__buf, sizeof( int ) ) != sizeof( int ) )
            {
                this_thread::sleep_for( chrono::milliseconds( 10 ) );
            }
        }
    };
    thread t_write = thread( __f_write );
    thread t_read  = thread( __f_read );
    t_write.join();
    t_read.join();

    return 0;
}