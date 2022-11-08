#ifndef STM32L4
   #define STM32L4
#endif
#include "circularBuffer.h"

/**
 * @file
 * Implementation of ring buffer functions.
 */
//**************************************************************************************************************************
/**
 * @brief Initialize circular buffer 
 * 
 * @param buffer - circular buffer to initialize
 */
void circular_buffer_init(circular_buffer_t *buffer) {
  buffer->tail_index = 0;
  buffer->head_index = 0;
}
//**************************************************************************************************************************
/**
 * @brief Push data to the circular buffer
 * 
 * @param buffer - buffer to push data to
 * @param data - data to push to buffer
 */
void circular_buffer_queue(circular_buffer_t *buffer, uint8_t data) {
  /* Is buffer full? */
  if(circular_buffer_is_full(buffer)) {
    /* Is going to overwrite the oldest byte */
    /* Increase tail index */
    buffer->tail_index = ((buffer->tail_index + 1) & CIRCULAR_BUFFER_MASK);
  }
  /* Place data in buffer */
  buffer->buffer[buffer->head_index] = data;
  buffer->head_index = ((buffer->head_index + 1) & CIRCULAR_BUFFER_MASK);
}
//**************************************************************************************************************************
/**
 * @brief Push an array onto circular buffer
 * 
 * @param buffer - buffer to push array to
 * @param data - data to push to buffer
 * @param size - size of array of data
 */
void circular_buffer_queue_arr(circular_buffer_t *buffer, const uint8_t *data, circular_buffer_size_t size) {
  /* Add bytes; one by one */
  circular_buffer_size_t i;
  for(i = 0; i < size; i++) {
    circular_buffer_queue(buffer, data[i]);
  }
}
//**************************************************************************************************************************
/**
 * @brief Pop data from circular buffer
 * 
 * @param buffer - buffer to pop from
 * @param data - location to pop to
 * @return uint8_t -> 1 if proper data was popped, 0 if no data to dequeue
 */
uint8_t circular_buffer_dequeue(circular_buffer_t *buffer, uint8_t *data) {
    if(circular_buffer_is_empty(buffer)) {
      /* No items */
      return 0;
    }
    *data = buffer->buffer[buffer->tail_index];
    buffer->tail_index = ((buffer->tail_index + 1) & CIRCULAR_BUFFER_MASK);
    return 1;
}
//**************************************************************************************************************************
/**
 * @brief Pop array from buffer
 * 
 * @param buffer - buffer to pop from
 * @param data - location to pop array to
 * @param len - length of array to pop
 * @return circular_buffer_size_t 
 */
circular_buffer_size_t circular_buffer_dequeue_arr(circular_buffer_t *buffer, uint8_t *data, circular_buffer_size_t len) {
    if(circular_buffer_is_empty(buffer)) {
      /* No items */
      return 0;
    }
    uint8_t *data_ptr = data;
    circular_buffer_size_t cnt = 0;
    while((cnt < len) && circular_buffer_dequeue(buffer, data_ptr)) {
      cnt++;
      data_ptr++;
    }
    return cnt;
}
//**************************************************************************************************************************
/**
 * @brief Get values held in circular buffer without popping
 * 
 * @param buffer - buffer to peak
 * @param data - memory to send data to
 * @param index - location in circular buffer that you want to peek
 * @return uint8_t - returns 1 if data available, 0 if not
 */
uint8_t circular_buffer_peek(circular_buffer_t *buffer, uint8_t *data, circular_buffer_size_t index) {
  if(index >= circular_buffer_num_items(buffer)) {
    /* No items at index */
    return 0;
  }
  
  /* Add index to pointer */
  circular_buffer_size_t data_index = ((buffer->tail_index + index) & CIRCULAR_BUFFER_MASK);
  *data = buffer->buffer[data_index];
  return 1;
}
//**************************************************************************************************************************
/**
 * @brief Check if buffer is empty
 * 
 * @param buffer - buffer to check for empty
 * @return uint8_t - 1 if empty, 0 if not
 */
extern inline uint8_t circular_buffer_is_empty(circular_buffer_t *buffer);
//**************************************************************************************************************************
/**
 * @brief Check if buffer is full
 * 
 * @param buffer - buffer to check if full
 * @return uint8_t - 1 if full, 0 if not
 */
extern inline uint8_t circular_buffer_is_full(circular_buffer_t *buffer);
//**************************************************************************************************************************
/**
 * @brief get number of items in the buffer
 * 
 * @param buffer - buffer to check for num items
 * @return circular_buffer_size_t - return size of buffer
 */
extern inline circular_buffer_size_t circular_buffer_num_items(circular_buffer_t *buffer);
//**************************************************************************************************************************