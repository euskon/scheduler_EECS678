/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
  q->first = NULL;
  q->comp = comparer;
}


/**
  Insert the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
  Node* to_insert = (Node*)malloc(sizeof(Node));
  to_insert->next = NULL;
  to_insert->ptr = ptr;

	int current_index = 0;
  Node* previous_node;
  Node* current_node = q->first;

  while (1)
  {
    if (current_node == NULL)
    {
      if (current_index == 0)
      {
        q->first = to_insert;
      }
      else
      {
        previous_node->next = to_insert;
      }
      break;
    }
    previous_node = current_node;
    current_node = current_node->next;
    current_index++;
  }
  
  return current_index;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	return q->first;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if (q->first == NULL)
  {
    return NULL;
  }
  else
  {
    q->first = q->first->next;
    return q-> first;
  }
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
  int current_index = 0;
  Node* current_node = q->first;

  while (1)
  {
    if (current_node == NULL)
    {
      return NULL;
    }
    if (index == current_index)
    {
      return current_node->ptr;
    }
    current_node = current_node->next;
  }
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
  
	int removed_elements = 0;
  Node* current_node = q->first;
  Node* previous_node = NULL;

  while (1)
  {
    if (current_node == NULL)
    {
      return removed_elements;
    }
    if (*(current_node->ptr) == *ptr)
    {
      if (previous_node == NULL)
      {
        q->first = q->first->next;
        current_node = q->first;
        previous_node = NULL;
      }
      else
      {
        previous_node->next = current_node->next;
        current_node = current_node;
        previous_node = previous_node;
      }
      removed_elements++;
    }
    else
    {
      previous_node = current_node;
      current_node = current_node->next;
    }
    
  }
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	return 0;
}


/**
  Return the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return 0;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{

}
