/** @file DALSYSDxe.c

  DALSYS library for DXE driver

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/14/14   aa      Updating DALSYS_GetDAL_Mod to return string device info list
 08/13/14   aa      Adding DALSYS_GetDAL_Mod for XBL Core
 08/11/14   sho     Creation 

=============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/EFISmem.h>


//StringDevice Info List for DALFwk
DALProps *gpDALModStringDevDrvInfoList[DALSYSCMN_MAX_NUM_MODS_PER_PROCESS];

DALResult
DALSYS_Malloc(UINT32 dwSize, VOID **ppMem)
{
   EFI_STATUS efiStatus;
   efiStatus = gBS->AllocatePool( EfiBootServicesData, dwSize, ppMem );
   return (EFI_SUCCESS == efiStatus)? DAL_SUCCESS: DAL_ERROR;
}

DALResult
DALSYS_Free(VOID *pmem)
{
   EFI_STATUS efiStatus;
   efiStatus = gBS->FreePool( pmem );
   return (EFI_SUCCESS == efiStatus)? DAL_SUCCESS: DAL_ERROR;
}

DALResult
DALSYSCMN_RegisterStrModDriverList(DALProps *pDALModStringDevDrvInfoList)
{
   uint32 dwModIdx = 0;
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      if(NULL == gpDALModStringDevDrvInfoList[dwModIdx])
      {
         gpDALModStringDevDrvInfoList[dwModIdx] = pDALModStringDevDrvInfoList;
         return DAL_SUCCESS;
      }
   }
   DEBUG(( EFI_D_ERROR, "Insufficient space for a new module driver list\n" ));
   return DAL_ERROR;
}

DALResult
DALSYSCMN_DeRegisterStrModDriverList(DALProps *pDALModStringDevDrvInfoList)
{
   uint32 dwModIdx = 0;
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      if(pDALModStringDevDrvInfoList == gpDALModStringDevDrvInfoList[dwModIdx])
      {
         gpDALModStringDevDrvInfoList[dwModIdx] = NULL;
         return DAL_SUCCESS;
      }
   }
   DEBUG(( EFI_D_ERROR, "Cannot find the specified module driver list 0x%x\n", 
         pDALModStringDevDrvInfoList ));
   return DAL_ERROR;
}

static DALProps *
DALSYS_GetDAL_Mod(const char *pszDevName)
{
   uint32 dwModIdx = 0;
   /*--------------------------------------------------------------------------
   For all modules in this list, find the drivers infos
   --------------------------------------------------------------------------*/
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      DALProps *pDALModStringDevDrvInfoList = gpDALModStringDevDrvInfoList[dwModIdx];
      if(NULL != pDALModStringDevDrvInfoList)
      {
         uint32 dwDriverIdx = 0;
         /*------------------------------------------------------------------
           For all drivers of this mod search for requested device string
          -----------------------------------------------------------------*/
         for(dwDriverIdx=0; dwDriverIdx < pDALModStringDevDrvInfoList->dwDeviceSize;dwDriverIdx++)
         {
            // Get the dwIndex, where the str might be
            if(AsciiStrLen(pszDevName) == AsciiStrLen(pDALModStringDevDrvInfoList->pDevices[dwDriverIdx].pszName))
            {
               if(AsciiStrnCmp(pszDevName, pDALModStringDevDrvInfoList->pDevices[dwDriverIdx].pszName, AsciiStrLen(pszDevName))==0)
               {
                  return pDALModStringDevDrvInfoList;
               }
            }
         }
      }
   }
   /*--------------------------------------------------------------------------
   Did not find the requested string device Id in this process
   --------------------------------------------------------------------------*/
   DEBUG(( EFI_D_ERROR, "DAL device (0x%s) not found\n", pszDevName ));
   return NULL;
}

DALResult
_DALSYSCMN_GetStringDeviceObject(const char *pszDevName, const char *pszArg, DalDeviceHandle **phDevice)
{ 
   const StringDevice *pStringDeviceObj = NULL;
   DALREG_DriverInfo *pDriverInfo = NULL;
   
   if(DAL_SUCCESS == _DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj, DALSYS_GetDAL_Mod(pszDevName))){
      //Get the Property Dir Info Ptr
      pDriverInfo = pStringDeviceObj->pFunctionName;
      return pDriverInfo->pfnDALNewFunc(pszDevName,0,phDevice);
   } 
   return DAL_ERROR;
}

DALResult
_DAL_StringDeviceObjectLookup(const char *pszDevName, const StringDevice **ppStringDeviceObj, DALProps *pDALPtr)
{
   int dwIndex = 0;
   
   if(pDALPtr!= NULL && pDALPtr->pDevices!=NULL && pszDevName!= NULL 
     && dwIndex< pDALPtr->dwDeviceSize && ppStringDeviceObj!=NULL)
   {
      // Finish the search with right device index. 
      for(dwIndex=0; dwIndex < pDALPtr->dwDeviceSize; dwIndex++)
      {
         // Get the dwIndex, where the str might be
         if(AsciiStrLen(pszDevName) == AsciiStrLen(pDALPtr->pDevices[dwIndex].pszName))
         {
            if(AsciiStrnCmp(pszDevName, pDALPtr->pDevices[dwIndex].pszName, AsciiStrLen(pszDevName))==0)
            {
               *ppStringDeviceObj =  &(pDALPtr->pDevices[dwIndex]);
               return DAL_SUCCESS;
            }
         }
      }   
   }
   return DAL_ERROR_NOT_FOUND;
}

// Allocate global context from shared memory driver
EFI_STATUS
_AllocGlbCtxtMem(DALSYSMemAddr *p_ctxt_addr, UINT32 size)
{
   EFI_SMEM_PROTOCOL * smem_protocol;
   EFI_STATUS status;

   status = gBS->LocateProtocol(&gEfiSMEMProtocolGuid, NULL,
      (void**)&smem_protocol);
   if(status != EFI_SUCCESS)
   {
      DEBUG ((EFI_D_ERROR, 
         "Unable to locate smem protocol; status=0x%x\n",
         status));
      return status;
   }

   status = smem_protocol->SmemAlloc(SMEM_DAL_AREA, size,
         (void **)p_ctxt_addr);
   return status;
}
