#include "block_queue.h"
#include <unistd.h>

using namespace std;

static mutex __print_mutex;  // 全局 mutex 定义

template < typename T > void product( BlockQueue< T >& queue )
{
    int start = 0;
    while ( 1 )
    {
        T t( start );
        queue.put( t );
        do
        {
            lock_guard< mutex > lock( __print_mutex );
            cout << "[thread] " << this_thread::get_id() << " ==== [put] val: " << t << " size:" << queue.size()
                 << endl;
        } while ( 0 );
        ++start;
        usleep( 1000 * 200 );
    }
}

template < typename T > void consumer( BlockQueue< T >& queue )
{
    while ( 1 )
    {
        T t = queue.get();
        do
        {
            lock_guard< mutex > lock( __print_mutex );
            cout << "[thread] " << this_thread::get_id() << " ==== [get] val: " << t << " size:" << queue.size()
                 << endl;
        } while ( 0 );
        usleep( 1000 * 200 );
    }
}

int main()
{
    BlockQueue< int > queue( 100 );
    thread            t1( product< int >, ref( queue ) );
    thread            t2( consumer< int >, ref( queue ) );
    t1.join();
    t2.join();
}