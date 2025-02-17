#include <stddef.h>
#define container_of(ptr, type, member)                        \
    ({                                                         \
        __typeof__(((type *)0)->member) *__mptr = ptr;         \
        ((type *)((char *)(__mptr) - offsetof(type, member))); \
    })

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define INIT_LIST_HEAD(list) \
    {                        \
        (list)->next = list; \
        (list)->prev = list; \
    }

struct list_head
{
    struct list_head *next;
    struct list_head *prev;
};

static inline bool list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline int list_size(struct list_head *head)
{
    int n;
    list_head *pos;
    for (pos = head->next, n = 0; pos != head; pos = pos->next, n++)
    {
    }
    return n;
}

static inline void list_del(struct list_head *entry)
{
    struct list_head *prev = entry->prev;
    struct list_head *next = entry->next;
    prev->next = next;
    next->prev = prev;
}

static inline void list_add(struct list_head *entry, struct list_head *head)
{
    entry->next = head;
    entry->prev = head->prev;
    entry->next->prev = entry;
    entry->prev->next = entry;
}

#define list_for_each(pos, head) for (pos = (head)->next; pos != head; pos = pos->next)
#define list_for_each_entry(pos, head, member) for (pos = list_entry((head)->next, __typeof__(*pos), member); &pos->member != head; pos = list_entry(pos->member.next, __typeof__(*pos), member))