/*===========================================================================
                             drv_lookup.c

DESCRIPTION:
    This file implements DRV lookup based on chip version

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#include "drv_mapping_internal.h"
#include "ChipInfo.h"

/**
 * <!-- get_version_drv_id -->
 *
 * @brief Lookup the appropriate drv_id based on chip version 
 *
 * @param drv_id_v1  : The v1 drv id that we want to lookup
 * @param ret_drv_id : The new drv_id based on chip version
 *
 * @return DRV_ID_LOOKUP_SUCCESS if the lookup was successful. 
 *          If it failed for any reason, DRV_ID_LOOKUP_INVALID
 */ 
drv_id_lookup_err get_version_drv_id(RSCSW_DRV_GLOBAL_V1_MAPPING drv_id_v1, uint32 *ret_drv_id)
{
  if(drv_id_v1 >= RSC_DRV_GLOBAL_MAX)
  {
    return DRV_ID_LOOKUP_INVALID;
  }
  else
  {
    ChipInfoVersionType version = ChipInfo_GetChipVersion();
    uint32 major_version = (version >> 16);

    if((major_version <= 1) || (NUM_VERSIONS == 0))
    {
      *ret_drv_id = (uint32)drv_id_v1;     
      //special case for DRVs not on v1 but on v2:
      if(drv_id_v1 >= RSC_DRV_GLOBAL_RESERVED)
      {
        return DRV_ID_LOOKUP_INVALID;
      }	      
    }
    else
    {
      major_version = MIN(major_version, NUM_VERSIONS); 	    
      *ret_drv_id = drv_version_mapping[major_version - 1][(uint32)drv_id_v1];

      //special case for DRVs on v1 but on v2:
      if(*ret_drv_id == 0xFFFFFFFF)
      {
        return DRV_ID_LOOKUP_INVALID;
      }
    }
  }
  return DRV_ID_LOOKUP_SUCCESS;
}
