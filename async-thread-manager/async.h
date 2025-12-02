#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std;

static const int e_exit    = -2;
static const int e_init    = -1;
static const int e_running = 0;
static const int e_done    = 1;

std::string get_thread_id( thread::id __id )
{
    stringstream ss;
    ss << __id;
    return ss.str();
}
class ParallelWork;
struct WorkThread
{
    int             status;
    int             ready;
    int             index;
    int             exit;
    std::thread::id thread_id;
    void ( *call )( void*, void* );  // function
    void*                     args;  // args
    void*                     ret;   // return value
    ParallelWork*             pWork;
    shared_ptr< std::mutex >  mtx;
    shared_ptr< std::thread > pThread;
    unsigned long             last_sleep_ts;
    unsigned long             last_wake_ts;
    WorkThread()
        : status( e_init ), ready( 0 ), index( -1 ), exit( 0 ), thread_id(), call( nullptr ), args( nullptr ),
          pWork( nullptr ), pThread( nullptr ), last_sleep_ts( 0 ), last_wake_ts( 0 )
    {
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
    int                       _M_exit;
    std::vector< WorkThread > _M_threads;
    condition_variable        _M_start_cv;  // start
    condition_variable        _M_done_cv;   // end
    unsigned long             _M_start_ts;
    unsigned long             _M_done_ts;
    ParallelWork( size_t sz = 32 ) : _M_status( e_init ), _M_sz( sz ), _M_exit( 0 ), _M_start_ts( 0 ), _M_done_ts( 0 )
    {
        _M_threads.resize( sz );
        SetUp();
    }

    void Operation( int id, void* func, void* args, void* ret )
    {
        _M_threads[ id ].call   = ( void ( * )( void*, void* ) )func;
        _M_threads[ id ].args   = args;
        _M_threads[ id ].ret    = ret;
        _M_threads[ id ].status = e_running;
        _M_threads[ id ].index  = id;
        if ( !_M_threads[ id ].pThread )
        {
            auto __pthread = __CreateThread( &_M_threads[ id ] );
            __pthread->detach();
            _M_threads[ id ].pThread = __pthread;
        }
    }
    void SetUp()
    {
        for ( int i = 0; i < _M_sz; ++i )
        {
            __SetUp( i, nullptr, nullptr, nullptr );
        }
    }
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
                    // printf( "Thread[%s] Ready\n", get_thread_id( this_thread::get_id() ).c_str() );
                    wkthread->last_sleep_ts = chrono::system_clock::now().time_since_epoch().count();
                    wkthread->pWork->_M_start_cv.wait( lk, [ wkthread ] {
                        return ( wkthread->pWork->_M_status == e_exit || wkthread->pWork->_M_status == e_running );
                    } );
                    wkthread->last_wake_ts = chrono::system_clock::now().time_since_epoch().count();
                } while ( 0 );
                if ( wkthread->pWork->_M_status == e_exit )
                {
                    // printf( "Thread[%s] exit\n", get_thread_id( this_thread::get_id() ).c_str() );
                    wkthread->exit = 1;
                    return;
                }
                // printf( "Thread[%s] start work\n", get_thread_id( this_thread::get_id() ).c_str() );
                if ( wkthread->call == nullptr )
                    continue;
                wkthread->call( wkthread->args, wkthread->ret );
                wkthread->pWork->_M_mtx.lock();
                wkthread->status = e_done;
                int id           = 0;
                for ( auto& tt : wkthread->pWork->_M_threads )
                {
                    if ( tt.status == e_init )
                        continue;
                    if ( tt.status == e_running )
                        goto still_busy;
                }
                printf( "All Finish[thread:%s]\n", get_thread_id( this_thread::get_id() ).c_str() );
                wkthread->pWork->_M_status = e_done;
                wkthread->pWork->_M_done_cv.notify_all();  // 唤醒所有子线程
                wkthread->pWork->_M_mtx.unlock();
                continue;
            still_busy:
                do
                {
                    wkthread->pWork->_M_mtx.unlock();
                    std::unique_lock< std::mutex > lk( wkthread->pWork->_M_mtx );
                    wkthread->last_sleep_ts = chrono::system_clock::now().time_since_epoch().count();
                    wkthread->pWork->_M_done_cv.wait(
                        lk, [ wkthread ] { return ( wkthread->pWork->_M_status == e_done ); } );
                    wkthread->last_wake_ts = chrono::system_clock::now().time_since_epoch().count();
                } while ( 0 );
            }
        } );
        return __pthread;
    }
    void __SetUp( int id, void ( *call )( void*, void* ), void* args, void* ret )
    {
        _M_threads[ id ].call   = call;
        _M_threads[ id ].args   = args;
        _M_threads[ id ].ret    = ret;
        _M_threads[ id ].pWork  = this;
        _M_threads[ id ].status = e_init;
    }
    void Start()
    {
        this->_M_mtx.lock();
        // 忙等优化 //(短时间等待)
        while ( !this->IsReady() )
        {
            this->_M_mtx.unlock();
            this_thread::yield();
            this->_M_mtx.lock();
        }
        this->_M_status = e_running;
        for ( auto& t : this->_M_threads )
        {
            t.ready = 0;  // 清除 prepare 标志
        }
        _M_start_ts = chrono::system_clock::now().time_since_epoch().count();
        _M_start_cv.notify_all();
        this->_M_mtx.unlock();
    }
    bool IsReady()
    {
        for ( const auto& t : this->_M_threads )
        {
            if ( t.status == e_init )
                continue;
            if ( !t.ready )
                return false;
        }
        return true;
    }
    bool IsExit()
    {
        for ( const auto& t : this->_M_threads )
        {
            if ( t.status == e_init )
                continue;
            if ( !t.exit )
                return false;
        }
        return true;
    }
    void WaitReady()
    {
        this->_M_mtx.lock();
        while ( !this->IsReady() )
        {
            this->_M_mtx.unlock();
            this_thread::yield();
            this->_M_mtx.lock();
        }
        this->_M_mtx.unlock();
    }
    void WaitExit()
    {
        this->_M_mtx.lock();
        while ( !this->IsExit() )
        {
            this->_M_mtx.unlock();
            this_thread::yield();
            this->_M_mtx.lock();
        }
        this->_M_mtx.unlock();
    }
    void WakeUp()
    {
        this->_M_mtx.lock();
        _M_start_cv.notify_all();
        this->_M_mtx.unlock();
    }
    void Stop()
    {
        WaitReady();  // 等待所有线程 Ready
        this->_M_mtx.lock();
        this->_M_status = e_exit;
        this->_M_mtx.unlock();
        WakeUp();
        WaitExit();
        Reset();
    }
    void Reset()
    {
        for ( auto& t : this->_M_threads )
        {
            if ( t.status == e_init )
                continue;
            __SetUp( t.index, nullptr, nullptr, nullptr );
            this->_M_threads[ t.index ].pThread = nullptr;
        }
    }
    void Wait()
    {
        std::unique_lock< std::mutex > lk( this->_M_mtx );
        _M_done_cv.wait( lk, [ this ] { return this->_M_status == e_done; } );  // wait all done
        _M_done_ts = chrono::system_clock::now().time_since_epoch().count();
    }
};  // namespace async
inline ParallelWork* GetParallelWork( size_t sz )
{
    static ParallelWork instance( sz );
    return &instance;
}