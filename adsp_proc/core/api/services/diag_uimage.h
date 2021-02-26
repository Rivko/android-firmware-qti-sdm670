#ifndef DIAG_UIMAGE_H
#define DIAG_UIMAGE_H
/*!
@ingroup diag_misc
  @file diag_uimage.h 
  @brief

  This file contains APIs for controlling buffering and streaming mode
  for the uimage Diag buffer.
  
*/  

/*
Copyright (c) 2018 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/services/diag_uimage.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/18   is      Add support for pd threshold buffering mode in island mode
===========================================================================*/

#include "comdef.h"    /* for byte and boolean */
#include "diag_defs.h" /* For buffering mode defines */


/*!
<!-- diag_set_uimage_tx_mode -->

@ingroup diag_uimage
@brief
   This function is to toggle the Diag uimage buffer buffering modes
   between threshold buffering and circular buffering.
   
   @param[in] mode -- parameter to set mode change value
   <ul>
     <li> mode == DIAG_TX_MODE_BUFFERED_THRESH ; Threshold buffering mode 
     <li> mode == DIAG_TX_MODE_BUFFERED_CIR ; Circular buffering mode 
   </ul>
   
   @dependencies
   None

   @return
   <ul>
      <li> DIAG_TX_MODE_INVALID -- Invalid mode request
      <li> DIAG_TX_MODE_BUFFERED_THRESH -- Mode set to threshold buffering
      <li> DIAG_TX_MODE_BUFFERED_CIR  -- Mode set to circular buffering
   </ul>

   @sideeffects
   None

   @sa
   None
*/
 uint8 diag_set_uimage_tx_mode( uint8 mode );

 
 /*!
<!-- diag_get_uimage_tx_mode -->

@ingroup diag_uimage
@brief
   This function is to get information about the uimage buffering mode.
   
   @param
   None
   
   @dependencies
   None

   @return
   <ul>
      <li> DIAG_TX_MODE_INVALID -- Mode hasn't been set yet
      <li> DIAG_TX_MODE_BUFFERED_THRESH -- Mode set to threshold buffering
      <li> DIAG_TX_MODE_BUFFERED_CIR  -- Mode set to circular buffering
   </ul>

   @sideeffects
   None

   @sa
   None
*/
uint8 diag_get_uimage_tx_mode( void );


 /*!
<!-- diag_set_uimage_tx_high_param_wm -->

@ingroup diag_uimage
@brief
   This function sets the high watermark percentage for uimage buffer to 
   trigger a drain while in threshold buffering mode. While in threshold 
   buffering mode, a drain is triggered when the number of buffered bytes has 
   reached the high watermark level and will stop draining when the buffer is 
   empty.
   
   @param
   high_per_value[in] - High watermark value (as a percentage of total uimage buffer size).
   
   @dependencies
   None

   @return
   TRUE - The high wm value was updated successfully.
   FALSE - The high watermark value was NOT updated successfully.

   @sideeffects
   None

   @sa
   None
*/
 boolean diag_set_uimage_tx_high_param_wm( uint8 high_per_value );
   
   
/*!
<!-- diag_get_uimage_tx_high_param_wm -->

@ingroup diag_uimage
@brief
   This function returns the high watermark percentage for uimage buffer to 
   trigger a drain while in threshold buffering mode.
   
   @param
   None
   
   @dependencies
   None

   @return
   High watermark percentage for uimage buffer 

   @sideeffects
   None

   @sa
   None
*/
 uint8 diag_get_uimage_tx_high_param_wm( void );


#endif /* DIAG_UIMAGE_H */
