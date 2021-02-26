////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chi.h
/// @brief CamX Hardware Interface (CHI) definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHI_H
#define CHI_H

#include "camxcdktypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#pragma pack(push, 8)

static const UINT32 MaxNumImageSensors  = 16;

static const UINT32 ZSLQueueDepth       = 8;    ///< The zsl queue depth
                                                ///  Depth should not be too big because the queue will be filled up
                                                ///  with buffers which impacts memory footprint.
                                                ///  It should not be too small so the usecase has the option to pick
                                                ///  old frames to achieve zsl behavior.

static const UINT32 MaxConfigSettings   = 64;

/// @brief Buffer Format
typedef enum ChiBufferFormat
{
    ChiFormatJpeg            = 0,
    ChiFormatY8              = 1,
    ChiFormatY16             = 2,
    ChiFormatYUV420NV12      = 3,
    ChiFormatYUV420NV21      = 4,
    ChiFormatYUV420NV16      = 5,
    ChiFormatBlob            = 6,
    ChiFormatRawYUV8BIT      = 7,
    ChiFormatRawPrivate      = 8,
    ChiFormatRawMIPI         = 9,
    ChiFormatRawPlain16      = 10,
    ChiFormatRawMeta8BIT     = 11,
    ChiFormatUBWCTP10        = 12,
    ChiFormatUBWCNV12        = 13,
    ChiFormatUBWCNV124R      = 14,
    ChiFormatYUV420NV12TP10  = 15,
    ChiFormatYUV420NV21TP10  = 16,
    ChiFormatYUV422NV16TP10  = 17,
    ChiFormatPD10            = 18,
    ChiFormatRawMIPI8        = 19,
    ChiFormatFDY8            = 20,
    ChiFormatFDYUV420NV12    = 21,
    ChiFormatP010            = 22,
} CHIBUFFERFORMAT;

typedef enum InSensorVideoHDRType
{
    InSensorVideoMode2RAWFrameSHDR = 0,             ///< RAW Frame SHDR 2 Frames
    InSensorVideoMode3RAWFrameSHDR,                 ///< RAW Frame SHDR 3 Frames
    InSensorVideoMode2YUVFrameSHDR,                 ///< YUV Frame SHDR 2 Frames Default :GPU stats
    InSensorVideoMode3YUVFrameSHDR,                 ///< YUV Frame SHDR 3 Frames
    InSensorVideoMode2YUVFrameSHDRHWStats,          ///< YUV Frame SHDR 2 Frames IFE Stats
    InSensorVideoMode3YUVFrameSHDRVCMode,           ///< YUV Frame SHDR 3 Frames
    MaxInSensorVideoMode
} CHISHDRTYPE;


/// @brief Buffer Heap
const UINT32 BufferHeapSystem         = 0;
const UINT32 BufferHeapIon            = 1;
const UINT32 BufferHeapDSP            = 2;
const UINT32 BufferHeapEGL            = 3;

/// @brief Buffer mem flags
const UINT32 BufferMemFlagHw          = (1 << 0);
const UINT32 BufferMemFlagProtected   = (1 << 1);
const UINT32 BufferMemFlagCache       = (1 << 2);
const UINT32 BufferMemFlagLockable    = (1 << 3);

typedef VOID* CHIBUFFERMANAGER;
typedef VOID* CHIBUFFERHANDLE;
typedef VOID* CHIHANDLE;
typedef VOID* CHIPIPELINEDESCRIPTOR;
typedef VOID* CHIPIPELINEHANDLE;
typedef VOID* CHIMETAHANDLE;

/// @brief Gralloc Usage flags
typedef UINT32 GrallocUsage;
typedef UINT32 NodeProperty;

static const GrallocUsage GrallocUsageSwReadNever       = 0x00000000;   ///< Buffer is never read in software
static const GrallocUsage GrallocUsageSwReadRarely      = 0x00000002;   ///< Buffer is rarely read in software
static const GrallocUsage GrallocUsageSwReadOften       = 0x00000003;   ///< Buffer is often read in software
static const GrallocUsage GrallocUsageSwReadMask        = 0x0000000F;   ///< Mask for the software read values

static const GrallocUsage GrallocUsageSwWriteNever      = 0x00000000;   ///< Buffer is never written in software
static const GrallocUsage GrallocUsageSwWriteRarely     = 0x00000020;   ///< Buffer is rarely written in software
static const GrallocUsage GrallocUsageSwWriteOften      = 0x00000030;   ///< Buffer is often written in software
static const GrallocUsage GrallocUsageSwWriteMask       = 0x000000F0;   ///< Mask for the software write values

static const GrallocUsage GrallocUsageHwTexture         = 0x00000100;   ///< Buffer will be used as an OpenGL ES texture
static const GrallocUsage GrallocUsageHwRender          = 0x00000200;   ///< Buffer will be used as an OpenGL ES render target
static const GrallocUsage GrallocUsageHw2D              = 0x00000400;   ///< Buffer will be used by the 2D hardware blitter
static const GrallocUsage GrallocUsageHwComposer        = 0x00000800;   ///< Buffer will be used by the HWComposer module
static const GrallocUsage GrallocUsageHwFrameBuffer     = 0x00001000;   ///< Buffer will be used with the framebuffer device

static const GrallocUsage GrallocUsageExternalDisplay   = 0x00002000;   ///< Buffer should be displayed full-screen on an
                                                                        ///  external display when possible

static const GrallocUsage GrallocUsageProtected         = 0x00004000;   ///< Must have a hardware-protected path to external
                                                                        ///  display sink for this buffer.  If a hardware-
                                                                        ///  protected path is not available, then either don't
                                                                        ///  composite only this buffer (preferred) to the
                                                                        ///  external sink, or (less desirable) do not route the
                                                                        ///  entire composition to the external sink.

static const GrallocUsage GrallocUsageCursor            = 0x00008000;   ///< Buffer may be used as a cursor

static const GrallocUsage GrallocUsageHwVideoEncoder    = 0x00010000;   ///< Buffer will be used with the HW video encoder
static const GrallocUsage GrallocUsageHwCameraWrite     = 0x00020000;   ///< Buffer will be written by the HW camera pipeline
static const GrallocUsage GrallocUsageHwCameraRead      = 0x00040000;   ///< Buffer will be read by the HW camera pipeline
static const GrallocUsage GrallocUsageHwCameraZSL       = 0x00060000;   ///< Buffer will be used in zero-shutter-lag queue
static const GrallocUsage GrallocUsageHwCameraMask      = 0x00060000;   ///< Mask for the camera access values
static const GrallocUsage GrallocUsageHwMask            = 0x00071F00;   ///< Mask for the software usage bit-mask

static const GrallocUsage GrallocUsageRenderScript      = 0x00100000;   ///< Buffer will be used as a RenderScript Allocation

static const GrallocUsage GrallocUsageForeignBuffers    = 0x00200000;   ///< Set by the consumer to indicate to the producer
                                                                        ///  that they may attach a buffer that they did not
                                                                        ///  detach from the BufferQueue. Will be filtered out
                                                                        ///  by GrallocUsageAllocMask, so gralloc modules will
                                                                        ///  not need to handle this flag.

static const GrallocUsage GrallocUsageAllocMask         = ~(GrallocUsageForeignBuffers);    ///< Mask of all flags which could
                                                                                            ///  be passed to a gralloc module
                                                                                            ///  for buffer allocation. Any
                                                                                            ///  flags not in this mask do not
                                                                                            ///  need to be handled by gralloc
                                                                                            ///  modules.

static const NodeProperty NodePropertyCustomLib                  = 1;     ///< Custom lib
static const NodeProperty NodePropertyProfileId                  = 2;     ///< IPE/BPS Instance Property Profile
static const NodeProperty NodePropertyStabilizationType          = 3;     ///< IPE Instance Property Stabilization Type
static const NodeProperty NodePropertyProcessingType             = 4;     ///< IPE Instance Property Processing Type
static const NodeProperty NodePropertyIPEDownscale               = 5;     ///< IPE Instance IPE downscale
static const NodeProperty NodePropertyStatsSkipPattern           = 6;
static const NodeProperty NodePropertyIFECSIDHeight              = 7;
static const NodeProperty NodePropertyIFECSIDWidth               = 8;
static const NodeProperty NodePropertyIFECSIDLeft                = 9;
static const NodeProperty NodePropertyIFECSIDTop                 = 10;
static const NodeProperty NodePropertyNodeClass                  = 11;    ///< Node class -
                                                                          ///  Bypassable node or Default Node or inplace node
static const NodeProperty NodePropertyGPUCapsMaskType            = 12;    ///< GPU Instance Capabilities Mask Type
static const NodeProperty NodePropertyIPEDownscaleWidth          = 13;    ///< IPE Instance IPE downscale width
static const NodeProperty NodePropertyIPEDownscaleHeight         = 14;    ///< IPE Instance IPE downscale height
static const NodeProperty NodePropertyEnbaleIPECHICropDependency = 15;    ///< Indicates if we should wait on crop info in IPE
static const NodeProperty NodePropertyEnableFOVC                 = 16;    ///< FOVC enabled
static const NodeProperty NodePropertyStitchMaxJpegSize          = 17;    ///< Max Jpegsize for Dual Cam Stitched frame
static const NodeProperty NodePropertyForceSingleIFEOn           = 18;    ///< Flag to force IFE to single IFE
static const NodeProperty NodePropertylibSHDRBitDepth            = 19;    ///< SHDR lib output Raw bit depth
static const NodeProperty NodePropertyContext                    = 20;    ///< Stats Parser Type SEF MEF LEF

static const NodeProperty NodePropertyVendorStart       = 1023;

/// @brief Stream config mode
typedef enum StreamConfigMode
{
    StreamConfigModeNormal                  = 0x0000,   ///< Normal stream configuration operation mode
    StreamConfigModeConstrainedHighSpeed    = 0x0001,   ///< Special constrained high speed operation mode for devices that can
                                                        ///  not support high speed output in NORMAL mode
    StreamConfigModeVendorStart             = 0x8000,   ///< First value for vendor-defined stream configuration modes
    StreamConfigModeVendorEnd               = 0xEFFF,   ///< End value for vendor-defined stream configuration modes
    StreamConfigModeQTIStart                = 0xF000,   ///< First value for QTI-defined stream configuration modes
    StreamConfigModeQTITorchWidget          = 0xF001,   ///< Operation mode for Torch Widget.
    StreamConfigModeVideoHdr                = 0xF002,   ///< Video HDR On
    StreamConfigModeQTIEISRealTime          = 0xF004,   ///< Operation mode for Real-Time EIS recording usecase.
    StreamConfigModeQTIEISLookAhead         = 0xF008,   ///< Operation mode for Look Ahead EIS recording usecase
    StreamConfigModeQTIFOVC                 = 0xF010,   ///< Field of View compensation in recording usecase.
    StreamConfigModeQTIEISDG                = 0xF020,   ///< Operation mode for IOT EIS recording usecase
    StreamConfigModeQTIEISDGCustom1         = 0xF040,   ///< Operation mode for IOT EIS recording usecase, EIS on main stream
                                                        ///  only. Total 3 streams to be configured
    StreamConfigModeQTIEISDGCustom2         = 0xF080,   ///< Operation mode for IOT EIS recording usecase, EIS on all streams
    StreamConfigModeSensorMode              = 0xF0000,  ///< Sensor Mode selection for >=60fps
                                                        ///  require 4 bits for sensor mode index
} STREAMCONFIGMODE;

/// @brief HDR mode for the stream
typedef enum StreamHDRMode
{
    HDRModeNone = 0,   ///< None HDR mode
    HDRModeHLG,               ///< HLG mode
    HDRModeHDR10,             ///< HDR 10 bit mode
    HDRModeHDR10Plus,         ///< HDR10+ mode
    HDRModePQ,                ///< PQ mode
    HDRModeMax                ///< Invalid Max value
} STREAMHDRMODE;

/// @brief Pixel Format
typedef enum ChiStreamFormat
{
    ChiStreamFormatYCrCb420_SP   = 0x00000011,   ///< YCrCb420_SP format
    ChiStreamFormatRaw16         = 0x00000020,   ///< Blob format
    ChiStreamFormatBlob          = 0x00000021,   ///< Carries data which does not have a standard image structure (e.g. JPEG)
    ChiStreamFormatImplDefined   = 0x00000022,   ///< Format is up to the device-specific Gralloc implementation.
    ChiStreamFormatYCbCr420_888  = 0x00000023,   ///< Efficient YCbCr/YCrCb 4:2:0 buffer layout, layout-independent
    ChiStreamFormatRawOpaque     = 0x00000024,   ///< Raw Opaque
    ChiStreamFormatRaw10         = 0x00000025,   ///< Raw 10
    ChiStreamFormatRaw12         = 0x00000026,   ///< Raw 10
    ChiStreamFormatPD10          = 0x00000026,   ///< PD10
    ChiStreamFormatY8            = 0x20203859,   ///< Y 8
    ChiStreamFormatY16           = 0x20363159,   ///< Y 16
    ChiStreamFormatP010          = 0x7FA30C0A,   ///< P010
    ChiStreamFormatUBWCTP10      = 0x7FA30C09,   ///< UBWCTP10
    ChiStreamFormatUBWCNV12      = 0x7FA30C06,   ///< UBWCNV12
} CHISTREAMFORMAT;

/// @brief Stream Type
typedef enum ChiStreamType
{
    ChiStreamTypeOutput        = 0,                ///< Output stream
    ChiStreamTypeInput         = 1,                ///< Input stream
    ChiStreamTypeBidirectional = 2                 ///< Input and output
} CHISTREAMTYPE;

/// @brief Dataspace of the buffer
typedef enum ChiDataSpace
{
    DataspaceUnknown            = 0x0000,                       ///< Default-assumption data space
    DataspaceArbitrary          = 0x0001,                       ///< Arbitrary dataspace
    DataspaceStandardShift      = 16,                           ///< Standard shift
    DataspaceTransferShift      = 22,                           ///< Transfer shift
    DataspaceRangeShift         = 27,                           ///< Range shift
    DataspaceStandardBT601_625  = 2 << DataspaceStandardShift,  ///< This adjusts the luminance interpretation
                                                                ///  for RGB conversion from the one purely determined
                                                                ///  by the primaries to minimize the color shift into
                                                                ///  RGB space that uses BT.709 primaries.
    DataspaceTransferSmpte170M  = 3 << DataspaceTransferShift,  ///< BT.601 525, BT.601 625, BT.709, BT.2020
    DataspaceRangeFull          = 1 << DataspaceRangeShift,     ///< Full range uses all values for Y, Cb and Cr from
                                                                ///  0 to 2 ^ b - 1, where b is the bit depth of the
                                                                ///  color format.
    DataspaceJFIF               = 0x0101,                       ///< JPEG File Interchange Format(JFIF). Same model as
                                                                ///  BT.601-625, but all YCbCr values range from 0 to 255.
    DataspaceV0JFIF             = DataspaceStandardBT601_625 |
                                  DataspaceTransferSmpte170M |
                                  DataspaceRangeFull,           ///< JPEG File Interchange Format(JFIF). Same model as
                                                                ///  BT.601-625, but all YCbCr values range from
                                                                ///  0 to 255.
    DataspaceBT601_625          = 0x0102,                       ///< ITU-R Recommendation BT.601 - 625line
    DataspaceBT601_525          = 0x0103,                       ///< ITU-R Recommendation BT.601 - 525line
    DataspaceBT709              = 0x0104,                       ///< ITU-R Recommendation BT.709
    DataspaceSRGBLinear         = 0x0200,                       ///< The red, green, and blue components are stored in sRGB
                                                                ///  space, but are linear and not gamma-encoded.
    DataspaceSRGB               = 0x0201,                       ///< The red, green and blue components are stored in sRGB
                                                                ///  space and converted to linear space
    DataspaceDepth              = 0x1000                        ///< The buffer contains depth ranging measurements from a
                                                                ///  depth camera.
} CHIDATASPACE;

/// @brief Stream rotation
typedef enum ChiStreamRotation
{
    StreamRotationCCW0      = 0,                ///< No rotation
    StreamRotationCCW90     = 1,                ///< Rotate by 90 degree counterclockwise
    StreamRotationCCW180    = 2,                ///< Rotate by 180 degree counterclockwise
    StreamRotationCCW270    = 3                 ///< Rotate by 270 degree counterclockwise
} CHISTREAMROTATION;

/// @brief camera facing values
typedef enum ChiDirection
{
    FacingBack              = 0,                ///< device facing back
    FacingFront             = 1,                ///< device facing front
    External                = 2                 ///< external devices
} CHIDIRECTION;

/// @brief camera sensor role/type
typedef enum ChiSensorPositionType
{
    NONE                    = 0,                /// not part of multicamera
    REAR                    = 1,                /// Rear main camera
    FRONT                   = 2,                /// Front main camera
    REAR_AUX                = 3,                /// Rear Aux Camera
    FRONT_AUX               = 4,                /// Front aux camera
} CHISENSORPOSITIONTYPE;

/// @brief Chi Stream param for stride and slice info
typedef struct ChiStreamParams
{
    UINT32              planeStride;     ///< The number of bytes between the first byte of two sequential lines on plane 1. It may be
                                         ///  greater than nWidth * nDepth / 8 if the line includes padding.
                                         ///  Macro-tile width aligned for UBWC
    UINT32              sliceHeight;     ///< The number of lines in the plane which can be equal to or larger than actual frame height.
                                         ///  Tile height aligned for UBWC
} CHISTREAMPARAMS;

/// @brief Stream structure
typedef struct ChiStream
{
    CHISTREAMTYPE     streamType;       ///< The type of the stream
    UINT32            width;            ///< The width in pixels of the buffers in this stream
    UINT32            height;           ///< The height in pixels of the buffers in this stream
    CHISTREAMFORMAT   format;           ///< The pixel format for the buffers in this stream. Format is a value from
                                        ///  the PixelFormat* list or from device-specific headers.
    GrallocUsage      grallocUsage;     ///< The gralloc usage (GRALLOC_USAGE_*) flags defined in gralloc.h for this
                                        ///  stream
    UINT32            maxNumBuffers;    ///< The maximum number of buffers that may be dequeued at the same time
    VOID*             pPrivateInfo;     ///< A handle to private information for the stream
    CHIDATASPACE      dataspace;        ///< A field that describes the contents of the buffer
    CHISTREAMROTATION rotation;         ///< The output rotation of the stream
    CHISTREAMPARAMS   streamParams;     ///< Stream specific definitions.
    UINT32            reserved[5];     ///< Padding reserved for future use
} CHISTREAM;

/// @brief Stream buffer
typedef struct ChiStreamBuffer
{
    CHISTREAM*          pStream;                ///< Stream information
    CHIBUFFERHANDLE     phBuffer;               ///< Output Buffer
    INT                 bufferStatus;           ///< Status of the buffer
    INT                 acquireFence;           ///< Acquire fence
    INT                 releaseFence;           ///< Release fence
} CHISTREAMBUFFER;

/// @brief Result Private data
typedef struct ChiPrivateData
{
    UINT32           requestIndex;                  ///< Pipeline handle for which results generated.
    UINT32           streamIndex;                   ///< Private data to save pipeline index from chi usecase
    UINT32           featureType;                   ///< Feature used for this specific request
    UINT32           numInputBuffers;               ///< Number of input buffers for this request
    CHIBUFFERHANDLE  inputBuffers[ZSLQueueDepth];   ///< Input buffer handle pointers for this request,
                                                    ///  the numer of input buffers cannot exceed ZSLQueueDepth.
    CHIBUFFERMANAGER bufferManagers[ZSLQueueDepth]; ///< The buffer managers which the input buffers belong to,
                                                    ///  in result callback we will release references to the input buffers.
} CHIPRIVDATA;

/// @brief Capture result
typedef struct ChiCaptureResult
{
    UINT32                      frameworkFrameNum;  ///< Unique frame number
    const VOID*                 pResultMetadata;    ///< Result metadata for this capture. Can be NULL
    UINT32                      numOutputBuffers;   ///< Number of output buffers returned in this result structure
    const CHISTREAMBUFFER*      pOutputBuffers;     ///< Handles for the output stream buffers for this capture
    const CHISTREAMBUFFER*      pInputBuffer;       ///< Handle for the input stream buffer for this capture. Can be NULL
    UINT32                      numPartialMetadata; ///< Number of partial metadata results
    CHIPRIVDATA*                pPrivData;          ///< Pointer to private data. Managed by client.
} CHICAPTURERESULT;

/// @brief Partial Capture result
typedef struct ChiPartialCaptureResult
{
    UINT32                  frameworkFrameNum;      ///< Unique frame number
    CHIMETAHANDLE           pPartialResultMetadata; ///< Partial Result metadata for this capture.
    CHIPRIVDATA*            pPrivData;              ///< Pointer to private data. Managed by client.
} CHIPARTIALCAPTURERESULT;

/// @brief Message type
typedef enum ChiMessageType
{
    ChiMessageTypeError                 = 1, ///< An error has occurred
    ChiMessageTypeShutter               = 2, ///< The exposure of a given request or processing a reprocess request has begun
    ChiMessageTypeSof                   = 3, ///< SOF event
    ChiMessageTypeMetaBufferDone        = 4, ///< Indicate that the metabuffer is done processing
    ChiMessageTypePartialMetaBufferDone = 5, ///< Indicate that the partial metabuffer is available
    ChiMessageTypeMax
} CHIMESSAGETYPE;

/// @brief Error message codes
typedef enum ChiErrorMessageCode
{
    MessageCodeDevice   = 1,    ///< A serious failure occurred and no further frames will be produced by the device
    MessageCodeRequest  = 2,    ///< An error has occurred in processing a request and no output will be produced for this
                                ///  request
    MessageCodeResult   = 3,    ///< An error has occurred in producing an output result metadata buffer for a request, but
                                ///  output stream buffers for it will still be available
    MessageCodeBuffer   = 4     ///< An error has occurred in placing an output buffer into a stream for a request. The frame
                                ///  metadata and other buffers may still be available
} CHIERRORMESSAGECODE;

/// @brief Pipeline output type
typedef enum ChiPipelineOutputType
{
    ChiPipelineOutputDefault,
    ChiPipelineOutputPreview,
    ChiPipelineOutputVideo,
    ChiPipelineOutputSnapshot,
    ChiPipelineOutputDepth,
    ChiPipelineOutputMax,
} CHIPIPELINEOUTPUTTYPE;

/// @brief Error message structure
typedef struct ChiErrorMessage
{
    UINT32              frameworkFrameNum;  ///< Frame number of the request the error applies to. 0 if the frame number
                                            ///  isn't applicable to the error
    CHISTREAM*          pErrorStream;       ///< Pointer to the stream that had a failure. NULL if the stream isn't
                                            ///  applicable to the error
    CHIERRORMESSAGECODE errorMessageCode;   ///< The code for this error
} CHIERRORMESSAGE;

/// @brief Shutter message structure
typedef struct ChiShutterMessage
{
    UINT32 frameworkFrameNum;               ///< Frame number of the request that has begun exposure or reprocessing
    UINT64 timestamp;                       ///< Timestamp for the start of capture
} CHISHUTTERMESSAGE;

/// @brief SOF message structure
typedef struct ChiSofMessage
{
    UINT32  frameworkFrameNum;         ///< Frame number of the request that has begun exposure or reprocessing
    UINT64  timestamp;                 ///< Timestamp for the start of capture
    UINT32  sofId;                     ///< SOF id
    BOOL    bIsFrameworkFrameNumValid; ///< if Frame Number is Valid
} CHISOFMESSAGE;

/// @brief Message descriptor structure
typedef struct ChiMessageDescriptor
{
    CHIMESSAGETYPE messageType;            ///< Message type that determines what to use from the union below

    union
    {
        CHIERRORMESSAGE   errorMessage;    ///< Error message contents. Valid if messageType is Error
        CHISHUTTERMESSAGE shutterMessage;  ///< Shutter message contents. Valid if messageType is Shutter
        CHISOFMESSAGE     sofMessage;      ///< SOF message contents. Valid if messageType is SOF
        UINT8             generic[32];     ///< Generic message contents used to ensure a minimum size for custom message types
    } message;

    CHIPRIVDATA *pPrivData;
} CHIMESSAGEDESCRIPTOR;

/// @brief Callbacks used by the CHI implementation to call into the CHI app
typedef struct ChiCallBacks
{
    /// @brief CHI app callback method to send back the capture results
    void (*ChiProcessCaptureResult)(
        CHICAPTURERESULT*   pCaptureResult,
        VOID*               pPrivateCallbackData);

    /// @brief Asynchronous notification callback method
    void (*ChiNotify)(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData);

    /// @brief CHI app callback method to send back the partial capture results
    void (*ChiProcessPartialCaptureResult)(
         CHIPARTIALCAPTURERESULT* pCaptureResult,
         VOID* pPrivateCallbackData);

} CHICALLBACKS;

/// @brief Chi link buffer properties
typedef struct ChiLinkBufferProperties
{
    UINT32  bufferFormat;                       ///< Buffer format
    UINT32  bufferSize;                         ///< Buffer size (in case its a raw bytes buffer)
    UINT32  bufferImmediateAllocCount;          ///< Initial buffers will be allocated for the link
    UINT32  bufferQueueDepth;                   ///< Max buffers that will ever exist on the link
    UINT32  bufferHeap;                         ///< Buffer heap
    UINT32  bufferFlags;                        ///< Buffer flags
} CHILINKBUFFERPROPERTIES;

/// @brief Chi output port descriptor
typedef struct ChiOutputPortDescriptor
{
    UINT32  portId;                             ///< Input/Output port id
    UINT32  isSinkPort;                         ///< Sink port indicator
    UINT32  isOutputStreamBuffer;               ///< Does the port output a stream buffer
    UINT32  portSourceTypeId;                   ///< Port source type id
    UINT32  numSourceIdsMapped;                 ///< Number of sources mapped to this output port for bypass
    UINT32* pMappedSourceIds;                   ///< Source Ids mapped to this output port for bypass
} CHIOUTPUTPORTDESCRIPTOR;

/// @brief Chi input port descriptor
typedef struct ChiInputPortDescriptor
{
    UINT32  portId;                             ///< Input/Output port id
    UINT32  isInputStreamBuffer;                ///< Does this input port take a source buffer as input
    UINT32  portSourceTypeId;                   ///< Port source type id
} CHIINPUTPORTDESCRIPTOR;

/// @brief Chi node ports info
typedef struct ChiNodePorts
{
    UINT32                   numInputPorts;     ///< Number of input ports
    CHIINPUTPORTDESCRIPTOR*  pInputPorts;       ///< Pointer to input ports
    UINT32                   numOutputPorts;    ///< Number of output ports
    CHIOUTPUTPORTDESCRIPTOR* pOutputPorts;      ///< Pointer to output ports
} CHINODEPORTS;

/// @brief Chi node property
typedef struct ChiNodeProperty
{
    UINT32       id;                            ///< ID of the property
    const VOID*  pValue;                        ///< Pointer to data. Type dependent on ID
} CHINODEPROPERTY;

/// @brief Chi node info
typedef struct ChiNode
{
    CHINODEPROPERTY* pNodeProperties;           ///< Properties associated with the node
    UINT32           nodeId;                    ///< Node identifier
    UINT32           nodeInstanceId;            ///< Node instance identifier
    CHINODEPORTS     nodeAllPorts;              ///< Information about all ports
    UINT32           numProperties;             ///< Count of NodeProperty instances in pNodeProperties
} CHINODE;

/// @brief Information about a node in the link descriptor that defines connection between two nodes
typedef struct ChiLinkNodeDescriptor
{
    UINT32  nodeId;                              ///< Node identifier
    UINT32  nodeInstanceId;                      ///< Node instance id
    UINT32  nodePortId;                          ///< Node port id
    UINT32  portSourceTypeId;                    ///< Port source type id
} CHILINKNODEDESCRIPTOR;

/// @brief Chi link properties
typedef struct ChiLinkProperties
{
    UINT32 isBatchedMode;                       ///< Batched mode indicator
} CHILINKPROPERTIES;

/// @brief Structure that represents one link originating from a node
typedef struct ChiNodeLink
{
    CHILINKNODEDESCRIPTOR   srcNode;            ///< Src node in a link
    UINT32                  numDestNodes;       ///< Dest nodes in a link that the src node can be connected to
    CHILINKNODEDESCRIPTOR*  pDestNodes;         ///< Pointer to all the dest nodes connected to the src node
    CHILINKBUFFERPROPERTIES bufferProperties;   ///< Buffer properties
    CHILINKPROPERTIES       linkProperties;     ///< Link properties
} CHINODELINK;

/// @brief Structure that contains all information required to create a pipeline
typedef struct ChiPipelineCreateDescriptor
{
    UINT32              size;                   ///< Size of this structure
    UINT32              numNodes;               ///< Number of pipeline nodes
    CHINODE*            pNodes;                 ///< Pipeline nodes
    UINT32              numLinks;               ///< Number of links
    CHINODELINK*        pLinks;                 ///< Each link descriptor
    UINT32              isRealTime;             ///< Is this a realtime pipeline
    UINT                numBatchedFrames;       ///< Number of framework frames batched
    UINT                maxFPSValue;            ///< maxFPSValue info of batched frames
    UINT32              cameraId;               ///< Camera Id of pipeline
} CHIPIPELINECREATEDESCRIPTOR;

/// @brief Structure that contains information about a capture request
typedef struct ChiCaptureRequest
{
    UINT64            frameNumber;               ///< Frame number
    CHIPIPELINEHANDLE hPipelineHandle;           ///< Pipeline handle
    UINT32            numInputs;                 ///< Number of inputs
    CHISTREAMBUFFER*  pInputBuffers;             ///< List of input buffers
    UINT32            numOutputs;                ///< Number of outputs
    CHISTREAMBUFFER*  pOutputBuffers;            ///< List of output buffers
    const VOID*       pMetadata;                 ///< The metadata buffer that contains the capture and processing parameters
    CHIPRIVDATA*      pPrivData;                 ///< Pointer to private data. Managed by client.
} CHICAPTUREREQUEST;

/// @brief Structure that represents all requests submitted at once, possibly to different pipelines
typedef struct ChiPipelineRequest
{
    CHIHANDLE                pSessionHandle;     ///< Session handle
    UINT32                   numRequests;        ///< Number of pipeline requests
    const CHICAPTUREREQUEST* pCaptureRequests;   ///< Individual request structure array
} CHIPIPELINEREQUEST;

/// @brief Buffer dimension
typedef struct ChiBufferDimension
{
    UINT32 width;                               ///< Width
    UINT32 height;                              ///< Height
} CHIBUFFERDIMENSION;

/// @brief Buffer dimension options
typedef struct ChiBufferOptions
{
    UINT32             size;                    ///< Size of this structure
    CHIBUFFERDIMENSION minDimension;            ///< Min Dimension
    CHIBUFFERDIMENSION maxDimension;            ///< Max Dimension
    CHIBUFFERDIMENSION optimalDimension;        ///< Optimal Dimension
} CHIBUFFEROPTIONS;

/// @brief Structure to describe the buffer on a input/output port
typedef struct ChiPortBufferDescriptor
{
    UINT32                   size;                          ///< Size of this structure
    CHILINKNODEDESCRIPTOR    nodePort;                      ///< Node + single-port info
    CHISTREAM*               pStream;                       ///< Stream representing the buffer, which is also passed
                                                            ///  in pipeline request
    BOOL                     bIsOverrideImplDefinedWithRaw; ///< Select MIPI Raw for app implementation defined format
} CHIPORTBUFFERDESCRIPTOR;

/// @brief Structure to describe the metadata information from the pipeline
typedef struct ChiPipelineMetadataInfo
{
    const UINT32*   pPublishTagArray;           ///< Array of tags published by the pipeline
    UINT32          publishTagCount;            ///< Count of the tags published by the pipeline
    UINT32          maxNumMetaBuffers;          ///< Maximum number of metadata buffers required by the pipeline
    BOOL            bPublishPartialMetaData;    ///< If pipeline will publish partial Meta Data
} CHIPIPELINEMETADATAINFO;

/// @brief Rational
typedef struct ChiRational
{
    UINT32   numerator;                         ///< Supported with no perf impact
    UINT32   denominator;                       ///< HQ mode available with perf impact
} CHIRATIONAL;

/// @brief Sensor capability
typedef union ChiSensorModeCaps
{
    struct
    {
        BIT Normal   : 1;                       ///< Normal   mode
        BIT HFR      : 1;                       ///< HFR      mode
        BIT IHDR     : 1;                       ///< IHDR     mode
        BIT PDAF     : 1;                       ///< PDAF     mode
        BIT QuadCFA  : 1;                       ///< Quad CFA mode
        BIT ZZHDR    : 1;                       ///< ZZHDR    mode
        BIT DEPTH    : 1;                       ///< DEPTH    mode
        BIT SHDR     : 1;                       ///< SHDR     mode
        BIT reserved : 24;                      ///< Reserved
    }u;
    UINT32 value;
} CHISENSORMODECAPS;


/// @brief SensorStreamType Type of the stream: BLOB, IMAGE, PDAF, HDR, META
typedef enum ChiSensorStreamType
{
    StreamBlob  = 0,                            ///< Blob  stream
    StreamImage = 1,                            ///< Image stream
    StreamPDAF  = 2,                            ///< Pdaf  stream
    StreamHDR   = 3,                            ///< Hdr   stream
    StreamMeta  = 4,                            ///< Meta  stream
    StreamImage1 = 5,                           ///< Image stream
    StreamImage2 = 6,                           ///< Image stream
    StreamImage3 = 7                            ///< Image stream
} CHISENSORSTREAMTYPE;

/// @brief Structure containing width and height integer values, along with a start offset
typedef struct ChiRect
{
    UINT32      left;                           ///< x coordinate for top-left point
    UINT32      top;                            ///< y coordinate for top-left point
    UINT32      width;                          ///< width
    UINT32      height;                         ///< height
} CHIRECT;

/// @brief Structure containing width and height integer values
typedef struct ChiDimension
{
    UINT32      width;                          ///< width
    UINT32      height;                         ///< height
} CHIDIMENSION;

/// @brief Resource policy use to communicate use cases intent so the driver can make the right trade-off in optimizing power
///        and usage of HW resources.
typedef enum ChiResourcePolicy
{
    GreedyPowerOptimal = 0,    ///< Will try to use more HW resources to optimize for power if power criteria pass
                               ///  (which may lead to out-of-resource situation for multi-sensor sessions and use cases.)
    HWResourceMinimal          ///< Will limit the number of HW resources used for each stream to maxmize the number
                               ///  of simultaneous sensors (which implies each stream will be limited in resolution.)
} CHIRESOURCEPOLICY;

/// @brief Sensor mode information
typedef struct ChiSensorModeInfo
{
    UINT32               size;                  ///< Size of this structure
    UINT32               modeIndex;             ///< Mode index
    UINT32               arraySizeInMPix;       ///< Array size
    CHIRECT              frameDimension;        ///< Sensor output Frame dimension
    CHIRECT              cropInfo;              ///< Sensor crop info
    CHIRATIONAL          aspectRatio;           ///< Aspect ratio
    UINT32               bpp;                   ///< Bits per pixel
    UINT32               frameRate;             ///< Frame rate
    UINT32               batchedFrames;         ///< Batched frames
    CHISENSORMODECAPS    sensorModeCaps;        ///< sensor mode capabilities like HFR, Quadra etc
    CHISENSORSTREAMTYPE  streamtype;            ///< sensor stream type
    UINT32               frameLengthLines;      ///< Sensor Frame Length Line
    UINT32               lineLengthPixelClock;  ///< Sensor Line Length Pixel Clock
} CHISENSORMODEINFO;

///< Capabilities related to the device's imaging characteristics
typedef struct ChiSensorCaps
{
    BOOL                    isYUVCamera;        ///< Camera is a YUV camera
    UINT32                  size;               ///< Size of this structure
    CHIDIRECTION            facing;             ///< Direction the camera faces relative to device screen
    CHISENSORPOSITIONTYPE   positionType;       ///< main, aux camera info
    FLOAT                   pixelSize;          ///< Physical size of one pixel in micro meter
    CHIRECT                 activeArray;        ///< Sensor active pixel array
    VOID*                   pRawOTPData;        ///< Pointer to raw OTP data
    UINT32                  rawOTPDataSize;     ///< Size of raw OTP data
} CHISENSORCAPS;

/// @brief Capabilities related to the device's lens characteristics
typedef struct ChiLensCaps {
    UINT32      size;                           ///< Size of this structure
    FLOAT       focalLength;                    ///< Lens focal length
    BOOL        isFixedFocus;                   ///< flag to indicate is fixed focus or not
    FLOAT       horViewAngle;                   ///< Lens Horizontal view angle in degrees

} CHILENSCAPS;

/// @brief Structure to represent information about a single camera
typedef struct ChiCameraInfo
{
    UINT32           size;                      ///< Structure size
    UINT32           numSensorModes;            ///< Number of sensor modes
    VOID*            pLegacy;                   ///< Legacy
    CHISENSORCAPS    sensorCaps;                ///< Capabilities related to the device's imaging characteristics
    CHILENSCAPS      lensCaps;                  ///< Capabilities related to the device's lens characteristics
} CHICAMERAINFO;

/// @brief Input buffer info to be used during session create
typedef struct ChiInputBufferInfo
{
    UINT32                         numInputBuffers;         ///< Number of input buffers
    const CHIPORTBUFFERDESCRIPTOR* pInputBufferDescriptors; ///< Input buffer descriptors
} CHIINPUTBUFFERINFO;

/// @brief Info about the sensor used in a pipeline
typedef struct ChiSensorInfo
{
    UINT32                   cameraId;                      ///< Camera Id
    const CHISENSORMODEINFO* pSensorModeInfo;               ///< Sensor mode info
} CHISENSORINFO;

/// @brief Information about pipeline inputs
typedef struct ChiPipelineInputInfo
{
    UINT32             isInputSensor;                   ///< Is the pipeline input a sensor
    union
    {
        CHISENSORINFO      sensorInfo;                      ///< If pipeline input is sensor, use this structure
        CHIINPUTBUFFERINFO inputBufferInfo;                 ///< If pipeline input is not sensor, its input buffer(s)
    };

} CHIPIPELINEINPUTINFO;

/// @brief Pipeline output Dimension
typedef struct ChiPipelineOutputDimension
{
    CHIBUFFERDIMENSION      outputDimension;
    CHIPIPELINEOUTPUTTYPE   outputType;
} CHIPIPELINEOUTPUTDIMENSION;

/// @brief Pipeline output information
typedef struct ChiPipelineOutputInfo
{
    CHIPIPELINEHANDLE hPipelineHandle;                     ///< Pipeline handle that represents the created pipeline
} CHIPIPELINEOUTPUTINFO;

/// @brief Pipeline info used during session create
typedef struct ChiPipelineInfo
{
    CHIPIPELINEDESCRIPTOR hPipelineDescriptor;              ///< Pipeline descriptor
    CHIPIPELINEINPUTINFO  pipelineInputInfo;                ///< Pipeline input info
    CHIPIPELINEOUTPUTINFO pipelineOutputInfo;               ///< Pipeline output info
    UINT32                pipelineResourcePolicy;           ///< Resource policy (CHIRRESOURCEPOLICY)
    BOOL                  isDeferFinalizeNeeded;            ///< Flag to indicate if need defer pipeline finalization
} CHIPIPELINEINFO;

/// @brief Pipeline input options
typedef struct ChiPipelineInputOptions
{
    CHILINKNODEDESCRIPTOR nodePort;                         ///< Information about the input buffer node/port
    CHIBUFFEROPTIONS      bufferOptions;                    ///< Buffer requirement options
} CHIPIPELINEINPUTOPTIONS;

/// @brief pipeline deactivate modes bitmask
typedef UINT32 CHIDEACTIVATEPIPELINEMODE;
static const CHIDEACTIVATEPIPELINEMODE CHIDeactivateModeDefault       = (1 << 0); ///< Default mode, deactivate all nodes in
                                                                                  /// pipeline
static const CHIDEACTIVATEPIPELINEMODE CHIDeactivateModeSensorStandby = (1 << 1); ///< Sensor stand by mode
static const CHIDEACTIVATEPIPELINEMODE CHIDeactivateModeReleaseBuffer = (1 << 2); ///< Release cslalloc buffers of all nodes in
                                                                                  ///  pipeline

/// @brief Session creation flags
typedef union ChiSessionFlags
{
    struct
    {
        BIT Deprecated          : 1;    ///< Do not use
        BIT SupportsPartialMeta : 1;    ///< TRUE if the session can handle a partial metadata callback
        BIT isNativeChi         : 1;    ///< To indicate if the session is related to native Chi
        BIT reserved            : 29;   ///< Reserved
    } u;
    UINT32 value;
} CHISESSIONFLAGS;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiQueryVendorTagLocation
///
/// @brief  Query vendor tag location assigned by vendor tag manager
///
/// @return CDKResultSuccess if successful otherwise CDKResultNoSuch
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIQUERYVENDORTAGLOCATION)(
    const CHAR* pSectionName,
    const CHAR* pTagName,
    UINT32*     pTagLocation);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSetMetaData
///
/// @brief  Set meta data by tag
///
/// @param  [in] metaHandle meta data handle
/// @param  [in] tag        tag need to be set
/// @param  [in] pData      pointer to data buffer
/// @param  [in] count      size of data buffer in byte
///
/// @return Result status
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PSETMETADATA)(
   CHIHANDLE    metaHandle,
   UINT32       tag,
   VOID*        pData,
   SIZE_T       count);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGetMetaData
///
/// @brief  Get meta data by tag
///
/// @param  [in] metaHandle meta data handle
/// @param  [in] tag        tag need to be set
/// @param  [in] pData      pointer to buffer to receive data
/// @param  [in] count      size of data buffer in byte
///
/// @return Result status
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PGETMETADATA)(
   CHIHANDLE    metaHandle,
   UINT32       tag,
   VOID*        pData,
   SIZE_T       count);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiVendorTagOps
///
/// @brief  This structure contains basic functions for Vendor Tag IDs
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ChiVendorTagsOps
{
    PFNCHIQUERYVENDORTAGLOCATION    pQueryVendorTagLocation;    ///< Get Vendor Tag Location
    PSETMETADATA                    pSetMetaData;               ///< Set meta data by tag id
    PGETMETADATA                    pGetMetaData;               ///< Get meta data by tag id
    VOID*                           reserved[8];                ///< Reserved for future use. These must be initialized to NULL.
} CHITAGSOPS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGetTagOps
///
/// @brief  Gets the tag related function pointers
///
/// @param  [in][out] CHITAGSOPS
///
/// @return None
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFNGETTAGOPS)(CHITAGSOPS*);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiOpenContext
///
/// @brief  Creates and returns a CHI context
///
/// @return Chi context pointer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CHIHANDLE (*PFNCHIOPENCONTEXT)();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiCloseContext
///
/// @brief  Close the CHI context
///
/// @param  [in] hChiContext    CHI Context
///
/// @return None
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFNCHICLOSECONTEXT)(CHIHANDLE hChiContext);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGetNumCameras
///
/// @brief  Get the number of cameras in the platform
///
/// This function is typically called at the camera service initialization
///
/// @return Number of camera devices in the platform
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef UINT (*PFNCHIGETNUMCAMERAS)(CHIHANDLE hChiContext);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiGetCameraInfo
///
/// @brief  Get the information about a specific camera
///
/// @param  [in]  hChiContext   CHI Context
/// @param  [in]  cameraId      Camera identifier
/// @param  [out] pCameraInfo   Pointer to the camera info structure
///
/// @return Success or failure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIGETCAMERAINFO)(
    CHIHANDLE      hChiContext,
    UINT32         cameraId,
    CHICAMERAINFO* pCameraInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiEnumerateSensorModes
///
/// @brief  Get all the modes information about a specific sensor
///
/// @param  [in]  hChiContext      CHI Context
/// @param  [in]  cameraId         Camera identifier
/// @param  [in]  numSensorModes   Number of sensor modes
/// @param  [in]  pSensorModeInfo  Array of modes with numModes elements in the array filled in by the driver
///
/// @return Success or failure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIENUMERATESENSORMODES)(
    CHIHANDLE          hChiContext,
    UINT32             cameraId,
    UINT32             numSensorModes,
    CHISENSORMODEINFO* pSensorModeInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiCreatePipelineDescriptor
///
/// @brief  Creates a pipeline for use by either a live or offline processing session
///
/// This function reads a pipeline descriptor, validates the pipeline contained within the descriptor, and then returns
/// a handle to the internal driver representation of the pipeline descriptor. The actual pipeline containing all the nodes
/// is created using this pipeline descriptor only when the session is created. This function can be called at any time
/// after the context is created.
///
/// @param  [in]  hChiContext               CHI Context
/// @param  [in]  pDescriptor               Pointer to a descriptor that contains all the necessary information to define a
///                                         processing pipeline.
/// @param  [in]  numOutputs                Number of outputs in the pipeline
/// @param  [in]  pOutputBufferDescriptors  Each output descriptor
/// @param  [in]  numInputs                 Number of input requirements for the pipeline
/// @param  [out] pInputBufferOptions       Each input buffer descriptor
///
/// @return A handle to the created pipeline descriptor, or 0 if it fails
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CHIPIPELINEDESCRIPTOR (*PFNCHICREATEPIPELINEDESCRIPTOR)(
    CHIHANDLE                          hChiContext,
    const CHIPIPELINECREATEDESCRIPTOR* pDescriptor,
    UINT32                             numOutputs,
    CHIPORTBUFFERDESCRIPTOR*           pOutputBufferDescriptors,
    UINT32                             numInputs,
    CHIPIPELINEINPUTOPTIONS*           pInputBufferOptions);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDestroyPipelineDescriptor
///
/// @brief  Destroys a pipeline descriptor
///
/// @param  [in] hChiContext            CHI Context
/// @param  [in] hPipelineDescriptor    Pipeline descriptor
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFNCHIDESTROYPIPELINEDESCRIPTOR)(
    CHIHANDLE             hChiContext,
    CHIPIPELINEDESCRIPTOR hPipelineDescriptor);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiCreateSession
///
/// @brief  Creates a camera session
///
/// This function creates a session that are independent processing units containing one or multiple pipelines. The
/// pipeline(s) for these processing sessions may get input data from memory(offline) or through streaming(realtime). This
/// function returns a handle, which is used by the driver to identify the processing session.
///
/// @param  [in] hChiContext            CHI Context
/// @param  [in] numPipelines           Number of pipelines associated with this session
/// @param  [in] pPipelineInfo          Pointer to array of structures describing the pipeline info
/// @param  [in] pCallbacks             Pointer to callback provided by the app to return session result/messages
/// @param  [in] pPrivateCallbackData   Private data passed back to the app as part of the callbacks
/// @param  [in] flags                  Session create flags
///
/// @return A handle to the processing session
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CHIHANDLE (*PFNCHICREATESESSION)(
    CHIHANDLE        hChiContext,
    UINT             numPipelines,
    CHIPIPELINEINFO* pPipelineInfo,
    CHICALLBACKS*    pCallbacks,
    VOID*            pPrivateCallbackData,
    CHISESSIONFLAGS  flags);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiActivatePipeline
///
/// @brief  Activate a pipeline in a session
///
/// This function will activate a pipeline in the session after which request submission to the pipeline can begin
///
/// @param  [in] hChiContext    CHI Context
/// @param  [in] hSession       Handle to session
/// @param  [in] pipeline       Pipeline (handle) to activate
/// @param  [in] pModeInfo      New sensor mode info
///
/// @return Success or failure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIACTIVATEPIPELINE)(
    CHIHANDLE          hChiContext,
    CHIHANDLE          hSession,
    CHIHANDLE          pipeline,
    CHISENSORMODEINFO* pSensorModeInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDeactivatePipeline
///
/// @brief  Deactivates a pipeline after which no request can be submitted to the pipeline
///
/// @param  [in] hChiContext    CHI Context
/// @param  [in] hSession       Handle to session
/// @param  [in] pipeline       Pipeline (handle) to deactivate
/// @param  [in] mode           Deactivate mode
///
/// @return Success or failure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIDEACTIVATEPIPELINE)(
    CHIHANDLE                   hChiContext,
    CHIHANDLE                   hSession,
    CHIHANDLE                   pipeline,
    CHIDEACTIVATEPIPELINEMODE   mode);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIQUERYPIPELINEMETADATAINFO
///
/// @brief  Queries the pipeline specific metadata information
///
/// @param  [in]  hPipeline              CHI Pipeline handle
/// @param  [out] pPipelineMetadataInfo  Pointer to the structure containing the pipeline metadata information which
///                                      must be filled by Camx
///
/// @return Success or failure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNCHIQUERYPIPELINEMETADATAINFO)(
    CHIPIPELINEHANDLE        hPipeline,
    CHIPIPELINEMETADATAINFO* pPipelineMetadataInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDestroySession
///
/// @brief  Destroys an camera session.
///
/// @param  [in] hChiContext    CHI Context
/// @param  [in] session        Session handle
///
/// @return None.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFNCHIDESTROYSESSION)(
    CHIHANDLE hChiContext,
    CHIHANDLE session,
    BOOL isForced);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiFlushSession
///
/// @brief  Flush a camera session.
///
/// @param  [in] hChiContext    CHI Context
/// @param  [in] session        Session handle
///
/// @return None.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFNCHIFLUSHSESSION)(
    CHIHANDLE hChiContext,
    CHIHANDLE session,
    BOOL      isForced);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSubmitPipelineRequest
///
/// @brief  Submits a request to (multiple) instantiated live or offline pipelines
///
/// @param  [in] hChiContext    CHI Context
/// @param  [in] pRequest       Handle of the offline processing session.
///
/// @return ChiResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHISUBMITPIPELINEREQUEST)(
    CHIHANDLE           hChiContext,
    CHIPIPELINEREQUEST* pRequest);

/// @brief CHI Context
typedef struct ChiContextOps
{
    UINT32                          size;                           ///< Size of this structure
    UINT32                          majorVersion;                   ///< Major version of the interface
    UINT32                          minorVersion;                   ///< Minor version of the interface
    PFNCHIOPENCONTEXT               pOpenContext;                   ///< OpenContext function
    PFNCHICLOSECONTEXT              pCloseContext;                  ///< CloseContext function
    PFNCHIGETNUMCAMERAS             pGetNumCameras;                 ///< Get number of cameras
    PFNCHIGETCAMERAINFO             pGetCameraInfo;                 ///< Get camera info of a single camera
    PFNCHIENUMERATESENSORMODES      pEnumerateSensorModes;          ///< Enumerate the sensor modes
    PFNCHICREATEPIPELINEDESCRIPTOR  pCreatePipelineDescriptor;      ///< Create a pipeline descriptor
    PFNCHIDESTROYPIPELINEDESCRIPTOR pDestroyPipelineDescriptor;     ///< Destroy a pipeline descriptor
    PFNCHICREATESESSION             pCreateSession;                 ///< Create a session
    PFNCHIDESTROYSESSION            pDestroySession;                ///< Destroy a session
    PFNCHIACTIVATEPIPELINE          pActivatePipeline;              ///< Activate a pipeline contained in a session
    PFNCHIDEACTIVATEPIPELINE        pDeactivatePipeline;            ///< Deactivate a pipeline contained in a session
    PFNCHISUBMITPIPELINEREQUEST     pSubmitPipelineRequest;         ///< Submit a pipeline request
    PFNGETTAGOPS                    pTagOps;                        ///< Vendor Tag Operation
    PFNCHIFLUSHSESSION              pFlushSession;                  ///< Flush session
    PFNCHIQUERYPIPELINEMETADATAINFO pQueryPipelineMetadataInfo;     ///< Queries metadata information specific to pipeline
} CHICONTEXTOPS;

/// @brief Sensor PDAF Pixel Coordinates information
typedef struct ChiPDAFPixelCoordinates
{
    UINT32  PDXCoordinate;  ///< PDAF X-Coordinate
    UINT32  PDYCoordinate;  ///< PDAF Y-Coordinate
} CHIPDAFPIXELCOORDINATES;

/// @brief Sensor PDAF information
typedef struct ChiSensorPDAFInfo
{
    UINT32                  PDAFBlockWidth;       ///< PDAF block width
    UINT32                  PDAFBlockHeight;      ///< PDAF block height
    UINT32                  PDAFGlobaloffsetX;    ///< PDAF offset X
    UINT32                  PDAFGlobaloffsetY;    ///< PDAF offset Y
    UINT32                  PDAFPixelCount;       ///< PDAF Pixel count
    ChiPDAFPixelCoordinates PDAFPixelCoords[256]; ///< PDAF Pixel Coordinates
} CHISENSORPDAFINFO;

/// @brief AEC data
typedef struct ChiAECData
{
    UINT64  exposureTime;   ///< Exposure time in nanoseconds
    INT32   sensitivity;    ///< Sensor sensitivity
} CHIAECDATA;

/// @brief Timestamp info
typedef struct ChiTimestampInfo
{
     UINT64 timestamp;
     UINT64 frameId;
} CHITIMESTAMPINFO;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiEntry
///
/// @brief  Main entry point of the CHI driver
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PCHIENTRY)(CHICONTEXTOPS* pContextOps);

CDK_VISIBILITY_PUBLIC VOID ChiEntry(CHICONTEXTOPS* pContextOps);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDumpState
///
/// @brief  Post-mortem dump function.  Used by tools.  Not intended for use in Chi applications.
///
/// @param  fd    file descriptor in which to write the dump
///
/// @note  Once Chi and below is in a separate .so, this should be a public entry function (like ChiEntry)
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDumpState(INT fd);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHI_H
