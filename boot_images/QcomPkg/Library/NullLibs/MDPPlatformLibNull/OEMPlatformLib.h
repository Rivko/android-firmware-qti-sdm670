#ifndef __OEMPLATFORMLIB_H__
#define __OEMPLATFORMLIB_H__
/*=============================================================================
 
  File: OEMPlatformLib.h
 
 OEM platforms specific

     Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "MDPTypes.h"
#include "MDPPlatformLib.h"


/*===========================================================================
                        Defines and Structs
===========================================================================*/

/*===========================================================================

                        Public Functions

===========================================================================*/

/* MDP platform specific configuration function 
 */
MDP_Status OEMPlatformConfigure(MDP_Display_IDType eDisplayId, MDPPlatformConfigType eConfig, MDPPlatformParams *pPlatformParams);


#endif // __OEMPLATFORMLIB_H__


