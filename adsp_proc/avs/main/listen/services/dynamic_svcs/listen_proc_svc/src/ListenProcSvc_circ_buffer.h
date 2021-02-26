#ifndef _LISTENPROCSVC_CIRC_BUFFER_H_
#define _LISTENPROCSVC_CIRC_BUFFER_H_
/**
@file ListenProcSvc_circ_buffer.h
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

$Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_circ_buffer.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
3/11/14   SivaNaga      Created file.

========================================================================== */
/* System */
#include "qurt_elite.h"

typedef struct
{
  uint32_t buffer_size;       /** circular buffer size in bytes*/
  uint32_t read_index;        /** Pointer from where data has to be read*/
  uint32_t write_index;       /** Pointer to which data has to be written*/
  uint32_t unread_bytes;      /** number of unread bytes in circular buffer */
  int8_t *buffer_ptr;         /** Circular buffer starting pointer*/
}circ_buf_struct_t;

/** Enumerations for return values of circular buffer operations */
typedef enum {
   CIRCBUF_SUCCESS = 0,       /** Operation was successful */
   CIRCBUF_FAIL,              /** General failure */
   CIRCBUF_OVERFLOW,          /** Buffer overflowed */
   CIRCBUF_UNDERFLOW          /** Buffer underflowed */
} circbuf_result;

/*
 * Initializes the Circular buffer
 * param  : pointer to a circular buffer structure
 *          Pointer to the memory allocated to circular buffer
 *          Circular buffer size
 * functionality : Initializes the circular buffer parameters
 * return : circbuf_result
 */
circbuf_result circ_buf_init(circ_buf_struct_t *circ_buf_struct_ptr, int8_t *buf_ptr, uint32_t buffer_size);

/*
 * Read samples from the Circular buffer
 * param  : pointer to a circular buffer structure
 *          Pointer to which data has to be read from circular buffer
 *          number of bytes to be read
 * functionality : copies the samples_to_read number of samples to out_ptr
 * return : circbuf_result
 */
circbuf_result circ_buf_read(circ_buf_struct_t *circ_buf_struct_ptr, int8_t *out_ptr, uint32_t bytes_to_read);

/*
 * Write samples to the Circular buffer
 * param  : pointer to a circular buffer structure
 *          Pointer from which data has to be written to circular buffer
 *          number of bytes to write
 * functionality : Checks the available unread space in the circular and
 *                 copies the samples_to_write amount of samples to inp_ptr
 * return : circbuf_result
 */
circbuf_result circ_buf_write(circ_buf_struct_t *circ_buf_struct_ptr, int8_t *inp_ptr, uint32_t bytes_to_write);

/*
 * Resets the circular buffer
 * param  : pointer to a circular buffer structure
 * functionality : Resets the circular buffer
 * return : circbuf_result
 */
circbuf_result circ_buf_reset(circ_buf_struct_t *circ_buf_struct_ptr);

/*
 * Adjusts the read pointer
 * param  : pointer to a circular buffer structure
 * functionality : Moves the read pointer to a new position to start reading from write_ptr-bytes_to_adjust
 * return : circbuf_result
 */
circbuf_result circ_adjust_read(circ_buf_struct_t *circ_buf_struct_ptr,uint32_t bytes_to_adjust);

/*
 * Deinit the circular buffer
 * param  : pointer to a circular buffer structure
 * functionality : Deinits the circular buffer
 * return : circbuf_result
 */
circbuf_result circ_buf_deinit(circ_buf_struct_t *circ_buf_struct_ptr);

#endif /*_LISTENPROCSVC_CIRC_BUFFER_H_*/
