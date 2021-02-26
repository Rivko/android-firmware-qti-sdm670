////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chinode.h
/// @brief Defines Chi Node interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHINODE_H
#define CHINODE_H

#include "camxcdktypes.h"
#include "chicommon.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Pack data structures to ensure consistent layout.
#pragma pack(push, 8)

static const UINT32 ChiNodeMaxProperties      = 64;
static const UINT32 ChiNodeMaxDependentFences = 8;

static const UINT32 ChiNodeFormatsMaxPlanes = 3;
static const UINT32 ChiNodeMaxImagesInBatch = 8;

static const UINT32 ChiNodeCapsScale = 0x1;
static const UINT32 ChiNodeCapsGpuMemcpy       = 1 << 1;               ///< Simply copy the image
static const UINT32 ChiNodeCapsGPUGrayscale    = 1 << 2;               ///< Support color --> Grayscale
static const UINT32 ChiNodeCapsGPURotate       = 1 << 3;               ///< Support rotation
static const UINT32 ChiNodeCapsGPUDownscale    = 1 << 4;               ///< Support downscale
static const UINT32 ChiNodeCapsGPUFlip         = 1 << 5;               ///< Support Horizontal or Vertical Flip
static const UINT32 ChiNodeCapsParallelReq     = 1 << 6;               ///< Support Horizontal or Vertical Flip
static const UINT32 ChiNodeCapsSHDR            = 1 << 7;               ///< For SHDR mode
static const UINT32 ChiNodeCapsEISDG           = 1 << 8;               ///< For Only EIS DG
static const UINT32 ChiNodeCapsDefog           = 1 << 9;               ///< For Defog mode
static const UINT32 ChiNodeCapsEISDGLDC        = 1 << 10;              ///< For EISDG and LDC
static const UINT32 ChiNodeCapsSHDR3           = 1 << 11;              ///< For YUV SHDR mode
static const UINT32 ChiNodeCapsTwoFrameSplit   = 1 << 12;              ///< For Split Node
static const UINT32 ChiNodeCapsThreeFrameSplit = 1 << 13;              ///< For Split Node

typedef VOID*          CHIHANDLE;
typedef VOID*          CHIMETADATAHANDLE;
typedef VOID*          CHIDATASOURCECAPS;
typedef VOID*          CHIDATAREQUESTHANDLE;
typedef VOID*          CHIDATAHANDLE;

/// @brief This enumerates pixel formats.
typedef enum ChiFormat
{
    JPEG,            ///< JPEG format.
    Y8,              ///< Luma only, 8 bits per pixel.
    Y16,             ///< Luma only, 16 bits per pixel.
    YUV420NV12,      ///< YUV 420 format as described by the NV12 fourcc.
    YUV420NV21,      ///< YUV 420 format as described by the NV21 fourcc.
    YUV422NV16,      ///< YUV 422 format as described by the NV16 fourcc
    Blob,            ///< Any non image data
    RawYUV8BIT,      ///< Packed YUV/YVU raw format. 16 bpp: 8 bits Y and 8 bits UV. U and V are interleaved as YUYV or YVYV.
    RawPrivate,      ///< Private RAW formats where data is packed into 64bit word.
                     ///  8BPP:  64-bit word contains 8 pixels p0-p7, p0 is stored at LSB.
                     ///  10BPP: 64-bit word contains 6 pixels p0-p5, most significant 4 bits are set to 0. P0 is stored at LSB.
                     ///  12BPP: 64-bit word contains 5 pixels p0-p4, most significant 4 bits are set to 0. P0 is stored at LSB.
                     ///  14BPP: 64-bit word contains 4 pixels p0-p3, most significant 8 bits are set to 0. P0 is stored at LSB.
    RawMIPI,         ///< MIPI RAW formats based on MIPI CSI-2 specification.
                     ///  8BPP: Each pixel occupies one bytes, starting at LSB. Output width of image has no restrictions.
                     ///  10BPP: 4 pixels are held in every 5 bytes. The output width of image must be a multiple of 4 pixels.
                     ///  12BPP: 2 pixels are held in every 3 bytes. The output width of image must be a multiple of 2 pixels.
                     ///  14BPP: 4 pixels are held in every 7 bytes. The output width of image must be a multiple of 4 pixels.
    RawPlain16,      ///< Plain16 RAW format. Single pixel is packed into two bytes, little endian format. Not all bits may be
                     ///  used as RAW data is generally 8, 10, or 12 bits per pixel. Lower order bits are filled first.
    RawMeta8BIT,     ///< Generic 8-bit raw meta data for internal camera usage.
    UBWCTP10,        ///< UBWC TP10 format (as per UBWC2.0 design specification)
    UBWCNV12,        ///< UBWC NV12 format (as per UBWC2.0 design specification)
    UBWCNV124R,      ///< UBWC NV12-4R format (as per UBWC2.0 design specification)
    YUV420NV12TP10,  ///< YUV 420 format 10bits per comp tight packed format.
    YUV420NV21TP10,  ///< YUV 420 format 10bits per comp tight packed format.
    YUV422NV16TP10,  ///< YUV 422 format 10bits per comp tight packed format.
    PD10,            ///< PD10 format
    RawMIPI8,        ///< 8BPP: Each pixel occupies one bytes, starting at LSB. Output width of image has no restrictions.
    FDY8,            ///< Luma only, 8 bits per pixel for FD
    FDYUV420NV12,    ///< YUV 420 format as described by the NV12 fourcc for FD.
    P010             ///< P010 format
} ChiFormat;

/// @brief This enumerates degrees of rotation in a clockwise direction. The specific variable or struct member must declare the
/// semantics of the rotation (e.g. the image HAS BEEN rotated or MUST BE rotated).
typedef enum  ChiRotation
{
    CW0Degrees,     ///< Zero degree rotation.
    CW90Degrees,    ///< 90 degree clockwise rotation.
    CW180Degrees,   ///< 180 degree clockwise rotation.
    CW270Degrees    ///< 270 degree clockwise rotation.
} ChiRotation;

/// Enumeration of the color filter pattern for RAW outputs
typedef enum ChiColorFilterPattern
{
    Y,      ///< Monochrome pixel pattern.
    YUYV,   ///< YUYV pixel pattern.
    YVYU,   ///< YVYU pixel pattern.
    UYVY,   ///< UYVY pixel pattern.
    VYUY,   ///< VYUY pixel pattern.
    RGGB,   ///< RGGB pixel pattern.
    GRBG,   ///< GRBG pixel pattern.
    GBRG,   ///< GBRG pixel pattern.
    BGGR,   ///< BGGR pixel pattern.
    RGB     ///< RGB pixel pattern.
} ChiColorFilterPattern;

/// @brief This enumerates color space specifications
typedef enum ChiColorSpace
{
    Unknown,    ///< Default-assumption data space, when not explicitly specified.
    BT601Full,  ///< ITU-R Recommendation 601 (BT.601) - Full range.
    BT601625,   ///< ITU-R Recommendation 601 (BT.601) - 625-line  SDTV, 625 Lines (PAL).
    BT601525,   ///< ITU-R Recommendation 601 (BT.601) - 525-line SDTV, 525 Lines (NTSC).
    BT709,      ///< ITU-R Recommendation 709 (BT.709) HDTV.
    Depth       ///< The buffer contains depth ranging measurements from a depth camera per HAL definition.
} ChiColorSpace;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure defines the raw format.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ChiRawFormat
{
    INT32                  bitsPerPixel;       ///< Bits per pixel.
    UINT32                 stride;             ///< Stride in bytes.
    UINT32                 sliceHeight;        ///< The number of lines in the plane which can be equal to or larger than actual
                                               ///  frame height.
    ChiColorFilterPattern  colorFilterPattern; ///< Color filter pattern of the RAW format.
} CHIRAWFORMAT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure defines the YUV formats.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ChiYUVFormat
{
    UINT32 width;           ///< Width of the YUV plane in pixels.
                            ///  Tile aligned width in bytes for UBWC
    UINT32 height;          ///< Height of the YUV plane in pixels.
    UINT32 planeStride;     ///< The number of bytes between the first byte of two sequential lines on plane 1. It may be
                            ///  greater than nWidth * nDepth / 8 if the line includes padding.
                            ///  Macro-tile width aligned for UBWC
    UINT32 sliceHeight;     ///< The number of lines in the plane which can be equal to or larger than actual frame height.
                            ///  Tile height aligned for UBWC

    UINT32 metadataStride;  ///< Aligned meta data plane stride in bytes, used for UBWC formats
    UINT32 metadataHeight;  ///< Aligned meta data plane height in bytes, used for UBWC formats
    UINT32 metadataSize;    ///< Aligned metadata plane size in bytes, used for UBWC formats
    SIZE_T pixelPlaneSize;  ///< Aligned pixel plane size in bytes, calculated once for UBWC formats
                            ///< and stored thereafter, since the calculations are expensive
} CHIYUVFORMAT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure defines the JPEG format.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CHIJPEGFormat
{
    UINT32              maxJPEGSizeInBytes;       ///< Size of JPEG for max resolution in Bytes.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This union contains parameters only specified in certain output format, YUV, raw or Jpeg.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
union ChiFormatParams
{
    CHIYUVFORMAT  yuvFormat[ChiNodeFormatsMaxPlanes];    ///< YUV format specific properties.
    CHIRAWFORMAT  rawFormat;                             ///< RAW format specific properties.
    CHIJPEGFormat jpegFormat;                            ///< JPEG format specific properties.
};

typedef struct ChiAlignmentInfo
{
    UINT32 strideAlignment;      ///< Stride alignment
    UINT32 scanlineAlignment;    ///< Scanline alignment
} CHIALIGNMENTINFO;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the format of an image
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ChiImageFormat
{
    UINT32           width;                                   ///< Width of the image in pixels.
    UINT32           height;                                  ///< Height of the image in pixels.
    ChiFormat        format;                                  ///< Format of the image.
    ChiColorSpace    colorSpace;                              ///< Color space of the image.
    ChiRotation      rotation;                                ///< Rotation applied to the image.
    ChiFormatParams  formatParams;                            ///< Format specific definitions.
    SIZE_T           alignment;                               ///< The required alignment in bytes of the starting
                                                              ///  address of the allocated buffer with this image format
    CHIALIGNMENTINFO planeAlignment[ChiNodeFormatsMaxPlanes]; ///< Stride and scanline alignment for each plane
} CHIIMAGEFORMAT;


typedef struct ChiImage
{
    UINT32           size;                                   ///< Size of the structure
    INT              fd[ChiNodeFormatsMaxPlanes];            ///< File descriptor, used by node to map the buffer to FD
    UCHAR*           pAddr[ChiNodeFormatsMaxPlanes];         ///< Starting virtual address of the allocated buffer.
    CHIHANDLE        pNativeHandle;                          ///< The pointer to native_handle_t
} CHIIMAGE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the master structure for a single image buffer from CamX, in batch mode, the single image buffer may
///        contain multiple actual images(ChiImage)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ChiImageList
{
    UINT32          size;                                   ///< size of the structure
    CHIIMAGEFORMAT  format;                                 ///< The buffer format.
    UINT            numberOfPlanes;                         ///< The number of planes in this image type
    SIZE_T          planeSize[ChiNodeFormatsMaxPlanes];     ///< Per plane size.
    SIZE_T          metadataSize[ChiNodeFormatsMaxPlanes];  ///< Metadata Size
    UINT32          imageCount;                             ///< The count of image in the pImageList
    CHIIMAGE        pImageList[ChiNodeMaxImagesInBatch];    ///< Array of ChiImage
    UINT32          portId;                                 ///< Port for the image
    INT32           handles[ChiNodeFormatsMaxPlanes];       ///< CSL memory handle of buffers
    VOID*           pfenceHandle;                           ///< Handle to image buffer fence.
                                                            ///< If NULL, do not need to set dependency,
                                                            ///< will be NULL for output buffers
    VOID*           pIsFenceSignaled;                       ///< Pointer to fence variable whose signaled status is checked,
                                                            ///< is valid only when fenceHandle in not NULL
} CHIIMAGELIST;

typedef CHIIMAGELIST* CHINODEBUFFERHANDLE;

typedef struct ChiNodeCallbacks CHINODECALLBACKS;
typedef struct ChiNodeInterface CHINODEINTERFACE;

/// @brief Node capabilities information.
typedef struct ChiNodeCapsInfo
{
    UINT32  size;           ///< Size of this structure
    UINT32  nodeCapsMask;   ///< Node capabilities mask
    UINT32  width;          ///< Node output width
    UINT32  height;         ///< Node output height
} CHINODECAPSINFO;

/// @brief Node flags
union CHINODEFLAGS
{
    struct
    {
        BIT isInplace                    : 1;      ///< Is inplace node i.e. output buffer same as input buffer
        BIT isRealTime                   : 1;      ///< Is this node part of a real time pipeline
        BIT isSecureMode                 : 1;      ///< Is this node  Secure/non-secure
        BIT isBypassable                 : 1;      ///< Is this node bypassable
        BIT canDRQPreemptOnStopRecording : 1;      ///< Can DRQ preempt node on recording stop/pause
        BIT canSetInputBufferDependency  : 1;      ///< Chi node can set input buffer depencency
        BIT reserved                     : 26;     ///< Reserved
    };

    UINT value;
};

/// @brief Node create information.
typedef struct ChiNodeCreateInfo
{
    UINT32          size;             ///< Size of this structure
    CHIHANDLE       hChiSession;      ///< Chi driver handle that node can use to callback into Chi
    CHIHANDLE*      phNodeSession;    ///< Handle of node that Chi driver will provide back when it calls into node
    UINT32          nodeId;           ///< Node id
    CHINODECAPSINFO nodeCaps;         ///< Capabilities of the node which are needed to be enabled for this node instance
    CHINODEFLAGS    nodeFlags;        ///< Node flags
    UINT32          nodeInstanceId;   ///< Node instance id
} CHINODECREATEINFO;

/// @brief Node destroy information.
typedef struct ChiNodedestroyInfo
{
    UINT32      size;           ///< Size of this structure
    CHIHANDLE   hNodeSession;    ///< Handle of node for current node instance
} CHINODEDESTROYINFO;

/// @brief Node stream on information.
typedef struct ChiNodeOnStreamOnInfo
{
    UINT32      size;            ///< Size of this structure
    CHIHANDLE   hNodeSession;    ///< Handle of node for current node instance
} CHINODEONSTREAMONINFO;

/// @brief Node stream off information.
typedef struct ChiNodeOnStreamOffInfo
{
    UINT32      size;            ///< Size of this structure
    CHIHANDLE   hNodeSession;    ///< Handle of node for current node instance
} CHINODEONSTREAMOFFINFO;

/// @brief Buffer requirement information.
typedef struct ChiNodeBufferRequirement
{
    UINT32           minW;        ///< Min width that the node requires to generate its final output buffer
    UINT32           minH;        ///< Min height that the node requires to generate its final output buffer
    UINT32           maxW;        ///< Max width that the node requires to generate its final output buffer
    UINT32           maxH;        ///< Max height that the node requires to generate its final output buffer
    UINT32           optimalW;    ///< Optimal width that the node would want on its input buffer
    UINT32           optimalH;    ///< Optimal height that the node would want on its input buffer
    ChiFormat        format;      ///< Buffer format
    CHIALIGNMENTINFO planeAlignment[ChiNodeFormatsMaxPlanes]; ///< Stride and scanline alignment for each plane
} CHINODEBUFFERREQUIREMENT;

/// @brief Image format information.
typedef struct ChiNodeImageFormat
{
    UINT32  width;  ///< Width of the image in pixels
    UINT32  height; ///< Height of the image in pixels
} CHINODEIMAGEFORMAT;

/// @brief Output port buffer info
typedef struct ChiOutputPortQueryBufferInfo
{
    UINT                            outputPortId;               ///< Output port Id
    UINT32                          numConnectedInputPorts;     ///< Number of input ports connected to this output port
    CHINODEBUFFERREQUIREMENT*       pBufferRequirement;         ///< Buffer requirement requested by the input ports
                                                                ///  connected to this outputPort Id
    CHINODEBUFFERREQUIREMENT        outputBufferOption;         ///< Buffer options for this output port returned by the node
} CHIOUTPUTPORTQUERYBUFFERINFO;

/// @brief Input port buffer info
typedef struct ChiInputPortQueryBufferInfo
{
    UINT                       inputPortId;                     ///< Input port Id
    CHINODEBUFFERREQUIREMENT   inputBufferOption;               ///< Buffer option for this input port
} CHIINPUTPORTQUERYBUFFERINFO;

/// @brief Input buffer requirement query information.
typedef struct ChiNodeQueryBufferInfo
{
    UINT32                          size;                       ///< Size of this structure
    CHIHANDLE                       hNodeSession;               ///< Handle of node for current node instance
    UINT32                          numOutputPorts;             ///< Number of output ports
    CHIOUTPUTPORTQUERYBUFFERINFO*   pOutputPortQueryInfo;       ///< Output port buffer query info
    UINT32                          numInputPorts;              ///< Number of input ports
    CHIINPUTPORTQUERYBUFFERINFO*    pInputOptions;              ///< Format and resolution requirement for the input ports
} CHINODEQUERYBUFFERINFO;

/// @brief Buffer properties information.
typedef struct ChiNodeSetBufferPropertiesInfo
{
    UINT32              size;            ///< Size of this structure
    CHIHANDLE           hNodeSession;    ///< Handle of node for current node instance
    UINT32              portId;          ///< Port ID of current node instance
    CHINODEIMAGEFORMAT* pFormat;         ///< Format and resolution for the port
} CHINODESETBUFFERPROPERTIESINFO;

/// @brief Dependency information.
typedef struct ChiDependencyInfo
{
    UINT32         size;                                  ///< Size of this structure
    CHIHANDLE      hNodeSession;                          ///< Handle of node for current node instance
    UINT32         count;                                 ///< Number of properties in this unit
    UINT32         properties[ChiNodeMaxProperties];      ///< Property dependencies in this unit
    UINT64         offsets[ChiNodeMaxProperties];         ///< Offset from current request for dependency
    INT32          processSequenceId;                     ///< Identifier for the node to track its processing order
                                                          ///< -2, -1, and 0 reserved
    CHIFENCEHANDLE pChiFences[ChiNodeMaxDependentFences]; ///< Chi fences to depend on
    UINT32         chiFenceCount;                         ///< Number of chi fences to depend on
    BOOL           negate[ChiNodeMaxProperties];          ///< Flag to determine if the offset should be negated
    VOID*          pInputBufferFence[ChiNodeMaxDependentFences];            ///< Input buffer fences
    VOID*          pInputBufferFenceIsSignaled[ChiNodeMaxDependentFences];  ///< Input buffer fences signaled flag
    UINT32         inputBufferFenceCount;                                   ///< Input buffer fence count
    BOOL           sequentialExecutionNeeded;                               ///< Flag to indicate if sequential execution
                                                                            ///< is needed
    BOOL           satisfySequentialExecutionDependency;                    ///< Flag to indicate that wrapper needs to
                                                                            ///< satisfy sequential execution dependency
} CHIDEPENDENCYINFO;

/// @brief bypass information.
typedef struct ChiBypassInfo
{
    UINT32      size;                               ///< Size of this structure
    BOOL        isBypassNeeded;                     ///< Flag to indicate whether bypass is needed or not
    UINT32      selectedInputPortIndex;             ///< Index of the selected port for bypass
} CHINODEBYPASSINFO;

/// @brief Process capture request information.
typedef struct ChiNodeProcessRequestInfo
{
    UINT32               size;                  ///< Size of this structure
    CHIHANDLE            hNodeSession;          ///< Handle of node for current node instance
    UINT64               frameNum;              ///< Frame number for current request
    CHIDEPENDENCYINFO*   pDependency;           ///< Dependency for this request
    UINT32               inputNum;              ///< Number of input buffer handles
    CHINODEBUFFERHANDLE* phInputBuffer;         ///< Pointer to array of input buffer handles
    UINT32               outputNum;             ///< Number of output buffer
    CHINODEBUFFERHANDLE* phOutputBuffer;        ///< Pointer to array of output buffer handles
    CHINODEBYPASSINFO*   pBypassData;           ///< Pointer to the bypass info for output ports
    CHIFENCEHANDLE       doneFence;             ///< If valid, will signal when the effects of processing are visible
    BOOL                 isDelayedRequestDone;  ///< This should be set by the custom node only if it wants to delay the
                                                ///  request done notification post ProcessRequest() call from the wrapper
    BOOL                 isEarlyMetadataDone;   ///< flag indicating if metadata done is already called
} CHINODEPROCESSREQUESTINFO;

/// @brief Process capture request done information.
typedef struct ChiNodeProcessRequestDoneInfo
{
    UINT32      size;                   ///< Size of this structure
    CHIHANDLE   hChiSession;            ///< Chi driver handle that node can use to callback into Chi
    UINT64      frameNum;               ///< Frame number for current request
    INT32       result;                 ///< Result of process capture request
    BOOL        isEarlyMetadataDone;    ///< flag indicating if metadata done is already called
} CHINODEPROCESSREQUESTDONEINFO;

/// @brief Process metadata done information.
typedef struct ChiNodeProcessMetadataDoneInfo
{
    UINT32      size;                   ///< Size of this structure
    CHIHANDLE   hChiSession;            ///< Chi driver handle that node can use to callback into Chi
    UINT64      frameNum;               ///< Frame number for current request
    INT32       result;                 ///< Result of process capture request
} CHINODEPROCESSMETADATADONEINFO;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNNODEGETCAPS
///
/// @brief  Get the capabilities supported by the node.
///
/// This function is used to get all the capabilities supported by the node. This information may be used to verify the
/// node properties that are set during node creation, is supported by the node.
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNNODEGETCAPS)(CHINODECAPSINFO* pCapsInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNNODECREATE
///
/// @brief  Creates a node instance.
///
/// This function is called at the start of a camera session during chi_create_pipeline() when a topology is created.
/// At this time, the node can allocate any additional resources that it might need to manage per-session states.
/// Multiple independent sessions might be active at the same time, so a node can have multiple calls into this function.
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNNODECREATE)(CHINODECREATEINFO* pCreateInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNNODEDESTROY
///
/// @brief  Destroys a node instance.
///
/// This function is called at the end of a camera session during chi_destroy_pipeline() when a topology is destroyed.
/// After this function call, the node session handle is invalid, and all per-session state resources associated with this
/// session are expected to be de-allocated by the node.
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNNODEDESTROY)(CHINODEDESTROYINFO* pDestroyInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNNODEQUERYBUFFERINFO
///
/// @brief  Queries for input buffer resolution and type information.
///
/// This function is called after node creation, when the topology walks through all nodes from SINK to SOURCE for negotiating
/// resolution and type. This function returns the input resolution and type requirements from the node for a given output
/// format (resolution and type). The node can add any margin or padding requirements that might be required for processing.
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNNODEQUERYBUFFERINFO)(CHINODEQUERYBUFFERINFO* pQueryBufferInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNNODESETBUFFERINFO
///
/// @brief  Sets a specific resolution and type on the node.
///
/// This function is called after the topology successfully walks through all nodes from SINK to SOURCE for negotiating
/// resolution and type (NV12 / YUV422, and so on).
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNNODESETBUFFERINFO)(CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNNODEPROCREQUEST
///
/// @brief  Process a request.
///
/// This function is called to process a request. The node is expected to check for all the dependencies being available.
/// If buffer or metadata tag dependencies are not available, the node is expected to set the corresponding dependency and
/// return the function with success without request processing. The node is invoked with the same function back again when
/// all the dependencies are available. At this point of re-entry, the node may post any new additional dependency. If there
/// are no dependencies, the node is expected to continue the processing and complete the request processing with the return
/// status as success. The function is expected to return a failure only when there is fatal failure that blocks the
/// processing of the request.
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNNODEPROCREQUEST)(CHINODEPROCESSREQUESTINFO* pProcessRequestInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHINODEPROCREQUESTDONE
///
/// @brief  Reports the status of request processing.
///
/// The node calls this function into Chi driver to report the status as success or failure. The fences that are associated
/// with the request are signaled accordingly by Chi for this request.
///
/// @param  pInfo   Pointer to the structure containing information about the request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHINODEPROCREQUESTDONE)(CHINODEPROCESSREQUESTDONEINFO* pInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHINODEPROCMETADATADONE
///
/// @brief  Reports the status of metadata.
///
/// The node calls this function into Chi driver to report the status as success or failure of metadata.
///
/// @param  pInfo   Pointer to the structure containing information about the request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNCHINODEPROCMETADATADONE)(CHINODEPROCESSMETADATADONEINFO* pInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHICREATEFENCE
///
/// @brief  Creates a Chi fence based on the provided input attributes. This can be used to import native fences as well
///         as create new fences.
///
/// @param  [in]        CHIHANDLE
/// @param  [in][out]   CHIFENCECREATEPARAMS
/// @param  [out]       CHIFENCEHANDLE
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHICREATEFENCE)(CHIHANDLE hChiSession, CHIFENCECREATEPARAMS*, CHIFENCEHANDLE*);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCACHEOPS
///
/// @brief  Cache operations for Chi buffer handle
///
/// @param  [in]        CHINODEBUFFERHANDLE
/// @param  [in]        BOOL
/// @param  [in]        BOOL
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNCACHEOPS)(
    CHINODEBUFFERHANDLE hChiBuffer,
    BOOL                invalidate,
    BOOL                clean);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNWAITCHIFENCEASYNC
///
/// @brief  Wait for the chi fence, asynchronously caling the function provided
///
/// @param  [in]       CHIHANDLE
/// @param  [in]       PFNCHIFENCECALLBACK
/// @param  [in]       CHIFENCEHANDLE
/// @param  [in][out]  VOID*
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNWAITCHIFENCEASYNC)(
    CHIHANDLE               hChiSession,
    PFNCHIFENCECALLBACK     pCallbackFn,
    CHIFENCEHANDLE          hChiFence,
    VOID*                   pData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNSIGNALFENCE
///
/// @brief  Signal the chi fence
///
/// @param  [in]       CHIHANDLE
/// @param  [in]       CHIFENCEHANDLE
/// @param  [in]       CDKResult
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNSIGNALFENCE)(
    CHIHANDLE               hChiSession,
    CHIFENCEHANDLE          hChiFence,
    CDKResult               statusResult);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNGETFENCESTATUS
///
/// @brief  Get the chi fence signal status
///
/// @param  [in]       CHIHANDLE
/// @param  [in]       CHIFENCEHANDLE
/// @param  [in][out]  Pointer to the CDKResult pointer to be filled with fence status
///                    the possible states would be success/failure/invalid
///
/// @return CDKResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNGETFENCESTATUS)(
    CHIHANDLE               hChiSession,
    CHIFENCEHANDLE          hChiFence,
    CDKResult*              result);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNQUERYCAPS
///
/// @brief  Queries and fills the capabilities for a given input data source
///
/// @param  [in]        CHIHANDLE
/// @param  [in][out]   CHIDATASOURCECAPS
/// @param  [in]        CHIDATASOURCETYPE
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNQUERYCAPS)(
    CHIHANDLE               hChiSession,
    CHIDATASOURCECAPS       pDataSourceCaps,
    CHIDATASOURCETYPE       sourceType);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNGETDATASOURCE
///
/// @brief  Get a handle to he data source by by configuring it to the said configuration
///
/// @param  [in]        CHIHANDLE
/// @param  [in][out]   CHIDATASOURCECAPS
/// @param  [in]        CHIDATASOURCETYPE
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNGETDATASOURCE)(
    CHIHANDLE            hChiSession,
    CHIDATASOURCE*       phDataSource,
    CHIDATASOURCECONFIG* pDataSourceConfig);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNPUTDATASOURCE
///
/// @brief  Put back/delete the data source handle
///
/// @param  [in]        CHIHANDLE
/// @param  [in]        CHIDATASOURCEHANDLE
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNPUTDATASOURCE)(
    CHIHANDLE           hChiSession,
    CHIDATASOURCEHANDLE hDataSourceHandle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNGETDATA
///
/// @brief  Get data from the data source
///
/// @param  [in]        CHIHANDLE
/// @param  [in]        CHIDATASOURCEHANDLE
/// @param  [in]        CHIDATAREQUEST
/// @param  [in][out]   Pointer to the data size, to be filled
///
/// @return Pointer to the data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID* (*PFNGETDATA)(
    CHIHANDLE            hChiSession,
    CHIDATASOURCEHANDLE  hDataSourceHandle,
    CHIDATAREQUEST*      pDataRequest,
    UINT*                pSize);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNPUTDATA
///
/// @brief  Put data back to the data source
///
/// @param  [in]        CHIHANDLE
/// @param  [in]        CHIDATASOURCEHANDLE
/// @param  [in]        CHIDATAHANDLE
///
/// @return CamxResultSuccess if success
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNPUTDATA)(
    CHIHANDLE            hChiSession,
    CHIDATASOURCEHANDLE  hDataSourceHandle,
    CHIDATAHANDLE        hDataHandle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIRELEASEFENCE
///
/// @brief  Releases a Chi fence. If this fence was provided back to CHI, only the reference is decremented. The actual
///         destruction occurs when no one else has a reference to it.
///
/// @param  [in]    CHIHANDLE
/// @param  [in]    CHIFENCEHANDLE
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIRELEASEFENCE)(CHIHANDLE hChiSession, CHIFENCEHANDLE);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFCHINODEENTRY
///
/// @brief  Entry point called by the Chi driver to initialize the custom node.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Node. This function is called
/// during the camera server initialization, which occurs during HAL process start. In addition to communicating the necessary
/// function pointers between Chi and external nodes, this function allows a node to do any initialization work that it
/// would typically do at process init. Anything done here should not be specific to a session, and any variables stored in
/// the node must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pNodeCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the node.
///                                 The node must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFCHINODEENTRY) (CHINODECALLBACKS* pNodeCallbacks);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFCHINODESETNODEINTERFACE
///
/// @brief  Entry point called by the Chi driver to get node interface.
///
/// This function interface for custom node to call into Chi.
///
/// @param [in,out] pNodeInterface  Pointer to a structure that defines callbacks that the node sends to chi.
///                                 The node must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFCHINODESETNODEINTERFACE) (CHINODEINTERFACE* pNodeInterface);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNPIPELINECREATED
///
/// @brief  Entry point called by the Chi driver to indicate post pipeline creation.
///         Note that this API is deprecated, start using PFNPOSTPIPELINECREATE.
///         Currently both PFNPIPELINECREATED and PFNPOSTPIPELINECREATE are called in
///         the same functionality : PostPipelineCreate, with preference given to
///         PFNPOSTPIPELINECREATE function call. PFNPIPELINECREATED is called only
///         if PFNPOSTPIPELINECREATE is NULL.
///
/// This function interface for custom node to call into Chi.
///
/// @param  [in]    CHIHANDLE
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID(*PFNPIPELINECREATED) (CHIHANDLE hChiHandle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNPOSTPIPELINECREATE
///
/// @brief  Entry point called by the Chi driver to indicate post pipeline create.
///
/// This function interface for custom node to call into Chi.
///
/// @param  [in]    CHIHANDLE
///
/// @return ChiResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNPOSTPIPELINECREATE) (CHIHANDLE hChiHandle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFCHINODEENTRY
///
/// @brief  Entry point called by the Chi driver to initialize the custom node.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Node. This function is called
/// during the camera server initialization, which occurs during HAL process start. In addition to communicating the necessary
/// function pointers between Chi and external nodes, this function allows a node to do any initialization work that it
/// would typically do at process init. Anything done here should not be specific to a session, and any variables stored in
/// the node must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pNodeCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the node.
///                                 The node must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFCHINODEENTRY) (CHINODECALLBACKS* pNodeCallbacks);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFCHINODESETNODEINTERFACE
///
/// @brief  Entry point called by the Chi driver to get node interface.
///
/// This function interface for custom node to call into Chi.
///
/// @param [in,out] pNodeInterface  Pointer to a structure that defines callbacks that the node sends to chi.
///                                 The node must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFCHINODESETNODEINTERFACE) (CHINODEINTERFACE* pNodeInterface);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIONSTREAMON
///
/// @brief  Will be called to indicate stream on. Chi nodes may use this hook to do any preparation/configurations
///
/// @param  pOnStreamOnInfo    Pointer to a structure that defines the session stream on information for the node.
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNCHIONSTREAMON)(CHINODEONSTREAMONINFO* pOnStreamOnInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIONSTREAMOFF
///
/// @brief  Will be called to indicate stream off. Chi nodes may use this hook to do any preparation/configurations
///
/// @param  pOnStreamOffInfo    Pointer to a structure that defines the session stream off information for the node.
///
/// @return ChiResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNCHIONSTREAMOFF)(CHINODEONSTREAMOFFINFO* pOnStreamOffInfo);

/// @brief Callback Interface for Chi to call into custom node.
typedef struct ChiNodeCallbacks
{
    UINT32                      size;                       ///< Size of this structure.
    UINT32                      majorVersion;               ///< Major version.
    UINT32                      minorVersion;               ///< Minor version.
    PFNNODEGETCAPS              pGetCapabilities;           ///< Get Node Capabilities.
    PFNCHIQUERYVENDORTAG        pQueryVendorTag;            ///< Optional function to get the vendor tags supported by node.
    PFNNODECREATE               pCreate;                    ///< Mandatory function to create an instance of the node.
    PFNNODEDESTROY              pDestroy;                   ///< Mandatory function to destroy an instance of the node.
    PFNNODEQUERYBUFFERINFO      pQueryBufferInfo;           ///< Mandatory function called to query input buffer requirements.
    PFNNODESETBUFFERINFO        pSetBufferInfo;             ///< Mandatory function called to set the buffer properties.
    PFNNODEPROCREQUEST          pProcessRequest;            ///< Mandatory function to process a request by node.
    PFCHINODESETNODEINTERFACE   pChiNodeSetNodeInterface;   ///< Mandatory function to set the node interface functions.
    PFNPIPELINECREATED          pPipelineCreated;           ///< Notify pipeline created. Deprecated.
    PFNPOSTPIPELINECREATE       pPostPipelineCreate;        ///< Indication of Post pipeline creation.
    PFNCHIONSTREAMON            pOnStreamOn;                ///< Nofify pipeline stream on.
    PFNCHIONSTREAMOFF           pOnStreamOff;               ///< Nofify pipeline stream off.
} CHINODECALLBACKS;

/// @brief Interface for custom node to call into Chi.
typedef struct ChiNodeInterface
{
    UINT32                      size;                       ///< Size of this structure.
    UINT32                      majorVersion;               ///< Major version.
    UINT32                      minorVersion;               ///< Minor version.
    PFNCHIGETMETADATA           pGetMetadata;               ///< Get the data from metadata if already published.
    PFNCHIGETMETADATACOUNT      pGetMetadataCount;          ///< Get the data from metadata if already published.
    PFNCHISETMETADATA           pSetMetadata;               ///< Set the metadata and publish.
    PFNCHIGETVENDORTAGBASE      pGetVendorTagBase;          ///< Get the vendor tag base for a tag.
    PFNCHINODEPROCREQUESTDONE   pProcessRequestDone;        ///< Report the status of process capture request.
    PFNCHINODEPROCMETADATADONE  pProcessMetadataDone;       ///< Reporte the status for metadata
    PFNCHICREATEFENCE           pCreateFence;               ///< Create a fence
    PFNCHIRELEASEFENCE          pReleaseFence;              ///< Release a fence
    PFNWAITCHIFENCEASYNC        pWaitFenceAsync;            ///< Wait on fence asynchronously
    PFNSIGNALFENCE              pSignalFence;               ///< Signal the fence
    PFNGETFENCESTATUS           pGetFenceStatus;            ///< Get the chi fence signal status
    PFNGETDATASOURCE            pGetDataSource;             ///< Get Data source
    PFNGETDATA                  pGetData;                   ///< Get Data from the source
    PFNPUTDATASOURCE            pPutDataSource;             ///< Put back Data source
    PFNPUTDATA                  pPutData;                   ///< Put back data object
    PFNCACHEOPS                 pCacheOps;                  ///< Cache ops
} CHINODEINTERFACE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiNodeEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom node.
///
///         This function must be exported by every <library>.so in order for driver to initialize the Node. This function is
///         called during the camera server initialization, which occurs during HAL process start. In addition to communicating
///         the necessary function pointers between Chi and external nodes, this function allows a node to do any initialization
///         work that it would typically do at process init. Anything done here should not be specific to a session, and any
///         variables stored in the node must be protected against multiple sessions accessing it at the same time.
///
/// @param  [in,out] pNodeCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the node.
///                                  The node must fill in these function pointers.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC VOID ChiNodeEntry(
    CHINODECALLBACKS* pNodeCallbacks);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHINODE_H
