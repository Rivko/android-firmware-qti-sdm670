#ifndef BOOT_FONT_H
#define BOOT_FONT_H

/*===========================================================================

                              XBL Font Engine Header

GENERAL DESCRIPTION
  This module provides support to display text on screen. 

Copyright 2016, 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/05/18   aus        Update the return from boot_font_init
05/02/16   kpa        Initial revision.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/

#include "boot_comdef.h"
#include "com_dtypes.h"
#include "DisplayLib.h"


/*=============================================================================

                              DEFINITIONS

=============================================================================*/
/* Text display constants in pixels*/

#define ROW_START_X_POSITION   0
#define ROW_START_Y_POSITION   0

#define DISPLAY_FLAGS 0x0                    

/* A glyph shall be 19x8 pixels with 4 bytes argb color data per pixel
  So glyph buff size is 19 x 8 pixel bitmap x 4 bytes color ie 608 bytes
*/
#define GLYPH_HEIGHT_IN_PIXEL 19
#define GLYPH_WIDTH_IN_PIXEL  8 
#define PIXEL_SIZE_IN_BYTES   4
#define GLYPH_BUFF_SIZE_BYTES (GLYPH_HEIGHT_IN_PIXEL * GLYPH_WIDTH_IN_PIXEL * PIXEL_SIZE_IN_BYTES) 
#define GLYPH_BUFF_2X_SIZE_BYTES (GLYPH_BUFF_SIZE_BYTES * SCALE_2X * SCALE_2X) 
#define GLYPH_BUFF_3X_SIZE_BYTES (GLYPH_BUFF_SIZE_BYTES * SCALE_3X * SCALE_3X)


/*Macro Defines for Color in (Blue,Green,Red,Alpha) value quadruplets */
#define COLOR_RED     0x00,0x00,0xFF,0x00
#define COLOR_BLUE    0xFF,0x00,0x00,0x00
#define COLOR_GREEN   0x00,0xFF,0x00,0x00
#define COLOR_YELLOW  0x00,0xFF,0xFF,0x00
#define COLOR_WHITE   0xFF,0xFF,0xFF,0x00



/**
  Definition of struct detailing row info.
**/
typedef struct row_info_type 
{
  uintnt x_pos;        // location of start of row in pixels in X dimension.
  uintnt y_pos;        // location of start of row in pixels in Y dimension.
  uintnt height;       // The height of the line, in pixels.
  uintnt width;        // The width of the text on the line, in pixels.
} row_info_type;

typedef struct pixel_data_type 
{
  uint8 pixel_b;       //blue component value byte
  uint8 pixel_g;       //green component value byte
  uint8 pixel_r;       //red component value byte
  uint8 pixel_a;       //alpha component value byte
}pixel_data_type;

typedef struct glyph_info_type 
{
  uintnt x_pos;        // location of glyph in pixels in X dimension.
  uintnt y_pos;        // location of glyph in pixels in Y dimension.
  uintnt height;       // The height of the glyph, in pixels.
  uintnt width;        // The width of the glyph in pixels.
  pixel_data_type *pixel_data;
  uint8  *argb_buffer; // Buffer containing pixel data in argb8888 format
} glyph_info_type;

typedef enum
{
  SCALE_1X  = 1,              
  SCALE_2X  = 2,                           /* Double the glyph size*/
  SCALE_3X  = 3,                           /* Triple the glyph size*/
  SCALE_4X  = 4,                           /* Quadruple the glyph size*/
  SCALE_FACTOR_SIZE_ENUM   = 0x7FFFFFFF   /* Size enum to 32 bits. */
} scale_factor_type;

#define SCALE_MAX SCALE_4X
  
/**
  Structure for Font driver info.
**/
typedef struct font_driver_state_type 
{
  boolean init;
  row_info_type current_row;  
  pixel_data_type pixel_info;
  scale_factor_type gscale_factor;
  scale_factor_type max_scale;
  DisplayLibBufferInfoType gframe_buffer_info;
  uintnt max_row_width_in_pixel;
} font_info_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
**  Function :  boot_font_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes font engine, display driver 
*   and other dependencies
*
* @param[in] 
* font_ptr               - structure to hold font driver paramters
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
boolean boot_font_init
( 
  font_info_type *font_ptr
 );

/*===========================================================================
**  Function :  boot_font_display_message
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays a string onto display
*
* @param[in] 
*   font_ptr     - pointer to font driver meta info
*   message_ptr  - String to be displayed
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_font_display_message(font_info_type *font_ptr, uint8 *message_ptr);

/*===========================================================================
**  Function :  boot_font_display_scaled_message
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays a string onto display with appropriate scaling
*
* @param[in] 
*            font_ptr           - pointer to font driver meta info
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
void boot_font_display_scaled_message
(
  font_info_type *font_ptr,
  uint8 *message_ptr,
  scale_factor_type  scale_factor,
  uint8 *scale_buffer_ptr,
  uintnt scale_buffer_size
);

/*===========================================================================
**  Function :  boot_font_display_buffer
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays a string onto display with appropriate scaling
*
* @param[in] 
*            font_ptr           - pointer to font driver meta info
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
void boot_font_display_buffer
(
  font_info_type *font_ptr,
  uint8 *message_ptr,
  uintnt message_length,
  scale_factor_type  scale_factor,
  uint8 *scale_buffer_ptr,
  uintnt scale_buffer_size   
);

/*===========================================================================
**  Function :  boot_set_scale_factor
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the default scale factor to be used
*
* @param[in] 
*   font_ptr        - pointer to font driver meta
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
boolean boot_set_scale_factor
(
  font_info_type *font_ptr,
  scale_factor_type scale
);

/*===========================================================================
**  Function :  boot_set_font_color
** ==========================================================================
*/
/*!
* 
* @brief
*   This function sets the default color to be for future font display
*
* @param[in] 
*            font_ptr - pointer to font driver meta
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
void boot_set_font_color
(
  font_info_type *font_ptr,
  uint8 blue,
  uint8 green,
  uint8 red,
  uint8 alpha
);

#endif /* BOOT_FONT_H */

