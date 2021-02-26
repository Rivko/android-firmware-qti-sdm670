#ifndef DALSYSINT_H
#define DALSYSINT_H
/*==============================================================================
@file DALSysInt.h


Device Config Internal Functionality


        Copyright © 2007 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

when       who     description
--------   ---     ----------------------------------------------------------
11/24/15   sg      Add support for multi device config

==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "PlatformInfo.h"
#include "ChipInfo.h"
/*------------------------------------------------------------------------------
* Defines
*-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
* Type declaration
*-----------------------------------------------------------------------------*/
typedef struct {
   DalPlatformInfoPlatformType platform_id;
   DALProps *plinker_platform_seg_sym;
}DEVCFG_PLATFORM_SEG_INFO;

typedef struct {
   uint32 tcsr_soc_info ;
   DALProps *plinker_soc_seg_sym;
   DEVCFG_PLATFORM_SEG_INFO *platform_info;
   uint32 num_platform_type;
}DEVCFG_TARGET_INFO;

/* 
Bits	Field Name	Description for TCSR_SOC_HW_VERSION register
31:28	FAMILY_NUMBER	
27:16	DEVICE_NUMBER	
15:8	MAJOR_VERSION	
7:0	MINOR_VERSION	
*/

typedef struct {
   uint16 hw_version ;
   uint16 soc_number ;
}DEVCFG_PLATFORM_INFO;

void DevCfg_Init_Platform(void);
void DevCfg_Init(void);
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*------------------------------------------------------------------------------
* Function declaration and documentation
*-----------------------------------------------------------------------------*/

/**
* @brief Identify the device config of interset for SOC/PLATFORM in execution 
*          -- internal API used by  DALPROPS.c 

* @param: pTargetInfoTableIter reference to meta data for 
*         all possible configuration
*
* @return void
*/
void
DALSYS_DoPropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter);

/**
* @brief Identify the chipset -- internal API used by  DALPROPS.c 

* @param: void
*
* @return void
*/
void
DALSYS_ReadSocHwInfo(void);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* DALSYSINT_H */
