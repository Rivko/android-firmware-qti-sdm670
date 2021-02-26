/**

@file   icb_sdi.h

@brief  ICB support for SDI. Save/restore BRIC segment/remap
        
===========================================================================

                             Edit History


when         who     what, where, why
----------   ---     --------------------------------------------------------
2015/02/23   sds     Created 

===========================================================================
           Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                       All Rights Reserved.
                       QUALCOMM Proprietary
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stdbool.h>

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Save BRIC SEGMENT and REMAP registers for SDI.
*/ 
/*============================================================================*/
void icb_sdi_save( void );

//*============================================================================*/
/**
@brief 
      Restore BRIC SEGMENT and REMAP registers for SDI.
 
@dependencies
      icb_sdi_save() must have been called first.
 
@return    
      true if successful
      false if no valid data to restore
*/ 
/*============================================================================*/
bool icb_sdi_restore( void );
