#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/*============================================================================
  @file circular_buffer.h

  Circular buffer utility header file

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

#include <stdint.h>
#include "matrix.h"

/*
 * since BUF_SIZE is a power of 2,
 * (x & (BUF_SIZE-1)) == (x % BUF_SIZE)
 */
#define BUF_INDEX(index, buf_size) ((index)&(buf_size-1))

typedef struct
{
  matrix_type data;
  int32_t end;
  int32_t window_size;
  int32_t count;
} buffer_type;

int32_t buffer_mem_req(int32_t window_size, int32_t cols);
void buffer_reset(buffer_type *buffer, int32_t window_size, int32_t cols, void *mem);
void buffer_insert(buffer_type *buffer, int32_t const *new_data);
void buffer_sprd(buffer_type *buffer, int32_t *sprd_data);
void buffer_sum(buffer_type *buffer, int32_t *sum_data);
void buffer_sum_sq(buffer_type *buffer, int32_t *sum_sq_data, int32_t q_format);
int32_t buffer_full(buffer_type *buffer);
uint8_t buffer_num_samples( buffer_type *buffer);
void buffer_sum_and_sum_sq(buffer_type *buffer, int32_t *sum_data, int32_t *sum_sq_data, int32_t q_format);
int32_t buffer_size(int32_t window_size);
int32_t buffer_partial_compute_num_data_rows(buffer_type const *buffer, int32_t start_index, int32_t end_index);
void buffer_partial_sum(buffer_type const *buffer, int32_t start_index, int32_t end_index, int32_t *sum_data);
void buffer_partial_sum_sq(buffer_type const *buffer, int32_t start_index, int32_t end_index, int32_t *sum_sq_data);
void buffer_partial_min_max_sprd_one_column(buffer_type const *buffer, int32_t start_index, int32_t stop_index, int32_t col,
                                 int32_t *min_data, int32_t *max_data, int32_t *sprd_data);


#endif /* CIRCULAR_BUFFER_H */
