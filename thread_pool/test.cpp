#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <pthread.h>
#include <regex>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <utility>
#include <vector>
#include <array>
#include <atomic>
#include <cstddef>
#include <cassert>
#include <cstdio>
#include <iostream>

using namespace std;

#include "thread_pool.h"

struct MyTask
{
    MyTask( int __id ) : _M_id( __id ) {}
    int  _M_id;
    void Run() const
    {
        cout << "task id: " << _M_id << endl;
    }
};

int main( int argc, char* argv[] )
{
    ThreadPool< MyTask > __thread_pool( 10, 20 );
    __thread_pool.start();
    __thread_pool.sleep( 10 );
    __thread_pool.stop();
    return 0;
}
