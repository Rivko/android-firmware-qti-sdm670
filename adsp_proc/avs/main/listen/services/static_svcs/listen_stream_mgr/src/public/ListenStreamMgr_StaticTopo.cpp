/** @file ListenStreamMgr_StaticTopo.cpp

 @brief This file contains Listen Specific Topology management

 */

/*==============================================================================
 $Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/public/ListenStreamMgr_StaticTopo.cpp#1 $

 Edit History

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 6/13/2014  Unni    Initial version
 =============================================================================*/

/*------------------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

 -----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Include Files
 * ---------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "adsp_amdb_static.h"
#include "adsp_lsm_session_commands.h"
#include "adsp_lsm_service_commands.h"
#include "adsp_core_api.h"
#include "EliteCmnTopology_db.h"


/*------------------------------------------------------------------------------
 * Macro definitions
 * ---------------------------------------------------------------------------*/
#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

static avcs_module_info_t avcs_lsm_none_mod_def =
{
};

/* Static Topology definitions */
static avcs_topology_definition_t avcs_lsm_topo_def_none=
{
    0,
    LSM_TX_TOPOLOGY_ID_NONE,
    1 << AVCS_TOPO_CFG_AREA_LSM_BIT,
    0
};

/* Static Topology List */
static avcs_topology_definition_t avcs_lsm_static_topos[]=
{
    avcs_lsm_topo_def_none
};

static avcs_module_info_t *avcs_lsm_static_topos_mods_ptr[] =
{
    &avcs_lsm_none_mod_def
};


/*
  This function add static topology to centralized topology db

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
ADSPResult lsm_add_static_topos_to_cmn_db(void)
{
  ADSPResult result = ADSP_EOK;

  for (uint32_t i=0; i< SIZE_OF_ARRAY(avcs_lsm_static_topos); i++)
  {
    avcs_topology_definition_t *topo_def_ptr = &avcs_lsm_static_topos[i];
    avcs_module_info_t *mod_def_ptr = avcs_lsm_static_topos_mods_ptr[i];

    //Add single topology
    result = elite_cmn_topo_db_add_topology(topo_def_ptr, mod_def_ptr,ELITE_CMN_STATIC_TOPOLOGIES);
    if (ADSP_FAILED(result))
    {
#ifdef LSM_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_add_topology failed with %d", result);
#endif
      return result;
    }
  }
  return result;
}
