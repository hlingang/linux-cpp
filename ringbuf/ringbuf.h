#include <iostream>
#include <cstring>
#include <thread>
#include "spin_lock.h"

using namespace std;

class Ringbuf
{
public:
    Ringbuf() : __M_sz( 0 ), __M_read(), __M_write(), __M_start(), __M_end(){};
    Ringbuf( size_t __n ) : __M_sz( 0 ), __M_cap( __n )
    {
        __M_start = __M_read = __M_write = new char[ __n ];
        __M_end                          = &__M_read[ __n ];
        printf( "Init  __M_read = %p\n", __M_read );
        printf( "Init __M_write = %p\n", __M_write );
    }
    ~Ringbuf()
    {
        delete[] __M_start;
        __M_start = __M_read = __M_write = __M_end = nullptr;
        __M_sz                                     = 0;
        __M_cap                                    = 0;
    }
    size_t write( void* __data, size_t __n )
    {
        static int                   __idx = 0;
        std::lock_guard< spin_lock > lock( __M_lock );
        size_t                       __remaining = __M_cap - __M_sz;
        if ( __n > __remaining )
        {
            return 0;
        }
        if ( __remaining == 0 || __n == 0 )
        {
            return 0;
        }
        size_t __len  = std::min( __remaining, __n );
        auto   __len1 = ( size_t )( __M_end - __M_write );
        if ( __len <= __len1 )
        {
            memcpy( __M_write, __data, __len );
            cout << "[" << __idx++ << "]"
                 << " write data: " << *reinterpret_cast< int* >( __M_write ) << " | " << ( void* )__M_write << endl;
            __M_write += __len;
            __M_sz += __len;
            return __len;
        }
        cout << "[" << __idx++ << "]"
             << " write data: " << *reinterpret_cast< int* >( __data ) << " | " << ( void* )__M_write << endl;
        if ( __len1 > 0 )
        {
            memcpy( __M_write, __data, __len1 );
            __M_write = __M_end;
            __M_sz += __len1;
        }
        if ( __len - __len1 > 0 )
        {
            memcpy( __M_start, static_cast< char* >( __data ) + __len1, __len - __len1 );
            __M_write = __M_start + ( __len - __len1 );
            __M_sz += __len - __len1;
        }
        return __len;
    }

    size_t read( void* __data, size_t __n )
    {
        static int __idx = 0;
        if ( __n > __M_sz )
        {
            return 0;
        }
        std::lock_guard< spin_lock > lock( __M_lock );
        if ( __n == 0 || __M_sz == 0 )
        {
            return 0;
        }
        auto __len = std::min( __M_sz, __n );

        auto __len1 = ( size_t )( __M_end - __M_read );
        if ( __M_read < __M_write || __len <= __len1 )
        {
            memcpy( __data, __M_read, __len );
            cout << "[" << __idx++ << "]"
                 << " read  data: " << *reinterpret_cast< int* >( __M_read ) << " | " << ( void* )__M_read << endl;
            __M_read += __len;
            __M_sz -= __len;
            return __len;
        }
        // actual read length = __len
        cout << "[" << __idx++ << "]"
             << " read  data: " << *reinterpret_cast< int* >( __data ) << " | " << ( void* )__M_read << endl;
        if ( __len1 > 0 )
        {
            memcpy( __data, __M_read, __len1 );
            __M_read = __M_end;
            __M_sz -= __len1;
        }
        if ( __len - __len1 > 0 )
        {
            memcpy( static_cast< char* >( __data ) + __len1, __M_start, __len - __len1 );
            __M_read = __M_start + ( __len - __len1 );
            __M_sz -= __len - __len1;
        }
        return __len;
    }

private:
    size_t    __M_sz;
    size_t    __M_cap;
    char*     __M_read;
    char*     __M_write;
    char*     __M_start;
    char*     __M_end;
    spin_lock __M_lock;
};
