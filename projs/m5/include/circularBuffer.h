


#ifndef STM32L4
#define STM32L4
#endif
#include <inttypes.h>

/**
 * @file
 * Prototypes and structures for the circular buffer module.
 */

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

 /**
  * The size of a circular buffer.
  * Due to the design only <tt> circular_BUFFER_SIZE-1 </tt> items
  * can be contained in the buffer.
  * The buffer size must be a power of two.
 */
#define CIRCULAR_BUFFER_SIZE 1024

#if (CIRCULAR_BUFFER_SIZE & (CIRCULAR_BUFFER_SIZE - 1)) != 0
#error "circular_BUFFER_SIZE must be a power of two"
#endif

 /**
  * The type which is used to hold the size
  * and the indicies of the buffer.
  * Must be able to fit \c circular_BUFFER_SIZE .
  */
typedef volatile uint8_t circular_buffer_size_t;

/**
 * Used as a modulo operator
 * as <tt> a % b = (a & (b − 1)) </tt>
 * where \c a is a positive index in the buffer and
 * \c b is the (power of two) size of the buffer.
 */
#define CIRCULAR_BUFFER_MASK (CIRCULAR_BUFFER_SIZE-1)

 /**
  * Simplifies the use of <tt>struct circular_buffer_t</tt>.
  */
typedef struct circular_buffer_t circular_buffer_t;

/**
 * Structure which holds a circular buffer.
 * The buffer contains a buffer array
 * as well as metadata for the circular buffer.
 */
struct circular_buffer_t {
    /** Buffer memory. */
    volatile uint8_t buffer[CIRCULAR_BUFFER_SIZE];
    /** Index of tail. */
    circular_buffer_size_t tail_index;
    /** Index of head. */
    circular_buffer_size_t head_index;
};

/**
 * Initializes the circular buffer pointed to by <em>buffer</em>.
 * This function can also be used to empty/reset the buffer.
 * @param buffer The circular buffer to initialize.
 */
void circular_buffer_init(circular_buffer_t* buffer);

/**
 * Adds a byte to a circular buffer.
 * @param buffer The buffer in which the data should be placed.
 * @param data The byte to place.
 */
void circular_buffer_queue(circular_buffer_t* buffer, uint8_t data);

/**
 * Adds an array of bytes to a circular buffer.
 * @param buffer The buffer in which the data should be placed.
 * @param data A pointer to the array of bytes to place in the queue.
 * @param size The size of the array.
 */
void circular_buffer_queue_arr(circular_buffer_t* buffer, const uint8_t* data, circular_buffer_size_t size);

/**
 * Returns the oldest byte in a circular buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @return 1 if data was returned; 0 otherwise.
 */
uint8_t circular_buffer_dequeue(circular_buffer_t* buffer, uint8_t* data);

/**
 * Returns the <em>len</em> oldest bytes in a circular buffer.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the array at which the data should be placed.
 * @param len The maximum number of bytes to return.
 * @return The number of bytes returned.
 */
circular_buffer_size_t circular_buffer_dequeue_arr(circular_buffer_t* buffer, uint8_t* data, circular_buffer_size_t len);
/**
 * Peeks a circular buffer, i.e. returns an element without removing it.
 * @param buffer The buffer from which the data should be returned.
 * @param data A pointer to the location at which the data should be placed.
 * @param index The index to peek.
 * @return 1 if data was returned; 0 otherwise.
 */
uint8_t circular_buffer_peek(circular_buffer_t* buffer, uint8_t* data, circular_buffer_size_t index);


/**
 * Returns whether a circular buffer is empty.
 * @param buffer The buffer for which it should be returned whether it is empty.
 * @return 1 if empty; 0 otherwise.
 */
inline uint8_t circular_buffer_is_empty(circular_buffer_t* buffer) {
    return (buffer->head_index == buffer->tail_index);
}

/**
 * Returns whether a circular buffer is full.
 * @param buffer The buffer for which it should be returned whether it is full.
 * @return 1 if full; 0 otherwise.
 */
inline uint8_t circular_buffer_is_full(circular_buffer_t* buffer) {
    return ((buffer->head_index - buffer->tail_index) & CIRCULAR_BUFFER_MASK) == CIRCULAR_BUFFER_MASK;
}

/**
 * Returns the number of items in a circular buffer.
 * @param buffer The buffer for which the number of items should be returned.
 * @return The number of items in the circular buffer.
 */
inline circular_buffer_size_t circular_buffer_num_items(circular_buffer_t* buffer) {
    return ((buffer->head_index - buffer->tail_index) & CIRCULAR_BUFFER_MASK);
}

#endif /* circularBUFFER_H */