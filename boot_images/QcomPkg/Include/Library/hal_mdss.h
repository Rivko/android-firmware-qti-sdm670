/*
===========================================================================

FILE:         hal_mdss.h

DESCRIPTION:  
  This is the common MDSS header file for all shared data types between
  MDP, DSI, HDMI and DP

===========================================================================
                             File Info

===========================================================================
Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef _HAL_MDSS_H_
#define _HAL_MDSS_H_

/* -----------------------------------------------------------------------
** Shared Data Types
** ----------------------------------------------------------------------- */
#ifndef _UINT64_DEFINED
typedef unsigned long long  uint64;      /* Unsigned 64 bit value */
  #define _UINT64_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned  int      uint32;      /* Unsigned 32 bit value */
  #define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
  #define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
  #define _UINT8_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  signed long long   int64;       /* Signed 64 bit value */
  #define _INT64_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed  int        int32;       /* Signed 32 bit value */
  #define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
  #define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
  #define _INT8_DEFINED
#endif

#ifndef _BOOL32_DEFINED
typedef  unsigned long int  bool32;      /* Boolean value type. */
  #define _BOOL32_DEFINED
#endif

#ifndef _UINTPTR_DEFINED
typedef  UINTN              uintPtr;     /* Unsigned integer based on 32/64 bit architecture  */
#define _UINTPTR_DEFINED

#endif

#ifndef TRUE
  #define TRUE   1   /* Boolean true value. */
#endif

#ifndef FALSE
  #define FALSE  0   /* Boolean false value. */
#endif

#ifndef NULL
  #define NULL   0
#endif

/**
 * Generic HAL Helper Macros
 */
#define HAL_MDSS_BIT(x)                               ((uint32)(1<<(x)))
#define HAL_MDSS_ENABLE_REG_FIELD(val, mask, enable)  ((enable)?(val | mask):(val & ~mask))

/**
 * Convert a color from 8bits to 12 bits
 *
 * This simplified calculation is not perfect but approximates by 
 * just shift 4 bits to the left and repeat the most significant nibble
*/
#define HAL_MDSS_CONVERT_8_TO_12BITS(uColor)          (((uColor) << 4) | ((uColor) >> 4)) 

/*
 * HAL_MDSS_ErrorType
 *
 * Common HAL MDSS Error Codes (shared between MDP, DSI, HDMI and DP
 */
#define HAL_MDSS_COMMON_ERROR_BASE              0x00000000
#define HAL_MDSS_MDP_ERROR_BASE                 0x00001000
#define HAL_MDSS_DSI_ERROR_BASE                 0x00002000
#define HAL_MDSS_HDMI_ERROR_BASE                0x00003000
#define HAL_MDSS_DP_ERROR_BASE                  0x00004000 		//Applicable to both eDP and DP
#define HAL_MDSS_LVDS_ERROR_BASE                0x00005000

typedef enum _HAL_MDSS_ErrorType
{
   // Common MDSS Errors
   HAL_MDSS_STATUS_SUCCESS                               = HAL_MDSS_COMMON_ERROR_BASE,      // Success

   // Development/Diagnostic Errors
   HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED,                             // API is not supported
   HAL_MDSS_STATUS_FAILED_ASSERT,                                    // An Assertion was triggered
   HAL_MDSS_STATUS_FAILED_NOT_IMPLEMENTED,                           // API has not been implemented yet
   HAL_MDSS_STATUS_FAILED_UNSPECIFIED_ERROR,                         // API returned without success but error unkown
   HAL_MDSS_STATUS_FAILED_NO_HW_SUPPORT,                             // HW does not support what is requested
   HAL_MDSS_STATUS_FAILED_EXPECTED_NON_NULL_PTR,                     // Expected Non Null Pointer for a parameter
   HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER,                   // Invalud Input parameter
   HAL_MDSS_STATUS_FAILED_SW_DRIVER_DOES_NOT_MATCH_HW,               // Mismatch between SW and HW
   HAL_MDSS_STATUS_FAILED_HW_OPTION_OBSOLETE,                        // HW option is obsolete

   // MDP Errors
   HAL_MDSS_MDP_STATUS_FAILED                            = HAL_MDSS_MDP_ERROR_BASE,

   // DSI Errors
   HAL_MDSS_DSI_STATUS_FAILED                            = HAL_MDSS_DSI_ERROR_BASE,
   HAL_MDSS_DSI_FAILED_NO_HW_SUPPORT,
   HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER,
   HAL_MDSS_DSI_FAILED_NO_SUPPORT_OR_NO_HW,
   HAL_MDSS_DSI_FAILED_INTERRUPTS_UNMAPPED,
   HAL_MDSS_DSI_FAILED_EXPECTED_NON_NULL_PTR,
   HAL_MDSS_DSI_FAILED_FXN_NOT_SUPPORTED,
   HAL_MDSS_DSI_FAILED_PARAMETER_OUT_OF_RANGE,
   HAL_MDSS_DSI_FAILED_NUMBER_OF_TABLE_ENTRIES,
   HAL_MDSS_DSI_FAILED_ZERO_AREA_WINDOW_SPECIFIED,
   HAL_MDSS_DSI_FAILED_BAD_CONNECTION_PARAMETER,
   HAL_MDSS_DSI_FAILED_DISPLAY_FILL_NOT_SUPPORTED,
   HAL_MDSS_DSI_FAILED_NO_SUPPORT_FOR_COLOR_FORMAT,
   HAL_MDSS_DSI_FAILED_YSTRIDE_TOO_LARGE_FOR_HW,
   HAL_MDSS_DSI_FAILED_IMAGE_SIZE_TOO_BIG_FOR_HW,
   HAL_MDSS_DSI_FAILED_HW_IS_STILL_ACTIVE,
   HAL_MDSS_DSI_FAILED_SW_IMPLEMENTATION_ERR,
   HAL_MDSS_DSI_FAILED_ADDRESS_NOT_MEMORY_ALIGNED,
   HAL_MDSS_DSI_FAILED_CANNOT_CLEAR_UNMAPPED_INTERRUPTS,
   HAL_MDSS_DSI_FAILED_SW_DRIVER_DOES_NOT_MATCH_HW,
   HAL_MDSS_DSI_FAILED_HW_OPTION_OBSOLETE,
   HAL_MDSS_DSI_FAILED_UNABLE_TO_INIT_HW,
   HAL_MDSS_DSI_FAILED_PACKET_CORRUPTED,


   // HDMI Errors
   HAL_MDSS_HDMI_STATUS_FAILED                           = HAL_MDSS_HDMI_ERROR_BASE,

   // DP Errors
   HAL_MDSS_DP_STATUS_FAILED                             = HAL_MDSS_DP_ERROR_BASE,

   // LVDS Errors
   HAL_MDSS_LVDS_STATUS_FAILED                           = HAL_MDSS_LVDS_ERROR_BASE,

} HAL_MDSS_ErrorType;

typedef enum _HAL_MDSS_ModuleType
{
	HAL_MDSS_MODULE_NONE = 0x00000000,
	HAL_MDSS_MODULE_MDP = 0x00000001,
	HAL_MDSS_MODULE_DSI0 = 0x00000002,
	HAL_MDSS_MODULE_DSI1 = 0x00000004,
	HAL_MDSS_MODULE_HDMI = 0x00000008,
	HAL_MDSS_MODULE_eDP = 0x00000010,                      //DEPRECATED
	HAL_MDSS_MODULE_DP = 0x00000020,
	HAL_MDSS_MODULE_ROT = 0x00000040,
	HAL_MDSS_MODULE_FORCE_32BIT = 0x7FFFFFFF
} HAL_MDSS_ModuleType;

/*
*  HAL_MDP_PixelFormatType
*
* Supported display/lcd color specific formats.
*
* The supported formats and their associated bit packings
* are listed below.
* Legend: 
*
*
*                    33222222222211111111110000000000  Bit                 \n
*                    10987654321098765432109876543210  Location            \n
*                   |--------------------------------|                     \n
*  RGB_565          |                RRRRRGGGGGGBBBBB|                     \n
*                   |--------------------------------|                     \n
*  RGB_666          |              RRRRRRGGGGGGBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  RGB_888          |        RRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  ARGB_1555        |                ARRRRRGGGGGBBBBB|                     \n
*                   |--------------------------------|                     \n
*  XRGB_8888        |xxxxxxxxRRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  ARGB_8888        |AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  BGR_565          |                BBBBBGGGGGGRRRRR|                     \n
*                   |--------------------------------|                     \n
*  BGR_8888         |        BBBBBBBBGGGGGGGGRRRRRRRR|                     \n
*                   |--------------------------------|                     \n
*  ABGR_1555        |                ABBBBBGGGGGRRRRR|                     \n
*                   |--------------------------------|                     \n
*  XBGR_8888        |xxxxxxxxBBBBBBBBGGGGGGGGRRRRRRRR|                     \n
*                   |--------------------------------|                     \n
*  ABGR_8888        |AAAAAAAABBBBBBBBGGGGGGGGRRRRRRRR|                     \n
*                   |--------------------------------|                     \n
*  Y_CBCR_H2V2      |                        YYYYYYYY|  Plane0             \n
* (Y_CBCR_H2V2_VC1) |                bbbbbbbbrrrrrrrr|  Plane1             \n
*                   |--------------------------------|                     \n
*  Y_CRCB_H2V2      |                        YYYYYYYY|  Plane0             \n
* (Y_CRCB_H2V2_VC1) |                rrrrrrrrbbbbbbbb|  Plane1             \n
*                   |--------------------------------|                     \n
*  YCRYCB_H2V1      |YYYYYYYYrrrrrrrrYYYYYYYYbbbbbbbb|                     \n
*                   |--------------------------------|                     \n
*  Y_CRCB_H2V1      |                Y0Y0Y0Y0Y1Y1Y1Y1|  Plane0             \n
*                   |                rrrrrrrrbbbbbbbb|  Plane1             \n
*                   |--------------------------------|                     \n
*  Y_CBCR_H2V1      |                Y0Y0Y0Y0Y1Y1Y1Y1|  Plane0             \n
*                   |                bbbbbbbbrrrrrrrr|  Plane1             \n
*                   |--------------------------------|                     \n
*  Y_CR_CB_H2V2     |                Y0Y0Y0Y0Y1Y1Y1Y1|  Plane0             \n
*                   |                        rrrrrrrr|  Plane1             \n
*                   |                        bbbbbbbb|  Plane2             \n
*                   |--------------------------------|                     \n
*  CrYCb_H1V1       |        rrrrrrrrYYYYYYYYbbbbbbbb|                     \n
*                   |--------------------------------|                     \n
*                    33222222222211111111110000000000  Bit                 \n
*                    10987654321098765432109876543210  Location            \n
*
* NOTE: Y_CBCR_H2V2_VC1 and Y_CRCB_H2V2_VC1 formats are identical in
*       packing order to Y_CBCR_H2V2 and Y_CRCB_H2V2 (respectively).  The
*       color range for the VC1 formats are WMV9, the same as main
*       profile (in advanced mode: nRangeY=nRangeC=16).
*
*/
typedef enum _HAL_MDP_PixelFormatType
{
   HAL_MDP_PIXEL_FORMAT_NONE                             = 0x00000000,

   // Interleaved Pixel Formats
   HAL_MDP_PIXEL_FORMAT_INTERLEAVED_BASE,

   HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP                    = HAL_MDP_PIXEL_FORMAT_INTERLEAVED_BASE,
   HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP,
   HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP,
   HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP,
   HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP,
   HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP,
   HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP,
   HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP,
   HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP,
   HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP,
   HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP,
   HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP,
   HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP,
   HAL_MDP_PIXEL_FORMAT_RGB_OXILI_TILE_4x4_24BPP,
   HAL_MDP_PIXEL_FORMAT_ARGB_4444_16BPP,
   
   HAL_MDP_PIXEL_FORMAT_XRGB_2101010_32BPP,
   HAL_MDP_PIXEL_FORMAT_ARGB_2101010_32BPP,
   HAL_MDP_PIXEL_FORMAT_XBGR_2101010_32BPP,
   HAL_MDP_PIXEL_FORMAT_ABGR_2101010_32BPP,
   HAL_MDP_PIXEL_FORMAT_RGBX_1010102_32BPP,
   HAL_MDP_PIXEL_FORMAT_RGBA_1010102_32BPP,
   HAL_MDP_PIXEL_FORMAT_BGRX_1010102_32BPP,
   HAL_MDP_PIXEL_FORMAT_BGRA_1010102_32BPP,

   // YUV pixel formats
   HAL_MDP_PIXEL_FORMAT_YUV_BASE,

   HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP                = HAL_MDP_PIXEL_FORMAT_YUV_BASE,
   HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP,
   HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP,
   HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP,
   HAL_MDP_PIXEL_FORMAT_CRYCB_H1V1_24BPP,
   HAL_MDP_PIXEL_FORMAT_AYCBCR_H1V1_32BPP,
   HAL_MDP_PIXEL_FORMAT_ACRCBY_H1V1_32BPP,

   HAL_MDP_PIXEL_FORMAT_INTERLEAVED_END                  = HAL_MDP_PIXEL_FORMAT_ACRCBY_H1V1_32BPP,

   // Pseudo plannar pixel formats
   HAL_MDP_PIXEL_FORMAT_PSEUDO_PLANNAR_BASE,

   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP                = HAL_MDP_PIXEL_FORMAT_PSEUDO_PLANNAR_BASE,
   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CBCR_VENUS_4x4_12BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP,

   HAL_MDP_PIXEL_FORMAT_PSEUDO_PLANNAR_END               = HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP,

   // Planar pixel formats
   HAL_MDP_PIXEL_FORMAT_PLANNAR_BASE,

   HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP               = HAL_MDP_PIXEL_FORMAT_PLANNAR_BASE,
   HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP,
   HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP,


   HAL_MDP_PIXEL_FORMAT_PLANNAR_END                      = HAL_MDP_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP,

   HAL_MDP_PIXEL_FORMAT_YUV_END                          = HAL_MDP_PIXEL_FORMAT_PLANNAR_END,

   HAL_MDP_PIXEL_FORMAT_MAX,
   HAL_MDP_PIXEL_FORMAT_FORCE_32BIT                      = 0x7FFFFFFF

} HAL_MDP_PixelFormatType;

/** Helper Macro Definitions for pixel formats */

#define HAL_MDP_CHECK_INTERLEAVED_PIXEL_FORMAT(_pixel_format_)    ((HAL_MDP_PIXEL_FORMAT_INTERLEAVED_BASE <= _pixel_format_) && (HAL_MDP_PIXEL_FORMAT_INTERLEAVED_END >= _pixel_format_))
#define HAL_MDP_CHECK_PSEUDO_PLANAR_PIXEL_FORMAT(_pixel_format_)  ((HAL_MDP_PIXEL_FORMAT_PSEUDO_PLANNAR_BASE <= _pixel_format_) && (HAL_MDP_PIXEL_FORMAT_PSEUDO_PLANNAR_END >= _pixel_format_))
#define HAL_MDP_CHECK_PLANAR_PIXEL_FORMAT(_pixel_format_)         ((HAL_MDP_PIXEL_FORMAT_PLANNAR_BASE <= _pixel_format_) && (HAL_MDP_PIXEL_FORMAT_PLANNAR_END >= _pixel_format_))

#define HAL_MDP_IS_MDP_PIXEL_YUV_FORMAT(_pixel_format_)           ((HAL_MDP_PIXEL_FORMAT_YUV_BASE <= _pixel_format_) && (HAL_MDP_PIXEL_FORMAT_YUV_END >= _pixel_format_))

#define HAL_MDP_IS_MDP_DITHERING_PIXEL_FORMAT(_a_)                       \
((bool32) (((_a_) == HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP)             ||  \
           ((_a_) == HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP)             ||  \
           ((_a_) == HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP)             ||  \
           ((_a_) == HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP)             ||  \
           ((_a_) == HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP)))


/** Data structures */

/*!
 * \b HAL_MDP_HardwareVersionType
 */
typedef struct _HAL_HW_VersionType
{
  uint32      uMajorVersion;
  uint32      uMinorVersion;
  uint32      uReleaseVersion;

} HAL_HW_VersionType;

/*!
 * \b HAL_MDSS_PhysicalAddressType
 * 
 * Holds the 64-bit device address type (either physical address or VCM or SVA address)
 */
typedef union _HAL_MDSS_DeviceAddressType
{
    struct {
        uint32 uLowPart;
        int32  iHighPart;
    } u;
    int64  iQuadPart;

} HAL_MDSS_DeviceAddressType;


/*!
 * \struct HAL_MDP_SWMappedHWBaseAddressesType
 *
 * The \b HAL_MDP_SWMappedHWBaseAddressesType data structure contains SW mapped HW block base addresses
 *
 */
typedef struct _HAL_MDP_SWMappedHWBaseAddressesType
{
  uintPtr        uMdpAddress;            /**< SW mapped MDP HW block address */
  uintPtr        uSecCtrlAddress;        /**< SW mapped secure control HW block address */
  uintPtr        uMMSSMiscAddress;       /**< SW mapped MMSS misc HW block address */
} HAL_MDP_SWMappedHWBaseAddressesType;

/*=========================================================================
      Common HAL functions
  ==========================================================================*/
#ifdef __cplusplus
    extern "C" {
#endif


/****************************************************************************
HAL_MDSS_SetBaseAddress()

Description:
*
* Setups global register base address pointers
*/
void HAL_MDSS_SetBaseAddress(HAL_MDP_SWMappedHWBaseAddressesType *pMappedBaseAddress);



/****************************************************************************
*
** FUNCTION: HAL_MDSS_IsInterruptActive()
*/
/*!
* \brief
*     HAL_MDSS_IsInterruptActive --- Report MDSS cores that have active interrupts
*
* \param [out]   pModuleIDs            - Reports the list of modules that have active interrupts
* \param [in]    uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDSS_IsInterruptActive(uint32              *pModuleIDs,
                                              uint32               uFlags);

#ifdef __cplusplus
}
#endif


#endif /* _HAL_MDSS_H_ */
