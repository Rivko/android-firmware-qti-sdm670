/** @file
  Header file for BDS Platform specific code
   
  Copyright (c) 2011, 2016, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  Portions copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.

  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/30/16   bh      Add guided FV support
 07/07/11   niting  Initial revision.

=============================================================================*/
#ifndef _BDS_PLATFORM_H_
#define _BDS_PLATFORM_H_

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/PlatformBdsLib.h>
#include <Library/QcomLib.h>
#include <Library/OfflineCrashDump.h>
#include <Protocol/EFIWriteProtect.h>
#include <Library/DevicePathLib.h>
#include <Protocol/FirmwareVolume2.h>

VOID ConfirmShutdownOnFailure (VOID);

#endif // _BDS_PLATFORM_H