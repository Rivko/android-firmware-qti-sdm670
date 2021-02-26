#pragma once
/*============================================================================
  @file sns_buffer.h

  Circular buffer utility header file

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <stdbool.h>
#include <stdint.h>

/*
 * since BUF_SIZE is a power of 2,
 * (x & (BUF_SIZE-1)) == (x % BUF_SIZE)
 */
#define SNS_BUF_INDEX(index,buf_size) ((index) & (buf_size-1))

#define SNS_BUF_ELEM(buf,row,column) (*((buf)->data + row*(buf)->num_data_axes + column))
#define SNS_1D_BUF_ELEM(buf, offset) (*((buf)->data + offset))

/* Structure for circular buffer of data stored as floating point */
typedef struct
{
   float *data;

   // number of axes in each data element
   uint32_t num_data_axes;

   // actual number of data elements that can be stored in the buffer,
   // rounding up requested window_size to nearest power of 2
   uint32_t actual_window_size;

   // requested number of data elements to be queued on buffer
   uint32_t window_size; 

   // tail index of the circular buffer
   uint32_t end;

   // number of data elements in the buffer
   uint32_t count;
} sns_buffer_type;

/*=========================================================================
  FUNCTION:  sns_roundup_pow_two
  =======================================================================*/
/*!
    @brief Returns closest power of 2 to the value passed in

    @param[i] val: input value for which closest power of 2 is to be estimated

    @return value rounded up to nearest power of 2
*/
/*=======================================================================*/
uint32_t sns_roundup_pow_two(
   uint32_t val);

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
uint32_t sns_buffer_mem_req(
   uint32_t window_size,
   uint32_t num_data_axes);

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
void sns_buffer_reset(
   sns_buffer_type* buffer,
   uint32_t window_size,
   uint32_t num_data_axes,
   void* mem);

/*=========================================================================
  FUNCTION:  sns_buffer_full
  =======================================================================*/
/*!
    @brief Indicate if buffer is full

    @param[i] buffer: pointer to buffer

    @return true - if buffer full, false - otherwise
*/
/*=======================================================================*/
bool sns_buffer_full(
   const sns_buffer_type* buffer);

/*=========================================================================
  FUNCTION:  sns_buffer_insert
  =======================================================================*/
/*!
    @brief Inserts an element into the end of buffer. If buffer is full,
           oldest element in the buffer is replaced with the new one.

    @param[io] buffer: pointer to buffer
    @param[i] data: pointer to data to be inserted in buffer

    @return None
*/
/*=======================================================================*/
void sns_buffer_insert(
   sns_buffer_type* buffer,
   const float* data);

/*=========================================================================
  FUNCTION:  sns_buffer_compute_variance
  =======================================================================*/
/*!
    @brief Compute the variance

    @param[i] buffer: pointer to buffer
    @param[o] sum_data: sum of data elements for variance computation
    @param[o] sum_sq_data: sum of square data elements for variance computation

    @return variance
*/
/*=======================================================================*/
double sns_buffer_compute_variance(
   const sns_buffer_type* buffer,
   double* sum_data,
   double* sum_sq_data);

/*=========================================================================
  FUNCTION:  sns_buffer_sprd
  =======================================================================*/
/*!
    @brief Calculate the maximum peak to minimum peak spread of the buffer
«
    @param[i] buffer: pointer to buffer
    @param[o] sprd_data: pointer to the spread value of buffer

    @return None
*/
/*=======================================================================*/

void sns_buffer_sprd(sns_buffer_type *buffer, float *sprd_data);


