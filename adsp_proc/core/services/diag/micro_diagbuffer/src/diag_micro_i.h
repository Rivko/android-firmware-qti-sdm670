#ifndef DIAG_MICRO_I_H
#define DIAG_MICRO_I_H
/*==========================================================================

              Diag APIs for uImage 

General Description
  
Copyright (c) 2015, 2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/diag_micro_i.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/18   is      Add support for pd threshold buffering mode in island mode
08/18/15   xy      Created
===========================================================================*/

#ifdef DIAG_UIMAGE_MODE
#define DIAG_UIMAGE_BUFFERED_HIGH_PER_WM (90) /* Exit island mode and drain when uimage buffer hits this % full if it's in threshold buffering mode */
#endif /* DIAG_UIMAGE_MODE */


/*===========================================================================
FUNCTION LOG_MASK_ENABLED

DESCRIPTION
  Checks if the log is enabled in the log mask.
  
PARAMETERS
  mask_ptr - pointer to the log mask
  id - log id
  item - equipment id
  
RETURN VALUE
  TRUE -  The log is enabled in the log mask
  FALSE - Otherwise.
  
===========================================================================*/
boolean log_mask_enabled (const byte *mask_ptr, unsigned int id, unsigned int item);
 

/*==========================================================================
 FUNCTION  micro_diagbuffer_drain_check
 
 DESCRIPTION
   This function checks the current bytes used in Diag micro diag buffer
   against the watermark value and triggers draining if the thresholds have been
   reached and it is in threhold buffering mode.
 
 PARAMETERS
   None
 
 RETURN VALUE
   None 
 
 =========================================================================*/
#ifdef DIAG_UIMAGE_MODE
 void micro_diagbuffer_drain_check( void );
#endif

 
#endif /*DIAG_MICRO_I_H*/