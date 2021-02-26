/** @file DALDevice.c

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/15/14   aa      Moved _DALSYSCMN_GetStringDeviceObject to Loader and Dxe 
                    specific file
 08/13/14   aa      Adding DALSYSCMN_RegisterModDriverList as common API
 08/11/14   sho     Creation 

=============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include "DALReg.h"
#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

/*-------------------------------------------------------------------
 Constant Definitions 
 ------------------------------------------------------------------*/

/*-------------------------------------------------------------------
 Structure Definitions 
 ------------------------------------------------------------------*/
// Driver module list
DALREG_DriverInfoList * gDALProcessDriverModList[DALSYSCMN_MAX_NUM_MODS_PER_PROCESS];

/*-------------------------------------------------------------------
 Function Definitions 
 ------------------------------------------------------------------*/
static DALResult
DALSYSCMN_GetDeviceObject(DALDEVICEID DevId, const char *pszArg, 
                          DalDeviceHandle **phDevice)
{
   uint32 dwModIdx = 0;
   /*--------------------------------------------------------------------------
   For all modules in this list, find the drivers infos
   --------------------------------------------------------------------------*/
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      DALREG_DriverInfoList *pModDriverInfoList = gDALProcessDriverModList[dwModIdx];

      if(NULL != pModDriverInfoList)
      {
         uint32 dwDriverIdx = 0;
         /*------------------------------------------------------------------
           For all drivers of this mod search for requested device id
          -----------------------------------------------------------------*/
         for(dwDriverIdx =0;dwDriverIdx<pModDriverInfoList->dwLen;dwDriverIdx++)
         {
            /*--------------------------------------------------------------
            For each devices supported by this driver
            --------------------------------------------------------------*/
            uint32 dwDeviceIdx = 0;
            DALREG_DriverInfo *pDriverInfo = pModDriverInfoList->pDriverInfo[dwDriverIdx];
            for(dwDeviceIdx=0;dwDeviceIdx<pDriverInfo->dwNumDevices;dwDeviceIdx++)
            {
               if(DevId == pDriverInfo->pDeviceId[dwDeviceIdx])
               {
                  /*------------------------------------------------------
                  search complete
                  ------------------------------------------------------*/
                  return 
                  pDriverInfo->pfnDALNewFunc(pszArg,DevId,phDevice);
               }
            }
         }
      }
   }
   /*--------------------------------------------------------------------------
   Did not find the requested device Id in this process
   --------------------------------------------------------------------------*/
   DEBUG(( EFI_D_ERROR, "DAL device (0x%x) not found\n", DevId ));
   return DAL_ERROR_DEVICE_NOT_FOUND;
}

DALResult
DALSYSCMN_RegisterModDriverList(DALREG_DriverInfoList *pModDriverList)
{
   uint32 dwModIdx = 0;
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      if(NULL == gDALProcessDriverModList[dwModIdx])
      {
         gDALProcessDriverModList[dwModIdx] = pModDriverList;
         return DAL_SUCCESS;
      }
   }
   DEBUG(( EFI_D_ERROR, "Insufficient space for a new module driver list\n" ));
   return DAL_ERROR;
}

DALResult
DALSYSCMN_DeRegisterModDriverList(DALREG_DriverInfoList *pModDriverList)
{
   uint32 dwModIdx = 0;
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      if(pModDriverList == gDALProcessDriverModList[dwModIdx])
      {
         gDALProcessDriverModList[dwModIdx] = NULL;
         return DAL_SUCCESS;
      }
   }
   DEBUG(( EFI_D_ERROR, "Cannot find the specified module driver list 0x%x\n", pModDriverList ));
   return DAL_ERROR;
}

DALResult
DAL_DeviceAttach(DALDEVICEID DevId,DalDeviceHandle **phDalDevice)
{
   DALResult ret;

   *phDalDevice = NULL; // in case of error

   ret = DALSYSCMN_GetDeviceObject(DevId, NULL, phDalDevice);
   
   if(DAL_SUCCESS == ret)
   {
      // grab reference count after attach but within lock
      uint32 refcnt = DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice);   

      // if first attach was me and this time, then call init 
      if(1 == refcnt)  
      {
         ret = DalDevice_Init(*phDalDevice);
         if(DAL_SUCCESS != ret)
         {
            DAL_DeviceDetach(*phDalDevice);
            *phDalDevice = NULL;
         }
      }
   }

   return ret;
}

DALResult
DAL_DeviceAttachEx(const char *pszArg,
                   DALDEVICEID DevId,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDalDevice)
{
   DalDeviceInfo DeviceInfo;
   DALResult ret = DAL_DeviceAttach(DevId,phDalDevice);

   /* if DAL_Device Attach failed, return immediately */
   if(DAL_SUCCESS != ret)
   {
      *phDalDevice = NULL;
      return ret;
   }

   if (DAL_SUCCESS != DalDevice_Info(*phDalDevice,&DeviceInfo,
                                     sizeof(DeviceInfo)))
   {   
      DEBUG(( EFI_D_ERROR, "Cannot retrieve info for device 0x%x\n", DevId ));
      return DAL_ERROR;
   }


   //do version check
   if(!DALINTERFACE_COMPATIBLE(ClientVersion, DeviceInfo.Version))
   {   
      //failed version check, interface change
      (void)DAL_DeviceDetach(*phDalDevice);
      *phDalDevice = NULL;
      DEBUG(( EFI_D_ERROR, "Client version 0x%x and device version 0x%x differ; device Id 0x%x\n", 
             ClientVersion, DeviceInfo.Version, DevId ));
      return DAL_ERROR_DDI_VERSION;
   }

   return DAL_SUCCESS;
}

DALResult
DAL_StringDeviceAttachLocal(const char *pszArg, const char *pszDevName,
                      DalDeviceHandle **phDalDevice)
{
   *phDalDevice = NULL;

   if(DAL_SUCCESS == _DALSYSCMN_GetStringDeviceObject(pszDevName,pszArg,phDalDevice))
   {
       //if a remote handle the remote side does the Init
       if(!DALISREMOTEHANDLE(*phDalDevice))
       {
          if(1 == DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice))
          {
              DALResult ret = DalDevice_Init(*phDalDevice);
              if(DAL_SUCCESS != ret)
              {
                 DAL_DeviceDetach(*phDalDevice);
                 *phDalDevice = NULL;
                 return ret;
              }
          }
       }
       return DAL_SUCCESS;
   }
   return DAL_ERROR_DEVICE_NOT_FOUND;
}

DALResult
DAL_StringDeviceAttach(const char *pszDevName,
                  DalDeviceHandle **phDalDevice)
{
  return DAL_StringDeviceAttachLocal(NULL, pszDevName, phDalDevice); 
} 

DALResult
DAL_StringDeviceAttachEx(const char *pszArg,
      const char *pszDevName,
      DALInterfaceVersion ClientVersion,
      DalDeviceHandle **phDALDevice)
{
   DalDeviceInfo DeviceInfo;
   DALResult ret = DAL_StringDeviceAttachLocal(pszArg,pszDevName,phDALDevice);
   if(DAL_SUCCESS != ret)
   {
      // no support for remote
      *phDALDevice = NULL;
      return ret;
   }

   if (DAL_SUCCESS != DalDevice_Info(*phDALDevice,&DeviceInfo,
                                     sizeof(DeviceInfo)))
      return DAL_ERROR;

   //do version check
   if (DALINTERFACE_GET_MAJOR_VERSION(ClientVersion) !=
       DALINTERFACE_GET_MAJOR_VERSION(DeviceInfo.Version))
   {
      //failed Major version check, interface change
      (void)DAL_DeviceDetach(*phDALDevice);
      *phDALDevice = NULL;
      return DAL_ERROR;
   }
   //has to be success...
   return DAL_SUCCESS;
}
             
DALResult
DAL_DeviceDetach(DalDeviceHandle * hDalDevice)
{
   if(hDalDevice)
   {
      if(DALISREMOTEHANDLE(hDalDevice))
         DalDevice_Detach(hDalDevice);
      else
      {
         if(1 == DAL_GET_DEVICE_ATTACH_COUNT(hDalDevice))
         {
             (void)DalDevice_DeInit(hDalDevice);
         }
         //do the detach
         DalDevice_Detach(hDalDevice);
      }
      return DAL_SUCCESS;
   }
   return DAL_ERROR;   
}
