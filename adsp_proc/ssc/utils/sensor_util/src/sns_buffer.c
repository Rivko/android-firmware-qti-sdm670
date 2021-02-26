/*============================================================================
  @file sns_buffer.c

  Circular buffer utility source file

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "sns_buffer.h"

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/*=========================================================================
  FUNCTION:  sns_roundup_pow_two
  =======================================================================*/
/*!
    @brief Returns closest power of 2 to the value passed in

    @param[i] val: input value for which closest power of 2 is to be estimated

    @return value rounded up to nearest power of 2
*/
/*=======================================================================*/
SNS_SECTION(".text.sns") uint32_t
sns_roundup_pow_two(
   uint32_t val)
{
   val |= val >> 1;
   val |= val >> 2;
   val |= val >> 4;
   val |= val >> 8;
   val |= val >> 16;
   val++;

   return val;
}

/*=========================================================================
  FUNCTION:  sns_buffer_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed for buffer with specified params

    @param[i] window_size: size of window (number of data elements)
    @param[i] num_data_axes: number of axes for each data element

    @return Size of memory required
*/
/*=======================================================================*/
SNS_SECTION(".text.sns") uint32_t
sns_buffer_mem_req(
   uint32_t window_size,
   uint32_t num_data_axes)
{
   return(sizeof(float) * sns_roundup_pow_two(window_size) * num_data_axes);
}

/*=========================================================================
  FUNCTION:  sns_buffer_reset
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[io] buffer: pointer to buffer
    @param[i] window_size: size of window (number of data elements)
    @param[i] num_data_axes: number of axes for each data element
    @param[i] mem: memory provided for data buffer

    @return None
*/
/*=======================================================================*/
SNS_SECTION(".text.sns") void
sns_buffer_reset(
   sns_buffer_type* buffer,
   uint32_t window_size,
   uint32_t num_data_axes,
   void *mem)
{
   uint32_t i, j;

   buffer->data = mem;
   buffer->num_data_axes = num_data_axes;
   buffer->actual_window_size = sns_roundup_pow_two(window_size);
   buffer->window_size = window_size;
   buffer->end   = 0;
   buffer->count = 0;

   for (i=0; i< buffer->actual_window_size; i++)
   {
      for (j=0; j< buffer->num_data_axes; j++)
      {
         SNS_BUF_ELEM(buffer, i, j) = 0;
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_buffer_full
  =======================================================================*/
/*!
    @brief Indicate if buffer is full

    @param[i] buffer: pointer to buffer

    @return true - if buffer full, false - otherwise
*/
/*=======================================================================*/
SNS_SECTION(".text.sns") bool 
sns_buffer_full(
   const sns_buffer_type* buffer)
{
   return(buffer->count >= buffer->window_size);
}

/*=========================================================================
  FUNCTION:  sns_buffer_insert
  =======================================================================*/
/*!
    @brief Inserts an element into the end of buffer. If buffer is full,
           oldest element in the buffer is replaced with the new one.

    @param[i] buffer: pointer to buffer
    @param[i] newData: pointer to data to be inserted in buffer

    @return None
*/
/*=======================================================================*/
SNS_SECTION(".text.sns") void 
sns_buffer_insert(
   sns_buffer_type* buffer,
   const float* newData)
{
   uint32_t i;

   for (i=0; i < buffer->num_data_axes; i++)
   {
      SNS_BUF_ELEM(buffer, buffer->end, i) = newData[i];
   }

   buffer->end = SNS_BUF_INDEX(buffer->end+1, buffer->actual_window_size);

   if (buffer->count < buffer->actual_window_size)
   {
      buffer->count++;
   }
}

/*=========================================================================
  FUNCTION:  sns_buffer_compute_variance
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[i] buffer: pointer to buffer
    @param[o] sum_data: sum of data elements for variance computation
    @param[o] sum_sq_data: sum of square data elements for variance computation

    @return variance
*/
/*=======================================================================*/
SNS_SECTION(".text.sns") double
sns_buffer_compute_variance(
   const sns_buffer_type* buffer,
   double* sum_data,
   double* sumSquareData)
{
   double varTerm1, varTerm2, variance = 0;
   uint32_t i, oldDataIndex, newDataIndex;
   double oldData, newData;

   oldDataIndex = SNS_BUF_INDEX(buffer->end - buffer->window_size-1, buffer->actual_window_size);
   newDataIndex = SNS_BUF_INDEX(buffer->end-1, buffer->actual_window_size);

   for (i=0; i<buffer->num_data_axes; i++)
   {
      oldData = SNS_BUF_ELEM(buffer, oldDataIndex, i);
      newData = SNS_BUF_ELEM(buffer, newDataIndex, i);
      sum_data[i] += (newData - oldData);
      sumSquareData[i] += (newData * newData - oldData * oldData);
   }

   if (sns_buffer_full(buffer))
   {
      varTerm1 = sumSquareData[0] + sumSquareData[1] + sumSquareData[2];

      varTerm2 = (sum_data[0]*sum_data[0] + sum_data[1]*sum_data[1] + sum_data[2]*sum_data[2]);

      variance = (varTerm1 - varTerm2 / buffer->window_size) / buffer->window_size;

   }

   return variance;
}

/*=========================================================================
  FUNCTION:  sns_buffer_sprd
  =======================================================================*/
/*!
    @brief Calculate the maximum peak to minimum peak spread of the buffer

    @param[i] buffer: pointer to buffer
    @param[o] sprd_data: Buffer spread

    @return variance
*/
/*=======================================================================*/

SNS_SECTION(".text.sns") void
sns_buffer_sprd(sns_buffer_type *buffer, float *sprd_data)
{
  int32_t j, min_index, max_index, start_index, end_index;
  float buf_min, buf_max, cur_val;

  end_index   = buffer->end;
  start_index = end_index - buffer->window_size;

  max_index = start_index;
  min_index = max_index;

  buf_min = SNS_1D_BUF_ELEM(buffer, SNS_BUF_INDEX(max_index, buffer->actual_window_size));
  buf_max = buf_min;

  for (j=start_index+1; j<end_index; j++)
  {
    cur_val = SNS_1D_BUF_ELEM(buffer, SNS_BUF_INDEX(j, buffer->actual_window_size));

    if (cur_val < buf_min)
    {
      buf_min = cur_val;
      min_index = j;
    }

    if (cur_val > buf_max)
    {
      buf_max = cur_val;
      max_index = j;
    }
  }
  *sprd_data = (max_index<min_index)?(-(buf_max-buf_min)):(buf_max-buf_min);
}

