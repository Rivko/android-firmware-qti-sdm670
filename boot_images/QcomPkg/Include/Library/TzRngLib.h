/** @file TzRngLib.h
   
  TZ RNG Functions

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/11/15   sm      Initial version
=============================================================================*/
#ifndef __TZRNGLIB_H__
#define __TZRNGLIB_H__

/**
  This function makes a call into TZ to retrive random data.

  @retval Random Data

**/ 
UINTN GetRNGVal(VOID);


#endif /* __TZRNGLIB_H__ */
