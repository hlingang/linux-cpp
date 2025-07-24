#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <cstring>
#include <condition_variable>
#include <mutex>
#include <chrono>

using namespace std;

struct thread_info_t
{
    std::thread  t;
    char         name[ 128 ];
    unsigned int flag;
    thread_info_t() : t(), flag( 0 )
    {
        memset( name, 0x00, sizeof( name ) );
    }
    /*
    右值引用只能绑定到临时变量
    右值引用只存在于 形参<->实参 的初始化过程中
    函数体里面的右值引用都会退化成左值[具名参数]
    */
    thread_info_t( std::thread&& __t ) : t( std::move( __t ) ), flag( 0 )
    {
        memset( name, 0x00, sizeof( name ) );
    }
    thread_info_t( const thread_info_t& ) = delete;
    thread_info_t( thread_info_t&& oth ) : t( std::move( oth.t ) ), flag( oth.flag )
    {
        memcpy( name, oth.name, sizeof( name ) );
    }
};

struct spin_lock_t
{
public:
    void lock() noexcept
    {
        // 循环直到成功将标志位设置为 true
        while ( flag.test_and_set( std::memory_order_acquire ) )
        {
// 降低 CPU 使用率的优化：提示处理器当前处于自旋等待
#if defined( __x86_64__ ) || defined( __i386__ )
            __builtin_ia32_pause();  // x86 架构的 PAUSE 指令
#elif defined( __arm__ ) || defined( __aarch64__ )
            __asm__ __volatile__( "yield" );  // ARM 架构的 YIELD 指令
#endif
        }
    }

    void unlock() noexcept
    {
        // 清除标志位，释放锁
        flag.clear( std::memory_order_release );
    }

    bool try_lock() noexcept
    {
        // 尝试一次获取锁，成功返回 true，失败返回 false
        return !flag.test_and_set( std::memory_order_acquire );
    }

private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;  // 保证无锁且原子操作
};

struct thread_info_list_t
{
    thread_info_t* info_list;
    spin_lock_t    spinlock;
};

condition_variable g_cond;
condition_variable g_end;
mutex              g_mutex;
atomic_bool        g_flag;
atomic_bool        g_run;
thread_info_list_t thread_list;

void thread_fn( int i )
{
    while ( g_run.load() )
    {
        do
        {
            thread_list.info_list[ i ].flag = 1;
            std::unique_lock< mutex > lock( g_mutex );
            cout << "thread " << i << " wait...[" << chrono::system_clock::now().time_since_epoch().count() << "]"
                 << endl;
            g_cond.wait( lock, []() { return g_flag.load() || !g_run.load(); } );
            if ( !g_run.load() )
                goto out;
        } while ( 0 );
        cout << "thread " << i << " work...[" << chrono::system_clock::now().time_since_epoch().count() << "]" << endl;
        sleep( 1 );
        thread_list.spinlock.lock();
        thread_list.info_list[ i ].flag = 0;
        for ( int i = 0; i < 5; i++ )
        {
            if ( thread_list.info_list[ i ].flag )
                goto still_busy;
        }
        thread_list.spinlock.unlock();
        cout << "End-thread: " << i << ", " << thread_list.info_list[ i ].name << endl;
        g_flag.store( false );
        g_cond.notify_one();
        continue;
    still_busy:
        thread_list.spinlock.unlock();
        std::unique_lock< mutex > lock( g_mutex );
        g_end.wait( lock );
    }
out:
    g_cond.notify_all();
}

//---------------------------------------------THE-END-------------------------------------------------------//

int main()
{
    g_flag.store( false );
    g_run.store( true );
    thread_list.info_list = new thread_info_t[ 8 ];
    for ( int i = 0; i < 5; i++ )
    {
        new ( &thread_list.info_list[ i ] ) thread_info_t( std::thread( thread_fn, i ) );
        thread_list.info_list[ i ].flag = 1;
        string __name                   = "Thread-" + to_string( i );
        memcpy( thread_list.info_list[ i ].name, __name.c_str(), __name.size() );
    }
    for ( int i = 0; i < 10; i++ )
    {
        g_flag.store( true );
        g_cond.notify_all();
        do
        {
            std::unique_lock< std::mutex > lock( g_mutex );
            g_cond.wait( lock );
        } while ( 0 );
        g_end.notify_all();
        sleep( 2 );
        cout << "-------------------" << "LOOP " << i << "-------------------" << endl;
    }
    g_run.store( false );
    cout << "----------------- STOP!!! ------------------" << endl;
    g_cond.notify_all();
    g_end.notify_all();
    for ( int i = 0; i < 5; i++ )
    {
        thread_list.info_list[ i ].t.join();
    }
    return 0;
}