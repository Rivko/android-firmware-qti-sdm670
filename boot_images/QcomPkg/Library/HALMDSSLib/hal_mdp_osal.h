#ifndef _HAL_MDP_OSAL_H
#define _HAL_MDP_OSAL_H

/*=============================================================================
 
  File: MDPPlatformLib.c
 
  MDP platform specific functions
  
 
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "MDPSystem.h"

/* -----------------------------------------------------------------------
** Preprocessor Defintions and Constants
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Enumerations and Data structures
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
 * OSAL Helper Macros
 * ----------------------------------------------------------------------- */
#define MDSS_OSAL_MEMZERO(_ptr_, _size_) \
    MDP_OSAL_MEMZERO(_ptr_, _size_)
  
#define MDSS_OSAL_MEMCPY(_dst_, _src_, _size_) \
    MDP_OSAL_MEMCPY(_dst_, _src_, _size_)
  
#define MDSS_OSAL_DELAYMS(_DelayMs_) \
    MDP_OSAL_DELAYMS(_DelayMs_)
  
#define MDSS_OSAL_DELAYUS(_DelayUs_) \
    MDP_OSAL_DELAYUS(_DelayUs_)
  
#define MDSS_OSAL_MEMSET(_ptr_, _val_, _size_) \
    MDP_OSAL_MEMSET(_ptr_, _size_, _val_)


#endif // _HAL_MDP_OSAL_H
