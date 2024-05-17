#include <iostream>
#include <vector>
#include <string.h>
#include <iterator>
#include <sstream>
using namespace std;

#define DEFAULT_SIZE 2

template < typename T > class mvector
{
public:
    class Iterator
    {
    public:
        Iterator( T* ptr ) : _ptr( ptr ) {}
        Iterator( const Iterator& rth )            = default;
        Iterator& operator=( const Iterator& rth ) = default;

        T& operator*()
        {
            return *_ptr;
        }
        T* operator->()
        {
            return _ptr;
        }
        Iterator& operator++()
        {
            ++_ptr;
            return *this;
        }
        Iterator& operator--()
        {
            --_ptr;
            return *this;
        }
        bool operator!=( const Iterator& rth )
        {
            return _ptr != rth._ptr;
        }

    private:
        T* _ptr;
    };
    mvector() : mvector( DEFAULT_SIZE ) {}
    mvector( size_t sz ) : size_( 0 )
    {
        cap_  = sz > 0 ? sz : DEFAULT_SIZE;
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
        if ( cap_ <= size_ + 1 )
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
        cout << " size=" << size_ << " cap=" << cap_ << " addr=0x" << data_ << endl;
    }
    T& operator[]( size_t index )
    {
        if ( index < size_ )
        {
            return data_[ index ];
        }
        throw out_of_range( "index out of range" );
    }
    T& back()
    {
        if ( size_ > 0 )
        {
            return data_[ size_ - 1 ];
        }
        throw out_of_range( "empty error" );
    }
    T& front()
    {
        if ( size_ > 0 )
        {
            return data_[ 0 ];
        }
        throw runtime_error( "empty error" );
    }
    template < typename... Args > T& emplace_back( Args... args )
    {
        if ( cap_ <= size_ )

        {
            resize( size_, cap_ );
        }
        size_t msz = size_;
        new ( &data_[ size_++ ] ) T( std::forward< Args >( args )... );
        return data_[ msz ];
    }
    size_t size()
    {
        return size_;
    }
    bool empty()
    {
        return size_ == 0;
    }
    T remove( size_t index )
    {
        if ( index < size_ )
        {
            T t = data_[ index ];
            if ( index < size_ - 1 )
            {
                memmove( &data_[ index ], &data_[ index + 1 ], sizeof( T ) * ( size_ - index - 1 ) );
                data_[ size_ - 1 ] = t;
            }
            --size_;
            return t;
        }
        throw out_of_range( "index out of range" );
    }

    string to_string()
    {
        stringstream __ss;
        if ( empty() )
        {
            __ss << "null";
        }
        else
        {
            __ss << "{";
            bool __start = false;
            for ( size_t i = 0; i < size_; i++ )
            {
                if ( !__start )
                {
                    __ss << data_[ i ];
                    __start = true;
                }
                else
                {
                    __ss << "," << data_[ i ];
                }
            }
            __ss << "}";
        }
        return __ss.str();
    }

    Iterator begin()
    {
        if ( size_ > 0 )
        {
            return Iterator( data_ );
        }
        return nullptr;
    }
    Iterator end()
    {
        if ( size_ > 0 )
        {
            return Iterator( data_ + size_ );
        }
        return nullptr;
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
