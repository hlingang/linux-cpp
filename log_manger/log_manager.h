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

using namespace std;

enum e_LogLevel
{
    e_Trace,
    e_Info,
    e_Error,
    e_All,
};

class LogManager
{
private:
    static mutex       m_mtx;
    static LogManager* pInstance;
    set< string >      m_funcSet;
    bitset< 8 >        m_logLevel;
    LogManager()
    {
        m_logLevel = 0;
        m_funcSet.clear();
    }
    ~LogManager()
    {
        lock_guard< mutex > lock( m_mtx );
        if ( pInstance )
        {
            delete pInstance;
            pInstance = nullptr;
        }
    }

public:
    static LogManager* GetInstance()
    {
        if ( pInstance == nullptr )
        {
            lock_guard< mutex > lock( m_mtx );
            if ( pInstance == nullptr )
            {
                pInstance = new LogManager();
            }
        }
        return pInstance;
    }
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
LogManager* LogManager::pInstance;
mutex       LogManager::m_mtx;

#define LOG_OUT( LOG_LEVEL, FORMAT, ... )                                                                              \
    {                                                                                                                  \
        if ( LogManager::GetInstance()->test( ( e_LogLevel )LOG_LEVEL )                                                \
             && ( LogManager::GetInstance()->empty() || LogManager::GetInstance()->count( string( __FUNCTION__ ) ) ) ) \
        {                                                                                                              \
            printf( "[%s:%s:%d] " FORMAT "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );                      \
        }                                                                                                              \
    }

#define SET_LOG_LEVEL( LOG_LEVEL )                           \
    {                                                        \
        LogManager::GetInstance()->setLogLevel( LOG_LEVEL ); \
    }

#define SET_LOG_LEVEL_BIT( LOG_LEVEL )                          \
    {                                                           \
        LogManager::GetInstance()->setLogLevelBit( LOG_LEVEL ); \
    }
#define ADD_FUNC( FUNC )                            \
    {                                               \
        LogManager::GetInstance()->addFunc( FUNC ); \
    }

#endif
