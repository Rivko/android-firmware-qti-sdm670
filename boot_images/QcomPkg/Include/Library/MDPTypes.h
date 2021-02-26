#ifndef __MDPTYPES_H__
#define __MDPTYPES_H__
/*=============================================================================
 
  File: MDPTypes.h
 
  Header file for common driver types
  
 
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include "hal_mdss.h"

/*===========================================================================

                                 Defines 

===========================================================================*/

#ifndef _BOOL32_DEFINED 
typedef  unsigned long int  bool32;        /* Boolean value type. */ 
#define _BOOL32_DEFINED 
#endif    


/*===========================================================================

                                Helper Macros

===========================================================================*/

/*===========================================================================

                                Enumeration

===========================================================================*/

/* Supported MDP Pixel formats 
 */
typedef enum
{
  MDP_PIXEL_FORMAT_NONE = 0,
  MDP_PIXEL_FORMAT_RGB_565_16BPP,
  MDP_PIXEL_FORMAT_RGB_666_18BPP,
  MDP_PIXEL_FORMAT_RGB_888_24BPP,
  MDP_PIXEL_FORMAT_ARGB_1555_16BPP,
  MDP_PIXEL_FORMAT_XRGB_8888_32BPP,
  MDP_PIXEL_FORMAT_ARGB_8888_32BPP,
  MDP_PIXEL_FORMAT_BGR_565_16BPP,
  MDP_PIXEL_FORMAT_BGR_888_24BPP,
  MDP_PIXEL_FORMAT_ABGR_1555_16BPP,
  MDP_PIXEL_FORMAT_XBGR_8888_32BPP,
  MDP_PIXEL_FORMAT_ABGR_8888_32BPP,
  MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP,          /**< 4:2:0, Pseudo planar, Cb(U) in MSB, viewing in bytes in chroma plane, it is CrCbCrCbCrCb..., same as NV21*/
  MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP,          /**< 4:2:0, Pseudo planar, Cr(V) in MSB, viewing in bytes in chroma plane, it is CbCrCbCrCbCr..., same as NV12*/
  MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP,          /**< 4:2:2, YCrYCb(YVYU) interleave */
  MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP,          /**< 4:2:2, YCbYCr(YUYV) interleave */
  MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP,          /**< 4:2:2, CrYCbY(VYUY) interleave */
  MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP,          /**< 4:2:2, CbYCrY(UYVY) interleave */
  MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP,          /**< 4:2:2, Pseduo planar, Cr(U) in MSB */
  MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP,          /**< 4:2:2, Pseudo planar, Cb(V) in MSB */
  MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP,      /**< Identical packing to Y_CBCR_H2V2_12BPP */
  MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP,      /**< Identical packing to Y_CRCB_H2V2_12BPP */
  MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP,         /**< 4:2:0, planar, Y,CR(V),Cb(U) in seperate plane */
  MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_16BPP,         /**< 4:2:2, planar, Y,CR(V),Cb(U) in seperate plane */
  MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP, /**< 4:2:0, Pseudo planar, Cr(V), Cb(U) are in 4x2 tile format*/
  MDP_PIXEL_FORMAT_RGBA_5551_16BPP,
  MDP_PIXEL_FORMAT_RGBA_8888_32BPP,
  MDP_PIXEL_FORMAT_MAX,
  MDP_PIXEL_FORMAT_FORCE_32BIT = 0x7FFFFFFF
} MDP_PixelFormatType;



typedef enum
{
  MDP_ROTATE_NONE,      /**< Do not rotate. */
  MDP_ROTATE_90,        /**< Rotate clockwise 90 degrees. */
  MDP_ROTATE_180,       /**< Rotate clockwise 180 degrees. */
  MDP_ROTATE_270,       /**< Rotate clockwise 270 degrees. */
  MDP_ROTATE_NONE_FLIP_HORZ, /**< Rotate 0 degrees, flip horizontally. */
  MDP_ROTATE_NONE_FLIP_VERT, /**< Rotate 0 degrees, flip vertically. */
  MDP_ROTATE_90_FLIP_HORZ,   /**< Rotate 90 degrees, flip horizontally. */
  MDP_ROTATE_90_FLIP_VERT,   /**< Rotate 90 degrees, flip vertically. */
  MDP_ROTATE_180_FLIP_HORZ,  /**< Rotate 180 degrees, flip horizontally. */
  MDP_ROTATE_180_FLIP_VERT,  /**< Rotate 180 degrees, flip vertically. */
  MDP_ROTATE_270_FLIP_HORZ,  /**< Rotate 270 degrees, flip horizontally. */
  MDP_ROTATE_270_FLIP_VERT,  /**< Rotate 270 degrees, flip vertically. */
  MDP_ROTATE_MAX,       /**< Maximum number of rotating modes. */
  MDP_ROTATE_FORCE_32BIT = 0x7FFFFFFF
} MDP_RotateFlipType;



/* MDP Status types
 */
typedef enum
{
  MDP_STATUS_OK            = 0,        /* Command successful */
  MDP_STATUS_BAD_HANDLE,               /* Bad Destination Handle */
  MDP_STATUS_BAD_PARAM,                /* Bad Parameter */
  MDP_STATUS_NOT_SUPPORTED,            /* Operation not supported */
  MDP_STATUS_NO_RESOURCES,             /* Out of resources */
  MDP_STATUS_FAILED,                   /* Command Failed */
  MDP_STATUS_TIMEOUT,                  /* Command not completed within expected time */
  MDP_STATUS_HW_ERROR,                 /* Hardware isn't responding as expected */
  MDP_STATUS_INVALID_STATE,            /* Not in correct state to perform task */
  MDP_STATUS_BUFFER_TOO_SMALL,         /* A function needs a larger buffer for the amount of data*/  
  MDP_STATUS_MAX,
  MDP_STATUS_FORCE_32BIT = 0x7FFFFFFF
} MDP_Status;


/* MDP Display interface
 */
typedef enum
{
  MDP_DISPLAY_PRIMARY,                 /* Primary display */
  MDP_DISPLAY_SECONDARY,               /* Secondary display */
  MDP_DISPLAY_EXTERNAL,                /* External display */
  MDP_DISPLAY_MAX,
  MDP_DISPLAY_FORCE_32BIT = 0x7FFFFFFF
} MDP_Display_IDType;

/*!
 * \enum MDP_DP_LinkTrainingType
 *
 * The \b MDP_DP_LinkTrainingType defines the types of Link Training.
 *
 */
typedef enum
{
   MDP_DP_LINK_TRAINING_TYPE_NO_TRAINING          = 0x00000000,        /**< No Link Training                        */
   MDP_DP_LINK_TRAINING_TYPE_FULL_TRAINING,                            /**< Full Link Training                      */
   MDP_DP_LINK_TRAINING_TYPE_FAST_TRAINING,                            /**< Fast Link Training                      */
   MDP_DP_LINK_TRAINING_TYPE_MAX,
   MDP_DP_LINK_TRAINING_TYPE_FORCE_32BIT          = 0x7FFFFFFF
}MDP_DP_LinkTrainingType;

/*!
 * \enum MDP_Lock_Type
 *
 * Software locks to prevent re-entrancy of various code sections.
 *
 */
typedef enum {
  MDP_LOCK_TYPE_DISPLAY_PWR_CTRL = 0,         /**< Display power control lock */
  MDP_LOCK_TYPE_MAX,
  MDP_LOCK_TYPE_FORCE_32BIT      = 0x7FFFFFFF
} MDP_Lock_Type;

/** Defines the OEM backlight configuration options.
*/
typedef enum
{
    MDP_PANEL_BACKLIGHTTYPE_NONE = 0, /**< No backlight adjustment is available. */
    MDP_PANEL_BACKLIGHTTYPE_PMIC,     /**< Backlight is controlled through PMIC PWM or WLED.  For OLED, PMIC uses SWIRE to control brightness */
    MDP_PANEL_BACKLIGHTTYPE_I2C,      /**< Backlight is controlled through I2C. */
    MDP_PANEL_BACKLIGHTTYPE_DSI,      /**< Backlight is controlled through the DSI command. */
    MDP_PANEL_BACKLIGHTTYPE_ACPI,     /**< Backlight is controlled through the ACPI method. */
    MDP_PANEL_BACKLIGHTTYPE_MAX,      /**< Enumeration marker for the max. */
    MDP_PANEL_BACKLIGHTTYPE_FORCE_32BIT = 0x7FFFFFFF /**< Forces the enumeration to 32 bits. */
} MDP_Panel_BacklightType;


/** PMIC module for secondary power sources
*/
typedef enum
{
    MDP_PMIC_MODULE_CONTROLTYPE_NONE = 0,
    /**< No secondary power control method */

    MDP_PMIC_MODULE_CONTROLTYPE_IBB_LAB_LCD,
    /**< Secondary power through IBB/LAB LCD mode. */

    MDP_PMIC_MODULE_CONTROLTYPE_IBB_LAB_OLED,
    /**< OLED power supply using IBB/LAB OLED mode */

    MDP_PMIC_MODULE_CONTROLTYPE_PM_LCD_WLED,
    /**< Secondary power through PMIC dedicated LCD/WLED module. */

    MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED,
    /**< OLED power supply using PMIC dedicated AMOLED modulee */

    MDP_PMIC_MODULE_CONTROLTYPE_MAX,
    /**< Enumeration marker for the max. */

    MDP_PMIC_MODULE_CONTROLTYPE_FORCE_32BIT = 0X7FFFFFFF
    /**< Forces the enumeration to 32 bits. */

} MDP_PmicModuleControlType;


/** PMIC control method for backlight control.
*/
typedef enum
{
    MDP_PMIC_BACKLIGHT_CONTROLTYPE_NONE = 0,
    /**< No backlight control method. */

    MDP_PMIC_BACKLIGHT_CONTROLTYPE_LPG,
    /**< Backlight controlled through LPG module. */

    MDP_PMIC_BACKLIGHT_CONTROLTYPE_WLED,
    /**< Backlight controlled through WLED module. */

    MDP_PMIC_BACKLIGHT_CONTROLTYPE_MAX,
    /**< Enumeration marker for the max. */

    MDP_PMIC_BACKLIGHT_CONTROLTYPE_FORCE_32BIT = 0X7FFFFFFF
    /**< Forces the enumeration to 32 bits. */

} MDP_PmicBacklightControlType;



/* MDP_RESET_STATE_TYPE
 * reset gpio pin polarity and state
 */
typedef enum
{
  MDP_RESET_STATE_LEVEL_LOW,
  MDP_RESET_STATE_LEVEL_HIGH,
  MDP_RESET_STATE_EDGE_LOW2HIGH,
  MDP_RESET_STATE_EDGE_HIGH2LOW,
  MDP_RESET_STATE_MAX,
} MDP_RESET_STATE_TYPE;

/*===========================================================================

                        Structures 

===========================================================================*/


/* MDP point descriptor
 */
typedef struct {
    uint32 uX;
    uint32 uY;
} MDPPointType;


/* Rectangle descriptor
 */
typedef struct {
    uint32 uX;
    uint32 uY;
    uint32 uWidth;
    uint32 uHeight;
} MDPRectType;

/*!
 * \b MDP_AddressType
 * 
 * Data structure holds the 64-bit address information
 */
typedef union 
{
  struct 
  {
      uint32 uLowPart;
      int32  iHighPart;
  } u;
  int64  iQuadPart;
} MDP_AddressType;


/* Frame buffer surface structure
 */
typedef struct {
    MDP_PixelFormatType  ePixelFormat;      /* Pixel format */
    uint32               uWidth;            /* Surface width */
    uint32               uHeight;           /* Surface height */
    void                *pPlane0Offset;     /* Plane 0 offset */
    uint32               uPlane0Stride;     /* Plane 0 stride */
    void                *pPlane1Offset;     /* Plane 1 offset */
    uint32               uPlane1Stride;     /* Plane 1 stride */
    void                *pPlane2Offset;     /* Plane 2 offset */  
    uint32               uPlane2Stride;     /* Plane 2 stride */
} MDPSurfaceInfo;


#endif // __MDPLIB_H__

