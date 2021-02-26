/** @file MpParkLib.h
  Common definitions for MpParkLib.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/20/12   jz      Added MpParkRpmVddMinCountAddr
 01/31/12   niting  Updated timer sync code
 01/25/12   yg      Added PowerupCPUCore API 
 11/08/11   plc     Adding support for 4 cores in MPPark 
 06/15/11   cwg     Initial version.

=============================================================================*/
#ifndef __MPPARKLIB_H_
#define __MPPARKLIB_H_

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                        Defines and Structs

===========================================================================*/
extern UINT32 MpParkGPT0CntAddr;
extern UINT32 MpParkGPT0MatchAddr;
extern UINT32 MpParkGPT0EnableAddr;
extern UINT32 MpParkMPMCountAddr;
extern UINT32 MpParkHalfDayAddr;
extern UINT32 MpParkRpmVddMinCountAddr;

extern UINT32 MpParkTimerSyncEnabled;

/*===========================================================================

                        Public Functions

===========================================================================*/
void CPU_Start(UINTN cpu);

EFI_STATUS PowerupCPUCore (UINTN  CoreNum);

/*===========================================================================

                        Private Functions

===========================================================================*/

#endif // __MPPARKLIB_H_

