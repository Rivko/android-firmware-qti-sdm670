/** @file
   
 Declaration of MorPpiLib Interface 

 Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 10/18/11    shl     Initial version.

=============================================================================*/
#ifndef _MORPPILIB_H_
#define _MORPPILIB_H_

/**
  This is called to handle Memoery Overwrite Request/ Physical Presence Interface.

  "A general description of the scheme is that after any type of
  Host Platform Reset event (with the exception of a CPU-only 
  reset that is used by some chipsets to turn off a CPU feature
  without re-setting other Host Platform components), if signaled
  to do so by the OS, the POST BIOS must, prior to executing any
  non-BIOS code, overwrite system memory."
  
  @retval EFI_STATUS

**/
EFIAPI  EFI_STATUS ProcessMorPpi( VOID );


#endif /* _MORPPILIB_H_ */

