#ifndef ERR_RESET_DETECT_H
#define ERR_RESET_DETECT_H
/*=============================================================================

                         E r r  R e s e t  D e t e c t i o n
                                  H e a d e r  F i l e

 
GENERAL DESCRIPTION
  This file contains the code to detect HW reset of the chip and
  transition into the downloader.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007,2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/07/14   kedara  Include com_dtypes.h instead of comdef.h
07/25/07   tbg     Initial revision (pulled from boot_hw_reset.h)

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#ifndef _ARM_ASM_
#include "com_dtypes.h"
#endif /* _ARM_ASM_ */


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifndef _ARM_ASM_
typedef enum
{
  ERR_RESET_NONE = 0x0,    /* Indicates flags are cleared */
  ERR_RESET_DETECT_ENABLED,/* Indicates reset flags are set */
  ERR_RESET_SPLASH_RESET,  /* APPSBL should use reset splash screen */
  ERR_RESET_SPLASH_DLOAD,  /* APPSBL should use dload splash screen */  
} err_reset_type;

/* Data struct to match the register logs saved by PBL and SBL code */
typedef enum
{
  ERR_RESET_SYS_SP = 0,
  ERR_RESET_SYS_LR,
  ERR_RESET_IRQ_SP,
  ERR_RESET_IRQ_LR,
  ERR_RESET_FIQ_SP,
  ERR_RESET_FIQ_LR,
  ERR_RESET_ABT_SP,
  ERR_RESET_ABT_LR,
  ERR_RESET_UDF_SP,
  ERR_RESET_UDF_LR,  
  ERR_RESET_SVC_R0,
  ERR_RESET_SVC_R1,
  ERR_RESET_SVC_R2,
  ERR_RESET_SVC_R3,
  ERR_RESET_SVC_R4,
  ERR_RESET_SVC_R5,
  ERR_RESET_SVC_R6,
  ERR_RESET_SVC_R7,
  ERR_RESET_SVC_R8,
  ERR_RESET_SVC_R9,
  ERR_RESET_SVC_R10,
  ERR_RESET_SVC_R11,
  ERR_RESET_SVC_R12,
  ERR_RESET_SVC_SP,
  ERR_RESET_SVC_LR,
  ERR_RESET_SVC_PC,
  SIZEOF_ERR_RESET_REGISTERS
} err_reset_register_type;

typedef struct
{
  uint32 reg[SIZEOF_ERR_RESET_REGISTERS];
} err_reset_log_type;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION  ERR_RESET_DETECT_ENABLE

DESCRIPTION
 This function will init hw reset detect flags.

DEPENDENC

IES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void err_reset_detect_enable(void);

/*===========================================================================

FUNCTION  ERR_RESET_DETECT_DISABLE

DESCRIPTION
  This function will clear hw reset detect flags.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void err_reset_detect_disable(void);

/*===========================================================================

FUNCTION  ERR_RESET_DETECT_STATUS

DESCRIPTION
  This function will return the current status of the reset detection flags
  
DEPENDENCIES
  None.
  
RETURN VALUE
  err_reset_type

SIDE EFFECTS
  None

===========================================================================*/

err_reset_type err_reset_detect_status(void);

/*===========================================================================

FUNCTION  ERR_RESET_DETECT_SET

DESCRIPTION
  This function set reset detect status to the value passed in
  as an err_reset_type
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void err_reset_detect_set(err_reset_type state);

/*===========================================================================

FUNCTION  ERR_RESET_DETECT_INIT

DESCRIPTION
  This function initializes the internal pionters used by the reset detection
  mechanism. 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean err_reset_detect_init(void);

/*===========================================================================

FUNCTION  ERR_RESET_DETECT_BOOT_ALLOC

DESCRIPTION
  This function is used to alloc the reset detection shared segment.
  Only needed when FEATURE_SHARED_SEGMENT is being used.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean err_reset_detect_boot_alloc(void);

/*===========================================================================

FUNCTION  ERR_RESET_LOG_INIT

DESCRIPTION
  This function is used to initialize the reset log and place it into a
  temporay, local array.  Should be caled before shared memory is initialized
  by the bootloader. Should be passed in a pointer to the saved reset registers
  and the size of the register array (these will be saved into the log)
  
DEPENDENCIES
  None.
  
RETURN VALUE
  1 indicates success
  other values may be used to denote specific failure conditions

SIDE EFFECTS
  None

===========================================================================*/
int err_reset_log_init(char *register_log, int sizeof_register_log);

/*===========================================================================

FUNCTION  ERR_RESET_LOG_FINALIZE

DESCRIPTION
  This function is used to finalize the reset log and place it into
  shared memory.  Should be caled after shared memory is initialized
  by the bootloader.
  
DEPENDENCIES
  err_reset_log_init must be called before calling this function. If not,
  the log will not be created.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void err_reset_log_finalize(void);


#endif /* _ARM_ASM_ */

#endif /* ERR_RESET_DETECT_H */
