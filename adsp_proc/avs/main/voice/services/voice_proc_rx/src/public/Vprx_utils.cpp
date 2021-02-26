
/*========================================================================
Vprx_utils.cpp

This file contains utility function definition for voice rx proc service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/08/16   HB      Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "Vprx_Modules.h"
#include "Vprx_Svc.h"

ADSPResult vprx_handle_fwk_extension(vprx_t* pVprx,voice_capi_module_t* module_info)
{
   ADSPResult result = ADSP_EOK;
   for (uint32_t i=0;i < module_info->num_extensions.num_extensions; i++)
   {
      switch (module_info->fwk_extn_ptr[i].id)
      {
      case FWK_EXTN_HPCM:
      {
          //just updating the pointer.
          //hanlding will be done after the module is created.
          pVprx->modules.special_capis[HPCM] = module_info;
          break;
      }
      default:
      {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR !! un supported fwk extention : %lx", module_info->fwk_extn_ptr[i].id);
          break;
      }
      }
   }
   return result;
}

bool_t vprx_is_custom_topology(uint32_t topology_id)
{
    switch (topology_id)
    {
    case(VPM_RX_NONE):
    {
        return FALSE;
    }
    default:
    {
        return TRUE;
    }
    }
}





