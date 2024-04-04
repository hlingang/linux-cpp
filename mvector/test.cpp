#include "mvector.h"
#include <unistd.h>

int main()
{
    mvector< int > mvec;
    for ( int i = 0; i < 1000; i++ )
    {
        mvec.append( i );
        mvec.print();
        usleep( 1000 );
    }
    mvector< int > am;
    am.emplace_back( 10 );
    am.emplace_back( 11 );
    am.emplace_back( 12 );
    am.emplace_back( 13 );
    am.emplace_back( 14 );
    am.emplace_back( 15 );
    am.to_string();
    am.remove( 2 );
    am.to_string();
    size_t id = 0;
    for ( auto& val : am )
    {
        cout << "[" << id++ << "]"
             << " val: " << val << endl;
    }
    return 0;
}