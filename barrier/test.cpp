#include <__barrier.h>
#include <vector>
#include <thread>
#include <unistd.h>

using namespace std;

#define N_THREADS (4 * 4)

static SyncBarrier __m_barrier(N_THREADS);

void funcx(int i)
{
  while (true)
  {
    __m_barrier.wait_start();
    uint64_t start_ts = std::chrono::system_clock::now().time_since_epoch().count();
    printf("thread id:%2d start time:%lu #########.\n", i, start_ts);
    sleep(2);
    __m_barrier.wait_stop();
    uint64_t end_ts = std::chrono::system_clock::now().time_since_epoch().count();
    printf("thread id:%2d stop  time:%lu #########.\n", i, end_ts);
  }
}

int main()
{

  vector<thread> threads;
  std::thread::id __tid = std::this_thread::get_id();
  for (int i = 0; i < N_THREADS; i++)
  {
    threads.emplace_back(thread(funcx, i));
  }
  sleep(1);
  while (true)
  {
    __m_barrier.start();
    __m_barrier.stop();
    sleep(1); // make sure child thread wait_start before next __start //
    printf("------------------------------------------------------------\n");
  }
  return 0;
}
