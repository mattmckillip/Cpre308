/**
 * @file      queue.c
 * @author    Jeramie Vens
 * @date      2015-02-13: Created
 * @date      2015-02-14: Last updated
 * @brief     A FIFO/LIFO queue list for any data type
 * @copyright MIT License (c) 2015
 */
 
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include "queue.h"

/// A node in a queue list
typedef struct QUEUE_NODE_STRUCT queue_node_t;
struct QUEUE_NODE_STRUCT
{
	/// the next node in the list
	queue_node_t *next;
	/// the previous node in the list
	queue_node_t *prev;
	/// the data in this node
	void* data;
};

/// A internal queue list object
struct QUEUE_STRUCT
{
	/// the head of the list
	queue_node_t *head;
	/// the tail of the list
	queue_node_t *tail;
	/// a lock for the list
	pthread_mutex_t lock;
	/// the number of elements in the list
	sem_t count;
};

/**
 * @brief     Create a new queue list
 * @return    A linked list object which can be passed to queue_push() and queue_pop() functions.
 */
queue_t queue_create(void)
{
	queue_t list;
	// allocate the linked list
	if ((list = (queue_t)malloc(sizeof(struct QUEUE_STRUCT))))
	{
		// set head and tail to nothing
		list->head = NULL;	
		list->tail = NULL;
		
		// initialize the mutex
		if(pthread_mutex_init(&list->lock, NULL))
		{
			fprintf(stderr, "%s: Failed to initialize list mutex\n", __func__);
			exit(-1);
		}
		
		// initialize the count semaphore
		if(sem_init(&list->count, 0, 0))
		{
			fprintf(stderr, "%s: Failed to initialize list count semaphore\n", __func__);
			exit(-1);
		}
		// return the new list
		pthread_mutex_unlock(&list->lock);
		return list;
	}
	else
	{	
		fprintf(stderr, "%s: failed to allocate new linked list\n", __func__);
		exit(-1);
	}	
}

/**
 * @brief     Destroy the list cleaning up any resources it holds.
 * @param     list
 *                 The list to be destroyed
 * @return    NULL
 */
queue_t queue_destroy(queue_t list)
{
	// assert this is a valid list
	assert(list > 0);
	
	queue_node_t* node;
	
	// destroy the mutex and semaphore
	pthread_mutex_destroy(&list->lock);
	sem_destroy(&list->count);
	
	// for each node in the list
	for(node = list->head; node; node = list->head)
	{
		// move the head forward
		list->head = node->next;
		// free the data and node
		free(node->data);
		free(node);
	}
	
	// free the list
	free(list);
	return NULL;
}


/**
 * @brief     Push new data onto the list
 * @param     list
 *                 The list to push the data into
 * @param     data
 *                 The data to push onto the list
 * @param     end
 *                 Which end of the list to push the data to
 */
void queue_push(queue_t list, void* data, queue_ends_t end)
{
	// assert this is a list
	assert(list > 0);
	
	// allocate a new node
	queue_node_t *node = (queue_node_t*)malloc(sizeof(queue_node_t));
	if(!node)
	{
		fprintf(stderr, "%s: failed to allocate new linked list node\n", __func__);
		exit(-1);
	}
	
	// save the data to the node
	node->data = data;
	node->next = NULL;
	node->prev = NULL;
	// lock the list
	pthread_mutex_lock(&list->lock);

	// if this is an empty list save the node to the head and tail and move on
	if(!list->head)
	{
		list->head = node;
		list->tail = node;

	}
	else
	{
		switch(end)
		{
			case QUEUE_HEAD:
				// push this node to the head and move the head
				node->next = list->head;
				list->head->prev = node;				
				list->head = node;
				
				break;

			case QUEUE_TAIL:
				// push this node to the tail and move the tail
				node->prev = list->tail;
				list->tail->next = node;				
				list->tail = node;				
				break;

			default:
				fprintf(stderr, "%s: unknown parameter passed\n", __func__);
				exit(0);
		}
	}
	
	// increment the list count
	sem_post(&list->count);	
	
	// unlock the list
	pthread_mutex_unlock(&list->lock);
	

}


/**
 * @brief     Pull an element out of the queue list and return the data stored in the element
 * This function will pull an element out of the list and return a pointer to the data it
 * contained.  If the list is empty this function will will block until data has been pushed
 * to the list and then will return the data.  If queue_release_threads() is called this will and
 * the list is empty this will return NULL.
 *
 * @param     list
 *                 The list to pull the element out of
 * @param     end
 *                 Which end of the list to pull the element from
 * @return    A pointer the data passed to queue_push() or NULL if the list is empty and 
 *            queue_release_threads() has been called.
 */
void* queue_pop(queue_t list, queue_ends_t end)
{
	
	// assert the list is valid
	assert(list > 0);
	queue_node_t* node;
	void* data;
	

	// wait for the list to contain something
	sem_wait(&list->count);

	// lock the list
	pthread_mutex_lock(&list->lock);

	switch(end)
	{
		case QUEUE_HEAD:
			// get the head of the list
			node = list->head;
			
			// if this is a valid node
			if(node)
			{	
				// move the head and fix links
				list->head = list->head->next;
				if(list->head)
				{
					list->head->prev = NULL;
				}
				else
				{
					list->tail = NULL;
				}
			}
			break;
		case QUEUE_TAIL:
			// get the tail of the list
			node = list->tail;
			
			// if this is a valid node
			if(node)
			{
				// move the tail and fix the links
				list->tail = list->tail->prev;
				if(list->tail)
				{
					list->tail->next = NULL;
				}
				else
				{
					list->head = NULL;
				}
			}
			break;
		default:
			fprintf(stderr, "%s: unknown parameter passed\n", __func__);
			exit(0);
	}
	
	// unlock the list
	pthread_mutex_unlock(&list->lock);
	
	// if this is a valid node
	if(node)
	{
		// get the data
		data = node->data;
		
		// free the node
		free(node);
	}
	else
	{
		
		// we must be releasing all threads with ll_release_threads()
		// return NULL
		data = NULL;
	}
	
	// return the data
	return data;
}


/**
 * @brief     Causes any threads waiting for data from a queue_pop() call to receive NULL from that call.
 * Normally when a thread calls queue_pop() and the list is empty the call will block.  After calling
 * this function the calls to queue_pop() will return NULL when the list is empty.
 * @param     list
 *                The list to release waiting threads of
 * @param     number
 *                The number of threads potentially waiting on the list.
 */
void queue_release_threads(queue_t list, int number)
{
	// assert this is a list
	assert(list > 0);
	
	int i;
	// for each thread
	for(i=0;i<number;i++)
	{
		// increment the list count
		if(sem_post(&list->count))
		{
			fprintf(stderr, "%s: failed to increment count semaphore\n", __func__);
			exit (-1);
		}
	}
	return;
}


