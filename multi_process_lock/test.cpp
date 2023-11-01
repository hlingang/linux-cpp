#include "multi_process_lock.h"

int main()
{
    if ( 1 )
    {
        MultiLock lock;
        printf( "lock pid=%d\n", ( int )getpid() );
        sleep( 10 );
    }
    printf( "unlock\n" );
    sleep( 5 );
}