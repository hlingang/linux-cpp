#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

using namespace std;

template < typename T > class BlockQueue
{
public:
    BlockQueue() : BlockQueue( _DEFAULT_CAPACITY ) {}
    BlockQueue( size_t cap ) : _cap( cap )
    {
        if ( _cap == 0 )
        {
            _cap = _DEFAULT_CAPACITY;
        }
    }
    bool empty()
    {
        lock_guard< std::mutex > lock( _mtx );
        return __empty();
    }
    bool full()
    {
        lock_guard< std::mutex > lock( _mtx );
        return __full();
    }
    T& put( T& t )
    {
        unique_lock< mutex > lock( _mtx );
        _full_cond.wait( lock, [ this ]() { return !this->__full(); } );
        _data.push( t );
        _empty_cond.notify_all();
        return _data.back();
    }
    T get()
    {
        unique_lock< mutex > lock( _mtx );
        _empty_cond.wait( lock, [ this ]() { return !this->__empty(); } );
        T _t = _data.front();
        _data.pop();
        _full_cond.notify_all();
        return _t;
    }
    size_t size()
    {
        lock_guard< std::mutex > lock( _mtx );
        return _data.size();
    }

private:
    bool __empty()
    {
        return _data.empty();
    }
    bool __full()
    {
        return _data.size() == _cap;
    }

private:
    queue< int >       _data;
    mutex              _mtx;
    size_t             _cap;
    condition_variable _full_cond;
    condition_variable _empty_cond;
    static const int   _DEFAULT_CAPACITY = 1000;
};