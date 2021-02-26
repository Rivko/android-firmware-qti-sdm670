/** @file MpPowerLib.h
   
  The Multicore power  common definitions

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/Library/MpPowerLib.h#1 $ 
  $DateTime: 2017/09/18 09:26:13 $ 
  $Author: pwbldsvc $ 

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/24/15   rli     Initial revision
=============================================================================*/
#ifndef __MPPOWERLIB_H_
#define __MPPOWERLIB_H_


/*=========================================================================
      Include Files
==========================================================================*/

/*===========================================================================

                        Defines and Structs

===========================================================================*/

/*===========================================================================

                        Public Functions

===========================================================================*/
/**
  @brief MpPowerInit - chipset-specific initialization
  @return - EFI_SUCCESS on success, otherwise error status
*/
EFI_STATUS MpPowerInit (VOID);

/**
  @brief MpPower - chipset-specific multicore enablement
  @param[in]  CoreNum - number of cores
  @return - EFI_SUCCESS on success, otherwise error status
*/
EFI_STATUS MpPower (UINTN  CoreNum);



#endif // __MPPOWERLIB_H_
