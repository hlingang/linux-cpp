#include <iostream>
#include <vector>
#include <string.h>
using namespace std;

#define DEFAULT_SIZE 2

template < typename T > class mvector
{
public:
    mvector() : mvector( DEFAULT_SIZE ) {}
    mvector( size_t sz ) : size_( 0 ), cap_( sz )
    {
        data_ = ( T* )malloc( cap_ * sizeof( mvector ) );
        memset( data_, 0x00, cap_ * sizeof( mvector ) );
    }
    ~mvector()
    {
        if ( data_ )
        {
            free( data_ );
            data_ = nullptr;
            cout << " ~mvector" << endl;
        }
    }
    T& append( T& t )
    {
        if ( cap_ <= size_ )
        {
            resize( size_, cap_ );
        }
        size_t msz       = size_;
        data_[ size_++ ] = t;
        return data_[ msz ];
    }
    T& append( T&& t )
    {
        return append( t );
    }
    void print()
    {
        cout << " size=" << size_ << " cap=" << cap_ << endl;
    }
    T& operator[]( size_t index )
    {
        if ( index < size_ )
        {
            return data_[ index ];
        }
        else
        {
            throw runtime_error( "index out of range" );
        }
    }
    T& back()
    {
        if ( size_ > 0 )
        {
            return data_[ size_ - 1 ];
        }
        else
        {
            throw runtime_error( "empty error" );
        }
    }
    template < typename... Args > T& emplace_back( Args... args )
    {
        if ( cap_ <= size_ )
        {
            resize( size_, cap_ );
        }
        size_t msz = size_;
        new ( &data_[ size_++ ] ) T( std::forward( args )... );
        return data_[ msz ];
    }

private:
    void resize( size_t sz, size_t cap )
    {
        size_t r_cap = cap == 0 ? DEFAULT_SIZE : 2 * cap;
        if ( data_ )
        {
            data_ = ( T* )realloc( data_, r_cap * sizeof( T ) );
            memset( data_ + sz, 0x00, sizeof( T ) * ( r_cap - sz ) );
        }
        else
        {
            data_ = ( T* )malloc( sizeof( T ) * r_cap );
            memset( data_, 0x00, sizeof( T ) * r_cap );
        }
        cap_ = r_cap;
    }

public:
    size_t size_;
    size_t cap_;

private:
    T* data_;
};
