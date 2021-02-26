/*==========================================================================
EliteCmnTopology_db.cpp

This file implements the common global topology database.

   Copyright (c) 2015, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

Edit History

when        who    what, where, why
--------        ---      ---------------------------------------------------------
03/13/15   KR     Created file.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "EliteCmnTopology_db_if.h"
#include "EliteCmnTopology_db.h"

#define UINT32_MAX  0xffffffff

typedef struct elite_cmn_topo_db_internal_entry_t elite_cmn_topo_db_internal_entry_t;

struct elite_cmn_topo_db_internal_entry_t
{
	uint32_t ref_count;
	bool_t is_active;
  elite_cmn_topo_type_t topo_type;
	uint32_t handle;
	elite_cmn_topo_db_internal_entry_t *next_ptr;
	uint32_t size;
	elite_cmn_topo_db_entry_t topo_entry;
};

struct elite_cmn_topo_db_t
{
	uint32_t handle_generator;
	qurt_elite_mutex_t db_lock;
	elite_cmn_topo_db_internal_entry_t *record_head_ptr;
	elite_cmn_topo_db_internal_entry_t *record_tail_ptr;
};

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

static void elite_cmn_topo_db_destroy_db_entry(void);
static ADSPResult elite_cmn_topo_db_process_check_remove_topology(elite_cmn_topo_db_internal_entry_t *curr_ptr,elite_cmn_topo_db_internal_entry_t *prev_ptr);


/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
elite_cmn_topo_db_t g_cmn_topology_db;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

void elite_cmn_topo_db_init()
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	qurt_elite_mutex_init(&me_ptr->db_lock);
	me_ptr->handle_generator = 0;
	me_ptr->record_head_ptr = NULL;
	me_ptr->record_tail_ptr = NULL;
}

void elite_cmn_topo_db_deinit()
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	elite_cmn_topo_db_destroy_db_entry();
	qurt_elite_mutex_destroy(&me_ptr->db_lock);
	me_ptr->handle_generator = 0;
	me_ptr->record_head_ptr = NULL;
	me_ptr->record_tail_ptr = NULL;
}

ADSPResult elite_cmn_topo_db_add_topology(avcs_topology_definition_t *topology_definition_ptr, void *module_definition_ptr,elite_cmn_topo_type_t topo_type)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	avcs_module_instance_info_t *temp_module_definition_dest_ptr;
	ADSPResult result = ADSP_EOK;
	uint32_t internal_total_size = 0, external_total_size = 0;
	uint32_t topo_version = 0;

	//Acquire mutex lock on database.
	qurt_elite_mutex_lock(&me_ptr->db_lock);

	topo_version = topology_definition_ptr->version;

	//Check for zero-module case
	if(0 == topology_definition_ptr->num_modules)
	{
		//zero-module case
		internal_total_size = sizeof(elite_cmn_topo_db_internal_entry_t);
		external_total_size = sizeof(elite_cmn_topo_db_entry_t) - sizeof(avcs_module_instance_info_t);
	}
	else
	{
		//non-zero-module case
		internal_total_size = sizeof(elite_cmn_topo_db_internal_entry_t) + (sizeof(avcs_module_instance_info_t)*(topology_definition_ptr->num_modules - 1));
		external_total_size = sizeof(elite_cmn_topo_db_entry_t) + (sizeof(avcs_module_instance_info_t)*(topology_definition_ptr->num_modules - 1));
	}

	//Allocate memory for the elite_cmn_topo_db_internal_entry_t entry.
	elite_cmn_topo_db_internal_entry_t *entry_ptr = (elite_cmn_topo_db_internal_entry_t*)qurt_elite_memory_malloc(internal_total_size, QURT_ELITE_HEAP_DEFAULT);
	if (NULL == entry_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_add_topology: Failed to allocate memory for topology definition: elite_cmn_topo_db_internal_entry_t");
		result = ADSP_ENOMEMORY;
		qurt_elite_mutex_unlock(&me_ptr->db_lock);
		return result;
	}
	else
	{
#ifdef DEBUG_ELITE_COMMON_TOPO
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_add_topology: Malloc %lu bytes (internal_size) success for elite_cmn_topo_db_internal_entry_t node @ 0x%p",
				internal_total_size, entry_ptr);
#endif
	}

	//Populate avcs_topology_definition_t information.
	entry_ptr->topo_entry.topo_def.num_modules = topology_definition_ptr->num_modules;
	entry_ptr->topo_entry.topo_def.topo_config = topology_definition_ptr->topo_config;
	entry_ptr->topo_entry.topo_def.topology_id = topology_definition_ptr->topology_id;
	entry_ptr->topo_entry.topo_def.version = topology_definition_ptr->version;
	//Set the topo type to be the right topo type.
	entry_ptr->topo_type = topo_type;

	//Populate next_ptr and size.
	entry_ptr->next_ptr = NULL;
	entry_ptr->size = external_total_size; //These are the number of bytes the client would be concerned about (reading).

	//Update ref count and is_active flag.
	entry_ptr->ref_count = 1;
	entry_ptr->is_active = TRUE; //This flag will be used to distinguish de-registered nodes in the future

	//Update handle_generator. Valid handle values 1 <= handle <= UINT32_MAX - 1
	me_ptr->handle_generator = (UINT32_MAX - 1 == me_ptr->handle_generator) ? (1) : (me_ptr->handle_generator+1);

	//Update handle for this topology node.
	entry_ptr->handle = me_ptr->handle_generator;

	//Populate avcs_module_instance_info_t information.
	temp_module_definition_dest_ptr = &(entry_ptr->topo_entry.module_info[0]);
	if(0 == topology_definition_ptr->num_modules)
	{
		temp_module_definition_dest_ptr->module_id = 0;
		temp_module_definition_dest_ptr->instance_id = 0;
	}
	else
	{
		if(topo_version == 0)
		{
			avcs_module_info_t *temp_module_definition_src_ptr = (avcs_module_info_t*)module_definition_ptr;
			for (uint32_t j = 0; j < topology_definition_ptr->num_modules; j++)
			{
				temp_module_definition_dest_ptr->module_id = temp_module_definition_src_ptr->module_id;
				temp_module_definition_dest_ptr->instance_id = 0; //hardcoding instance id to zero. Topo version remains zero.
				temp_module_definition_src_ptr++;
				temp_module_definition_dest_ptr++;
			}
		}
		else
		{
			avcs_module_instance_info_t *temp_module_definition_src_ptr = (avcs_module_instance_info_t*)module_definition_ptr;
			for (uint32_t j = 0; j < topology_definition_ptr->num_modules; j++)
			{
				temp_module_definition_dest_ptr->module_id = temp_module_definition_src_ptr->module_id;
				temp_module_definition_dest_ptr->instance_id = temp_module_definition_src_ptr->instance_id;
				temp_module_definition_src_ptr++;
				temp_module_definition_dest_ptr++;
			}
		}
	}

	//Add node into linked list.
	if(NULL == me_ptr->record_tail_ptr)
	{
		me_ptr->record_head_ptr = entry_ptr;
		me_ptr->record_tail_ptr = entry_ptr;
	}
	else
	{
		me_ptr->record_tail_ptr->next_ptr = entry_ptr;
		me_ptr->record_tail_ptr = entry_ptr;
	}
#ifdef DEBUG_ELITE_COMMON_TOPO

	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_add_topology: success: topo_id %lx added, area_bit_mask %lx, external_size: %lu, #modules: %lu, new_ref_count: %lu",
			entry_ptr->topo_entry.topo_def.topology_id, (entry_ptr->topo_entry.topo_def.topo_config & 0x0000000f), entry_ptr->size,
			entry_ptr->topo_entry.topo_def.num_modules, entry_ptr->ref_count);
#endif
  //Release mutex lock on database.
  qurt_elite_mutex_unlock(&me_ptr->db_lock);
  return result;
}

ADSPResult elite_cmn_topo_db_deregister_all_custom_topologies(void)
{
  elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
  ADSPResult result = ADSP_EOK;
  //Acquire mutex lock on database.
  qurt_elite_mutex_lock(&me_ptr->db_lock);

  elite_cmn_topo_db_internal_entry_t  *curr_ptr, *prev_ptr, *next_ptr;
  curr_ptr = me_ptr->record_head_ptr;
  prev_ptr = NULL;
  next_ptr = NULL;
  while(NULL != curr_ptr)
  {
    next_ptr = curr_ptr->next_ptr;
    if( ELITE_CMN_CUSTOM_TOPOLOGIES  ==  curr_ptr->topo_type && curr_ptr->is_active)
    {
      curr_ptr->is_active = FALSE;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_deregister_topology: deregister successful. handle: %lu, topo_id: %lx, ref_count: %lu",
            curr_ptr->handle, curr_ptr->topo_entry.topo_def.topology_id, curr_ptr->ref_count);
      //At this point if ref_count becomes 1 and this node is marked for deletion (de-register), go ahead and delete the node.
      //Else, this node will not be removed, and we can therefore move prev ptr to here.
      if(ADSP_ENOTREADY == elite_cmn_topo_db_process_check_remove_topology(curr_ptr,prev_ptr))
      {
        prev_ptr = curr_ptr;
      }
    }
    else
    {
#ifdef DEBUG_ELITE_COMMON_TOPO
      //Remove all custom topologies until the end of topo db is reached.
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_deregister_topology:  Topology %lu is no longer active, continuing", curr_ptr->handle);
#endif
      prev_ptr = curr_ptr;
    }


    curr_ptr = next_ptr;

  }

	//Release mutex lock on database.
	qurt_elite_mutex_unlock(&me_ptr->db_lock);
	return result;
}
ADSPResult elite_cmn_topo_db_deregister_single_topology(uint32_t topology_id)
{
  elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
  ADSPResult   result = ADSP_EFAILED;

  //Acquire mutex lock on database.
  qurt_elite_mutex_lock(&me_ptr->db_lock);

  elite_cmn_topo_db_internal_entry_t  *curr_ptr, *prev_ptr,*next_ptr;
  curr_ptr = me_ptr->record_head_ptr;
  prev_ptr = NULL;
  next_ptr = NULL;
  while(NULL != curr_ptr)
  {
    next_ptr = curr_ptr->next_ptr;
    if(topology_id  == curr_ptr->topo_entry.topo_def.topology_id && ELITE_CMN_CUSTOM_TOPOLOGIES == curr_ptr->topo_type)
    {
      if(FALSE == curr_ptr->is_active)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_deregister_topology:  Topology %lu is no longer active, continuing", curr_ptr->handle);
      }
      else
      {
        curr_ptr->is_active = FALSE;
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_deregister_topology: deregister successful. handle: %lu, topo_id: %lx, ref_count: %lu",
              curr_ptr->handle, curr_ptr->topo_entry.topo_def.topology_id, curr_ptr->ref_count);
        //At this point if ref_count becomes 1 and this node is marked for deletion (de-register), go ahead and delete the node.
        elite_cmn_topo_db_process_check_remove_topology(curr_ptr,prev_ptr);
#ifdef DEBUG_ELITE_COMMON_TOPO
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_deregister_topology: head ptr %p, tail ptr %p",me_ptr->record_head_ptr,me_ptr->record_tail_ptr);
#endif
        result = ADSP_EOK;
        qurt_elite_mutex_unlock(&me_ptr->db_lock);
        return result;
      }
    }
    prev_ptr = curr_ptr;
    curr_ptr = next_ptr;
  }
  //If we reach here, it means the topo is not found. We will return an error here.
  //Release mutex lock on database.
  qurt_elite_mutex_unlock(&me_ptr->db_lock);
  return result;
}

ADSPResult elite_cmn_topo_db_get_topology(uint32_t area_bit_mask, uint32_t topology_id, elite_cmn_topo_db_entry_t **topology_ptr_ptr, uint32_t *actual_size_ptr, uint32_t *handle_ptr)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	ADSPResult result = ADSP_EOK;

	qurt_elite_mutex_lock(&me_ptr->db_lock);

	elite_cmn_topo_db_internal_entry_t  *curr_ptr;
	curr_ptr = me_ptr->record_head_ptr;
	while(NULL != curr_ptr)
	{
		if((topology_id == curr_ptr->topo_entry.topo_def.topology_id) &&
				(TRUE == curr_ptr->is_active) &&
				(area_bit_mask == (curr_ptr->topo_entry.topo_def.topo_config & 0x0000000f & area_bit_mask)))
		{
			//Wrap-around protection
			if(UINT32_MAX == curr_ptr->ref_count)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_get_topology: an active topo_id %lx was found with provided bitmask %lx. However, there are too many references.",
						topology_id, area_bit_mask);
				*handle_ptr = ELITE_CMN_TOPO_DB_INVALID_HANDLE;
				result = ADSP_EBADPARAM;
				qurt_elite_mutex_unlock(&me_ptr->db_lock);
				return result;
			}

			//Fill in details
			curr_ptr->ref_count++;
			*handle_ptr = curr_ptr->handle;
			*actual_size_ptr = curr_ptr->size;
			*topology_ptr_ptr = &(curr_ptr->topo_entry);

			MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_get_topology: an active topo_id %lx was found with provided bitmask %lx. actual_size: %lu, handle: %lu, node @0x%p, new_ref_count: %lu",
					topology_id, area_bit_mask, curr_ptr->size, curr_ptr->handle, &(curr_ptr->topo_entry), curr_ptr->ref_count);
			qurt_elite_mutex_unlock(&me_ptr->db_lock);
			return result;
		}
		else
		{
			curr_ptr = curr_ptr->next_ptr;
		}
	}

	//If code execution has reached here, then an active topology with the provided topology_id with provided area_bit_mask was not found. This is an erroneous use case.
	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_get_topology: an active topo_id %lx was not found with provided bitmask %lx.",
			topology_id, area_bit_mask);
	*handle_ptr = ELITE_CMN_TOPO_DB_INVALID_HANDLE;
	result = ADSP_EBADPARAM;
	qurt_elite_mutex_unlock(&me_ptr->db_lock);
	return result;
}

ADSPResult elite_cmn_topo_db_process_check_remove_topology(elite_cmn_topo_db_internal_entry_t *curr_ptr,elite_cmn_topo_db_internal_entry_t *prev_ptr)
{
  ADSPResult result = ADSP_ENOTREADY;
  elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
if(NULL == curr_ptr)
{
  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_check_remove_topology: ERROR: curr_ptr is null, exiting.");
  goto __bailout;
}

  if (FALSE == curr_ptr->is_active && 1 == curr_ptr->ref_count && ELITE_CMN_CUSTOM_TOPOLOGIES == curr_ptr->topo_type)
  {
    //Remove the next ptr of the current pointer from linked list
    if(curr_ptr == me_ptr->record_head_ptr )//Indicates the node to remove is the first node.
    {
      me_ptr->record_head_ptr = curr_ptr->next_ptr;
    }
    if(curr_ptr == me_ptr->record_tail_ptr)
    {
      me_ptr->record_tail_ptr = prev_ptr;
    }
    if(prev_ptr != NULL)
    {
      prev_ptr->next_ptr = curr_ptr->next_ptr;
    }
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_check_remove_topology: freeing ptr at  %p", curr_ptr);
    qurt_elite_memory_free(curr_ptr);
    result = ADSP_EOK;
  }
  __bailout:
  return result;
}

ADSPResult elite_cmn_topo_db_release_topology(uint32_t handle)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	ADSPResult result = ADSP_EOK;

	if(ELITE_CMN_TOPO_DB_INVALID_HANDLE == handle || 0 == handle)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_release_topology: Invalid handle %lu provided", handle);
		result = ADSP_EBADPARAM;
		return result;
	}

	qurt_elite_mutex_lock(&me_ptr->db_lock);

  elite_cmn_topo_db_internal_entry_t  *curr_ptr, *prev_ptr, *next_ptr;
	curr_ptr = me_ptr->record_head_ptr;
  prev_ptr = NULL;
  next_ptr = NULL;
	while(NULL != curr_ptr)
	{
    next_ptr = curr_ptr->next_ptr;
		if(handle == curr_ptr->handle)
		{
			//Wrap-around protection. There should always be one ref_count (after all client releases). Check done prior to decrementing ref_count.
			if(1 >= curr_ptr->ref_count)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_release_topology: Too many releases on handle %lu", curr_ptr->handle);
				result = ADSP_EBADPARAM;
			}
			else
			{
				curr_ptr->ref_count--;
				MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_release_topology: release successful. handle: %lu, topo_id: %lx, new ref_count: %lu",
						curr_ptr->handle, curr_ptr->topo_entry.topo_def.topology_id, curr_ptr->ref_count);
			}

      //At this point if ref_count becomes 1 and this node is marked for deletion (de-register), go ahead and delete the node.
      elite_cmn_topo_db_process_check_remove_topology(curr_ptr,prev_ptr);

			qurt_elite_mutex_unlock(&me_ptr->db_lock);
			return result;
		}
		else
		{
      prev_ptr = curr_ptr;
      curr_ptr = next_ptr;
		}
	}

	//If code execution has reached here, then the provided handle was not found. This is an erroneous use case.
	MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_release_topology: release failed. handle: %lu does not exist", handle);
	result = ADSP_EBADPARAM;
	qurt_elite_mutex_unlock(&me_ptr->db_lock);
	return result;
}


ADSPResult elite_cmn_topo_db_is_topology_unique(uint32_t topology_id)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	ADSPResult result = ADSP_EOK;

	qurt_elite_mutex_lock(&me_ptr->db_lock);

	elite_cmn_topo_db_internal_entry_t  *curr_ptr;
	curr_ptr = me_ptr->record_head_ptr;
	while(NULL != curr_ptr)
	{
		if(topology_id == curr_ptr->topo_entry.topo_def.topology_id && TRUE == curr_ptr->is_active)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_is_topology_unique: an active topo_id %lx already exists. It is not unique.", topology_id);
			result = ADSP_EBADPARAM;
			qurt_elite_mutex_unlock(&me_ptr->db_lock);
			return result;
		}
		else
		{
			curr_ptr = curr_ptr->next_ptr;
		}
	}

	//If code execution has reached here, then the provided topology was not found. Therefore it is unique.
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_is_topology_unique: not an active topo_id %lx. It is unique.", topology_id);
	qurt_elite_mutex_unlock(&me_ptr->db_lock);
	return result;
}

ADSPResult elite_cmn_topo_db_is_active_custom_topology_found(uint32_t topology_id)
{
  elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
   ADSPResult result = ADSP_EBADPARAM;

   qurt_elite_mutex_lock(&me_ptr->db_lock);

   elite_cmn_topo_db_internal_entry_t  *curr_ptr;
   curr_ptr = me_ptr->record_head_ptr;
   while(NULL != curr_ptr)
   {
     if(topology_id == curr_ptr->topo_entry.topo_def.topology_id && TRUE == curr_ptr->is_active && ELITE_CMN_CUSTOM_TOPOLOGIES == curr_ptr->topo_type)
     {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_is_active_custom_topology_found: an active custom topo_id %lx is found.", topology_id);
       result = ADSP_EOK;
       qurt_elite_mutex_unlock(&me_ptr->db_lock);
       return result;
     }
     else
     {
       curr_ptr = curr_ptr->next_ptr;
     }
   }

   //If code execution has reached here, then the provided topology was not found. Return error.
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_is_active_custom_topology_found: no active custom topo_id %lx is found here.", topology_id);
   qurt_elite_mutex_unlock(&me_ptr->db_lock);
   return result;
}

ADSPResult elite_cmn_topo_db_is_module_present(uint32_t area_bit_mask, uint32_t topology_id, uint32_t module_id)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	ADSPResult result = ADSP_EOK;

	qurt_elite_mutex_lock(&me_ptr->db_lock);

	elite_cmn_topo_db_internal_entry_t  *curr_ptr;
	curr_ptr = me_ptr->record_head_ptr;
	while(NULL != curr_ptr)
	{
#ifdef DEBUG_ELITE_COMMON_TOPO

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_is_module_present: current topology id %lx bitmask %lx. addr %p ",
          curr_ptr->topo_entry.topo_def.topology_id, curr_ptr->topo_entry.topo_def.topo_config, curr_ptr);
#endif
		if((topology_id == curr_ptr->topo_entry.topo_def.topology_id) &&
				(TRUE == curr_ptr->is_active) &&
				(area_bit_mask == (curr_ptr->topo_entry.topo_def.topo_config & 0x0000000f & area_bit_mask)))
		{

			avcs_module_instance_info_t *temp_module_definition_ptr = &(curr_ptr->topo_entry.module_info[0]);
			for (uint32_t j = 0; j < curr_ptr->topo_entry.topo_def.num_modules; j++)
			{
				if(module_id == temp_module_definition_ptr->module_id)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_is_module_present: an active topo_id %lx was found with provided bitmask %lx. mod_id %lx found.",
							topology_id, area_bit_mask, module_id);
					qurt_elite_mutex_unlock(&me_ptr->db_lock);
					return result;
				}
				else
				{
					temp_module_definition_ptr++;
				}
			}

			//If code execution has reached here, then an active topology with the provided topology_id with provided area_bit_mask was found, but module was not found. This is an erroneous use case.
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_is_module_present: an active topo_id %lx was found with provided bitmask %lx. but mod_id %lx was not found.",
					topology_id, area_bit_mask, module_id);
			result = ADSP_EBADPARAM;
			qurt_elite_mutex_unlock(&me_ptr->db_lock);
			return result;
		}
		else
		{
			curr_ptr = curr_ptr->next_ptr;
		}
	}

	//If code execution has reached here, then an active topology with the provided topology_id with provided area_bit_mask was not found. This is an erroneous use case.
	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_is_module_present: an active topology %lx was not found with provided bitmask %lx.",
			topology_id, area_bit_mask);
	result = ADSP_EBADPARAM;
	qurt_elite_mutex_unlock(&me_ptr->db_lock);
	return result;
}

ADSPResult elite_cmn_topo_db_is_active_topology_present(uint32_t area_bit_mask, uint32_t topology_id)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	ADSPResult result = ADSP_EOK;

	qurt_elite_mutex_lock(&me_ptr->db_lock);

	elite_cmn_topo_db_internal_entry_t  *curr_ptr;
	curr_ptr = me_ptr->record_head_ptr;
	while(NULL != curr_ptr)
	{
		if((topology_id == curr_ptr->topo_entry.topo_def.topology_id) &&
				(TRUE == curr_ptr->is_active) &&
				(area_bit_mask == (curr_ptr->topo_entry.topo_def.topo_config & 0x0000000f & area_bit_mask)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_is_active_topology_present: an active topo_id %lx was found with provided bitmask %lx",
					topology_id, area_bit_mask);
			qurt_elite_mutex_unlock(&me_ptr->db_lock);
			return result;
		}
		else
		{
			curr_ptr = curr_ptr->next_ptr;
		}
	}

	//If code execution has reached here, then an active topology with the provided topology_id with provided area_bit_mask was not found.
	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_is_module_present: an active topology %lx was not found with provided bitmask %lx.",
			topology_id, area_bit_mask);
	result = ADSP_EBADPARAM;
	qurt_elite_mutex_unlock(&me_ptr->db_lock);
	return result;
}

static void elite_cmn_topo_db_destroy_db_entry(void)
{
	elite_cmn_topo_db_t *me_ptr = &g_cmn_topology_db;
	elite_cmn_topo_db_internal_entry_t  *curr_ptr;
	elite_cmn_topo_db_internal_entry_t  *next_ptr;
	curr_ptr = me_ptr->record_head_ptr;
	while(NULL != curr_ptr)
	{
		next_ptr = curr_ptr->next_ptr;
#ifdef DEBUG_ELITE_COMMON_TOPO
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "elite_cmn_topo_db_destroy_db_entry: Freeing elite_cmn_topo_db_internal_entry_t node @ 0x%p", curr_ptr);
#endif
		qurt_elite_memory_free(curr_ptr);
		curr_ptr = next_ptr;
	}
}

ADSPResult elite_cmn_db_init(void)
{
	ADSPResult result;

	//Initialize database
	elite_cmn_topo_db_init();

	//Add audio PP static topologies followed by modules into this database
	result = aud_add_static_topos_and_mods_to_cmn_db();
	if (ADSP_FAILED(result))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_db_init: failed to aud_add_static_topos_and_mods_to_cmn_db with %d", result);
		elite_cmn_topo_db_deinit();
		return result;
	}

	//Adding AFE static topologies and by modules into common database
	result = afe_add_static_topos_and_mods_to_cmn_db();
	if (ADSP_FAILED(result))
	{
	  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_db_init: failed to afe_add_static_topos_and_mods_to_cmn_db with %d", result);
	  elite_cmn_topo_db_deinit();
	  return result;
	}

	//Adding LSM static topologies into common database
	result = lsm_add_static_topos_to_cmn_db();
	if (ADSP_FAILED(result))
	{
	  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_db_init: failed to lsm_add_static_topos_and_mods_to_cmn_db with %d", result);
	  elite_cmn_topo_db_deinit();
	  return result;
	}

	result = voice_add_static_topologies_and_modules_v2();
	if (ADSP_FAILED(result))
	{
	   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_db_init: failed to voice_add_static_topologies_and_modules_v2 with %d", result);
	   elite_cmn_topo_db_deinit();
	   return result;
	}

	return result;
}

void elite_cmn_db_deinit(void)
{
	elite_cmn_topo_db_deinit();
}

ADSPResult elite_cmn_topo_db_reset(void)
{
	ADSPResult result;
	elite_cmn_topo_db_deinit();
	elite_cmn_topo_db_init();
	result = aud_add_static_topos_to_cmn_db();
	if (ADSP_FAILED(result))
	{
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_reset: failed to aud_add_static_topos_to_cmn_db with %d", result);
            elite_cmn_topo_db_deinit();
            return result;
	}

	result = afe_add_static_topos_to_cmn_db();
	if (ADSP_FAILED(result))
	{
	  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_reset: failed to afe_add_static_topos_to_cmn_db with %d", result);
	  elite_cmn_topo_db_deinit();
	  return result;
	}

	result = lsm_add_static_topos_to_cmn_db();
	if (ADSP_FAILED(result))
	{
	  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_reset: failed to lsm_add_static_topos_to_cmn_db with %d", result);
	  elite_cmn_topo_db_deinit();
	  return result;
	}

	result = voice_add_static_topos_to_cmn_db();
	if (ADSP_FAILED(result))
	{
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_reset: failed to voice_add_static_topos_to_cmn_db with %d", result);
           elite_cmn_topo_db_deinit();
           return result;
	}

	return result;
}
