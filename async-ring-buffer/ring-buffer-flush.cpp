#include <ring-buffer-flush.h>
#include <ring-buffer.h>
#include <flush_base.h>

using namespace std;

void RingBufferFlush::__do_flush()
{
    for ( ;; )
    {
        unique_lock< std::mutex > lock( m_mtx );
        if ( m_swap_buffer_list.empty() )
            m_cv.wait( lock, [ this ] { return !this->m_swap_buffer_list.empty() || !m_running; } );
        if ( !m_running )  // 标记运行状态改变
        {
            if ( m_swap_buffer_list.empty() )
                break;
        }
        vector< FlushBase* > __tmp_swap_buffer_list{};
        __tmp_swap_buffer_list.swap( m_swap_buffer_list );
        while ( !__tmp_swap_buffer_list.empty() )
        {
            auto ptr = __tmp_swap_buffer_list.back();
            __tmp_swap_buffer_list.pop_back();
            RingBuffer* ring_buffer = static_cast< RingBuffer* >( ptr );
            if ( !( ring_buffer->status & B_SCHE ) )
                continue;
            ring_buffer->status &= ~B_SCHE;  // any thread wait B_SCHE flag?
            ring_buffer->status |= B_SYNC;
            lock.unlock();
            ring_buffer->flush();
            lock.lock();
            ring_buffer->status &= ~B_SYNC;
            ring_buffer->m_cv.notify_one();
        }
        if ( !m_running )
            break;
    }
    m_exit = 1;  // 标记退出完成
    return;
}

void RingBufferFlush::__request_flush( FlushBase* ptr )
{
    unique_lock< std::mutex > lock( m_mtx );
    m_swap_buffer_list.push_back( ptr );
    m_cv.notify_one();
}

void RingBufferFlush::__stop()
{
    unique_lock< std::mutex > lock( m_mtx );
    m_running = 0;
    m_cv.notify_all();
    if ( !m_exit )
        wait_status( &m_exit, 1, 1, &lock );
}

void RingBufferFlush::init()
{
    m_running = 1;
    m_exit    = 0;
    m_thread  = thread( [ this ] { this->__do_flush(); } );
    m_thread.detach();
}

RingBufferFlush* get_ring_buffer_flush()
{
    static RingBufferFlush __ring_buffer_flush{};
    static std::once_flag  init_flag;
    std::call_once( init_flag, [] { __ring_buffer_flush.init(); } );
    return &__ring_buffer_flush;
}

void request_flush( FlushBase* ptr )
{
    get_ring_buffer_flush()->__request_flush( ptr );
}

void exit_flush()
{
    get_ring_buffer_flush()->__stop();
}