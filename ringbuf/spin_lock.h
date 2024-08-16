#ifndef __SPIN_LOCK_H_
#define __SPIN_LOCK_H_

#include <atomic>
#include <mutex>

class spin_lock
{
public:
    explicit spin_lock()                     = default;
    spin_lock( const spin_lock& )            = delete;
    spin_lock( spin_lock&& )                 = delete;
    spin_lock& operator=( const spin_lock& ) = delete;
    void       lock()
    {
        // return flag status(until flag = false)
        while ( __M_flag.test_and_set( std::memory_order_acquire ) )
        {
        }
    }
    void unlock()
    {
        return __M_flag.clear( std::memory_order_release );
    }

private:
    std::atomic_flag __M_flag = ATOMIC_FLAG_INIT;
};
#endif