#include "mvector.h"

int main()
{
    mvector< int > mvec( 10 );
    for ( int i = 0; i < 1000; i++ )
    {
        mvec.append( i );
        mvec.print();
    }
    mvector< int > am;
    am.emplace_back( 10 );
    am.emplace_back( 11 );
    am.emplace_back( 12 );
    cout << am.front() << endl;
    cout << am.back() << endl;
    return 0;
}