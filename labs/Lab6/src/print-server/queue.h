/**
 * @file      queue.h
 * @author    Jeramie Ven1
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


#ifndef QUEUE_H 
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif 

struct QUEUE_STRUCT;
/// A publically accessable type representing a private queue object
typedef struct QUEUE_STRUCT* queue_t;

/// Which end of the queue we are working with
typedef enum QUEUE_ENDS_ENUM
{
	/// represents the head or first element of the queue
	QUEUE_HEAD,
	/// represents the tail or last element of the queue
	QUEUE_TAIL
} queue_ends_t;

queue_t       queue_create             (void)                                            __attribute__ ((malloc));
queue_t       queue_destroy            (queue_t list)                                    __attribute__ ((warn_unused_result));
void          queue_push               (queue_t list, void* data, queue_ends_t end);
void*         queue_pop                (queue_t list, queue_ends_t end)                  __attribute__ ((warn_unused_result));
void          queue_release_threads    (queue_t list, int number);

#ifdef __cplusplus
}
#endif 

#endif

