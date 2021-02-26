/*======================================================================
    Copyright (c) 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
=======================================================================*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chipdlibcommon.h
/// @brief Phase Detection Library interface definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CHIPDLIBCOMMON_H
#define CHIPDLIBCOMMON_H

#include "camxcdktypes.h"
#include "chistatsinterfacedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const UINT16 PDLibMaxWindowCount   = 200;  ///< Maximum fixed pd window number
static const UINT16 PDLibMaxPixelCount    = 256;  ///< Maximum number of pd pixels in a block
static const UINT8  PDLibMaxBlockHeight   = 64;   ///< Maximum height of pd block
static const UINT16 PDLibAdaptiveDataSize = 960;  ///< Adaptive k data size

/// @brief Defines the peripheral window type.
typedef enum
{
    PDLibPeripheralWindowTopLeft = 0,      ///< selected Top left corner of the window
    PDLibPeripheralWindowTopCenter,        ///< selected Top center of the window
    PDLibPeripheralWindowTopRight,         ///< selected Top right corner of the window
    PDLibPeripheralWindowLeft,             ///< selected left corner of the window
    PDLibPeripheralWindowRight,            ///< selected right corner of the window
    PDLibPeripheralWindowBottomLeft,       ///< selected Bottom left corner of the window
    PDLibPeripheralWindowBottomCenter,     ///< selected Bottom center of the window
    PDLibPeripheralWindowBottomRight,      ///< selected Bottom right corner of the window
    PDLibPeripheralWindowMax               ///< Anchor to indicate the last item in the defines
} PDLibPeripheralWindow;

/// @brief Defines the pdlib processing result.
typedef enum
{
    PDLibResultInital,         ///< initial value, buffer not processed
    PDLibResultCenter,         ///< pdlib select center window
    PDLibResultPeripheral      ///< pdlib select peripheral window
} PDLibProcessingResult;

///< @brief Defines Bayer pattern streamed by sensor
typedef enum
{
    PDLibBayerBGGR, ///< bayer pattern : BGGR
    PDLibBayerGBRG, ///< bayer pattern : GBRG
    PDLibBayerGRBG, ///< bayer pattern : GRBG
    PDLibBayerRGGB  ///< bayer pattern : RGGB
} PDLibSensorBayerPattern;

///< @brief Defines the function return status */
typedef enum
{
    PDLibSuccess,           ///< returns success
    PDLibMemoryError,       ///< returns memory error
    PDLibParamError,        ///< returns invalid input parameter
    PDLibRunningError,      ///< returns processing error
    PDLibErrorMax = 1023    ///< Anchor to indicate the last item in the defines
} PDLibReturnStatus;

///< @brief Defines the shielded pd pixel pattern
typedef enum
{
    PDLibLeftDiodePixel        = 1,    ///< Shielded Left Pixel for dual diode and 2x1 sensors
    PDLibRightDiodePixel       = 0,    ///< Shielded Right Pixel for dual diode and 2x1 sensors

    PDLibLeftShieldedPixel     = 0,    ///< metal shielded left pd pixel
    PDLibRightShieldedPixel    = 1     ///< metal shielded right pd pixel
} PDLibPixelShieldInfo;

///< @brief Defined as a percentage of image size
typedef struct
{
    FLOAT startX;      ///< (0-1), percentage of start X coordinate in terms of image width
    FLOAT startY;      ///< (0-1), percentage of start Y coordinate in terms of image height
    FLOAT endX;        ///< (0-1), percentage of end X coordinate in terms of image width
    FLOAT endY;        ///< (0-1), percentage of end Y coordinate in terms of image height
} PDLibWindowInfo;

/// @brief Defines the main window type.
typedef enum
{
    PDLibRoiTypeCenter = 0x0,  ///<  Default type
    PDLibRoiTypeFace,          ///<  Face AF
    PDLibRoiTypeTouch,         ///<  Touch-AF
    PDLibRoiTypeTopLeft,       ///<  Peripheral TOP_LEFT ROI
    PDLibRoiTypeTopCenter,     ///<  Peripheral TOP_CENTER ROI
    PDLibRoiTypeTopRight,      ///<  Peripheral TOP_RIGHT ROI
    PDLibRoiTypeLeft,          ///<  Peripheral LEFT ROI
    PDLibRoiTypeRight,         ///<  Peripheral RIGHT ROI
    PDLibRoiTypeBottomLeft,    ///<  Peripheral BOTTOM_LEFT ROI
    PDLibRoiTypeBottomCenter,  ///<  Peripheral BOTTOM_CENTER ROI
    PDLibRoiTypeBottomRight,   ///<  Peripheral BOTTOM_RIGHT ROI
} PDLibRoiType;

/// @brief Defines window configuration in pdlib configuration
typedef struct
{
    PDLibRoiType      roiType;                ///< type of roi coordinates
    PDLibWindowInfo   fixedAFWindow;          ///< coordinates of ROI
    UINT              horizontalWindowCount;  ///<  number of windows along horizontal direction
    UINT              verticalWindowCount;    ///<  number of windows along vertical direction
} PDLibWindowConfig;

/// @brief Defines pd pixel location in one block
typedef struct
{
    INT                   x;      ///< pixel x coordinate
    INT                   y;      ///< pixel y coordinate
    PDLibPixelShieldInfo  type;   ///< pixel shield type
} PDLibPDPixelInfo;

/// @brief Defines sensor orientation
typedef enum
{
    PDLibOrientationDefault,       ///< default orientation
    PDLibOrientationMirror,        ///< mirrored orientation
    PDLibOrientationFlip,          ///< flipped orientation
    PDLibOrientationMirrorAndFlip, ///< flipped and mirrored orientation
    PDLibOrientationMax,           ///< Anchor to indicate the last item in the defines
} PDLibSensorOrientation;

/// @brief Defines camif dimensions
typedef struct
{
    UINT width;    ///< width of camif window
    UINT height;   ///< height of camif window
} PDLibDimensionInfo;

/// @brief Defines defocus range
typedef struct
{
    INT16 rangeNear;     ///< range near
    INT16 rangeFar;      ///< range far
} PDLibRangeInfo;

/// @brief Defines output data of pd library
typedef struct
{
    INT   defocus;                ///< defocus in DAC range
    UINT  confidence;             ///< confidence level
    FLOAT phaseDiff;              ///< phase difference
    PDLibRangeInfo defocusRange;  ///< defocus range
} PDLibDefocusInfo;

/// @brief Defines roi coordinates
typedef struct
{
    INT x;              ///< x coordinate
    INT y;              ///< y coordinate
    UINT width;         ///< width coordinate
    UINT height;        ///< height coordinate
} PDLibRectangleInfo;

/// @brief Defines the type of PD data.
typedef enum
{
    PDLibSensorInvalid,               ///< invalid information
    PDLibSensorType1,                 ///< sparse pd type 1
    PDLibSensorType2,                 ///< sparse pd type 2
    PDLibSensorType3,                 ///< sparse pd type 3
    PDLibSensorDualPD,                ///< dual PD Type 2
    PDLibSensorTypeMax,               ///< Anchor to indicate the last item in the defines
} PDLibSensorType;

// @brief Defines Sensor PD Stats Format for TYPE1
typedef enum
{
    PDLibSensorPDStatsCustom                = 0,
    PDLibSensorPDStatsRaw10Conf8BitPd10Bit  = 1,
    PDLibSensorPDStatsRaw10Conf11BitPd11Bit = 2
} PDLibSensorPDStatsFormat;

// @brief Defines the packing of pd data
typedef enum
{
    PDLibBufferFormatMipi8,        ///< mipi8
    PDLibBufferFormatMipi10,       ///< mipi10, [9:2] [9:2] [9:2] [9:2] [1:0][1:0][1:0][1:0]
    PDLibBufferFormatUnpacked16,   ///< raw 16 unpacked
    PDLibBufferFormatPacked10,     ///< raw 10 packed, Q10 format
} PDLibBufferFormat;

/// @brief Defines the pd data order
typedef enum
{
    PDLibPixelOrderDefault, ///< default order
    PDLibPixelOrderLR,      ///< left followed by right
    PDLibPixelOrderRL,      ///< right followed by left
} PDLibPixelOrderType;

/// @brief Defines the pd block pattern filled by sensor for type 2 and isp for type 3
typedef struct
{
    UINT                  pixelCount;                          ///< pd pixel number inside of a window
    PDLibPDPixelInfo      pixelCoordinate[PDLibMaxPixelCount]; ///< pixel 2D pos, left_pixel,right_pixel
    UINT32                horizontalPDOffset;                  ///< horizontal offsets
    UINT32                verticalPDOffset;                    ///< vertical offsets
    PDLibDimensionInfo    blockDimension;                      ///< pattern window width and height
} PDLibBlockPattern;

/// @brief Defines the lcr pd block pattern
typedef struct
{
    UINT                  lineCount;                          ///< number of lines of pd data in a block
    UINT                  linePosition[PDLibMaxBlockHeight];  ///< position of pd data in a block
    UINT32                horizontalPDOffset;                 ///< horizontal offsets
    UINT32                verticalPDOffset;                   ///< vertical offsets
    PDLibDimensionInfo    blockDimension;                     ///< pattern window width and height
} PDLibLcrBlockPattern;

/// @brief Defines the pd block pattern info
typedef struct
{
    UINT                  stride;         ///< stride information
    PDLibBufferFormat     bufferFormat;   ///< type of pd packing
    PDLibSensorType       sensorType;     ///< type of buffer
    PDLibBlockPattern     blockPattern;   ///< block pattern of pd data
} PDLibBlockPatternInfo;

/// @brief Defines native sensor pattern info
typedef struct
{
    PDLibBlockPattern blockPattern;           ///< defines block pattern
    UINT              horizontalBlockCount;   ///< number of PD blocks in x
    UINT              verticalBlockCount;     ///< number of PD blocks in y
} PDLibSensorNativePatternInfo;

/// @brief Defines native sensor pattern
typedef struct
{
    PDLibSensorNativePatternInfo    patternInfo;                ///< defines native sensor pattern
    PDLibRectangleInfo              cropRegion;                 ///< insensor cropped
    UINT                            originalImageWidth;         ///< orignal image width in pixel
    UINT                            originalImageHeight;        ///< orignal image height in pixel
    UINT                            currentImageWidth;          ///< current image width in pixel
    UINT                            currentImageHeight;         ///< current image height in pixel
    PDLibSensorOrientation          orientation;                ///< sensor orientation
    PDLibBufferFormat               bufferFormat;               ///< buffer data type
    FLOAT                           horizontalDownscaleFactor;  ///< horizontal downscale binning factor; 2 for 2PD IMX362
    FLOAT                           verticalDownscaleFactor;    ///< vertical downscale binning factor; 4 for 2PD IMX362
    FLOAT                           PDOffsetCorrection;         ///< pd offset correction
    FLOAT                           lcrPDOffset;                ///< lc pd offset
    PDLibLcrBlockPattern            lcrBlockPattern;            ///< lcr block pattern
    PDLibSensorBayerPattern         bayerPattern;               ///< bayer pattern streamed by sensor
} PDLibSensorNativeInfo;

/// @brief Defines lcr buffer data information
typedef struct
{
    PDLibLcrBlockPattern  isp1BlockPattern;       ///< left vfe camif data pattern
    PDLibLcrBlockPattern  isp2BlockPattern;       ///< right vfe camif data pattern
    UINT                  isp1BufferWidth;        ///< camif left buffer width
    UINT                  isp1BufferStride;       ///< camif left buffer stride
    UINT                  isp2BufferWidth;        ///< camif right buffer width
    UINT                  isp2BufferStride;       ///< camif right buffer stride
    UINT                  ispBufferHeight;        ///< camif buffer height
    UINT                  imageOverlap;           ///< pixel when images overlap for dual IFE
    UINT                  isp2ImageOffset;        ///< offset of right images
    PDLibBufferFormat     bufferFormat;           ///< buffer data type
} PDLibLcrBufferDataInfo;

/// @brief Defines pd data buffer information. or T3, camif data pattern. for T2, the pd stats pattern
typedef struct
{
    PDLibBlockPattern      isp1BlockPattern;            ///< isp1 camif data pattern, NULL in single VFE
    PDLibBlockPattern      isp2BlockPattern;            ///< isp2 camif data pattern, NULL in single VFE
    UINT                   isp1BufferWidth;             ///< subsampled image width in pixel
    UINT                   isp1BufferStride;            ///< subsampled image stride in byte
    UINT                   isp2BufferWidth;             ///< subsampled image width in pixel
    UINT                   isp2BufferStride;            ///< subsampled image stride in byte
    UINT                   ispBufferHeight;             ///< subsampled image height
    UINT                   imageOverlap;                ///< overlap in pixel for dual vfe in the original img prio subsample
    UINT                   isp2ImageOffset;             ///< right image offsets
    PDLibBufferFormat      bufferFormat;                ///< buffer data type
    PDLibSensorType        sensorType;                  ///< buffer status descriptor
    PDLibLcrBufferDataInfo lcrBufferData;               ///< lcr buffer data info
} PDLibDataBufferInfo;

/// @brief Defines  lcr raw type
typedef enum
{
    PDLibMIPIRaw,    ///< Raw type MIPI type
    PDLibIdealRaw,   ///< ideal raw type
    PDLibCamifRaw    ///< camif raw type
} PDLibLcrRawType;

/// @brief Defines  lcr sensor configuration
typedef struct
{
    INT                   enable;              ///< enable flag
    PDLibLcrRawType       rawType;             ///< lcr raw type
    PDLibLcrBlockPattern  lcrNativePattern;    ///< lcr native pattern
} PDLibLcrSensorConfigure;


/// @brief Defines input paratemers for initialization
typedef struct
{
    PDLibSensorNativeInfo       nativePatternInfo;          ///< pd pattern, location of pd pixels
    UINT                        blackLevel;                 ///< sensor black level
    UINT                        pixelDepth;                 ///< pixel depth
    UINT                        isHdrModeEnabled;           ///< is hdr mode enabled
    UINT                        PDPixelOverflowThreshold;   ///< pix threshold
    INT16                       macroDac;                   ///< macro dac value
    INT16                       infinityDac;                ///< infinity dac value
    FLOAT                       actuatorSensitivity;        ///< actuator sensitivity
    PDLibPixelOrderType         pixelOrderType;             ///< pixel order type
    VOID*                       pCalibrationParam;          ///< pointer to calibration parameters from sensor
    UINT32                      defocusBitShift;            ///< defocus bit shift
    INT32                       defocusConfidenceThreshold; ///< defocus confidence threshold
    PDLibSensorPDStatsFormat    sensorPDStatsFormat;        ///< sensor pd stats format for type1
    PDLibDataBufferInfo         bufferDataInfo;             ///< buffer data info
    StatsCameraInfo             cameraInfo;                 ///< Holds camera information
} PDLibInitParam;

/// @brief Defines input structure for defocus  processing
typedef struct
{
    UINT16*             pPDBuffer;                  ///< pointer to pd buffer, 16bit per pixel
    INT                 PDBufferFd;                 ///< file descriptor used by PDLib to map the buffer
    SIZE_T              PDBufferSize;               ///< PD Buffer size
    UINT16*             pRawBuffer;                 ///< pointer to the pixel buffer
    PDLibWindowConfig   windowConfig;               ///< support fixed window configuration
    FLOAT               imageAnalogGain;            ///< image sensor gain
    FLOAT               currentFps;                 ///< real-time FPS
    FLOAT               integrationTime;            ///< exposure time in nanoseconds
    INT16               lensPosition;               ///< current logical lens position
    StatsCameraInfo     cameraInfo;                 ///< Holds camera information
} PDLibParam;

/// @brief Defines structure for version
typedef struct
{
    UINT        majorVersion;    ///< major version of the library
    UINT        minorVersion;    ///< minor version of the library
    UINT        tinyVersion;     ///< sub minor version of the library
} PDLibVersion;

/// @brief Defines output structure for defocus
typedef struct
{
    PDLibDefocusInfo       defocus[PDLibMaxWindowCount];                 ///< defocus output for each roi
    UINT16                 isPeripheralValid;                            ///< is peripheral data valid
    PDLibDefocusInfo       resultPeripheral[PDLibPeripheralWindowMax];   ///< type of peripheral processing
    UINT                   horizontalWindowCount;                        ///< number of horizontal window count
    UINT                   verticalWindowCount;                          ///< number of vertical window count
    UINT                   windowNumber;                                 ///< number of grids
    PDLibProcessingResult  processingResult;                             ///< pdlib processing results
} PDLibOutputData;

/// @brief Identifies the type of create parameter
typedef enum
{
    PDLibCreateParamTypeInvalid                 = -1,           ///< Type Invalid
    PDLibCreateParamTypeCameraOpenIndicator,                    ///< Actual Camera Open state
    PDLibCreateParamTypeLibName,                                ///< PDLib name
    PDLibCreateParamTypeCameraInfo,                             ///< Camera information
                                                                ///  Payload: StatsCameraInfo
    PDLibCreateParamTypeCount,                                  ///< Create Param Type Count
    PDLibCreateParamTypeMax                     = 0x7FFFFFFF    ///< Max Create Param Type
} PDLibCreateParamType;

/// @brief Represents an PDLib input parameter
typedef struct
{
    PDLibCreateParamType    createParamType; ///< Type of parameter passed
    VOID*                   pParam;          ///< Payload of the particular parameter type
    UINT32                  sizeOfParam;     ///< Size of the payload.
} PDLibCreateParam;

/// @brief Represents PDLib creation parameters
typedef struct
{
    PDLibCreateParam*   pParamList;    ///< Pointer to PDLib create-parameter
    UINT32              paramCount;    ///< Number of input create-parameters
} PDLibCreateParamList;

/// @brief Identifies the type of destroy parameter
typedef enum
{
    PDLibDestroyParamTypeInvalid                = -1,           ///< Type Invalid
    PDLibDestroyParamTypeCameraCloseIndicator,                  ///< Camera Close Indicator
                                                                ///< UINT 0 - Camera Close 1 Camera Open
    PDLibDestroyParamTypeCameraInfo,                            ///< Camera information
                                                                ///  Payload: StatsCameraInfo
    PDLibDestroyParamTypeCount,                                 ///< Destroy Param Type Count
    PDLibDestroyParamTypeMax                    = 0x7FFFFFFF    ///< Max Destroy Param Type
} PDLibDestroyParamType;

/// @brief Represents an PDLib destroy parameter
typedef struct
{
    PDLibDestroyParamType   destroyParamType;       ///< Type of parameter passed
    VOID*                   pParam;                 ///< Payload of the particular parameter type
    UINT32                  sizeOfParam;            ///< Size of the payload.
} PDLibDestroyParam;

/// @brief Represents PDLib destroy parameters
typedef struct
{
    PDLibDestroyParam*  pParamList;    ///< Pointer to PDLib destroy-parameter
    UINT32              paramCount;    ///< Number of input destroy-parameters
} PDLibDestroyParamList;


#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIPDLIBCOMMON_H
