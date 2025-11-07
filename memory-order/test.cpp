#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <tuple>
#include <sstream>
#include <thread>
#include <stdarg.h>
#include <atomic>

using namespace std;

atomic< int > data1{ 0 };
atomic< int > data2{ 0 };

atomic< bool > __flag{ false };

void write_func( void )
{
    data1.store( 100, memory_order_relaxed );    // memory-order-relaxed 保证自身操作的原子性
    data2.store( 200, memory_order_relaxed );    // memory-order-relaxed 保证自身操作的原子性
    __flag.store( true, memory_order_release );  // memory-order-release 保证前面的写操作完成
    // 通过 flag 进行同步，保证前面的写操作完成 // 监控变量 //
}

int main()
{
    std::thread t_write( write_func );
    while ( !__flag.load( memory_order_acquire ) )  // memory-order-acquire
    {
        cout << "waiting... data1 = " << data1.load( memory_order_relaxed ) << " | "
             << "data2 = " << data2.load( memory_order_relaxed ) << endl;
    }
    cout << "data1 = " << data1 << " | " << "data2 = " << data2 << endl;
    t_write.join();
    return 0;
}