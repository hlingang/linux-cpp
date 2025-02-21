#include <pthread.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace std;

class SyncBarrier
{
public:
    SyncBarrier(int __child_nthread) : inited(false), __tid(std::this_thread::get_id()), nwait(0), child_nthread(__child_nthread), total_nthread(__child_nthread + 1)
    {
        init();
    }
    ~SyncBarrier()
    {
        __destroy();
    }
    void init()
    {
        pthread_barrier_init(&barrier_stop, nullptr, total_nthread);
        pthread_barrier_init(&barrier_prepare, nullptr, total_nthread);
        inited = true;
    }
    void start()
    {
        while (__get_nwait() < child_nthread)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        cv_start.notify_all();
    }
    void stop()
    {
        wait_stop();
    }
    void wait_start()
    {
        __reset_barrier(&barrier_prepare);
        __inc_wait();
        std::unique_lock<std::mutex> lock(__mutex);
        cv_start.wait(lock);
    }
    void wait_stop()
    {
        pthread_barrier_wait(&barrier_stop);
        __wait_prepare();
    }

private:
    void __inc_wait()
    {
        std::lock_guard<std::mutex> lock(__mutex);
        nwait++;
    }
    int __get_nwait()
    {
        std::lock_guard<std::mutex> lock(__mutex);
        return nwait;
    }
    void __reset_nwait()
    {
        if (inited && __tid == this_thread::get_id())
        {
            std::lock_guard<std::mutex> lock(__mutex);
            nwait = 0;
        }
    }
    void __wait_prepare()
    {
        __reset_barrier(&barrier_stop);
        __reset_nwait();
        pthread_barrier_wait(&barrier_prepare);
    }
    void __destroy()
    {
        pthread_barrier_destroy(&barrier_stop);
        pthread_barrier_destroy(&barrier_prepare);
    }
    void __reset_barrier(pthread_barrier_t *__barrier)
    {
        if (inited && __tid == this_thread::get_id())
        {
            pthread_barrier_destroy(__barrier);
            pthread_barrier_init(__barrier, nullptr, total_nthread);
        }
    }
    condition_variable cv_start;
    pthread_barrier_t barrier_stop;
    pthread_barrier_t barrier_prepare;
    std::mutex __mutex;
    std::thread::id __tid;
    int nwait;
    bool inited;
    int child_nthread;
    int total_nthread;
};