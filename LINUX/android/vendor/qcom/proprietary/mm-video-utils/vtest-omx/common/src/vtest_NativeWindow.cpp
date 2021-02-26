/* ========================================================================= *
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

 Not a Contribution, Apache license notifications and license are retained
 for attribution purposes only.
* ========================================================================= */
/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vtest_Debug.h"
#include "vtest_NativeWindow.h"
#include "vtest_ISource.h"
#include "vtest_Sleeper.h"
#include <sys/mman.h>
#include "vtest_ComDef.h"

#undef LOG_TAG
#define LOG_TAG "VTEST_NATIVE_WINDOW"
namespace vtest {

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
NativeWindow::NativeWindow(OMX_BOOL bRotate,
        OMX_U32 nColorFormat, OMX_BOOL bShowVideo)
    : ISource(),
      m_pSurfaceComposer(NULL),
      m_pSurface(NULL),
      m_pControl(NULL),
      m_pNativeWindow(NULL),
      m_pBackgroundControl(NULL),
      m_nFrames(0),
      m_nDisplayWidth(0),
      m_nDisplayHeight(0),
      m_nMinBufferCount(0),
      m_eRotation(bRotate ? ROTATION_90 : ROTATION_0),
      mBufMap(NULL),
      m_nBuffersWithNativeWindow(0),
      m_pFreeBufferQueue(new SignalQueue(32, sizeof(BufferInfo*))),
      m_bFreeBuffers(OMX_FALSE),
      m_bShowVideo(bShowVideo),
      m_pCRCChecker(NULL),
      m_hUBWC(NULL),
      m_pUBWCConfig(NULL),
      m_pTempNV12Buf(NULL),
      m_ePlaybackMode(DynamicBufferMode) {

    snprintf(m_pName, MAX_NAME_LENGTH, "%s", "NativeWindow");
    VTEST_MSG_LOW("%s created", Name());

    m_pSurfaceComposer = new SurfaceComposerClient();
    if (m_pSurfaceComposer->initCheck() != (status_t)OK) {
        VTEST_MSG_ERROR("m_pSurfaceComposer->initCheck() failed");
    }

    /* get display dimensions */
    m_nDisplayWidth = NativeWindow::DEFAULT_DISPLAY_WIDTH;
    m_nDisplayHeight = NativeWindow::DEFAULT_DISPLAY_HEIGHT;
    sp<IBinder> primaryDisplay = m_pSurfaceComposer->getBuiltInDisplay(
        ISurfaceComposer::eDisplayIdMain);
    DisplayInfo displayInfo;
    status_t ret = m_pSurfaceComposer->getDisplayInfo(
        primaryDisplay, &displayInfo);
    if (ret == NO_ERROR) {
        if (displayInfo.orientation == DISPLAY_ORIENTATION_90
            || displayInfo.orientation == DISPLAY_ORIENTATION_270) {
            m_nDisplayHeight = displayInfo.w;
            m_nDisplayWidth = displayInfo.h;
        } else {
            m_nDisplayWidth = displayInfo.w;
            m_nDisplayHeight = displayInfo.h;
        }
    }

    VTEST_MSG_HIGH("Display W x H = %u x %u", (unsigned int)m_nDisplayWidth, (unsigned int)m_nDisplayHeight);
    m_pBackgroundControl = m_pSurfaceComposer->createSurface(
        String8("vdec-bkgnd"), m_nDisplayWidth,
        m_nDisplayHeight, PIXEL_FORMAT_RGB_565);

    m_pControl = m_pSurfaceComposer->createSurface(
        String8("vdec-surface"), m_nDisplayWidth,
        m_nDisplayHeight, PIXEL_FORMAT_OPAQUE);

#if NWFIX != 0
    m_pSurfaceComposer->openGlobalTransaction();
    VTEST_MSG_MEDIUM("ShowVideo is %s", m_bShowVideo ? "ON" : "OFF");
    if (m_bShowVideo) {
        //show-up video on the top
        if (m_pControl->setLayer(100000 + 1) != (status_t)OK) {
            VTEST_MSG_ERROR("m_pControl->setLayer failed");
        }
    } else {
        //Don't show
        if (m_pControl->setLayer(-1) != (status_t)OK) {
            VTEST_MSG_ERROR("m_pControl->setLayer failed");
        }
    }
    //show-up background below video
    if (m_pBackgroundControl->setLayer(100000) != (status_t)OK) {
        VTEST_MSG_ERROR("m_pBackgroundControl->setLayer failed");
    }
    m_pBackgroundControl->show();
    m_pSurfaceComposer->closeGlobalTransaction();
#endif

    m_pSurface = m_pControl->getSurface();
    m_pNativeWindow = m_pSurface;
    m_sOutputRes.nColorFmt = nColorFormat;

    native_window_api_connect(m_pNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
NativeWindow::~NativeWindow() {

    Cleanup();

    if (m_pNativeWindow.get() != NULL) {
        native_window_api_disconnect(m_pNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
        m_pNativeWindow.clear();
        m_pNativeWindow = NULL;
    }

    if (m_pSurface.get() != NULL) {
        m_pSurface.clear();
        m_pSurface = NULL;
    }

    if (m_pControl.get() != NULL) {
        m_pControl.clear();
        m_pControl = NULL;
    }

    if (m_pBackgroundControl.get() != NULL) {
        m_pBackgroundControl.clear();
        m_pBackgroundControl = NULL;
    }

    if (m_pSurfaceComposer.get() != NULL) {
        m_pSurfaceComposer->dispose();
        m_pSurfaceComposer.clear();
        m_pSurfaceComposer = NULL;
    }

    if (m_pFreeBufferQueue != NULL) {
        delete m_pFreeBufferQueue;
        m_pFreeBufferQueue = NULL;
    }
    if(m_pCRCChecker)
    {
        /* Check pending CRC count and update error code */
        int cnt = m_pCRCChecker->GetPendingCRCCount(m_pCRCChecker);
        if(cnt)
        {
            SetError();
            VTEST_MSG_ERROR("CRC pending cnt %d!!",cnt);
        }
        m_pCRCChecker->Destroy(m_pCRCChecker);
        free(m_pCRCChecker);
    }
    if (m_pTempNV12Buf)
        free(m_pTempNV12Buf);
    if (m_pUBWCConfig)
        free(m_pUBWCConfig);
    m_pUBWCConfig = NULL;
    if (m_hUBWC && pFnUBWCConvertDeInit)
	    pFnUBWCConvertDeInit(m_hUBWC);
    m_hUBWC = NULL;
    if (pLibUBWCHandle)
        dlclose(pLibUBWCHandle);
    VTEST_MSG_ERROR("I'm getting free!!");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PortBufferCapability NativeWindow::GetBufferRequirements(OMX_U32 ePortIndex) {

    PortBufferCapability sBufCap;

    Mutex::Autolock autoLock(m_pLock);
    memset(&sBufCap, 0, sizeof(PortBufferCapability));

    // make sure we have been configured here
    if (m_nMinBufferCount <= 0) {
        VTEST_MSG_ERROR("Error: must call configure to get valid buf reqs");
        return sBufCap;
    }

    if ((ePortIndex != PORT_INDEX_IN) && (ePortIndex != PORT_INDEX_OUT)) {
        VTEST_MSG_ERROR("Error: invalid port selection");
        return sBufCap;
    }

    // Note: NativeWindow can work in "pass-through" mode where input
    // buffers are displayed then passed to the output port.
    // this allows playback and "transcode":
    // i.e. decoder ==> native-window ==> encoder
    sBufCap.bAllocateBuffer = OMX_TRUE;
    sBufCap.bUseBuffer = OMX_FALSE;
    sBufCap.pSource = this;
    sBufCap.ePortIndex = ePortIndex;
    sBufCap.nWidth = 0;
    sBufCap.nHeight = 0;
    sBufCap.nMinBufferSize = (m_pSource != NULL) ?
        m_pSource->GetBufferRequirements(PORT_INDEX_OUT).nMinBufferSize : 0x1000;
    sBufCap.nMinBufferCount = (m_pSource != NULL) ?
        m_pSource->GetBufferRequirements(PORT_INDEX_OUT).nMinBufferCount
        : m_nMinBufferCount;
    sBufCap.nExtraBufferCount = m_nMinBufferCount;
    sBufCap.nExtraDataBufferSize = 0;
    sBufCap.nExtraDataBufferCount = 0;
    sBufCap.nBufferUsage = 0;
    sBufCap.bUseSameBufferPool = OMX_TRUE;
    return sBufCap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::Start() {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    BufferInfo *pBuffers = NULL;
    OMX_U32 nBufferCount;
    {
        Mutex::Autolock autoLock(m_pLock);

        // duplicate starts ok, as any source can start
        if (m_pThread->Started()) {
            return result;
        }

        /* Do not call ISource::Start() because in case of transcode,
         * since we are sharing the buffers between both ports, we will
         * end up queuing the same set of buffers twice to decoder output
         * port(Initially). One by Decoder and One by NativeWindow since it is
         * in pass through mode. The queuing of buffers twice results in a
         * non-fatal error from the decoder but still best to avoid it. */

        VTEST_MSG_MEDIUM("%s, thread start...", Name());
        result = m_pThread->Start(
            this,   // threaded object
            this,   // thread data
            0);     // thread priority
        FAILED1(result, "Failed to start thread");
    }

    if (m_pSource != NULL) {
        result = m_pSource->Start();
        FAILED1(result, "Error: cannot start source %s!", m_pSource->Name());
    }

    if (m_pSink != NULL) {
        result = m_pSink->Start();
        FAILED1(result, "Error: cannot start sink %s!", m_pSink->Name());
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::Configure(CodecConfigType *pConfig,
        BufferManager *pBufManager, ISource *pSource, ISource *pSink) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    int minUndequeuedBufs = 0;
    VidcStatus ret = VidcStatusSuccess;

    result = ISource::Configure(pConfig, pBufManager, pSource, pSink);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("DecoderFileSink configure failed");
        return result;
    }

    Mutex::Autolock autoLock(m_pLock);

    if (!pConfig->bThumbnailMode)
        m_nFrames = pConfig->nFrames;
    else {
        if (pConfig->eCodec == OMX_VIDEO_CodingAVC ||
            pConfig->eCodec == OMX_VIDEO_CodingMPEG2 ||
            pConfig->eCodec == OMX_VIDEO_CodingWMV)
        {
            m_nFrames = 2; //incase of interlace frames
        }
        else
        {
            m_nFrames = 1;
        }
    }
    m_sOutputRes.nColorFmt = pConfig->nOutputColorFormat;
    m_sOutputRes.nWidth = pConfig->nFrameWidth;
    m_sOutputRes.nHeight = pConfig->nFrameHeight;
    m_bThumbnailMode = pConfig->bThumbnailMode;
    m_sOutputRes.nStartX = 0;
    m_sOutputRes.nStartY = 0;
    m_sOutputRes.nCropWidth =  pConfig->nFrameWidth;
    m_sOutputRes.nCropHeight = pConfig->nFrameHeight;
    if (pConfig->bThumbnailMode)
        m_ePlaybackMode = DynamicPortReconfig;
    else
        m_ePlaybackMode = pConfig->ePlaybackMode;

    if (QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m == m_sOutputRes.nColorFmt )
    {
        m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_NV12, pConfig->nFrameWidth);
        m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, pConfig->nFrameHeight);
    }
    else
    {
        VTEST_MSG_HIGH("Color format is not NV12! StridexScanlines is set to same as widhthxheight");
        m_sOutputRes.nStride = pConfig->nFrameWidth;
        m_sOutputRes.nScanlines = pConfig->nFrameHeight;
        if (QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed == m_sOutputRes.nColorFmt)
        {
            pLibUBWCHandle = dlopen("libUBWC.so", RTLD_NOW);
            VTEST_MSG_HIGH("Lib UBWC loading handle");
            if (pLibUBWCHandle)
            {
                pFnUBWCConvertInit = reinterpret_cast<void*(*)(void)>(dlsym(pLibUBWCHandle,"UBWCConvertInit"));
                pFnUBWCConvertInitZero = reinterpret_cast<void*(*)(void *)>(dlsym(pLibUBWCHandle,"UBWCConvertInitZero"));
                pFnUBWCConfigInit = reinterpret_cast<void*(*)(void)>(dlsym(pLibUBWCHandle,"UBWCConfigInit"));
                pFnUBWCConfigInitZero = reinterpret_cast<void*(*)(void *)>(dlsym(pLibUBWCHandle,"UBWCConfigInitZero"));
                pFnConvertUBWCToNV12 = reinterpret_cast<VidcStatus(*)(void*, void*, uint8*, uint8**, VideoResolution*)>(dlsym(pLibUBWCHandle,"ConvertUBWCToNV12"));
                pFnUBWCConvertConfigure = reinterpret_cast<void(*)(void*, OMX_U32)>(dlsym(pLibUBWCHandle,"UBWCConvertConfigure"));
                pFnUBWCConvertDeInit = reinterpret_cast<void(*)(void*)>(dlsym(pLibUBWCHandle,"UBWCConvertDeInit"));
                if (!pFnUBWCConvertInit || !pFnUBWCConvertInitZero || !pFnUBWCConfigInit ||
                    !pFnUBWCConfigInitZero || !pFnConvertUBWCToNV12 || !pFnUBWCConvertConfigure ||
                    !pFnUBWCConvertDeInit)
                {
                    dlclose(pLibUBWCHandle);
                    pFnUBWCConvertInit = NULL;
                    pFnUBWCConvertInitZero = NULL;
                    pFnUBWCConfigInit = NULL;
                    pFnUBWCConfigInitZero = NULL;
                    pFnConvertUBWCToNV12 = NULL;
                    pFnUBWCConvertConfigure = NULL;
                    pFnUBWCConvertDeInit = NULL;
                    VTEST_MSG_ERROR("Error: dlsym failing after loading libUBWC.so");
                    return OMX_ErrorUndefined;
                }
            }
            else
            {
                VTEST_MSG_ERROR("Error: dlopen failed");
                return OMX_ErrorUndefined;
            }
        }
    }
    if(!pConfig->bSecureSession)
    {
#ifdef PRE_SDM845
        if (sGlobalStaticVideoProp.bGenerateCrc)
        {
            /* Create CRC checker instance,
             * for generating CRC dump.
             * CRC validation is skipped  */
            m_pCRCChecker = (VideoCRCChecker*)malloc(sizeof(VideoCRCChecker));
            if (!m_pCRCChecker)
            {
                VTEST_MSG_ERROR("Alloc failed! Unable to create CRCchecker instance");
                return OMX_ErrorUndefined;
            }

            ret = VideoCRCCheckerInit(m_pCRCChecker,
                                      sGlobalStaticVideoProp.bGenerateCrc,
                                      VIDEO_CRCMODE_SW);
            if (ret)
            {
                VTEST_MSG_ERROR("Failed to initialize VideoCRCChekcer %d",ret);
                return OMX_ErrorUndefined;
            }

            /* Update CRC file name */
            SNPRINTF(pConfig->cCrcFileName, VTEST_MAX_STRING, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                                    pConfig->cInFileName, ".fcrc");

            /* Configure CRC Checker with CRC file name */
            ret = m_pCRCChecker->Configure(m_pCRCChecker,pConfig->cCrcFileName);
            if (ret)
            {
                VTEST_MSG_ERROR("Failed to configure VideoCRCChekcer %d",ret);
                return OMX_ErrorUndefined;
            }
        }
        else if ((!sGlobalStaticVideoProp.bSkipCrcCheck) &&
                (pConfig->cCrcFileName[0] != '\0'))
        {
            /* Create CRC checker instance
             * For validating CRC agianst reference
             * unless validation is not disable through Master config */
            m_pCRCChecker = (VideoCRCChecker*)malloc(sizeof(VideoCRCChecker));
            if (!m_pCRCChecker)
            {
                VTEST_MSG_ERROR("Alloc failed! Unable to create CRCchecker instance");
                return OMX_ErrorUndefined;
            }

            ret = VideoCRCCheckerInit(m_pCRCChecker,
                                      sGlobalStaticVideoProp.bGenerateCrc,
                                      VIDEO_CRCMODE_SW);
            if (ret)
            {
                VTEST_MSG_ERROR("Failed to initialize VideoCRCChekcer %d",ret);
                return OMX_ErrorUndefined;
            }
            char sName[VTEST_MAX_STRING];
            SNPRINTF( sName, VTEST_MAX_STRING, "%s%s", sGlobalStaticVideoProp.sCRCRoot,
                                                                pConfig->cCrcFileName);

            /* Configure CRC Checker with refCRC file name */
            ret = m_pCRCChecker->Configure(m_pCRCChecker, sName);
            if (ret)
            {
                VTEST_MSG_ERROR("Failed to configure VideoCRCChekcer %d",ret);
                return OMX_ErrorUndefined;
            }
        }
#endif
    }

    result = (OMX_ERRORTYPE)m_pNativeWindow->query(m_pNativeWindow.get(),
            NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, &minUndequeuedBufs);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS query failed: %d", result);
        return OMX_ErrorUndefined;
    }
    // Add one extra buffer to guarantee that dequeue never blocks
    // Add two extra buffers to match the android framework
    m_nMinBufferCount = (OMX_U32)minUndequeuedBufs + 1 + 2;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::SetBuffer(
        BufferInfo *pBuffer, ISource *pSource, OMX_U32 framenum) {

    (void) framenum;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    {
        Mutex::Autolock autoLock(m_pLock);
        /* TODO : give all buffers back to Buffer Manager */
        if (m_bThreadStop) {
            result = m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
            return OMX_ErrorNoMore;
        }
    }

    result = ISource::SetBuffer(pBuffer, pSource);
    if (result != OMX_ErrorNone) {
        return result;
    }

    if (pSource == m_pSource) {

        VTEST_MSG_LOW("queue push (%p %p)",
                pBuffer->pHeaderIn, pBuffer->pHeaderOut);
        result = m_pBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
        FAILED1(result, "Error: queue push failed");
    } else if (pSource == m_pSink) {

        BufferInfo *pFreeBuffer = NULL;
        int bufferindex = -1;
        /* "pass-through" mode, pass the buffer to sink for "transcode"
           get the corresponding pBuffer from the other pool with output header*/
        bufferindex = m_pBufferManager->GetBuffer(this, PORT_INDEX_IN,
                pBuffer->pHeaderOut, &pFreeBuffer);

        result = m_pSource->SetBuffer(pFreeBuffer, this);
        /* This scenario will only happen between setting InputEOS &
         * ThreadStop. Save the buffers in that case. */
        if (result != OMX_ErrorNone) {
            VTEST_MSG_MEDIUM("SetBuffer on source failed pBuffer: %p", pFreeBuffer);
            m_pFreeBufferQueue->Push(&pFreeBuffer, sizeof(BufferInfo **));
        }
    } else {
        VTEST_MSG_ERROR("Invalid source : %s", pSource->Name());
        result = OMX_ErrorBadParameter;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::AllocateBuffers(BufferInfo **pBuffers,
        OMX_BUFFERHEADERTYPE **pExtraDataBuffers, OMX_U32 nWidth,
        OMX_U32 nHeight, OMX_U32 nBufferCount, OMX_U32 nBufferSize,
        OMX_U32 nExtraDataBufferCount, OMX_U32 nExtraDataBufferSize,
        OMX_U32 ePortIndex, OMX_U32 nBufferUsage) {
    OMX_ERRORTYPE result = OMX_ErrorNone;
    (void) pExtraDataBuffers, (void) nExtraDataBufferCount;
    (void) nExtraDataBufferSize;
    Mutex::Autolock autoLock(m_pLock);

    // check valid PortReconfig
    if (m_sOutputRes.nWidth != nWidth || m_sOutputRes.nHeight!= nHeight) {
        VTEST_MSG_ERROR("Error: alloc height/width do not match");
        return OMX_ErrorUndefined;
    }

    status_t err = native_window_set_scaling_mode(
            m_pNativeWindow.get(), NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
    if (err != OK) {
        VTEST_MSG_ERROR("native_window_set_scaling_mode failed");
        return OMX_ErrorUndefined;
    }

    OMX_U32 transform;
    switch (m_eRotation) {
        case 0:
            transform = 0; break;
        case 90:
            transform = HAL_TRANSFORM_ROT_90; break;
        default:
            transform = 0; break;
    }

    if (transform) {
        err = native_window_set_buffers_transform(m_pNativeWindow.get(), transform);
    }

    OMX_U32 w = 0;
    OMX_U32 h = 0;
    OMX_U32 x = 0;
    OMX_U32 y = 0;
    FixScaling(transform, w, h, x, y);

#if NWFIX != 0
    m_pSurfaceComposer->openGlobalTransaction();
    m_pControl->setPosition(x, y);
    m_pControl->setSize(w, h);
    m_pSurfaceComposer->closeGlobalTransaction();
#endif

    android_native_rect_t crop;
    crop.left = 0;
    crop.top = 0;
    crop.right = m_sOutputRes.nWidth;
    crop.bottom = m_sOutputRes.nHeight;

    VTEST_MSG_MEDIUM("nativeWindow set crop: [%d, %d] [%d, %d]",
            crop.left, crop.top, crop.right, crop.bottom);
    err = native_window_set_crop(m_pNativeWindow.get(), &crop);
    if (err != OK) {
        VTEST_MSG_ERROR("native_window_set_crop failed");
        return OMX_ErrorUndefined;
    }

    VTEST_MSG_MEDIUM("nativeWindow set geometry: w=%u h=%u colorFormat=%x",
                                         (unsigned int)m_sOutputRes.nWidth,
                                        (unsigned int)m_sOutputRes.nHeight,
                                                   m_sOutputRes.nColorFmt);
    err = native_window_set_buffers_geometry(m_pNativeWindow.get(),
                                               m_sOutputRes.nWidth,
                                              m_sOutputRes.nHeight,
                        GetGrallocFormat( m_sOutputRes.nColorFmt));
    if (err != OK) {
        VTEST_MSG_ERROR("native_window_set_buffers_geometry failed");
        return OMX_ErrorUndefined;
    }

    // Make sure to check whether video decoder
    // requested protected buffers.
    if (nBufferUsage & GRALLOC_USAGE_PROTECTED) {
        // Verify that the ANativeWindow sends images directly to
        // SurfaceFlinger.
        int queuesToNativeWindow = 0;
        err = m_pNativeWindow->query(m_pNativeWindow.get(),
                NATIVE_WINDOW_QUEUES_TO_WINDOW_COMPOSER, &queuesToNativeWindow);
        if (err != 0) {
            VTEST_MSG_ERROR("error authenticating native window: %d", err);
            return OMX_ErrorUndefined;
        }
        if (queuesToNativeWindow != 1) {
            VTEST_MSG_ERROR("native window could not be authenticated");
            return OMX_ErrorUndefined;
        }
    }

    err = native_window_set_usage(m_pNativeWindow.get(), nBufferUsage |
            GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP);
    if (err != 0) {
        VTEST_MSG_ERROR("native_window_set_usage failed: %s (%d)",
                strerror(-err), -err);
        return OMX_ErrorUndefined;
    }

    err = native_window_set_buffer_count(m_pNativeWindow.get(), nBufferCount);
    if (err != 0) {
        VTEST_MSG_ERROR("native_window_set_buffer_count failed: %s (%d)",
                strerror(-err), -err);
        return OMX_ErrorUndefined;
    }
    VTEST_MSG_MEDIUM(
        "allocating %u buffers from a native window of size %u on output port",
        (unsigned int)nBufferCount, (unsigned int)nBufferSize);

    int minUndequeuedBufs = 0;
    result = (OMX_ERRORTYPE)m_pNativeWindow->query(m_pNativeWindow.get(),
    NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, &minUndequeuedBufs);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS query failed: %d", result);
        return OMX_ErrorUndefined;
    }
    else {
        VTEST_MSG_LOW(" Min undequed buffers are : %d", minUndequeuedBufs );
    }

    // Dequeue buffers and save them locally
    mBufMap = new BufMap(nBufferCount);
    *pBuffers = new BufferInfo[nBufferCount];

    if(*pBuffers==NULL)
    {
        VTEST_MSG_ERROR("BufferInfo creation failed with a buffer count %d", nBufferCount);
        return OMX_ErrorBadParameter;
    }
    memset(*pBuffers, 0, sizeof(BufferInfo) * nBufferCount);

    m_nBuffersWithNativeWindow = 0;
    OMX_U32 iterator =  nBufferCount - (minUndequeuedBufs + 1 + 2);
    // We dequeue only nBufferCount - (minUndequeuedBufs + 1 + 2) Buffers,
    // We should keep (minUndequeuedBufs + 1 + 2) buffers to NativeWindow
    for (OMX_U32 i = 0; i < iterator; i++) {
        ANativeWindowBuffer *buf;
        err = native_window_dequeue_buffer_and_wait(m_pNativeWindow.get(), &buf);
        if (err != 0) {
            VTEST_MSG_ERROR("dequeueBuffer failed: %s (%d)",
                    strerror(-err), -err);
            return OMX_ErrorUndefined; //TBD: cancel allocated ones
        }

        BufferInfo *pBuffer = &((*pBuffers)[i]);
        pBuffer->pHandle = (unsigned long)buf->handle;
        if (mBufMap->add(buf) != OMX_ErrorNone) {
            VTEST_MSG_ERROR("Failed to add buffer to local Map");
            err = m_pNativeWindow->cancelBuffer(m_pNativeWindow.get(), buf, -1);
            return OMX_ErrorUndefined;
        }

        VTEST_MSG_HIGH("%s alloc_ct=%u sz=%u handle=0x%lx hdr=(%p %p)",
                OMX_PORT_NAME(ePortIndex), (unsigned int)i + 1, (unsigned int)nBufferSize,
                pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::FreeAllocatedBuffers(BufferInfo **pBuffers,
        OMX_BUFFERHEADERTYPE **ppExtraDataBuffers, OMX_U32 nBufferCount,
        OMX_U32 nExtraDataBufferCount, OMX_U32 ePortIndex) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    /* Wait for all pending buffers to be queued first */
    {
        Mutex::Autolock autoLock(m_pLock);
        m_bFreeBuffers = OMX_TRUE;
    }

    /* Worst case this loop won't get hit if the buffer has already been
     * popped and is in the middle of ThreadRun.
     * Regardless all buffers will be cancelled again for which nativewindow
     * will throw a warning(not error) which is harmless.*/
    if (m_pBufferQueue->GetSize() > 0) {
        while (m_bFreeBuffers) {
            Sleeper::Sleep(1);
        }
    }

    {
        Mutex::Autolock autoLock(m_pLock);
        m_bFreeBuffers = OMX_FALSE;
    }

    if (mBufMap) {
        result = ISource::FreeAllocatedBuffers(pBuffers, ppExtraDataBuffers,
                    nBufferCount, nExtraDataBufferCount, ePortIndex);
        Mutex::Autolock autoLock(m_pLock);
        Cleanup();
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::PortReconfig(OMX_U32 ePortIndex,
        OMX_U32 nWidth, OMX_U32 nHeight, const OMX_CONFIG_RECTTYPE& sRect) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    Mutex::Autolock autoLock(m_pLock);
    VTEST_MSG_LOW("PortReconfig port %s", OMX_PORT_NAME(ePortIndex));

    if (m_sOutputRes.nWidth != nWidth || m_sOutputRes.nHeight != nHeight) {
        VTEST_MSG_LOW("PortReconfig WxH (%uX%u) ==> WxH (%uX%u)",
                               (unsigned int)m_sOutputRes.nWidth,
                              (unsigned int)m_sOutputRes.nHeight,
                                            (unsigned int)nWidth,
                                          (unsigned int)nHeight);
        m_sOutputRes.nWidth = nWidth;
        m_sOutputRes.nHeight = nHeight;
        if(m_pTempNV12Buf)
            free(m_pTempNV12Buf);
    } else {
        VTEST_MSG_LOW("PortReconfig crop ([%u %u] [%u %u])",
                (unsigned int)sRect.nLeft, (unsigned int)sRect.nTop,
                (unsigned int)(sRect.nLeft + sRect.nWidth),
                (unsigned int)(sRect.nTop + sRect.nHeight));

        android_native_rect_t crop;
        crop.left = sRect.nLeft;
        crop.top = sRect.nTop;
        crop.right = sRect.nLeft + sRect.nWidth;
        crop.bottom = sRect.nTop + sRect.nHeight;
        native_window_set_crop(m_pNativeWindow.get(), &crop);
    }

    m_sOutputRes.nStartX = sRect.nLeft;
    m_sOutputRes.nStartY = sRect.nTop;
    m_sOutputRes.nCropWidth  = sRect.nWidth;
    m_sOutputRes.nCropHeight = sRect.nHeight;
    if(QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m == m_sOutputRes.nColorFmt )
    {
        m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_NV12, m_sOutputRes.nWidth);
        m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, m_sOutputRes.nHeight);

    }
    else if(QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed== m_sOutputRes.nColorFmt )
    {
	m_sOutputRes.nStride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, m_sOutputRes.nWidth);
	m_sOutputRes.nScanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, m_sOutputRes.nHeight);

    }
    if (m_pSink != NULL) {
        result = m_pSink->PortReconfig(ePortIndex, nWidth, nHeight, sRect);
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::ThreadRun(OMX_PTR pThreadData) {

    (void)pThreadData;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    BufferInfo *pBuffer = NULL;
    OMX_BOOL bGotEOS = OMX_FALSE;
    VidcStatus ret = VidcStatusSuccess;
    BOOL bCurFrameCRC = FALSE;

    /* current limitation - the last buffer with EOS won't be displayed as
     * the thread will exit as soon as stop is called. One way would be to
     * hold stop until all buffers are cleared in the queue */
    for (OMX_U32 i = 1; i <= m_nFrames && !m_bThreadStop; i++) {

        BufferInfo *pFreeBuffer = NULL;
        OMX_BUFFERHEADERTYPE *pHeader = NULL, *pFreeHeader = NULL;

        {
            Mutex::Autolock autoLock(m_pLock);
            if (m_bFreeBuffers) {
                if (m_pBufferQueue->GetSize() == 0) {
                    m_bFreeBuffers = OMX_FALSE;
                }
            }
        }

        result = m_pBufferQueue->Pop(&pBuffer, sizeof(BufferInfo **), 0);
        VTEST_MSG_LOW("queue pop %u of %u (qsize %u)",
                      (unsigned int)i, (unsigned int)m_nFrames, (unsigned int)m_pBufferQueue->GetSize());

        if ((pBuffer == NULL) || (result != OMX_ErrorNone)) {
            /* Can only happen if stop is called or someone else ran into an
             * error */
            VTEST_MSG_HIGH("Stopping thread");
            result = OMX_ErrorNone;
            continue;
        }

        pHeader = pBuffer->pHeaderIn;

        if ((pHeader->nFlags & OMX_BUFFERFLAG_EOS) || (i >= m_nFrames)|| m_pSource->m_EOS3) {
            VTEST_MSG_HIGH("got EOS for frame : %u", (unsigned int)i);
            bGotEOS = OMX_TRUE;
            m_pSource->m_EOS3 = OMX_FALSE;
        }

        if (pHeader->nFilledLen > 0) {
            if(m_pCRCChecker)
            {
                private_handle_t *handle = NULL;
                VideoResolution res = m_sOutputRes;
                OMX_U8 *data = NULL;
                OMX_U8 *pdata = NULL;

                if(m_ePlaybackMode == DynamicBufferMode)
                {
                    MetaBuffer *meta = (MetaBuffer *)pHeader->pBuffer;
                    handle = (private_handle_t *)meta->meta_handle;
                }
                else
                {
                    handle = (private_handle_t *)pHeader->pBuffer;
                }

                pdata  = (OMX_U8*)mmap(0, handle->size,
                         PROT_READ|PROT_WRITE, MAP_SHARED, handle->fd, 0);

                if (pdata == MAP_FAILED)
                {
                    VTEST_MSG_ERROR("mmap failed for %p",pHeader->pBuffer);
                    result = OMX_ErrorUndefined;
                    SetError();
                    continue;
                }
                if(QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed  == m_sOutputRes.nColorFmt)
                {
                    result = UBWCtoNV12(pdata, &m_pTempNV12Buf, &res);
                    if ((OMX_ErrorNone != result) ||
                        (m_pTempNV12Buf == NULL))
                    {
                        VTEST_MSG_ERROR("UBWCtoNV12 conversion failed for %p",pHeader->pBuffer);
                        result = OMX_ErrorUndefined;
                        SetError();
                        continue;
                    }
                    data = m_pTempNV12Buf;
                }
                else
                {
                    data = pdata;
                }
                #ifdef PRE_SDM845
                if (!sGlobalStaticVideoProp.bSkipCrcCheck)
                {
                    ret = m_pCRCChecker->CheckCRCofYUVframe(m_pCRCChecker,
                                     data, &res, TRUE, &bCurFrameCRC);
                    if ( ret || (!bCurFrameCRC))
                    {
                        VTEST_MSG_HIGH("CRC failure %d  at Frame no: %d",ret, m_pCRCChecker->m_fCRCCounter);
                        result = OMX_ErrorUndefined;
                        SetError();
                        continue;
                    }
                    else if (bCurFrameCRC == TRUE && m_bThumbnailMode)
                    {
                        sGlobalStaticVideoProp.bSkipCrcCheck = OMX_TRUE;
                    }
                }
                #endif
                if (munmap(pdata, pHeader->nFilledLen))
                {
                    VTEST_MSG_ERROR("munmap failed for data : %p", data);
                }
            }

            result = EnqueueBuffer(pHeader);
            if (result != OMX_ErrorNone) {
                VTEST_MSG_ERROR("Error while enquing buffer : %p", pHeader);
                SetError();
                pFreeHeader = pHeader;
            } else if (!m_bShowVideo) {
                pFreeHeader = pHeader;
            }
        } else  {
            VTEST_MSG_HIGH("skipping frame because of 0 length %u...", (unsigned int)i);
            pFreeHeader = pHeader;
        }

        if ((pFreeHeader != NULL) && (m_bFreeBuffers)) {
            result = CancelBuffer(pFreeHeader);
            FAILED2(result, SetError(), "Error while cancelling buffer");
            pFreeHeader = NULL;
            }

        if ((pFreeHeader == NULL) && (m_nBuffersWithNativeWindow > m_nMinBufferCount) && !m_bFreeBuffers) {
            result = DequeueBuffer(pFreeHeader);
            FAILED2(result, SetError(), "Error while dequing buffer");
        }

        if (pFreeHeader != NULL) {
            if (m_pSink != NULL) {
                m_pBufferManager->GetBuffer(this, PORT_INDEX_OUT, pFreeHeader, &pFreeBuffer);
                // "pass-through" mode, pass the buffer to sink for "transcode"
                /* notify eos on the current buffer, because the buffer with eos
                 * flag will now always be with NativeWindow, and never dqd.
                 * Set it on headerOut, so that ISource can copy it to headerIn */
                if (bGotEOS) {
                    pFreeBuffer->pHeaderOut->nFlags |= OMX_BUFFERFLAG_EOS;
                    VTEST_MSG_HIGH("enable OMX_BUFFERFLAG_EOS on frame %u", (unsigned int)i);
                }
                m_pSink->SetBuffer(pFreeBuffer, this);
            } else {
                m_pBufferManager->GetBuffer(this, PORT_INDEX_IN, pFreeHeader, &pFreeBuffer);
                m_pSource->SetBuffer(pFreeBuffer, this);
            }
        }

        /* Delay setting threadstop to true until after major work like q/dq
         * has finished and the sink has gotten the eos */
        if (bGotEOS) {
            m_bThreadStop = OMX_TRUE;
        }
    }

    //clean up
    while(m_pBufferQueue->GetSize() > 0) {
        VTEST_MSG_LOW("Cleanup: q-wait (qsize %u)", (unsigned int)m_pBufferQueue->GetSize());
        m_pBufferQueue->Pop(&pBuffer, sizeof(BufferInfo **), 0);
        if (m_pSink != NULL) {
            BufferInfo *pFreeBuffer = NULL;
            /* "pass-through" mode, pass the buffer to sink for "transcode"
               get the corresponding pBuffer from the other pool with output header*/
            m_pBufferManager->GetBuffer(this, PORT_INDEX_OUT,
                    pBuffer->pHeaderOut, &pFreeBuffer);
            m_pSink->SetBuffer(pFreeBuffer, this);
        } else {
            m_pSource->SetBuffer(pBuffer, this);
        }
    }

    VTEST_MSG_HIGH("thread is exiting...");
    return result;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::FreeBuffer(
        BufferInfo *pBuffer, OMX_U32 ePortIndex) {

    (void)ePortIndex;
    OMX_ERRORTYPE result = OMX_ErrorNone;

    VTEST_MSG_MEDIUM("FreeBuffer: 0x%lu pHeader: (%p %p)",
            pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);

    if ((pBuffer->pHandle == 0)
            || (mBufMap->mCurrentCount == 0)) {
        return result;
    }
    mBufMap->mCurrentCount--;

    /*TODO: Should only cancelbuffer if you have it */
    result = CancelBuffer(pBuffer->pHeaderIn);
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::QueryAllocatedNativeBuffer(
        OMX_U32 nIndex, ANativeWindowBuffer *& nativeBuf) {

    if ((mBufMap == NULL) || (nIndex >= mBufMap->GetSize())) {
        return OMX_ErrorUndefined;
    }
    nativeBuf = (*mBufMap)[nIndex];
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::RegisterBuffer(
        OMX_U8 *pBufferHandle, OMX_BUFFERHEADERTYPE *pBuffer) {

    if (mBufMap == NULL) return OMX_ErrorUndefined;
    if (pBufferHandle == NULL || pBuffer == NULL) return OMX_ErrorUndefined;
    return mBufMap->map(pBufferHandle, pBuffer);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::CancelBuffer(OMX_BUFFERHEADERTYPE *pHeader) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    ANativeWindowBuffer *buf = NULL;
    int err;

    result = FindBuffer(pHeader, &buf);
    FAILED1(result, "Failed to find buffer : %p", pHeader);

    VTEST_MSG_HIGH("Dropping buffer [%p]", buf);
    err = m_pNativeWindow->cancelBuffer(m_pNativeWindow.get(), buf, -1);
    if (err != 0) {
        VTEST_MSG_ERROR("cancelBuffer failed w/ error 0x%08x", err);
        return OMX_ErrorUndefined;
    }
    m_nBuffersWithNativeWindow++;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::DequeueBuffer(OMX_BUFFERHEADERTYPE *& bufHdr) {

    ANativeWindowBuffer *buf;
    int err = native_window_dequeue_buffer_and_wait(m_pNativeWindow.get(), &buf);
    if (err != 0) {
        VTEST_MSG_ERROR("dequeueBuffer failed w/ error 0x%08x", err);
        return OMX_ErrorUndefined;
    }

    if (mBufMap == NULL) {
        FAILED1(OMX_ErrorUndefined, "mBufMap not allocated");
    }

    bufHdr = mBufMap->find(buf);
    if (bufHdr == NULL) {
        VTEST_MSG_MEDIUM("Adding nativeBuffer=%p to BufMap", buf);

        OMX_U32 nBufferCount;
        BufferInfo *pBuffers;
        OMX_ERRORTYPE result = m_pBufferManager->GetBuffers(this, PORT_INDEX_IN, &pBuffers, &nBufferCount);
        FAILED1(result, "Error in Getting Buffers");

        for ( OMX_U32 k = 0; k < nBufferCount; ++k){
            BufferInfo *pBuffer = &(pBuffers[k]);
            if(!pBuffer->pHandle)
            {
                pBuffer->pHandle = (unsigned long)buf->handle;

                if (mBufMap->add(buf) != OMX_ErrorNone) {
                    VTEST_MSG_ERROR("Failed to add buffer to local Map");
                    return OMX_ErrorUndefined;
                }
                VTEST_MSG_HIGH("%s alloc_ct=%u handle=0x%lx hdr=(%p %p)",
                OMX_PORT_NAME(PORT_INDEX_IN), (unsigned int)k + 1,
                pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);

                if(m_ePlaybackMode == DynamicBufferMode)
                {
                    MetaBuffer *meta = new MetaBuffer;
                    meta->meta_handle = (NativeHandle *) pBuffer->pHandle;
                    pBuffer->pHeaderOut->pBuffer  = (OMX_U8 *) meta;
                }
                else
                {
                    pBuffer->pHeaderOut->pBuffer = (OMX_U8 *) pBuffer->pHandle;
                }
                result = m_pSource->SetBuffer(pBuffer, this);
                if (result != OMX_ErrorNone && result != (OMX_ERRORTYPE)VTEST_ErrorThreadStop) {
                    VTEST_MSG_ERROR("SetBuffer on source failed for pBuffer: %p", pBuffer);
                    return result;
                }
                return OMX_ErrorNone;
            }
        }
        return OMX_ErrorUndefined;
    }
    m_nBuffersWithNativeWindow--;
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::EnqueueBuffer(OMX_BUFFERHEADERTYPE *pHeader) {

    OMX_ERRORTYPE result;
    ANativeWindowBuffer *buf = NULL;
    int err;

    result = FindBuffer(pHeader, &buf);
    FAILED1(result, "Failed to find buffer : %p", pHeader);

    if (!m_bShowVideo) {
        VTEST_MSG_LOW("ShowVideo is OFF, not enqueing buffer");
        return OMX_ErrorNone;
    }

    VTEST_MSG_MEDIUM("Displaying buffer [%p]", buf);
    err = m_pNativeWindow->queueBuffer(m_pNativeWindow.get(), buf, -1);
    if (err != 0) {
        VTEST_MSG_ERROR("queueBuffer failed w/ error 0x%08x", err);
        return OMX_ErrorUndefined;
    }
    m_nBuffersWithNativeWindow++;
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::FindBuffer(
        OMX_BUFFERHEADERTYPE *pHeader, ANativeWindowBuffer **pNativeBuf) {

    OMX_ERRORTYPE result;
    int bufferindex;

    if (mBufMap == NULL) {
        FAILED1(OMX_ErrorUndefined, "mBufMap not allocated");
    }

    if (pNativeBuf == NULL) {
        FAILED1(OMX_ErrorUndefined, "Native Buffer NULL");
    }

    *pNativeBuf = mBufMap->find(pHeader);
    if (*pNativeBuf == NULL) {

        Mutex::Autolock autoLock(m_pLock);
        BufferInfo *pBuffer;
        VTEST_MSG_MEDIUM("BufHdr=%p has no mapping, creating one", pHeader);

        bufferindex = m_pBufferManager->GetBuffer(this, PORT_INDEX_IN, pHeader, &pBuffer);

        result = RegisterBuffer((OMX_U8 *)pBuffer->pHandle, pHeader);
        *pNativeBuf = mBufMap->find(pHeader);
        if ((result != OMX_ErrorNone) || (*pNativeBuf == NULL)) {
            VTEST_MSG_ERROR("Could not register buffer : %p", pHeader);
            return OMX_ErrorUndefined;
        }
    }
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void NativeWindow::Cleanup() {

    VTEST_MSG_MEDIUM("NativeWindow::cleanup");
    if (mBufMap) {
        OMX_U32 count = mBufMap->GetSize();
        for (OMX_U32 i = 0; i < count; ++i) {
            ANativeWindowBuffer *nativeBuf = NULL;
            VTEST_MSG_LOW("removing buf=%u of %u", (unsigned int)i, (unsigned int)count - 1);
            mBufMap->removeLast(nativeBuf);
            if (nativeBuf != NULL) {
                //TBD: cancelbuf if not owned by ANW ??
                nativeBuf = NULL;
            }
        }
        delete mBufMap;
        mBufMap = NULL;
        VTEST_MSG_MEDIUM("NativeWindow::cleanup done");
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void NativeWindow::FixScaling(OMX_U32 transform,
        OMX_U32& w, OMX_U32& h, OMX_U32& x, OMX_U32& y) {

    //handles only 0 and 90 for now !
    double dstW = (transform == 0) ? m_nDisplayWidth : m_nDisplayHeight;
    double dstH = (transform == 0) ? m_nDisplayHeight : m_nDisplayWidth;

    double scaleX_x1000 = (dstW * 1000) / m_sOutputRes.nWidth;
    double scaleY_x1000 = (dstH * 1000) / m_sOutputRes.nHeight;

    //maintain the aspect ratio
    double scale_x1000 = (scaleX_x1000 < scaleY_x1000) ? scaleX_x1000 : scaleY_x1000;

    //scale down
    w = (scale_x1000 * m_sOutputRes.nWidth) / 1000;
    h = (scale_x1000 * m_sOutputRes.nHeight) / 1000;
    w = (w > dstW) ? dstW : w;
    h = (h > dstH) ? dstH : h;

    x = (dstW - w) / 2;
    y = (dstH - h) / 2;

    //flip back for 90 deg transform
    if (transform != 0) {
        OMX_U32 temp = w;
        w = h;
        h = temp;

        temp = x;
        x = y;
        y = temp;
    }
    VTEST_MSG_HIGH("Scaled rect : [%u x %u] @(%u, %u)", (unsigned int)w, (unsigned int)h, (unsigned int)x, (unsigned int)y);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::UBWCtoNV12(uint8 *pData, uint8 **bufNV12, VideoResolution* res)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    VidcStatus result = VidcStatusSuccess;

    if ((NULL == pData)||(NULL == bufNV12)||(NULL == res))
    {
        VTEST_MSG_ERROR("UBWCtoNV12: pData=%p, bufNV12=%p, res = %p", pData, bufNV12,res);
        return OMX_ErrorBadParameter;
    }

    VTEST_MSG_HIGH("Converting buffer %p from UBWC to NV12",pData);

    if (!m_hUBWC)
    {
        m_hUBWC =  pFnUBWCConvertInit();
    }
    else
    {
        pFnUBWCConvertInitZero(m_hUBWC);
    }
    if (!m_pUBWCConfig)
    {
        m_pUBWCConfig = pFnUBWCConfigInit();
    }
    else
    {
        pFnUBWCConfigInitZero(m_pUBWCConfig);
    }
    if (!m_hUBWC || !m_pUBWCConfig)
    {
        VTEST_MSG_ERROR( "Allocation Failure pDec->hUBWC=%p pDec->pUBWCConfig=%p", m_hUBWC, m_pUBWCConfig);
        return OMX_ErrorUndefined;
    }
    pFnUBWCConvertConfigure(m_hUBWC, FORMAT_NV12_NV21);

    result = (pFnConvertUBWCToNV12(m_hUBWC, m_pUBWCConfig, pData, bufNV12, res));
    if (result)
    {
        VTEST_MSG_ERROR( "ConvertUBWCToNV12 failed in conversion\n");
        status = OMX_ErrorUndefined;
    }
    res->nColorFmt = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;

    return status;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
NativeWindow::BufMap::BufMap(const OMX_U32 limit)
    : mCurrentCount(0),
      mCount(0),
      mLimit(limit) {

    mNativeList = (ANativeWindowBuffer **)
                malloc(sizeof(ANativeWindowBuffer *) * limit);
    if (mNativeList)
        memset(mNativeList, 0x00, sizeof(ANativeWindowBuffer *) * limit);

    mBufHdrList = (OMX_BUFFERHEADERTYPE **)
                malloc(sizeof(OMX_BUFFERHEADERTYPE *) * limit);
    if (mBufHdrList)
        memset(mBufHdrList, 0x00, sizeof(OMX_BUFFERHEADERTYPE *) * limit);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
NativeWindow::BufMap::~BufMap() {

    //assert(mCount == 0)
    free(mNativeList);
    free(mBufHdrList);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::BufMap::add(ANativeWindowBuffer *nativeBuf) {

    if (mCount >= mLimit) {
        VTEST_MSG_ERROR("BufMap::add : exceeded limit !!");
        return OMX_ErrorUndefined;
    }

    mNativeList[mCount] = nativeBuf;
    mBufHdrList[mCount] = NULL;
    ++mCount;
    ++mCurrentCount;

    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::BufMap::map(
        OMX_U8 *pBufferHandle, OMX_BUFFERHEADERTYPE *pHeader) {

    for (OMX_U32 i = 0; i < mCount; ++i) {
        if (mNativeList[i]->handle ==
                (reinterpret_cast<buffer_handle_t>(pBufferHandle))) {
            assert(mBufHdrList[i] == NULL);
            VTEST_MSG_HIGH("Associating NativeBuf=%p <--> BufHdr=%p",
                    mNativeList[i], pHeader);
            mBufHdrList[i] = pHeader;
            return OMX_ErrorNone;
        }
    }
    VTEST_MSG_ERROR("Cannot map bufHdr %p ", pHeader);
    return OMX_ErrorBadParameter;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE NativeWindow::BufMap::removeLast(
        ANativeWindowBuffer *& nativeBuf) {

    nativeBuf = NULL;
    if (mCount == 0) {
        VTEST_MSG_ERROR("BufMap::removeLast : Queue is empty!!");
        return OMX_ErrorUndefined;
    }

    --mCount;
    nativeBuf = mNativeList[mCount];
    mNativeList[mCount] = NULL;
    mBufHdrList[mCount] = NULL;
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ANativeWindowBuffer* NativeWindow::BufMap::find(
        OMX_BUFFERHEADERTYPE *pHeader) {

    for (OMX_U32 i = 0; i < mCount; ++i) {
        if (mBufHdrList[i] == pHeader) {
            return mNativeList[i];
        }
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_BUFFERHEADERTYPE* NativeWindow::BufMap::find(ANativeWindowBuffer *buf) {

    for (OMX_U32 i = 0; i < mCount; ++i) {
        if (mNativeList[i] == buf) {
            return mBufHdrList[i];
        }
    }
    return NULL;
}

}
