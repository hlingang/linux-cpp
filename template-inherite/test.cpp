
#include <iostream>
#include <string.h>
using namespace std;

struct Test
{
    int init()
    {
        return 0;
    }  // 👈 加这个
};

template < typename... Args > struct _my_void
{
    using type = void;
};
template < typename... Args > struct _my_int
{
    using type = int;
};

template < typename T, typename > struct __S
{
    void func()
    {
        cout << "func() default" << endl;
    }
};

template < typename T > struct __S< T, typename _my_int< decltype( declval< T >().init() ) >::type >
{
    void func()
    {
        cout << "func() in __S<T, int>" << endl;
    }
};  // 👍

template < typename... Args > struct __S_common_format
{
    const char* operator()( const char* __format, Args... __args )
    {
        memset( _m_buf, 0, sizeof( _m_buf ) );
        snprintf( _m_buf, sizeof( _m_buf ), __format, std::forward< Args >( __args )... );
        return _m_buf;
    }
    const char* operator()()
    {
        return _m_buf;
    }
    const char* format( const char* __format, Args... __args )
    {
        return this->operator()( __format, std::forward< Args >( __args )... );
    }
    char _m_buf[ 4096 ];
};

template <> struct __S_common_format<>
{
    const char* operator()( const char* __format )
    {
        return ( _m_buf = __format );
    }
    const char* format( const char* __format )
    {
        return this->operator()( __format );
    }
    const char* operator()()
    {
        return _m_buf;
    }

private:
    const char* _m_buf;
};

template < typename... Args > void __printf( const char* __format, Args... __args )
{
    auto __fmt = __S_common_format< Args... >();  // 先创建对象
    printf( "%s", __fmt( __format, std::forward< Args >( __args )... ) );
}

int main()
{

    __S< Test, int >().func();
    __printf( "jkfheiuwoiuio\n" );
    __printf( "jkfheiuwoiuio %d\n", 123 );
}