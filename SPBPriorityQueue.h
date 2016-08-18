#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * The function creates an empty priority queue of capacity maxSize
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * The function creates a copy of the given priority queue
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * The function destroys an allocated priority queue
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * The function clears a queue
 */
void spBPQueueClear(SPBPQueue source);

/**
 * The function returns the current occupied size of the queue
 */
int spBPQueueSize(SPBPQueue source);

/**
 * The function returns the capacity of the given queue
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * The function enqueues a copy of the given element to the given queue
 * Returns SP_BPQUEUE_INVALID_ARGUMENT if any given argument is invalid
 * Returns SP_BPQUEUE_OUT_OF_MEMORY if it fails to allocate a copy of the element
 * Returns SP_BPQUEUE_FULL if the queue reached its capacity
 * Returns SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * The function dequeues the element at the head of the queue
 * Returns SP_BPQUEUE_EMPTY if there is no element to dequeue
 * Returns SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * The function returns a copy of the element at the head of the queue
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * The function returns a copy of the element at the end of the queue
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * The function returns the value of the element at the head of the queue
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * The function returns the value of the element at the end of the queue
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * The function returns true if and only if the queue is empty
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * The function returns true if and only if the queue is full (reached its capacity)
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
