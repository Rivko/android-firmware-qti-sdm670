/*==========================================================================
ELite Source File

This file implements the topology database for the AVS.

Copyright (c) 2014 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
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
#include "EliteTopology_db_if.h"
#include "EliteTopology_db.h"

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

ADSPResult Elite_InitializeDataBases(void)
{
   ADSPResult result;
   result = topo_db_init();
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize topo db with %d", result);
      return result;
   }

   result = AudPP_AddStaticTopologiesAndModules();
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add static topologies and modules with %d", result);
      topo_db_deinit();
      return result;
   }

   result = voice_add_static_topologies_and_modules();
   if (ADSP_FAILED(result))
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add  voice static topologies and modules with %d", result);
     topo_db_deinit();
     return result;
   }

   return result;
}

void Elite_DeinitializeDataBases()
{
      topo_db_deinit();
}

ADSPResult Elite_ResetTopologyDatabase(void)
{
   topo_db_deinit();

   ADSPResult result;
   result = topo_db_init();
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize topo db with %d", result);
      return result;
   }

   result = AudPP_AddStaticTopologies();
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add static topologies with %d", result);
      topo_db_deinit();
      return result;
   }

   return result;
}


