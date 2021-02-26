/*===========================================================================

                              XBL Font Engine

GENERAL DESCRIPTION
  This module provides support to display text on screen. 

Copyright 2016-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/05/18   aus        Added support to handle display not supported scenario
05/31/18   yps        Fixed ramdump crashed at font.c
04/27/17   kpa        Update dalsys free api argument.
05/02/16   kpa        Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "boot_util.h"
#include "boot_error_if.h"
#include "StdFont.h"
#include "Font.h"
#include "DALSys.h"

#define ASCII_FONT_MAP_START 0x20
#define ASCII_FONT_MAP_END   0x7e
#define GLYPH_COLUMN_MSB     0x80

/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_clear_screen
** ==========================================================================
*/
/*!
* 
* @brief
*   This function clears the display frame buffer row
*
* @param[in] 
*         gframe_buffer_info - pointer to display driver meta info
*         x_pos, y_pos       - start coordinates of region to be cleared
*         width              - width if region to be cleared
*         height             - height of region to be cleared
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_clear_screen
(
  DisplayLibBufferInfoType *gframe_buffer_info,
  uintnt x_pos,
  uintnt y_pos,
  uintnt width,
  uintnt height
)     
{
  pixel_data_type clear_pixel;
  DisplayLibBufferInfoType  clear_region;
  MDP_Status status;
  
  BL_VERIFY((gframe_buffer_info != NULL && 
             gframe_buffer_info->pBufferAddr != NULL), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
            
  qmemset((void *)&clear_pixel,0x0, sizeof(pixel_data_type));
  
  clear_region.uWidth = 1;
  clear_region.uHeight = 1;
  clear_region.uStride = 1 * 4;
  clear_region.pBufferAddr = &clear_pixel;
  
  status = DisplayBlt( gframe_buffer_info,
                        x_pos,            /* DestX */
                        y_pos,            /* DestY */
                        DISPLAY_BLT_FRAMEFILL,
                        &clear_region,
                        0,                /* SrcX */
                        0,                /* SrcY */
                        width,  
                        height,
                        DISPLAY_FLAGS);
                        
  BL_VERIFY((status == MDP_STATUS_OK), 
            status|BL_ERROR_GROUP_DISPLAY);                        
}

/*===========================================================================
**  Function :  boot_goto_next_row
** ==========================================================================
*/
/*!
* 
* @brief
*   This function updates row data struct to point to next row
*
* @param[in] 
*         font_ptr       - pointer to font driver meta info
*         character      - char to be checked
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE                - If input character is a word break char
*   FALSE               - In all other cases
* 
*/
static void boot_goto_next_row( font_info_type *font_ptr)     
{
	
  //Point to next row
  font_ptr->current_row.x_pos = ROW_START_X_POSITION;
  font_ptr->current_row.width = 0;  
  if((font_ptr->current_row.y_pos + font_ptr->current_row.height +
     (GLYPH_HEIGHT_IN_PIXEL * font_ptr->gscale_factor))
      < font_ptr->gframe_buffer_info.uHeight)
  {
    font_ptr->current_row.y_pos += font_ptr->current_row.height;
  }
  else
  {
    font_ptr->current_row.y_pos = ROW_START_Y_POSITION;
  }
  // Clear next row
  boot_clear_screen(&font_ptr->gframe_buffer_info, font_ptr->current_row.x_pos, 
                 font_ptr->current_row.y_pos, font_ptr->max_row_width_in_pixel,
                 (GLYPH_HEIGHT_IN_PIXEL * font_ptr->gscale_factor));
}

/*===========================================================================
**  Function :  boot_glyph_to_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function transforms input glyph into a image suitable for display
*   It references the font bitmap and prepares framebuffer to be consumed by
*   Display driver
*
* @param[in] character      - Glyph to be displayed 
*            current_glyph  - glyph structure describing output buffer
* 
* @retval
*   TRUE - If character was successfully mapped else FALSE
* 
*/
static boolean boot_glyph_to_image(uint8 character, glyph_info_type *current_glyph)
{
  uint8 *buff_ptr, *font_map, bit_mask = GLYPH_COLUMN_MSB;
  boolean status = FALSE;
  uintnt column_index, row_index;
  
  
  BL_VERIFY((current_glyph != NULL), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT); 
  do
  { 
    /* Validate character. Only ascii chars 0x20 to 0x7e supported */
    if((character < ASCII_FONT_MAP_START) || (character > ASCII_FONT_MAP_END) )
    {
      break;
    }
    buff_ptr = current_glyph->argb_buffer;
    font_map = &gStdNarrowFontData[(character - ASCII_FONT_MAP_START)*GLYPH_HEIGHT_IN_PIXEL];
    
    for(row_index =0; row_index < GLYPH_HEIGHT_IN_PIXEL; row_index++ ) 
    {
      bit_mask = GLYPH_COLUMN_MSB; // Start with MSB
      for(column_index =0; column_index < GLYPH_WIDTH_IN_PIXEL; column_index++)
      {
        if((*font_map & bit_mask) !=0)
        {
          qmemcpy((void *)buff_ptr, (void *)current_glyph->pixel_data, sizeof(pixel_data_type));
        }
        else
        {
          qmemset((void *)buff_ptr,0x0, sizeof(pixel_data_type));
        }
        bit_mask = bit_mask >> 0x1;
        buff_ptr = (uint8 *)((uintnt)buff_ptr + (uintnt)sizeof(pixel_data_type));
      }
      /* point to next row from bitmap. */
      font_map++;
    }    
    status = TRUE;
    
  }while(0);
  return status;
} 

/*===========================================================================
**  Function :  boot_scale_image_buffer
** ==========================================================================
*/
/*!
* 
* @brief
*   This function scales Up the image according to scale factor. Only the 
*   output buffer is updated to have resized data. The user of the api has 
*   to take care of updating new scaled dimensions and other factors
*
*   Note: Does not support downscaling/ reducing glyph size. User has to 
*         ensure dst_buffer is big enough to contain resized image.
*
* @param[in] current_glyph - Pointer to the glyph data structure 
*            dst_buffer    - Output buffer containing scaled image.
*            scale_factor  - scale multiplier.
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If scaling operation is successful else FALSE
* 
*/
static boolean boot_scale_image_buffer
(
  glyph_info_type *current_glyph,
  uint8 *dst_buffer,
  scale_factor_type scale_factor
)
{
  boolean status = FALSE;
  pixel_data_type *src_pixel_ptr, *dst_pixel_ptr, *dst_pixel_row_ptr;
  uintnt src_row_index, src_col_index;
  scale_factor_type  scale_index;

  BL_VERIFY((current_glyph != NULL)&& (dst_buffer != NULL), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  do
  {
    if(current_glyph->argb_buffer == NULL || scale_factor < SCALE_1X)
    {
      break;
    }
    dst_pixel_ptr = (pixel_data_type *)dst_buffer;
    
    for(src_row_index =0; src_row_index < GLYPH_HEIGHT_IN_PIXEL; src_row_index++)
    {
      src_pixel_ptr = (pixel_data_type *)((uintnt)current_glyph->argb_buffer + 
                      (src_row_index * sizeof(pixel_data_type) * GLYPH_WIDTH_IN_PIXEL));
      
      // store current row pointer in scaled buffer
      dst_pixel_row_ptr = dst_pixel_ptr;
                      
      // Scale width for each row (in x direction)
      for(src_col_index =0; src_col_index < GLYPH_WIDTH_IN_PIXEL; src_col_index++)
      {
        // scale every pixel in row
        for(scale_index = SCALE_1X; scale_index <= scale_factor; scale_index++)
        {
          qmemcpy((void *)dst_pixel_ptr, (void *)src_pixel_ptr, sizeof(pixel_data_type));
          dst_pixel_ptr++;
        }
        src_pixel_ptr++;
      }
      //copy the scaled row in y direction (scale -1) times to complete the scaling in both coordinates
      for(scale_index = SCALE_1X; scale_index < scale_factor; scale_index++)
      {
        qmemcpy((void *)dst_pixel_ptr, (void *) dst_pixel_row_ptr,
                (sizeof(pixel_data_type)*GLYPH_WIDTH_IN_PIXEL * scale_factor));
                      
        dst_pixel_ptr = (pixel_data_type *)((uintnt)dst_pixel_ptr + 
                           (sizeof(pixel_data_type)* GLYPH_WIDTH_IN_PIXEL * scale_factor));     
      }
    }
    status = TRUE;
  }while(0);
  return status;
}

/*===========================================================================
**  Function :  boot_is_special_character
** ==========================================================================
*/
/*!
* 
* @brief
*   This function detects if a special character is under process
*
* @param[in] 
*         font_ptr          - pointer to font driver meta info
*         character         - pointer to character
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static boolean boot_is_special_character (font_info_type *font_ptr, uint8* character)
{
  boolean status = FALSE;
  BL_VERIFY((character != NULL), 
          BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
          
  if(*character == (uint8)0xD)  // carriage return
  {
    font_ptr->current_row.x_pos = ROW_START_X_POSITION;
    font_ptr->current_row.width = 0;
    status = TRUE;
  }
  if(*character == (uint8)0xA)  // newline
  {
    if(font_ptr->current_row.y_pos + font_ptr->current_row.height + GLYPH_HEIGHT_IN_PIXEL*font_ptr->gscale_factor
       > font_ptr->gframe_buffer_info.uHeight)
    {
      //reset to first row
      font_ptr->current_row.y_pos = ROW_START_Y_POSITION;
    }
    else
    {
      font_ptr->current_row.y_pos += font_ptr->current_row.height; 
    }
    status = TRUE;
    
    // Clear next row
    boot_clear_screen(&font_ptr->gframe_buffer_info, font_ptr->current_row.x_pos, 
                    font_ptr->current_row.y_pos, font_ptr->max_row_width_in_pixel, 
                     (GLYPH_HEIGHT_IN_PIXEL * font_ptr->gscale_factor));     
  }
  return status;
}

/*===========================================================================
**  Function :  boot_is_word_break
** ==========================================================================
*/
/*!
* 
* @brief
*   This function checks if the input character is word break char such as
*   a space
*
* @param[in] character      - char to be checked
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE                - If input character is a word break char
*   FALSE               - In all other cases
* 
*/
static boolean boot_is_word_break(uint8 character)     
{
  boolean status = FALSE;
  if(character == ' ')  // character is space
  {
    status = TRUE;
  }
  return status;
}

/*===========================================================================
**  Function :  boot_display_substring
** ==========================================================================
*/
/*!
* 
* @brief
*   Function to display words and spaces in input string
*
* @param[in] 
*   font_ptr          - pointer to font driver meta info
*   substring_ptr     - start of substring to be displayed
*   message_ptr       - Original message string
*   index             - location of char being processed in string
*   scale_factor      - Image scaling multiplier
*   scale_buffer_ptr  - buffer for scaling
*   scale_buffer_size - Size of scaling buffer
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE                - If input character is a word break char
*   FALSE               - In all other cases
* 
*/
static void boot_display_substring
(
  font_info_type *font_ptr,
  uint8 **substring_ptr,
  uint8 *message_ptr,
  uintnt index,
  scale_factor_type  scale_factor,
  uint8 *scale_buffer_ptr,
  uintnt scale_buffer_size  
)    
{
  uintnt substring_length = 0;
  substring_length = ((uintnt)message_ptr + index - (uintnt)*substring_ptr);
  
  //Calculate remaining space in row. substring_length has number of glyphs(chars). 
  //need to convert them to pixels
  if( (substring_length * GLYPH_WIDTH_IN_PIXEL * scale_factor) 
                            < font_ptr->max_row_width_in_pixel &&
      ((substring_length * GLYPH_WIDTH_IN_PIXEL * scale_factor) >
       (font_ptr->gframe_buffer_info.uWidth - 
         (font_ptr->current_row.x_pos + font_ptr->current_row.width))
      )
    )
  {
    boot_goto_next_row(font_ptr);
  }
  boot_font_display_buffer(font_ptr, *substring_ptr, substring_length, 
                      scale_factor, scale_buffer_ptr, scale_buffer_size);
  *substring_ptr = (uint8*)((uintnt)message_ptr + index);
}


/*===========================================================================
                        PUBLIC FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_font_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes Font engine, display driver
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
 )
{
  uint32 uflags = DISPLAY_FLAGS;
  MDP_Status eStatus;
  
  BL_VERIFY((font_ptr != NULL), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT); 
  
  qmemset((void *)font_ptr,0x0, sizeof(font_info_type));
              
  /* Initialize Display Driver*/
  eStatus = DisplayInitialize(&(font_ptr->gframe_buffer_info), uflags);
  
  if(eStatus != MDP_STATUS_OK) return FALSE;

  BL_VERIFY((font_ptr->gframe_buffer_info.pBufferAddr != NULL), 
          BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);   
   
  font_ptr->max_row_width_in_pixel = font_ptr->gframe_buffer_info.uWidth
                                      - ROW_START_X_POSITION;
   
  font_ptr->gscale_factor = SCALE_1X;
  
  /*Initialize data structures */
  font_ptr->current_row.x_pos = ROW_START_X_POSITION;
  font_ptr->current_row.y_pos = ROW_START_Y_POSITION;
  font_ptr->current_row.height = GLYPH_HEIGHT_IN_PIXEL;
  font_ptr->current_row.width = 0;
  
  /*Set Default Color info */
  font_ptr->pixel_info.pixel_b = 0xFF;  
  font_ptr->pixel_info.pixel_g = 0xFF;
  font_ptr->pixel_info.pixel_r = 0xFF;
  font_ptr->pixel_info.pixel_a = 0;
  
  font_ptr->init = TRUE;
  
  boot_clear_screen(&font_ptr->gframe_buffer_info, 0, 0,
                    font_ptr->gframe_buffer_info.uWidth, 
                    font_ptr->gframe_buffer_info.uHeight);  
  
  return TRUE;  
}

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
)     
{
  uintnt index = 0;
  boolean status;
  glyph_info_type current_glyph;
  uint8 argb_glyph_buff[GLYPH_BUFF_SIZE_BYTES];
  MDP_Status disp_status;
  
  DisplayLibBufferInfoType pBltBufferInfo;
  
  BL_VERIFY((message_ptr != NULL && font_ptr != NULL &&
            font_ptr->init == TRUE), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
   
  //  Before proceeding ensure current row can accomodate glyph
  //  within display buffer bounds
  if(font_ptr->current_row.y_pos + (GLYPH_HEIGHT_IN_PIXEL * scale_factor)
     > font_ptr->gframe_buffer_info.uHeight)
  {
    //reset to first row
    font_ptr->current_row.x_pos = ROW_START_X_POSITION;
    font_ptr->current_row.y_pos = ROW_START_Y_POSITION;
    font_ptr->current_row.width = 0;
    boot_clear_screen(&font_ptr->gframe_buffer_info, font_ptr->current_row.x_pos,
                      font_ptr->current_row.y_pos, font_ptr->max_row_width_in_pixel, 
                      (GLYPH_HEIGHT_IN_PIXEL * scale_factor));  
  }
  
  for(index = 0; index < message_length; index++)
  {
    current_glyph.height = GLYPH_HEIGHT_IN_PIXEL;  
    current_glyph.width = GLYPH_WIDTH_IN_PIXEL;  
    current_glyph.argb_buffer = &argb_glyph_buff[0];  
    current_glyph.pixel_data = &font_ptr->pixel_info;
    
    //Process special characters
    if( boot_is_special_character(font_ptr, &message_ptr[index]) == TRUE)
    {
      continue;
    }
    
    // convert character to glyph before blt operation
    status = boot_glyph_to_image(message_ptr[index],
                                &current_glyph); 
    
    
    if(status == FALSE)
    {
      //If glyph mapping failed or not suppored move to next 
      //character rather than erroring out.
      continue;    
    }
    
    if(scale_factor != SCALE_1X)
    {
      BL_VERIFY((scale_buffer_ptr != NULL), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
            
      BL_VERIFY((scale_buffer_size >= 
                (GLYPH_BUFF_SIZE_BYTES * scale_factor*scale_factor)), 
                BL_ERR_INVALID_DISPLAY_SIZE|BL_ERROR_GROUP_BOOT);            
            
      status = boot_scale_image_buffer(&current_glyph, 
                                       scale_buffer_ptr, 
                                       scale_factor);
      current_glyph.width *= scale_factor;
      current_glyph.height *= scale_factor;
      current_glyph.argb_buffer = scale_buffer_ptr;
    }
    
    BL_VERIFY((current_glyph.width <= font_ptr->gframe_buffer_info.uWidth) && 
            (current_glyph.height <= font_ptr->gframe_buffer_info.uHeight),
             BL_ERR_INVALID_DISPLAY_SIZE|BL_ERROR_GROUP_BOOT);
                               
    // Check if current row has sufficient space for glyph else move to next row
    // and accordingly update coordinates
    if(font_ptr->current_row.x_pos + font_ptr->current_row.width +current_glyph.width 
       > font_ptr->gframe_buffer_info.uWidth)
    {
      // Check if next row can be accomodated on display
      if(font_ptr->current_row.y_pos + font_ptr->current_row.height + current_glyph.height 
         > font_ptr->gframe_buffer_info.uHeight)
      {
        //reset to first row
        font_ptr->current_row.x_pos = ROW_START_X_POSITION;
        font_ptr->current_row.y_pos = ROW_START_Y_POSITION;
        font_ptr->current_row.width = 0;
        boot_clear_screen(&font_ptr->gframe_buffer_info, font_ptr->current_row.x_pos,
                          font_ptr->current_row.y_pos, font_ptr->max_row_width_in_pixel, 
                          (GLYPH_HEIGHT_IN_PIXEL * scale_factor));
      }
      else
      {
        boot_goto_next_row(font_ptr);
      }      
    }
    
    // First update glyph x coordinate then row width to account for 
    // glyph under process
    current_glyph.x_pos = font_ptr->current_row.x_pos + font_ptr->current_row.width;
    font_ptr->current_row.width = font_ptr->current_row.width + current_glyph.width;    
    
    if(font_ptr->current_row.height > current_glyph.height)
    {
      // Adjust glyph's y coordinate
      current_glyph.y_pos = font_ptr->current_row.y_pos +
                            (font_ptr->current_row.height - current_glyph.height);
    }
    else
    {
      if(font_ptr->current_row.height < current_glyph.height)
      {
        // keep track of max glyph height seen so far on a row  
        // so as to prevent next row overlap
        font_ptr->current_row.height = current_glyph.height;
      }
      // Glyph y coordinate remains same in less than or equal to cases.
      current_glyph.y_pos = font_ptr->current_row.y_pos;
    }
                             
    // Blt the character. The buffer contains the entire glyph mapped to image. 
    pBltBufferInfo.uWidth      = current_glyph.width;
    pBltBufferInfo.uHeight     = current_glyph.height;
    pBltBufferInfo.uStride     = (current_glyph.width*PIXEL_SIZE_IN_BYTES);
    pBltBufferInfo.pBufferAddr = (void *)current_glyph.argb_buffer;
    
    disp_status = DisplayBlt( &font_ptr->gframe_buffer_info,
                          current_glyph.x_pos,          // DestX 
                          current_glyph.y_pos,          // DestY 
                          DISPLAY_BLT_BUFFERTOFRAME,
                          &pBltBufferInfo,
                          0,                            // SrcX 
                          0,                            // SrcY  
                          current_glyph.width,    // blt the whole buffer
                          current_glyph.height,
                          DISPLAY_FLAGS);
                          
   BL_VERIFY((disp_status == MDP_STATUS_OK), 
            disp_status|BL_ERROR_GROUP_DISPLAY);   

  }
}

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
)
{
  uintnt message_length = 0, index = 0;
  uint8 *str_start_ptr;

  BL_VERIFY((font_ptr != NULL && message_ptr != NULL &&
             font_ptr->init == TRUE), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
            
  message_length = boot_strlen((char*)message_ptr);
  
  // Support Word Wrapping to avoid word spread across display boundary
  // Accumulate words and spaces into groups and blt accordingly
  str_start_ptr = message_ptr;
  for(index=0; index < message_length; index++)
  {
    if(boot_is_word_break(message_ptr[index]) == TRUE)
    { 
      // character under process is space
      if((index >0) && (boot_is_word_break(message_ptr[index-1]) == FALSE))
      {
        // display word formed so far (first unprocessed space after a word)
        boot_display_substring(font_ptr, &str_start_ptr, message_ptr, index,
                            scale_factor, scale_buffer_ptr, scale_buffer_size);
      }    
    }
    else if((index >0) && (boot_is_word_break(message_ptr[index-1]) == TRUE))
    {
      // This is first word character after multiple spaces
      boot_display_substring(font_ptr, &str_start_ptr, message_ptr, index,
                           scale_factor, scale_buffer_ptr, scale_buffer_size);
    }
  }
  //Display last substring
  boot_display_substring(font_ptr, &str_start_ptr, message_ptr, index, 
                         scale_factor, scale_buffer_ptr, scale_buffer_size);
}

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
void boot_font_display_message(font_info_type *font_ptr, uint8 *message_ptr)
{
/* Buffer for scaling Image*/
  uint8* scale_buffer = NULL;
  DALResult result;
  uintnt scale_size = 0;
  
  BL_VERIFY((font_ptr != NULL && message_ptr != NULL), 
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);  
  
  scale_size = (GLYPH_BUFF_SIZE_BYTES * font_ptr->gscale_factor
                *font_ptr->gscale_factor);
                
  result = DALSYS_Malloc(scale_size, (void *)&scale_buffer);  
  BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result);

  boot_font_display_scaled_message(font_ptr, message_ptr, font_ptr->gscale_factor,
                              scale_buffer, scale_size);
                              
  result = DALSYS_Free((void *)scale_buffer);  
  BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result);                              
}

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
)
{
  boolean status = FALSE;
  if(scale <= SCALE_MAX)
  {
    font_ptr->gscale_factor = scale;
    status = TRUE;
  }
  return status;
}

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
)
{
  font_ptr->pixel_info.pixel_b = blue;  
  font_ptr->pixel_info.pixel_g = green;
  font_ptr->pixel_info.pixel_r = red;  
  font_ptr->pixel_info.pixel_a = alpha;  
}