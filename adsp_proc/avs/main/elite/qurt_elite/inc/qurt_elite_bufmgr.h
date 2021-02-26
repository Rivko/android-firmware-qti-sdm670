/*======================================================================
Copyright (c) 2010-2011, 2013-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/**
@file qurt_elite_bufmgr.h
*/
/** @weakgroup weakf_qurt_elite_bufmgr_intro
The following functions are used to manage a set of pre-allocated buffers among
multiple clients:
 - qurt_elite_bufmgr_create()
 - qurt_elite_bufmgr_return_buf()
 - qurt_elite_bufmgr_destroy()
 - qurt_elite_bufmgr_poll_for_buffer()
 - qurt_elite_is_bufmgr_node()

The client is responsible for allocating the memory. The only constraint is
the managed buffers must be in multiples of 2 bytes, and they must be a
minimum of 8 bytes. Any number of each size buffer can be managed.

Following is the protocol for the client when using this buffer manager:
  -# Form an array of 32 integers, where the value of each a[n] is the number
     of buffers of size 2^n that the buffer manager is to manage.
  -# Call #QURT_ELITE_BUFMGR_REQUIRED_SIZE, passing in this array. \n
     This macro does a sanity check on the array for validity, and it returns
     the memory block size required to populate the buffer manager.
  -# Allocate the memory block from the specified memory pool.
  -# Call qurt_elite_bufmgr_create() with the allocated memory and 
     configuration array.
  -# When a buffer is needed, call qurt_elite_bufmgr_poll_for_buffer() along
     with a specified size.\n
     The function returns a node corresponding to the smallest available buffer
     that can accommodate the requested size. The node is 8 bytes: \n
     @vertspace{2}
       - The first 4 bytes are the pointer to the buffer.
       - The second 4 bytes are the qurt_elite_queue_t, where the node must
         be returned when the buffer is consumed.
  -# When the client finishes with a buffer, call qurt_elite_queue_push_back()
     to return the node to its queue.
  -# When finished, call qurt_elite_bufmgr_destroy() and free the
     memory block.

@note1hang Clients are trusted not to corrupt the nodes that are given to
           them and then returned to the buffer manager queues.

@note1hang Currently, if a buffer is requested but no buffers are available,
           ADSP_ENEEDMORE is returned. There is no blocking, and there is no
           setup for a signal on a free buffer.
@newpage
*/
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_bufmgr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16    sw     (Tech Pubs) General edits in Doxygen comments.
05/11/15    sw     (Tech Pubs) Edited Doxygen hyperlinks for 8952.
09/05/14    sw     (Tech Pubs) Merged Doxygen comments from 2.4; modified
                   discussion above for 2.6.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
02/04/10   mwc     Created file.
03/09/10   brs     Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_BUFMGR_H
#define QURT_ELITE_BUFMGR_H
#include "qurt_elite_types.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_bufmgr
@{ */

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/** Metadata for freed buffers.

  When freed, all buffers managed by the buffer manager require extra
  metadata for the following:
  - To help return the buffers to their proper place in the buffer manager
  - To detect double-free scenarios
*/
#define QURT_ELITE_BUFMGR_METADATA_SIZE ((uint32_t)(4 * sizeof(uint32_t)))

/** Macro that computes the memory size needed for a specified set of buffers
  to be managed.

  Each input argument (b0, b1, b2, etc.) is the number of buffers of
  size 2^n that are required.
*/
#define QURT_ELITE_BUFMGR_REQUIRED_SIZE(b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, \
   b11, b12, b13, b14, b15, b16, b17, b18, b19, b20, b21, b22, b23, b24, b25, b26, \
   b27, b28, b29, b30, b31) \
   ((b0)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 0)) + \
   (b1)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 1)) + \
   (b2)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 2)) + \
   (b3)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 3)) + \
   (b4)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 4)) + \
   (b5)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 5)) + \
   (b6)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 6)) + \
   (b7)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 7)) + \
   (b8)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 8)) + \
   (b9)  * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 9)) + \
   (b10) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 10)) + \
   (b11) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 11)) + \
   (b12) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 12)) + \
   (b13) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 13)) + \
   (b14) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 14)) + \
   (b15) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 15)) + \
   (b16) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 16)) + \
   (b17) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 17)) + \
   (b18) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 18)) + \
   (b19) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 19)) + \
   (b20) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 20)) + \
   (b21) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 21)) + \
   (b22) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 22)) + \
   (b23) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 23)) + \
   (b24) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 24)) + \
   (b25) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 25)) + \
   (b26) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 26)) + \
   (b27) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 27)) + \
   (b28) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 28)) + \
   (b29) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 29)) + \
   (b30) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 30)) + \
   (b31) * (QURT_ELITE_BUFMGR_METADATA_SIZE + (1 << 31)))


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/* Forward declaration. */
typedef struct qurt_elite_bufbin_t qurt_elite_bufbin_t;

/* Forward declaration. */
typedef struct qurt_elite_queue_t qurt_elite_queue_t;

/* Forward declaration. */
typedef struct qurt_elite_bufmgr_node_t qurt_elite_bufmgr_node_t;

/* Forward declaration. */
typedef struct qurt_elite_bufmgr_t qurt_elite_bufmgr_t;

/** Node that represents a buffer. When clients request a buffer from the
    buffer manager, this node is provided.
 */
struct qurt_elite_bufmgr_node_t {
   QURT_ELITE_ALIGN(qurt_elite_queue_t*, 8) pReturnQ;
   /**< Queue to which this buffer must be returned. */

   void*                   pBuffer;
   /**< Pointer to the buffer. @newpage */
};

/**
  Creates the buffer manager. This function also performs a sanity check on
  the sizes of the requested set of buffers.

  @param[in]  nBufSizeArray Pointer to an array of 32 integers. \n
                            nBufSizeArray[n] is the number of buffers of size
                            2^n bytes to be managed by the buffer manager. The
                            first three elements must be 0 because 8 is the
                            minimum supported buffer size.
  @param[in]  pStartAddr    Pointer to the starting address of the memory for
                            the buffer pool. To create the buffer
                            manager described by nBufSizeArray, the caller must
                            allocate the buffer pool before calling this
                            function.
  @param[in]  size          Size of the buffer pool in bytes.
  @param[out] ppBufMgr      Double pointer to the buffer manager created if
                            this function returns ADSP_EOK.

  @return
  ADSP_EBADPARAM -- Invalid nBufSizeArray (e.g., a request for buffers smaller
                    than 8 bytes).
  @par
  ADSP_EOK -- nBufSizeArray is valid. The buffer manager is created and its
              pointer is put in the ppBufMgr parameter.
  @par
  For more information, refer to @xrefcond{Q1,80-NF774-4,80-NA610-4}.

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_bufmgr_create(const uint32_t* nBufSizeArray, char* pStartAddr, uint32_t size, qurt_elite_bufmgr_t **ppBufMgr);

/**
  Takes the address of a managed buffer, looks up the metadata, and pushes the
  address to the home queue.

  @param[in] pBuf  Pointer to the buffer manager instance.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the buffer manager must be created, and it must
  have polled for a buffer.
*/
ADSPResult qurt_elite_bufmgr_return_buf(void *pBuf);

/**
  Waits for all managed buffers to be returned to their queues, and
  then destroys all resources.

  @datatypes
  qurt_elite_bufmgr_t

  @param[in] pBufMgr  Pointer to the buffer manager instance.

  @return
  None.

  @dependencies
  Before calling this function, the buffer manager must be created. @newpage
*/
void qurt_elite_bufmgr_destroy(qurt_elite_bufmgr_t *pBufMgr);

/**
  Requests a buffer from the manager. If a buffer of adequate size is
  available, a node is returned with pointers to the buffer and the return
  queue of the buffer.

  @datatypes
  qurt_elite_bufmgr_node_t \n
  qurt_elite_bufmgr_node_t

  @param[in]  pBufMgr       Pointer to the buffer manager instance.
  @param[in]  nDesiredSize  Number of bytes needed in the requested buffer.
  @param[out] pNode         Returned node that points to the buffer and
                            return queue.
  @param[out] pnActualSize  Pointer to the actual size of the buffer that is
                            becoming available.

  @return
  ADSP_ENEEDMORE -- No buffers of adequate size are available.
  @par
  ADSP_EOK -- A buffer was found and is becoming available.
  @par
  For more information, refer to @xrefcond{Q1,80-NF774-4,80-NA610-4}.

  @dependencies
  Before calling this function, the buffer manager must be created. @newpage
*/
ADSPResult qurt_elite_bufmgr_poll_for_buffer(qurt_elite_bufmgr_t *pBufMgr, int nDesiredSize,
   qurt_elite_bufmgr_node_t *pNode, int* pnActualSize);

/**
  Determines if the buffer manager allocated this buffer.

  @datatypes
  qurt_elite_queue_t

  @param[in] bufmgr_ptr    Pointer to the buffer manager instance.
  @param[in] return_q_ptr  Pointer to the return queue of the buffer.
  @param[in] buf_ptr       Pointer to the buffer.
 
  @return
  TRUE  -- The buffer manager allocated this buffer.
  @par
  FALSE -- The buffer manager did not allocate this buffer.

  @dependencies
  Before calling this function, the buffer manager must be created.
*/
bool_t qurt_elite_is_bufmgr_node(qurt_elite_bufmgr_t *bufmgr_ptr,
                                 qurt_elite_queue_t* return_q_ptr, 
                                 void* buf_ptr);

/** @} */ /* end_addtogroup qurt_elite_bufmgr */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_BUFMGR_H

