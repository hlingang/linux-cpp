
#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <flush_base.h>

#define BUF_SIZE ( 1024 )

using namespace std;

struct payload_t
{
    uint64_t start_time;
    uint64_t end_time;
};

struct Header
{
    uint64_t id;
    uint64_t size;
    char*    payload;
};

class RingBufferBase
{
private:
    char*      m_data;
    uint64_t   write_pos;
    uint64_t   read_pos;
    uint64_t   m_index;
    uint64_t   capacity;
    std::mutex m_mtx;

public:
    RingBufferBase();
    char*           request( uint64_t __size );
    RingBufferBase& swap( RingBufferBase& other );
    int             __reset();
    int             reset();
    uint64_t        _M_write_pos()
    {
        return write_pos;
    }
    uint64_t _M_read_pos()
    {
        return read_pos;
    }
    std::mutex& _M_mutex()
    {
        return m_mtx;
    }
    char* _M_data()
    {
        return m_data;
    }
    template < typename Tp > char* emplace_back( Tp& arg )
    {
        char* ptr = request( sizeof( Tp ) );
        if ( ptr == nullptr )
            return nullptr;
        using Up = typename std::remove_reference< Tp >::type;
        new ( ptr ) Up( std::forward< Tp >( arg ) );
        return ptr;
    }
};

class RingBufferHeader
{
    using header_t = Header;
    using buffer_t = RingBufferBase;

private:
    vector< header_t > m_header;
    buffer_t           m_buffer;
    uint64_t           m_index;
    std::mutex         m_mtx;

public:
    RingBufferHeader();
    char*                          request( uint64_t __size );
    RingBufferHeader&              swap( RingBufferHeader& other );
    uint64_t                       get_index();
    int                            flush();
    template < typename Tp > char* emplace_back( Tp&& arg )
    {
        auto* payload = m_buffer.emplace_back( std::forward< Tp >( arg ) );
        if ( nullptr == payload )
            return nullptr;
        auto     id = get_index();
        header_t header{ id, sizeof( Tp ), nullptr };
        header.payload    = payload;
        m_header.at( id ) = header;
        return payload;
    }
};

class RingBuffer : public FlushBase
{
public:
    using buffer_t = RingBufferHeader;
    uint64_t           buffer_id;
    buffer_t           m_buffer;
    buffer_t           m_swap_buffer;
    condition_variable m_cv;
    mutex              m_mtx;
    RingBuffer();
    void                           swap();
    int                            flush() override;
    int                            overflow( int wait = 0 ) override;
    buffer_t&                      get_swap();
    buffer_t&                      get_buff();
    void                           register_self();
    void                           wait_complete();
    template < typename Tp > char* emplace_back( Tp&& value )
    {
        char* ret;
        if ( ( ret = m_buffer.emplace_back( std::forward< Tp >( value ) ) ) == nullptr )
        {
            overflow();
            ret = m_buffer.emplace_back( std::forward< Tp >( value ) );  //******* Tp 类型的完美转发 ********//
        }
        printf( "thread[%lu] emplace back ret=[%p]\n", ( pthread_t )pthread_self(), ( void* )ret );
        return ret;
    }
};

#endif