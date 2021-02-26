#ifndef DRV_LOOKUP_H
#define DRV_LOOKUP_H

#include "drv_mapping.h"

typedef enum
{
  DRV_ID_LOOKUP_SUCCESS = 0,
  DRV_ID_LOOKUP_INVALID = 1,
} drv_id_lookup_err;


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
drv_id_lookup_err get_version_drv_id(RSCSW_DRV_GLOBAL_V1_MAPPING drv_id_v1, uint32 *drv_id_v2);

#endif // DRV_LOOKUP_H

