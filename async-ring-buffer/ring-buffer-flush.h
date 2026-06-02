#ifndef _RING_BUFFER_FLUSH_H
#define _RING_BUFFER_FLUSH_H
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <flush_base.h>
#include <iostream>

enum FlushStatus
{
    _B_SCHE = 0,
    _B_SYNC,
};

#define B_SYNC ( 1U << _B_SYNC )
#define B_SCHE ( 1U << _B_SCHE )

struct RingBufferManager
{
    std::vector< FlushBase* > ring_buffer_list;
    std::mutex                m_mtx;
    void                      overflow()
    {
        std::lock_guard< std::mutex > lock( m_mtx );
        for ( auto* buffer : ring_buffer_list )
        {
            buffer->overflow( 1 );
        }
    }
    void __register_ring_buff( FlushBase* ring_buff )
    {
        ring_buffer_list.push_back( ring_buff );
    }
};

class RingBufferFlush
{
    int                       m_running;
    int                       m_exit;
    std::vector< FlushBase* > m_swap_buffer_list;
    std::mutex                m_mtx;
    std::condition_variable   m_cv;
    std::thread               m_thread;

public:
    void init();
    void __do_flush();
    void __request_flush( FlushBase* ptr );
    void __stop();
};
RingBufferFlush*                get_ring_buffer_flush();
void                            request_flush( FlushBase* );
RingBufferManager*              get_ring_buffer_manager();
void                            register_ring_buffer( FlushBase* ring_buff );
void                            overflow_ring_buff();
void                            do_exit();
template < typename Lock > void wait_status( int* status, int bitmap, int target, Lock* __lock )
{
    do
    {
        __lock->unlock();
        std::this_thread::yield();
        __lock->lock();

    } while ( ( ( *( int* )status ) & bitmap ) != target );
}
#endif