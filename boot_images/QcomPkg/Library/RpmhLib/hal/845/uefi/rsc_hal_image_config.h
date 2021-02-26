/*
==============================================================================

FILE:         rsc_hal_image_config.h

DESCRIPTION:  This file contains description about RSC Hal. Rsc HAL is mainly responsible for 
              managing TCSs/AMCs associcated with RSC hw block on each subsystem


==============================================================================

                             Edit History


==============================================================================
   Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/
#include "rsc_hal.h"


#define PLATFORM                           HWIO_APSS_RSC
//Enable following pre-processor for Drv1
//#define DRV_1_VALID

#define RSC_HAL_ARRAY_SIZE(array)          (sizeof(array)/sizeof(array[0]))
#define RSC_HAL_ARRAY(array)               RSC_HAL_ARRAY_SIZE(array), &array[0]
/**
* Config Entry for each DRV. This config information would be stored in a 
* chipset-specific/image-specific file and will be read by RSC HAL at init. E.g. HLOS-DRV will have 
* two entries: one for HLOS and another for Display
* DrvName:            Refers to unique identifier, RSCSW_DRV_MAPPING  defined above, for each DRV
* BaseAddress+ Drvid: BaseAddress for that specific DRV--e.g. Hlos baseaddress could be: TZ base address +  2,DRV Id,*1000
* 
*/

typedef struct DrvBaseAddressMappingEntry{
    RSCSW_DRV_MAPPING   RscSwDrvMapId;
    uint32              BaseAddress;
    uint8               DrvId;
} DrvBaseAddressMappingEntry;

/**
* Config table to define all applicable DRVs
* Nrows:                      Total number of rows of applicable DRVs 
* DrvBaseAddressMappingEntry: Start address for first DRV in config table
**/

typedef struct DrvBaseAddressMappingTable {
    uint32                               Nrows;
    DrvBaseAddressMappingEntry          *Prows;
} DrvBaseAddressMappingTable;

DrvBaseAddressMappingEntry DrvBaseAddressMapping_array_subsystem[] = {
        {RSC_DRV_HLOS,(APSS_RSC_RSCC_RSCC_RSC_REG_BASE), 2}
};

DrvBaseAddressMappingTable DrvBaseAddressMappingTable_subsystem =
        { RSC_HAL_ARRAY(DrvBaseAddressMapping_array_subsystem) };



