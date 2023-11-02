#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

using namespace std;

template < typename T > class BlockQueue
{
public:
    BlockQueue() : BlockQueue( _MAX_SIZE ) {}
    BlockQueue( size_t size_ ) : _max_size( size_ )
    {
        if ( _max_size == 0 )
        {
            _max_size = _MAX_SIZE;
        }
    }
    bool empty()
    {
        return _data.empty();
    }
    bool full()
    {
        return _data.size() == _max_size;
    }
    T& put( T& t )
    {
        unique_lock< mutex > lock( _mtx );
        full_cond.wait( lock, [ this ]() { return !this->full(); } );
        _data.push( t );
        empty_cond.notify_all();
        return _data.back();
    }
    T& get()
    {
        unique_lock< mutex > lock( _mtx );
        empty_cond.wait( lock, [ this ]() { return !this->empty(); } );
        T& _t = _data.front();
        _data.pop();
        full_cond.notify_all();
        return _t;
    }
    size_t size()
    {
        return _data.size();
    }

private:
    mutex              _mtx;
    size_t             _max_size;
    condition_variable full_cond;
    condition_variable empty_cond;
    queue< int >       _data;
    static const int   _MAX_SIZE = 100;
};