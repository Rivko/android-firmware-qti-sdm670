/** @file QcomLib.h
   
  Contains target specific BDS functionality

  Copyright (c) 2016, Qualcomm Technologies, Inc. All rights reserved. 
 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/07/16   ml      Renamed splash screen function
 03/04/16   bh      Allow fat mounting by partition type also
 02/08/16   vk      Remove GetOsType()
 
=============================================================================*/
#ifndef __BDS_TARGET_LIB_H__
#define __BDS_TARGET_LIB_H__

EFI_STATUS OverrideAblLoadAddr(UINTN *Addr);

#endif /* __BDS_TARGET_LIB_H__ */
