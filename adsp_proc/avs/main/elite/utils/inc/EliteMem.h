#ifndef ELITEMEM_H
#define ELITEMEM_H

/**
@file EliteMem.h

@brief This file  contains the global memory management structure for
the Elite framework. This currently includes a global buffer
pool where Elite message can withdraw the payload buffer from.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013-2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/05/14   sw      (Tech Pubs) Edited Doxygen markup/comment for 2.6.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* system */
#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/** @addtogroup elitemm_datatypes
@{ */

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
/* Forward declaration for the elite_mem_global_pool_t structure. */
typedef struct elite_mem_global_pool_t elite_mem_global_pool_t;

/** Global memory resource structure used to track resources such as 
  command payload buffers. Tracking resources allows allocated memory to be
  reduced or freed.
 */
struct elite_mem_global_pool_t
{
   qurt_elite_bufmgr_t      *pEliteMemBufMgr; /**< Message buffer pool. */
};

/** Heap ID of the AFE DMA buffers used in low-power audio (LPA) playback. */
extern QURT_ELITE_HEAP_ID elite_mem_afe_dma_buf_heap_id;

/** Heap ID for voice memory, to be used if voice has a separate placement
    scheme. */
extern QURT_ELITE_HEAP_ID elite_mem_voice_heap_id;

/** ID of the LPA heap used to allocate dynamically allocated buffers. */
extern QURT_ELITE_HEAP_ID elite_mem_l2_heap_id;

/** ID of the low-power memory (LPM) heap used to allocate dynamically
    allocated buffers. */
extern QURT_ELITE_HEAP_ID elite_mem_lpm_heap_id;

/** Global memory pool. */
extern elite_mem_global_pool_t elite_mem_global_pool_global;

/** @} */ /* end_addtogroup elitemm_datatypes */

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

/** @ingroup elitemm_func_init
  Initializes the global Elite memory resource structure. This function is to
  be used once during bootup.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult elite_mem_init(void);

/** @ingroup elitemm_func_destroy
  Destroys the global Elite memory structure. This function is to be called
  before exiting to perform final cleanup.

  @return
  None.

  @dependencies
  None.
*/
void elite_mem_destroy(void);

/** @ingroup elitemm_func_get_buff
  Wrapper used to get one buffer from the Elite global memory pool.

  @param[in] nDesiredSize   Size of the buffer to be retrieved.
  @param[in] pNode          Pointer to the buffer node containing the buffer
                            pointer and buffer return queue.
  @param[in] pnActualSize   Pointer to the actual size of the buffer that is
                            retrieved. This size might be larger than what is
                            required.

  @return
  None.

  @dependencies
  None.
 */
static inline ADSPResult elite_mem_get_buffer( int nDesiredSize,
   qurt_elite_bufmgr_node_t *pNode, int* pnActualSize )
{
   return qurt_elite_bufmgr_poll_for_buffer( elite_mem_global_pool_global.pEliteMemBufMgr,
                                     nDesiredSize,
                                     pNode,
                                     pnActualSize) ;
}


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_PARAM_H

