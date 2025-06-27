#include <algorithm>
#include <iostream>
#include <cstring>
#include <thread>
#include <string>
#include <mutex>

using namespace std;

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
inline void list_del(list_head *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
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
    list_head name = {&name, &name}

struct obj_data {
    int s_count;
    int id;
    string val;
    list_head node;
    std::mutex mtx;
};


LIST_HEAD(obj_list);
std::mutex list_mutex;

int __put_obj(obj_data *obj)
{
    size_t ref  = obj->s_count;
    cout <<this_thread::get_id()<<   " [__put_obj] Object ID: " << obj->id << ", Current Ref Count: " << ref << endl;
    if (--obj->s_count <= 0) {
        list_del(&obj->node);
        cout <<this_thread::get_id()<< " Deleting Object ID: " << obj->id << ", Value: " << obj->val << endl;
        delete obj;
        return 1;
    }
    return 0;
}

int put_obj(obj_data *obj)
{
    int ret = 0;
    list_mutex.lock();
    ret = __put_obj(obj);
    list_mutex.unlock();
    return ret;
}

struct obj_data* get_obj(int id)
{
    obj_data *obj;
    obj_data *temp = nullptr;
    list_mutex.lock();
    list_for_each_entry(obj, &obj_list, node) {
       if(obj->id == id) {
        obj->s_count++;
        temp = obj;
        break;
       }
    }
    list_mutex.unlock();
    return temp;
}

void func_read(int id)
{
    obj_data *obj;
    if((obj = get_obj(id)) == nullptr)
    {
        return;
    }
    obj->mtx.lock();
    cout << this_thread::get_id() << " Read Object ID: " << id << ", Value: " << obj->val << "[" << obj->s_count << "]" <<endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    obj->mtx.unlock();
    put_obj(obj);
}

void func_read_all()
{
    struct obj_data *obj;
    list_mutex.lock();
    restart:
    cout << this_thread::get_id() << " ReadAll: List-Size: "<< list_size(&obj_list) << endl;
    list_for_each_entry(obj, &obj_list, node) {
        obj->s_count++;
        list_mutex.unlock();
        obj->mtx.lock();
        cout << this_thread::get_id() << " ReadAll: Object ID: " << obj->id << ", Value: " << obj->val << "[" << obj->s_count << "]" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        obj->mtx.unlock();
        list_mutex.lock();
        if(__put_obj(obj))
        {
            cout << this_thread::get_id() << " [ReadAll Start] =======: Object ID: " << obj->id  << endl;
            goto restart;
        }
    }
    list_mutex.unlock();
}

void func_write(int id)
{
    obj_data *obj;
    if((obj = get_obj(id)) == nullptr)
    {
        return;
    }
    obj->mtx.lock();
    obj->val += "+";
    cout <<this_thread::get_id()<<  " Write Object ID: " << id << ", Value: " << obj->val << "[" << obj->s_count << "]" << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    obj->mtx.unlock();
    put_obj(obj);
    
}

void func_delete(int id)
{
    obj_data *obj;
    list_mutex.lock();
    list_for_each_entry(obj, &obj_list, node) 
    {
       if(obj->id == id) 
       {
         __put_obj(obj);
       }
    }
    list_mutex.unlock();
}

void t_read_func(int size)
{
    size_t s = 0;
    for(;;)
    {
        size_t id = rand() % size;
        func_read(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if(s++ >100)
        {
            break;
        }
    }
}
void t_write_func(int size)
{
    size_t s = 0;
    for(;;)
    {
        size_t id = rand() % size;
        func_write(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if(s++ >100)
        {
            break;
        }
    }
}
void t_delete_func(int size)
{
    for(;;)
    {
        size_t id = rand() % size;
        func_delete(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void t_readall_func()
{
    for(;;)
    {
        func_read_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main( int argc, char* argv[] )
{
    size_t num_objects = 100;
    for(size_t i = 0; i < num_objects; ++i) {
        obj_data *obj = new obj_data;
        obj->id = i;
        obj->s_count = 1;
        obj->val = "Object_" + to_string(i);
        INIT_LIST_HEAD(&obj->node);
        list_add(&obj->node, &obj_list);
    }
    thread t1 = thread(t_read_func, 100);
    thread t2 = thread(t_write_func, 100);
    thread t3 = thread(t_delete_func, 100);
    thread t4 = thread(t_readall_func);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
}
