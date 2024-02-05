#include <iostream>
#include <memory>
#include <map>

using namespace std;

struct Sample
{
    Sample() = default;
    Sample( int _a ) : a( _a ) {}
    int a;
};

struct Deleter
{
    void operator()( Sample* p )
    {
        if ( p )
        {
            free( p );
        }
    }
};

void func_Deleter( Sample* p )
{
    if ( p )
    {
        free( p );
    }
}

struct mySort
{
    bool operator()( const Sample& a1, const Sample& a2 ) const
    {
        return a1.a < a2.a;
    }
};

bool func_mySort( const Sample& a1, const Sample& a2 )
{
    return a1.a < a2.a;
}

int main()
{
    shared_ptr< Sample >          sptr( new Sample, Deleter() );
    unique_ptr< Sample, Deleter > uptr( new Sample, Deleter() );

    shared_ptr< Sample >                        _sptr( new Sample( 5 ), func_Deleter );
    unique_ptr< Sample, void ( * )( Sample* ) > _uptr( new Sample( 10 ), &func_Deleter );

    map< int, Sample, mySort >                                           ms;
    map< int, Sample, bool ( * )( const Sample& a1, const Sample& a2 ) > _ms( &func_mySort );
    ms.insert( { 1, Sample( 1 ) } );
    ms.insert( { 2, Sample( 2 ) } );

    _ms.insert( { 1, Sample( 2 ) } );
    _ms.insert( { 2, Sample( 1 ) } );
}