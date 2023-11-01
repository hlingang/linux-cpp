#include <unistd.h>
#include <semaphore.h>
#include <string>
#include <fcntl.h>
#include <stdexcept>

#define DEFAULT_NAME "multi_process_sem_lock"

using namespace std;

class MultiLock
{
public:
    MultiLock() : MultiLock( DEFAULT_NAME ) {}
    MultiLock( const char* name ) : m_psem( nullptr )
    {
        if ( name == nullptr )
        {
            throw runtime_error( "argument name is null" );
        }
        m_name = name;
        m_psem = sem_open( m_name.c_str(), O_RDWR | O_CREAT, 0664, 1 );
        if ( m_psem == SEM_FAILED )
        {
            perror( "sem_open fail" );
            m_psem = nullptr;
            throw runtime_error( "sem open fail" );
        }
        if ( m_psem )
        {
            sem_wait( m_psem );
        }
    }
    ~MultiLock()
    {
        if ( m_psem )
        {
            sem_post( m_psem );
            sem_close( m_psem );
            sem_unlink( m_name.c_str() );
        }
    }

private:
    sem_t* m_psem;
    string m_name;
};