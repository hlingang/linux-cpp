#include <ring-buffer.h>
#include <ring-buffer-flush.h>
#include <file-buffer.h>
#include <ring-buffer-manager.h>

using namespace std;

RingBufferBase::RingBufferBase() : write_pos( 0 ), read_pos( 0 ), m_index( 0 ), capacity( BUF_SIZE )
{
    m_data = new char[ BUF_SIZE ];
}
char* RingBufferBase::request( uint64_t __size )
{
    std::lock_guard< std::mutex > lock( m_mtx );
    if ( write_pos + __size > capacity )
        return nullptr;
    char* ptr = m_data + write_pos;  // get the pointer to the current write position
    write_pos += __size;
    return ptr;
}
int RingBufferBase::__reset()
{
    write_pos = 0;
    read_pos  = 0;
    m_index   = 0;
    return 0;
}
int RingBufferBase::reset()
{
    std::lock_guard< std::mutex > lock( m_mtx );
    return __reset();
}
RingBufferBase& RingBufferBase::swap( RingBufferBase& other )
{
    std::swap( m_data, other.m_data );
    std::swap( write_pos, other.write_pos );
    std::swap( read_pos, other.read_pos );
    std::swap( m_index, other.m_index );
    std::swap( capacity, other.capacity );
    return *this;
}
RingBufferHeader::RingBufferHeader() : m_index( 0 ), m_buffer()
{
    m_header.resize( BUF_SIZE );
}
uint64_t RingBufferHeader::get_index()
{
    std::lock_guard< std::mutex > lock( m_mtx );
    return m_index++;
}
char* RingBufferHeader::request( uint64_t __size )
{
    return m_buffer.request( __size );
}
RingBufferHeader& RingBufferHeader::swap( RingBufferHeader& other )
{
    m_header.swap( other.m_header );
    m_buffer.swap( other.m_buffer );
    std::swap( m_index, other.m_index );
    return *this;
}
int RingBufferHeader::flush()
{
    unique_lock< std::mutex > lock( m_mtx );
    for ( uint64_t i = 0; i < m_index; ++i )
    {
        auto& header = m_header.at( i );
        if ( header.payload != nullptr )
        {
            payload_t* payload = reinterpret_cast< payload_t* >( header.payload );
            write_to_file_buffer( *payload, i );
            printf( "[this:%p]write to file buff: payload->start_time=%ld\n", ( void* )this, payload->start_time );
        }
    }
    m_buffer.__reset();
    m_index = 0;
    return 0;
}
RingBuffer::RingBuffer() : buffer_id( 0 )
{
    this->status = 0;
    set_buff_type( _B_RING_BUFF );
}
void RingBuffer::swap()
{
    m_buffer.swap( m_swap_buffer );
}
RingBuffer::buffer_t& RingBuffer::get_swap()
{
    return m_swap_buffer;
}
RingBuffer::buffer_t& RingBuffer::get_buff()
{
    return m_buffer;
}

int RingBuffer::overflow( int wait )
{
    unique_lock< std::mutex > lock( m_mtx );
    if ( status & ( B_SCHE | B_SYNC ) )
        m_cv.wait( lock, [ this ] { return !( status & ( B_SCHE | B_SYNC ) ); } );
    swap();
    this->status |= B_SCHE;
    request_flush( this );
    if ( wait )
        wait_status( &( this->status ), ( B_SCHE | B_SYNC ), 0, &lock );
    return 0;
}

int RingBuffer::flush()
{
    return this->m_swap_buffer.flush();
}

void RingBuffer::register_self()
{
    register_ring_buffer( this );
}