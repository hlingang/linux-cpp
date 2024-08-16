#include "mvector.h"
#include <unistd.h>
#include <algorithm>

struct TX
{
    int* p;
    TX() : p() {}
    TX( int __a )
    {
        p = new int( __a );
    }
    ~TX()
    {
        delete p;
        p = nullptr;
    }
    TX& operator=( TX&& rth ) noexcept
    {
        cout << " =&& " << endl;
        this->p = rth.p;
        rth.p   = nullptr;
        return *this;
    }

    TX& operator=( const TX& rth )
    {
        cout << " =& " << endl;
        p = new int( *rth.p );
        return *this;
    }
};

class MyClass
{
    constexpr static int value = 1;
};

int main()
{
    mvector< int > mvec;
    for ( int i = 0; i < 1000; i++ )
    {
        mvec.append( i );
        mvec.print();
        usleep( 1000 );
    }
    mvector< int > am;
    am.emplace_back( 10 );
    am.emplace_back( 11 );
    am.emplace_back( 12 );
    am.emplace_back( 13 );
    am.emplace_back( 14 );
    am.emplace_back( 15 );
    am.to_string();
    am.remove( 2 );
    am.to_string();
    size_t id = 0;
    for ( auto& val : am )
    {
        cout << "[" << id++ << "]"
             << " val: " << val << endl;
    }
    mvector< int > v1;
    mvector< int > v2;
    for ( int i = 0; i < 5; i++ )
    {
        v1.append( i + 10 );
        v2.append( i + 20 );
    }
    v2.insert( v1.begin(), v1.end(), v2.end() );
    std::for_each( v2.begin(), v2.end(), []( int val ) { cout << val << endl; } );
    aligned_storage< 32, alignof( int ) >::type __buf;
    new ( &__buf ) int( 4 );
    cout << "__buf    size = " << sizeof( __buf ) << endl;
    cout << "__buf address = " << ( void* )&__buf << endl;
    auto  alloc = allocator< TX >();
    auto* p     = alloc.allocate( 10 );
    for ( int i = 0; i < 5; i++ )
    {
        alloc.construct( p + i, i + 1 );
    }
    for ( int i = 0; i < 5; i++ )
    {
        cout << i << " : " << *( p + i )->p << endl;
    }
    cout << "---------------------------" << endl;
    alloc.destroy( p + 2 );
    std::copy( p + 3, p + 4, p + 2 );
    // std::move( p + 3, p + 4, p + 2 );
    for ( int i = 0; i < 5; i++ )
    {
        cout << i << " : " << *( p + i )->p << endl;
    }
    return 0;
}