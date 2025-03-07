#include <iostream>
#include <cstring>
#include <cstdlib>
#include <__list.h>

using namespace std;

typedef unsigned int bufctl_t;

#define PAGE_SIZE (1 << 10)
#define PAGE_MASK (PAGE_SIZE - 1)
#define MAX_PAGE 10
#define NCPUS 1

#define get_cpu_id() (0)
#define buf_ctl(s) ((unsigned int *)(s + 1))
#define get_slab(o) ((slab *)(((unsigned long)o) & (~PAGE_MASK)))
#define obj_to_index(o, s) ((kobj_t<T> *)o - (kobj_t<T> *)(s)->mem)
#define index_to_obj(n, s) ((kobj_t<T> *)s->mem + n)
#define BUF_END (~0U)

struct slab
{
    list_head node;
    int inuse;
    int free;
    void *mem;
};

struct kpage_t
{
    char data[PAGE_SIZE];
} __attribute__((aligned(PAGE_SIZE)));

template <typename T>
struct kobj_t
{
    T obj;
};

struct L3
{
    list_head full;
    list_head partial;
    list_head free;
    int nfree;
    int ntotal;
    int nlimit;
    int nslab;
};

struct cache_array
{
    int avail;
    int batchcount;
    void *entry[];
};

#define dump_slab(head, label, cachep)          \
    {                                           \
        slab *s;                                \
        list_for_each_entry(s, head, node)      \
        {                                       \
            cachep->__dump_slab_data(s, label); \
        }                                       \
    }

template <typename T>
struct KmemCache
{
    struct cache_array *array[NCPUS];
    int batchcount;
    int slabsize;
    int num;
    int nlimit;
    int npages;
    struct L3 l3;
    KmemCache(int __batchcount = 1) : batchcount(__batchcount), slabsize(0), num(0), nlimit(0), npages(0)
    {
        l3.nfree = 0;
        l3.nlimit = 0;
        l3.ntotal = 0;
        l3.nslab = 0;
        INIT_LIST_HEAD(&l3.full);
        INIT_LIST_HEAD(&l3.partial);
        INIT_LIST_HEAD(&l3.free);
        cache_array *ac = (cache_array *)malloc(sizeof(struct cache_array) + sizeof(void *) * batchcount);
        ac->avail = 0;
        ac->batchcount = batchcount;
        memset(ac->entry, 0x00, sizeof(void *) * batchcount);
        array[get_cpu_id()] = ac;
        __setupcache();
    }
    int get_num()
    {
        return num;
    }
    int get_batchcount()
    {
        return batchcount;
    }
    void print_size()
    {
        cout << " l3.free   .size = " << list_size(&l3.free) << endl;
        cout << " l3.partial.size = " << list_size(&l3.partial) << endl;
        cout << " l3.full   .size = " << list_size(&l3.full) << endl;
        cout << " nfree     .size = " << l3.nfree << "/" << l3.ntotal << endl;
        cout << " nslab     .size = " << l3.nslab << endl;
    }
    ~KmemCache()
    {
        cout << "~KmemCache" << '\n';
        cache_array *ac = array[get_cpu_id()];
        while (ac->avail)
        {
            void *obj = ac->entry[--ac->avail];
            put_obj(obj);
        }
        while (!list_empty(&l3.free))
        {
            auto *s = list_entry(l3.free.next, slab, node);
            list_del(&s->node);
            free(s);
        }
        while (!list_empty(&l3.partial))
        {
            auto *s = list_entry(l3.partial.next, slab, node);
            list_del(&s->node);
            free(s);
        }
        while (!list_empty(&l3.full))
        {
            auto *s = list_entry(l3.full.next, slab, node);
            list_del(&s->node);
            free(s);
        }
        for (auto *ac : array)
        {
            if (ac)
            {
                free(ac);
            }
        }
    }
    void __setupcache()
    {
        int n = 0;
        int obj_num = 0;
        int slab_page_size = 0;
        for (n = 1; n <= MAX_PAGE; n++)
        {
            slab_page_size = n * sizeof(kpage_t);
            obj_num = (slab_page_size - sizeof(slab)) / (sizeof(kobj_t<T>) + sizeof(bufctl_t));
            if (obj_num > 0)
            {
                num = obj_num;
                npages = n;
                break;
            }
        }
        num = obj_num;
        npages = n;
        nlimit = num + 3;
        slabsize = sizeof(slab) + num * sizeof(bufctl_t);
        printf("setup cache[num=%d, npages=%d, nlimit=%d]\n", num, npages, nlimit);
    }
    T *kmem_alloc()
    {
    retry:
        cache_array *ac = array[get_cpu_id()];
        if (ac->avail)
        {
            return (T *)ac->entry[--ac->avail];
        }
        __grow();
        goto retry;
    }
    void kmem_free(void *obj)
    {
    retry:
        cache_array *ac = array[get_cpu_id()];
        if (ac->avail < ac->batchcount)
        {
            ac->entry[ac->avail++] = obj;
            return;
        }
        else
        {
            __shrink();
            goto retry;
        }
    }
    slab *__alloc_slab()
    {
        slab *s = (slab *)aligned_alloc(PAGE_SIZE, PAGE_SIZE);
        for (int i = 0; i < num; i++)
        {
            buf_ctl(s)[i] = i + 1;
        }
        buf_ctl(s)[num - 1] = BUF_END;
        s->inuse = 0;
        s->free = 0;
        s->mem = (char *)s + slabsize;
        return s;
    }
    void __destroy_slab(slab *s)
    {
        s->free = 0;
        s->inuse = 0;
        INIT_LIST_HEAD(&s->node);
        free(s);
    }
    void *get_obj(slab *s)
    {
        auto *kobj = &((kobj_t<T> *)s->mem)[s->free];
        s->free = buf_ctl(s)[s->free];
        s->inuse++;
        l3.nfree--;
        return &kobj->obj;
    }
    void put_obj(void *obj)
    {
        slab *s = get_slab(obj);
        int offset = obj_to_index(obj, get_slab(obj));
        buf_ctl(s)[offset] = s->free;
        s->free = offset;
        s->inuse--;
        l3.nfree++;
    }
    void dump()
    {
        dump_slab(&l3.full, "full", this);
        dump_slab(&l3.partial, "partial", this);
        dump_slab(&l3.free, "free", this);
    }
    void __dump_slab_data(slab *s, const char *label)
    {
        T *obj = (T *)s->mem;
        for (int i = 0; i < num; i++)
        {
            printf("%s.%d: {id=%d, val=%d, name=%s}\n", label, i, obj->id, obj->val, obj->name);
            obj++;
        }
    }
    int __grow()
    {
        slab *s = nullptr;
        int __batchcount = batchcount;
    retry:
        list_head *li = l3.partial.next;
        if (li == &l3.partial)
        {
            li = l3.free.next;
            if (li == &l3.free)
            {
                s = __alloc_slab();
                list_add(&s->node, &l3.free);
                l3.nfree += num;
                l3.ntotal += num;
                l3.nslab++;
                li = &s->node;
            }
        }
        list_del(li);
        s = list_entry(li, slab, node);
        cache_array *ac = array[get_cpu_id()];
        while (__batchcount-- && s->inuse < num)
        {
            ac->entry[ac->avail++] = get_obj(s);
        }
        if (s->inuse == num)
        {
            list_add(&s->node, &l3.full);
        }
        else
        {
            list_add(&s->node, &l3.partial);
        }
        return 0;
    }
    void __shrink()
    {
        cache_array *ac = array[get_cpu_id()];
        for (int i = 0; i < batchcount; i++)
        {
            void *obj = ac->entry[i];
            slab *s = get_slab(obj);
            list_del(&s->node);
            put_obj(obj);
            if (s->inuse == 0)
            {
                if (l3.nfree > l3.nlimit)
                {
                    __destroy_slab(s);
                    l3.nfree -= num;
                    l3.ntotal -= num;
                    l3.nslab--;
                }
                else
                {
                    list_add(&s->node, &l3.free);
                }
            }
            else
            {
                list_add(&s->node, &l3.partial);
            }
        }
        ac->avail -= batchcount;
        memmove(ac->entry, ac->entry[batchcount], sizeof(void *) * (ac->avail));
    }
};