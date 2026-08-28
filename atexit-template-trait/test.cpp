
#include <iostream>
#include <invoker.h>
#include <random>
#include <numeric>
#include <unistd.h>
#include <tuple>
#include <sstream>
#include <vector>
#include <array>
#include <string.h>

using namespace std;

unsigned long val = 1000;

void main_loop()
{
    printf( "main-loop...\n" );
    sleep( 2 );
}

template < int... Is > struct __seq
{
};

template < int N, int... M > struct __seq_index : __seq_index< N - 1, N - 1, M... >
{
};

template < int... M > struct __seq_index< 0, M... >
{
    using type = struct __seq< M... >;
};
template < typename Tp > int __sput( stringstream& __ss, Tp&& __v )
{

    __ss << __v << ", ";
    return 0;
}
template < typename > struct __printHelper
{
    template < typename... Args > static void __print( tuple< Args... >& __t ) {}
};

template < int... Is, template < int... > class _Temp > struct __printHelper< _Temp< Is... > >
{
    template < typename... Args > static void __print( tuple< Args... >& __t )
    {
        stringstream  __ss;
        vector< int > __vec = { __sput( __ss, get< Is >( __t ) )... };
        cout << __ss.str() << endl;
    }
};

template < int... Is, typename... Args > void __print( const __seq< Is... >&, const tuple< Args... >& __t )
{
    stringstream  __ss;
    vector< int > __vec = { __sput( __ss, get< Is >( __t ) )... };
    cout << __ss.str() << endl;
}

template < typename... Args > void printTuple( tuple< Args... >& __t )
{

    __print( typename __seq_index< sizeof...( Args ) >::type(), __t );
    __printHelper< typename __seq_index< sizeof...( Args ) >::type >::__print( __t );
}
template < bool cond > struct _Cond
{
    static const bool value = cond;
};
template < bool... cond > struct And
{
};
template < bool cond1, bool cond2 > struct And< cond1, cond2 > : _Cond< cond1 && cond2 >
{
};
template < bool cond, bool... conds > struct And< cond, conds... > : And< cond, And< conds... >::value >
{
};

template < int N, typename Tp > struct _Value
{
    using value_type = Tp;
    _Value( const Tp& __val ) : value( __val ) {}
    _Value() : value() {}
    Tp value;
    Tp get() const
    {
        return value;
    }
};
template < int N, typename... Args > struct TupleImpl
{
};
template < int N, typename Arg1, typename... Args >
struct TupleImpl< N, Arg1, Args... > : _Value< N, Arg1 >, TupleImpl< N + 1, Args... >
{
    using _BaseType = _Value< N, Arg1 >;
    TupleImpl( Arg1 __arg1, Args... __args ) : _Value< N, Arg1 >( __arg1 ), TupleImpl< N + 1, Args... >( __args... ) {};
    TupleImpl() : _Value< N, Arg1 >(), TupleImpl< N + 1, Args... >() {};
    typename _BaseType::value_type get() const
    {
        return _BaseType::get();
    }
};
template < int N, typename Arg > struct TupleImpl< N, Arg > : _Value< N, Arg >
{
    using _BaseType = _Value< N, Arg >;
    TupleImpl( Arg __arg ) : _Value< N, Arg >( __arg ) {}
    TupleImpl() : _Value< N, Arg >() {}
    typename _BaseType::value_type get() const
    {
        return _BaseType::get();
    }
};

template < typename... Args > struct Tuple : TupleImpl< 0, Args... >
{
    Tuple( Args... __args ) : TupleImpl< 0, Args... >( __args... ) {}
    Tuple() : TupleImpl< 0, Args... >() {}
};
// 自动完成协变类型转换
template < int N, typename... Args >
typename TupleImpl< N, Args... >::_BaseType::value_type tp_get( const TupleImpl< N, Args... >& __t )
{
    return __t.get();
}

class A
{
    friend class B;

public:
    int a;

protected:
    int b;

private:
    int c;
};

class B : private A
{
    B() : A()
    {
        a = 1;
        b = 2;
        c = 3;
    }
};

template < typename... Args > struct status_check
{
    status_check( Args&&... args ) : __m_args( args... ) {}
    operator() {}
    tuple< Args..> __m_args;
};

int main()
{
    atexit( exit_0 );  // 反序注册
    atexit( exit_1 );
    main_loop();
    tuple< int, string, double > __t( 99, "hello-world", 3.14 );
    printTuple( __t );
    cout << And< 0, 1, 1, 0, 1 >::value << endl;
    // count << AND< 1 >::value << endl;  // 编译期屏蔽
    Tuple< int, string, double, char > __tp( 1, "hello", 3.14, 'w' );
    Tuple< int, string, double, char > __tpx{};
    cout << tp_get< 0 >( __tp ) << endl;
    cout << tp_get< 1 >( __tp ) << endl;
    cout << tp_get< 2 >( __tp ) << endl;
    cout << tp_get< 3 >( __tp ) << endl;
    return 0;
}
