////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiisphvxdefs.h
///
/// @brief Qualcomm Technologies, Inc. ISP HVX definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIISPHVXDEFS_H
#define CHIISPHVXDEFS_H

#include "camxcdktypes.h"
#include "camxchinodeutil.h"

#ifdef ANDROID
#include <utils/Log.h>
#endif

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

/* Start of Macros */

#define MAX_ISP 2
#define HVX_TRUE 1
#define HVX_FALSE 0

#undef LOG_TAG
#define LOG_TAG "ISPHVX"

/* End of Macros */

/* Start of Enums */

typedef enum
{
  HVX_STUB_IFE_NULL,
  HVX_STUB_IFE0,          ///< VFE0
  HVX_STUB_IFE1,          ///< VFE1
  HVX_STUB_IFE_BOTH,      ///< Dual VFE
  HVX_INVALID_ISP
}HVXIFEType;

typedef enum
{
  HVX_SENSOR_BGGR,
  HVX_SENSOR_GBRG,
  HVX_SENSOR_GRBG,
  HVX_SENSOR_RGGB,
  HVX_SENSOR_UYVY,
  HVX_SENSOR_YUYV,
  HVX_SENSOR_MAX
}HVXSensorFilterArrangement;

typedef enum
{
   HVX_LIB_VECTOR_NULL,
   HVX_LIB_VECTOR_128,
   HVX_LIB_VECTOR_MAX,
}HVXVectorMode;

typedef enum
{
    IFEBEGINNING = 0,   ///< IFE BEGINNING
    BEFOREHDR    = 1,   ///< BEFORE HDR
    AFTERHDR     = 2,   ///< AFTER HDR
    BEFOREWB     = 3,   ///< BEFORE WB
}HVXTapPoint ;

/* Start of struct definition */

 /// @brief HVXSensorInfo
typedef struct  {
  HVXSensorFilterArrangement bayer_format;    ///< bayer format for sensor output
  UINT                       bits_per_pixel;  ///< number of bits per pixel
}HVXSensorInfo;

 /// @brief HVXISPInfo
typedef struct  {
  UINT camifWidth;    ///< camif width
  UINT camifHeight;   ///<  camif height
  UINT sensorOffset;  ///<  offset for sensor output width
}HVXISPInfo;

 /// @brief HVXSingleISPInfo
typedef struct  {
  HVXISPInfo isp_info;
}HVXSingleISPInfo;

 /// @brief HVXDualISPInfo
typedef struct  {
  HVXISPInfo isp_info[MAX_ISP];
}HVXDualISPInfo;

 /// @brief HVXRequestDataType
typedef enum  {
    HVX_DATA_STATS,
    HVX_DATA_FRAME,
    HVX_DATA_MAX
}hvxRequestDataType;

 /// @brief HVXRequestDataMode
typedef enum  {
    REQUEST_STATS_ONLY,                             ///< OEM stub requesting Stats buffer only
    REQUEST_FRAME_DUMP_ONLY,                        ///< OEM stub requesting Frame dump only
    REQUEST_BOTH,                                   ///< OEM stub requesting both stats and frame dump
    REQUEST_MODE_MAX,
}hvxRequestDataMode;

 /// @brief requestCalculatedData
typedef struct
{
    hvxRequestDataType type;              ///< input type of request_buffer_t
    INT                numBuffers;        ///< input num buffers
    INT                dataSizes;         ///< input data Sizes
    UINT64*            dataAddresses;     ///< output array of buffers
    INT                dataLength;        ///< output number of addresses
    INT*               dataLabel;         ///< output label mapping
    INT                dataLabelLen;      ///< output length of label array
}requestCalculatedData;

 /// @brief HVXStubStaticConfig
typedef struct
{
    HVXTapPoint                tappingPoint;          ///< tap point
    HVXIFEType                 IFEid;                 ///< input
    INT                        hvxUnitNo[2];          ///< input
    INT                        width;                 ///< input
    INT                        height;                ///< input
    INT                        imageOverlap;          ///< input
    INT                        rightImageOffset;      ///< input
    INT                        OutWidth;              ///< input
    INT                        OutHeight;             ///< input
    HVXSensorFilterArrangement pixelFormat;           ///< input
    INT                        bitsPerPixel;          ///< input
    UINT                       ifeClkFreq[2];         ///< input
    HVXVectorMode              hvxStubVectorMode;     ///< input
    requestCalculatedData      data[2];               ///< request data
    hvxRequestDataMode         dataMode;              ///< data modee
}HVXStubStaticConfig;

 /// @brief HVXConfig
typedef struct  {
 HVXStubStaticConfig stubStaticConfig;  ///< Configuration
 HVXSensorInfo       sensor_info;       ///< sensor information
 union {
   HVXSingleISPInfo single_isp_info;    ///< single isp info
   HVXDualISPInfo   dual_isp_info;      ///< dual isp info
 } u;
  HVXIFEType         isp_type;          ///< ISP type (single / dual)
}HVXConfig;

 /// @brief HVXStatsData
typedef struct
{
  VOID         *stats_left;       ///<  left stats pointer
  UINT          stats_left_size;  ///<  left stats pointer size
  VOID         *stats_right;      ///<  right stats pointer
  UINT          stats_right_size; ///<  right stats pointer size
}HVXStatsData;

/// @brief HVXResolutionInfo
typedef struct
{
    UINT          sensorWidth;                ///<  [INPUT] sensor output width
    UINT          sensorHeight;               ///<  [INPUT] sensor output height
    HVXTapPoint   tappingPoint;               ///<  [INPUT]tap point
    UINT          outputFormat;               ///<  [INPUT] hvx output format
    UINT          hvxOutWidth;                ///<  [OUTPUT] HVX output width
    UINT          hvxOutHeight;               ///<  [OUTPUT] HVX output height
}HVXResolutionInfo;

 /// @brief HVXGetSetInfo
typedef struct
{
  UINT          availableHVXUnits;          ///<  [INPUT] currently available hvx units
                                            ///<  on DSP side(1 / 2 / 3 / 4)
  HVXVectorMode availableHVXVectorMode;     ///<  [INPUT] currently available hvx vector mode
  UINT          availableL2Size;            ///<  [INPUT] available L2 cache size
  BOOL          isStatsNeeded;              ///<  [INPUT]Stats data needed.
  UINT          hvxEnable;                  ///<  [OUTPUT] enable / disable HVX for current configuration
  char          algorithmName[32];          ///<  [OUTPUT] algorithm name to run on ADSP
  UINT          requestHVXUnits;            ///<  [OUTPUT] hvx units to be used for this sensor
  HVXVectorMode requestHVXVectorMode;       ///<  [OUTPUT] HVX vector mode to be used for this sensor
  UINT          kernelSize;                 ///<  [OUTPUT] Kernel size for Dual IFE calculation
} HVXGetSetInfo;

 /// @brief Input Data to HVX IQ Algorithm
typedef struct
{
  UINT         requestId;       ///< current Request id

  /* AEC params */
  FLOAT        realGain;        ///< global real gain
  FLOAT        exposureTime;    ///< exposure time in ns
  FLOAT        AECSensitivity;  ///< AEC Sensitivity
  FLOAT        DRCGain;         ///< DRC gain
  FLOAT        luxIndex;        ///< lux index

  /* AWB params */
  FLOAT        RGain;           ///< R awb gain
  FLOAT        GGain;           ///< G awb gain
  FLOAT        BGain;           ///< B awb gain
  UINT         colorTemp;       ///< color temperature

  FLOAT        LEDTrigger;      ///< Input trigger value:  Led Index
}HVXInputData;


typedef CDKResult(*PFDSPSTATICCONFIG)  (INT*, HVXStubStaticConfig*);
typedef CDKResult(*PFDSPDYANMCICONFIG) (INT*, void*, UINT);


/// @brief HVXDSPConfig
typedef struct HVXDSPConfig {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXDSPConfig:
    ///
    /// @brief ADSP config call
    /// @param  adspConfig_call function pointer to pass params ADSP
    ///
    ///
    /// @return CDKResultSuccess upon success.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PFDSPDYANMCICONFIG pDSPDynamicConfig;
    PFDSPSTATICCONFIG  pDSPStaticConfig;
} HVXDSPConfig;

typedef struct CHIISPHVXALGORITHMCALLBACKS
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXInitialize:
    ///
    /// @brief Allocate oem_data and return to caller
    /// @param oem_data double pointer to oem_data, to be
    ///             filled by OEM stub
    ///
    /// @return CDKResultSuccess upon success.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*pHVXInitialize)(VOID **oem_data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXGetResolutionInfo:
    ///
    /// @brief  Send Input from ISP and get OEM ouput information
    ///
    /// @param oem_data  OEM specific private data
    /// @param hvx_info  hvx information
    ///
    /// @return CDKResultSuccess upon success.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*pHVXGetResolutionInfo)(VOID *oem_data,
        HVXResolutionInfo **res_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXGetSetHVXInfo:
    ///
    /// @brief  Send Input from ISP and get OEM ouput information
    ///
    /// @param oem_data  OEM specific private data
    /// @param hvx_info  hvx information
    ///
    /// @return CDKResultSuccess upon success.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*pHVXGetSetHVXInfo)(VOID *oem_data,
        HVXGetSetInfo *hvx_info);

     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /// HVXSetConfig:
     ///
     ///  @brief Set sensor output along with Single / dual vfe information
     ///
     ///  @param oem_data: OEM specific private data
     ///  @param lib_config   sensor output info
     ///  @param adspConfig  ADSP config handle
     ///  @param handle       caller private data to be passed back in
     ///              adspConfig_call
     ///
     /// @return CDKResultSuccess upon success.
     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*pHVXSetConfig)(VOID *oem_data,
      HVXConfig*          stubConfig,
      const HVXDSPConfig *adspConfig,
      INT*                handle);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXCallbackData
    ///
    ///  @brief  This method destroys the derived instance of the interface
    ///
    ///  @param handle     handle
    ///  @param RequestID  Input feedback params
    ///  @param bufLabel   Buffer label
    ///  @param caller_data  caller private data to be passed back in
    ///         adspConfig_call
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*pHVXCallbackData)(INT handle,
        HVXIFEType IFEType, UCHAR bufLabel);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXExecute
    ///
    ///  @brief  This method destroys the derived instance of the interface
    ///
    ///  @param OEMData      OEM specific private data
    ///  @param InputParams  Input feedback params
    ///  @param ADSPConfig   ADSP config handle
    ///  @param handle       caller private data to be passed back in
    ///         adspConfig call
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*pHVXExecute)(VOID *oem_data,
      const HVXInputData *InputParams,
      const HVXDSPConfig *adspConfig,
      INT*  handle);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// HVXDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pCHIISPHVXAlgorithm   Pointer to CHIISPHVXAlgorithm instance
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*pHVXDestroy)(void *oem_data);

} CHIISPHVXALGORITHMCALLBACKS;
/* End of struct definition */

/* Start of function declaration */

/** hvx_lib_fill_function_table:
 *
 *  @func_table: [INPUT]function table handle which has place holders to
 *  get other HVX functions
 *
 *  OEM library implement this function. Call will open this function using
 *  dlsym to get other HVX function pointers
 *
 *  Return type of CDKResult
 *
 *  Finish in <= 1 ms
 **/
 typedef VOID (*PFCHIISPALGORITHMENTRY) (CHIISPHVXALGORITHMCALLBACKS* pAECALGORITHMCallbacks);

CAMX_VISIBILITY_PUBLIC CDKResult ChiISPHVXAlgorithmEntry(
        CHIISPHVXALGORITHMCALLBACKS *pAlgorithmCallbacks);

/* End of function declaration */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
