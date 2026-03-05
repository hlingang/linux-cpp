#ifndef __KERNEL_BASIC_API_H__
#define __KERNEL_BASIC_API_H__

using namespace std;

namespace ns_kapi
{

int isupper( const char c )
{
    return ( c >= 'A' && c <= 'Z' );
}
int islower( const char c )
{
    return ( c >= 'a' && c <= 'z' );
}

int isspace( const char c )
{
    return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v' );
}

char toupper( const char c )
{
    if ( islower( c ) )
        return c & ( ( ~0x20 ) & 0xff );
    return c;
}
char tolower( const char c )
{
    if ( isupper( c ) )
        return c | ( 0x20 & 0xff );
    return c;
}

int strcmp( const char* s1, const char* s2 )
{
    while ( *s1 && *s1 == *s2 )
        ++s1, ++s2;
    return int( *s1 ) - int( *s2 );  // 中间返回和最后返回值一致
}

int strcmp_2( const char* s1, const char* s2 )
{
    while ( *s1 && *s2 )
    {
        if ( *s1 != *s2 )
            break;  // 中间返回和最后返回值一致
        ++s1, ++s2;
    }
    return int( *s1 ) - int( *s2 );
}

int strnicmp( const char* s1, const char* s2, unsigned maxlen )
{
    int c1, c2;
    while ( *s1 && maxlen-- )
    {
        c1 = toupper( *s1 );
        c2 = toupper( *s2 );
        if ( c1 != c2 )
            break;
        s1++;
        s2++;
    }
    return c1 - c2;
}

int strcasecmp( const char* s1, const char* s2 )
{
    int c1, c2;
    while ( *s1 && ( c1 = toupper( *s1 ) ) == ( c2 = toupper( *s2 ) ) )
        s1++, s2++;
    return c1 - c2;
}

unsigned long strlen( const char* s )
{
    const char* sc;
    for ( sc = s; *sc != '\0'; ++sc )
        ;
    return sc - s;
}
unsigned long strnlen( const char* s, unsigned maxlen )
{
    const char* sc;
    for ( sc = s; *sc != '\0' && maxlen--; ++sc )
        ;
    return sc - s;
}

char* strchr( const char* s, char c )
{
    for ( ; *s != '\0'; s++ )
    {
        if ( *s == c )
            return ( char* )s;  // 不同情况下返回不同值
    }
    return nullptr;  // 不同情况下返回不同值
}

char* strnchr( const char* s, char c, size_t maxlen )
{
    for ( ; *s != '\0' && maxlen; s++, maxlen-- )
    {
        if ( *s == c )
            return ( char* )s;
    }
    return nullptr;
}

char* strstrip( char* s )
{
    char* end = s + strlen( s );
    end--;
    while ( end >= s && ( isspace( *end ) ) )
        --end;
    *( end + 1 ) = '\0';
    while ( s <= end && ( isspace( *s ) ) )
        s++;
    return s;
}

void* memcpy( void* dest, void* src, size_t n )
{
    if ( !n )
        return dest;
    char* __dest = ( char* )dest;
    char* __src  = ( char* )src;
    do
    {
        *__dest++ = *__src++;
    } while ( --n );
    return dest;
}
void* memset( void* s, int c, size_t n )
{
    if ( !n )
        return s;
    char* __s = ( char* )s;
    do
    {
        *__s++ = ( char )c;
    } while ( --n );
    return s;
}
void* memmove( void* dest, void* src, size_t n )
{
    if ( !n )
        return dest;
    char* __dest = ( char* )dest;
    char* __src  = ( char* )src;
    if ( __dest > __src )
    {
        char* __last_s = __src + ( n - 1 );
        char* __last_d = __dest + ( n - 1 );
        do
        {
            *__last_d-- = *__last_s--;
        } while ( --n );
    }
    else
    {
        do
        {
            *__dest++ = *__src++;
        } while ( --n );
    }
    return dest;
}

int memcmp( void* s1, void* s2, size_t n )
{
    char* __s1 = ( char* )s1;
    char* __s2 = ( char* )s2;
    while ( n && *__s1 == *__s2 )
        __s1++, __s2++, n--;
    return n ? int( *__s1 ) - int( *__s2 ) : 0;
}

int memcmp_2( void* s1, void* s2, size_t n )
{
    char* __s1 = ( char* )s1;
    char* __s2 = ( char* )s2;
    while ( n )
    {
        if ( *__s1 != *__s2 )
            return int( *__s1 ) - int( *__s2 );
        __s1++, __s2++, n--;
    }
    return 0;
}

int memcmp_3( void* s1, void* s2, size_t n )
{
    char* __s1 = ( char* )s1;
    char* __s2 = ( char* )s2;
    while ( n-- && *__s1 == *__s2 )  // 条件判断中的[后]自增和[后]自减操作会带来副作用
        __s1++, __s2++;
    return ++n ? int( *__s1 ) - int( *__s2 ) : 0;
}

char* strstr( const char* s1, const char* s2 )
{
    if ( !*s1 || !*s2 || strlen( s2 ) > strlen( s1 ) )
        return nullptr;
    const char* __s1  = s1;
    const char* __s2  = s2;
    const char* __end = __s1 + strlen( __s1 ) - strlen( __s2 );
    while ( __s1 <= __end && memcmp( ( void* )__s1, ( void* )__s2, strlen( __s2 ) ) )
        ++__s1;
    return __s1 <= __end ? ( char* )__s1 : nullptr;
}

char* strstr_2( const char* s1, const char* s2 )
{
    if ( !*s1 || !*s2 || strlen( s2 ) > strlen( s1 ) )
        return nullptr;
    const char* __s1  = s1;
    const char* __s2  = s2;
    const char* __end = __s1 + strlen( __s1 ) - strlen( __s2 );
    while ( __s1 <= __end && memcmp( ( void* )__s1, ( void* )__s2, strlen( __s2 ) ) )
    {
        if ( !memcmp( ( void* )__s1, ( void* )__s2, strlen( __s2 ) ) )
            return ( char* )__s1;
        ++__s1;
    }
    return nullptr;
}

void* memchr( void* s1, const char c, size_t n )
{
    char* __s1 = ( char* )s1;
    while ( n && *__s1 != c )
        __s1++, n--;
    return n ? ( char* )__s1 : nullptr;
}

void* memchr_2( void* s1, const char c, size_t n )
{
    char* __s1 = ( char* )s1;
    while ( n )
    {
        if ( *__s1 == c )
            return ( char* )__s1;
        __s1++, n--;
    }
    return nullptr;
}

/**
 * strpbrk - Find the first occurrence of a set of characters
 * @cs: The string to be searched
 * @ct: The characters to search for
 */
char* strpbrk( const char* s1, const char* s2 )
{
    const char* __s1 = s1;
    while ( *__s1 )
    {
        const char* __s2 = s2;
        while ( *__s2 )
        {
            if ( *__s1 == *__s2 )
                return ( char* )__s1;
            ++__s2;
        }
        __s1++;
    }
    return nullptr;
}
char* strcat( char* s1, char* s2 )
{
    char* __s1 = ( char* )s1;
    while ( *__s1 )
        __s1++;
    const char* __s2 = s2;
    while ( *__s2 )
        *__s1++ = *__s2++;
    *__s1 = '\0';
    return ( char* )s1;
}
}  // namespace ns_kapi

#endif  // __KERNEL_BASIC_API_H__