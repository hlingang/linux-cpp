#ifndef _SINGLE_H_
#define _SINGLE_H_
#include <mutex>

template < typename T > class Singleton
{
public:
    Singleton() {}
    virtual ~Singleton()
    {
        if ( m_pInstance )
        {
            std::lock_guard< std::mutex > lock( m_mtx );
            if ( m_pInstance )
            {
                delete m_pInstance;
                m_pInstance = nullptr;
            }
        }
    }
    static T* getInstance()
    {
        if ( m_pInstance == NULL )
        {
            std::lock_guard< std::mutex > lock( m_mtx );
            if ( m_pInstance == nullptr )
            {
                m_pInstance = new T();
            }
        }
        return m_pInstance;
    }

private:
    static std::mutex m_mtx;
    static T*         m_pInstance;
};
template < typename T > T*         Singleton< T >::m_pInstance = NULL;
template < typename T > std::mutex Singleton< T >::m_mtx;

#endif
