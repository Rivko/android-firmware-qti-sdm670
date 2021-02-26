////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  BufferManager.cpp
/// @brief Buffer Manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BufferManager.h"
#include <stdlib.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
//#include <gralloc_priv.h>

#undef LOG_TAG
#define LOG_TAG "CameraTest"
#include <log/log.h>

BufferManager::BufferManager():\
mNumBuffers(0),
mBufferStride(0){
    Initialize();
#ifndef USE_GRALLOC1
    mIonFd = -1;
#endif
}

/************************************************************************
* name : ~BufferManager
* function: destruct object
************************************************************************/
BufferManager::~BufferManager()
{
    Destroy();
}

/************************************************************************
* name : Destory
* function: release all resource
************************************************************************/
void BufferManager::Destroy()
{
    FreeAllBuffers();
#ifdef USE_GRALLOC1
    gralloc1_close(mGralloc1Device);
#else
    close(mIonFd);
#endif
}

/************************************************************************
* name : Initialize
* function: Inint Gralloc interface
************************************************************************/
int BufferManager::Initialize()
{
    int result = 0;
#ifdef USE_GRALLOC1
    result = SetupGralloc1Interface();
#endif
    return result;
}

#ifdef USE_GRALLOC1
/************************************************************************
* name : SetupGralloc1Interface
* function: dlsym all symboles which used to alloc buffers
************************************************************************/
int BufferManager::SetupGralloc1Interface()
{
    int result = 0;

    hw_get_module(GRALLOC_HARDWARE_MODULE_ID, const_cast<const hw_module_t**>(&mHwModule));

    if (NULL != mHwModule)
    {
        gralloc1_open(mHwModule, &mGralloc1Device);
    }
    else
    {
        ALOGE("Can not get Gralloc hardware module\n");
        result = -1;
    }

    if (NULL != mGralloc1Device)
    {
        mGrallocInterface.CreateDescriptor  = reinterpret_cast<GRALLOC1_PFN_CREATE_DESCRIPTOR>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_CREATE_DESCRIPTOR));
        mGrallocInterface.DestroyDescriptor = reinterpret_cast<GRALLOC1_PFN_DESTROY_DESCRIPTOR>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_DESTROY_DESCRIPTOR));
        mGrallocInterface.SetDimensions     = reinterpret_cast<GRALLOC1_PFN_SET_DIMENSIONS>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_SET_DIMENSIONS));
        mGrallocInterface.SetFormat         = reinterpret_cast<GRALLOC1_PFN_SET_FORMAT>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_SET_FORMAT));
        mGrallocInterface.SetProducerUsage  = reinterpret_cast<GRALLOC1_PFN_SET_PRODUCER_USAGE>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_SET_PRODUCER_USAGE));
        mGrallocInterface.SetConsumerUsage  = reinterpret_cast<GRALLOC1_PFN_SET_CONSUMER_USAGE>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_SET_CONSUMER_USAGE));
        mGrallocInterface.Allocate          = reinterpret_cast<GRALLOC1_PFN_ALLOCATE>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_ALLOCATE));
        mGrallocInterface.GetStride         = reinterpret_cast<GRALLOC1_PFN_GET_STRIDE>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_GET_STRIDE));
        mGrallocInterface.Release           = reinterpret_cast<GRALLOC1_PFN_RELEASE>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_RELEASE));
        mGrallocInterface.Lock              = reinterpret_cast<GRALLOC1_PFN_LOCK>(
            mGralloc1Device->getFunction(mGralloc1Device,
                GRALLOC1_FUNCTION_LOCK));
    }
    else
    {
        ALOGE("gralloc1_open failed\n");
        result = -1;
    }

    return result;
}
#endif

/************************************************************************
* name : AllocateBuffers
* function: Alloc buffrer based on input paramaters
************************************************************************/
int BufferManager::AllocateBuffers(
    uint32_t numBuffers,
    uint32_t width,
    uint32_t height,
    uint32_t format,
    uint64_t producerFlags,
    uint64_t consumerFlags,
    uint32_t isVideoMeta,
    uint32_t isUBWC)
{
    int result = 0;

    ALOGI("AllocateBuffers, Enter");
    mIsMetaBuf = isVideoMeta;
    mIsUBWC = isUBWC;

    for (uint32_t i = 0; i < numBuffers; i++)
    {
        AllocateOneBuffer(width, height, format, producerFlags, consumerFlags, &mBuffers[i], &mBufferStride,i);
        mNumBuffers++;
        mBuffersFree.push_back(&mBuffers[i]);
    }

    ALOGI("AllocateBuffers, EXIT");
    return result;
}


/************************************************************************
* name : AllocateOneBuffer
* function: subcase to alloc buf
************************************************************************/
int BufferManager::AllocateOneBuffer(
    uint32_t           width,
    uint32_t           height,
    uint32_t           format,
    uint64_t           producerUsageFlags,
    uint64_t           consumerUsageFlags,
    buffer_handle_t*   pAllocatedBuffer,
    uint32_t*          pStride,
    uint32_t           index)
{
    int32_t result =  0;
#ifdef USE_GRALLOC1
    result = GRALLOC1_ERROR_NONE;
    gralloc1_buffer_descriptor_t gralloc1BufferDescriptor;

    result = mGrallocInterface.CreateDescriptor(mGralloc1Device, &gralloc1BufferDescriptor);

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = mGrallocInterface.SetDimensions(mGralloc1Device, gralloc1BufferDescriptor, width, height);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = mGrallocInterface.SetFormat(mGralloc1Device, gralloc1BufferDescriptor, format);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = mGrallocInterface.SetProducerUsage(mGralloc1Device, gralloc1BufferDescriptor, producerUsageFlags);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = mGrallocInterface.SetConsumerUsage(mGralloc1Device, gralloc1BufferDescriptor, consumerUsageFlags);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = mGrallocInterface.Allocate(mGralloc1Device, 1, &gralloc1BufferDescriptor, &pAllocatedBuffer[0]);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = mGrallocInterface.GetStride(mGralloc1Device, *pAllocatedBuffer, pStride);
    }

    if (GRALLOC1_ERROR_NONE != result)
    {
        ALOGE("allocate buffer failed\n");
        return result;
    }

    private_handle_t *hnl = ((private_handle_t *)(*pAllocatedBuffer));
    mBufferinfo[index].vaddr= mmap(NULL, hnl->size, PROT_READ  | PROT_WRITE, MAP_SHARED, hnl->fd, 0);
    mBufferinfo[index].size = hnl->size;
    ALOGE("vaddr: %p fd: %d", mBufferinfo[index].vaddr, ((private_handle_t *)(*pAllocatedBuffer))->fd);

    mGrallocInterface.DestroyDescriptor(mGralloc1Device, gralloc1BufferDescriptor);
#else
    struct ion_allocation_data alloc;
    struct ion_fd_data ion_info_fd;
    struct ion_fd_data data2;
    native_handle_t* nh = nullptr;
    int rc;
    size_t buf_size;
    if (mIonFd <= 0) {
        mIonFd = open("/dev/ion", O_RDONLY);
    }
    if (mIonFd <= 0) {
        ALOGE("Ion dev open failed %s\n", strerror(errno));
        return NULL;
    }
    memset(&alloc, 0, sizeof(alloc));
    if (height == 1) {
        // Blob
        buf_size = (size_t)width;
    } else {
        buf_size = (size_t)(width * height * 2);
    }

    alloc.len = (size_t)(buf_size);
    alloc.len = (alloc.len + 4095U) & (~4095U);
    alloc.align = 4096;
    alloc.flags = ION_FLAG_CACHED;
    alloc.heap_id_mask = ION_HEAP(ION_SYSTEM_HEAP_ID);
    rc = ioctl(mIonFd, ION_IOC_ALLOC, &alloc);
    if (rc < 0) {
        ALOGE("ION allocation failed %s with rc = %d fd:%d\n", strerror(errno), rc, mIonFd);
        return NULL;
    }
    memset(&ion_info_fd, 0, sizeof(ion_info_fd));
    ion_info_fd.handle = alloc.handle;
    rc = ioctl(mIonFd, ION_IOC_SHARE, &ion_info_fd);
    if (rc < 0) {
        ALOGE("ION map failed %s\n", strerror(errno));
        return NULL;
    }
    ALOGD("%s ION FD %d len %d\n", __func__, ion_info_fd.fd, alloc.len);
    if (!mIsMetaBuf) {
        *pAllocatedBuffer = native_handle_create(2, 4);
        nh = (native_handle_t*)(*pAllocatedBuffer);
        (nh)->data[0] = ion_info_fd.fd;
        (nh)->data[1] = 0;
        (nh)->data[2] = 0;
        (nh)->data[3] = 0;
        (nh)->data[4] = alloc.len;
        (nh)->data[5] = 0;
    } else {
        /*alloc private handle_t */

        /*(buffer_handler_t **)*/
        if (!mIsUBWC) {
            *pAllocatedBuffer = native_handle_create(1, 2);
            nh = (native_handle_t*)(*pAllocatedBuffer);
            (nh)->data[0] = ion_info_fd.fd;
            (nh)->data[1] = 0;
            (nh)->data[2] = alloc.len;
        } else {
            /*UBWC Mode*/
            #if 0
            private_handle_t *pnh = new private_handle_t(ion_info_fd.fd,
                alloc.len,
                0,
                0,
                0,
                width,
                height);
            *pAllocatedBuffer = (native_handle_t *)pnh;
            pnh->flags |= private_handle_t::PRIV_FLAGS_UBWC_ALIGNED;
            pnh->width = width;
            pnh->height = height;
            pnh->size = alloc.len;
            pnh->unaligned_width = width;
            pnh->unaligned_height = height;
            pnh->fd = ion_info_fd.fd;
            pnh->format = format;
            #endif
        }
    }

    data2.handle = ion_info_fd.handle;
    rc = ioctl(mIonFd, ION_IOC_MAP, &data2);
    if (rc) {
        ALOGE("ION MAP failed %s\n", strerror(errno));
        return NULL;
    }
    mBufferinfo[index].vaddr = mmap(NULL, alloc.len, PROT_READ  | PROT_WRITE, MAP_SHARED, data2.fd, 0);
    ALOGI("Alloc buffer fd:%d len:%d vaddr:%p\n",data2.fd,alloc.len,mBufferinfo[index].vaddr);
    mBufferinfo[index].size = alloc.len;
    mBufferinfo[index].allocData = alloc;
    mBufferinfo[index].fd = ion_info_fd.fd;
#endif
    return result;
}

/************************************************************************
* name : FreeAllBuffers
* function: free buffers
************************************************************************/
void BufferManager::FreeAllBuffers()
{
    for (uint32_t i = 0; i < mNumBuffers; i++)
    {
        if (NULL != mBuffers[i])
        {
            munmap(mBufferinfo[i].vaddr, mBufferinfo[i].size);
#ifdef USE_GRALLOC1
            mGrallocInterface.Release(mGralloc1Device, mBuffers[i]);
#else
            ioctl(mIonFd, ION_IOC_FREE,mBufferinfo[i].allocData.handle);
            close(mBufferinfo[i].fd);
            native_handle_close((native_handle_t *)mBuffers[i]);
            native_handle_delete((native_handle_t *)mBuffers[i]);
#endif
            mBuffers[i] = NULL;
        }
    }
}


