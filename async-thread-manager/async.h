#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdarg.h>

using namespace std;

#define DEFINE_TEST_FUNC( name, flag, val, ret ) \
    bool name()                                  \
    {                                            \
        for ( const auto& t : this->_M_threads ) \
        {                                        \
            if ( t.status == e_init )            \
                continue;                        \
            if ( t.flag == val )                 \
                return ret;                      \
        }                                        \
        return !( ret );                         \
    }
#define DEFINE_WAIT_FUNC( name, func )                     \
    void name()                                            \
    {                                                      \
        std::unique_lock< std::mutex > lk( this->_M_mtx ); \
        while ( !this->func() )                            \
        {                                                  \
            lk.unlock();                                   \
            this_thread::yield();                          \
            lk.lock();                                     \
        }                                                  \
    }

namespace ns_async
{
static const int e_exit    = -2;
static const int e_init    = -1;
static const int e_running = 0;
static const int e_done    = 1;

template < typename Tp > inline std::string Stringify( const Tp& __id )
{
    stringstream ss;
    ss << __id;
    return ss.str();
}
template <> inline std::string Stringify( const std::string& __id )
{
    return __id;
}
class ParallelWork;
struct WorkThread
{
    int             status;
    int             ready;
    int             index;
    int             exit;
    std::thread::id thread_id;
    const char*     thread_name;
    char            __tname[ 32 ];
    void ( *call )( void*, void* );  // function
    void*                     args;  // args
    void*                     ret;   // return value
    ParallelWork*             pWork;
    shared_ptr< std::mutex >  mtx;
    shared_ptr< std::thread > pThread;
    unsigned long             last_sleep_ts;
    unsigned long             last_wake_ts;
    WorkThread()
        : status( e_init ), ready( 0 ), index( -1 ), exit( 0 ), thread_id(), thread_name( nullptr ), call( nullptr ),
          args( nullptr ), pWork( nullptr ), pThread( nullptr ), last_sleep_ts( 0 ), last_wake_ts( 0 )
    {
        memset( __tname, 0x00, sizeof( __tname ) );
    }
    WorkThread( const WorkThread& t )            = default;
    WorkThread& operator=( const WorkThread& t ) = default;
    WorkThread( WorkThread&& t ) : WorkThread( t ) {};
};
class ParallelWork
{
public:
    int                       _M_status;
    std::mutex                _M_mtx;
    size_t                    _M_sz;
    std::vector< WorkThread > _M_threads;
    condition_variable        _M_start_cv;  // start
    condition_variable        _M_done_cv;   // end
    unsigned long             _M_start_ts;
    unsigned long             _M_done_ts;
    ParallelWork( size_t sz = 32 ) : _M_status( e_init ), _M_sz( sz ), _M_start_ts( 0 ), _M_done_ts( 0 )
    {
        _M_threads.resize( sz );
        Init();
    }

    void __attribute__( ( nonnull( 6 ) ) ) SetUp( int id, void* func, void* args, void* ret,
                                                  const char* format = nullptr, ... )
    {
        _M_threads[ id ].call   = ( void ( * )( void*, void* ) )func;
        _M_threads[ id ].args   = args;
        _M_threads[ id ].ret    = ret;
        _M_threads[ id ].status = e_running;
        if ( _M_threads[ id ].pThread )
            return;
        auto __pthread = __CreateThread( &_M_threads[ id ] );
        __pthread->detach();
        _M_threads[ id ].pThread = __pthread;
        if ( !format )
            return;
        char    __buf[ 1024 ] = { 0 };  // max temp buffer
        va_list va_args;
        va_start( va_args, format );
        vsnprintf( __buf, sizeof( __buf ), format, va_args );
        va_end( va_args );
        auto len = strlen( __buf );
        if ( len <= sizeof( _M_threads[ id ].__tname ) - 1 )
        {
            memcpy( __buf, _M_threads[ id ].__tname, len );
            _M_threads[ id ].thread_name = _M_threads[ id ].__tname;
            return;
        }
        auto* __ptr = ::operator new( len + 1 );
        memset( __ptr, 0x00, len + 1 );
        memcpy( __ptr, __buf, len );
        _M_threads[ id ].thread_name = ( char* )__ptr;
    }
    void Init()
    {
        for ( int i = 0; i < _M_sz; ++i )
        {
            __InitEntry( i, nullptr, nullptr, nullptr );
        }
    }
    // 线程启动入参为当前线程的直接管理对象
    shared_ptr< std::thread > __CreateThread( WorkThread* wkthread )
    {
        auto __pthread = make_shared< std::thread >( [ wkthread ]() -> void {
            wkthread->thread_id = this_thread::get_id();
            for ( ;; )
            {
                do
                {
                    std::unique_lock< std::mutex > lk( wkthread->pWork->_M_mtx );
                    wkthread->status = e_running;
                    wkthread->ready  = 1;  // start 前必须保证所有的线程 ready
                    // printf( "Thread[%s] Ready\n", Stringify( this_thread::get_id() ).c_str() );
                    wkthread->last_sleep_ts = chrono::system_clock::now().time_since_epoch().count();
                    wkthread->pWork->_M_start_cv.wait( lk, [ wkthread ] {
                        return ( wkthread->pWork->_M_status == e_exit || wkthread->pWork->_M_status == e_running );
                    } );
                    wkthread->last_wake_ts = chrono::system_clock::now().time_since_epoch().count();
                } while ( 0 );
                if ( wkthread->pWork->_M_status == e_exit )
                    break;
                printf( "Thread[%s] start work[%lu]\n", Stringify( this_thread::get_id() ).c_str(),
                        wkthread->last_wake_ts );
                if ( wkthread->call == nullptr )
                    continue;
                wkthread->call( wkthread->args, wkthread->ret );
                do
                {
                    std::unique_lock< std::mutex > lk( wkthread->pWork->_M_mtx );
                    wkthread->status = e_done;
                    if ( wkthread->pWork->IsBusy() )
                    {
                        wkthread->last_sleep_ts = chrono::system_clock::now().time_since_epoch().count();
                        wkthread->pWork->_M_done_cv.wait(
                            lk, [ wkthread ] { return ( wkthread->pWork->_M_status == e_done ); } );
                        wkthread->last_wake_ts = chrono::system_clock::now().time_since_epoch().count();
                    }
                    else
                    {
                        printf( "All Finish[thread:%s]\n", Stringify( this_thread::get_id() ).c_str() );
                        wkthread->pWork->_M_status = e_done;
                        wkthread->pWork->_M_done_cv.notify_all();  // 唤醒所有子线程
                    }
                } while ( 0 );
            }
            std::unique_lock< std::mutex > lk( wkthread->pWork->_M_mtx );
            if ( wkthread->thread_name && wkthread->thread_name != wkthread->__tname )
                ::operator delete( ( void* )wkthread->thread_name );
            // printf( "Thread[%s] exit\n", Stringify( this_thread::get_id() ).c_str() );
            wkthread->exit = 1;
        } );
        return __pthread;
    }
    void __InitEntry( int id, void ( *call )( void*, void* ), void* args, void* ret )
    {
        _M_threads[ id ].ready   = 0;
        _M_threads[ id ].exit    = 0;
        _M_threads[ id ].index   = id;
        _M_threads[ id ].call    = call;
        _M_threads[ id ].args    = args;
        _M_threads[ id ].ret     = ret;
        _M_threads[ id ].pWork   = this;
        _M_threads[ id ].status  = e_init;
        _M_threads[ id ].pThread = nullptr;
    }
    void WakeUp()
    {
        std::lock_guard< std::mutex > lk( this->_M_mtx );
        _M_start_cv.notify_all();
    }
    void SetExit()
    {
        std::lock_guard< std::mutex > lk( this->_M_mtx );
        this->_M_status = e_exit;
    }
    void Exit()
    {
        WaitReady();  // 等待所有线程 Ready
        SetExit();
        WakeUp();
        WaitExit();
        Reset();
    }
    void Reset()
    {
        this->_M_status   = e_init;
        this->_M_start_ts = this->_M_done_ts = 0;
        for ( auto& t : this->_M_threads )
        {
            if ( t.status == e_init )
                continue;
            __InitEntry( t.index, nullptr, nullptr, nullptr );
        }
    }
    void Start()
    {
        WaitReady();
        std::lock_guard< std::mutex > lk( this->_M_mtx );
        this->_M_status = e_running;
        for ( auto& t : this->_M_threads )
        {
            t.ready = 0;  // 清除 prepare 标志
        }
        _M_start_ts = chrono::system_clock::now().time_since_epoch().count();
        _M_start_cv.notify_all();
    }
    void Wait()
    {
        std::unique_lock< std::mutex > lk( this->_M_mtx );
        _M_done_cv.wait( lk, [ this ] { return this->_M_status == e_done; } );  // wait all done
        _M_done_ts = chrono::system_clock::now().time_since_epoch().count();
    }
    DEFINE_TEST_FUNC( IsBusy, status, e_running, true )
    DEFINE_TEST_FUNC( IsReady, ready, 0, false )
    DEFINE_TEST_FUNC( IsExit, exit, 0, false )
    DEFINE_WAIT_FUNC( WaitReady, IsReady )
    DEFINE_WAIT_FUNC( WaitExit, IsExit )
};  // namespace async
inline shared_ptr< ParallelWork > GetParallelWork( size_t sz )
{
    return make_shared< ParallelWork >( sz );
}
using ParallelWork_S_ptr_t = shared_ptr< ParallelWork >;
}  // namespace ns_async