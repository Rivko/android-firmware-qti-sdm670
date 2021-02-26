#if defined (DIAG_IMAGE_QDSP6_PROC)
/*========================================================================

  Copyright (c) 2011-2013,2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  02/10/13    sg      Fixed Klock work warnings
  03/01/13    sg      Migrated to new MMPM apis
  6/13/11     sg      Created file.

  ========================================================================== */
#include "comdef.h"
#include <stdlib.h> 
#include "mmpm.h"
#include "diagMmpm.h"
#include "msg.h"

/*===========================================================================     
     
FUNCTION DIAG_MMPM_REGISTER   
     
DESCRIPTION     
  Register with MMPM to get a handle for Diag MIPs and BW.
	 
  This function must be called prior to any other MMPM function. The handle is
  used in all other functions.
     
RETURN VALUE     
     On success, returns ADSP_EOK
=============================================================================*/
uint32 diag_mmpm_register(void)
{
   MmpmRegParamType  mmpmRegParam;
   char              client_name[] = "Diag_Stress_test";
   uint32            client_id = 0;

   memset(&mmpmRegParam, 0,  sizeof(MmpmRegParamType));
   mmpmRegParam.rev = MMPM_REVISION;
   mmpmRegParam.coreId = MMPM_CORE_ID_LPASS_ADSP;
   mmpmRegParam.instanceId = MMPM_CORE_INSTANCE_0;
   mmpmRegParam.MMPM_Callback = NULL;
   mmpmRegParam.pClientName = client_name;
   mmpmRegParam.pwrCtrlFlag = PWR_CTRL_NONE;
   mmpmRegParam.callBackFlag = CALLBACK_NONE;
   client_id = MMPM_Register_Ext(&mmpmRegParam);

   if (!client_id)
   {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MMPM_Register for DIAG Stress Test  failed");
   }

   return client_id;
}

/*===========================================================================     
     
FUNCTION DIAG_MMPM_DEREGISTER   
     
DESCRIPTION     
  Deregister with MMPM
	
  All power resources requested by this handle are removed and their MIPS and
  bus bandwith resources are released. The MMPM deallocates the client's context
  and the handle becomes invalid for any other MMPM function. 
     
RETURN VALUE     
    Returns TRUE on success and FALSE on failure  
=============================================================================*/

int diag_mmpm_deregister(uint32 client_id)
{
   MMPM_STATUS mmpmResult =  MMPM_Deregister_Ext(client_id);

   if (MMPM_STATUS_SUCCESS != mmpmResult)
   {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MMPM_DeRegister for DIAG Stress Test  failed");
   }

   return mmpmResult;
}
/*===========================================================================     
     
FUNCTION DIAG_MMPM_RELEASE 
     
DESCRIPTION     
  MMPM Release for Stress Test.
 
 Releases CPU MIPS and bus bandwidth resources associated with a handle. 

 
     
RETURN VALUE     
    Returns TRUE on success and FALSE on failure  
=============================================================================*/
int diag_mmpm_release(uint32 client_id)
{
   MMPM_STATUS       sts = MMPM_STATUS_FAILED;
   MmpmRscParamType  mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != client_id)
   {

      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &sts;

      sts = MMPM_Release_Ext(client_id, &mmpmRscExtParam);
      if (MMPM_STATUS_SUCCESS != sts)
      {
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, " MMPM Power(diag) release FAILED for clientId %ld \n", client_id);
      }

   } 
   else
   {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Diag No valid MMPM client id \n");
   }

   return sts;
}
/*===========================================================================     
     
FUNCTION DIAG_MMPM_CONFIG
     
DESCRIPTION     
  MMPM config for Stress Test.Takes values DIAG_VOTE_FOR_POWER_COLLAPSE or 
  WDT_VOTE_AGAINST_POWER_COLLAPSE
  Allocates CPU MIPS and bus bandwidth resources for a power request 
  from a registered client. An MMPM client uses this function to request 
  demanded CPU MIPS or bus bandwidth with respect to its registered client
  type to the MMPM.
 
     
RETURN VALUE     
    Returns TRUE on success and FALSE on failure  
=============================================================================*/
int diag_mmpm_config(uint32 client_id)
{
   MMPM_STATUS        status = MMPM_STATUS_FAILED;
   MmpmRscParamType   mmpmRscParam;
   MmpmRscExtParamType mmpmRscExtParam;

   if (0 != client_id)
   {
      memset(&mmpmRscParam, 0,  sizeof(MmpmRscParamType));
      memset(&mmpmRscExtParam, 0,  sizeof(MmpmRscParamType));
      mmpmRscParam.rscId = MMPM_RSC_ID_POWER;
      mmpmRscExtParam.apiType = MMPM_API_TYPE_SYNC;
      mmpmRscExtParam.numOfReq = 1;
      mmpmRscExtParam.pReqArray = &mmpmRscParam;
      mmpmRscExtParam.pStsArray = &status;
      status = MMPM_Request_Ext(client_id, &mmpmRscExtParam);
      if (MMPM_STATUS_SUCCESS != status)
      {
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "MMPM Power(Diag) request FAILED for clientId %ld \n", client_id);
      }

   }
   else
   {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Diag No valid MMPM client ID \n");
   }

   return status;
}

#endif
