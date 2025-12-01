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

static const int e_init    = -1;
static const int e_running = 0;
static const int e_done    = 1;

std::string get_thread_id( thread::id __id )
{
    stringstream ss;
    ss << __id;
    return ss.str();
}
class ParallelWork
{
public:
    struct ParallelThread
    {
        int             status;
        int             ready;
        int             index;
        int             exit;
        std::thread::id thread_id;
        void ( *call )( void*, void* );  // function
        void*                    args;   // args
        void*                    ret;    // return value
        ParallelWork*            pWork;
        shared_ptr< std::mutex > mtx;
        ParallelThread()
            : status( e_init ), ready( 0 ), index( -1 ), exit( 0 ), thread_id(), call( nullptr ), args( nullptr ),
              pWork( nullptr )
        {
        }
        ParallelThread( const ParallelThread& t )            = default;
        ParallelThread& operator=( const ParallelThread& t ) = default;
        ParallelThread( ParallelThread&& t ) : ParallelThread( t ) {};
    };
    int                                      _M_status;
    std::mutex                               _M_mtx;
    size_t                                   _M_sz;
    int                                      _M_exit;
    std::vector< shared_ptr< std::thread > > _M_threads;
    std::vector< ParallelThread >            _M_data;
    condition_variable                       start_cv;  // start
    condition_variable                       done_cv;   // end
    ParallelWork( size_t sz = 32 ) : _M_status( e_init ), _M_sz( sz ), _M_exit( 0 )
    {
        _M_threads.resize( sz );
        _M_data.resize( sz );
        SetUp();
    }

    void Operation( int id, void* func, void* args, void* ret )
    {
        _M_data[ id ].call   = ( void ( * )( void*, void* ) )func;
        _M_data[ id ].args   = args;
        _M_data[ id ].ret    = ret;
        _M_data[ id ].status = e_running;
        if ( !_M_threads[ id ] )
        {
            auto __pthread = __CreateThread( id );
            __pthread->detach();
            _M_threads[ id ] = __pthread;
        }
    }
    void SetUp()
    {
        for ( int i = 0; i < _M_sz; ++i )
        {
            __SetUp( i, nullptr, nullptr, nullptr );
        }
    }
    shared_ptr< std::thread > __CreateThread( int id )
    {
        auto __pthread = make_shared< std::thread >( [ this, id ]() -> void {
            ParallelThread& t = this->_M_data[ id ];
            t.thread_id       = this_thread::get_id();
            for ( ;; )
            {
                do
                {
                    std::unique_lock< std::mutex > lk( this->_M_mtx );
                    t.status = e_running;
                    t.ready  = 1;  // start 前必须保证所有的线程 ready
                    this->start_cv.wait(
                        lk, [ this, &t ] { return ( this->_M_status == e_running && t.call != nullptr ); } );
                } while ( 0 );
                if ( this->_M_exit )
                {
                    // printf( "Thread[%s] exit\n", get_thread_id( this_thread::get_id() ).c_str() );
                    t.exit = 1;
                    return;
                }
                // printf( "Thread[%s] start work\n", get_thread_id( this_thread::get_id() ).c_str() );
                if ( t.call == nullptr )
                    continue;
                t.call( t.args, t.ret );
                this->_M_mtx.lock();
                t.status = e_done;
                int id   = 0;
                for ( auto& tt : this->_M_data )
                {
                    if ( tt.status == e_init )
                        continue;
                    if ( tt.status == e_running )
                        goto still_busy;
                }
                printf( "All Finish[thread:%s]\n", get_thread_id( this_thread::get_id() ).c_str() );
                this->_M_status = e_done;
                this->done_cv.notify_all();  // 唤醒所有子线程
                this->_M_mtx.unlock();
                continue;
            still_busy:
                do
                {
                    this->_M_mtx.unlock();
                    std::unique_lock< std::mutex > lk( this->_M_mtx );
                    this->done_cv.wait( lk, [ this, &t ] { return ( this->_M_status == e_done ); } );
                } while ( 0 );
            }
        } );
        return __pthread;
    }
    void __SetUp( int id, void ( *call )( void*, void* ), void* args, void* ret )
    {
        _M_data[ id ].call   = call;
        _M_data[ id ].args   = args;
        _M_data[ id ].ret    = ret;
        _M_data[ id ].pWork  = this;
        _M_data[ id ].status = e_init;
    }
    void Start()
    {
        this->_M_mtx.lock();
        // 忙等优化 //(短时间等待)
        while ( !this->AllReady() )
        {
            this->_M_mtx.unlock();
            this_thread::yield();
            this->_M_mtx.lock();
        }
        this->_M_status = e_running;
        for ( auto& t : this->_M_data )
        {
            t.ready = 0;  // 清除 prepare 标志
        }
        start_cv.notify_all();
        this->_M_mtx.unlock();
    }
    bool AllReady()
    {
        for ( const auto& t : this->_M_data )
        {
            if ( t.status == e_init )
                continue;
            if ( !t.ready )
                return false;
        }
        return true;
    }
    bool AllExit()
    {
        for ( const auto& t : this->_M_data )
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
        while ( !this->AllReady() )
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
        while ( !this->AllExit() )
        {
            this->_M_mtx.unlock();
            this_thread::yield();
            this->_M_mtx.lock();
        }
        this->_M_mtx.unlock();
    }
    void Stop()
    {
        this->_M_mtx.lock();
        this->_M_exit = 1;
        this->_M_mtx.unlock();
        Start();
        WaitExit();
    }
    void Wait()
    {
        std::unique_lock< std::mutex > lk( this->_M_mtx );
        done_cv.wait( lk, [ this ] { return this->_M_status == e_done; } );  // wait all done
    }
};  // namespace async
inline ParallelWork* GetParallelWork( size_t sz )
{
    static ParallelWork instance( sz );
    return &instance;
}