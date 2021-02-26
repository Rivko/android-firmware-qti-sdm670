/**
@file EliteMem.cpp

@brief This file  contains the implementation for Elite
global memory pool management.

*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010-2017 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "EliteMem.h"
#include "EliteMem_Util.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
// Audio PD heap define for Multi-PD. Note: AUDIO_PD_HEAP_SIZE is defined in elite/utils/build/utils.scons
#ifndef USES_AUDIO_IN_ROOTPD
#ifdef USES_MPSS_MULTI_PD //ToDo: this is temp change to compile AVS.2.8 for 9x65 target
#define GCONST const __attribute__((section(".start")))
static char my_sbrk_heap[AUDIO_PD_HEAP_SIZE] __attribute__((aligned(4096))); 
extern "C" void * GCONST override_heap_Base = &my_sbrk_heap[0];                   // sys_sbrk looks for this symbol
extern "C" void * GCONST override_heap_Limit = &my_sbrk_heap[AUDIO_PD_HEAP_SIZE]; // sys_sbrk looks for this symbol
extern "C" unsigned GCONST heapSize = AUDIO_PD_HEAP_SIZE;                         // mem check script looks for this symbol
#endif
#endif
/* =======================================================================
**                          Function Definitions
** ======================================================================= */

//***************************************************************************
// Global state structure for resource monitoring & debugging
//***************************************************************************

// heap id of LPA heap used to allocate dynamic stream buffers
QURT_ELITE_HEAP_ID elite_mem_l2_heap_id = QURT_ELITE_HEAP_DEFAULT;

elite_mem_global_pool_t elite_mem_global_pool_global;


ADSPResult elite_mem_init()
{
   ADSPResult result;

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Enter elite_mem_init");

   /*
   This is a configuraton parameter for Elite global memory pool
   This is the distribution of different size buffer.
   It can be optimized based on Elite system load.
   Currernt configurations:
   16 uint8_t:  32 buffers
   32 uint8_t:  16 buffers
   64 uint8_t:  8 buffers
   128 uint8_t: 8 buffers
   256 uint8_t: 2 buffers
   512 uint8_t: 4 buffer
   */

   const uint32_t buf_bins[] = {0,0,0,0,32,16,8,8,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   static QURT_ELITE_ALIGN(char, 8) elite_mem_buf_blob_ptr\
         [QURT_ELITE_BUFMGR_REQUIRED_SIZE(0,0,0,0,32,16,8,8,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)];

   /* Initialized Elite global buffer manager*/

   /* init everything to zero */
   elite_mem_global_pool_global.pEliteMemBufMgr = NULL;

   if (ADSP_FAILED(result = qurt_elite_bufmgr_create(buf_bins, elite_mem_buf_blob_ptr, 
                                                     sizeof(elite_mem_buf_blob_ptr),
                                                     &elite_mem_global_pool_global.pEliteMemBufMgr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to create buffer manager!!\n");
      return ADSP_EFAILED;
   }

   /* Initializing heap manager for LPA heap */
   elite_mem_l2_heap_id = QURT_ELITE_HEAP_DEFAULT;
   if (0 != qurt_elite_globalstate.size_lpa_heap_from_ddr)
   {
      if (ADSP_FAILED(result = qurt_elite_memory_heapmgr_create(&elite_mem_l2_heap_id,
            qurt_elite_globalstate.ddr_lpa_heap, qurt_elite_globalstate.size_lpa_heap_from_ddr)))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to initialize heap manager for LPA heap!!\n");
         return ADSP_EFAILED;
      }
   }
#ifdef DBG_BUFFER_ADDRESSES
   if (0 != qurt_elite_globalstate.size_lpa_heap_from_ddr){
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR LPA heap: 0x%8x, size %d",
         elite_mem_lpa_stream_heap, qurt_elite_globalstate.size_lpa_heap_from_ddr);}
#endif /* DBG_BUFFER_ADDRESSES */

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Leave elite_mem_init");

   return ADSP_EOK;
}

void elite_mem_destroy()
{

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Enter elite_mem_destroy");

   // clean up all resources.
   qurt_elite_bufmgr_destroy(elite_mem_global_pool_global.pEliteMemBufMgr);
   elite_mem_global_pool_global.pEliteMemBufMgr = NULL;

   // delete heap manager for LPA heap
   qurt_elite_memory_heapmgr_delete(elite_mem_l2_heap_id);

   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO,"Leave elite_mem_destroy");

}

