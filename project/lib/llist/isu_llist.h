/**
 * @addtogroup libraries
 * @{
 * @addtogroup llist Linked List
 * @author    Jeramie Vens
 * @date      3/12/15 - Created
 * @date      3/13/15 - Documentation added
 * @brief     A linked list library for CprE308 final project
 * 
 * @details
 * This module is a linked list library created for the CprE308 final project at Iowa State University.
 * It is currently not thread safe.  Thread safeness may be added at a future time, but for now it should
 * not be considered safe to use across multiple threads.
 * 
 * To use link project with the -lllist flag and include "include/llist.h"
 * 
 * @copyright MIT 
 * @{
 * @file      isu_llist.h
 * @author    Jeramie Vens
 * @date      3/12/15 - Created
 * @date      3/13/15 - Documentation added
 * @brief     The main include file for the linked list library
 */


#ifndef ISU_LLIST_H
#define ISU_LLIST_H

#include "common/isu_types.h"

/**
 * @brief Linked list end
 * Represents an end of the linked list.
 */
typedef enum
{
	/// The head of the linked list
	ISU_LLIST_HEAD,
	/// The tail of the linked list
	ISU_LLIST_TAIL
}isu_llist_end_t;

/** 
 * @class isu_llist_t
 * @brief     A linked list object.
 * @details   A linked list object can hold any data type and dynamically
 *            changes in size as new data is added and removed from the object.
 */
typedef struct ISU_LLIST_STRUCT *isu_llist_t;

/**
 * @def       for_each
 * @brief     Macro for creating a for_each loop.  
 * @details
 * A for_each loop works simillarly to a normal for loop, except with a linked list.
 * It will itterate over each element of the linked list assigning the first parameter
 * To the value of the next element in the list.  It will walk the list from front
 * to back.  It can be thought of as for each element i in list l.
 * 
 * \b Example \n
 * @code{C}
 *     isu_llist_t list = isu_llist_create();
 *     // push data to list
 *     void* data;
 *     for_each(data, list)
 *     {
 *         use_data(data);
 *     }
 * @endcode
 */
#define for_each(i, list) for( i = (typeof(i))isu_llist_ittr_start(list, ISU_LLIST_HEAD); i != NULL; i = (typeof(i))isu_llist_ittr_next(list))

/**
 * @brief     Construct a new linked list object
 * @memberof  isu_llist_t
 * @return    The linked list object or null if failure
 * @details   This creates a new linked list object which can be worked on with
 *            The other linked list methods.
 */
isu_llist_t        isu_llist_create              (void);
/** 
 * @brief     Destroy the linked list object
 * @memberof  isu_llist_t
 * @param     list
 *                 The list to be destroyed
 */
void               isu_llist_destroy             (isu_llist_t list);
/**
 * @brief     Get the current number of elements in the linked list object
 * @memberof  isu_llist_t
 * @param     list
 *                 The list to work on
 * @return    The number of elements in the list
 */
int                isu_llist_count               (isu_llist_t list);
/**
 * @brief     Push a new element onto the linked list.
 * @memberof  isu_llist_t
 * @param     list
 *                 The list to push the data onto
 * @param     data
 *                 The data to push onto the list
 * @param     end
 *                 The end of the list to push the data onto
 * @return    ISU_RET_SUCCESS:
 *                 Data successfully pushed to the list
 * @return    ISU_RET_EMEM:
 *                 Malloc call failed
 * @return    ISU_RET_EPARAM:
 *                 Invalid parameter passed
 */
isu_return_t       isu_llist_push                (isu_llist_t list, void* data, isu_llist_end_t end);
/**
 * @brief     Pop an element form the list and return the data in that element.
 * @memberof  isu_llist_t
 * @param     list
 *                 The list to remove the element from
 * @param     end
 *                 The end of the list to pop the elemnt from
 * @return    NULL:
 *                 The list was empty or an error occured.  Error indicated
 *                 by setting isu_errno.
 * @return    data:
 *                 The data popped from the list.
 */
void*              isu_llist_pop                 (isu_llist_t list, isu_llist_end_t end);
/**
 * @brief     Start a new itterator at the given end of the list
 * @memberof  isu_llist_t
 * @param     list
 *                 The list to itterate over
 * @param     end
 *                 The end of the list to start at
 * @return    NULL:
 *                 If creating the itterator failed.  isu_errno set to indicate error.
 * @return    NULL:
 *                 If the list is empty.
 * @return    data:
 *                 The data at the given end of the list.
 * @details
 * To itterate over a list first call isu_llist_ittr_start() to get the first
 * piece of data from the list.  Each subsiquent call to isu_llist_ittr_next()
 * will return the data stored in the next element of the list.
 * @remark    Itterators are not considered thread safe on a linked list.
 */
void*              isu_llist_ittr_start          (isu_llist_t list, isu_llist_end_t end);
/**
 * @brief     Get the data at the next element of the list being itterated on.
 * @memberof  isu_llist_t
 * @param     list
 *                 List being itterated on
 * @return    NULL:
 *                 If error occures.  isu_errno set to indicate error.
 * @return    NULL:
 *                 The list is empty.
 * @return    data:
 *                 The data stored in the next element of the list.
 * @remark    isu_llist_ittr_start() must be called before a call to this function.
 */
void*              isu_llist_ittr_next           (isu_llist_t list);
/**
 * @brief     Insert new data into the list after the itterator.
 * @memberof  isu_llist_t
 * @param     list
 *                 List to add the data to.
 * @param     data
 *                 Data to add to the list.
 * @return    ISU_RET_EMEM:
 *                 Malloc call failed.
 * @return    ISU_RET_EUSAGE:
 *                 If the itterattor has not been started before this call.
 * @return    ISU_RET_SUCCESS:
 *                 Data was successfully added.
 * @remark    The next call to isu_llist_ittr_next() will return this new data.
 */ 
isu_return_t       isu_llist_insert_after        (isu_llist_t list, void *data);
/**
 * @brief     Insert new data into the list before the itterator.
 * @memberof  isu_llist_t
 * @param     list
 *                 List to add the data to.
 * @param     data
 *                 Data to add to the list.
 * @return    ISU_RET_EMEM:
 *                 Malloc call failed.
 * @return    ISU_RET_EUSAGE:
 *                 If the itterattor has not been started before this call.
 * @return    ISU_RET_SUCCESS:
 *                 Data was successfully added.
 * @remark    The next call to isu_llist_ittr_next() will return this new data.
 */ 
isu_return_t       isu_llist_insert_before       (isu_llist_t list, void *data);

/**
 * @brief     Remove the current element from the list.
 * @memberof  isu_llist_t
 * @param     list
 *                 The list to remove the element from.
 * @return    NULL:
 *                 If there is an error.  Sets isu_errno to indicate the error.
 * @return    data:
 *                 The data in the removed node.
 */
void*              isu_llist_remove              (isu_llist_t list);



#endif

/// @} 
/// @}


