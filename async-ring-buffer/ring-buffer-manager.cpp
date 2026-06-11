#include <ring-buffer-manager.h>
#include <ring-buffer.h>
#include <ring-buffer-flush.h>
#include <random>

void RingBufferManager::overflow()
{
    std::unique_lock< std::mutex > lock( m_mtx );
    if ( ring_buffer_list.empty() )
        return;
    for ( auto* __ring_buff : ring_buffer_list )
    {
        if ( __ring_buff->m_type == _B_FILE_BUFF )
        {
            ring_buffer_list.remove( __ring_buff );
            ring_buffer_list.emplace_back( __ring_buff );
        }
    }
    std::list< FlushBase* > __temp;
    do
    {
        __temp.swap( ring_buffer_list );
        lock.unlock();
        while ( !__temp.empty() )
        {
            auto* __ring_buffer = __temp.front();
            __temp.pop_front();
            printf( "BUFF[%d][TYPE:%d] overflow\n", __ring_buffer->get_buff_id(), __ring_buffer->get_buff_type() );
            __ring_buffer->overflow( 1 );
        }
        lock.lock();
    } while ( !ring_buffer_list.empty() );
}
int RingBufferManager::__get_offset()
{
    mt19937                  eng{};
    uniform_int_distribution __rand( 0, numeric_limits< int >::max() );
    return __rand( eng );
}
void RingBufferManager::__register_ring_buff( FlushBase* ring_buff )
{
    std::lock_guard< std::mutex > lock( m_mtx );
    auto                          id       = ring_buffer_list.size();
    auto                          __offset = __get_offset();
    ring_buff->set_buff_id( id + __offset );
    ring_buffer_list.push_back( ring_buff );
}

RingBufferManager* get_ring_buffer_manager()
{
    static RingBufferManager __ring_buff_manger{};
    return &__ring_buff_manger;
}

void register_ring_buffer( FlushBase* ring_buff )
{
    get_ring_buffer_manager()->__register_ring_buff( ring_buff );
    printf( "Register buffer:[%d]\n", ring_buff->get_buff_id() );
}
void do_exit()
{
    return get_ring_buffer_flush()->__stop();
}
void overflow_ring_buff()
{
    return get_ring_buffer_manager()->overflow();
}