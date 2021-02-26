/**
@file 

@brief 

*/

/*========================================================================
$Header: 

Edit History

when       who             what, where, why
--------   ---             ----------------------------------------------
09/15/2014  sabdagir       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef VOICE_MODULE_TOPO_DB_ID_H
#define VOICE_MODULE_TOPO_DB_ID_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite.h"
#include "adsp_vparams_internal_api.h"
#include "adsp_vparams_api.h"
#include "adsp_vpm_api.h"
#include "adsp_amdb_static.h"
#include "voice_capi_utils.h"
#include "EliteCmnTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#include "adsp_core_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef avcs_module_instance_info_t voice_module_t;

typedef struct voice_topo_db_entry_t
{
   avcs_topology_definition_t definition;
   avcs_module_info_t* modules;
} voice_topo_db_entry_t;

// Module definition table for CAPIv2 modules
typedef struct voice_capi_v2_module_definition_t
{
   uint32_t module_id;
   capi_v2_get_static_properties_f get_static_properties_f;
   capi_v2_init_f init_f;
} voice_capi_v2_module_definition_t;


ADSPResult voice_update_topo_information(uint32_t topo_id, elite_cmn_topo_db_entry_t** topo_entry_ptr, uint32_t* actual_size_ptr, uint32_t* handle_ptr);

ADSPResult voice_release_topo_information(uint32_t handle);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //VOICE_MODULE_TOPO_DB_ID_H
