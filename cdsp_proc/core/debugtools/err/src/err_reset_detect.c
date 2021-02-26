/*=============================================================================

                        E r r  R e s e t  D e t e c t i o n
 
GENERAL DESCRIPTION
  This file contains the code to detect HW reset of the chip and
  transition into the downloader.

EXTERNALIZED FUNCTIONS 
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_reset_detect.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/24/10   niting  Added Sahara memory dump support by moving magic numbers 
                   to a specified memory location.
07/25/07   tbg     Initial revision (pulled from boot_hw_reset.c)
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "err_reset_detect.h"

#ifdef FEATURE_DLOAD_HW_RESET_DETECT

#ifndef ERR_SINGLEPROC
  #include "smem.h"
#elif defined FEATURE_SHARED_SEGMENT
  #include "boot_shared_seg.h"
#endif /* if MULTIPROC elif SHARED_SEGMENT */

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#ifndef  ERR_SINGLEPROC
typedef smem_hw_reset_id_type err_reset_id_type;
#elif defined FEATURE_SHARED_SEGMENT
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} err_reset_id_type;

typedef struct
{
  BOOT_SHARED_SEG_HEADER
  err_reset_id_type reset_flags;
} err_reset_shared_type;

#endif /* if ERR_SINGLEPROC elif SHARED_SEGMENT */

static boolean reset_detect_init = FALSE;
static err_reset_id_type* err_reset_id_ptr = NULL;

#endif /* FEATURE_DLOAD_HW_RESET_DETECT */


/* Magic numbers for notifying OEMSBL that a reset has occurred */
#define HW_RESET_DETECT_ENABLED_MAGIC1     0x12345678
#define HW_RESET_DETECT_ENABLED_MAGIC2     0x9ABCDEF0

/* Magic numbers for notifying APPSBL to display dload splash screen */
#define HW_RESET_SPLASH_DLOAD_MAGIC1     0xFCDE8462
#define HW_RESET_SPLASH_DLOAD_MAGIC2     0x0F1E2D3C

/* Magic numbers for notifying APPSBL to display reset splash screen */
#define HW_RESET_SPLASH_RESET_MAGIC1     0x01928374
#define HW_RESET_SPLASH_RESET_MAGIC2     0x7FAEBDC7

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
boolean err_reset_detect_boot_alloc(void)
{
  #if defined FEATURE_DLOAD_HW_RESET_DETECT && defined FEATURE_SHARED_SEGMENT && defined BUILD_BOOT_CHAIN
  err_reset_shared_type *err_reset_shared_info_ptr = NULL;
  
  /* Single proc implementation using SHARED_SEGMENT */
  
  /* Allocate hw_reset_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC( hw_reset_shared,  
    sizeof(err_reset_shared_type), 
    FALSE,
    (void**)(&err_reset_shared_info_ptr)
    );
  
  if(err_reset_shared_info_ptr == NULL)
  {
    err_reset_id_ptr = NULL;
  }
  else
  {
    err_reset_id_ptr = &(err_reset_shared_info_ptr->reset_flags);
  }

  if(err_reset_id_ptr != NULL)
  {
    reset_detect_init = TRUE;
    /* Zero out flags on init (precautionary) */
    err_reset_detect_set(ERR_RESET_NONE);    
  }
  else
  {
    reset_detect_init = FALSE;
  }
  
  return reset_detect_init;
  
  #else

  return FALSE;

  #endif
}

boolean err_reset_detect_init(void)
{
#ifndef FEATURE_DLOAD_HW_RESET_DETECT
  return FALSE;
#else

  
  if(reset_detect_init != TRUE)
  {
    #if defined(MEMDEBUG_HW_RESET_ID_ADDR_PHYS) && defined(BUILD_BOOT_CHAIN)
    err_reset_id_ptr = (err_reset_id_type*) MEMDEBUG_HW_RESET_ID_ADDR_PHYS;

    #elif defined(MEMDEBUG_HW_RESET_ID_ADDR_VIRT) && !defined(BUILD_BOOT_CHAIN)
    err_reset_id_ptr = (err_reset_id_type*) MEMDEBUG_HW_RESET_ID_ADDR_VIRT;

    #elif !defined ERR_SINGLEPROC
    /* Multiproc implementation using SMEM */
    err_reset_id_ptr = smem_get_static_addr(SMEM_HW_RESET_DETECT,
      sizeof(smem_hw_reset_id_type));
    
    #elif defined FEATURE_SHARED_SEGMENT
    /* Single proc implementation using SHARED_SEGMENT */
    {
      err_reset_shared_type *err_reset_shared_info_ptr = NULL;
      boolean seg_data_invalid;

      /* Get the pointer to err_reset_shared */
      seg_data_invalid = BOOT_SHARED_SEG_GET_DATA( hw_reset_shared,
        (void**)(&err_reset_shared_info_ptr)
        );

      if ( (err_reset_shared_info_ptr != NULL) && (seg_data_invalid == FALSE))
      {
        err_reset_id_ptr = &(err_reset_shared_info_ptr->reset_flags);
      }
      else
      {
        err_reset_id_ptr = NULL;
      }
    }
    
    #else /* no supporting feature set defined above, so we can't init the reset detect code */
    err_reset_id_ptr = NULL;
    #endif /* MULTIPROC elif SHARED_SEGMENT else */
    
    if(err_reset_id_ptr != NULL)
    {
      reset_detect_init = TRUE;
    }
  }
  return reset_detect_init;
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */
}

/*===========================================================================

FUNCTION  ERR_RESET_DETECT_ENABLE

DESCRIPTION
 This function will init hw reset detect flags.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void err_reset_detect_enable(void)
{
  err_reset_detect_set(ERR_RESET_DETECT_ENABLED);
}

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
void err_reset_detect_disable(void)
{
  err_reset_detect_set(ERR_RESET_NONE);
}

void err_reset_detect_set(err_reset_type state)
{
#ifndef FEATURE_DLOAD_HW_RESET_DETECT
  return;
#else
  err_reset_detect_init();
  if(!reset_detect_init)
  {
    return;
  }
  
  switch(state)
  {
    case ERR_RESET_NONE:
      err_reset_id_ptr->magic_1 = 0;
      err_reset_id_ptr->magic_2 = 0;
      break;
      
    case ERR_RESET_DETECT_ENABLED:
      err_reset_id_ptr->magic_1 = HW_RESET_DETECT_ENABLED_MAGIC1;
      err_reset_id_ptr->magic_2 = HW_RESET_DETECT_ENABLED_MAGIC2;    
      break;
      
    case ERR_RESET_SPLASH_RESET:
      err_reset_id_ptr->magic_1 = HW_RESET_SPLASH_RESET_MAGIC1;
      err_reset_id_ptr->magic_2 = HW_RESET_SPLASH_RESET_MAGIC2;
      break;
      
    case ERR_RESET_SPLASH_DLOAD:
      err_reset_id_ptr->magic_1 = HW_RESET_SPLASH_DLOAD_MAGIC1;
      err_reset_id_ptr->magic_2 = HW_RESET_SPLASH_DLOAD_MAGIC2;
      break;
      
    default:
      break;
  }
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */
}

/*===========================================================================

FUNCTION  ERR_RESET_STATUS

DESCRIPTION
  This function will clear hw reset detect flags.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Boolean -- TRUE if flags are set, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

err_reset_type err_reset_detect_status(void)
{
#ifndef FEATURE_DLOAD_HW_RESET_DETECT
  /* If feature is not supproted return FALSE */
  return ERR_RESET_NONE;
#else
  err_reset_type rval = ERR_RESET_NONE;
  
  err_reset_detect_init();

  if(!reset_detect_init)
  {
    rval = ERR_RESET_NONE;
  }
  else if (
    (err_reset_id_ptr->magic_1 == HW_RESET_DETECT_ENABLED_MAGIC1) &&
    (err_reset_id_ptr->magic_2 == HW_RESET_DETECT_ENABLED_MAGIC2))
  {
    rval = ERR_RESET_DETECT_ENABLED;
  }
  else if (
    (err_reset_id_ptr->magic_1 == HW_RESET_SPLASH_RESET_MAGIC1) &&
    (err_reset_id_ptr->magic_2 == HW_RESET_SPLASH_RESET_MAGIC2))
  {
    rval = ERR_RESET_SPLASH_RESET;
  }
  else if (
    (err_reset_id_ptr->magic_1 == HW_RESET_SPLASH_DLOAD_MAGIC1) &&
    (err_reset_id_ptr->magic_2 == HW_RESET_SPLASH_DLOAD_MAGIC2))
  {
    rval = ERR_RESET_SPLASH_DLOAD;
  }  
  else
  {
    rval = ERR_RESET_NONE;
  }

  return rval;
#endif
}

