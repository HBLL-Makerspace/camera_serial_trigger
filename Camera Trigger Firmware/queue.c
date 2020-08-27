#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <util\atomic.h>
#include "queue.h"
#include "statusLight.h"
#include <assert.h>

#define INCREMENT_SIZE 1 //Amount to increment
#define EMPTY_SIZE 1 //number of empty elements
#define FIRST_INDEX 0 //first index of array
// Uncomment line below to print out informational messages during queue operation.
// #define QUEUE_PRINT_INFO_MESSAGES

void declareError(){
 	statusLight_setError(0b01010101);
}

// Standard queue implementation that leaves one spot empty so easier to check for full/empty.
void queue_init(queue_t* q, queue_size_t size) {
	q->underflowFlag = false;  // True if queue_pop() is called on an empty queue.
	q->overflowFlag = false;  // True if queue_push() is called on a full queue.
	q->indexIn = 0;
	q->indexOut = 0;
	q->elementCount = 0;       // Not required but may ease implementation.
	q->size = size + EMPTY_SIZE;	// Add one additional location for the empty location; size is data-array size, exactly.
	q->data = (queue_data_t *) malloc(q->size * sizeof(queue_data_t));
}

//returns the next index in queue
queue_index_t getNextIndex(queue_t* q, queue_index_t index){
	return ((index + 1) % q->size); //increment index and then return it
}

// Returns true if the queue is full.
bool queue_full(queue_t* q){
	if(getNextIndex(q, q->indexIn) == q->indexOut){ //if the queue is full...
		return true;
	} else { //else the queue is not full...
		return false;
	}
}

// Returns true if the queue is empty.
bool queue_empty(queue_t* q){
	if(q->indexOut == q->indexIn){ //if the first and last indexes are the same...
		return true; //the queue is empty
	} else { //else it is not empty
		return false;
	}
}

// If the queue is not full, pushes a new element into the queue and clears the underflowFlag.
// IF the queue is full, set the overflowFlag, print an error message and DO NOT change the queue.
void queue_push(queue_t* q, queue_data_t value){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		if(queue_full(q)){//if the queue is full
			q->overflowFlag = true;//set our overflowFlag
			declareError();
		} else {
			q->underflowFlag = false;//clear the overflowFlag
			q->data[q->indexIn] = value;//write the value
			q->indexIn = getNextIndex(q, q->indexIn);//move to the popped value
			q->elementCount++;//increment elementCount
		}
	}
}

// If the queue is not empty, remove and return the oldest element in the queue.
// If the queue is empty, set the underflowFlag, print an error message, and DO NOT change the queue.
queue_data_t queue_pop(queue_t* q){
	queue_data_t temp = q->data[q->indexOut];//read data value
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		if(queue_empty(q)){
			q->underflowFlag = true;//set underflowFlag
			declareError();
			return QUEUE_RETURN_ERROR_VALUE;
		} else {
			q->overflowFlag = false;//clear flag

			q->indexOut = getNextIndex(q, q->indexOut);//move the index
			q->elementCount--;//decrement elementCount
		}
	}
	return temp;//return value read

}

// If the queue is full, call queue_pop() and then call queue_push().
// If the queue is not full, just call queue_push().
void queue_overwritePush(queue_t* q, queue_data_t value){
	if(queue_full(q)){//if the queue is full
		queue_pop(q);//pop the queue
	}
	queue_push(q, value);//push the value
}

// Provides random-access read capability to the queue.
// Low-valued indexes access older queue elements while higher-value indexes access newer elements
// (according to the order that they were added).
// Print a meaningful error message if an error condition is detected.
queue_data_t queue_readElementAt(queue_t* q, queue_index_t index){
	if(index >= q->elementCount){//check if we have exceded element count
		declareError();
		return QUEUE_RETURN_ERROR_VALUE;//return error value
	} else {
		queue_index_t myIndex = q->indexOut + index;//offset indexes
		if(myIndex > q->size - EMPTY_SIZE){//if we are going to leave the array`
			myIndex = myIndex - (q->size);//wrap around
		}
		return q->data[myIndex];//acctually read data
	}
}

// Returns a count of the elements currently contained in the queue.
queue_size_t queue_elementCount(queue_t* q){
	return q->elementCount;//return the count
}

// Returns true if an underflow has occurred (queue_pop() called on an empty queue).
bool queue_underflow(queue_t* q){
	return q->underflowFlag;//return underflow status
}

// Returns true if an overflow has occurred (queue_push() called on a full queue).
bool queue_overflow(queue_t* q){
	return q->overflowFlag;//return overflow status
}


// Tell the user size in terms of usable locations.
queue_size_t queue_size(queue_t* q) {return q->size-1;}


// Just free the data array in the queue.
void queue_garbageCollect(queue_t* q) {
	free(q->data);
}




