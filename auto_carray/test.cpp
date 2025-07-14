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
        signed char sec;       // seconds(signed)
        signed char min;       // minutes(signed)
        signed char hour;      // hours(signed)
        signed char reserved;  // reserved for future use
    } ts;
    Clock( char __hour, char __min, char __sec ) : ts( { __sec, __min, __hour, 0 } ) {}
    Clock() : total( 0 ) {}
    void print( const char* __name = 0 ) const
    {
        if ( __name )
        {
            printf( "%-10s Clock: %02d:%02d:%02d [%-8d][%08b][%08b][%08b]\n", __name, ts.hour, ts.min, ts.sec, total,
                    ts.hour, ts.min, ts.sec );
        }
        else
        {
            printf( "Clock: %02d:%02d:%02d [%-8d][%08b][%08b][%08b]\n", ts.hour, ts.min, ts.sec, total, ts.hour, ts.min,
                    ts.sec );
        }
    }
    Clock operator+( const Clock& other ) const
    {
        Clock result;
        result.total = this->total + other.total;

        // Handle overflow for seconds and minutes
        if ( result.ts.sec >= NR_SECS_PER_MIN )
        {
            result.total += NSECS;
        }
        if ( result.ts.min >= NR_MINS_PER_HOUR )
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
            result.ts.sec += NR_SECS_PER_MIN;
        }
        if ( result.ts.min < 0 )
        {
            result.ts.min += NR_MINS_PER_HOUR;
        }
        return result;
    }
    bool operator<( const Clock& other ) const
    {
        return this->total < other.total;
    }
    bool operator==( const Clock& other ) const
    {
        return this->total == other.total;
    }
};

int main()
{
    int loop = 0;
    for ( ;; )
    {
        printf( "============ Loop %d ==============\n", loop++ );
        Clock clock1( random() % 24, random() % 60, random() % 60 );
        Clock clock2( random() % 24, random() % 60, random() % 60 );
        Clock result1 = clock1 + clock2;
        Clock __max   = std::max< Clock >( clock1, clock2 );
        Clock __min   = std::min< Clock >( clock1, clock2 );
        Clock result2 = __max - __min;
        clock1.print( "Clock1" );
        clock2.print( "Clock2" );
        __max.print( "Clock_max" );
        __min.print( "Clock_min" );
        result1.print( "Result(+)" );
        result2.print( "Result(-)" );
        sleep( 2 );
    }
    return 0;
}