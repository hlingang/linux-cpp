
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <numeric>
using namespace std;

enum _S_status1
{
    _S_ok1 = 1,
    _S_err1,
};

enum _S_status2
{
    _S_ok2 = 2,
    _S_err2,
};

enum _S_status3
{
    _S_ok3 = 3,
    _S_err3,
};
enum _S_status4
{
    _S_ok4 = 4,
    _S_err4,
};

template < typename T > bool validate( T t )
{
    if constexpr ( is_same< T, _S_status1 >::value )
    {
        return t <= _S_ok1;
    }
    else if constexpr ( is_same< T, _S_status2 >::value )
    {
        return t <= _S_ok2;
    }
    else if constexpr ( is_same< T, _S_status3 >::value )
    {
        return t <= _S_ok3;
    }
    else if constexpr ( is_same< T, _S_status4 >::value )
    {
        return t <= _S_ok4;
    }
    else
    {
        return false;
    }
}

template < typename Tp > struct _S_base
{
    _S_base( Tp value ) : _M_value( value ) {}
    bool validate()
    {
        return false;
    }
    Tp _M_value;
};

template < typename Tp > struct _S_validate : public _S_base< Tp >
{
    _S_validate( Tp value ) : _S_base< Tp >( value ) {}
};

template <> struct _S_validate< _S_status1 > : public _S_base< _S_status1 >
{
    _S_validate( _S_status1 value ) : _S_base< _S_status1 >( value ) {}
    bool validate()
    {
        return this->_M_value <= _S_ok1;
    }
};

template <> struct _S_validate< _S_status2 > : public _S_base< _S_status2 >
{
    _S_validate( _S_status2 value ) : _S_base< _S_status2 >( value ) {}
    bool validate()
    {
        return this->_M_value <= _S_ok2;
    }
};

template <> struct _S_validate< _S_status3 > : public _S_base< _S_status3 >
{
    _S_validate( _S_status3 value ) : _S_base< _S_status3 >( value ) {}
    bool validate()
    {
        return this->_M_value <= _S_ok3;
    }
};

template <> struct _S_validate< _S_status4 > : public _S_base< _S_status4 >
{
    _S_validate( _S_status4 value ) : _S_base< _S_status4 >( value ) {}
    bool validate()
    {
        return this->_M_value <= _S_ok4;
    }
};

int main()
{
    cout << "validate _S_status1: " << validate( _S_ok1 ) << endl;
    cout << "validate _S_status2: " << validate( _S_err2 ) << endl;
    cout << "validate _S_status3: " << validate( _S_err3 ) << endl;
    cout << "validate _S_status4: " << validate( _S_ok4 ) << endl;
    cout << "---------------I am split line--------------" << endl;
    cout << "validate _S_status1: " << _S_validate< _S_status1 >( _S_ok1 ).validate() << endl;
    cout << "validate _S_status2: " << _S_validate< _S_status2 >( _S_err2 ).validate() << endl;
    cout << "validate _S_status3: " << _S_validate< _S_status3 >( _S_err3 ).validate() << endl;
    cout << "validate _S_status4: " << _S_validate< _S_status4 >( _S_ok4 ).validate() << endl;
    return 0;
}
