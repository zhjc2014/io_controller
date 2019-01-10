#ifndef __LIST_H
#define __LIST_H

#include "stdint.h"

typedef struct list_head list_t;

struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
struct list_head name = LIST_HEAD_INIT(name)
    

/**
* list_entry ?get the struct for this entry
* @ptr:    the &struct list_head pointer.
* @type:    the type of the struct this is embedded in.
* @member:    the name of the list_struct within the struct.
*/
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
* list_for_each    -    iterate over a list
* @pos:    the &struct list_head to use as a loop counter.
* @head:    the head for your list.
*/
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
        pos = pos->next)
/**
* list_for_each_prev    -    iterate over a list backwards
* @pos:    the &struct list_head to use as a loop counter.
* @head:    the head for your list.
*/
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); \
        pos = pos->prev)

/**
* list_for_each_safe    -    iterate over a list safe against removal of list entry
* @pos:    the &struct list_head to use as a loop counter.
* @n:        another &struct list_head to use as temporary storage
* @head:    the head for your list.
*/
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head);\
        pos = n, n = pos->next)


/**
* list_for_each_entry    -    iterate over list of given type
* @pos:    the type * to use as a loop counter.
* @head:    the head for your list.
* @member:    the name of the list_struct within the struct.
*/
#define list_for_each_entry(pos, head, member)                \
    for (pos = list_entry((head)->next,typeof(*pos), member); \
        &pos->member != (head);                                  \
            pos = list_entry(pos->member.next, typeof(*pos), member))

/**
* list_for_each_entry_safe ?iterate over list of given type safe against removal of list entry
* @pos:    the type * to use as a loop counter.
* @n:        another type * to use as temporary storage
* @head:    the head for your list.
* @member:    the name of the list_struct within the struct.
*/
#define list_for_each_entry_safe(pos, n, head, member)            \
for (pos = list_entry((head)->next, typeof(*pos), member),    \
    n = list_entry(pos->member.next, typeof(*pos), member);    \
        &pos->member != (head);                     \
            pos = n, n = list_entry(n->member.next, typeof(*n), member))

#if 1

#define INLINE   __inline

static INLINE void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}
/*
* Insert a new entry between two known consecutive entries.
*
* This is only for internal list manipulation where we know
* the prev/next entries already!
*/
static INLINE void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}


static INLINE void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static INLINE void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}


static INLINE void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static INLINE void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void *) 0;
    entry->prev = (void *) 0;
}


static INLINE void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static INLINE void list_move(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}

static INLINE void list_move_tail(struct list_head *list,
struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

static INLINE int list_empty(struct list_head *head)
{
    return head->next == head;
}

static INLINE void __list_splice(struct list_head *list,
struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}
static INLINE void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}

static INLINE void list_splice_init(struct list_head *list,
                                    struct list_head *head)
{
    if (!list_empty(list)) {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}




#else
void INIT_LIST_HEAD(struct list_head *list);
void list_add(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head *new, struct list_head *head);
void list_del(struct list_head *entry);
void list_del_init(struct list_head *entry);
void list_move(struct list_head *list, struct list_head *head);
void list_move_tail(struct list_head *list,struct list_head *head);
int list_empty(struct list_head *head);
void list_splice(struct list_head *list, struct list_head *head);
void list_splice_init(struct list_head *list,struct list_head *head);


#endif

#endif
