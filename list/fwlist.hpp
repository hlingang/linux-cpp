#ifndef __FWLIST_HPP_
#define __FWLIST_HPP_

#include <cstdlib>
#include <type_traits>
#include <utility>
#include <iterator>

struct FWListNodeBase
{
    FWListNodeBase() : _M_next() {}

    FWListNodeBase* _M_next;
    void            _M_hook_after( FWListNodeBase* __p )
    {
        this->_M_next = __p->_M_next;
        __p->_M_next  = this;
    }
    void _M_unhook_after()
    {
        this->_M_next = this->_M_next->_M_next;
    }
};

struct FWListNodeHead : FWListNodeBase
{
    size_t _M_sz;
    FWListNodeHead()
    {
        _M_init();
    }
    void _M_init()
    {
        _M_next = nullptr;
        _M_sz   = 0;
    }
    FWListNodeBase* _M_base()
    {
        return this;
    }
    void _M_reverse()
    {
        FWListNodeBase* __next = nullptr;
        FWListNodeBase* __cur  = this->_M_next;
        FWListNodeBase* __temp = __cur->_M_next;
        for ( ; __cur; )
        {
            __cur->_M_next = __next;
            __next         = __cur;
            __cur          = __temp;
            __temp         = __cur->_M_next;
        }
        this->_M_next = __next;
    }
};

template < typename T > struct FWListNode : FWListNodeBase
{
    FWListNode() : FWListNodeBase(), _M_storage() {}
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

template < typename T > class FWListIterator
{
public:
    /// T 为实际存储数据类型
    using NodeType            = FWListNode< T >;
    using iterator_trait_type = std::iterator_traits< T* >;
    using iterator_category   = typename iterator_trait_type::iterator_category;
    using value_type          = typename iterator_trait_type::value_type;
    using pointer             = typename iterator_trait_type::pointer;
    using difference_type     = typename iterator_trait_type::difference_type;
    using reference           = typename iterator_trait_type::reference;

    FWListIterator() : _M_data(){};
    FWListIterator( const FWListIterator& oth ) = default;
    FWListIterator( FWListNodeBase* __data ) : _M_data( __data ) {}
    FWListIterator& operator=( const FWListIterator& oth ) = default;
    T&              operator*()
    {
        return *( static_cast< NodeType* >( _M_data )->_M_data() );
    }
    T* operator->()
    {
        return static_cast< NodeType* >( _M_data )->_M_data();
    }
    FWListIterator& operator++()
    {
        _M_data = _M_data->_M_next;
        return *this;
    }
    FWListIterator operator++( int )
    {
        FWListIterator temp = *this;
        ++this;
        return temp;
    }
    bool operator==( const FWListIterator& oth )
    {
        return this->_M_data == oth._M_data;
    }
    bool operator!=( const FWListIterator& oth )
    {
        return this->_M_data != oth._M_data;
    }
    FWListNodeBase* _M_node()
    {
        return _M_data;
    }

private:
    FWListNodeBase* _M_data;
};

template < typename T > class FWListBase
{
protected:
    using value_type  = T;
    using alloc_type  = std::allocator< T >;
    using alloc_trait = std::allocator_traits< alloc_type >;
    using NodeType    = FWListNode< T >;
    struct Impl
    {
        Impl() = default;
        FWListNodeHead _M_node;
    };
    Impl _M_impl;
    FWListBase() : _M_impl() {}
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
        FWListNodeBase* __p = _M_impl._M_node._M_base();
        for ( FWListNodeBase* __temp = __p->_M_next; __temp != nullptr; __temp = __p->_M_next )
        {
            __p->_M_unhook_after();
            _M_destroy_node( static_cast< NodeType* >( __temp ) );
        }
        _M_impl._M_node._M_init();
    }
};

template < typename T > class FWList : protected FWListBase< T >
{
public:
    using iterator     = FWListIterator< T >;
    using _Base        = FWListBase< T >;
    using value_type   = T;
    using pointer      = T*;
    using reference    = T&;
    using NodeType     = FWListNode< T >;
    using NodeBaseType = FWListNodeBase;
    using _Base::_M_create_node;
    using _Base::_M_dec_sz;
    using _Base::_M_destroy_node;
    using _Base::_M_impl;
    using _Base::_M_inc_sz;
    FWList() : _Base(){};
    FWList( size_t __n, const value_type& __x ) : _Base()
    {
        _M_default_init( __n, __x );
    }
    iterator insert_after( iterator __pos, const value_type& __x )
    {
        NodeType* __p = _M_create_node();
        ::new ( __p->_M_data() ) value_type( __x );
        __p->_M_hook_after( __pos._M_node() );
        _M_inc_sz( 1 );
        return iterator( __p );
    }
    iterator insert_after( iterator __pos, iterator __first, iterator __last )
    {
        for ( ; __first != __last; ++__first )
        {
            __pos = insert_after( __pos, *__first );
            _M_inc_sz( 1 );
        }
        return __pos;
    }
    template < typename... Args > iterator emplace_after( iterator __pos, Args&&... args )
    {
        NodeType* __p = _M_create_node( std::forward< Args >( args )... );
        __p->_M_hook_after( __pos._M_node() );
        _M_inc_sz( 1 );
        return iterator( __p );
    }
    template < typename... Args > iterator emplace( Args&&... args )
    {
        iterator __pos = _M_impl._M_node._M_base();
        for ( ; __pos._M_node()->_M_next; ++__pos )
        {
        }
        return emplace_after( __pos, std::forward< Args >( args )... );
    }
    iterator erase_after( iterator __pos )
    {
        auto* __temp = static_cast< NodeType* >( __pos._M_node()->_M_next );
        __pos._M_node()->_M_unhook_after();
        _M_destroy_node( __temp );
        _M_dec_sz( 1 );
        return __pos._M_node()->_M_next;
    }
    void erase_after( iterator __first, iterator __last )
    {
        iterator __pos = __first;
        for ( ; __first != __last; )
        {
            __first = erase_after( __pos );
        }
    }
    void erase_after_until_end( iterator __pos )
    {
        erase_after( __pos, end() );
    }
    iterator begin()
    {
        return iterator( _M_impl._M_node._M_base()->_M_next );
    }
    iterator begin_before()
    {
        return iterator( _M_impl._M_node._M_base() );
    }
    iterator end()
    {
        return iterator( nullptr );
    }
    void assign( iterator __first, iterator __last )
    {
        iterator __pre = begin_before();
        iterator __cur = begin();
        for ( ; __cur != end() && __first != __last; ++__cur, ++__first )
        {
            *__cur = *__first;
            __pre  = __cur;  // 备份前一个节点
        }
        if ( __cur == end() )
        {
            insert_after( __pre, __first, __last );
        }
        else
        {
            this->erase_after_until_end( __pre );
        }
    }
    void assign( size_t __n, const value_type& __x )
    {
        iterator __pre = begin_before();
        iterator __cur = begin();
        for ( ; __cur != end() && __n > 0; ++__cur, --__n )
        {
            *__cur = __x;
            __pre  = __cur;
        }
        if ( __cur == end() )
        {
            for ( ; __n > 0; --__n )
            {
                __pre = insert_after( __pre, __x );
            }
        }
        else
        {
            this->erase_after_until_end( __pre );
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