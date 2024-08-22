#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#include <cstdlib>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <queue>

template < typename Task > class ThreadPoolBase
{
    static const size_t default_thread_num      = 1;
    static const size_t default_task_queue_size = 20;

protected:
    ThreadPoolBase() : ThreadPoolBase( default_thread_num, default_task_queue_size ){};
    ThreadPoolBase( size_t __thread_num, size_t __task_queue_sz )
        : _M_flag( false ), _M_thread_num( __thread_num ), _M_task_queue_sz( __task_queue_sz )
    {
    }
    void _M_add_task( Task* __p )
    {
        std::unique_lock< std::mutex > __lock( _M_mutex );
        _M_cond_full.wait( __lock, [ this ]() { return !_M_flag || !this->_M_task_full(); } );
        if ( !_M_flag )
        {
            return;
        }
        _M_tasks.push( __p );
        _M_cond_empty.notify_all();
        std::cout << "push task: " << __p->_M_id << " task_cnt=" << _M_tasks.size() << std::endl;
    }
    void _M_destroy_task( Task* __p )
    {
        __p->~Task();
        delete __p;
    }
    Task* _M_get_task()
    {
        std::unique_lock< std::mutex > __lock( _M_mutex );
        _M_cond_empty.wait( __lock, [ this ]() { return !_M_flag || !_M_tasks.empty(); } );
        if ( !_M_flag )
        {
            return nullptr;
        }
        Task* __temp = _M_tasks.front();
        _M_tasks.pop();
        _M_cond_full.notify_all();
        return __temp;
    }
    bool _M_task_full()
    {
        return _M_tasks.size() == _M_task_queue_sz;
    }
    bool _M_task_empty()
    {
        return _M_tasks.empty();
    }
    void _M_join()
    {
        for ( auto& __j : _M_threads )
        {
            if ( __j.joinable() )
            {
                __j.join();
            }
        }
        if ( _M_produce_thread.joinable() )
        {
            _M_produce_thread.join();
        }
    }
    void _M_set_flag( bool __flag )
    {
        _M_flag = __flag;
    }
    void _M_notify_all()
    {
        _M_cond_empty.notify_all();
        _M_cond_full.notify_all();
    }
    void _M_clean_task()
    {
        for ( ; !_M_tasks.empty(); )
        {
            Task* __p = _M_tasks.front();
            _M_tasks.pop();
            __p->Run();
            _M_destroy_task( __p );
        }
    }
    std::atomic_bool           _M_flag;
    size_t                     _M_thread_num;
    size_t                     _M_task_queue_sz;
    std::vector< std::thread > _M_threads;
    std::queue< Task* >        _M_tasks;
    std::mutex                 _M_mutex;
    std::condition_variable    _M_cond_empty;
    std::condition_variable    _M_cond_full;
    std::thread                _M_produce_thread;
};

template < typename Task > class ThreadPool : protected ThreadPoolBase< Task >
{
public:
    using _Base = ThreadPoolBase< Task >;
    using _Base::_M_add_task;
    using _Base::_M_destroy_task;
    using _Base::_M_flag;
    using _Base::_M_get_task;
    using _Base::_M_produce_thread;
    using _Base::_M_set_flag;
    using _Base::_M_thread_num;
    using _Base::_M_threads;
    ThreadPool() = default;
    ThreadPool( size_t __thread_num, size_t __task_queue_sz ) : _Base( __thread_num, __task_queue_sz ) {}
    void start()
    {
        _M_start();
    }
    void stop()
    {
        _M_stop();
    }
    void sleep( size_t __n )
    {
        std::this_thread::sleep_for( std::chrono::seconds( __n ) );
    }

private:
    void _M_start()
    {
        _M_set_flag( true );
        _M_start_produce();
        _M_start_work();
    }
    void _M_stop()
    {
        _Base::_M_set_flag( false );
        _Base::_M_notify_all();
        _Base::_M_join();
        _Base::_M_clean_task();
    }
    void _M_produce()
    {
        static int __id = 0;
        for ( ; _M_flag; )
        {
            Task* __p = new Task( __id++ );
            _M_add_task( __p );
            std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
        }
    }
    void _M_start_produce()
    {
        std::thread __t   = thread( &ThreadPool::_M_produce, this );
        _M_produce_thread = std::move( __t );
    }
    void _M_work()
    {
        for ( ; _M_flag; )
        {
            Task* __p = _M_get_task();
            if ( !__p )
            {
                return;
            }
            __p->Run();
            std::cout << "[thread: " << std::this_thread::get_id() << "]"
                      << " process task: " << __p->_M_id << std::endl;
            _M_destroy_task( __p );
            std::this_thread::sleep_for( std::chrono::milliseconds( 280 ) );
        }
    }
    void _M_start_work()
    {
        for ( size_t j = 0; j < _M_thread_num; j++ )
        {
            _M_threads.emplace_back( &ThreadPool::_M_work, this );
        }
    }
};
#endif