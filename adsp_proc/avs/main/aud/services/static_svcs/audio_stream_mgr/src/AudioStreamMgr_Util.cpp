/**
@file AudStreamMgr_Util.cpp

@brief This file contains the implementation of utility functions for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Util.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/10   DC      Created file.
11/10/10   RP      Added more utility functions
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_GetSetBits.h"

#include "adsp_error_codes.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
extern uint32_t asmMemoryMapClient;

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_GetProcInitParamFromEncConfig(
      PPOutputParams_t *pParam,
      uint32_t unFmtBlkId,
      void     *pFrmtBlk
)
{
   switch ( unFmtBlkId)
   {
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
   {
      asm_multi_channel_pcm_enc_cfg_v3_t *pEncConfig = (asm_multi_channel_pcm_enc_cfg_v3_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->num_channels;
      memscpy(pParam->channelMap, sizeof(pParam->channelMap), pEncConfig->channel_mapping,
            sizeof(pEncConfig->channel_mapping));
      return ADSP_EOK;
   }
   case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4:
   {
      asm_multi_channel_pcm_enc_cfg_v4_t *pEncConfig = (asm_multi_channel_pcm_enc_cfg_v4_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->num_channels;
      memscpy(pParam->channelMap, sizeof(pParam->channelMap), pEncConfig->channel_mapping,
            sizeof(pEncConfig->channel_mapping));
      return ADSP_EOK;
   }
   default:
   {
      return AudioStreamMgr_GetProcInitParamFromNonPcmEncConfig(pParam, unFmtBlkId, pFrmtBlk);
   }
   }
   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_GenerateProcInitParam( PPOutputParams_t *pParam,
      uint32_t unSamplingRate, /* 0 for keeping native sampling rate*/
      uint16_t usNumChannels   /* 0 for keeping native channel number */
)
{
   memset(pParam,0, sizeof(PPOutputParams_t));
   pParam->ulSampleRate = unSamplingRate;
   pParam->ulNumChannels = usNumChannels;

   return ADSP_EOK;
}

static bool_t ASM_StreamHasRxAprClient(AudioStreamMgr_StreamStateType *pStream)
{
   return ((ASM_TOPO_NT_MODE == pStream->pTopo->TopoId) ||
         (ASM_TOPO_RX_REGULAR == pStream->pTopo->TopoId) ||
         (ASM_TOPO_RX_ULL == pStream->pTopo->TopoId) ||
         (ASM_TOPO_RX_ULLPP == pStream->pTopo->TopoId) ||
         (ASM_TOPO_RX_GAPLESS == pStream->pTopo->TopoId) ||
         (ASM_TOPO_RX_COMPR == pStream->pTopo->TopoId) );
}

static bool_t ASM_StreamHasTxAprClient(AudioStreamMgr_StreamStateType *pStream)
{
   return ((ASM_TOPO_NT_MODE == pStream->pTopo->TopoId) ||
         (ASM_TOPO_TX_REGULAR == pStream->pTopo->TopoId) ||
         (ASM_TOPO_TX_COMPR == pStream->pTopo->TopoId) ||
         (ASM_TOPO_TX_LLNP == pStream->pTopo->TopoId));
}

ADSPResult AudioStreamMgr_RegisterQs(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK;
   /*  Register Dynamic Service: Data and Command Queue's to router
   This should be done before ACK back to client because once ACK back to client,
   new commands might come. */
   qurt_elite_queue_t   *pDataQ;

   /* Direction : 0 for register Rx data queue.1 for register Tx data queue */
   uint32_t   unDirection;

   elite_apr_port_t port = PORT_FROM_SESSION_STREAM(pSession, pStream);
   uint8_t stream_id = pStream->ucStreamId;

   //Register data queue for Rx direction, if any
   if ( ASM_StreamHasRxAprClient(pStream) )
   {
      {
         pDataQ = (pStream->nodes[ASM_NODE_DEC]).pSvcHandle->dataQ;
      }

      unDirection = AUDIO_STREAM_MGR_APR_ROUTER_RX_DIR;

      if ( ADSP_FAILED( result = AudioStreamMgr_AprRouterRegisterDatQ(port, unDirection, pDataQ) ) )
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: can not register [%lu, 0x%4x]",
               pSession->ucInternalSessionId,
               stream_id,
               unDirection, port );
         return result;
      }
   }

   if ( ASM_StreamHasTxAprClient(pStream) )
   {
      pDataQ = (pStream->nodes[ASM_NODE_ENC]).pSvcHandle->gpQ;
      unDirection = AUDIO_STREAM_MGR_APR_ROUTER_TX_DIR;

      // Generate ACK and free apr packet
      if ( ADSP_FAILED( result = AudioStreamMgr_AprRouterRegisterDatQ(port, unDirection, pDataQ) ) )
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: can not register [%lu, 0x%4x]",
               pSession->ucInternalSessionId, stream_id, unDirection, port );
         return result;
      }
   }

   //Register the PP callback
   //need to register PP callback handle even for second stream in gapless session
   if ( ADSP_FAILED( result = AudioStreamMgr_AprRouterRegisterPPCb(port, pStream->PPCallback) ) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: can not register PP callback [0x%4x]",
            pSession->ucInternalSessionId,
            stream_id,
            port );
      return result;
   }

   return result;
}

/* This function deregisters the decoder/encoder data queues from the APR
Also deregisters PP callback*/
ADSPResult AudioStreamMgr_DeregisterQs(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   /* Immediately deregister data queue and PP callback to prevent further data being send to
   dynamic serice such as dec/enc/PP */
   uint32_t   unDirection = 0xFFFFFFFF;
   ADSPResult result1 = ADSP_EOK;
   ADSPResult result2 = ADSP_EOK;
   ADSPResult result3 = ADSP_EOK;

   elite_apr_port_t port = PORT_FROM_SESSION_STREAM(pSession, pStream);

   if ( ASM_StreamHasRxAprClient(pStream) )
   {
      unDirection = AUDIO_STREAM_MGR_APR_ROUTER_RX_DIR;
      result1 = AudioStreamMgr_AprRouterDeRegisterDatQ ( port, unDirection);
   }

   if ( ASM_StreamHasTxAprClient(pStream) )
   {
      unDirection = AUDIO_STREAM_MGR_APR_ROUTER_TX_DIR;
      result2 = AudioStreamMgr_AprRouterDeRegisterDatQ ( port, unDirection);
   }

   result3 = AudioStreamMgr_AprRouterDeRegisterPPCb ( port );

   if ( ADSP_FAILED( result1 ) || ADSP_FAILED( result2 ) || ADSP_FAILED( result3 ))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: can not de-register [%lu, 0x%lx]",
            pSession->ucInternalSessionId,
            pStream->ucStreamId,
            unDirection, (uint32_t) port );

      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

ADSPResult ASM_ExecSuspendFTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   //get the suspend opcode
   uint32_t op_code = ASM_SuspendFTable[node_index].unOpcode;
   //get the matrix port direction (if applicable)
   uint8_t dir = ASM_SuspendFTable[node_index].unDir;
   //call the suspend handler for this node
   return ASM_SuspendFTable[node_index].func_ptr(pMe, pSession, pStream, op_code, dir);
}

ADSPResult ASM_ExecFlushFTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   //get the flush opcode
   uint32_t op_code = ASM_FlushFTable[node_index].unOpcode;
   //get the matrix port direction (if applicable)
   uint8_t dir = ASM_FlushFTable[node_index].unDir;
   //call the flush handler for this node
   return ASM_FlushFTable[node_index].func_ptr(pMe, pSession,pStream, op_code, dir);
}

ADSPResult ASM_ExecPause1FTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   //get the flush opcode
   uint32_t op_code = ASM_PauseFTable1[node_index].unOpcode;
   //get the matrix port direction (if applicable)
   uint8_t dir = ASM_PauseFTable1[node_index].unDir;
   //call the flush handler for this node
   return ASM_PauseFTable1[node_index].func_ptr(pMe, pSession, pStream, op_code, dir);
}

ADSPResult ASM_ExecPause2FTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   //get the flush opcode
   uint32_t op_code = ASM_PauseFTable2[node_index].unOpcode;
   //get the matrix port direction (if applicable)
   uint8_t dir = ASM_PauseFTable2[node_index].unDir;
   //call the flush handler for this node
   return ASM_PauseFTable2[node_index].func_ptr(pMe, pSession, pStream, op_code, dir);
}

ADSPResult ASM_ExecRunFTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   //get the flush opcode
   uint32_t op_code = ASM_RunFTable[node_index].unOpcode;
   //get the matrix port direction (if applicable)
   uint8_t dir = ASM_RunFTable[node_index].unDir;
   //call the flush handler for this node
   return ASM_RunFTable[node_index].func_ptr(pMe, pSession, pStream, op_code, dir);
}

ADSPResult ASM_ExecQueryDelayFTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   // get the pointer to the delay query table
   uint32_t table_index = static_cast<uint32_t>(pSession->unMode);
   if (table_index >= ASM_QueryDelayFTablesSize)
   {
      QURT_ELITE_ASSERT(0);
      table_index = ASM_QueryDelayFTablesSize - 1;
   }
   const ASM_RunPauseFlushFTableEntry *ASM_QueryDelayFTable = ASM_QueryDelayFTables[table_index];

   //get the flush opcode
   uint32_t op_code = ASM_QueryDelayFTable[node_index].unOpcode;
   //get the matrix port direction (if applicable)
   uint8_t dir = ASM_QueryDelayFTable[node_index].unDir;
   //call the flush handler for this node
   return ASM_QueryDelayFTable[node_index].func_ptr(pMe, pSession, pStream, op_code, dir);
}

/**
 * This function is specifically added for Pull Mode play back and push mode record
 * where, mem map info comes in ASM open command.
 * During close, mem unmap is done.
 *
 */
ADSPResult ASM_MemMapHandler(uint32_t nMemMapClient, uint16_t memPoolId,
      uint16_t propertyFlag, bool_t isCached, uint16_t numRegions,
      avs_shared_map_region_payload_t *pReg, uint32_t *memMapHandle)
{
   ADSPResult rc = ADSP_EOK;
   *memMapHandle = 0;
   QURT_ELITE_MEMORYPOOLTYPE eMemPool;

   switch (memPoolId)
   {
   case ADSP_MEMORY_MAP_SHMEM8_4K_POOL:
      eMemPool = QURT_ELITE_MEMORYMAP_SHMEM8_4K_POOL;
      break;
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM_MemMapHandler: Received Invalid PoolID: %d", memPoolId);
      return ADSP_EBADPARAM;
   }

   qurt_elite_memorymap_cache_mode_t cacheability = isCached ? HMEM_CACHE_WRITEBACK_L2CACHEABLE : HMEM_CACHE_NONE_SHARED;

   // allocate qurt_elite_memorymap_shm_region_t, why not direct cast? because want to be free from
   // api data structure change or any compiler pack
   qurt_elite_memorymap_shm_region_t* phy_regions;
   if (NULL == (phy_regions = (qurt_elite_memorymap_shm_region_t*) qurt_elite_memory_malloc(
         sizeof(qurt_elite_memorymap_shm_region_t) * numRegions,
         QURT_ELITE_HEAP_DEFAULT)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to allocate to shared memory region struct.It requires %lu bytes",sizeof(qurt_elite_memorymap_shm_region_t));
       return ADSP_ENOMEMORY;
   }

   for (int i = 0; i < numRegions; i++)
   {
      phy_regions[i].shm_addr_lsw = (pReg+i)->shm_addr_lsw;
      phy_regions[i].shm_addr_msw = (pReg+i)->shm_addr_msw;
      phy_regions[i].mem_size     = (pReg+i)->mem_size_bytes;
   }

   if (0 == propertyFlag)
   {
      //physical mapping
      if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_mem_map(nMemMapClient, phy_regions,
            numRegions,
            cacheability,
            eMemPool,
            memMapHandle)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM_MemMapHandler: Failed to map the physical memory, error code is 0x%x",rc );
         goto __bailout;
      }
   }
   else if (1 ==  propertyFlag)
   {
      //virtual mapping
      if (ADSP_FAILED(rc = qurt_elite_memorymap_virtaddr_mem_map(nMemMapClient,phy_regions,
            numRegions,
            cacheability,
            eMemPool,
            memMapHandle)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM_MemMapHandler: Failed to map the virual memory, error code is 0x%x",rc );
         goto __bailout;
      }
   }
   else
   {
      rc = ADSP_EBADPARAM;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM_MemMapHandler: invalid property flag received in the payload, error code is 0x%x",rc );
      goto __bailout;
   }

   __bailout:
   qurt_elite_memory_free(phy_regions);
   return rc;
}

ADSPResult ASM_MemUnmapHandler(uint32_t nMemMapClient, uint32_t memMapHandle)
{
   ADSPResult rc = ADSP_EOK;

   if (ADSP_FAILED(rc = qurt_elite_memorymap_shm_mem_unmap(nMemMapClient, memMapHandle)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM_MemUmapHandler: Failed to unmap the phyiscal memory, error code is 0x%x",rc );
   }
   return rc;
}

void AudioStreamMgr_AggregateBufDelay(AudioStreamMgr_StreamStateType *pStream, uint8_t unDir, uint32_t delay)
{
   if (ASM_TX_DIR == unDir)
   {
      pStream->pathRTBufDelayUs = (delay > pStream->pathRTBufDelayUs) ? delay : pStream->pathRTBufDelayUs;
   }
   else
   {
      pStream->pathFTRTBufDelayUs += delay;
   }
}

ADSPResult AudioStreamMgr_FinalizeGetPathDelayV2(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession, ADSPResult errorCode)
{
   uint64_t delay = 0;
   if (ADSP_SUCCEEDED(errorCode))
   {
      for (uint32_t j = 0; j < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; j++)
      {
         AudioStreamMgr_StreamStateType *pStream = &pSession->aStreams[j];
         if (ASM_STREAM_CLOSE == pStream->state)
         {
            continue;
         }
         //includes first gapless stream
         delay += pStream->pathFTRTBufDelayUs + pStream->pathRTBufDelayUs + pStream->pathAlgDelayUs;
      }
   }

   asm_session_cmdrsp_get_path_delay_v2_t  response;
   response.status = errorCode;
   response.audio_delay_lsw = (uint32_t)delay;
   response.audio_delay_msw = (uint32_t)(delay>>32);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: Session path delay query =%lu %lu us",
         pSession->ucInternalSessionId, response.audio_delay_msw, response.audio_delay_lsw);

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ADSPResult result =  AudioStreamMgr_GenerateAck( pPkt, errorCode, &response,
         sizeof(asm_session_cmdrsp_get_path_delay_v2_t),
         ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u, -]: failed to send response to get_path_delay. %d.",
            pSession->ucInternalSessionId, result);
   }

   return result;
}
