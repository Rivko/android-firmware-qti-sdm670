/*==========================================================================
ELite Source File

This file implements the topology database for the AVS.

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/21/14   mk      Created file.
=========================================================================== */

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#ifndef TST_TOPOLOGY_DB
#include "qurt_elite.h"
#else
#include "tst_topology_db.h"
#endif

#include "EliteTopology_db_if.h"

//#include "AFESvc_db_if.h"
//#include "AudDynaPPSvc.h"


/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/
static const uint32_t MAX_TOPOLOGIES = 128;
static const uint32_t MIN_TOPOLOGY_ID = 0x10000000;
static const uint32_t MAX_TOPOLOGY_ID = 0x1FFFFFFF;

struct topo_db_entry_t
{
   elite_topo_db_client_type client_type;
   audproc_topology_definition_t def;
};

struct topo_db_t
{
   uint32_t num_topologies;
   qurt_elite_mutex_t db_lock;
   topo_db_entry_t *record[MAX_TOPOLOGIES];
};

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
static ADSPResult topo_db_find_topology(const topo_db_t *me_ptr, elite_topo_db_client_type client_type, uint32_t topology_id, uint32_t *index_ptr);
static bool_t topo_db_is_valid_topology(const audproc_topology_definition_t *def_ptr);
static bool_t is_valid_topology_id(const uint32_t topology_id);
static ADSPResult topo_db_add_single_topology(topo_db_t *me_ptr, elite_topo_db_client_type client_type, const audproc_topology_definition_t *topology_definitions_ptr);

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
topo_db_t g_topology_db;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

ADSPResult topo_db_init()
{
   topo_db_t *me_ptr = &g_topology_db;

   me_ptr->num_topologies = 0;

   qurt_elite_mutex_init(&me_ptr->db_lock);

   memset(&me_ptr->record, 0, sizeof(me_ptr->record));

   return ADSP_EOK;
}

void topo_db_deinit()
{
   topo_db_t *me_ptr = &g_topology_db;

   for (uint32_t i = 0; i < me_ptr->num_topologies; i++)
   {
      qurt_elite_memory_free(me_ptr->record[i]);
   }

   qurt_elite_mutex_destroy(&me_ptr->db_lock);

   me_ptr->num_topologies = 0;
}

ADSPResult topo_db_add_topologies(elite_topo_db_client_type client_type, uint32_t num_topologies, const audproc_topology_definition_t topology_definitions[])
{
   topo_db_t *me_ptr = &g_topology_db;

   ADSPResult result = ADSP_EOK;

   qurt_elite_mutex_lock(&me_ptr->db_lock);

   // Loop over all topologies
   for (uint32_t i = 0; i < num_topologies; i++)
   {
      // Validate the topology ID
      if (!is_valid_topology_id(topology_definitions[i].topology_id))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topology ID %lu is outside the allowed range %lu to %lu.", topology_definitions[i].topology_id, MIN_TOPOLOGY_ID, MAX_TOPOLOGY_ID);
         result = ADSP_EBADPARAM;
         continue;
      }

      ADSPResult errCode = topo_db_add_single_topology(me_ptr, client_type, &topology_definitions[i]);
      if (ADSP_FAILED(errCode))
      {
         result = errCode;
      }
   }

   qurt_elite_mutex_unlock(&me_ptr->db_lock);

   return result;
}

ADSPResult topo_db_add_internal_topologies(elite_topo_db_client_type client_type, uint32_t num_topologies, const audproc_topology_definition_t topology_definitions[])
{
   topo_db_t *me_ptr = &g_topology_db;

   ADSPResult result = ADSP_EOK;

   qurt_elite_mutex_lock(&me_ptr->db_lock);

   // Loop over all topologies
   for (uint32_t i = 0; i < num_topologies; i++)
   {
      ADSPResult errCode = topo_db_add_single_topology(me_ptr, client_type, &topology_definitions[i]);
      if (ADSP_FAILED(errCode))
      {
         result = errCode;
      }
   }

   qurt_elite_mutex_unlock(&me_ptr->db_lock);

   return result;
}

ADSPResult topo_db_get_topology(elite_topo_db_client_type client_type, uint32_t topology_id, const audproc_topology_definition_t **topology_ptr)
{
   topo_db_t *me_ptr = &g_topology_db;
   ADSPResult result;
   uint32_t index;

   qurt_elite_mutex_lock(&me_ptr->db_lock);

   result = topo_db_find_topology(me_ptr, client_type, topology_id, &index);
   if (ADSP_FAILED(result))
   {
      qurt_elite_mutex_unlock(&me_ptr->db_lock);
      return result;
   }

   *topology_ptr = &(me_ptr->record[index]->def);

   qurt_elite_mutex_unlock(&me_ptr->db_lock);

   return result;
}

ADSPResult topo_db_is_module_present(elite_topo_db_client_type client_type, uint32_t topology_id, uint32_t module_id)
{
   const audproc_topology_definition_t *def_ptr;
   // Get topology definition from topology ID
   ADSPResult result = topo_db_get_topology(client_type, topology_id, &def_ptr);
   if (ADSP_FAILED(result))
   {
      return result;
   }
   // Check for module id in topology definition
   result = ADSP_EFAILED;
   for (uint32_t i = 0; i < def_ptr->num_modules; i++)
   {
         if (module_id == def_ptr->module_info[i].module_id)
         {
            //MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Module id %lu found in topology %lu", def_ptr->module_info[i].module_id, def_ptr->topology_id);
            result = ADSP_EOK;
            break;
         }
   }

   return result;

}

static ADSPResult topo_db_find_topology(const topo_db_t *me_ptr, elite_topo_db_client_type client_type, uint32_t topology_id, uint32_t *index_ptr)
{
   for (uint32_t i = 0; i < me_ptr->num_topologies; i++)
   {
      if (client_type == me_ptr->record[i]->client_type)
      {
         if (topology_id == me_ptr->record[i]->def.topology_id)
         {
            *index_ptr = i;
            return ADSP_EOK;
         }
      }
   }

   return ADSP_ENOTEXIST;
}

static bool_t topo_db_is_valid_topology(const audproc_topology_definition_t *def_ptr)
{
   const uint32_t MAX_MODULES = sizeof(def_ptr->module_info)/sizeof(def_ptr->module_info[0]);

   if (def_ptr->num_modules > MAX_MODULES)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Number of modules %lu in topology ID %lu exceeds the max %lu.", def_ptr->num_modules, def_ptr->topology_id, MAX_MODULES);
      return FALSE;
   }

   for (uint32_t i = 0; i < def_ptr->num_modules; i++)
   {
      if (def_ptr->module_info[i].use_lpm > 1)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The use_lpm flag of the module id %lu of topology %lu is set to the invalid value of %lu.", def_ptr->module_info[i].module_id, def_ptr->topology_id, def_ptr->module_info[i].use_lpm);
         return FALSE;
      }
   }

   // Check for duplicates
   for (uint32_t i = 0; i < def_ptr->num_modules; i++)
   {
      for (uint32_t j = i + 1; j < def_ptr->num_modules; j++)
      {
         if (def_ptr->module_info[j].module_id == def_ptr->module_info[i].module_id)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Module id %lu is repeated in topology %lu", def_ptr->module_info[i].module_id, def_ptr->topology_id);
            return FALSE;
         }
      }
   }

   return TRUE;
}

static bool_t is_valid_topology_id(const uint32_t topology_id)
{
   if ((topology_id >= MIN_TOPOLOGY_ID) && (topology_id <= MAX_TOPOLOGY_ID))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

static ADSPResult topo_db_add_single_topology(topo_db_t *me_ptr, elite_topo_db_client_type client_type, const audproc_topology_definition_t *topology_definitions_ptr)
{
   if (!topo_db_is_valid_topology(topology_definitions_ptr))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Topology ID %lu is not well formed. It will not be stored.", topology_definitions_ptr->topology_id);
      return ADSP_EBADPARAM;
   }

   // Find if the topology exists
   uint32_t index;
   ADSPResult retVal;
   retVal = topo_db_find_topology(me_ptr, client_type, topology_definitions_ptr->topology_id, &index);
   if (ADSP_SUCCEEDED(retVal))
   {
      // Overwrite if it exists
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Topology ID %lu already exists. Overwriting with new definition.", topology_definitions_ptr->topology_id);
      me_ptr->record[index]->def = *topology_definitions_ptr;
   }
   else
   {
      // Check if we have space
      if (me_ptr->num_topologies >= MAX_TOPOLOGIES)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Maximum number of topologies stored. Cannot accept any more.");
         return ADSP_ENORESOURCE;
      }

      // Allocate and copy if we have space
      topo_db_entry_t *entry_ptr = (topo_db_entry_t*)qurt_elite_memory_malloc(sizeof(topo_db_entry_t), QURT_ELITE_HEAP_DEFAULT);
      if (NULL == entry_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for topology definition.");
         return ADSP_ENOMEMORY;
      }

      entry_ptr->client_type = client_type;
      entry_ptr->def = *topology_definitions_ptr;
      me_ptr->record[me_ptr->num_topologies] = entry_ptr;
      me_ptr->num_topologies++;
   }

   return ADSP_EOK;
}



