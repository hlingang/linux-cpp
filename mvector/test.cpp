#include "mvector.h"

int main()
{
    mvector< int > mvec( 10 );
    for ( int i = 0; i < 100; i++ )
    {
        mvec.append( i );
        mvec.print();
    }
    return 0;
}