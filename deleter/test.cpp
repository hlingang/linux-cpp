#include <iostream>
#include <memory>
#include <map>
#include <sstream>
#include <fstream>

using namespace std;

/*
1. 同时传递类型和对象 （类和函数）    unique_ptr
2. 传递对象          （类和函数）    shared_ptr
3. 传递类型          （类）         sort

*/

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
        cout << "~deleter .class" << endl;
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
        cout << "~deleter .function" << endl;
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

ostream& operator<<( ostream& ss, const Sample& _a )  // 参数位置规则
{
    ss << "Sample{" << _a.a << "}";
    return ss;
}

int main()
{
    do
    {
        shared_ptr< Sample >          sptr( new Sample, Deleter() );
        unique_ptr< Sample, Deleter > uptr( new Sample, Deleter() );

        shared_ptr< Sample >                        _sptr( new Sample( 5 ), func_Deleter );
        unique_ptr< Sample, void ( * )( Sample* ) > _uptr( new Sample( 10 ), &func_Deleter );

        map< Sample, int, mySort >                                           ms;
        map< Sample, int, bool ( * )( const Sample& a1, const Sample& a2 ) > _ms( &func_mySort );
        ms.insert( { Sample( 1 ), 1 } );
        ms.insert( { Sample( 2 ), 2 } );

        for ( auto iter = ms.begin(); iter != ms.end(); iter++ )
        {
            cout << "key:" << iter->first << ", value:" << iter->second << endl;
        }

        _ms.insert( { Sample( 2 ), 2 } );
        _ms.insert( { Sample( 1 ), 1 } );

        for ( auto iter = _ms.begin(); iter != _ms.end(); iter++ )
        {
            cout << "key:" << iter->first << ", value:" << iter->second << endl;
        }
    } while ( 0 );
}