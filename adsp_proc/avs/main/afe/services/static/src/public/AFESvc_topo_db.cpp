/** @file AFESvc_topo_db.cpp

 @brief This file contains AFE Service static topology definitions

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/public/AFESvc_topo_db.cpp#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/09/2014 mangeshk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2014 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AFEInternal.h"
#include "AFESvc_topo_db.h"
#include "adsp_amdb_static.h"
#include "adsp_afe_service_commands.h"
#include "adsp_core_api.h"
#include "EliteCmnTopology_db.h"

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))


static avcs_module_info_t avcs_afe_none_mod_def =
{ };

static avcs_topology_definition_t avcs_afe_tx_topo_def_none =
{
   0,
   AFE_TX_TOPOLOGY_ID_NONE,
   1 << AVCS_TOPO_CFG_AREA_AFE_BIT,
   0
};


static avcs_topology_definition_t avcs_afe_rx_topo_def_none =
{
   0,
   AFE_RX_TOPOLOGY_ID_NONE,
   1 << AVCS_TOPO_CFG_AREA_AFE_BIT,
   0
};


static avcs_topology_definition_t avcs_afe_static_topos[] =
{
   avcs_afe_tx_topo_def_none,
   avcs_afe_rx_topo_def_none
};

static avcs_module_info_t *avcs_afe_static_topos_mods_ptr[] =
{
   &avcs_afe_none_mod_def,
   &avcs_afe_none_mod_def 
};

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

ADSPResult afe_add_static_topos_to_cmn_db(void)
{
   ADSPResult result = ADSP_EOK;

   for (uint32_t i = 0; i < SIZE_OF_ARRAY(avcs_afe_static_topos); i++)
   {
      avcs_topology_definition_t *topo_def_ptr = &avcs_afe_static_topos[i];
      avcs_module_info_t *mod_def_ptr = avcs_afe_static_topos_mods_ptr[i];

      //Add single topology
      result = elite_cmn_topo_db_add_topology(topo_def_ptr, mod_def_ptr, ELITE_CMN_STATIC_TOPOLOGIES);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_add_topology failed with %d", result);
         return result;
      }
   }

   return result;
}

ADSPResult afe_add_static_topos_and_mods_to_cmn_db(void)
{
   ADSPResult result;

   result = afe_add_static_topos_to_cmn_db();
   if (ADSP_FAILED(result))
   {
      return result;
   }

   return result;
}


ADSPResult afe_add_topologies_handler(elite_apr_packet_t *pkt_ptr)
{
   ADSPResult result = ADSP_EOK, rc = ADSP_EOK;

   afe_cmd_add_topologies_t *pPayload = (afe_cmd_add_topologies_t *)elite_apr_if_get_payload_ptr(pkt_ptr);

   // Map the buffer
   uint32_t phyAddrLsw = (uint32_t)(pPayload->data_payload_addr_lsw);
   uint32_t phyAddrMsw = (uint32_t)(pPayload->data_payload_addr_msw);

   static const uint32_t CACHE_LINE_SIZE = 32; // Must be a power of 2
   static const uint32_t CACHE_ALIGNMENT_MASK = (CACHE_LINE_SIZE - 1);
   bool_t isAlignedToCacheLine = ((phyAddrLsw & CACHE_ALIGNMENT_MASK) == 0);

   if (!isAlignedToCacheLine)
   {
      result = ADSP_EBADPARAM;

      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE: Physical address is not aligned to cache line.");
      goto done;
   }

   uint8_t *pVirtAddr;
   {
      elite_mem_shared_memory_map_t bufferMemNode;
      bufferMemNode.unMemMapHandle = pPayload->mem_map_handle;
      bufferMemNode.unMemMapClient = afe_svc_global_ptr->afe_memory_map_client;
      result =  elite_mem_map_get_shm_attrib(phyAddrLsw, phyAddrMsw, pPayload->buffer_size, &bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE: Failed to map physical memory.");
         goto done;
      }

      // Since this buffer will be read, need to invalidate the cache.
      result = elite_mem_invalidate_cache(&bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE: Failed to invalidate memory.");
         goto done;
      }

      pVirtAddr = (uint8_t *)(bufferMemNode.unVirtAddr);
   }

   {
      uint32_t buffer_size = pPayload->buffer_size;

      if (buffer_size < sizeof(audproc_custom_topologies_t))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE: Size of custom topologies buffer %lu bytes is insufficient.", buffer_size);
         result = ADSP_ENEEDMORE;
         goto done;
      }

      audproc_custom_topologies_t *pHeader = (audproc_custom_topologies_t *)(pVirtAddr);
      pVirtAddr += sizeof(audproc_custom_topologies_t);
      buffer_size -= sizeof(audproc_custom_topologies_t);

      uint32_t size_required = pHeader->num_topologies * sizeof(audproc_topology_definition_t);
      if (buffer_size < size_required)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE: Size of custom topologies array %lu bytes is insufficient. %lu bytes needed", buffer_size, size_required);
         result = ADSP_ENEEDMORE;
         goto done;
      }

      result = topo_db_add_topologies(TOPO_DB_CLIENT_TYPE_AFE, pHeader->num_topologies, (const audproc_topology_definition_t *)pVirtAddr);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE: Failed to insert all topologies for AFE into the database.");
         goto done;
      }

   }

done:
   if (APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_add_topologies_handler: Failed in elite_apr_if_end_cmd() with return code: %d", rc);
   }

   return result;
}
