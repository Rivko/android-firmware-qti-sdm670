/**
@file EliteMem_HeapId.h

@brief This file contains macro definitions for mapping buffer identifiers to heap IDs.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
Copyright (c) 2010, 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/inc/EliteMem_HeapId.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) Updated Doxygen comment for name group; added
                   condition commands.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup from 2.4.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
11/08/10   aprasad Created file.
========================================================================== */


#ifndef ELITEMEM_HEAPID_H
#define ELITEMEM_HEAPID_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "EliteMem.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** @addtogroup shared_mem_buf_heap_map
@{ */
/** @name 
The following macro definitions map buffer identifiers to heap IDs.
@{ */

#define ADEC_SVC_OBJ_INSTANCE             elite_mem_l2_heap_id
#define ADEC_SVC_THRD_STACK               elite_mem_l2_heap_id
#define ADEC_SVC_OUT_BUF                  elite_mem_l2_heap_id

/** @cond OEM_only
*/
#define MP3_DEC_LIB                       elite_mem_l2_heap_id
#define ADEC_SVC_INTERNAL_BIT_STREAM_BUF  elite_mem_l2_heap_id
#define AAC_DEC_LIB                       elite_mem_l2_heap_id
#define AAC_DEC_THRD_STACK                elite_mem_l2_heap_id
#define MP3DEC_24BIT_MEMORY               elite_mem_l2_heap_id
#define AACDEC_24BIT_MEMORY               elite_mem_l2_heap_id
/** @endcond
*/

#define PP_SVC_POPP_OBJ_INSTANCE          elite_mem_l2_heap_id
#define PP_SVC_POPP_THRD_STACK            elite_mem_l2_heap_id
#define PP_SVC_POPP_OUT_BUF               elite_mem_l2_heap_id
#define PP_SVC_POPP_ALWAYS_ENABLED_ALGS   elite_mem_l2_heap_id
#define PP_SVC_TMP_BUF                    elite_mem_l2_heap_id

#define PP_SVC_COPP_OBJ_INSTANCE          elite_mem_l2_heap_id
#define PP_SVC_COPP_THRD_STACK            elite_mem_l2_heap_id
#define PP_SVC_COPP_OUT_BUF               elite_mem_l2_heap_id
#define PP_SVC_COPP_ALWAYS_ENABLED_ALGS   elite_mem_l2_heap_id
#define MTMX_SVC_INPORT_LOCAL_BUF         elite_mem_l2_heap_id
#define MTMX_SVC_OUTPORT_BUF              elite_mem_l2_heap_id
#define MTMX_SVC_OUTPORT_ACC_BUF          elite_mem_l2_heap_id
#define MTMX_SVC_PORT_STRUCTURE           elite_mem_l2_heap_id

#define AFE_LSM_LOW_POWER_MODULE          elite_mem_l2_heap_id
#define AFE_SVC_AUD_VOICE_LIMITER_LIB     elite_mem_l2_heap_id
#define AFE_SINK_ADD_BUF                  elite_mem_l2_heap_id
#define AFE_SINK_COPY_BUF                 elite_mem_l2_heap_id
#define AFE_SINK_ADD32_BUF                elite_mem_l2_heap_id
#define AFE_SINK_COPY32_BUF               elite_mem_l2_heap_id

#define AFE_DMA_BUF_HEAP_ID               elite_mem_afe_dma_buf_heap_id
#define VOICE_HEAP_ID                     elite_mem_voice_heap_id

/** @} */ /* end_name */
/** @} */ /* end_addtogroup shared_mem_buf_heap_map */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMEM_HEAPID_H

