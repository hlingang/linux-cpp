#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

#define check_ret_call( cmd ) \
    do                        \
    {                         \
        int ret = ( cmd );    \
        if ( ret < 0 )        \
            return ret;       \
    } while ( 0 )

template < size_t... Is > struct Index_seq
{
};

template < size_t N, size_t... M > struct Index_seq_impl : Index_seq_impl< N - 1, N - 1, M... >
{
};
template < size_t... Is > struct Index_seq_impl< 0, 0, Is... >
{
    using type = Index_seq< 0, Is... >;
};
template < size_t N > typename Index_seq_impl< N >::type make_index_seq()
{
    return typename Index_seq_impl< N >::type();
}
void func( void* args, void* ret )
{
    int a             = *( ( int* )args );
    *( ( int* )ret )  = ++a;
    *( ( int* )args ) = a;
    return;
};

struct die_handle
{
    unsigned int handle;
};

template < int N, typename T > struct Params_check_base
{
    Params_check_base( T __t ) : value( __t ) {}
    virtual int operator()()
    {
        return 0;
    }
    T value;
};
template < int N, typename T > struct Params_check_entity : public Params_check_base< N, T >
{
    using Params_check_base< N, T >::value;
    Params_check_entity( T __t ) : Params_check_base< N, T >( __t ) {}
    int operator()()
    {
        return 0;
    }
};

template < int N, typename T > struct Params_check_entity< N, T* > : Params_check_base< N, T* >
{
    using Params_check_base< N, T* >::value;
    Params_check_entity( T* __t ) : Params_check_base< N, T* >( __t ) {}
    int operator()()
    {
        if ( value == nullptr )
        {
            return -10;
        }
        return 0;
    }
};
template < int N > struct Params_check_entity< N, const char* > : Params_check_base< N, const char* >
{
    using Params_check_base< N, const char* >::value;
    Params_check_entity( const char*&& __t ) : Params_check_base< N, const char* >( __t ) {}
    int operator()()
    {
        if ( !strcasecmp( value, "hello" ) )
        {
            return -12;
        }
        return 0;
    }
};
template < int N > struct Params_check_entity< N, die_handle* > : Params_check_base< N, die_handle* >
{
    using Params_check_base< N, die_handle* >::value;
    Params_check_entity( die_handle* __t ) : Params_check_base< N, die_handle* >( __t ) {}
    int operator()()
    {
        if ( value->handle == 100 )
        {
            return -13;
        }
        return 0;
    }
};
template < int N, typename... > struct Params_check_impl;
template < int N, typename T, typename... Args >
struct Params_check_impl< N, T, Args... > : Params_check_entity< N, T >, Params_check_impl< N + 1, Args... >
{
    using _Base     = Params_check_entity< N, T >;
    using _Inherite = Params_check_impl< N + 1, Args... >;
    Params_check_impl( T&& __t, Args&&... args )
        : _Base( std::forward< T >( __t ) ), _Inherite( std::forward< Args >( args )... )
    {
    }
    int operator()()
    {
        int ret = _Base::operator()();
        printf( "ret=[%d]\n", ret );
        return _Inherite::operator()();
    }
};
template < int N, typename T > struct Params_check_impl< N, T > : Params_check_entity< N, T >
{
    using _Base = Params_check_entity< N, T >;
    Params_check_impl( T&& __t ) : _Base( std::forward< T >( __t ) ) {}
    int operator()()
    {
        int ret = _Base::operator()();
        printf( "ret=[%d]\n", ret );
        return ret;
    }
};

template < typename... Args > struct Params_check
{
    Params_check( Args&&... args ) : _M_check( std::forward< Args >( args )... ) {}
    Params_check_impl< 0, Args... > _M_check;
    template < size_t... Is > int   operator()()
    {
        return _M_check();
    }
};

template < typename... Args > using params_check_t = Params_check< Args... >;

template < typename... Args > Params_check< Args... > get_params_check( Args&&... args )
{
    return Params_check< Args... >( std::forward< Args >( args )... );
}
#define check_init                 \
    {                              \
        printf( "check init!\n" ); \
    }
template < bool b_check_init = true > struct Params_check_dispatch
{
    template < typename... Args > int operator()( Args&&... args )
    {
        check_init;
        return get_params_check< Args... >( std::forward< Args >( args )... )();
    }
};
template <> struct Params_check_dispatch< false >
{
    template < typename... Args > int operator()( Args&&... args )
    {
        return get_params_check< Args... >( std::forward< Args >( args )... )();
    }
};
template < bool b_check_init > Params_check_dispatch< b_check_init > get_params_check_dispatch()
{
    return Params_check_dispatch< b_check_init >();
}
Params_check_dispatch<> get_params_check_dispatch()
{
    return Params_check_dispatch<>();
}

template < typename... Args > struct Type_check
{
    static void test()
    {
        vector< int > ret = { std::is_same< Args, int >::value... };
        for ( auto& value : ret )
            printf( "check ret = [%d]\n", value );
    }
};

template < int N, typename T > struct Type_check_impl_base
{
    using type = T;
    template < typename U > static void is_convertible()
    {
        int ret = std::is_convertible< T, U >::value;
        printf( "type-check-ret = [%d]\n", ret );
    }
};

template < int N, typename... Args > struct Type_check_impl
{
};
// 递归继承，通过索引 [N] 避免重复继承
template < int N, typename T, typename... Args >
struct Type_check_impl< N, T, Args... > : Type_check_impl_base< N, T >, Type_check_impl< N + 1, Args... >
{
    template < typename U >  //// 编译期递归展开函数调用体
    static void recursive_convertible()
    {
        Type_check_impl_base< N, T >::template is_convertible< U >();              // 调用模板类的模板函数//多级模板参数
        Type_check_impl< N + 1, Args... >::template recursive_convertible< U >();  // 调用模板类的模板函数//多级模板参数
    }
    template < typename U > static void check_convertible()
    {
        Type_check_impl_base< N, T >::template is_convertible< U >();
    }
};

template < int N, typename T > struct Type_check_impl< N, T > : Type_check_impl_base< N, T >
{
    template < typename U > static void check_convertible()
    {
        Type_check_impl_base< N, T >::template is_convertible< U >();
    }

    template < typename U > static void recursive_convertible()
    {
        Type_check_impl_base< N, T >::template is_convertible< U >();
    }
};

template < int N, typename U, typename... Args > void check_convertible_by_index( Type_check_impl< N, Args... >& __t )
{
    Type_check_impl< N, Args... >::template check_convertible< U >();
}

template < typename... Args > Type_check_impl< 0, Args... > get_type_check()
{
    return Type_check_impl< 0, Args... >();
}

template < typename... Args > struct Type_check_interface : Type_check_impl< 0, Args... >
{
};

template < typename T, typename U > void is_convertible_x( int i )
{
    cout << "ret = " << std::is_convertible< T, U >::value << endl;
}
template < typename T, typename U, typename... Args > void is_convertible_x()
{
    is_convertible_x< T, U >( 0 );
    if constexpr ( sizeof...( Args ) > 0 )
        is_convertible_x< T, Args... >();
}
template < typename T > struct is_convertible_aux
{
};

template < size_t N > struct is_convertible_aux_base
{
    static int update_table()
    {
        return 0;
    }
};

template < size_t N > struct is_convertible_aux< Index_seq< N > > : is_convertible_aux_base< N >
{
};
template < size_t N, size_t... Is >
struct is_convertible_aux< Index_seq< N, Is... > > : is_convertible_aux_base< N >,
                                                     is_convertible_aux< Index_seq< Is... > >
{
    static int update_table()
    {
        is_convertible_aux_base< N >::update_table();
        is_convertible_aux< Index_seq< Is... > >::update_table();
        return 0;
    }
};

struct s_test_type
{
    int value;
};
void __test_type( const s_test_type& __t )
{
    __test_type( std::move( __t ) );
}

void __test_type( s_test_type&& __t )
{
    cout << __t.value << endl;
}

template < int N, typename Tp, typename... Args > struct Nth_type : Nth_type< N - 1, Args... >
{
};

template < typename Tp, typename... Args > struct Nth_type< 0, Tp, Args... >
{
    using type = Tp;
};

template < typename... Args > struct type_helper
{
};
template < int N, typename... Args > struct type_get
{
};
template < int N, typename... Args > struct type_get< N, type_helper< Args... > >
{
};
template < int N, typename Tp, typename... Args >
struct type_get< N, type_helper< Tp, Args... > > : type_get< N - 1, type_helper< Args... > >
{
};

template < typename Tp, typename... Args > struct type_get< 0, type_helper< Tp, Args... > >
{
    using type = Tp;
};

struct __x_undefined;
template < int N, typename Tp > struct type_get< N, type_helper< Tp > >
{
    using type = __x_undefined;
};

struct __x_undefined;
template < typename Tp > struct type_get< 0, type_helper< Tp > >
{
    using type = Tp;
};

int main()
{
    int        ap           = 99;
    die_handle __die_handle = { .handle = 100 };
    Type_check< int, char >::test();
    Type_check_impl< 0, int, char, std::string, float, int >::recursive_convertible< int >();
    get_type_check< int, char, std::string, float, int >().recursive_convertible< char >();
    Type_check_interface< int, char, std::string, float, int >::recursive_convertible< std::string >();
    check_ret_call( ( get_params_check_dispatch< false >().operator()< int, int*, const char*, die_handle*, int >(
        5, nullptr, "hello", &__die_handle, 99 ) ) );
    check_ret_call( ( get_params_check_dispatch().operator()< int, int*, const char*, die_handle*, int* >(
        5, nullptr, "hello", &__die_handle, &ap ) ) );
    is_convertible_x< int, char, int >();
    __test_type( ( s_test_type ){ .value = 19 } );

    Type_check_interface< int, char, std::string, float, int > __check;
    check_convertible_by_index< 0, int >( __check );
    check_convertible_by_index< 1, std::string >( __check );
    check_convertible_by_index< 2, char >( __check );
    check_convertible_by_index< 3, int >( __check );
    check_convertible_by_index< 4, double >( __check );
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
}