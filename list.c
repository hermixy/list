/*William Johnson
 *
 *A two-way linked list ADT along with some sorting features
*/
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "list.h"        /* defines public functions for list ADT */

/* definitions for private constants used in list.c only */

#define SORTED_LIST   -123456
#define UNSORTED_LIST -621354
//static int (*comp_proc)(void *, void *);

/* prototypes for private functions used in list.c only */
void list_debug_validate(list_t *L);
void insert_sort(list_t *list_ptr);
void recursive_select_sort(list_t *, Iterator, Iterator);
void iter_select_sort(list_t *, Iterator, Iterator);
void merge_sort(list_t *);
list_t* split_list(list_t *);
void merge(list_t *, list_t *);
Iterator find_max(list_t *, Iterator, Iterator);

/* Allocates a new, empty list 
 *
 * By convention, the list is initially assumed to be sorted.  The field sorted
 * can only take values SORTED_LIST or UNSORTED_LIST
 *
 * The inital empty list must have
 * 1.  a dummy head list_node_t
 * 2.  a dummy tail list_node_t
 * 3.  current_list_size = 0
 * 4.  list_sorted_state = SORTED_LIST
 *
 * Use list_decontruct to remove and deallocate all elements on a list,
 * the dummy head and tail, and the header block.
 */
list_t * list_construct(void)
{
    list_t *L;

    L = (list_t *) malloc(sizeof(list_t));

    //Malloc Head and Tail nodes
    L->head = (list_node_t *) malloc(sizeof(list_node_t));
    L->tail = (list_node_t *) malloc(sizeof(list_node_t));

    //Link The head and Tail
    L->head->next = L->tail;
    L->tail->prev = L->head;

    //Set unessecary node ptrs to NULL
    L->head->prev = NULL;
    L->head->data_ptr = NULL;
    L->tail->next = NULL;
    L->tail->data_ptr = NULL;

    //Set list size to zero
    L->current_list_size = 0;
    
    //Set List sorted State
    L->list_sorted_state = SORTED_LIST;
    
    //Set comparison function = NULL need to call set_comp
    L->comp_proc = NULL;

    /* the last line of this function must call validate */
    //list_debug_validate(L);
    return L;
}

/* Sets the pointer to the comparison function
 * Must be set non NULL before any list or sorting functions
 * are used
*/
void set_comp(list_t *list_ptr, comparer comp_proc)
{
    if(list_ptr == NULL)
	{
		printf("List must be constructed\n");
		return;
	}
    list_ptr->comp_proc = comp_proc;
    assert(list_ptr->comp_proc != NULL);
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list, the dummy head and tail, and the header 
 * block.
 */
void list_destruct(list_t *list_ptr)
{
    list_node_t  *Current = NULL, *Next = NULL;
    
    /* the first line must validate the list */
    //list_debug_validate(list_ptr);

    Current = list_ptr->head;    
        
    while(Current != list_ptr->tail)
    {
        Next = Current->next;
        if(Current->data_ptr != NULL)
        {
            free(Current->data_ptr);
        }
        free(Current);
        Current = Next;
    }
        free(Current);
        free(list_ptr);
}

/* Return an Iterator that points to the first element in the list.  If the
 * list is empty the pointer that is returned is equal to the dummy tail
 * list_node_t.
 */
list_node_t * list_iter_first(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    //list_debug_validate(list_ptr);
    return list_ptr->head->next;

}

/* Return an Iterator that always points to the dummy tail list_node_t. 
 */
list_node_t * list_iter_tail(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    //list_debug_validate(list_ptr);
    return list_ptr->tail;
}

/* Advance the Iterator to the next item in the list.  The Iterator must not be
 * pointing to the dummy tail list_node_t.
 */
list_node_t * list_iter_next(list_node_t * idx_ptr)
{
    assert(idx_ptr != NULL && idx_ptr->next != NULL);
    return idx_ptr->next;
}

/* Obtains a pointer to an element stored in the specified list, at the
 * specified Iterator position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_first, or
 *          list_iter_next.  
 *
 * return value: pointer to the data_t element found in the list at the 
 * iterator position. A value NULL is returned if 
 *     1.  the list is empty 
 *     2.  the idx_ptr points to the dummy head or dummy tail
 */
void * list_access(list_t *list_ptr, list_node_t * idx_ptr)
{
    assert(list_ptr != NULL);
    assert(idx_ptr != NULL);

    /* debugging function to verify that the structure of the list is valid */
    //list_debug_validate(list_ptr);

    if(list_ptr->current_list_size == 0)
    {
        return NULL;
    }
    else if(idx_ptr == list_ptr->head || idx_ptr == list_ptr->tail)
    {
        return NULL;
    }
    else
    {
        return idx_ptr->data_ptr;
    }
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 *           Note: uses the compare function defined in datatypes.h
 *
 * The function returns an Iterator pointer to the list_node_t that contains 
 * the first matching element if a match if found.  If a match is not found 
 * the return value is NULL.
 */
list_node_t * list_elem_find(list_t *list_ptr, void *elem_ptr)
{
    Iterator elem_node = NULL, current;    
    
    current = list_iter_first(list_ptr);
    while(current != list_iter_tail(list_ptr) && list_ptr->comp_proc(elem_ptr, current->data_ptr) != 0)
    {
        current = list_iter_next(current);
    }
    
    if(current != list_iter_tail(list_ptr))
    {
        elem_node = current;
    }    
    
    //list_debug_validate(list_ptr);

    /* fix the return value */
    return elem_node;
}

/* Inserts the data element into the list in front of the list_node_t 
 * position.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * idx_ptr: pointer to a list_node_t.  The element is to be inserted as a 
 *          member in the list at the position that is immediately in front 
 *          of the position of the provided Iterator pointer.
 *
 * If idx_ptr is set using 
 *    -- list_iter_first, then the new element becomes the first item in 
 *       the list.
 *    -- list_iter_tail, then the new element becomes the last item in 
 *       the list.
 *    -- for any other idx_ptr, the new element is insert before the 
 *       Iterator
 *
 * For example    packet = (data_t *)malloc(sizeof(data_t);, to insert at the tail of the list do
 *      list_insert(mylist, myelem, list_iter_tail(mylist))
 * to insert at the front of the list do
 *      list_insert(mylist, myelem, list_iter_front(mylist))
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->list_sorted_state must be equal 
 * to UNSORTED_LIST.
 */
void list_insert(list_t *list_ptr, void *elem_ptr, list_node_t * idx_ptr)
{
    list_node_t * new_node;

    assert(list_ptr != NULL);

    //Malloc node memory
    new_node = (list_node_t *) malloc(sizeof(list_node_t));

    //Link node to data element
    new_node->data_ptr = elem_ptr;

    //Link node into the list
    new_node->next = idx_ptr;
    new_node->prev = idx_ptr->prev;
    new_node->prev->next = new_node;
    idx_ptr->prev = new_node;
    
    //Increment the List size
    list_ptr->current_list_size++;

    /* the last two lines of this function must be the following */
    if (list_ptr->list_sorted_state == SORTED_LIST) 
	list_ptr->list_sorted_state = UNSORTED_LIST;
    //list_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the compare_proc.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call list_insert_sorted, this subroutine
 * must generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the list head than B
 *   -1: B should be closer to the list head than A
 *    0: A and B are equal in rank
 *
 * Note: if the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed after all the
 * elements of equal rank that are already in the list.
 */
void list_insert_sorted(list_t *list_ptr, void *elem_ptr)
{
    Iterator node;
    list_node_t *new;  
    
    assert(list_ptr != NULL);
    assert(list_ptr->list_sorted_state == SORTED_LIST);
    assert(list_ptr->comp_proc != NULL);

    /* insert your code here */
    
    //Set node to first list node
    node = list_iter_first(list_ptr);
    while(node != list_iter_tail(list_ptr) && list_ptr->comp_proc(elem_ptr, node->data_ptr) != 1)
    {
        node = list_iter_next(node);
    }
    
    //malloc a new list node to put in list
    new = (list_node_t *) malloc(sizeof(list_node_t));
    
    //Link the new node into the list
    new->next = node;
    node->prev->next = new;
    new->prev = node->prev;
    node->prev = new;

    //Link the Data ptr to the new node
    new->data_ptr = elem_ptr;  

    //Increment List size
    list_ptr->current_list_size++;  

   
    /* the last line of this function must be the following */
    //list_debug_validate(list_ptr);
}

/* Removes the element from the specified list that is found at the list_node_t  index.  A pointer to the data element is returned.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_first, or
 *          list_iter_next.  
 *
 * Note it is easy to remove the element at the front of the list using
 *     list_remove(mylist, list_iter_front(mylist))
 * However, removing the element at the end of the list is not easy since
 * there is no way to get the pointer to the last element in the list.  
 * Notice that list_iter_tail return a pointer to the dummy tail, not the 
 * last element in the list.
 */
void * list_remove(list_t *list_ptr, list_node_t * idx_ptr)
{
    void *data;

    if (idx_ptr == NULL)
	return NULL;
    assert(idx_ptr != list_ptr->head && idx_ptr != list_ptr->tail);
    assert(idx_ptr->data_ptr != NULL);
    assert(list_ptr->current_list_size > 0);

    data = idx_ptr->data_ptr;
    
    //Remove node from the list and recconect the links
    idx_ptr->next->prev = idx_ptr->prev;
    idx_ptr->prev->next = idx_ptr->next;

    //Free node memory
    free(idx_ptr);

    //Decrement the List size
    list_ptr->current_list_size--;

    /* the last line should verify the list is valid after the remove */
    //list_debug_validate(list_ptr);

    return data;
}

void list_sort(list_t *list_ptr)
{
    
    merge_sort(list_ptr);
    
    /* No longer supports selection of sorting algorithm instead defaults to merge sort
    if(sort_type == 1)
    {
        insert_sort(list_ptr);
    }
    else if(sort_type == 2)
    {
        recursive_select_sort(list_ptr, list_ptr->head->next, list_ptr->tail);
    }
    else if(sort_type == 3)
    {
        iter_select_sort(list_ptr, list_ptr->head->next, list_ptr->tail);
    }
    else if(sort_type == 4)
    {
        merge_sort(list_ptr);
    }
    */
    list_ptr->list_sorted_state = SORTED_LIST;
    list_debug_validate(list_ptr);
}

void insert_sort(list_t *list_ptr)
{
    list_t *list_2;
    
    list_2 = list_construct();
    set_comp(list_2, list_ptr->comp_proc);
    
    do
    {
        list_insert_sorted(list_2, list_remove(list_ptr, list_ptr->head->next));
    }
    while(list_ptr->current_list_size != 0);
    
    list_ptr->head->next = list_2->head->next;
    list_2->head->next->prev = list_ptr->head;
    list_ptr->tail->prev = list_2->tail->prev;
    list_2->tail->prev->next = list_ptr->tail;
    list_ptr->current_list_size = list_2->current_list_size;
    
    free(list_2->tail);
    free(list_2->head);
    free(list_2);
}

void recursive_select_sort(list_t *list_ptr, Iterator min, Iterator max)
{
    Iterator maxpos;
    Iterator hold_min;
    void *hold;
    
    if(min != max->prev)
    {
        maxpos = find_max(list_ptr, min, max->prev);
        if(maxpos != min)
        {
            hold_min = min;
            min = min->prev;
            hold = list_remove(list_ptr, hold_min);
            list_insert(list_ptr, hold, maxpos);
            hold = list_remove(list_ptr, maxpos);
            min = min->next;
            list_insert(list_ptr, hold, min);
        }
        else
        {
            min = min->next;
        }
        
        recursive_select_sort(list_ptr, min, max);
    }
}

void iter_select_sort(list_t *list_ptr, Iterator min, Iterator max)
{
    Iterator maxpos;
    Iterator hold_min;
    void *hold;
    
    while(min != max->prev)
    {
        maxpos = find_max(list_ptr, min, max->prev);
        
        if(maxpos != min)
        {
            hold_min = min;
            min = min->prev;
            hold = list_remove(list_ptr, hold_min);
            list_insert(list_ptr, hold, maxpos);
            hold = list_remove(list_ptr, maxpos);
            min = min->next;
            list_insert(list_ptr, hold, min);
        }
        else
        {
            min = min->next;
        }
        
    }
}

void merge_sort(list_t *list_ptr)
{
    list_t *list_r;    
    
    if(list_ptr->current_list_size > 1)
    {
        list_r = split_list(list_ptr);
        merge_sort(list_ptr);
        merge_sort(list_r);
        merge(list_ptr, list_r);
    }
}

list_t* split_list(list_t *list_ptr)
{
    list_t *list_r;
    int list_size;
    int i;
    Iterator current;
    
    list_r = list_construct();
    set_comp(list_r, list_ptr->comp_proc);
    
    current = list_ptr->head->next;
    list_size = list_ptr->current_list_size;
    
    for(i = 0; i < list_size/2; i++)
    {
        current = current->next;
    }
    
    list_r->head->next = current;
    list_r->tail->prev = list_ptr->tail->prev;
    list_ptr->tail->prev->next = list_r->tail;
    current->prev->next = list_ptr->tail;
    list_ptr->tail->prev = current->prev;
    current->prev = list_r->head;
    
    list_r->current_list_size = list_ptr->current_list_size - list_size/2;
    list_ptr->current_list_size = list_size/2;
    
    //list_debug_validate(list_ptr);
    //list_debug_validate(list_r);
    
    return list_r;
}

void merge(list_t *list_l, list_t *list_r)
{
    Iterator idx_l;
    Iterator idx_r;
    
    idx_r = list_r->head->next;
    idx_l = list_l->head->next;
    
    while(idx_r != list_r->tail && idx_l != list_l->tail)
    {
        while(idx_r != list_r->tail && idx_l != list_l->tail && list_l->comp_proc(idx_l->data_ptr, idx_r->data_ptr) != -1)
        {
            idx_l = idx_l->next;
        }
        idx_l->prev->next = idx_r;
        idx_r->prev = idx_l->prev;
        
        if(idx_l == list_l->tail)
        {
            list_l->tail->prev = list_r->tail->prev;
            list_r->tail->prev->next = list_l->tail;
        }
        else
        {
            while(idx_r != list_r->tail && idx_l != list_l->tail && list_l->comp_proc(idx_r->data_ptr, idx_l->data_ptr) != -1)
            {
             idx_r = idx_r->next;
            }
            idx_r->prev->next = idx_l;
            idx_l->prev = idx_r->prev;
        }
    }
    
    list_l->current_list_size = list_l->current_list_size + list_r->current_list_size;
    
    free(list_r->tail);
    free(list_r->head);
    free(list_r);
}
        

Iterator find_max(list_t *list_ptr, Iterator min, Iterator max)
{
    Iterator i = min;
    Iterator j = min;
    //int k = 0;
       
    do{
        /*printf("K: %d\n", k);
        k++;*/
        i = i->next;
        if(list_ptr->comp_proc(i->data_ptr, j->data_ptr) == 1)
        {
            j = i;
        }
    }while(i != max);
    
    return j;
}
/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains. 
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 */
int list_size(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->current_list_size >= 0);
    return list_ptr->current_list_size;
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void list_debug_validate(list_t *L)
{
    list_node_t *N;
    int count = 0;
    assert(L != NULL && L->head != NULL && L->tail != NULL);
    assert(L->head != L->tail);
    assert(L->head->prev == NULL && L->tail->next == NULL);
    assert(L->head->data_ptr == NULL && L->tail->data_ptr == NULL);
    assert(L->head->next != NULL && L->tail->prev != NULL);
    assert(L->list_sorted_state == SORTED_LIST || L->list_sorted_state == UNSORTED_LIST);

    N = L->head->next;
    assert(N->prev == L->head);
    while (N != L->tail) {
	assert(N->data_ptr != NULL);
	assert(N->next->prev == N);
	count++;
	N = N->next;
    }
    assert(count == L->current_list_size);
    if (L->list_sorted_state == SORTED_LIST && L->current_list_size > 0) {
        N = L->head->next;
        while (N->next != L->tail) {
            assert(L->comp_proc(N->data_ptr, N->next->data_ptr) != -1);
            N = N->next;
        }
    }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

