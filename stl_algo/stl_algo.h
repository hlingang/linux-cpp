#ifndef __STL_ALGO_H_
#define __STL_ALGO_H_
#include <ctime>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <iostream>
#include "ops.h"
using namespace std;

template < typename Iter > Iter __search( Iter __first1, Iter __last1, Iter __first2, Iter __last2 )
{
    if ( __first1 == __last1 || __first2 == __last2 )
    {
        return __last1;
    }
    Iter p = __first2;
    if ( ++p == __last2 )
    {
        return __find( __first1, __last1, *__first1 );
    }
    for ( ;; )
    {
        __first1 = __find( __first1, __last1, *__first2 );
        if ( __first1 == __last1 )
        {
            return __last1;
        }
        Iter current = __first1;
        Iter p       = __first2;
        while ( current != __last1 && p != __last2 && *current == *p )
        {
            ++current;
            ++p;
        }
        if ( p == __last2 )
        {
            return __first1;
        }
        if ( current == __last1 )
        {
            return __last1;
        }
        ++__first1;  // 序列匹配
    }
}
// 序列1 和序列2 均已排序
template < typename Iter > Iter __include( Iter __first1, Iter __last1, Iter __first2, Iter __last2 )
{
    if ( __first1 == __last1 || __first2 == __last2 )
    {
        return __last1;
    }
    for ( ; __first1 != __last1 && __first2 != __last2; )
    {
        if ( *__first1 > *__first2 )
        {
            return false;
        }
        if ( *__first1 < *__first2 )
        {
            ++__first1;
        }
        ++__first1;
        ++__first2;
    }
    return static_cast< bool >( __first2 == __last2 );
}

template < typename Iter, typename T > Iter __find( Iter __first1, Iter __last1, T __val )
{
    while ( __first1 != __last1 && *__first1 != __val )
    {
        ++__first1;
    }
    return __first1;
};

template < typename Iter, typename T > Iter __find_not( Iter __first1, Iter __last1, T __val )
{
    while ( __first1 != __last1 && *__first1 == __val )
    {
        ++__first1;
    }
    return __first1;
};

template < typename Iter, typename T > Iter __find_n_aux( Iter __first1, Iter __last1, int n, T __val )
{

    if ( __first1 == __last1 )
    {
        return __last1;
    }
    for ( ;; )
    {
        __first1 = __find( __first1, __last1, __val );
        if ( __first1 == __last1 )
        {
            return __last1;
        }
        Iter p = __first1;
        for ( ; p != __last1 && *p == __val && n; )
        {
            ++p;
            --n;
        }
        if ( n == 0 )
        {
            return __first1;
        }
        if ( p == __last1 )
        {
            return __last1;
        }
        __first1 = p;
    }
    return __first1;
};

template < typename Iter, typename T > Iter __remove( Iter __first1, Iter __last1, T __val )
{
    if ( __first1 == __last1 )
    {
        return __last1;
    }
    __first1 = __find( __first1, __last1, __val );
    if ( __first1 == __last1 )
    {
        return __last1;
    }
    Iter p = __first1;
    while ( ++p != __last1 )
    {
        if ( *p != __val )
        {
            *__first1 = *p;
            ++__first1;
        }
    }
};

template < typename Iter > Iter __shuffle( Iter __first1, Iter __last1 )
{
    Iter p = __first1;
    if ( p == __last1 || ++p == __last1 )
    {
        return __last1;
    }
    int __i = 1;
    srand( time( nullptr ) );  // 随机化种子
    for ( ; p != __last1; ++p, ++__i )
    {
        int __j = rand() % ( __i + 1 );
        if ( __i != __j )
        {
            Iter __s = __first1 + __j;
            iter_swap( __s, p );
        }
    }
    return __first1;
};

template < typename Iter > bool equal( Iter __first1, Iter __last1, Iter __first2 )
{
    for ( ; __first1 != __last1 && *__first1 == *__first2; )
    {
        ++__first1;
        ++__first2;
    }
    return static_cast< bool >( __first1 == __last1 );
};

template < typename Iter > bool equal( Iter __first1, Iter __last1, Iter __first2, Iter __last2 )
{
    for ( ; __first1 != __last1 && __first2 != __last2 && *__first1 == *__first2; )
    {
        ++__first1;
        ++__first2;
    }
    return static_cast< bool >( __first1 == __last1 && __first2 == __last2 );
};

template < typename Iter > Iter find_adjacent( Iter __first1, Iter __last1 )
{
    Iter p = __first1;
    // 解引用前必须检查
    if ( p == __last1 || ++p == __last1 )
    {
        return __last1;
    }
    for ( ; p != __last1; ++p, ++__first1 )
    {
        if ( *p == *__first1 )
        {
            return __first1;  // 返回首位置
        }
    }
    return __last1;
};

template < typename Iter > Iter __unique( Iter __first1, Iter __last1 )
{

    __first1 = find_adjacent( __first1, __last1 );
    if ( __first1 == __last1 )
    {
        return __last1;
    }
    Iter p = __first1;
    ++p;
    if ( p == __last1 || ++p == __last1 )
    {
        return __last1;
    }
    for ( ; p != __last1; ++p )
    {
        if ( *p != *__first1 )
        {
            *++__first1 = *p;
        }
    }
    return ++__first1;  // 返回结尾的后一个指针
};

template < typename Iter, typename _Function > void __for_each( Iter __first1, Iter __last1, _Function __f )
{
    if ( __first1 == __last1 )
    {
        return;
    }
    for ( ; __first1 != __last1; ++__first1 )
    {
        __f( *__first1 );
    }
};
#endif