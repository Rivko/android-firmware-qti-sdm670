#ifndef LAYOUT_MGR_H_
#define LAYOUT_MGR_H_

/** @file layout_manager.h
  @brief
  This file contains the interfaces to the Secure UI Layout Manager.
*/

/*===========================================================================
 Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/07/16   dr      Remove screen dimensions from the layout load API
05/22/16   sn      Added touched object structure.
04/10/16   sn      Refactoring of the API to allow custom layout usage.
12/10/15   kl      (Tech Pubs) Comment updates for 80-NJ546-1 Rev. J.
10/27/15   gs	   Add rendering of the layouts bg image to a secure buffer
09/06/15   ls      Add documentation to all the external functions
03/09/15   sn      Changed the API to get the stride in each rendering.
08/03/15   ls      Add one-time per device keypad randomization.
11/10/13   sn      Initial Version.

===========================================================================*/
#include "SecureTouchLayout.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** @addtogroup layout_manager
@{ */

/** Layout manager return codes. */
typedef enum {
	LAYOUT_MGR_SUCCESS                        = 0,
	/**< Success. */
	LAYOUT_MGR_GENERAL_ERROR                  = -1,
	/**< General error.  */
	LAYOUT_MGR_BAD_LAYOUT                     = -2,
	/**< Received an invalid layout structure. */
	LAYOUT_MGR_GOT_NULL_INPUT                 = -3,
	/**< Received a null parameter. */
	LAYOUT_MGR_UNSUPPORTED                    = -4,
	/**< Operation is not supported with the specified parameters. */
	LAYOUT_MGR_OBJECT_NOT_FOUND               = -5,
	/**< The specified object does not exist in the layout.  */
	LAYOUT_MGR_TEXT_EXCEED_BUFFER             = -6,
	/**< Received text exceeds the given buffer size. */
	LAYOUT_MGR_UNEXPECTED_TIMEOUT             = -7,
	/**< An unexpected timeout event occurred.  */
	LAYOUT_MGR_BUTTON_BUSY                    = -8,
	/**< The button is currently in the PRESSED state, so it cannot be disabled. */
	LAYOUT_MGR_FAILED_GEN_RAND                = -9,
	/**< Random number generation failed. */
	LAYOUT_MGR_INVALID_UTF8_STR               = -10,
	/**< Received an invalid UTF8 string. */
	LAYOUT_MGR_TEXT_EXCEED_OBJECT             = -11,
	/**< Text exceeds the object size limitations.  */
	LAYOUT_MGR_UNCHANGED                      = -12,
	/**< No change occurred in the layout. */
	LAYOUT_MGR_LAYOUT_NOT_LOADED              = -13,
	/**< Layout is not loaded; layout_mgr_load_layout() must be called before 
         calling this function. */
	LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED         = -14,
	/**< Object is not the required type. */
	LAYOUT_MGR_FONT_MISSING_CHAR              = -15,
	/**< Font is missing a character. */
	LAYOUT_MGR_DYNAMIC_FONT_ERROR             = -16,
	/**< Dynamic font error. */
	LAYOUT_MGR_MEM_ALLOC_ERROR                = -17,
	/**< Memory allocation error. */
	LAYOUT_MGR_IMAGE_EXCEED_OBJECT            = -18,
	/**< Image exceeded the object. */
	LAYOUT_MGR_INVALID_IMAGE                  = -19,
	/**< Image has an unsupported format or is corrupted. */

	LAYOUT_MGR_ENTER_PRESSED                  = 1,
	/**< Function button Enter was pressed. */
	LAYOUT_MGR_CANCEL_PRESSED                 = 2,
	/**< Function button Cancel was pressed. */
	LAYOUT_MGR_F1_PRESSED                     = 3,
	/**< Function button F1 was pressed. */
	LAYOUT_MGR_F2_PRESSED                     = 4,
	/**< Function button F2 was pressed. */
	LAYOUT_MGR_F3_PRESSED                     = 5,
	/**< Function button F3 was pressed.  */
    /** @cond */
	LAYOUT_MGR_ERR_SIZE                       = 0x7FFFFFFF
	/**< @endcond */
} layout_mgr_err_t;

typedef enum {
	LAYOUT_MGR_EVENT_NONE                      = 0,
	/**< No event. */
	LAYOUT_MGR_EVENT_UP                        = 1,
	/**< UP touch event. */
	LAYOUT_MGR_EVENT_DOWN                      = 2,
	/**< DOWN touch event. */
	LAYOUT_MGR_EVENT_MOVE                      = 3,
	/**< MOVE touch event. @newpage */
    /** @cond */
	LAYOUT_MGR_EVENT_SIZE                      = 0x7FFFFFFF
	/**< @endcond */
} layout_mgr_event_t;

typedef struct{
	const char* objectName;         /**< Object name. */
	layout_mgr_event_t eventType; 	/**< Event type. */
} layout_mgr_touched_obj_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
   External function that loads a layout struct. This function must be called
   before using most other functions in the Layout Manager API.

   @param[in] layoutPage  Secure UI screen layout that represents the appearance 
                          of the screen statically. This layout will be later 
                          validated and loaded on the screen.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_BAD_LAYOUT
*/

layout_mgr_err_t layout_mgr_load_layout(LayoutPage_t* layoutPage);

/*****************************************************************************************************************/

/**
   External function for setting the buffer of the label of some input object
   in the layout.

   @note1hang This function does not set the text itself (curr_len is zeroed).

   @param[in] objectName  Object name (as the unique identifier in the layout).
   @param[in] buffer      Buffer to be set for this object.
   @param[in] len         Length of the buffer, in bytes.
   
   @return 
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
   @newpage
*/
layout_mgr_err_t layout_mgr_set_buffer_for_input(const char* objectName, uint8_t* buffer, uint32_t len);

/*****************************************************************************************************************/

/**
   External function for getting the buffer of an input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] buffer     Buffer set for this object.
   @param[out] len        Length of the buffer.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_buffer_of_input(const char* objectName, const uint8_t** buffer, uint32_t* len);

/*****************************************************************************************************************/

/**
   External function for setting the show flag of some object in the layout.
         
   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] show        The show flag to be set.
   
   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_show_flag(const char* objectName, uint32_t show);

/*****************************************************************************************************************/
/**
   External function for getting the show flag value of some object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] show       The show flag.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_show_flag(const char* objectName, uint32_t* show);

/*****************************************************************************************************************/


/**
   External function for setting a focused input object in the layout.

   @param[in] objectName  Object name (as a unique identifier in the layout).

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
   @newpage
*/
layout_mgr_err_t layout_mgr_set_focus_input(const char* objectName);

/*****************************************************************************************************************/
/**
   External function for setting the image buffer and the alignment of some image 
   object in the layout.

   @param[in] objectName   Object name (as a unique identifier in the layout).
   @param[in] imageBuffer  Buffer that contains the PNG representation of the
                           required image.
   @param[in] vAlign       Vertical alignment.
   @param[in] hAlign       Horizontal alignment. 

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_IMAGE_EXCEED_OBJECT \n
   #LAYOUT_MGR_INVALID_IMAGE \n
   #LAYOUT_MGR_UNSUPPORTED -- Unsupported parameter values. \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an image type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_image(const char* objectName, const uint8_t* imageBuffer, VerticalAlignment_t vAlign,HorizontalAlignment_t hAlign);

/*****************************************************************************************************************/

/**
   External function for setting the active keyboard in the layout.
   
   @param[in] keyboardName  Keyboard name to be set as active.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_active_keyboard(const char* keyboardName);

/*****************************************************************************************************************/

/**
   External function for keyboard randomization in the layout.

   @param[in] keyboardName  Keyboard name to be randomized.
   @param[in] isOneTime     Set to TRUE if randomization is to be one time
                            per device lifetime.

   @return 
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_FAILED_GEN_RAND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_randomize_keyboard(const char* keyboardName, uint8_t isOneTime);

/*****************************************************************************************************************/

/**
   External function for setting the length of the text label of some input
   object in the layout; length is to be less than or equal to the specified
   maximum length. If the length is longer, the text is truncated to this
   length. The maximum length is in characters.

   @param[in] objectName     Object name (as a unique identifier in the layout).
   @param[in] max_chars_len  Maximum length, in characters.
   
   @return 
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_truncate_input_string(const char* objectName, uint32_t max_chars_len);

/*****************************************************************************************************************/

/**
   External function for deleting the last character of the focused input object.
   If the focused input is empty, nothing should happen.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND -- In case of no focused input. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/

layout_mgr_err_t layout_mgr_delete_input_last_character(void);

/*****************************************************************************************************************/

/**
   External function for setting the buffer of an input object to zero.

   @param[in] objectName  Object name (as a unique identifier in the layout).

   @return 
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
   @newpage
*/
layout_mgr_err_t layout_mgr_clear_input_buffer(const char* objectName);

/*****************************************************************************************************************/

/**
   External function for setting the hidden character of an input object.
   In the case of DISP_MODE_HIDDEN (as declared in #LayoutInputDisplayMode_t),
   this character is displayed instead of the clear string characters.

   @param[in] objectName   Object name (as a unique identifier in the layout).
   @param[in] hidden_char  Hidden character in UTF-8 format.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_input_hidden_char(const char* objectName, utf8_char_t hidden_char);

/*****************************************************************************************************************/
/**
   External function for getting the hidden character of an input object.
   In the case of DISP_MODE_HIDDEN (as declared in #LayoutInputDisplayMode_t),
   this character is displayed instead of the clear string characters.

   @param[in] objectName    Object name (as a unique identifier in the layout).
   @param[out] hidden_char  Hidden character in UTF-8 format.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_input_hidden_char(const char* objectName, utf8_char_t* hidden_char);

/*****************************************************************************************************************/
/**
   External function for setting the display mode of some input object in the
   layout.
      
   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] mode        Display mode to be set.
   
   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_input_display_mode(const char* objectName, LayoutInputDisplayMode_t mode);

/*****************************************************************************************************************/
/**
   External function for getting the display mode of some input object in the
   layout.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] mode       Display mode that is set.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_input_display_mode(const char* objectName, LayoutInputDisplayMode_t* mode);

/*****************************************************************************************************************/
/**
   External function for getting the status of some input object in the
   layout; status is focused or not focused.

   @param[in] objectName     Object name (as a unique identifier in the layout).
   @param[out] input_status  Current status.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_input_status(const char* objectName, LayoutInputStatus_t* input_status);

/*****************************************************************************************************************/
/**
   External function for checking that some text string is not exceeding the
   object size.

   @param[in] objectName    Object name (as a unique identifier in the layout)
                            with which to check the text.
   @param[in] text          Buffer containing the UTF-8 representation of the
                            string to be checked.
   @param[in] len           Length of the string to be checked, in bytes.
   @param[out] fitting_len  Length of the substring that can fit the object,
                            in bytes.
   @param[out] char_length  Length of the substring that can fit the object,
                            in characters.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_FONT_MISSING_CHAR \n
   #LAYOUT_MGR_GENERAL_ERROR \n
   #LAYOUT_MGR_INVALID_UTF8_STR \n
   #LAYOUT_MGR_DYNAMIC_FONT_ERROR

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_check_exceeding_text(const char* objectName, const uint8_t* text, uint32_t len, uint32_t* fitting_len, uint32_t* char_length);

/*****************************************************************************************************************/
/**
   External function for setting some text string to some object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] text        Buffer containing the UTF-8 representation of the
                          string to be set.
   @param[in] len         Length of the string to be set, in bytes.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_FONT_MISSING_CHAR \n
   #LAYOUT_MGR_GENERAL_ERROR \n
   #LAYOUT_MGR_INVALID_UTF8_STR \n
   #LAYOUT_MGR_DYNAMIC_FONT_ERROR

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text(const char* objectName, const uint8_t* text, uint32_t len);

/*****************************************************************************************************************/
/**
   External function for getting the text string of some object.

   @param[in] objectName    Object name (as a unique identifier in the layout).
   @param[out] text         Buffer containing the UTF-8 representation of the
                            string.
   @param[out] len          Length of the string, in bytes.
   @param[out] char_length  Length of the substring that can fit the object,
                            in characters.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text(const char* objectName, const uint8_t** text, uint32_t* len, uint32_t* char_length);

/*****************************************************************************************************************/
/**
   External function for setting the text font of some object.
   
   @note1hang This function is not supported for input type objects.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] font_path   Absolute path to a font file.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_FONT_MISSING_CHAR \n
   #LAYOUT_MGR_DYNAMIC_FONT_ERROR

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_font(const char* objectName, const char* font_path);

/*****************************************************************************************************************/
/**
   External function for getting the path for the dynamic font of some object.
   This function should return NULL for input type objects.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] font_path  Font path ASCII string.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_font(const char* objectName, const char** font_path);

/*****************************************************************************************************************/
/**
   External function for setting the text color of some object in the layout.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] color       Color to be set.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_color(const char* objectName, Color_t color);

/*****************************************************************************************************************/
/**
   External function for getting the text color of some object in the layout.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] color      Color that is set.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_color(const char* objectName, Color_t* color);

/*****************************************************************************************************************/
/**
   External function for setting the text alignment, horizontal and vertical,
   of some object in the layout.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] v_align     Vertical alignment.
   @param[in] h_align     Horizontal alignment.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_alignment(const char* objectName, VerticalAlignment_t v_align, HorizontalAlignment_t h_align);

/*****************************************************************************************************************/
/**
   External function for getting the text alignment, horizontal and vertical,
   of some object in the layout.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] v_align    Vertical alignment.
   @param[out] h_align    Horizontal alignment.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_alignment(const char* objectName, VerticalAlignment_t* v_align, HorizontalAlignment_t* h_align);

/*****************************************************************************************************************/
/**
   External function for setting the flag to allow line breaking of text of
   some object.


   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] allow_line_breaking  TRUE: allow; FALSE: do not allow.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_line_breaking(const char* objectName, uint32_t allow_line_breaking);

/*****************************************************************************************************************/
/**
   External function for getting the flag to allow line breaking of text of
   some object.


   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] allow_line_breaking  Allow or do not allow.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_line_breaking(const char* objectName, uint32_t* allow_line_breaking);


/*****************************************************************************************************************/
/**
   External function for checking that some text string is not exceeding the
   object size; used for hint label of an input object.

   @param[in] objectName    Object name (as unique identifier in the layout)
                            with which to check the text.
   @param[in] text          Buffer containing the UTF-8 representation of the
                            string to be checked.
   @param[in] len           Length of the string to be checked, in bytes.
   @param[out] fitting_len  Length of the substring that can fit the object,
                            in bytes.
   @param[out] char_length  Length of the substring that can fit the object,
                            in characters.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_FONT_MISSING_CHAR \n
   #LAYOUT_MGR_GENERAL_ERROR \n
   #LAYOUT_MGR_INVALID_UTF8_STR \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type. \n
   #LAYOUT_MGR_DYNAMIC_FONT_ERROR

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_check_exceeding_text_hint(const char* objectName, const uint8_t* text, uint32_t len, uint32_t* fitting_len, uint32_t* char_length);

/*****************************************************************************************************************/
/**
   External function for setting some text string; used for hint label of an
   input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] text        Buffer containing the UTF-8 representation of the
                          string to be set.
   @param[in] len         Length of the string to be set, in bytes.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_FONT_MISSING_CHAR \n
   #LAYOUT_MGR_GENERAL_ERROR \n
   #LAYOUT_MGR_INVALID_UTF8_STR \n
   #LAYOUT_MGR_DYNAMIC_FONT_ERROR \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_hint(const char* objectName, const uint8_t* text, uint32_t len);

/*****************************************************************************************************************/
/**
   External function for getting the text string; used for hint label of an
   input object.

   @param[in] objectName    Object name (as a unique identifier in the layout).
   @param[out] text         Buffer containing the UTF-8 representation of the
                            string.
   @param[out] len          Length of the string, in bytes.
   @param[out] char_length  Length of the substring that can fit the object,
                            in characters.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
   @newpage
*/
layout_mgr_err_t layout_mgr_get_text_hint(const char* objectName, const uint8_t** text, uint32_t* len, uint32_t* char_length);


/*****************************************************************************************************************/
/**
   External function for setting the text font; used for hint label of an
   input object.
   
   @note1hang This function is not supported for input type objects.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] font_path   Font path ASCII string.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_FONT_MISSING_CHAR \n
   #LAYOUT_MGR_DYNAMIC_FONT_ERROR \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_font_hint(const char* objectName, const char* font_path);

/*****************************************************************************************************************/
/**
   External function for getting the path for the dynamic font; used for
   hint label of an input object.
   This function should return NULL for input type objects.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] font_path  Font path ASCII string.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_font_hint(const char* objectName, const char** font_path);

/*****************************************************************************************************************/
/**
   External function for setting the text color; used for hint label of an
   input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] color       Color to be set.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_color_hint(const char* objectName, Color_t color);

/*****************************************************************************************************************/
/**
   External function for getting the text color; used for hint label of an
   input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] color      Color that is set.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_color_hint(const char* objectName, Color_t* color);

/*****************************************************************************************************************/
/**
   External function for setting the text alignment, horizontal and vertical;
   used for hint label of an input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] v_align     Vertical alignment.
   @param[in] h_align     Horizontal alignment.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_alignment_hint(const char* objectName, VerticalAlignment_t v_align, HorizontalAlignment_t h_align);

/*****************************************************************************************************************/
/**
   External function for getting the text alignment, horizontal and vertical;
   used for hint label of an input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] v_align    Vertical alignment.
   @param[out] h_align    Horizontal alignment.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_alignment_hint(const char* objectName, VerticalAlignment_t* v_align, HorizontalAlignment_t* h_align);

/*****************************************************************************************************************/
/**
   External function for setting the flag to allow line breaking of text;
   used for hint label of an input object.


   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] allow_line_breaking  TRUE: allow; FALSE: do not allow.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_TEXT_EXCEED_OBJECT \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_text_line_breaking_hint(const char* objectName, uint32_t allow_line_breaking);

/*****************************************************************************************************************/
/**
   External function for getting the flag to allow line breaking of text;
   used for hint label of an input object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] allow_line_breaking  Allow or do not allow.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_UNSUPPORTED \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an input type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_text_line_breaking_hint(const char* objectName, uint32_t* allow_line_breaking);

/*****************************************************************************************************************/
/**
   External function for getting the layout flavor: pin, login, msg, or custom.

   @param[out] flavor  Layout flavor.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_layout_flavor(LayoutFlavor_t* flavor);

/*****************************************************************************************************************/
/**
   External function for getting the layout size in pixels.

   @param[out] layout_height  Layout height, in pixels.
   @param[out] layout_width   Layout width, in pixels.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_layout_size(uint32_t* layout_height, uint32_t* layout_width);

/*****************************************************************************************************************/
/**
   External function for getting the currently displayed keyboard.

   @param[out] keyboardName  Keyboard name.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_active_keyboard(const char** keyboardName);

/*****************************************************************************************************************/
/**
   External function for getting the currently selected input object.

   @param[out] focused_input_name  Input object name.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_focus_input(const char** focused_input_name);

/*****************************************************************************************************************/
/**
   External function for getting the size, in pixels, of some object.

   @param[in] objectName      Object name (as a unique identifier in the layout).
   @param[out] object_height  Object height, in pixels.
   @param[out] object_width   Object width, in pixels.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_object_size(const char* objectName, uint32_t* object_height, uint32_t* object_width);

/*****************************************************************************************************************/
/**
   External function for getting the type of some button object: data or function.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] type       Button type.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_button_type(const char* objectName, LayoutButtonType_t* type);

/*****************************************************************************************************************/
/**
   External function for getting the function of some function button object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] function   Button function.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type. \n
   #LAYOUT_MGR_UNSUPPORTED -- Button is not a function type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_button_function(const char* objectName, LayoutButtonFunctionType_t* function);

/*****************************************************************************************************************/
/**
   External function for getting the data of some data button object.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] data       Button data; UTF-8 encoded.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type. \n
   #LAYOUT_MGR_UNSUPPORTED -- Button is not a data type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_button_data(const char* objectName, utf8_char_t* data);

/*****************************************************************************************************************/
/**
   External function for setting the state of some button object:
   released, pressed, or disabled.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[in] state       Button state.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type. \n
   #LAYOUT_MGR_UNSUPPORTED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_set_button_state(const char* objectName, LayoutButtonStatus_t state);

/*****************************************************************************************************************/
/**
   External function for getting the state of some button object:
   released, pressed, or disabled.

   @param[in] objectName  Object name (as a unique identifier in the layout).
   @param[out] state      Button state.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not a button type.

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_button_state(const char* objectName, LayoutButtonStatus_t* state);

/*****************************************************************************************************************/
/**
   External function for getting the image buffer, size, and alignment of some
   image object in the layout.

   @param[in] objectName     Object name (as a unique identifier in the layout).
   @param[out] image_buffer  Buffer that contains the PNG representation of the
                             required image.
   @param[out] v_align       Vertical alignment.
   @param[out] h_align       Horizontal alignment.
   @param[out] image_height  Image height, in pixels.
   @param[out] image_width   Image width, in pixels.

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT \n
   #LAYOUT_MGR_OBJECT_NOT_FOUND \n
   #LAYOUT_MGR_OBJ_TYPE_NOT_SUPPORTED -- Object is not an image type. \n
   #LAYOUT_MGR_LAYOUT_NOT_LOADED

   @dependencies
   layout_mgr_load_layout() must be called before calling this function.
*/
layout_mgr_err_t layout_mgr_get_image(const char* objectName, const uint8_t** image_buffer,
														VerticalAlignment_t* v_align,HorizontalAlignment_t* h_align,
														uint32_t* image_height, uint32_t* image_width);


/*****************************************************************************************************************/
/**
   External function for setting the default timeout of the touch session
   (in milliseconds), i.e., the maximum time to wait for user input. 0 means 
   return immediately, and -1 means no timeout (wait indefinitely).

   @param[in] timeout  Default timeout to be returned in layout_mgr_eval_event()
                       in case the Layout Manager itself does not request a
                       different timeout for operating the common behavior of
                       the layout. (If not applicable, use NO_TIMEOUT.)

   @return
   #LAYOUT_MGR_SUCCESS

   @dependencies
   None.
*/
layout_mgr_err_t layout_mgr_set_default_timeout(int32_t timeout);

/*****************************************************************************************************************/
/**
   External function for getting the default timeout of the touch session
   (in milliseconds), i.e., the maximum time to wait for user input. 0 means
   return immediately, and -1 means no timeout (wait indefinitely).

   @param[out] timeout  Default timeout to be returned in layout_mgr_eval_event()
                        in case the Layout Manager itself does not request a
                        different timeout for operating the common behavior of
                        the layout. (If not applicable, use NO_TIMEOUT.)

   @return
   #LAYOUT_MGR_SUCCESS \n
   #LAYOUT_MGR_GOT_NULL_INPUT

   @dependencies
   None.
*/
layout_mgr_err_t layout_mgr_get_default_timeout(int32_t* timeout);

/*****************************************************************************************************************/

/**
   Cleans up the allocated buffers.

   @dependencies
   None.
*/
void free_layout_buffers();

/** @} */ /* end_addtogroup layout_manager */

#endif /* LAYOUT_MGR_H_ */

