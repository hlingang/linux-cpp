#ifndef _SINGLE_H_
#define _SINGLE_H_
#include <stddef.h>

template <typename T>
class Singleton
{
public:
    Singleton() {}
    virtual ~Singleton()
    {
        if (m_Instance)
        {
            delete m_Instance;
            m_Instance = NULL;
        }
    }
    static T *getInstance()
    {
        if (m_Instance == NULL)
        {
            m_Instance = new T();
        }
        return m_Instance;
    }

private:
    static T *m_Instance;
};
template <typename T>
T *Singleton<T>::m_Instance = NULL;

#endif
