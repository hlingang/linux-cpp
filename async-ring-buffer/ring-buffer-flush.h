#ifndef _RING_BUFFER_FLUSH_H
#define _RING_BUFFER_FLUSH_H
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <flush_base.h>
#include <iostream>
#include <list>

enum FlushStatus
{
    _B_SCHE = 0,
    _B_SYNC,
};

enum RingBufferType
{
    _B_RING_BUFF,
    _B_FILE_BUFF,
    _B_BUFF_TYPE_NUM,
};

#define B_SYNC ( 1U << _B_SYNC )
#define B_SCHE ( 1U << _B_SCHE )

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
void                            exit_flush();
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