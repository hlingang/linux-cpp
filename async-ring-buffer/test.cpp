
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <ring-buffer.h>
#include <ring-buffer-flush.h>

using namespace std;

void wait_done()
{
    overflow_ring_buff();
    do_exit();
}

int main()
{
    RingBuffer buf;
    register_ring_buffer( &buf );
    for ( int i = 0; i < 1000; i++ )
    {
        payload_t pyload;
        pyload.start_time = i;
        pyload.end_time   = pyload.start_time + 2;
        payload_t& key    = pyload;
        buf.emplace_back( key );
        this_thread::sleep_for( chrono::milliseconds( 1 ) );
    }
    wait_done();
    return 0;
}
