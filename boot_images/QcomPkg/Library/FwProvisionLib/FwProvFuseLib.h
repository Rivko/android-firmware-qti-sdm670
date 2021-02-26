/** @file FwProvFuseEntry.h
   
  Header for Fuse related config file  

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights 
  Reserved. Qualcomm Technologies Proprietary and Confidential. 

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/03/22     mic           Removed Fuse blowing support
 2013/09/23     mic/manoj     Initial version
=============================================================================*/

#ifndef _FW_PROV_FUSEENTRY_H_
#define _FW_PROV_FUSEENTRY_H_

#include <Uefi.h>

#define CHIP_INFO_MAX_SIZE 256


/**
 * This function is the entry function for QRD fuse. 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
QrdProvMain (VOID);

/**
 * This function performs QRD provisioning Stage 1 function. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
QrdProvStage1 (VOID);

#endif
