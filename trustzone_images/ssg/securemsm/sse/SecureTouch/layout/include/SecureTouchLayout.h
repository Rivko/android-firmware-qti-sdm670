/** @file SecureTouchLayout.h
*/

/*
 * Copyright(c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/16    gs      Added support for versioning in layout struct
04/11/16   sn      const fixes.
12/10/15   kl      (Tech Pubs) Comment updates for 80-NJ546-1 Rev. J.
===========================================================================*/


#pragma once

#include <stdint.h>

/** @cond */

#define UTF8_MAX_CHAR_LEN               6
#define MAX_KEYBOARD_NAME_LEN           20
#define MAX_LAYOUT_OBJECTS_IN_PAGE      100
#define MAX_LAYOUT_OBJECTS_IN_KEYBOARD  48
#define MAX_KEYBOARD_LAYOUTS            15
#define KEYBOARD_INVALID                0xFFFFFFFF
#define OBJ_NAME_MAX_LEN                20
#define MAX_CORES (MAX_LAYOUT_OBJECTS_IN_PAGE + MAX_LAYOUT_OBJECTS_IN_KEYBOARD*MAX_KEYBOARD_LAYOUTS + 1)

#ifdef __RVCT__
  #pragma anon_unions
#endif

typedef enum {
  LAYOUT_ALIGN_TOP      = 0,
  LAYOUT_ALIGN_MID      = 1,
  LAYOUT_ALIGN_BTM      = 2,
  LAYOUT_ALIGN_V_LAST   = LAYOUT_ALIGN_BTM,

  LAYOUT_ALIGN_V_MAX    = 0x7FFFFFFF,
} VerticalAlignment_t;

typedef enum {
  LAYOUT_ALIGN_LEFT     = 0,
  LAYOUT_ALIGN_CENTER   = 1,
  LAYOUT_ALIGN_RIGHT    = 2,
  LAYOUT_ALIGN_H_LAST   = LAYOUT_ALIGN_RIGHT,

  LAYOUT_ALIGN_H_MAX    = 0x7FFFFFFF,
} HorizontalAlignment_t;

typedef enum {
  LAYOUT_ELEMENT_TYPE_KEYBOARD = 0,
  LAYOUT_ELEMENT_TYPE_OBJECT   = 1,
  LAYOUT_ELEMENT_TYPE_PAGE     = 2,
  LAYOUT_ELEMENT_TYPE_MAX      = 0x7FFFFFFF,
} LayoutElementType_t;

typedef enum {
  LAYOUT_OBJECT_TYPE_UNUSED  = 0,
  LAYOUT_OBJECT_TYPE_INPUT   = 1,
  LAYOUT_OBJECT_TYPE_BUTTON  = 2,
  LAYOUT_OBJECT_TYPE_TEXT    = 3,
  LAYOUT_OBJECT_TYPE_IMAGE   = 4,
  LAYOUT_OBJECT_TYPE_LAST    = LAYOUT_OBJECT_TYPE_IMAGE,

  LAYOUT_OBJECT_TYPE_OTHER   = 0x800,
  LAYOUT_OBJECT_TYPE_MAX     = 0x7FFFFFFF
} LayoutObjectType_t;

typedef enum {
  TEXT_SHOW_ALL         = 0,
  TEXT_HIDE_ALL         = 1,
  TEXT_SHOW_ONLY_LAST   = 2,

  TEXT_VISABLE_MAX      = 0x7FFFFFFF,
} TextVisiblityOptions_t;

typedef enum {
  LAYOUT_BUTTON_TYPE_DATA      = 0,
  LAYOUT_BUTTON_TYPE_FUNCTION  = 1,
  LAYOUT_BUTTON_TYPE_MAX       = 0x7FFFFFFF,
} LayoutButtonType_t;

typedef enum {
  LAYOUT_BUTTON_STATUS_RELEASED = 0,
  LAYOUT_BUTTON_STATUS_PRESSED  = 1,
  LAYOUT_BUTTON_STATUS_DISABLED = 2,
  LAYOUT_BUTTON_STATUS_LAST     = LAYOUT_BUTTON_STATUS_DISABLED,
  LAYOUT_BUTTON_STATUS_MAX      = 0x7FFFFFFF,
} LayoutButtonStatus_t;

typedef enum {
  LAYOUT_INPUT_STATUS_UNSELECTED = 0,
  LAYOUT_INPUT_STATUS_SELECTED   = 1,
  LAYOUT_INPUT_STATUS_MAX      = 0x7FFFFFFF,
} LayoutInputStatus_t;

typedef enum {
  LAYOUT_BUTTON_FUNCTION_FIRST     = 0,
  LAYOUT_BUTTON_FUNCTION_ENTER     = LAYOUT_BUTTON_FUNCTION_FIRST,
  LAYOUT_BUTTON_FUNCTION_CANCEL    = 1,
  LAYOUT_BUTTON_FUNCTION_BACKSPACE = 2,
  LAYOUT_BUTTON_FUNCTION_CLEAR     = 3,
  LAYOUT_BUTTON_FUNCTION_SHIFT     = 4,
  LAYOUT_BUTTON_FUNCTION_SYM       = 5, // attribute: index of the pointed keyboard
  LAYOUT_BUTTON_FUNCTION_F1        = 6,
  LAYOUT_BUTTON_FUNCTION_F2        = 7,
  LAYOUT_BUTTON_FUNCTION_F3        = 8,
  LAYOUT_BUTTON_FUNCTION_LAST      = LAYOUT_BUTTON_FUNCTION_F3,
  LAYOUT_BUTTON_FUNCTION_MAX       = 0x7FFFFFFF,
} LayoutButtonFunctionType_t;

/** @endcond */

/** @addtogroup qsee_gp_screens
@{ */

/** Display mode of the text in an input object on the screen. */
typedef enum {
  DISP_MODE_VISIBLE      = 0,  /**< Text is always visible to the user. */
  DISP_MODE_HIDDEN       = 1,  /**< Text is always hidden; some other characters
                                    (for example, asterisks) are displayed instead of
                                    the actual characters. */
  DISP_MODE_VIS_THEN_HID = 2,  /**< Text is hidden and only the most recently
                                    typed character is visible for a short time. */
  DISP_MODE_LAST         = DISP_MODE_VIS_THEN_HID,
  DISP_MODE_MAX          = 0x7FFFFFFF,
} LayoutInputDisplayMode_t;

/** @} */ /* end_addtogroup qsee_gp_screens */

/** @cond */

typedef enum {
  LAYOUT_KEYBOARD_SHIFT_OFF     = 0,
  LAYOUT_KEYBOARD_SHIFT_ONCE    = 1,
  LAYOUT_KEYBOARD_SHIFT_CAPS    = 2,
  LAYOUT_KEYBOARD_SHIFT_MAX     = 0x7FFFFFFF,
} LayoutKeyboardShiftStatus_t;

typedef enum {
  LAYOUT_OBJECT_RENDER_MODE_COLORS      = 0,
  LAYOUT_OBJECT_RENDER_MODE_BG_IMAGE    = 1,
  LAYOUT_OBJECT_RENDER_MODE_MAX         = 0x7FFFFFFF,
} LayoutObjectRenderMode_t;

typedef struct __attribute__((packed)){
  uint8_t enc_bytes[UTF8_MAX_CHAR_LEN]; //6
} utf8_char_t;

typedef struct __attribute__((packed)){
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
} Rect_t;

typedef struct __attribute__((packed)) {
  uint16_t x;
  uint16_t y;
} Point_t;

typedef struct {
  TextVisiblityOptions_t visible;
  utf8_char_t hiddenChar;
}TextVisiblity_t;


typedef struct  __attribute__((packed)){
  uint8_t alpha;
  uint8_t blue;
  uint8_t green;
  uint8_t red;
} RGBAColor_t;

typedef enum{
  COLOR_FORMAT_RGBA = 0,

  COLOR_FORMAT_MAX  = 0x7FFFFFFF
} ColorFormat_t;

typedef struct{
  ColorFormat_t format;
  union {
    RGBAColor_t rgba_color;
  };
} Color_t;

typedef struct {
  void* font;
  const char* fontPath;
  Color_t color;
  VerticalAlignment_t v_align;
  HorizontalAlignment_t h_align;
  uint8_t * text;
  uint32_t max_buff_size; //in bytes
  uint32_t curr_len; //in utf8 characters
  TextVisiblity_t visibility;
  uint32_t bAllowLineBreaking;
} Text_t;

typedef struct {
  uint32_t changed;
  uint32_t visible;
  uint32_t index;
  LayoutElementType_t type;
} LayoutElementCore_t;

typedef struct __attribute__((packed)){
  utf8_char_t lower;
  uint16_t lowerLength;
  utf8_char_t upper;
  uint16_t upperLength;
} LayoutButtonData_t;

typedef struct {
  LayoutButtonFunctionType_t type;
  uint32_t data; // ancillary data for the function
} LayoutButtonFunction_t;

typedef struct {
  Color_t frameColor;
  uint32_t frameWidth;
} Frame_t;

typedef struct {
  Color_t fgColor;
  Color_t bgColor;
  Frame_t frame;
} ButtonColor_t;

typedef struct {
  VerticalAlignment_t v_align;
  HorizontalAlignment_t h_align;
  uint8_t * image_buffer;
} LayoutObjectImage_t;

typedef struct {
  LayoutObjectRenderMode_t renderMode;
  ButtonColor_t stateColor[LAYOUT_BUTTON_STATUS_LAST + 1];
  LayoutObjectImage_t stateBgImage[LAYOUT_BUTTON_STATUS_LAST + 1];
  LayoutButtonType_t type;
  LayoutButtonStatus_t status;
  union {
    LayoutButtonData_t data;
    LayoutButtonFunction_t function;
  };
} LayoutObjectButton_t;

typedef struct {
  LayoutObjectRenderMode_t renderMode;
  uint32_t index;
  LayoutInputStatus_t status;
  LayoutInputDisplayMode_t displayMode;
  Text_t default_label;
  union{
    Frame_t focusFrame;
    LayoutObjectImage_t focusImage;
  };
  union{
   Frame_t normalFrame;
   LayoutObjectImage_t normalImage;
  };
} LayoutObjectInput_t;

typedef struct {
  char name[OBJ_NAME_MAX_LEN];
  Color_t bgColor;
  Frame_t frame;
  LayoutObjectImage_t bgImage;
  LayoutElementCore_t core;
  LayoutObjectType_t type;
  Rect_t rect;
  uint8_t dirty;
  uint8_t show;
  Text_t label;
  union {
    LayoutObjectButton_t button;
    LayoutObjectInput_t input;
  };
} LayoutObject_t;

typedef struct {
  LayoutElementCore_t core;
  LayoutKeyboardShiftStatus_t shift_status;
  uint32_t number; // number of objects in the keyboard
  Point_t origin;
  char name[MAX_KEYBOARD_NAME_LEN];
  uint32_t nameLength;
  LayoutObject_t objects[MAX_LAYOUT_OBJECTS_IN_KEYBOARD];
} LayoutKeyboard_t;

typedef enum {
  LAYOUT_FLAVOR_TYPE_CUSTOM               = 0,
  LAYOUT_FLAVOR_TYPE_PIN                  = 1,
  LAYOUT_FLAVOR_TYPE_MESSAGE_SINGLE       = 2,
  LAYOUT_FLAVOR_TYPE_MESSAGE_DOUBLE       = 3,
  LAYOUT_FLAVOR_TYPE_MESSAGE_TRIPLE       = 4,
  LAYOUT_FLAVOR_TYPE_lOGIN_SINGLE_INPUT       = 5,
  LAYOUT_FLAVOR_TYPE_lOGIN_DOUBLE_INPUT         = 6,
  LAYOUT_FLAVOR_TYPE_MAX                  = 0x7FFFFFFF,
} LayoutFlavor_t;

typedef struct {
  LayoutElementCore_t core;
  uint32_t number; // number of objects exluding the keyboard
  uint32_t focused_input; // index of the current focused input object
  uint32_t numberKeyboards; // number of keyboards
  uint32_t activeKeyboard;
  LayoutObject_t objects[MAX_LAYOUT_OBJECTS_IN_PAGE];
  LayoutKeyboard_t keyboards[MAX_KEYBOARD_LAYOUTS];
  uint8_t dirtyKeyboard;

  LayoutObject_t keyboardBg;
  LayoutObject_t layoutBg;

  uint8_t renderAll;
  LayoutFlavor_t flavor;
  uint32_t major_version;
  uint32_t minor_version;
} LayoutPage_t;

typedef struct {
  uint32_t number;
  LayoutElementCore_t *core[MAX_CORES];
} LayoutCores_t;

/** @endcond */
