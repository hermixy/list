/* list.h 
 *
 * Public functions for two-way linked list
 *
 */
#ifndef _MYLIST_H_
#define _MYLIST_H_

typedef int (*comparer)(void *, void *);

typedef struct list_node_tag {
    /* private members for list.c only */
    struct list_node_tag *prev;
    struct list_node_tag *next;
    void *data_ptr;
} list_node_t;

typedef struct list_tag {
    /* private members for list.c only */
    list_node_t *head;
    list_node_t *tail;
    int current_list_size;
    int list_sorted_state;
    comparer comp_proc;
} list_t;

/* public definition of pointer into linked list */
typedef list_node_t * Iterator;
typedef list_t * List;

/* public prototype definitions for list.c */

/* build and cleanup lists */
void set_comp(List, comparer);
List list_construct(void);
void list_destruct(List list_ptr);

/* iterators into positions in the list */
Iterator list_iter_first(List list_ptr);
Iterator list_iter_tail(List list_ptr);
Iterator list_iter_next(Iterator idx_ptr);

void * list_access(List list_ptr, Iterator idx_ptr);
Iterator list_elem_find(List list_ptr, void *elem_ptr);

void list_insert(List list_ptr, void *elem_ptr, Iterator idx_ptr);
void list_insert_sorted(List list_ptr, void *elem_ptr);

void * list_remove(List list_ptr, Iterator idx_ptr);
void list_sort(List);

int list_size(List list_ptr);
#endif

/* commands for vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
