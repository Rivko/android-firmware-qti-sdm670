/*=============================================================================

FILE:         qdss_imem_nondal.c

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_imem.h"
#include "Chipinfo.h"
#include "qdss_imem_config.h"


uint32 qdss_imem_base(void)
{
   ChipInfoFamilyType  chip_family;
   ChipInfoVersionType chip_version;
   uint32 imem_base = QDSS_INVALID_IMEM_ADDR;

   chip_family=Chipinfo_GetFamily ();
   chip_version=Chipinfo_GetVersion ();

   switch (chip_family) {
   case CHIPINFO_FAMILY_MSM8974:
      if (chip_version >= CHIPINFO_VERSION(2,0)) {
         imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT;
      }
      else {
         //msg ram workaround for v1
         imem_base=QDSS_IMEM_BASE_BADGER_MSGRAM;
      }
      break;

   case CHIPINFO_FAMILY_MDM9x25:
      if (chip_version >= CHIPINFO_VERSION(2,0)) {
         imem_base=QDSS_IMEM_BASE_9x25_V2;

      }
      else {
         //msg ram workaround for v1
         imem_base=QDSS_IMEM_BASE_BADGER_MSGRAM;
      }
      break;

   case CHIPINFO_FAMILY_MSM8x10:
   case CHIPINFO_FAMILY_MSM8x26:
      imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT;
      break;

   case CHIPINFO_FAMILY_MPQ8092:
   case CHIPINFO_FAMILY_APQ8x94:
   case CHIPINFO_FAMILY_MSM8x32:
   case CHIPINFO_FAMILY_MDM9x35:
      imem_base=QDSS_IMEM_BASE_BADGER_DEFAULT; //These not yet in the IMEM doc
      break;
   default:
      imem_base=QDSS_INVALID_IMEM_ADDR;
   }

   if (QDSS_INVALID_IMEM_ADDR != imem_base) {
      imem_base += QDSS_IMEM_QDSS_OFFSET;
   }
   
   return imem_base;
}

