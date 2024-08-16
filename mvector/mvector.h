#include <iostream>
#include <vector>
#include <string.h>
#include <iterator>
#include <sstream>
#include <cassert>
using namespace std;

template < typename T > class Iterator
{
public:
    using alloc_type          = std::allocator< T >;
    using alloc_trait_type    = allocator_traits< alloc_type >;
    using iterator_trait_type = iterator_traits< T* >;
    using difference_type     = typename iterator_trait_type::difference_type;
    using iterator_category   = typename iterator_trait_type::iterator_category;  // 或其他适当的迭代器标签
    using pointer             = typename iterator_trait_type::pointer;
    using reference           = typename iterator_trait_type::reference;
    using value_type          = typename iterator_trait_type::value_type;
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
    bool operator==( const Iterator& rth )
    {
        return _ptr == rth._ptr;
    }
    Iterator operator+( ptrdiff_t __n )
    {
        return Iterator( _ptr + __n );
    }
    Iterator& operator+=( ptrdiff_t __n )
    {
        _ptr += __n;
        return *this;
    }
    Iterator operator-( ptrdiff_t __n )
    {
        return Iterator( _ptr - __n );
    }
    Iterator& operator-=( ptrdiff_t __n )
    {
        _ptr -= __n;
        return *this;
    }
    ptrdiff_t operator-( const Iterator& oth )
    {
        return ( _ptr - oth._ptr );
    }
    value_type& operator[]( ptrdiff_t __n )
    {
        return _ptr[ __n ];
    }
    value_type* base()
    {
        return _ptr;
    }

private:
    T* _ptr;
};

template < typename T > class mvector
{
public:
    using iterator = Iterator< T >;
    mvector() : _M_cap(), _M_sz(), _M_start(), _M_end(), _M_storage(){};
    mvector( size_t sz ) : _M_sz()
    {
        assert( sz > 0 );
        _M_cap   = sz;
        _M_start = _M_end = new T[ _M_cap ];
        _M_storage        = _M_start + _M_cap;
        memset( _M_start, 0x00, sizeof( T ) * _M_cap );
    }
    ~mvector()
    {
        delete[] _M_start;
        _M_start = _M_end = _M_storage = nullptr;
        cout << " ~mvector" << endl;
    }
    T& append( T& t )
    {
        if ( _M_sz == _M_cap )
        {
            resize( 1 );
        }
        *_M_end = t;
        ++_M_end;
        ++_M_sz;
        return *( _M_end - 1 );
    }
    T& append( T&& t )
    {
        return append( t );
    }
    void print()
    {
        cout << " size=" << _M_sz << " cap=" << _M_cap << " addr=0x" << _M_start << endl;
    }
    T& operator[]( size_t index )
    {
        if ( index < _M_sz )
        {
            return *( _M_start + index );
        }
        throw out_of_range( "index out of range" );
    }
    T& back()
    {
        if ( _M_sz > 0 )
        {
            return *( _M_end - 1 );
        }
        throw out_of_range( "empty error" );
    }
    T& front()
    {
        if ( _M_sz > 0 )
        {
            return *( _M_start );
        }
        throw runtime_error( "empty error" );
    }
    template < typename... Args > T& emplace_back( Args... args )
    {
        if ( _M_sz == _M_cap )
        {
            resize( 1 );
        }
        new ( _M_end ) T( std::forward< Args >( args )... );
        ++_M_end;
        ++_M_sz;
        return *( _M_end - 1 );
    }
    size_t size()
    {
        return _M_sz;
    }
    bool empty()
    {
        return _M_sz == 0;
    }
    T remove( size_t index )
    {
        if ( index >= _M_sz )
        {
            throw out_of_range( "index out of range" );
        }
        T t = *( _M_start + index );
        if ( index != _M_sz - 1 )
        {
            memmove( _M_start + index, _M_start + index + 1, sizeof( T ) * ( _M_sz - index - 1 ) );
            *( _M_end - 1 ) = t;
        }
        --_M_end;
        --_M_sz;
        return t;
    }
    void insert( iterator __first1, iterator __last1, iterator __position )
    {
        size_t insert_n = __last1 - __first1;
        size_t __offset = __position - begin();
        if ( insert_n == 0 )
        {
            return;
        }
        if ( _M_sz + insert_n >= _M_cap )
        {
            resize( _M_sz + insert_n - _M_cap + 1 );
        }
        iterator __pos   = begin() + __offset;
        iterator new_end = std::copy_backward( __pos, end(), __pos + insert_n );
        std::copy( __first1, __last1, __pos );
        _M_end = new_end.base();
        _M_sz += insert_n;
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
            for ( size_t i = 0; i < _M_sz; i++ )
            {
                if ( !__start )
                {
                    __ss << *( _M_start + i );
                    __start = true;
                }
                else
                {
                    __ss << "," << *( _M_start + i );
                }
            }
            __ss << "}";
        }
        return __ss.str();
    }

    iterator begin()
    {
        return iterator( _M_start );
    }
    iterator end()
    {
        return iterator( _M_end );
    }

private:
    void resize( size_t __n )
    {
        __n              = std::max( __n, size_t( 1 ) );
        size_t __len     = std::max( _M_sz, __n );
        size_t new_cap   = _M_sz + __len;
        T*     old_start = _M_start;
        T*     new_start = new T[ new_cap ];
        if ( _M_sz > 0 )
        {
            std::copy( iterator( _M_start ), iterator( _M_end ), iterator( new_start ) );
        }
        _M_start   = new_start;
        _M_end     = _M_start + _M_sz;
        _M_storage = _M_start + new_cap;
        _M_cap     = new_cap;
        for ( int i = 0; i < _M_sz; i++ )
        {
            cout << "append: " << i << " :" << *( _M_start + i ) << endl;
        }
        cout << "------------------------------" << endl;
        delete[] old_start;
    }
    void __insert_resize( iterator __first1, iterator __last1, iterator __position )
    {
        size_t   __n       = __last1 - __first1;
        size_t   new_cap   = _M_sz + std::max( _M_sz, __n );
        T*       new_start = new T[ new_cap ];
        iterator new_end   = std::copy( begin(), __position, iterator( new_start ) );
        new_end            = std::copy( __first1, __last1, new_end );
        new_end            = std::copy( __position, iterator( _M_end ), new_end );
        _M_start           = new_start;
        _M_end             = new_end.base();
        _M_sz              = _M_end - _M_start;
        _M_storage         = _M_start + new_cap;
        _M_cap             = new_cap;
        for ( int i = 0; i < _M_sz; i++ )
        {
            cout << "[" << i << "] " << *( _M_start + i ) << endl;
        }
    }

public:
    size_t _M_sz;
    size_t _M_cap;

private:
    T* _M_start;
    T* _M_end;
    T* _M_storage;
};
