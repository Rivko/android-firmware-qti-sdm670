/**
  @file bamosalprop.c
  @brief
  This file contains implementation of interfaces for BAM device
  config. 

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/16   SS      Created

===============================================================================
                   Copyright (c) 2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "com_dtypes.h"
#include "bamosal.h"
#include "bam.h"
#include "bamtgtcfg.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"

DALSYS_PROPERTY_HANDLE_DECLARE(hPropBAM);

extern bam_target_config_type* bam_tgt_config_ptr;


/* ============================================================================
**  Function : bam_osal_init
** ============================================================================
*/
/**
  Initializes BAM device config .

  Initializes OSAL Interfaces. Performs any intiialization pertaining to the
  BAM device config.

  @param  None

  @return
  None.

*/
void bam_osal_init(void)
{
    DALSYSPropertyVar bamprops;
    DALResult status = DAL_SUCCESS;
   
    status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/bam", hPropBAM);
    if(status != DAL_SUCCESS) 
        return;
    if((status = DALSYS_GetPropertyValue(hPropBAM, "bam_tgt_config", 0, &bamprops)) != DAL_SUCCESS)
        return;

    bam_tgt_config_ptr = (bam_target_config_type*)bamprops.Val.pStruct;
}

/* ============================================================================
**  Function : bam_osal_deinit
** ============================================================================
*/
/**
  DeInitializes OSAL Interfaces.

  Deinitializes OSAL Interfaces. Performs any deinitialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void bam_osal_deinit(void)
{
   
}
