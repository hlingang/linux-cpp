#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <tuple>
#include <type_traits>
#include <unistd.h>
#include <set>
#include <algorithm>
#include <functional>
#include <vector>
#include <pthread.h>

#define CMD _IO( 0xA5, 1 )
#define NAME "/dev/demo"

using namespace std;

struct SelfStruct
{
    int id;
    SelfStruct( int _id ) : id( _id ) {}
};

ostream& operator<<( ostream& _os, const SelfStruct& _s )
{
    _os << "SelfStruct{id=" << _s.id << "}";
    return _os;
}

// 通用型辅助结构
template < bool cond > struct Condition
{
    static const bool value = cond;
};

using _false_type = Condition< false >;
using _true_type  = Condition< true >;

// OR 结构通用模板
template < bool... condn > struct OR
{
};
template <> struct OR<> : _false_type
{
};
template < bool cond > struct OR< cond > : Condition< cond >
{
};
// OR 结构特例模板
template < bool cond1, bool cond2 > struct OR< cond1, cond2 > : _true_type
{
};
// OR 结构特例模板
template <> struct OR< false, false > : _false_type
{
};
// OR 结构递推公式
template < bool cond1, bool cond2,  bool... condn >
struct OR< cond1, cond2, condn... > : OR< cond1, OR< cond2, condn... >::value >
{
};
// AND 结构通用模板
template < bool... condn > struct AND
{
};

template <> struct AND<> : _false_type
{
};

template < bool cond > struct AND< cond > : Condition< cond >
{
};
// AND 结构特例模板
template < bool cond1, bool cond2 > struct AND< cond1, cond2 > : _false_type
{
};
// AND 结构特例模板
template <> struct AND< true, true > : _true_type
{
};
// AND 结构递推模板
template < bool cond1, bool cond2, bool... condn >
struct AND< cond1, cond2, condn... > : AND< cond1, AND< cond2, condn... >::value >
{
};
// 通用型辅助结构
template < size_t... Ns > struct seq_helper
{
};
// 通用模板结构及其递推公式
template < size_t N, size_t... Is > struct index_seq : index_seq< N - 1, N - 1, Is... >
{
};
// 特例模板结构
template < size_t... Is > struct index_seq< 0, 0, Is... >
{
    using type = seq_helper< 0, Is... >;
};
template < size_t N, typename... Args > size_t __getdata( stringstream& __s, tuple< Args... >& t )
{
    __s << get< N >( t ) << ",";
    return N;
}
template < size_t... Is, typename... Args > void __printTuple( tuple< Args... >& t, seq_helper< Is... >& )
{
    stringstream                    ss;
    std::initializer_list< size_t > li{ __getdata< Is >( ss, t )... };
    cout << "value = " << ss.str() << endl;
}
template < typename... Args > void printTuple( tuple< Args... >& t )
{
    typename index_seq< sizeof...( Args ) >::type __value;
    return __printTuple( t, __value );
}

template < int N = 0 > class cv
{
};
template <> class cv< 0 >
{
public:
    const int a = 2;
};
template <> class cv< 1 >
{
};
template <> class cv< 2 >
{
};
template <> class cv< 3 >
{
};

struct Comp
{
    bool operator()( const int& a, const int& b ) const
    {
        return a > b;
    }
};

template < typename T > void call( T* p )
{
    p();
}
template < typename T > std::size_t __get_data__( stringstream& s, T& t )
{
    s << t << ",";
    return 0;
}

template < typename... T > void printArgs( T&&... t )
{
    stringstream                    ss;
    initializer_list< std::size_t > li{ __get_data__< T >( ss, t )... };
    cout << ss.str() << endl;
}

int main()
{
    printArgs( "abc", 11, 22, "def", 'v', 101, SelfStruct( 202 ) );
    cout << OR< 0, 0, 0, 1 >::value << endl;
    cout << OR< 0, 0, 1, 1 >::value << endl;
    cout << OR< 0, 1, 0, 1 >::value << endl;
    cout << OR< 1, 0, 0, 1 >::value << endl;
    cout << OR< 0, 0, 0, 0 >::value << endl;
    cout << "-------------\n";
    cout << AND< 1, 1, 1, 0 >::value << endl;
    cout << AND< 1, 1, 0, 1 >::value << endl;
    cout << AND< 1, 0, 1, 1 >::value << endl;
    cout << AND< 0, 1, 1, 1 >::value << endl;
    cout << AND< 1, 1, 1, 1 >::value << endl;
    return 0;
}
