#include "log_manager.h"
#include "cxxopts.hpp"

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
    int              log_level = 0;
    vector< string > func_set;
    cxxopts::Options options( "log-manager", "log-manager" );
    options.positional_help( "[optional args]" ).set_width( 80 ).set_tab_expansion();
    options.add_options(
        "", {
                { "loglevel", "set loglevel", cxxopts::value< int >( log_level )->default_value( "0xf" ) },
                { "func", "set function set", cxxopts::value< vector< string > >( func_set )->default_value( "" ) },
            } );
    options.parse_positional( { "command", "positional" } );
    auto result = options.parse( argc, argv );

    SET_LOG_LEVEL_BIT( e_All );
    if ( result.count( "loglevel" ) )
    {
        SET_LOG_LEVEL( log_level );
    }
    if ( result.count( "func" ) )
    {
        ADD_FUNC( func_set );
    }
    testA();
    testB();
}
