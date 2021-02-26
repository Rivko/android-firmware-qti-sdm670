/*=============================================================================

FILE:         qdss_imem_dal.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_imem.h"
#include "DDIChipInfo.h"
#include  "DALSys.h"
#include "qdss_imem_config.h"
#include "qdss_utils.h"


typedef struct {
   DALSYSMemHandle hMem;
   DALSYSMemInfo   memInfo;
   uint32 imem_base;
}qdss_imem_t;

static qdss_imem_t qdss_imem;


static DALResult qdss_imem_memmap(uint32 physAddr,
                                  uint32 size,
                                  DALSYSMemHandle *phMem,
                                  DALSYSMemInfo *pMemInfo)
{
   int nErr = DAL_ERROR;
   TRY(nErr,DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physAddr,
                                  size, phMem, NULL )) ;

   DALSYS_MemInfo(*phMem, pMemInfo);

   if (pMemInfo->PhysicalAddr != physAddr) {
      //could happen if our address is not 4K aligned
      uint32 offset= physAddr & 0xFFF; //our offset from the  4K page

      pMemInfo->VirtualAddr    += offset;
      pMemInfo->PhysicalAddr   += offset;
      pMemInfo->dwLen          -= offset;
   }
   nErr = DAL_SUCCESS;

   CATCH(nErr) {}
   return nErr;
}


#define QDSS_IMEM_PAGE_SIZE 0x1000

uint32 qdss_imem_base(void)
{
   DALDEVICEHANDLE  hChipInfo = 0;
   DalChipInfoFamilyType  chip_family;
   DalChipInfoVersionType chip_version;
   int nErr;
   uint32 imem_base = qdss_imem.imem_base;
   
   if (QDSS_INVALID_IMEM_ADDR != imem_base) {
      return imem_base; //Already have it just return
   }

   TRY(nErr,DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo));
   TRY(nErr,DalDevice_Open(hChipInfo, DAL_OPEN_SHARED));
   TRY(nErr,DalChipInfo_GetChipFamily(hChipInfo, &chip_family));
   TRY(nErr,DalChipInfo_GetChipVersion(hChipInfo, &chip_version));


   switch (chip_family) {
   case DALCHIPINFO_FAMILY_MSM8974:
      if (chip_version >= DALCHIPINFO_VERSION(2,0)) {
         imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT;
      }
      else {
         //msg ram workaround for v1
         imem_base=QDSS_IMEM_BASE_BADGER_MSGRAM;
      }
      break;

   case DALCHIPINFO_FAMILY_MDM9x25:
      if (chip_version >= DALCHIPINFO_VERSION(2,0)) {
         imem_base=QDSS_IMEM_BASE_9x25_V2;
      }
      else {
         //msg ram workaround for 9x25 v1
         imem_base=QDSS_IMEM_BASE_9x25_V1;
      }
      break;

   case DALCHIPINFO_FAMILY_MDM9x35:
      imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT; 
      break;

      //For now, we don't care about the IMEM cookie on LA targets
   case DALCHIPINFO_FAMILY_MSM8x10:
   case DALCHIPINFO_FAMILY_MSM8x26:
   case DALCHIPINFO_FAMILY_APQ8064:
   case DALCHIPINFO_FAMILY_MSM8x62:
   case DALCHIPINFO_FAMILY_MPQ8092:
   case DALCHIPINFO_FAMILY_MSM8994:
   default:
      imem_base=QDSS_INVALID_IMEM_ADDR;
      THROW(nErr,DAL_ERROR);
   }


   TRY(nErr,qdss_imem_memmap(imem_base,
                             QDSS_IMEM_PAGE_SIZE,
                             &qdss_imem.hMem,
                             &qdss_imem.memInfo));
                             
   imem_base = qdss_imem.memInfo.VirtualAddr;
   imem_base += QDSS_IMEM_QDSS_OFFSET;


   CATCH(nErr) {
      imem_base = QDSS_INVALID_IMEM_ADDR;
   }
   
   if (0!=hChipInfo) {
      DalDevice_Close(hChipInfo);
      DAL_DeviceDetach(hChipInfo);
   }

   qdss_imem.imem_base = imem_base; 
   return imem_base;
}

