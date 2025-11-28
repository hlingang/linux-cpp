#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

using namespace std;

static const int e_init    = -1;
static const int e_running = 0;
static const int e_done    = 1;

class ParallelWork
{
public:
    struct ParallelThread
    {
        int status;
        void ( *call )( void*, void* );  // function
        void*                    args;   // args
        void*                    ret;    // return value
        ParallelWork*            pWork;
        shared_ptr< std::mutex > mtx;
        ParallelThread() : status( e_init ), call( nullptr ), args( nullptr ), pWork( nullptr ) {}
        ParallelThread( const ParallelThread& t )            = default;
        ParallelThread& operator=( const ParallelThread& t ) = default;
        ParallelThread( ParallelThread&& t ) : ParallelThread( t ) {};
    };
    int                                      _M_status;
    std::mutex                               _M_mtx;
    size_t                                   _M_sz;
    std::vector< shared_ptr< std::thread > > _M_threads;
    std::vector< ParallelThread >            _M_data;
    condition_variable                       s_cv;  // start
    condition_variable                       e_cv;  // end
    ParallelWork( size_t sz = 32 ) : _M_status( e_init ), _M_sz( sz )
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
    }
    void SetUp()
    {
        for ( int i = 0; i < _M_sz; ++i )
        {
            __SetUp( i, nullptr, nullptr, nullptr );
        }
    }
    void __SetUp( int id, void ( *call )( void*, void* ), void* args, void* ret )
    {
        _M_data[ id ].call   = call;
        _M_data[ id ].args   = args;
        _M_data[ id ].ret    = ret;
        _M_data[ id ].pWork  = this;
        _M_data[ id ].status = e_init;
        auto pthread         = make_shared< std::thread >( [ this, id ]() -> void {
            ParallelThread& t = this->_M_data[ id ];
            for ( ;; )
            {
                do
                {
                    t.status = e_running;
                    std::unique_lock< std::mutex > lk( this->_M_mtx );
                    this->s_cv.wait( lk,
                                             [ this, &t ] { return ( this->_M_status == e_running && t.call != nullptr ); } );
                } while ( 0 );
                if ( t.call == nullptr )
                    continue;
                t.call( t.args, t.ret );
                this->_M_mtx.lock();
                t.status = e_done;
                printf( "Thread[%ld] done\n", this_thread::get_id() );
                if ( this->_M_status != e_done )
                {
                    int id = 0;
                    for ( auto& tt : this->_M_data )
                    {
                        printf( "[%ld] check status[%d] : %d\n", this_thread::get_id(), id++, tt.status );
                        if ( tt.status == e_running )
                        {
                            this->_M_mtx.unlock();
                            std::unique_lock< std::mutex > lk( this->_M_mtx );
                            this->e_cv.wait( lk, [ this, &t ] { return ( this->_M_status == e_done ); } );
                        }
                    }
                    printf( "All Finish[thread:%ld]\n", this_thread::get_id() );
                    this->_M_status = e_done;
                    this->e_cv.notify_all();
                    this->_M_mtx.unlock();
                }
            }
        } );
        _M_threads[ id ]     = pthread;
    }
    void Start()
    {
        this->_M_mtx.lock();
        this->_M_status = e_running;
        s_cv.notify_all();
        this->_M_mtx.unlock();
    }
    void Wait()
    {
        std::unique_lock< std::mutex > lk( this->_M_mtx );
        e_cv.wait( lk, [ this ] { return this->_M_status == e_done; } );
    }
};  // namespace async
inline ParallelWork* GetParallelWork( size_t sz )
{
    static ParallelWork instance( sz );
    return &instance;
}