/*========================================================================
   This file contains Elite thread priorities device configuration functions

   Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
   QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
   by the U.S. Government, Diversion contrary to U.S. law prohibited.


  $Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/elite_thrd_prio.c#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "Elite.h"
#include "elite_thrd_prio.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DALPropDef.h"
#include "DDIChipInfo.h"

elite_thrd_prio_prop_struct_t *thrd_prio_prop_ptr = NULL;

ADSPResult elite_thrd_prio_init(void)
{
   DALSYSPropertyVar prop_value;
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);
   const char *dev_id="THRD_PRIO";

   /* Get handle to the device */
   if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, handle))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DALSYS_GetDALPropertyHandleStr failed for elite thread prio!");
      return ADSP_EFAILED;
   }

   if(DAL_SUCCESS != DALSYS_GetPropertyValue(handle,"ThrdPrioPropStructPtr", 0, &prop_value))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DALSYS_GetPropertyValue failed for elite thread prio!");
      return ADSP_EFAILED;
   }

   /* Back up the pointer to device config data base for thread priorities */
   thrd_prio_prop_ptr = (elite_thrd_prio_prop_struct_t*)(prop_value.Val.pStruct);

   return ADSP_EOK;
}

uint32_t elite_get_thrd_prio(elite_thrd_prio_id_t thrd_prio_id)
{
  uint32_t thrd_prio = ELITETHREAD_UNSUPPORTED_PRIO;

  bool_t is_found = FALSE;

  elite_thrd_prio_struct_t *thrd_prio_ptr = thrd_prio_prop_ptr->thrd_prio_ptr;

  if(ELITETHREAD_THRD_PRIO_ID_MAX <= thrd_prio_id)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported thread prio ID %ld", (uint32_t)thrd_prio_id);
    return thrd_prio;
  }

  for(uint32_t i = 0; i < thrd_prio_prop_ptr->num_threads; i++)
  {
    if(thrd_prio_id == thrd_prio_ptr[i].id)
    {
      thrd_prio = thrd_prio_ptr[i].prio;
      is_found = TRUE;
      break;
    }
  }

  if(FALSE == is_found)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported thrd_prio_id %ld, thrd_prio %ld",
          (uint32_t)thrd_prio_id, thrd_prio);
  }
  return thrd_prio;
}




