/*===========================================================================

                              XBL Display Lib Interface

GENERAL DESCRIPTION
  This module provides an interface to boot font lib api's for display 
  on screen.

Copyright 2016, 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/05/18   aus        Added support to handle display not supported scenario
05/23/16   kpa        Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "BootDisplay.h"
#include "boot_error_if.h"
#include "Font.h"
#include "DALSys.h"


/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/
font_info_type *font_info_ptr = NULL;

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
                        PUBLIC FUNCTION DEFINITIONS
=============================================================================*/

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
*   Boolean
* 
*/
boolean boot_display_init( void )
{
  DALResult result;
  
  /*Allocate structure to hold font engine meta info*/
  result = DALSYS_Malloc(sizeof(font_info_type), (void *)&font_info_ptr);  
  BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result);
  
  if(boot_font_init(font_info_ptr))
  {
	  return TRUE;
  }
  else
  {
     /* Free Allocated Buffers */
     result = DALSYS_Free((void *)&font_info_ptr);  
     BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result); 
  	 return FALSE;
  }
}

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
void boot_display_deinit( void )
{
  DALResult result;
  
  /* Free Allocated Buffers */
  if(font_info_ptr != NULL)
  {
     result = DALSYS_Free((void *)&font_info_ptr);  
     BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result); 
  }
}

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
)     
{
  boot_font_display_buffer(font_info_ptr,  
  message_ptr, message_length,
  scale_factor, scale_buffer_ptr, scale_buffer_size);
  
}

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
)
{
  boot_font_display_scaled_message(font_info_ptr, message_ptr,
    scale_factor, scale_buffer_ptr, scale_buffer_size);
}

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
void boot_display_message( uint8 *message_ptr)
{
  boot_font_display_message(font_info_ptr, message_ptr);                             
}

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
)
{
  return boot_set_scale_factor(font_info_ptr,scale);
}

/*===========================================================================
**  Function :  boot_display_set_font_color
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the default color for future font display
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
)
{
  boot_set_font_color(font_info_ptr, blue, green, red, alpha);
}
