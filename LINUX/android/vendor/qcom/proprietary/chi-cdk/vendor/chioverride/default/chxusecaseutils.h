////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecaseselector.h
/// @brief CHX usecase selector utility class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASEUTILS_H
#define CHXUSECASEUTILS_H

#include <assert.h>
#include "chxincs.h"
#include "chxextensionmodule.h"

#ifdef LE_CAMERA
#include <gbm_priv.h>
#include <cutils/native_handle.h>
#define DRM_DEVICE_NAME "/dev/dri/card0"
typedef struct gbm_device GBM_DEVICE;

typedef enum {
    GRALLOC1_CONSUMER_USAGE_CPU_READ = 1ULL << 1,
    GRALLOC1_CONSUMER_USAGE_CAMERA = 1ULL << 18,
    GRALLOC1_CONSUMER_USAGE_PRIVATE_0 = 1ULL << 28,
} gralloc1_consumer_usage_t;

typedef enum {
    GRALLOC1_PRODUCER_USAGE_CPU_READ = 1ULL << 1,
    GRALLOC1_PRODUCER_USAGE_CPU_WRITE = 1ULL << 5,	
    GRALLOC1_PRODUCER_USAGE_CAMERA = 1ULL << 17,
    GRALLOC1_PRODUCER_USAGE_PRIVATE_0 = 1ULL << 28,

} gralloc1_producer_usage_t;

typedef enum {
   GRALLOC1_ERROR_NONE = 0,
   GRALLOC1_ERROR_BAD_DESCRIPTOR = 1,
   GRALLOC1_ERROR_BAD_HANDLE = 2,
   GRALLOC1_ERROR_BAD_VALUE = 3,
   GRALLOC1_ERROR_NOT_SHARED = 4,
   GRALLOC1_ERROR_NO_RESOURCES = 5,
   GRALLOC1_ERROR_UNDEFINED = 6,
   GRALLOC1_ERROR_UNSUPPORTED = 7,
} gralloc1_error_t;
#else
#include <hardware/gralloc1.h>
#endif // LE_CAMERA

#define WAIT_BUFFER_TIMEOUT 700 // should be less than the timeout used for each request (800ms)

/// Forward Declaration
struct ChiUsecase;
class  Usecase;

/// Constants
static const UINT32 MaxOutstandingRequests = 128;                   ///< Number is bigger to prevent throttling of the preview
                                                                    ///  pipeline
static const UINT32 MaxPendingFrameNumber  = 200;                   ///< Max valid number mapping between Framework and CHI
                                                                    ///  about 6s in 30fps
static const UINT32 BufferQueueDepth       = ZSLQueueDepth;         ///< Copy to reference operations that are not ZSL specific
static const UINT64 InvalidFrameNumber     = 0xFFFFFFFFFFFFFFFF;    ///< Invalid frame number

static const UINT   tuningUsecasePreviewMask     = 1;               ///< Mask to indicate to preview usecase.
static const UINT   tuningUsecaseVideoMask       = 2;               ///< Mask to indicate to video usecase.
static const UINT   tuningUsecaseSnapshotMask    = 4;               ///< Mask to indicate to snapshot usecase.
static const UINT   tuningUsecaseZSLMask         = 8;               ///< Mask to indicate to ZSL usecase.

static const UINT   CHIImmediateBufferCountDefault   = 2;           ///< Number of buffers to allocate immediately when
                                                                    ///  chi buffer manager is being created.
static const UINT   CHIImmediateBufferCountZSL       = 3;           ///< Number of buffers to allocate immediately when
                                                                    ///  chi buffer manager is being created for zsl usecase.

/// @brief Usecase identifying enums
enum class UsecaseId
{
    NoMatch         = 0,
    Default         = 1,
    Preview         = 2,
    PreviewZSL      = 3,
    MFNR            = 4,
    MFSR            = 5,
    MultiCamera     = 6,
    QuadCFA         = 7,
    RawJPEG         = 8,
    MultiCameraVR   = 9,
    Torch           = 10,
    YUVInBlobOut    = 11,
    VideoLiveShot   = 12,
    MaxUsecases     = 13,
};
/// @brief Gralloc1 interface functions
struct Gralloc1Interface
{
#ifndef LE_CAMERA
    INT32 (*CreateDescriptor)(
        gralloc1_device_t*             pGralloc1Device,
        gralloc1_buffer_descriptor_t*  pCreatedDescriptor);
    INT32 (*DestroyDescriptor)(
        gralloc1_device_t*            pGralloc1Device,
        gralloc1_buffer_descriptor_t  descriptor);
    INT32 (*SetDimensions)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        UINT32                       width,
        UINT32                       height);
    INT32 (*SetFormat)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        INT32                        format);
    INT32 (*SetProducerUsage)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        UINT64                       usage);
    INT32 (*SetConsumerUsage)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        UINT64                       usage);
    INT32 (*Allocate)(
        gralloc1_device_t*                  pGralloc1Device,
        UINT32                              numDescriptors,
        const gralloc1_buffer_descriptor_t* pDescriptors,
        buffer_handle_t*                    pAllocatedBuffers);
    INT32 (*GetStride)(
        gralloc1_device_t* pGralloc1Device,
        buffer_handle_t    buffer,
        UINT32*            pStride);
    INT32 (*Release)(
        gralloc1_device_t* pGralloc1Device,
        buffer_handle_t    buffer);
    INT32 (*Lock)(
            gralloc1_device_t*      device,
            buffer_handle_t         buffer,
            uint64_t                producerUsage,
            uint64_t                consumerUsage,
            const gralloc1_rect_t*  accessRegion,
            void**                  outData,
            int32_t                 acquireFence);
#endif // LE_CAMERA
};


/// @brief Original BufferManager create data structure
struct BufferManagerCreateData
{
    UINT32 width;               ///< Buffer width
    UINT32 height;              ///< Buffer height
    UINT32 format;              ///< Buffer format
    UINT64 producerFlags;       ///< Buffer producer gralloc flags
    UINT64 consumerFlags;       ///< Buffer consumer gralloc flags
    UINT32 numBuffers;          ///< Number of buffers to allcoate
};

/// @brief New BufferManager create data structure
struct CHIBufferManagerCreateData
{

    UINT32  width;                  ///< Buffer width
    UINT32  height;                 ///< Buffer height
    UINT32  format;                 ///< Buffer format
    UINT64  producerFlags;          ///< Buffer producer gralloc flags
    UINT64  consumerFlags;          ///< Buffer consumer gralloc flags
    UINT32  bufferStride;           ///< Buffer stride
    UINT    maxBufferCount;         ///< The Maximum number of buffers to be allocated
    UINT    immediateBufferCount;   ///< The number of buffers to be allocated immediately
};

/// @breif Generic lightweight doubly lined list node
struct LightweightDoublyLinkedListNode
{
    VOID*                                   pData; ///< Pointer to the data that needs to be stored.
    struct LightweightDoublyLinkedListNode* pPrev; ///< Pointer to the previous element in the list.
    struct LightweightDoublyLinkedListNode* pNext; ///< Pointer to the next element in the list.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class that helps select a usecase from the usecases the override module supports
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseSelector
{
public:

    /// Creates an instance of this class
    static UsecaseSelector* Create(
        const ExtensionModule* pExtModule);

    /// Destroys an instance of the class
    VOID Destroy();

    /// Get Camera ID of Max resolution camera in Dualcamera usecase
    static UINT32 FindMaxResolutionCameraID(
        LogicalCameraInfo* pCameraInfo);

    /// Get Sensor Dimension
    static VOID getSensorDimension(
        const UINT32 cameraID, const camera3_stream_configuration_t* pStreamConfig,
        UINT32 *sensorw, UINT32 *sensorh, UINT32 downscaleFactor);

    /// Returns a matching usecase
    UsecaseId GetMatchingUsecase(
        const LogicalCameraInfo *pCamInfo,
        camera3_stream_configuration_t* pStreamConfig); ///< Stream configuration

    /// Returns a default matching usecase for YUV camera, Null otherwise
    static ChiUsecase* DefaultMatchingUsecaseforYUVCamera(
        camera3_stream_configuration_t* pStreamConfig); ///< Stream configuration

    /// Returns a default matching usecase, Null otherwise
    static ChiUsecase* DefaultMatchingUsecase(
        camera3_stream_configuration_t* pStreamConfig); ///< Stream configuration

    /// Is the stream config a match for MFNR
    static BOOL MFNRMatchingUsecase(
        const camera3_stream_configuration_t* pStreamConfig);

    /// Is the stream config a match for video liveshot
    static BOOL IsVideoLiveShotConfig(
        const camera3_stream_configuration_t* pStreamConfig);


    /// Is the stream config a match for Quad CFA
    static BOOL QuadCFAMatchingUsecase(
        const LogicalCameraInfo*              pCamInfo,
        const camera3_stream_configuration_t* pStreamConfig);

    /// Is this a Quad CFA sensor
    static BOOL IsQuadCFASensor(
        const LogicalCameraInfo* pCamInfo);

    /// Is this a SHDR sensor
    static BOOL IsSHDRSensor(
        const LogicalCameraInfo* pCamInfo);


    /// Is this a preview stream
    static BOOL IsPreviewStream(
        const camera3_stream_t* pStream);

    /// Is this a video stream
    static BOOL IsVideoStream(
        const camera3_stream_t* pStream);

    /// Is this a snapshot YUV stream
    static BOOL IsYUVSnapshotStream(
        const camera3_stream_t* pStream);

    /// Is this a snapshot JPEG stream
    static BOOL IsJPEGSnapshotStream(
        const camera3_stream_t* pStream);

    /// Is this a Raw stream
    static BOOL IsRawStream(
        const camera3_stream_t* pStream);

    /// Is this a YUV input stream
    static BOOL IsYUVInStream(
        const camera3_stream_t* pStream);

    /// Is this a YUV output stream
    static BOOL IsYUVOutStream(
        const camera3_stream_t* pStream);

private:

    /// Initializes the instance
    CDKResult Initialize();

    /// Is the stream config a match for Preview+ZSL-YUV
    static BOOL IsPreviewZSLStreamConfig(
        const camera3_stream_configuration_t* pStreamConfig);

    /// Is the stream config a match for Raw+JPEG usecase
    static BOOL IsRawJPEGStreamConfig(
        const camera3_stream_configuration_t* pStreamConfig);

    /// Is the stream config a match YUV In and Blob out
    static BOOL IsYUVInBlobOutConfig(
        const camera3_stream_configuration_t* pStreamConfig);

    static BOOL IsMatchingUsecase(
        const camera3_stream_configuration_t* pStreamConfig,    ///< Stream config
        const ChiUsecase*                     pUsecase);        ///< Usecase attempted to match

    static BOOL IsMatchingFormat(
        ChiStream*             pStream,
        UINT32                 numFormats,
        const ChiBufferFormat* pFormats);

    static BOOL IsAllowedImplDefinedFormat(
        ChiBufferFormat format,
        GrallocUsage    streamGrallocUsage);

    UsecaseSelector() = default;
    ~UsecaseSelector();
    // Do not support the copy constructor or assignment operator
    UsecaseSelector(const UsecaseSelector& rUsecaseSelector) = delete;
    UsecaseSelector& operator= (const UsecaseSelector& rUsecaseSelector) = delete;

    static UINT            NumImplDefinedFormats;
    static ChiBufferFormat AllowedImplDefinedFormats[16];
    static BOOL            GPURotationUsecase;             ///< Flag to select gpu rotation usecases
    static BOOL            GPUDownscaleUsecase;            ///< Flag to select gpu rotation usecases
    static BOOL            HFRNo3AUsecase;                 ///< Flag to select HFR without 3A usecases
    static UINT            VideoEISV2Usecase;              ///< Flag to select EIS V2 usecases
    static UINT            VideoEISV3Usecase;              ///< Flag to select EIS V3 usecases

    const ExtensionModule*      m_pExtModule;              ///< const pointer to the extension module
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class that implements the factory to create usecase specific objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseFactory
{
public:
    /// Creates an object of this factory type
    static UsecaseFactory* Create(
        const ExtensionModule* pExtModule);
    /// Destroy the factory
    VOID Destroy();
    /// Create an usecase object of the specified type
    Usecase* CreateUsecaseObject(
        LogicalCameraInfo*              pLogicalCameraInfo, ///< [In] Camera associated with the usecase
        UsecaseId                       usecaseId,          ///< [In] Usecase Id
        camera3_stream_configuration_t* pStreamConfig);     ///< [In] Stream config

private:
    UsecaseFactory() = default;
    ~UsecaseFactory();
    UsecaseFactory(const UsecaseFactory&) = delete;             ///< Disallow the copy constructor
    UsecaseFactory& operator=(const UsecaseFactory&) = delete;  ///< Disallow assignment operator

    const ExtensionModule* m_pExtModule;              ///< const pointer to the extension module
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Gralloc1 Buffer Manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BufferManager
{
public:
    /// Creates an instance of this class
    static BufferManager* Create(BufferManagerCreateData* pCreateData);

    /// Destroys an instance of the class
    VOID Destroy();

    /// Get a buffer
    buffer_handle_t* GetBuffer()
    {
        buffer_handle_t* buffer = &m_buffers[m_nextFreeBuffer];

        m_nextFreeBuffer = ((m_nextFreeBuffer + 1) % m_numBuffers);

        return buffer;
    }

private:

    /// Initializes the instance
    CDKResult Initialize(BufferManagerCreateData* pCreateData);

    BufferManager() = default;
    ~BufferManager();

    // Do not support the copy constructor or assignment operator
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator= (const BufferManager&) = delete;

    // Initialize Gralloc1 interface functions
    CDKResult SetupGralloc1Interface();
    // Pre allocate the max number of buffers the buffer manager needs to manage
    CDKResult AllocateBuffers(UINT32 numBuffers,
                              UINT32 width,
                              UINT32 height,
                              UINT32 format,
                              UINT64 producerFlags,
                              UINT64 consumerFlags);
#ifdef LE_CAMERA
    // Allocate one buffer
    CDKResult AllocateGbmBuffer(UINT32              width,
                                UINT32              height,
                                UINT32              format,
                                UINT64              producerUsageFlags,
                                UINT64              consumerUsageFlags,
                                buffer_handle_t*    pAllocatedBuffer,
                                UINT32*             pStride);
#else
    CDKResult AllocateOneBuffer(UINT32           width,
                                UINT32           height,
                                UINT32           format,
                                UINT64           producerUsageFlags,
                                UINT64           consumerUsageFlags,
                                buffer_handle_t* pAllocatedBuffer,
                                UINT32*          pStride);
#endif
    // Free all buffers
    VOID FreeAllBuffers();

    buffer_handle_t     m_buffers[BufferQueueDepth];    ///< Max Buffer pool
    UINT32              m_numBuffers;                   ///< Num of Buffers
    UINT32              m_bufferStride;                 ///< Buffer stride
    UINT32              m_nextFreeBuffer;               ///< Next free ZSL buffer to be used in the preview pipeline
#ifndef LE_CAMERA
    hw_module_t*        m_pHwModule;                    ///< Gralloc1 module
    gralloc1_device_t*  m_pGralloc1Device;              ///< Gralloc1 device
    Gralloc1Interface   m_grallocInterface;             ///< Gralloc1 interface
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Generic POD lightweight doubly linked list implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LightweightDoublyLinkedList final
{
public:
    // Insert to Tail of the list
    CHX_INLINE VOID InsertToTail(
        LightweightDoublyLinkedListNode* pNodeToInsert)
    {
        CHX_ASSERT(pNodeToInsert->pPrev == NULL);
        CHX_ASSERT(pNodeToInsert->pNext == NULL);

        if (m_pHead == NULL)
        {
            m_pHead = pNodeToInsert;

            // First node going into the doubly linked list, so our head/tail are the same.
            m_pTail = m_pHead;
        }
        else
        {
            // Since we can always know where the tail it, inserts become trivial.
            m_pTail->pNext = pNodeToInsert;
            pNodeToInsert->pPrev = m_pTail;

            m_pTail = pNodeToInsert;
        }

        m_numNodes++;
    }

    // Remove and return head node
    CHX_INLINE LightweightDoublyLinkedListNode* RemoveFromHead()
    {
        LightweightDoublyLinkedListNode* pNode = m_pHead;

        if (pNode != NULL)
        {
            // If the only node was removed, the tail must be updated to reflect the empty list.
            if (m_numNodes == 1)
            {
                CHX_ASSERT(pNode == m_pTail);

                m_pTail = NULL;
                m_pHead = NULL;
            }
            else
            {
                m_pHead = pNode->pNext;
                if (NULL != m_pHead)
                {
                    m_pHead->pPrev = NULL;
                }
            }

            pNode->pPrev = NULL;
            pNode->pNext = NULL;

            m_numNodes--;
        }

        return pNode;
    }

    // Remove Node from list
    CHX_INLINE VOID RemoveNode(
        LightweightDoublyLinkedListNode* pNode)
    {
        if (pNode == m_pHead)
        {
            m_pHead = pNode->pNext;

            if (m_pHead != NULL)
            {
                m_pHead->pPrev = NULL;
            }
        }
        else
        {
            pNode->pPrev->pNext = pNode->pNext;

            if (pNode->pNext != NULL)
            {
                pNode->pNext->pPrev = pNode->pPrev;
            }
        }

        if (pNode == m_pTail)
        {
            m_pTail = pNode->pPrev;
        }

        pNode->pPrev = NULL;
        pNode->pNext = NULL;

        m_numNodes--;
    }

    // Helper to fetch next node from current node
    CHX_INLINE static LightweightDoublyLinkedListNode* NextNode(
        LightweightDoublyLinkedListNode* pNode)
    {
        return (NULL != pNode) ? pNode->pNext : NULL;
    }

    // Helper to fetch head node
    CHX_INLINE LightweightDoublyLinkedListNode* Head() const { return m_pHead; }

    // Helper to fetch tail node
    CHX_INLINE LightweightDoublyLinkedListNode* Tail() const { return m_pTail; }

    // Helper to fetch node count
    CHX_INLINE UINT NumNodes() const { return m_numNodes; }

    // constructor
    // NOWHINE CP016: Basic final class wont have overrides
    CHX_INLINE LightweightDoublyLinkedList()
    {
        m_pHead     = NULL;
        m_pTail     = NULL;
        m_numNodes  = 0;
    }

    // destructor
    // NOWHINE CP022: Basic final class wont have overrides
    CHX_INLINE ~LightweightDoublyLinkedList()
    {
        CHX_ASSERT(NumNodes() == 0);
    }

private:
    // Member methods
    LightweightDoublyLinkedList(const LightweightDoublyLinkedList&) = delete;
    LightweightDoublyLinkedList& operator=(const LightweightDoublyLinkedList&) = delete;

    LightweightDoublyLinkedListNode* m_pHead;    ///< The first element in the list
    LightweightDoublyLinkedListNode* m_pTail;    ///< The last element in the list
    UINT                             m_numNodes; ///< The number of elements in the list
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class that implements the ImageBuffer to allocate gralloc buffers and track reference count
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ImageBuffer
{
public:

    /// Static create function to make an instance of this class
#ifdef LE_CAMERA
    static  ImageBuffer* Create(
                                UINT32              width,
                                UINT32              height,
                                UINT32              format,
                                UINT64              producerUsageFlags,
                                UINT64              consumerUsageFlags,
                                UINT32*             pStride);
    /// Destroys an instance of the class
    VOID Destroy(void);
#else
    static ImageBuffer* Create(Gralloc1Interface*  pGrallocInterface,
                               gralloc1_device_t*  pGralloc1Device,
                               UINT32              width,
                               UINT32              height,
                               UINT32              format,
                               UINT64              producerUsageFlags,
                               UINT64              consumerUsageFlags,
                               UINT32*             pStride);
    /// Destroys an instance of the class
    VOID Destroy(Gralloc1Interface*  pGrallocInterface,
                 gralloc1_device_t*  pGralloc1Device);

#endif


    /// Return reference count of this image buffer
    UINT32 GetReferenceCount();

    /// Add a reference to this image buffer
    VOID AddReference();

    /// Release a reference to this image buffer
    VOID ReleaseReference();

    /// Return the handle of this image buffer
    CHX_INLINE buffer_handle_t* GetBufferHandle()
    {
        return &pGrallocBuffer;
    }

protected:
    ImageBuffer();
    ~ImageBuffer();

private:
#ifdef LE_CAMERA
    /// Allocate Gbm buffer
    CDKResult AllocateGbmBuffer(
            UINT32              width,
            UINT32              height,
            UINT32              format,
            UINT64              producerUsageFlags,
            UINT64              consumerUsageFlags,
            UINT32*             pStride);
#else
    /// Allocate gralloc buffer
    CDKResult AllocateGrallocBuffer(Gralloc1Interface*  pGrallocInterface,
                                    gralloc1_device_t*  pGralloc1Device,
                                    UINT32              width,
                                    UINT32              height,
                                    UINT32              format,
                                    UINT64              producerUsageFlags,
                                    UINT64              consumerUsageFlags,
                                    UINT32*             pStride);
#endif
    buffer_handle_t pGrallocBuffer;     ///< The buffer handle
    volatile UINT32 m_aReferenceCount;  ///< The reference count.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CHI Buffer Manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CHIBufferManager
{
public:
    /// Creates an instance of this class
    static CHIBufferManager* Create(
        const CHAR*                 pBufferManagerName,
        CHIBufferManagerCreateData* pCreateData);

    /// Destroys an instance of the class
    VOID Destroy();

    /// Return an image buffer. Reference count is set to 1 when an image buffer is returned,
    /// Caller needs to release reference when the buffer is no longer needed.
    ImageBuffer* GetImageBuffer();

    /// Add reference to the image buffer
    CDKResult AddReference(buffer_handle_t* pBufferHandle);

    /// Release reference to the image buffer
    CDKResult ReleaseReference(buffer_handle_t* pBufferHandle);

    /// Get reference of the image buffer
    UINT GetReference(buffer_handle_t* pBufferHandle);

    /// Free buffers
    VOID FreeBuffers(BOOL isPartialFree);

private:
    CHIBufferManager();
    ~CHIBufferManager();

    /// Do not support the copy constructor or assignment operator
    CHIBufferManager(const CHIBufferManager&) = delete;
    CHIBufferManager& operator= (const CHIBufferManager&) = delete;

#ifndef LE_CAMERA
    /// Setup gralloc1 interface functions
    CDKResult SetupGralloc1Interface();
#endif

    /// Initializes the instance
    CDKResult Initialize(
        const CHAR*                 pBufferManagerName,
        CHIBufferManagerCreateData* pCreateData);

    /// Reverse look up for image buffer pointer given buffer handle
    LightweightDoublyLinkedListNode* LookupImageBuffer(buffer_handle_t* pBufferHandle);


    CHAR                            m_pBufferManagerName[MaxStringLength64];    ///< Name of the buffer manager
#ifndef LE_CAMERA
    hw_module_t*                    m_pHwModule;                                ///< Gralloc1 module
    gralloc1_device_t*              m_pGralloc1Device;                          ///< Gralloc1 device
    Gralloc1Interface               m_grallocInterface;                         ///< Gralloc1 interface
#endif
    Mutex*                          m_pLock;                                    ///< Mutex protects free and busy list
    Condition*                      m_pWaitFreeBuffer;                          ///< Wait for a busy buffer becomes free
    LightweightDoublyLinkedList*    m_pFreeBufferList;                          ///< List manages free buffers
    LightweightDoublyLinkedList*    m_pBusyBufferList;                          ///< List manages busy buffers
    CHIBufferManagerCreateData      m_pBufferManagerData;                       ///< Data structure contains image buffer information
};

#ifdef LE_CAMERA
class GBMBufferAllocator
{
    private:
        GBMBufferAllocator();
        ~GBMBufferAllocator();

        INT deviceFD;                                                          /// Gdm device FD

        struct gbm_device * m_pGbmDevice;                                       ///< GBM device object

        /// Do not support the copy constructor or assignment operator
        GBMBufferAllocator(const GBMBufferAllocator& rgbmbufferallocator) = delete;
        GBMBufferAllocator& operator= (const GBMBufferAllocator& rgbmbufferallocator) = delete;

    public:
        /// Create GBM device
	GBM_DEVICE CreateGbmDeviceObject();

        /// Get handle of GBMBufferAllocator object	
        static GBMBufferAllocator * GetHandle();

        /// Get GBM usage flag corresponsing to Gralloc flag and gbm format
        static UINT GetGbmUsageFlag(uint32_t user_format, uint32_t cons_flag, uint32_t prod_flag);

        /// Get GBM format from gralloc format	
        static UINT GetGbmFormat(uint32_t user_format);

        /// Allocate GBM buffer object	
        struct gbm_bo* AllocateGbmBufferObj(UINT32              width,
                                            UINT32              height,
                                            UINT32              format,
                                            UINT64              producerUsageFlags,
                                            UINT64              consumerUsageFlags);

        /// Allocate private handle and pack GBM buffer object in that	
        buffer_handle_t AllocateNativeHandle(struct gbm_bo *bo);

        /// Free GBM buffer object	
        void FreeGbmBufferObj(struct gbm_bo *m_pGbmBuffObject);
};
#endif
#endif // CHXUSECASEUTILS_H
