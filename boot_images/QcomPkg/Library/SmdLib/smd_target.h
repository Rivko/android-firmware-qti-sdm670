/**
 * @file smd_target.h
 *
 * Target-specific definitions for the SMD driver on Windows on ARM
 *
 * On other environments, SMD defines a different target header file for each
 * chip, uses that target header file on most OSes, and carefully selects the
 * appropriate file for each target at compile time. WoA is different in that
 * the same binary driver must run on multiple chips, and the target-specific
 * information (which processors SMD can talk to: which interrupts are
 * received from each processor, and which interrupts are sent to each
 * processor) are stored in the target-specific ACPI table.
 */

/*==============================================================================
     Copyright (c) 2011 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/14   bm      Remove SMD_APSS_RPM_IPC_OUT_PHY as it is dynamically
                   calculated based on HWIO/PCD.
01/31/13   bm      Initial version for UEFI
===========================================================================*/

/** \defgroup SMD Shared Memory Driver Target
 *
 * This module provides the SMD target specific definitions.
 *
 */
/*@{*/

#ifndef SMD_TARGET_H
#define	SMD_TARGET_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include "smd_internal.h"
#include "smem_type.h"

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/** \name SMD Host
 *
 * This macro defines the local processor in which SMD runs.
 *
 * @{
 */
#define SMD_THIS_HOST   ( SMEM_APPS )
#define SMD_APPS_PROC

#define SMD_NUM_CHANNEL_INFO 1     /* UEFI would have a single Apps-RPM port */

#define SMD_APPS_RPM_IPC_OUT_MASK 0x00000001
/*@}*/


/*===========================================================================
                            FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smd_target_init
===========================================================================*/
/*!
@brief
  This function initializes target specific configuration.
  
@return None

@sideeffect   Initializes target specific variables

@dependencies None
 
@ingroup smd_target
*/
/*==========================================================================*/
void smd_target_init( void );

/*===========================================================================
  FUNCTION  smd_target_init_interrupts
===========================================================================*/
/*!
@brief
  This function initializes the interprocessor interrupts for this target.
  
@param[in]  this_host   source of the interrupts

@return None

@sideeffect   Initializes the interrupt table

@dependencies None
 
@ingroup smd_target
*/
/*==========================================================================*/
void smd_target_init_interrupts( smem_host_type this_host );

/*@}*/

#endif	/* SMD_TARGET_H */
