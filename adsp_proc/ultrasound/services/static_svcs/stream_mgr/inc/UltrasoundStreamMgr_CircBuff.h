/*========================================================================*/
/**<
 @file UltrasoundStreamMgr_CircBuff.h
@brief This file contains Elite Ultrasound service circular  
 buffer components.

 Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/services/static_svcs/stream_mgr/inc/UltrasoundStreamMgr_CircBuff.h#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
 07/02/12   NR      Initial version
 01/07/13   NR      Fix compilation warnings
 07/23/13   LD      re-architecture: make circular buffer structure generic
========================================================================== */

#ifndef _ULTRASOUNDSVC_CIRC_BUFF_INC_H_
#define _ULTRASOUNDSVC_CIRC_BUFF_INC_H_
/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
/* System */
#include "qurt_elite.h"

/* Ultrasound and Audio */
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include <audio_basic_op_ext.h>
#include <Elite_CAPI.h>


/* -----------------------------------------------------------------------
 * Constant / Define Declarations
 * ----------------------------------------------------------------------- */

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
/** @brief Data structure for the circular/ring buffers that are maintained per channel
 * of data */
typedef struct
{
   uint64_t timestamp;
   /**< Timestamp that is filled in the ELITE_DATA_BUFFER by the sending port */
   int16_t *read_ptr;
   /**< Pointer to the next read operation */
   int16_t *write_ptr;
   /**< Pointer to the next write operation */
   int16_t *start_ptr;
   /**< Pointer to the start of the ring buffer */
   int16_t *end_ptr;
   /**< Pointer to the end of the ring buffer */
   int32_t unread_sample_cnt;
   /**< Number of unread samples in the ring buffer */
   int32_t max_buf_size;
   /**< Size of the ring buffer in samples */
   bool first_sample_recvd;
   /**< Indicates if first sample of the channel has been received or not
    */
} us_circ_buff_state_t;


/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/* 
  Reset the circular buffer structure to the same state as newly allocated,
  empty buffer.
 
  @param[in] circ_buf_ptr [in/out] This points to a circular buffer structure
 
  @return NONE
*/
void ultrasound_svc_circ_buff_reset(us_circ_buff_state_t *circ_buf_ptr);

/* 
  Init the members of the circular buffer structure. Allocates memory for
  the buffer data. Caller should allocate the structure itself in advance.
 
  @param[in] circ_buf_ptr [in/out] This points to a circular buffer structure
  @param[in] max_size Maximum size of the buffer in bytes
 
  @return ADSPResult, result code
*/
ADSPResult ultrasound_svc_circ_buff_init(us_circ_buff_state_t *circ_buf_ptr, uint32_t max_size);

/* 
  Deallocate memory allocated for members of the circular buffer structure.
  Does not free the structure itself.
 
  @param[in] circ_buf_ptr [in/out] This points to a circular buffer structure
 
  @return NONE
*/
void ultrasound_svc_circ_buff_deinit(us_circ_buff_state_t *circ_buf_ptr);

/* 
   This function is to adjust the read pointer and unread samples of ports data
 
  @param circ_buf_ptr [in/out] This points to the circular buffer structure
 
  @param[in] correction_in_samples, adjustment factor in samples
 
  @return NONE
*/
void ultrasound_svc_circ_buff_adjust_samples(us_circ_buff_state_t *circ_buf_ptr, 
                                             int32_t correction_in_samples);

/*
  This function is to copy channel data from source to the ring buffer
 
  @param[in] circ_buf_ptr, pointer to the per channel circular buffer
  @param[in] src_ptr, pointer to the samples received from port
  @param[in] copy_size, number of samples to copy to ring buffer
  @param[in] timestamp, if this this indeed the first samples of the buffer,
             this timestamp is saved as the buffer's timestamp
  @return NONE
*/
void ultrasound_svc_circ_buff_copy_to_buff
(
   us_circ_buff_state_t *circ_buf_ptr, 
   int16_t *src_ptr, 
   int32_t copy_size, 
   int64_t timestamp
);


/*
  This is a utility function to increment pointers in a circular fashion.
 
  @param[in] ptr, current pointer location
  @param[in] start_ptr, pointer to the start of ring buffer
  @param[in] end_ptr, pointer to the last location of ring buffer
  @param[in] incr_size, pointer incremenent value
 
  @param[out] ptr, pointer location after increment
 
  @return NONE
*/
void ultrasound_svc_circ_buff_increment
(
   int16_t **ptr, 
   int16_t *start_ptr,
   int16_t *end_ptr, 
   int32_t incr_size
);


/*
  This function is to copy channel data from circular buffer to output buffer
 
  @param[in] circ_buf_ptr, pointer to the per channel circular buffer
  @param[in] dst_ptr, pointer to library output buffer
  @param[in/out] dst_size - on input - available size in samples in dst_ptr. on output - actaull size copied
  @param[in] output_buf_available - is output buffer available. Actual copying is done only if this parameter
             is TRUE. If it is false all pointers are relocated as if copying is done, but the actuall copying
             is not performed. 
  @return NONE
*/
void ultrasound_svc_circ_buff_copy_from_buff
(
   us_circ_buff_state_t *circ_buf_ptr, 
   int16_t *dst_ptr, 
   uint32_t* dst_size,
   uint32_t output_buf_available
);

/*
  This function searches until the pattern of size pattern_size if found. All data until this pattern
  is found is deleted
 
  @param[in] circ_buf_ptr, pointer to the per channel circular buffer
  @param[in] pattern_ptr, pattern to match
  @param[in] pattern_size - pattern size in samples
  @param[out]adjusted_samples - output parametrs applicable even if pattern wasn't found. Number of samples that were read
  in the process of searching for the pattern
 
  @return ADSP_EOK - if pattern was matched. Read pointer will point to the beginning of the pattern
          ADSP_ENOTEXIST - if pattern wasn't found. In this case the buffer will be empty.
*/
ADSPResult ultrasound_svc_circ_buff_erase_until_pattern_match 
(
   us_circ_buff_state_t *circ_buf_ptr, 
   const uint16_t *pattern, 
   uint32_t patter_size, 
   uint32_t *adjusted_samples
);

/*
  This function creates a CAPI buffer list from the circular buffer data. 
  @param[in] circ_buf_ptr, pointer to the per channel circular buffer
  @param[out] capi_buf_list - output parameter, containing the number of buffers
             in th list and a list of data buffers. This list is dynamically
             allocated by this function and should be deleted when not used
             by the client.
  
  @return None
*/
void ultrasound_svc_circ_buff_create_CAPI_input_buffer 
(
   us_circ_buff_state_t *circ_buf_ptr,
   CAPI_BufList_t* capi_buf_list
);

/*
  This function returns the number of available samples in the buffer 
  @param[in] circ_buf_ptr, pointer to the per channel circular buffer
  
  @return Number of available samples in the buffer 
*/
uint32_t ultrasound_svc_circ_buff_get_available_samples (us_circ_buff_state_t *circ_buf_ptr);

/*
  This function clears the buffer content without dealocating the buffer memory
  @param[in] circ_buf_ptr, pointer to the per channel circular buffer
  
  @return None
*/
void ultrasound_svc_circ_buff_clear_content (us_circ_buff_state_t *circ_buf_ptr);
#endif //_ULTRASOUNDSVC_CIRC_BUFF_INC_H_ 
