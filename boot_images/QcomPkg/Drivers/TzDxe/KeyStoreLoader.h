/** @file  
               KeyStoreLoader.h

GENERAL DESCRIPTION
   Declare interface for loading SSD key store.

	
  Copyright (c) 2012 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/

/*===========================================================================
                             EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 11/08/12   shl      Initial version

===========================================================================*/

#ifndef __KEY_STORE_LOADER_H__
#define __KEY_STORE_LOADER_H__

/**
*  LoadKeyStore ()
*
*  @brief 
*  LoadKeyStore function reads key store from RPMB and passes it to TZ
*  
*  @param[in]  VOID
*
*  @return
*  EFI_STATUS
*/

EFI_STATUS LoadKeyStore (void);

#endif  /* __KEY_STORE_LOADER_H__  */

