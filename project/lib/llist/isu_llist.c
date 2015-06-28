/**
 * @addtogroup llist
 * @{
 * @file      isu_llist.c
 * @author    Jeramie Vens
 * @date      3/12/15 - Created
 * @date      3/13/15 - Documentation created
 */
 
#include <stdlib.h>
#include <stdio.h>
#include "isu_llist.h"
#include "common/isu_error.h"
#include "common/isu_types.h"

#ifdef PRINT_LEVEL
#undef PRINT_LEVEL
#endif
#define PRINT_LEVEL 4

/**
 * A private node of the linked list
 */
typedef struct ISU_LLIST_NODE_STRUCT *isu_llist_node_t;
struct ISU_LLIST_NODE_STRUCT
{
	/// The next node in the list
	struct ISU_LLIST_NODE_STRUCT *next;
	/// The previous node in the list
	struct ISU_LLIST_NODE_STRUCT *prev;
	/// The data of this node
	void *data;
};

struct ISU_LLIST_STRUCT
{
	/// The head of the list
	isu_llist_node_t head;
	/// The tail of the list
	isu_llist_node_t tail;
	/// The current itterator possition in the list
	isu_llist_node_t ittr;
	/// The direction the itterator is traveleing in the list
	isu_llist_end_t ittr_dir;
	/// Flag if the last isu_llist_remove() removed the head or the tail
	int remove_flag;
	/// The current number of elements in the list
	int count;
};

// create new linked list object
isu_llist_t isu_llist_create(void)
{
	isu_llist_t list;
	list = calloc(1, sizeof(struct ISU_LLIST_STRUCT));
	if ( list == NULL )
	{
		isu_print(PRINT_ERROR, "malloc returned NULL");
		return NULL;
	}
	isu_print(PRINT_DEBUG, "created new linked list");
	return list;
}


void isu_llist_destroy(isu_llist_t list)
{
	void* i;
	do
	{
		i = isu_llist_pop(list, ISU_LLIST_HEAD);
	}while(i);
	isu_print(PRINT_DEBUG, "destroyed linked list");
	free(list);
}

int isu_llist_count(isu_llist_t list)
{
	if(!list)
		return -1;
	return list->count;
}

isu_return_t isu_llist_push(isu_llist_t list, void* data, isu_llist_end_t end)
{
	isu_llist_node_t node = calloc(1, sizeof(struct ISU_LLIST_NODE_STRUCT));
	if ( node == NULL )
	{
		isu_print(PRINT_ERROR, "malloc returned NULL");
		SET_ERRNO(ISU_RET_EMEM);
		return ISU_RET_EMEM;
	}
	node->data = data;
	
	// if this is an empty list save the node to the head and tail
	if(!list->head)
	{
		list->head = node;
		list->tail = node;
	}
	else
	{
		switch ( end )
		{
			case ISU_LLIST_HEAD:
				node->next = list->head;
				list->head->prev = node;
				list->head = node;
				break;
			case ISU_LLIST_TAIL:
				node->prev = list->tail;
				list->tail->next = node;
				list->tail = node;
				break;
			default:
				isu_print(PRINT_ERROR, "Unknown list end");
				SET_ERRNO(ISU_RET_EPARAM);
				return ISU_RET_EPARAM;
				break;
		}
	}
	list->count ++;
	isu_print(PRINT_DEBUG, "succesfully pushed data to linked list");
	return ISU_RET_SUCCESS;
}

void* isu_llist_pop(isu_llist_t list, isu_llist_end_t end)
{
	isu_llist_node_t node;
	void *data;
	
	switch ( end )
	{
		case ISU_LLIST_HEAD:
			// get the head of the list
			node = list->head;
			
			// if this is a valid node
			if ( node )
			{
				// move the head forward and fix links
				list->head = list->head->next;
				if ( list->head ) 
					list->head->prev = NULL;
				else
					list->tail = NULL;
			}
			break;
		case ISU_LLIST_TAIL:
			// get the tail of the list
			node = list->tail;
			
			// if this is a valid node
			if ( node )
			{
				// move the tail and fix the links
				list->tail = list->tail->prev;
				if ( list->tail )
					list->tail->next = NULL;
				else
					list->head = NULL;
			}
			break;
		default:
			isu_print(PRINT_ERROR, "Unknown list end");
			SET_ERRNO(ISU_RET_EPARAM);
			return;
	}
	
	// if this is a valid node
	if ( node )
	{
		list->count --;
		data = node->data;
		free (node);
	}
	else
	{
		data = NULL;
	}
	
	isu_print(PRINT_DEBUG, "succesfully popped data from linked list");
	return data;	
}

void* isu_llist_ittr_start(isu_llist_t list, isu_llist_end_t end)
{
	switch ( end )
	{
		case ISU_LLIST_HEAD:
			list->ittr = list->head;
			list->ittr_dir = end;
			break;
		case ISU_LLIST_TAIL:
			list->ittr = list->tail;
			list->ittr_dir = end;
			break;
		default:
			SET_ERRNO(ISU_RET_EPARAM);
			isu_print(PRINT_ERROR, "Unknown list end");
			return NULL;
	}
	
	list->remove_flag = 0;
	
	isu_print(PRINT_DEBUG, "succesfully started linked list itterator");
	
	if ( list->ittr )
		return list->ittr->data;
	else
		return NULL;
}

void* isu_llist_ittr_next(isu_llist_t list)
{
	if ( list->remove_flag )
	{
		list->remove_flag = 0;
		switch ( list->ittr_dir )
		{
			case ISU_LLIST_HEAD:
				list->ittr = list->head;
				break;
			case ISU_LLIST_TAIL:
				list->ittr = list->tail;
				break;
		}
	}
	else if ( list->ittr == NULL)
	{
		isu_print(PRINT_WARN, "List itterator not started");
		SET_ERRNO(ISU_RET_EUSAGE);
		return NULL;
	}
	else
	{
		switch ( list->ittr_dir )
		{
			case ISU_LLIST_HEAD:
				list->ittr = list->ittr->next;
				break;
			case ISU_LLIST_TAIL:
				list->ittr = list->ittr->prev;
				break;
		}
	}
	
	isu_print(PRINT_DEBUG, "Succesfully got next element of linked list");
	if ( list->ittr )
		return list->ittr->data;
	else
		return NULL;
}

/**
 * @brief     Insert a new element into the list at the location of the itterator.
 * @private @memberof isu_llist_t
 * @param     list
 *                 List to add the element to
 * @param     data
 *                 Data to add to the list
 * @param     end
 *                 Which side of the itterator to add the element to.  If
 *                 ISU_LLIST_HEAD add between the itterator and the tail.
 *                 If ISU_LLIST_TAIL add between head and itterator.
 * @return    ISU_RET_EMEM:
 *                 Malloc call failed.
 * @return    ISU_RET_EUSAGE:
 *                 If the itterattor has not been started before this call.
 * @return    ISU_RET_SUCCESS:
 *                 Data was successfully added.
 */ 
isu_return_t isu_llist_insert(isu_llist_t list, void *data, isu_llist_end_t end)
{
	isu_llist_node_t node;
	
	if ( list->ittr == NULL)
	{
		isu_print(PRINT_ERROR, "List itterator not started");
		SET_ERRNO(ISU_RET_EUSAGE);
		return ISU_RET_EUSAGE;
	}
	
	node = calloc(1, sizeof(struct ISU_LLIST_NODE_STRUCT));
	if ( node == NULL )
	{
		isu_print(PRINT_ERROR, "malloc returned NULL");
		SET_ERRNO(ISU_RET_EMEM);
		return ISU_RET_EMEM;
	}
	node->data = data;
	
	switch ( end )
	{
		case ISU_LLIST_HEAD:
			node->next = list->ittr->next;
			node->prev = list->ittr;
			if ( list->ittr->next )
				list->ittr->next->prev = node;
			else
				list->tail = node;
			list->ittr->next = node;
			break;
		case ISU_LLIST_TAIL:
			node->next = list->ittr;
			node->prev = list->ittr->prev;
			if ( list->ittr->prev )
				list->ittr->prev->next = node;
			else
				list->head = node;
			list->ittr->prev = node;
			break;
	}
	
	list->count ++;
	isu_print(PRINT_DEBUG, "Succesfully inserted element to linked list");
	return ISU_RET_SUCCESS;
}


isu_return_t isu_llist_insert_after(isu_llist_t list, void *data)
{
	return isu_llist_insert(list, data, list->ittr_dir);
}


isu_return_t isu_llist_insert_before(isu_llist_t list, void *data)
{
	return isu_llist_insert(list, data, list->ittr_dir == ISU_LLIST_HEAD ? ISU_LLIST_TAIL : ISU_LLIST_HEAD);
}


void* isu_llist_remove(isu_llist_t list)
{
	isu_llist_node_t node;
	void* data;
	
	if ( list->ittr == NULL)
	{
		isu_print(PRINT_ERROR, "List itterator not started");
		SET_ERRNO(ISU_RET_EUSAGE);
		return NULL;
	}
	
	node = list->ittr;
	if ( node->prev )
		node->prev->next = node->next;
	else
		list->head = node->next;
	
	if ( node->next )
		node->next->prev = node->prev;
	else
		list->tail = node->prev;
	
	switch ( list->ittr_dir )
	{
		case ISU_LLIST_HEAD:
			list->ittr = list->ittr->prev;
			break;
		case ISU_LLIST_TAIL:
			list->ittr = list->ittr->next;
			break;
	}
	
	if ( !list->ittr )
		list->remove_flag = 1;
	
	data = node->data;
	free (node);
	
	list->count --;
	isu_print(PRINT_DEBUG, "Succesfully removed element from linked list");
	return data;
}


/// @}





