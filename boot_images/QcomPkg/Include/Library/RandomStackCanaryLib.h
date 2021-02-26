/** @file RandomStackCanaryLib.h
   
  RandomStackCanary Functions

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/28/15   aus     Initial version
=============================================================================*/
#ifndef __RANDOMSTACKCANARYLIB_H__
#define __RANDOMSTACKCANARYLIB_H__

/**
  This function gets a random stack canary.

  @retval Random number

**/ 
UINTN GetRandomStackCanary (VOID);


#endif /* __RANDOMSTACKCANARYLIB_H__ */
