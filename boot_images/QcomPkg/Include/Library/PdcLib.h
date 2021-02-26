#ifndef __PDC_LIB_H__
#define __PDC_LIB_H__
/*============================================================================
  @file PdcLib.h

  MSM Pdc library API.

  This file contains data structures and functions used to configure, control,
  and query limits sw.

               Copyright (c) 2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include <Uefi.h>

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   @brief Initializes Pdc library.

   Initialize the Pdc library for core.

   @return EFI_STATUS
 */
EFI_STATUS PdcTargetInit(void);

#endif /* #ifndef __PDC_LIB_H__ */

