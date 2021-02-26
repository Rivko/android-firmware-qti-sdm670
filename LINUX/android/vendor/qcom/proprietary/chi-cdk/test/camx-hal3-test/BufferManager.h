////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  BuffeerManager.h
/// @brief buffer manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __BUFFER_MANAGER__
#define __BUFFER_MANAGER__

#ifdef USE_GRALLOC1
#include <hardware/gralloc1.h>
#endif
#include <cutils/native_handle.h>
#include <deque>
#include <stddef.h>
#include <linux/ion.h>
#include <linux/msm_ion.h>
#include <condition_variable>
#include <mutex>

#define  BUFFER_QUEUE_DEPTH  256
typedef const native_handle_t* buffer_handle_t;

typedef struct _BufferInfo{
    void*   vaddr;
    int     size;
    int     fd;
    struct ion_allocation_data allocData;
}BufferInfo;

#ifdef USE_GRALLOC1
/// @brief Gralloc1 interface functions
struct Gralloc1Interface
{
    int32_t (*CreateDescriptor)(
        gralloc1_device_t*             pGralloc1Device,
        gralloc1_buffer_descriptor_t*  pCreatedDescriptor);
    int32_t (*DestroyDescriptor)(
        gralloc1_device_t*            pGralloc1Device,
        gralloc1_buffer_descriptor_t  descriptor);
    int32_t (*SetDimensions)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        uint32_t                       width,
        uint32_t                       height);
    int32_t (*SetFormat)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        int32_t                        format);
    int32_t (*SetProducerUsage)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        uint64_t                       usage);
    int32_t (*SetConsumerUsage)(
        gralloc1_device_t*           pGralloc1Device,
        gralloc1_buffer_descriptor_t descriptor,
        uint64_t                       usage);
    int32_t (*Allocate)(
        gralloc1_device_t*                  pGralloc1Device,
        uint32_t                              numDescriptors,
        const gralloc1_buffer_descriptor_t* pDescriptors,
        buffer_handle_t*                    pAllocatedBuffers);
    int32_t (*GetStride)(
        gralloc1_device_t* pGralloc1Device,
        buffer_handle_t    buffer,
        uint32_t*            pStride);
    int32_t (*Release)(
        gralloc1_device_t* pGralloc1Device,
        buffer_handle_t    buffer);
    int32_t (*Lock)(
        gralloc1_device_t*      device,
        buffer_handle_t         buffer,
        uint64_t                producerUsage,
        uint64_t                consumerUsage,
        const gralloc1_rect_t*  accessRegion,
        void**                  outData,
        int32_t                 acquireFence);
};
#endif
class BufferManager
{
public:
    BufferManager();
    ~BufferManager();

    // Initializes the instance
    int Initialize();
    // Destroys an instance of the class
    void Destroy();
    // Pre allocate the max number of buffers the buffer manager needs to manage
    int AllocateBuffers(uint32_t numBuffers,
        uint32_t width,
        uint32_t height,
        uint32_t format,
        uint64_t producerFlags,
        uint64_t consumerFlags,
        uint32_t isVideoMeta = 0,
        uint32_t isUBWC = 0);

    // Free all buffers
    void FreeAllBuffers();
    /// Get a buffer
    buffer_handle_t* GetBuffer()
    {
        std::unique_lock<std::mutex> lock(mBufMutex);
        if (mBuffersFree.size() == 0) {
            //wait for usable buf
            mBufCv.wait(lock);
        }
        buffer_handle_t* buffer = mBuffersFree.front();
        mBuffersFree.pop_front();
        return buffer;
    }
    void PutBuffer(buffer_handle_t* buffer)
    {
        std::unique_lock<std::mutex> lock(mBufMutex);
        mBuffersFree.push_back(buffer);
        mBufCv.notify_all();
    }
    size_t QueueSize()
    {
        std::unique_lock<std::mutex> lock(mBufMutex);
        return mBuffersFree.size();
    }

    BufferInfo* getBufferInfo(buffer_handle_t* buffer)
    {
        std::unique_lock<std::mutex> lock(mBufMutex);
        for (uint32_t i = 0;i < mNumBuffers;i++){
            if (*buffer == mBuffers[i]){
                return &(mBufferinfo[i]);
            }
        }
        return NULL;
    }
private:

    // Do not support the copy constructor or assignment operator
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator= (const BufferManager&) = delete;

    // Initialize Gralloc1 interface functions
    int SetupGralloc1Interface();
    // Allocate one buffer
    int AllocateOneBuffer(uint32_t           width,
        uint32_t           height,
        uint32_t           format,
        uint64_t           producerUsageFlags,
        uint64_t           consumerUsageFlags,
        buffer_handle_t* pAllocatedBuffer,
        uint32_t*          pStride,
        uint32_t           index);

    buffer_handle_t     mBuffers[BUFFER_QUEUE_DEPTH];    ///< Max Buffer pool
    BufferInfo          mBufferinfo[BUFFER_QUEUE_DEPTH];
    uint32_t            mNumBuffers;                   ///< Num of Buffers
    uint32_t            mBufferStride;                 ///< Buffer stride
    std::deque<buffer_handle_t*> mBuffersFree;
    uint32_t            mIsMetaBuf;
    uint32_t            mIsUBWC;
    std::mutex          mBufMutex;
    std::condition_variable  mBufCv;

#ifdef USE_GRALLOC1
    hw_module_t*        mHwModule;                    ///< Gralloc1 module
    gralloc1_device_t*  mGralloc1Device;              ///< Gralloc1 device
    Gralloc1Interface   mGrallocInterface;             ///< Gralloc1 interface
#else
    int                 mIonFd;
#endif
};
#endif //__BUFFER_MANAGER__
