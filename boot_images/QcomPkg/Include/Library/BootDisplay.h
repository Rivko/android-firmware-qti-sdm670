#ifndef BOOT_DISPLAY_H
#define BOOT_DISPLAY_H

/*===========================================================================

                              XBL Display Lib Interface Header

GENERAL DESCRIPTION
  This module provides a wrapper/interface to boot font lib api's 
  for display on screen.

Copyright 2016, 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/05/18   aus        Update the return from boot_display_init
05/23/16   kpa        Initial revision.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/

#include "Font.h"

/*=============================================================================

                              DEFINITIONS

=============================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
**  Function :  boot_display_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes boot display lib
*
* @param[in] 
*   None
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
boolean boot_display_init( void );

/*===========================================================================
**  Function :  boot_display_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes boot display lib
*
* @param[in] 
*   None
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_display_deinit( void );

/*===========================================================================
**  Function :  boot_display_message
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays a string onto display
*
* @param[in] 
*   message_ptr  - String to be displayed
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_display_message( uint8 *message_ptr );

/*===========================================================================
**  Function :  boot_display_scaled_message
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays a string onto display with appropriate scaling
*
* @param[in] 
*            message_ptr        - String to be displayed
*            scale_factor       - Image scaling multiplier
*            scale_buffer_ptr   - Buffer for scaling
*            scale_buffer_size  - Size of Buffer for scaling
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_display_scaled_message
(
  uint8 *message_ptr,
  scale_factor_type  scale_factor,
  uint8 *scale_buffer_ptr,
  uintnt scale_buffer_size
);

/*===========================================================================
**  Function :  boot_display_buffer
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays a string onto display with appropriate scaling
*
* @param[in] 
*            message_ptr        - Buffer to be displayed
*            message_length     - Length of Buffer to be displayed
*            scale_factor       - Image scaling multiplier
*            scale_buffer_ptr   - Bufer for Image scaling
*            scale_buffer_size  - Image scaling Buffer Size
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_display_buffer
(
  uint8 *message_ptr,
  uintnt message_length,
  scale_factor_type  scale_factor,
  uint8 *scale_buffer_ptr,
  uintnt scale_buffer_size   
);

/*===========================================================================
**  Function :  boot_display_set_scale_factor
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the default scale factor to be used
*
* @param[in] 
*   scale           - Default glyph Scaling factor for all future 
*                     display operations
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE                If default scale factor was updated
*   FALSE               If update failed.
* 
*/
boolean boot_display_set_scale_factor
(
  scale_factor_type scale
);

/*===========================================================================
**  Function :  boot_display_set_font_color
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the default color to be for future font display
*
* @param[in] 
*            blue     - blue color component value
*            green    - green color component value
*            red      - Red color component value
*            alpha    - alpha component value
*            
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_display_set_font_color
(
  uint8 blue,
  uint8 green,
  uint8 red,
  uint8 alpha
);

#endif /* BOOT_DISPLAY_H */

