#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H
#include <flush_base.h>
#include <ring-buffer.h>
#include <fstream>
#include <ring-buffer-manager.h>

using namespace std;

template < typename Tp > struct domin_type
{
    using vaoue_type                  = Tp;
    static constexpr const char* name = "pyload_unknown";
};
template <> struct domin_type< payload_t& >
{
    using type                        = payload_t;
    static constexpr const char* name = "pyload_t";
};

template < typename Tp > struct FileBuffer : public FlushBase
{
    using buffer_t   = RingBufferBase;
    using value_type = Tp;
    buffer_t                      m_buffer;
    fstream                       fs;
    string                        name;
    vector< streampos >           m_vecpos;
    uint64_t                      m_nbytes;
    condition_variable            m_cv;
    static constexpr const size_t value_size = sizeof( value_type );
    int                           write( Tp&& arg, int id )
    {
        if ( nullptr == m_buffer.emplace_back( arg ) )
        {
            overflow();
            return m_buffer.emplace_back( arg ) == nullptr ? -1 : 0;  // retry
        }
        return 0;
    }
    int flush() override
    {
        overflow();
        return 0;
    }
    FileBuffer( const char* __name ) : name( __name ), m_nbytes( 0 )
    {
        fs.open( name, ios::in | ios::out | ios::binary | ios::trunc );
        set_buff_type( _B_FILE_BUFF );
    }
    FileBuffer( const string& __name ) : name( __name )
    {
        fs.open( name, ios::in | ios::out | ios::binary | ios::trunc );
        set_buff_type( _B_FILE_BUFF );
    }
    int overflow( int wait = 0 ) override
    {
        std::lock_guard< std::mutex > lock( m_buffer._M_mutex() );
        if ( !m_buffer._M_read_pos() && !m_buffer._M_write_pos() )
            return 0;  // double check if the buffer is empty, if it is empty, just return
        status |= B_SYNC;
        fs.write( m_buffer._M_data(), m_buffer._M_write_pos() );
        if ( wait )
            fs.flush();
        m_vecpos.push_back( m_nbytes );
        m_nbytes += m_buffer._M_write_pos();
        m_buffer.__reset();
        status &= ~B_SYNC;
        m_cv.notify_one();
        return 0;
    }
};
template < typename Tp > FileBuffer< Tp >* get_file_buffer()
{
    static FileBuffer< Tp > __file_buffer( string( domin_type< Tp >::name ) + "-" + string( "data.bin" ) );
    static std::once_flag   __flag;
    std::call_once( __flag, []() {
        printf( "file ring-buff=[%p]\n", &__file_buffer );
        register_ring_buffer( &__file_buffer );
    } );
    return &__file_buffer;
}
template < typename Tp > int write_to_file_buffer( Tp&& value, int id )
{
    auto* file_buffer = get_file_buffer< Tp >();
    if ( file_buffer )
        return file_buffer->write( std::forward< Tp >( value ), id );
    return -1;
}
#endif