#include "log_manager.h"

void testA()
{
    LOG_OUT( e_Trace, "A - trace" );
    LOG_OUT( e_Info, "A - info" );
    LOG_OUT( e_Error, "A - error" );
}

void testB()
{
    LOG_OUT( e_Trace, "B - trace" );
    LOG_OUT( e_Info, "B - info" );
    LOG_OUT( e_Error, "B - error" );
}

int main( int argc, char* argv[] )
{
    if ( argc > 1 )
    {
        int log_level = 0;
        try
        {
            log_level = stoi( argv[ 1 ] );
        }
        catch ( const exception& e )
        {
            cout << "[error]: " << e.what() << endl;
            throw runtime_error( "parse loglevel fail" );
        }
        SET_LOG_LEVEL( log_level );
        for ( int i = 2; i < argc; i++ )
        {
            ADD_FUNC( string( argv[ i ] ) );
        }
    }
    else
    {
        SET_LOG_LEVEL_BIT( e_All );
    }
    testA();
    testB();
}
