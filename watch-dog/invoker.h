#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>
#include <unistd.h>
#include <tuple>
#include <utility>

using namespace std;

class WatchDog
{
public:
    condition_variable         _M_cond;
    std::mutex                 _M_mutex;
    unsigned long              _M_touch_ts;
    unsigned long              _M_sched_count;
    unsigned long              _M_last_count;
    unsigned long              _M_last_sched_ts;
    bool                       running;
    static const unsigned long TIMEOUT = 5;  // seconds
    WatchDog() : _M_mutex(), _M_touch_ts( 0 ), _M_sched_count( 0 ), _M_last_count( 0 ), running( true )
    {
        thread t1( &WatchDog::work, this );
        thread t2( &WatchDog::touch, this );
        thread t3( &WatchDog::watch, this );
        t1.join();
        t2.join();
        t3.join();
    }
    void work()
    {
        while ( running )
        {
            printf( "do something...\n" );
            this_thread::sleep_for( chrono::seconds( 2 ) );
            ++_M_sched_count;
        }
    }
    void touch()
    {
        static int count = 0;
        while ( running )
        {
            std::unique_lock< std::mutex > lock( _M_mutex );
            _M_touch_ts = static_cast< unsigned long >( time( nullptr ) );
            printf( "touch dog at %lu\n", _M_touch_ts );
            do
            {
                _M_cond.wait( lock );
            } while ( ++count > 15 && running );
        }
    }
    void watch()
    {
        while ( running )
        {
            this_thread::sleep_for( chrono::seconds( 1 ) );
            do
            {
                std::unique_lock< std::mutex > lock( _M_mutex );
                _M_cond.notify_one();
            } while ( 0 );
            if ( !_M_touch_ts )
            {
                _M_touch_ts = static_cast< unsigned long >( time( nullptr ) );
                continue;
            }
            auto now = static_cast< unsigned long >( time( nullptr ) );
            // 通过判断看门狗的时间戳判断看门狗线程[进程]在一段时间内是否得到调度执行
            // 如果看门狗线程长时间得不到执行，说明其他线程长时间占据CPU资源，导致看门狗线程得不到调度，说明系统可能出现了死锁或者其他问题
            if ( now > _M_touch_ts + TIMEOUT )
            {
                std::cout << "watch dog timeout, last sched ts: " << _M_touch_ts << ", now: " << now << std::endl;
                running = false;
                _M_cond.notify_all();
                break;
            }
            check_hung_task( now );
        }
    }
    // 检查正常 [task] 是否长时间得不到调度(长时间休眠) //
    void check_hung_task( unsigned long __now )
    {
        if ( _M_last_count != _M_sched_count )
        {
            _M_last_count    = _M_sched_count;
            _M_last_sched_ts = __now;  // 此处没有记录精确时间，而是通过 count 记录颗粒化时间
            return;
        }
        printf( "check hung task now: %lu, _M_last_sched_ts: %lu\n", __now, _M_last_sched_ts );
        if ( __now > _M_last_sched_ts + TIMEOUT )
        {
            std::cout << "detected hung task, last sched count: " << _M_last_count << ", now count: " << _M_sched_count
                      << std::endl;
            running = false;
            _M_cond.notify_all();
        }
    }
};