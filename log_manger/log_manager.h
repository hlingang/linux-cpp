#ifndef __LOG_MANAGER_H_
#define __LOG_MANAGER_H_
#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <set>
#include <mutex>
#include <bitset>
#include <algorithm>
#include "singleton.h"

using namespace std;

#define LOG_LEVEL_BIT ( 8 )

enum e_LogLevel
{
    e_Trace,
    e_Info,
    e_Error,
    e_All,
};

class LogManager final : public Singleton< LogManager >
{
private:
    set< string >           m_funcSet;
    bitset< LOG_LEVEL_BIT > m_logLevel;

public:
    LogManager()
    {
        m_logLevel = 0;
        m_funcSet.clear();
    }
    ~LogManager() {}

public:
    bool empty()
    {
        return m_funcSet.empty();
    }
    bool test( e_LogLevel _logLevel )
    {
        return m_logLevel.test( _logLevel );
    }
    size_t count( const string& func )
    {
        string __func = func.c_str();
        transform( __func.begin(), __func.end(), __func.begin(), []( char c ) { return tolower( c ); } );
        return m_funcSet.count( __func );
    }
    unsigned long setLogLevelBit( e_LogLevel _logLevel )
    {
        if ( _logLevel > e_All )
        {
            throw out_of_range( "log level out of range" );
        }
        if ( _logLevel == e_All )
        {
            m_logLevel = ( 1U << e_All ) - 1;
        }
        else
        {
            m_logLevel.set( _logLevel, true );
        }

        return m_logLevel.to_ulong();
    }
    unsigned long setLogLevel( unsigned long _log_level )
    {
        m_logLevel = _log_level;
        return m_logLevel.to_ulong();
    }
    void addFunc( const string& func )
    {
        string __func = func.c_str();
        transform( __func.begin(), __func.end(), __func.begin(), []( char c ) { return tolower( c ); } );
        m_funcSet.insert( __func );
    }
    void addFunc( const vector< string >& func )
    {
        for ( auto& _func : func )
        {
            string __func = _func.c_str();
            transform( __func.begin(), __func.end(), __func.begin(), []( char c ) { return tolower( c ); } );
            m_funcSet.insert( __func );
        }
    }
};

#define LOG_OUT( LOG_LEVEL, FORMAT, ... )                                                                              \
    {                                                                                                                  \
        if ( LogManager::getInstance()->test( ( e_LogLevel )LOG_LEVEL )                                                \
             && ( LogManager::getInstance()->empty() || LogManager::getInstance()->count( string( __FUNCTION__ ) ) ) ) \
        {                                                                                                              \
            printf( "[%s:%s:%d] " FORMAT "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );                      \
        }                                                                                                              \
    }

#define SET_LOG_LEVEL( LOG_LEVEL )                           \
    {                                                        \
        LogManager::getInstance()->setLogLevel( LOG_LEVEL ); \
    }

#define SET_LOG_LEVEL_BIT( LOG_LEVEL )                          \
    {                                                           \
        LogManager::getInstance()->setLogLevelBit( LOG_LEVEL ); \
    }
#define ADD_FUNC( FUNC )                            \
    {                                               \
        LogManager::getInstance()->addFunc( FUNC ); \
    }

#endif
