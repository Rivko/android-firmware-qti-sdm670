/**
@file ListenProcSvc_circ_buffer.cpp
@brief This file contains api for circular buffer implementation for LAB
 */

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The core_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      core_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2013-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_circ_buffer.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
3/11/14   SivaNaga      Created file.

========================================================================== */
#include "adsp_error_codes.h"
#include "ListenProcSvc_circ_buffer.h"
#include "audio_basic_op_ext.h"

#ifdef LSM_DEBUG
circbuf_result circ_buf_check_if_corrupted(circ_buf_struct_t *circ_buf_struct_ptr);
#endif

/*
 * Initializes the Circular buffer
 * param  : pointer to a circular buffer structure
 *          Pointer to the memory allocated to circular buffer
 *          Circular buffer size
 * functionality : Initializes the circular buffer parameters
 * return : circbuf_result
 */
circbuf_result circ_buf_init(circ_buf_struct_t *circ_buf_struct_ptr, int8_t *buf_ptr, uint32_t buffer_size)
{
  if((NULL == circ_buf_struct_ptr) || (NULL == buf_ptr) || (0 == buffer_size))
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Rcvd Bad params");
#endif
    return CIRCBUF_FAIL;
  }

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"LAB Circ buf size = %lu",buffer_size);

  /* clear */
  circ_buf_struct_ptr->write_index = 0;
  circ_buf_struct_ptr->read_index = 0;
  circ_buf_struct_ptr->unread_bytes = 0;

  /* set buffer */
  circ_buf_struct_ptr->buffer_ptr = buf_ptr;
  circ_buf_struct_ptr->buffer_size = buffer_size;

  return CIRCBUF_SUCCESS;
}

/*
 * Read samples from the Circular buffer
 * param  : pointer to a circular buffer structure
 *          Pointer to which data has to be read from circular buffer
 *          number of bytes to be read
 * functionality : copies the samples_to_read number of samples to out_ptr
 * return : circbuf_result
 */
circbuf_result circ_buf_read(circ_buf_struct_t *circ_buf_struct_ptr, int8_t *out_ptr, uint32_t bytes_to_read)
{
  uint32_t read_index = circ_buf_struct_ptr->read_index;
  uint32_t iter = 0;
  const uint32_t buffer_size = circ_buf_struct_ptr->buffer_size;

  if((bytes_to_read > buffer_size))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Rcvd bytes to read = %lu",bytes_to_read);
#endif
    return CIRCBUF_FAIL;
  }
#ifdef LSM_DEBUG
  if(CIRCBUF_FAIL == circ_buf_check_if_corrupted(circ_buf_struct_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Circ buf corrupted");
    return CIRCBUF_FAIL;
  }
#endif
  if(bytes_to_read > circ_buf_struct_ptr->unread_bytes)
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Circ Buf underflowed, bytes to read = %lu, unread bytes = %lu",
          bytes_to_read,
          circ_buf_struct_ptr->unread_bytes);
#endif
    if (out_ptr != NULL)
    {
      memset(out_ptr,0,bytes_to_read);
    }
    return CIRCBUF_UNDERFLOW;
  }


  if (out_ptr != NULL) 
  {
    for(iter = 0; iter < bytes_to_read; iter++)
    {
      out_ptr[iter] = circ_buf_struct_ptr->buffer_ptr[read_index];
      read_index = s32_modwrap_s32_u32(read_index + 1, buffer_size);
    }
  }
  else
  {
    /*this case comes in circ buffer overflow case, where we are adjusting read pointer
      by flushing out oldest data  */
    read_index = s32_modwrap_s32_u32(read_index + bytes_to_read, buffer_size);
  }


  circ_buf_struct_ptr->read_index = read_index;
  circ_buf_struct_ptr->unread_bytes -= bytes_to_read;

  return CIRCBUF_SUCCESS;
}

/*
 * Write samples to the Circular buffer
 * param  : pointer to a circular buffer structure
 *          Pointer from which data has to be written to circular buffer
 *          number of bytes to write
 * functionality : Checks the available unread space in the circular and
 *                 copies the samples_to_write amount of samples to inp_ptr
 * return : circbuf_result
 */
circbuf_result circ_buf_write(circ_buf_struct_t *circ_buf_struct_ptr, int8_t *inp_ptr, uint32_t bytes_to_write)
{
  uint32_t free_bytes = 0;
  uint32_t iter = 0;
  uint32_t write_index = circ_buf_struct_ptr->write_index;
  const uint32 buffer_size = circ_buf_struct_ptr->buffer_size;
  circbuf_result result = CIRCBUF_SUCCESS;

  if((NULL == inp_ptr) || (bytes_to_write > buffer_size))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Rcvd bytes to write = %lu",bytes_to_write);
#endif
    return CIRCBUF_FAIL;
  }
#ifdef LSM_DEBUG
  if(CIRCBUF_FAIL == circ_buf_check_if_corrupted(circ_buf_struct_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Circ buf corrupted");
    return CIRCBUF_FAIL;
  }
#endif

  free_bytes = circ_buf_struct_ptr->buffer_size - circ_buf_struct_ptr->unread_bytes;
  if(bytes_to_write > free_bytes)
  {
    /* Since there is no space in circular buffer for writing samples_to_write number of samples,
	   create space by flushing samples_to_write-free_samples number of samples*/      
    circ_buf_read(circ_buf_struct_ptr, NULL, bytes_to_write - free_bytes);
    result = CIRCBUF_OVERFLOW;
  }

  for(iter = 0; iter < bytes_to_write; iter++)
  {
    circ_buf_struct_ptr->buffer_ptr[write_index] = inp_ptr[iter];
    write_index = s32_modwrap_s32_u32(write_index+1, buffer_size);
  }

  circ_buf_struct_ptr->write_index = write_index;
  circ_buf_struct_ptr->unread_bytes += bytes_to_write;

  return result;
}

/*
 * Resets the circular buffer
 * param  : pointer to a circular buffer structure
 * functionality : Resets the circular buffer
 * return : circbuf_result
 */
circbuf_result circ_buf_reset(circ_buf_struct_t *circ_buf_struct_ptr)
{
  circ_buf_struct_ptr->read_index = 0;
  circ_buf_struct_ptr->write_index = 0;
  circ_buf_struct_ptr->unread_bytes = 0;
  return CIRCBUF_SUCCESS;
}

/*
 * Adjusts the read pointer
 * param  : pointer to a circular buffer structure
 * functionality : Moves the read pointer to a new position to start reading from write_ptr-bytes_to_adjust
 * return : circbuf_result
 */
circbuf_result circ_adjust_read(circ_buf_struct_t *circ_buf_struct_ptr,uint32_t bytes_to_adjust)
{
  uint32_t bytes_to_read = 0;
  circbuf_result result = CIRCBUF_SUCCESS;

  if(bytes_to_adjust > circ_buf_struct_ptr->unread_bytes)
  {
#ifdef LSM_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "circ_adjust_read fail, bytes to adjust = %lu, unread bytes = %lu",
          bytes_to_adjust,
          circ_buf_struct_ptr->unread_bytes);
#endif
    return CIRCBUF_FAIL;
  }

  bytes_to_read = circ_buf_struct_ptr->unread_bytes - bytes_to_adjust;
  circ_buf_struct_ptr->read_index = s32_modwrap_s32_u32(circ_buf_struct_ptr->read_index + bytes_to_read,
                                                        circ_buf_struct_ptr->buffer_size);
  circ_buf_struct_ptr->unread_bytes -= bytes_to_read;
  return result;
}

#ifdef LSM_DEBUG
/* This function checks if the gap between write_ptr and read_ptr is valid
 * and is used to check if any corruptions happened in circ buf struct*/

circbuf_result circ_buf_check_if_corrupted(circ_buf_struct_t *circ_buf_struct_ptr)
{
  int32_t write_read_gap = 0;
  write_read_gap = circ_buf_struct_ptr->write_index - circ_buf_struct_ptr->read_index;
  if(write_read_gap < 0)
  {
    write_read_gap += circ_buf_struct_ptr->buffer_size;
    if(write_read_gap < 0)
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Circ buf read/write corrupted");
      return CIRCBUF_FAIL;
    }
  }
  if(write_read_gap > (int32_t) circ_buf_struct_ptr->buffer_size)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Circ buf read/write corrupted");
    return CIRCBUF_FAIL;
  }
  return CIRCBUF_SUCCESS;
}
#endif
/*
 * Deinit the circular buffer
 * param  : pointer to a circular buffer structure
 * functionality : Deinits the circular buffer
 * return : circbuf_result
 */
circbuf_result circ_buf_deinit(circ_buf_struct_t *circ_buf_struct_ptr)
{
  memset(circ_buf_struct_ptr,0,sizeof(circ_buf_struct_t));
  return CIRCBUF_SUCCESS;
}
