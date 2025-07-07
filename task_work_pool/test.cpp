#include <algorithm>
#include <iostream>
#include <cstring>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

using namespace std;

#define MAX_THREADS 20
#define MIN_THREADS 2

struct list_head {
    list_head *next;
    list_head *prev;
};

static inline int list_size(struct list_head *head)
{
    int n;
    list_head *pos;
    for (pos = head->next, n = 0; pos != head; pos = pos->next, n++)
    {
    }
    return n;
}

inline void INIT_LIST_HEAD(list_head *list) {
    list->next = list;
    list->prev = list;
}

inline bool list_empty(const list_head *list) {
    return list->next == list;
}
inline void list_add(list_head *new_node, list_head *head) {
    new_node->next = head->next;
    new_node->prev = head;
    head->next->prev = new_node;
    head->next = new_node;
}
inline void list_move(list_head *new_node, list_head *head) {
    list_del(new_node);
    list_add(new_node, head);
}
inline void list_del(list_head *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
}

inline void list_del_init(list_head *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    INIT_LIST_HEAD(entry);
}

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define list_for_each(pos, head) \
    for (list_head *pos = (head)->next; pos != (head); pos = pos->next)
#define list_for_each_entry(pos, head, member) \
    for ( pos = list_entry((head)->next, __typeof__(*pos), member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, __typeof__(*pos), member))

#define LIST_HEAD(name)\
    list_head name = {&name, &name};

struct work_struct 
{
    void (*func)(void *);
    void *data;
    list_head node;
    std::thread::id who;
    condition_variable cond;
    mutex mtx;
    std::chrono::system_clock::time_point last_sleep_time;
};

void test_func(void *data) {
    std::cout << "Executing test function with data: " << *(int *)data << std::endl;
}

vector<std::thread> threads;
LIST_HEAD(work_list);
LIST_HEAD(work_thread_list);
std::mutex list_mutex;
unsigned long nr_threads = 0;
std::chrono::system_clock::time_point last_empty_time = std::chrono::system_clock::now();

void start_one_thread();
int operation(void test_func(void * data), void * data )
{
    int ret = -1;
     struct work_struct *work = nullptr;
    list_mutex.lock();
    if(list_empty(&work_list)) 
    {
        cout << "work-list is empty, starting a new thread." << std::endl;
        ret = -1;
        goto out;
    }
    cout << "work-list size = " << list_size(&work_list) << std::endl;
    work = list_entry(work_list.next, struct work_struct, node);
    list_del_init(work_list.next);
    work->func = test_func;
    work->data = data;
    work->cond.notify_one(); // 唤醒工作线程
    cout << "Thread " << work->who << " is notified to execute work." << std::endl;
out:
    list_mutex.unlock();
    return ret;
}
void work_thread_func()
{
    struct work_struct work;
    work.func = nullptr;
    work.data = nullptr;
    work.who = std::this_thread::get_id();
    INIT_LIST_HEAD(&work.node); // 保证链表操作的安全性
    nr_threads++;
    list_mutex.lock(); 
    for(;;)
    {
        list_move(&work.node, &work_list); // 保证聊表添加的安全性，避免重复添加 
        cout << "Thread " << work.who << " added to work list." << std::endl;
        cout << "Current work list size: " << list_size(&work_list) << std::endl;
        list_mutex.unlock();
        unique_lock<mutex> lock(work.mtx);
        work.last_sleep_time = std::chrono::system_clock::now();
        work.cond.wait(lock); // 开始休眠
        list_mutex.lock();
        cout << "Thread " << work.who << " woke up." << std::endl;
        if(!list_empty(&work.node)) 
        {
            work.func = nullptr; // 清除函数指针，避免重复执行
            work.data = nullptr; // 清除数据指针
            continue;
        }
        list_mutex.unlock();
        if (work.func) 
        {
            work.func(work.data);
        }
        list_mutex.lock();
        auto now = std::chrono::system_clock::now();
        if(list_empty(&work_list)) 
        {
            auto empty_duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_empty_time).count();
            if(empty_duration > 10 && nr_threads < MAX_THREADS) 
            {
                start_one_thread();
            }
        }
        if(std::chrono::duration_cast<std::chrono::seconds>(now - work.last_sleep_time).count() > 5 && nr_threads > MIN_THREADS) 
        {
            break;
        }
    }
    cout << "Thread " << work.who << " is exiting." << std::endl;
    nr_threads--;
    list_mutex.unlock();
}

void start_one_thread()
{
   
   auto t = thread(work_thread_func);
    cout << "Started a new thread: " << t.get_id() << endl;
    t.detach(); // 分离线程
}

int main( int argc, char* argv[] )
{
    for(int i = 0; i < 5; ++i) {
        start_one_thread();
    }
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 等待线程启动
    for(;;)
    {
        unsigned int x = rand() % 100;
        operation(test_func, new int(x));
        operation(test_func, new int(x+10));
        operation(test_func, new int(x+21));
        operation(test_func, new int(x+32));
        std::this_thread::sleep_for(std::chrono::seconds(10)); // 模拟工作间隔
    }
    cout << "All threads have finished execution." << endl;
    return 0;
}
