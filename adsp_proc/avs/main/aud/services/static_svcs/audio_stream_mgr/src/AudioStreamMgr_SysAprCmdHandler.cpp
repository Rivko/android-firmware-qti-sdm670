/**
@file AudStreamMgr_SysAprCmdHandler.cpp*************

@brief This file contains the implementation for System*
APR cmdQ Handler functions for******************
AudioStreamMgr.*********

*/

/*========================================================================
$Header $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/06/10   KK      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_SysAprCmdHandler.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_Util.h"
#include "EliteMem_Util.h"
#include "EliteTopology_db_if.h"
#include "AdspCoreSvc.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/* =======================================================================
**        System APR CmdQ State Machine: CmdQ message handler
** ======================================================================= */
extern uint32_t asmMemoryMapClient;

/* This is to process SC command.
    RULE OF THUMB: final consumer (leaf function)
    generate ACK and free memory.
 */
ADSPResult AudioStreamMgr_SysQAprPktHandler( AudioStreamMgr_t *pMe)
{

   ADSPResult rc = ADSP_EOK;

   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (pMe->msg.pPayload);

   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );


   switch ( ulOpCode ) {

   case ASM_CMD_SHARED_MEM_MAP_REGIONS:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: Processing ASM_CMD_SHARED_MEM_MAP_REGIONS Command");
         rc = elite_mem_shared_memory_map_regions_cmd_handler(asmMemoryMapClient,AudioStreamMgr_GetAprHandle(), pPkt, ASM_CMDRSP_SHARED_MEM_MAP_REGIONS);
         break;
      }

   case ASM_CMD_SHARED_MEM_UNMAP_REGIONS:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: Processing ASM_CMD_SHARED_MEM_UNMAP_REGIONS Command");
         rc = elite_mem_shared_memory_un_map_regions_cmd_handler(asmMemoryMapClient,AudioStreamMgr_GetAprHandle(), pPkt);
         break;
      }

   case ASM_CMD_ADD_TOPOLOGIES:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: Processing ASM_CMD_ADD_TOPOLOGIES Command");
         rc = AudioStreamMgr_AddTopologiesHandler(pMe);
         break;
      }
   case ASM_CMD_REGISTER_FOR_ADSPPM_VOTES:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: Processing ASM_CMD_REGISTER_FOR_ADSPPM_VOTES Command");
         rc = AudioStreamMgr_RegisterForAdsppmVotes(pMe);
         break;
      }
   case ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTES:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: Processing ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTES Command");
         rc = AudioStreamMgr_DeregisterFromAdsppmVotes(pMe);
         break;
      }
   case ASM_CMD_GET_ADSPPM_VOTES:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: Processing ASM_CMD_GET_ADSPPM_VOTES Command");
         rc = AudioStreamMgr_GetAdsppmVotes(pMe);
         break;
      }
   default:
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: unsupported command 0x%lx at port [0x%lx]",
               ulOpCode, (uint32_t) ( elite_apr_if_get_dst_port( pPkt ) ));
         rc = ADSP_EUNSUPPORTED;
         // Generate ACK and free apr packet
         AudioStreamMgr_GenerateAck( pPkt, rc, NULL, 0, 0);
      }
   }

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_AddTopologiesHandler(AudioStreamMgr_t *pMe)
{
    ADSPResult result = ADSP_EOK;

    elite_apr_packet_t *pAprPacket = (elite_apr_packet_t*) (pMe->msg.pPayload);

    asm_cmd_add_topologies_t *pPayload = (asm_cmd_add_topologies_t*)elite_apr_if_get_payload_ptr(pAprPacket);

    // Map the buffer
    uint32_t phyAddrLsw = (uint32_t)(pPayload->data_payload_addr_lsw);
    uint32_t phyAddrMsw = (uint32_t)(pPayload->data_payload_addr_msw);

    static const uint32_t CACHE_LINE_SIZE = 32; // Must be a power of 2
    static const uint32_t CACHE_ALIGNMENT_MASK = (CACHE_LINE_SIZE - 1);
    bool_t isAlignedToCacheLine = ((phyAddrLsw & CACHE_ALIGNMENT_MASK) == 0);

    if (!isAlignedToCacheLine)
    {
       result = ADSP_EBADPARAM;

       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Physical address is not aligned to cache line.");
       goto done;
    }

    uint8_t *pVirtAddr;
    {
       elite_mem_shared_memory_map_t bufferMemNode;
       bufferMemNode.unMemMapHandle = pPayload->mem_map_handle;
       bufferMemNode.unMemMapClient = asmMemoryMapClient;
       result =  elite_mem_map_get_shm_attrib(phyAddrLsw, phyAddrMsw, pPayload->buffer_size, &bufferMemNode);
       if (ADSP_FAILED(result))
       {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Failed to map physical memory.");
          goto done;
       }

       // Since this buffer will be read, need to invalidate the cache.
       result = elite_mem_invalidate_cache(&bufferMemNode);
       if (ADSP_FAILED(result))
       {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Failed to invalidate memory.");
          goto done;
       }

       pVirtAddr = (uint8_t*)(bufferMemNode.unVirtAddr);
    }

    {
       uint32_t buffer_size = pPayload->buffer_size;

       if (buffer_size < sizeof(audproc_custom_topologies_t))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Size of custom topologies buffer %lu bytes is insufficient.", buffer_size);
          result = ADSP_ENEEDMORE;
          goto done;
       }

       audproc_custom_topologies_t *pHeader = (audproc_custom_topologies_t*)(pVirtAddr);
       pVirtAddr += sizeof(audproc_custom_topologies_t);
       buffer_size -= sizeof(audproc_custom_topologies_t);

       uint32_t size_required = pHeader->num_topologies * sizeof(audproc_topology_definition_t);
       if (buffer_size < size_required)
       {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Size of custom topologies array %lu bytes is insufficient. %lu bytes needed", buffer_size, size_required);
          result = ADSP_ENEEDMORE;
          goto done;
       }

       result = topo_db_add_topologies(TOPO_DB_CLIENT_TYPE_POPP, pHeader->num_topologies, (const audproc_topology_definition_t*)pVirtAddr);
       if (ADSP_FAILED(result))
       {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Failed to insert all topologies for POPP into the database.");
          goto done;
       }

       result = topo_db_add_topologies(TOPO_DB_CLIENT_TYPE_POPREP, pHeader->num_topologies, (const audproc_topology_definition_t*)pVirtAddr);
       if (ADSP_FAILED(result))
       {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Failed to insert all topologies for POPREP into the database.");
          goto done;
       }
    }

 done:
    result = AudioStreamMgr_GenerateAck(pAprPacket, result, NULL, 0, 0 );

    return result;
}


ADSPResult AudioStreamMgr_RegisterForAdsppmVotes(AudioStreamMgr_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pMe->msg.pPayload);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: calling qurt_elite_adsppm_wrapper_begin_profiling from ASM");

   result = qurt_elite_adsppm_wrapper_begin_profiling();

   result = AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );

   return result;
       }

ADSPResult AudioStreamMgr_DeregisterFromAdsppmVotes(AudioStreamMgr_t *pMe)
       {
   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pMe->msg.pPayload);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: calling qurt_elite_adsppm_wrapper_end_profiling from ASM");

   result = qurt_elite_adsppm_wrapper_end_profiling();

   result = AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );

   return result;
}

ADSPResult AudioStreamMgr_GetAdsppmVotes(AudioStreamMgr_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pMe->msg.pPayload);
   avcs_component_adsppm_voting_info_t info;
   asm_cmdrsp_get_adsppm_votes_t votes;
   memset(&votes, 0, sizeof(asm_cmdrsp_get_adsppm_votes_t));

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: calling qurt_elite_adsppm_wrapper_aggregate from ASM");

   qurt_elite_adsppm_wrapper_aggregate(&info, NULL, NULL, NULL);
   if (info.mask|AVCS_COMPONENT_ADSPPM_VOTING_MASK__MIPS_CHANGED_MASK)
   {
      votes.event_mask |= ASM_CMDRSP_ADSPPM_VOTE_MASK_MIPS;
      votes.mips_per_thread = info.mips_per_thread;
      votes.total_mips = info.total_mips;
      }
   if (info.mask|AVCS_COMPONENT_ADSPPM_VOTING_MASK__ADSP_DDR_BW_CHANGED_MASK)
      {
      votes.event_mask |= ASM_CMDRSP_ADSPPM_VOTE_MASK_BW;
      votes.bw_adsp_ddr_Bps_lsw = info.ab_adsp_ddr_Bps_lsw;
      votes.bw_adsp_ddr_Bps_msw = info.ab_adsp_ddr_Bps_msw;
      votes.total_mips = info.total_mips;
      }
   votes.event_mask |= ASM_CMDRSP_ADSPPM_VOTE_MASK_LATENCY;
   votes.latency_request = pMe->adsppm.prev_latency_req_rel;
   votes.latency_us = pMe->adsppm.prev_latency_vote;

   result = AudioStreamMgr_GenerateAck( pPkt, result, &votes, sizeof(asm_cmdrsp_get_adsppm_votes_t),
         ASM_CMDRSP_GET_ADSPPM_VOTES );

   return result;
}
