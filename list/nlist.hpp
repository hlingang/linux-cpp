#ifndef __NODE_HPP_
#define __NODE_HPP_

#include <cstdlib>
#include <type_traits>
#include <utility>
#include <iterator>

struct ListNodeBase
{
    ListNodeBase() : _M_pre(), _M_next() {}
    ListNodeBase* _M_pre;
    ListNodeBase* _M_next;
    void          _M_hook( ListNodeBase* __p )
    {
        this->_M_next         = __p;
        this->_M_pre          = __p->_M_pre;
        this->_M_next->_M_pre = this;
        this->_M_pre->_M_next = this;
    }
    void _M_unhook()
    {
        this->_M_pre->_M_next = this->_M_next;
        this->_M_next->_M_pre = this->_M_pre;
    }
};

struct ListNodeHead : ListNodeBase
{
    size_t _M_sz;
    ListNodeHead()
    {
        _M_init();
    }
    void _M_init()
    {
        _M_pre = _M_next = this;
        _M_sz            = 0;
    }
    ListNodeBase* _M_base()
    {
        return this;
    }
    void _M_reverse()
    {
        ListNodeBase* __next = this->_M_base();
        ListNodeBase* __cur  = this->_M_next;
        for ( ; __cur != this->_M_base(); )
        {
            __cur->_M_pre  = __cur->_M_next;
            __cur->_M_next = __next;
            __next         = __cur;
            __cur          = __cur->_M_pre;
        }
        __cur->_M_next = __next;
        __next->_M_pre = __cur;
    }
};

template < typename T > struct ListNode : ListNodeBase
{
    ListNode() : ListNodeBase(), _M_storage() {}
    typename std::aligned_storage< sizeof( T ), alignof( T ) >::type _M_storage;
    T*                                                               _M_data()
    {
        return reinterpret_cast< T* >( &_M_storage );
    }
    const T* _M_data() const
    {
        return reinterpret_cast< T* >( &_M_storage );
    }
};

template < typename T > class ListIterator
{
public:
    /// T 为实际存储数据类型
    using NodeType            = ListNode< T >;
    using iterator_trait_type = std::iterator_traits< T* >;
    using iterator_category   = typename iterator_trait_type::iterator_category;
    using value_type          = typename iterator_trait_type::value_type;
    using pointer             = typename iterator_trait_type::pointer;
    using difference_type     = typename iterator_trait_type::difference_type;
    using reference           = typename iterator_trait_type::reference;

    ListIterator() : _M_data(){};
    ListIterator( const ListIterator& oth ) = default;
    ListIterator( ListNodeBase* __data ) : _M_data( __data ) {}
    ListIterator& operator=( const ListIterator& oth ) = default;
    T&            operator*()
    {
        return *( static_cast< NodeType* >( _M_data )->_M_data() );
    }
    T* operator->()
    {
        return static_cast< NodeType* >( _M_data )->_M_data();
    }
    ListIterator& operator++()
    {
        _M_data = _M_data->_M_next;
        return *this;
    }
    ListIterator& operator--()
    {
        _M_data = _M_data->_M_pre;
        return *this;
    }
    ListIterator operator++( int )
    {
        ListIterator temp = *this;
        ++this;
        return temp;
    }
    ListIterator operator--( int )
    {
        ListIterator temp = *this;
        --this;
        return temp;
    }
    bool operator==( const ListIterator& oth )
    {
        return this->_M_data == oth._M_data;
    }
    bool operator!=( const ListIterator& oth )
    {
        return this->_M_data != oth._M_data;
    }
    ListNodeBase* _M_node()
    {
        return _M_data;
    }

private:
    ListNodeBase* _M_data;
};

template < typename T > class ListBase
{
protected:
    using value_type  = T;
    using alloc_type  = std::allocator< T >;
    using alloc_trait = std::allocator_traits< alloc_type >;
    using NodeType    = ListNode< T >;
    struct Impl
    {
        Impl() = default;
        ListNodeHead _M_node;
    };
    Impl _M_impl;
    ListBase() : _M_impl() {}
    NodeType* _M_create_node()
    {
        auto* __p = ::new NodeType();
        return __p;
    }
    template < typename... _Args > NodeType* _M_create_node( _Args&&... args )
    {
        NodeType* __p = _M_create_node();
        ::new ( __p->_M_data() ) value_type( std::forward< _Args >( args )... );
        return __p;
    }
    void _M_destroy_node( NodeType* __p )
    {
        __p->_M_data()->~value_type();
        delete __p;
    }
    size_t _M_inc_sz( size_t __n )
    {
        return _M_impl._M_node._M_sz += __n;
    }
    size_t _M_dec_sz( size_t __n )
    {
        return _M_impl._M_node._M_sz -= __n;
    }
    void _M_clear()
    {
        ListNodeBase* __p = _M_impl._M_node._M_base();
        for ( ListNodeBase* __temp = __p->_M_next; __temp != _M_impl._M_node._M_base(); __temp = __p->_M_next )
        {
            __temp->_M_unhook();
            _M_destroy_node( static_cast< NodeType* >( __temp ) );
        }
        _M_impl._M_node._M_init();
    }
};

template < typename T > class List : protected ListBase< T >
{
public:
    using iterator     = ListIterator< T >;
    using _Base        = ListBase< T >;
    using value_type   = T;
    using pointer      = T*;
    using reference    = T&;
    using NodeType     = ListNode< T >;
    using NodeBaseType = ListNodeBase;
    using _Base::_M_create_node;
    using _Base::_M_dec_sz;
    using _Base::_M_destroy_node;
    using _Base::_M_impl;
    using _Base::_M_inc_sz;
    List() : _Base(){};
    List( size_t __n, const value_type& __x ) : _Base()
    {
        _M_default_init( __n, __x );
    }
    iterator insert( iterator __pos, const value_type& __x )
    {
        NodeType* __p = _M_create_node();
        ::new ( __p->_M_data() ) value_type( __x );
        __p->_M_hook( __pos._M_node() );
        _M_inc_sz( 1 );
        return iterator( __p );
    }
    iterator insert( iterator __pos, iterator __first, iterator __last )
    {
        iterator __iter = __pos;
        for ( ; __first != __last; ++__first )
        {
            __iter = insert( __pos, *__first );
            __pos  = ++__iter;
            _M_inc_sz( 1 );
        }
        return __iter;
    }
    template < typename... Args > iterator emplace( iterator __pos, Args&&... args )
    {
        NodeType* __p = _M_create_node( std::forward< Args >( args )... );
        __p->_M_hook( __pos._M_node() );
        _M_inc_sz( 1 );
        return iterator( __p );
    }
    template < typename... Args > iterator emplace( Args&&... args )
    {
        return emplace( end(), std::forward< Args >( args )... );
    }
    iterator erase( iterator __pos )
    {
        iterator __next = iterator( __pos._M_node()->_M_next );
        auto*    __temp = static_cast< NodeType* >( __pos._M_node() );
        __pos._M_node()->_M_unhook();
        _M_destroy_node( __temp );
        _M_dec_sz( 1 );
        return __next;
    }
    void erase( iterator __first, iterator __last )
    {
        for ( ; __first != __last; )
        {
            __first = erase( __first );
        }
    }
    void erase_until_end( iterator __pos )
    {
        erase( __pos, end() );
    }
    iterator begin()
    {
        return iterator( _M_impl._M_node._M_base()->_M_next );
    }
    iterator end()
    {
        return iterator( _M_impl._M_node._M_base() );
    }
    void assign( iterator __first, iterator __last )
    {
        iterator __cur = begin();
        for ( ; __cur != end() && __first != __last; ++__cur, ++__first )
        {
            *__cur = *__first;
        }
        if ( __cur == end() )
        {
            insert( end(), __first, __last );
        }
        else
        {
            this->erase_until_end( __cur );
        }
    }
    void assign( size_t __n, const value_type& __x )
    {
        iterator __cur = begin();
        for ( ; __cur != end() && __n > 0; ++__cur, --__n )
        {
            *__cur = __x;
        }
        if ( __cur == end() )
        {
            for ( ; __n > 0; --__n )
            {
                insert( end(), __x );
            }
        }
        else
        {
            this->erase_until_end( __cur );
        }
    }
    void clear()
    {
        _Base::_M_clear();
    }
    void reverse()
    {
        _M_impl._M_node._M_reverse();
    }

private:
    void _M_default_init( size_t __n, const value_type& __x )
    {
        for ( ; __n > 0; --__n )
        {
            emplace( __x );
        }
    }
};

#endif