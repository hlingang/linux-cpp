
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <async-ring-buffer.h>

using namespace std;

void wait_done()
{
    overflow_ring_buff();
    exit_flush();
}

void func1( RingBuffer* __buf )
{
    for ( int i = 0; i < 200; i += 2 )
    {
        payload_t pyload;
        pyload.start_time = i;
        pyload.end_time   = pyload.start_time + 2;
        payload_t& key    = pyload;
        __buf->emplace_back( key );
        this_thread::sleep_for( chrono::milliseconds( 1 ) );
    }
}

void func2( RingBuffer* __buf )
{
    for ( int i = 1; i < 200; i += 2 )
    {
        payload_t pyload;
        pyload.start_time = i;
        pyload.end_time   = pyload.start_time + 2;
        payload_t& key    = pyload;
        __buf->emplace_back( key );
        this_thread::sleep_for( chrono::milliseconds( 1 ) );
    }
}

void func3( RingBuffer* __buf )
{
    for ( int i = 200; i < 300; i++ )
    {
        payload_t pyload;
        pyload.start_time = i;
        pyload.end_time   = pyload.start_time + 2;
        payload_t& key    = pyload;
        __buf->emplace_back( key );
        this_thread::sleep_for( chrono::milliseconds( 1 ) );
    }
}

int main()
{
    RingBuffer buf1;
    register_ring_buffer( &buf1 );
    RingBuffer buf2;
    register_ring_buffer( &buf2 );
    thread t1( func1, &buf1 );
    thread t2( func2, &buf1 );
    thread t3( func3, &buf2 );
    t1.join();
    t2.join();
    t3.join();
    wait_done();
    return 0;
}
