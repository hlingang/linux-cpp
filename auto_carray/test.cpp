#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <iostream>

#define NR_SEC_BITS 8
#define NR_MIN_BITS 8
#define NR_HOUR_BITS 8

#define NR_SECS_PER_MIN 60
#define NR_MINS_PER_HOUR 60

#define SEC_MASK ( ( 1U << NR_SEC_BITS ) - 1 )
#define MIN_MASK ( SEC_MASK << NR_MIN_BITS )
#define HOUR_MASK ( SEC_MASK << ( NR_HOUR_BITS + NR_SEC_BITS ) )

#define NSECS ( ( ( unsigned int )( -NR_SECS_PER_MIN ) ) & 0xff )
#define NMINS ( ( ( ( unsigned int )( -NR_MINS_PER_HOUR ) ) << NR_SEC_BITS ) & 0xff00 )

union Clock
{
    int total;
    struct
    {
        signed char sec : 8;   // seconds(signed)
        signed char min : 8;   // minutes(signed)
        signed char hour : 8;  // hours(signed)
    } ts;
    Clock( char __hour, char __min, char __sec ) : ts( { __sec, __min, __hour } ) {}
    Clock() : total( 0 ) {}
    void print()
    {
        printf( "Clock: %02d:%02d:%02d\n", ts.hour, ts.min, ts.sec );
    }
    Clock operator+( const Clock& other ) const
    {
        Clock result;
        result.total = this->total + other.total;

        // Handle overflow for seconds and minutes
        if ( result.ts.sec >= 60 )
        {
            result.total += NSECS;
        }
        if ( result.ts.min >= 60 )
        {
            result.total += NMINS;
        }
        return result;
    }
    Clock operator-( const Clock& other ) const
    {
        Clock result;
        result.total = this->total - other.total;

        // Handle overflow for seconds and minutes
        if ( result.ts.sec < 0 )
        {
            printf( "result.ts.sec = %d\n", result.ts.sec );
            result.ts.sec += NR_SECS_PER_MIN;
        }
        if ( result.ts.min < 0 )
        {
            result.ts.min += NR_MINS_PER_HOUR;
        }
        return result;
    }
};

int main()
{

    Clock clock1( 35, 31, 21 );
    Clock clock2( 30, 42, 40 );
    Clock result1 = clock1 + clock2;
    Clock result2 = clock1 - clock2;
    clock1.print();
    clock2.print();
    result1.print();
    result2.print();
    return 0;
}