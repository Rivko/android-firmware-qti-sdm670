/**
 * @file ddr_shared_params.c
 * @brief
 * DDR related shared data between BOOT and AOP
 */
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/24/17   rp      Initial version
================================================================================
                   Copyright 2014-2017 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_drivers.h"
#include "ddr_params.h"
#include "HAL_SNS_DDR.h"
#include "ddr_external.h"
#include "CoreVerify.h"
#include "ddr_shared_params.h"
#include "boot_util.h"
#include "smem.h"
#include "CoreMsgRam.h"


/*==============================================================================
                                  MACROS
==============================================================================*/


/*==============================================================================
                                  DATA
==============================================================================*/


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* ============================================================================
**  Function : ddr_clock_query
** ============================================================================
*/
/*!
*   @brief
*   This function is called by clock driver to query ddr clock info
*
*   @details
*   Clock driver queries DDR driver to get all the MC and SHUB clock plan related info
*
*   @param
*   ddr_clock_query_type
*
*   @par Dependencies
*   Needs to be called after HAL_DDR_Init once DCB is parsed
*
*   @par Side Effects
*   None
*
*   @retval  struct ddr_clock_plan
*
*   @sa None
*/
ddr_clock_plan ddr_clock_query(ddr_clock_query_type query)
{
	ddr_clock_plan mc_shub_plan;
	switch (query)
	{
		case MC_CLOCK_PLAN:
			mc_shub_plan.num_levels = (sizeof(ddrsns_share_data->aop_params.mc_states)/sizeof(ddrsns_share_data->aop_params.mc_states[0]));
			mc_shub_plan.state = &ddrsns_share_data->aop_params.mc_states[0];
			break;
		
		case SHUB_CLOCK_PLAN:
			mc_shub_plan.num_levels = (sizeof(ddrsns_share_data->aop_params.shub_states)/sizeof(ddrsns_share_data->aop_params.shub_states[0]));
			mc_shub_plan.state = &ddrsns_share_data->aop_params.shub_states[0];
			break;
			
		default:
			mc_shub_plan.num_levels = 0;
			mc_shub_plan.state = NULL;
	}
	return mc_shub_plan;
} /* ddr_clock_query */

/* ============================================================================
**  Function : ddr_share_data_to_aop
** ============================================================================
*/
/*!
*   @brief
*   This function is called to copy all the ddr related info required by AOP
*
*   @details
*   All the DDR related information required by AOP during runtime is copied 
*   into SMEM in this function
*
*   @param
*   None
*
*   @par Dependencies
*   SMEM should have been initialized before calling this function
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void ddr_share_data_to_aop(void)
{
  uint32 i, smem_size;
  ddr_smem_info *ddr_smem_info_ptr;
  uint16 size_info[MAX_NUM_TABLES];

  size_info[MC_STATES] = sizeof(ddrsns_share_data->aop_params.mc_states);
  size_info[SHUB_STATES] = sizeof(ddrsns_share_data->aop_params.shub_states);
  size_info[DEFAULT_ALC_POLICY] = sizeof(ddrsns_share_data->aop_params.default_alc_policy);
  size_info[ALC_TIMINGS] = sizeof(ddrsns_share_data->aop_params.alc_timings);
  
  /* Calculate the total smem size requirement */
  smem_size = sizeof(ddr_smem_info);
  for (i = 0; i < MAX_NUM_TABLES; i++)
  {
    smem_size += size_info[i];
  }
  
  void *smem_ptr = smem_alloc(DDR_AOP_SMEM_ID, smem_size);
  CORE_VERIFY(smem_ptr != NULL); 
  
  /* Store smem address in AOP msg_ram */
  msg_ram_set_smem_address((uint32)DDR_AOP_SMEM_ID, (uint32)smem_ptr);
  
  ddr_smem_info_ptr = smem_ptr;
  ddr_smem_info_ptr->major_version = AOP_PARAMS_MAJOR_VERSION;
  ddr_smem_info_ptr->minor_version = AOP_PARAMS_MINOR_VERSION;
  
  /* Store all the sizes for all the smem_entries */
  for (i = 0; i < MAX_NUM_TABLES; i++)
  {
    ddr_smem_info_ptr->shared_table_header[i].size = size_info[i];
  }

  /* Calculate offsets for all the smem entries */
  ddr_smem_info_ptr->shared_table_header[MC_STATES].offset = sizeof(ddr_smem_info);
  for (i = 1; i < MAX_NUM_TABLES; i++)
  {
    ddr_smem_info_ptr->shared_table_header[i].offset = ddr_smem_info_ptr->shared_table_header[i-1].size + ddr_smem_info_ptr->shared_table_header[i-1].offset;
  }
  
  /* Ensure that the data fits within the allocated smem size */
  CORE_VERIFY(ddr_smem_info_ptr->shared_table_header[MAX_NUM_TABLES-1].offset + ddr_smem_info_ptr->shared_table_header[MAX_NUM_TABLES-1].size == smem_size);

  /* Copy mc_states struct to smem */
  qmemscpy(smem_ptr+ddr_smem_info_ptr->shared_table_header[MC_STATES].offset, ddr_smem_info_ptr->shared_table_header[MC_STATES].size, 
           &ddrsns_share_data->aop_params.mc_states[0], ddr_smem_info_ptr->shared_table_header[MC_STATES].size);
  
  /* Copy shub_states struct to smem */
  qmemscpy(smem_ptr+ddr_smem_info_ptr->shared_table_header[SHUB_STATES].offset, ddr_smem_info_ptr->shared_table_header[SHUB_STATES].size, 
           &ddrsns_share_data->aop_params.shub_states[0], ddr_smem_info_ptr->shared_table_header[SHUB_STATES].size);
  
  /* Copy default alc_policy to smem */
  qmemscpy(smem_ptr+ddr_smem_info_ptr->shared_table_header[DEFAULT_ALC_POLICY].offset, ddr_smem_info_ptr->shared_table_header[DEFAULT_ALC_POLICY].size, 
           &ddrsns_share_data->aop_params.default_alc_policy, ddr_smem_info_ptr->shared_table_header[DEFAULT_ALC_POLICY].size);
  
  /* Copy alc_timings to smem */
  qmemscpy(smem_ptr+ddr_smem_info_ptr->shared_table_header[ALC_TIMINGS].offset, ddr_smem_info_ptr->shared_table_header[ALC_TIMINGS].size, 
           &ddrsns_share_data->aop_params.alc_timings[0], ddr_smem_info_ptr->shared_table_header[ALC_TIMINGS].size);

} /* ddr_share_data_to_aop */