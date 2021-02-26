/* =========================================================================
Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_buffer_manager.h
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

#ifndef VPP_BUFFER_MANAGER_H
#define VPP_BUFFER_MANAGER_H
//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils/Mutex.h>
#include <linux/videodev2.h>

extern "C" {
#include <utils/Log.h>
#include "vpp.h"
}

#undef LOG_TAG
#define LOG_TAG "OMX_VPP"

using namespace android;

enum ExtraDataType {
    NO_EXTRADATA=0x00,
    EXTRADATA_INTERNAL=0x01,
    EXTRADATA_EXTERNAL=0x02,
};

class VppOmxComponent;

class VppBufferManager
{

public:

    enum OmxBufferPool {
        OMX_POOL_NONE,
        OMX_POOL_DECODER,
        OMX_POOL_VPP,
    };

    enum OmxBufferOwner {
        UNKNOWN,
        OMX_CLIENT = 0x01,
        OMX_VDEC = 0x02,
        VPP_IN = 0x03,
        VPP_OUT = 0x04,
    };

    struct BufferCounts {
        uint32_t decoder;
        uint32_t vppIn;
        uint32_t vppOut;
        uint32_t client;
        uint32_t seen;
    };

    struct OmxVppBuffer {
        OmxBufferPool pool;
        OmxBufferOwner owner;
        OMX_BUFFERHEADERTYPE *omxHeader;
        struct vpp_buffer vppBuffer;
        bool invalid;
        bool seen;
        uint32_t obdIdx;
        //fd handle to be passed to VPP is stored in handle
        //for static buffer mode
        OMX_U8* handle;
        OMX_U32 bufferID;

        OmxVppBuffer() : pool(OMX_POOL_NONE),
                         owner(UNKNOWN),
                         omxHeader(NULL),
                         invalid(false),
                         seen(false),
                         obdIdx(0),
                         handle(NULL),
                         bufferID(0)
        {
        }
        const char *getOwnerStr() {
            switch (owner) {
                case OMX_CLIENT:    return "client";
                case OMX_VDEC:      return "decoder";
                case VPP_IN:        return "vpp_in";
                case VPP_OUT:       return "vpp_out";
                case UNKNOWN:
                default:            return "unknown";
            }
        }

        const char *getPoolStr() {
            switch (pool) {
                case OMX_POOL_DECODER:  return "decoder";
                case OMX_POOL_VPP:      return "vpp";
                case OMX_POOL_NONE:
                default:                return "none";
            }
        }

    };

    struct OmxVppExtraData {
    private:
        uint32_t mSize;
        uint32_t mCount;
        enum ExtraDataType mType;
        VppOmxComponent *mCmp;
        int32_t mFd;
        void *mBase;
        // In External mode, mCacheBuffers will store the buffer pointer of client
        // extradata buffers. The type will be <OMX_U8 *>.
        // In Internal mode, mCacheBuffers will store the buffer header of Internal
        // extradata buffers. The type will be <OMX_BUFFERHEADERTYPE *>.
        Vector<void *> mCacheBuffers;
        bool mDebug;
        void clear();
    public:
        OmxVppExtraData(uint32_t size, uint32_t count, enum ExtraDataType type,
                VppOmxComponent *cmp);
        ~OmxVppExtraData();
        bool isValid();
        void cacheBufferFromClient(OMX_U32 bytes, OMX_U8 **clientBuf);
        void useBufferToVpp(OmxVppBuffer &buf);
        void copyExtraDataToClient(OmxVppBuffer &buf);
    };
private:

    VppOmxComponent &mComponent;

    // Map of omx_buffers to their owners
    Vector<OmxVppBuffer *> mBuffers;
    Mutex       mVppBufferLock;

    uint32_t mDecoderPoolCnt;
    uint32_t mVppPoolCnt;

    bool                          mDisableDynamicBufMode;

    // Criterion for determining whether buffer requirements are adequate or not
    uint32_t                      mOmxMinBufCnt;
    uint32_t                      mOmxActBufCnt;

    uint32_t                      mOmxFtbCount;
    uint32_t                      mVppFtbCount;
    uint32_t                      mMinVppOutCount;

    uint32_t mOMXBufferID;
    uint32_t mObdCnt;

    OmxVppExtraData *mOmxVppExtraData;

    void PrepareBufferForVppIn(OmxVppBuffer &buf);
    void PrepareBufferForVppOut(OmxVppBuffer &buf);
    void PrepareBufferForVdecOut(OmxVppBuffer &buf);
    void PrepareBufferForClient(OmxVppBuffer &buf, bool translateParams = true);

    // Get omx_buffer
    OmxVppBuffer *GetOmxVppBuffer(OMX_BUFFERHEADERTYPE *buffer);
    OmxVppBuffer *GetOldestClientOwnedVppBuffer_l();

    private_handle_t *GetPrivateHandle(OmxVppBuffer &buf);
    void IncreasePoolCount(OmxBufferPool pool);
    void DecreasePoolCount(OmxBufferPool pool);
    void AssignBufferToPool(OmxVppBuffer &buf);
    void UnassignBufferFromPool_l(OmxVppBuffer &buf);
    void UnassignBufferFromPool(OmxVppBuffer &buf);
    void ReassignBufferToPool(OmxVppBuffer &buf);
    void GetBufferCounts_l(BufferCounts &counts);

public:
    VppBufferManager(VppOmxComponent &component);
    ~VppBufferManager();

    // Updates the port definition to or from a port definition. That is, if
    // |isClientSet| is true, this will inform the buffer manager of
    // nBufferCountActual that will be in circulation. If |isClientSet| is false,
    // this will update the port defintiion with the number of buffers that are
    // required based on the vpp requirements in |req|.
    // Returns true if successful, false otherwise.
    bool UpdateBufferRequirements(struct vpp_requirements &req,
                                  OMX_PARAM_PORTDEFINITIONTYPE *portDef,
                                  bool isClientSet);

    // Manage buffer mappings between OMX headers and VPP buffer structures
    void ClearBuffers();
    void DebugBuffers();
    bool RegisterClientBuffer(OMX_BUFFERHEADERTYPE*, OMX_U8*);
    bool UnregisterClientBuffer(OMX_BUFFERHEADERTYPE*);

    // To reset Buffer requirements
    bool ResetBufferRequirements(OMX_PARAM_PORTDEFINITIONTYPE* portDef);

    // Return ftb destination, returns the vpp_buffer_pointer if needed
    OmxVppBuffer* GetFtbDestination(OMX_BUFFERHEADERTYPE* omxHdr);
    OmxVppBuffer* GetVdecFbdDestination(OMX_BUFFERHEADERTYPE* buffer);
    OmxVppBuffer* GetVppObdDestination(OMX_BUFFERHEADERTYPE* omxHdr);

    // Get buffer statistics.
    void PrintBufferStatistics();

    // Returns 1 if dynamic buffer mode is disabled
    bool GetBufferMode();

    // Set 1 to disable dynamic buffer mode
    void SetBufferMode(bool);

    // Translate Buffer flags from Vpp to Omx and viceversa.
    void ConvertOmxVppFlags(uint32_t inflags, uint32_t& outflags,bool fromOmxToVpp);

    // Get buffer ownership statistics
    void GetBufferCounts(BufferCounts &counts);

    uint32_t GetActBufCount() { return mOmxActBufCnt; }

    void EnableOmxVppExtraData(uint32_t size, uint32_t count);

    void UseExtraBufferByClient(OMX_U32 bytes, OMX_U8 **buffer);

    void FreeOmxVppExtraData();
};

#endif /* VPP_BUFFER_MANAGER_H */
