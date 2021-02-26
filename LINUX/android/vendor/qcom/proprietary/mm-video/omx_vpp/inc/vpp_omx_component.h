/* =========================================================================
Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_omx_component.h
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

#ifndef VPP_OMX_COMPONENT_H
#define VPP_OMX_COMPONENT_H
//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <cutils/properties.h>
#ifndef PC_DEBUG
#include <semaphore.h>
#endif

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_IndexExt.h"
#include "qc_omx_component.h"
#include "vidc_vendor_extensions.h"
#include "vpp_buffer_manager.h"

#include "VppClient.h"

extern "C" {
#include <utils/Log.h>
#include "vpp.h"
OMX_API void * get_omx_component_factory_fn(void); //used by qc_omx_core
}

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "OMX_VPP"

typedef void * (*create_qc_omx_component)(void);

using namespace android;

class VppOmxComponent : public qc_omx_component
{
private:

    friend class VppBufferManager;

    enum port_indexes {
       OMX_CORE_INPUT_PORT_INDEX        =0,
       OMX_CORE_OUTPUT_PORT_INDEX       =1,
       OMX_CORE_INPUT_EXTRADATA_INDEX   =2,
       OMX_CORE_OUTPUT_EXTRADATA_INDEX  =3
    };

    // Vpp Flush and Reconfigure event masks
    enum VppEventPorts {
       VPP_NONE,
       VPP_INPUT,
       VPP_OUTPUT,
       VPP_ALL
    };

public:

    VppOmxComponent();
    // this is critical, otherwise, sub class destructor will not be called
    virtual ~VppOmxComponent();

    // Initialize the component after creation
    virtual OMX_ERRORTYPE component_init(OMX_IN OMX_STRING componentName);

    /*******************************************************************/
    /*           Standard OpenMAX Methods                              */
    /*******************************************************************/

    // Query the component for its information
    virtual
    OMX_ERRORTYPE  get_component_version(OMX_HANDLETYPE       cmp_handle,
                                       OMX_STRING             cmp_name,
                                       OMX_VERSIONTYPE*    cmp_version,
                                       OMX_VERSIONTYPE*   spec_version,
                                       OMX_UUIDTYPE*          cmp_UUID);

    // Invoke a command on the component
    virtual
    OMX_ERRORTYPE  send_command(OMX_HANDLETYPE cmp_handle,
                              OMX_COMMANDTYPE       cmd,
                              OMX_U32            param1,
                              OMX_PTR          cmd_data);

    // Get a Parameter setting from the component
    virtual
    OMX_ERRORTYPE  get_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data);

    // Send a parameter structure to the component
    virtual
    OMX_ERRORTYPE  set_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data);

    // Get a configuration structure from the component
    virtual
    OMX_ERRORTYPE  get_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data);

    // Set a component configuration value
    virtual
    OMX_ERRORTYPE  set_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data);

    // Translate the vendor specific extension string to
    // standardized index type
    virtual
    OMX_ERRORTYPE  get_extension_index(OMX_HANDLETYPE  cmp_handle,
                                     OMX_STRING       paramName,
                                     OMX_INDEXTYPE*   indexType);

    // Get Current state information
    virtual
    OMX_ERRORTYPE  get_state(OMX_HANDLETYPE  cmp_handle,
                           OMX_STATETYPE*       state);

    // Component Tunnel Request
    virtual
    OMX_ERRORTYPE  component_tunnel_request(OMX_HANDLETYPE           cmp_handle,
                                          OMX_U32                        port,
                                          OMX_HANDLETYPE       peer_component,
                                          OMX_U32                   peer_port,
                                          OMX_TUNNELSETUPTYPE*   tunnel_setup);

    // Use a buffer already allocated by the IL client
    // or a buffer already supplied by a tunneled component
    virtual
    OMX_ERRORTYPE  use_buffer(OMX_HANDLETYPE                cmp_handle,
                            OMX_BUFFERHEADERTYPE**        buffer_hdr,
                            OMX_U32                             port,
                            OMX_PTR                         app_data,
                            OMX_U32                            bytes,
                            OMX_U8*                           buffer);


    // Request that the component allocate new buffer and associated header
    virtual
    OMX_ERRORTYPE  allocate_buffer(OMX_HANDLETYPE                cmp_handle,
                                 OMX_BUFFERHEADERTYPE**        buffer_hdr,
                                 OMX_U32                             port,
                                 OMX_PTR                         app_data,
                                 OMX_U32                            bytes);

    // Release the buffer and associated header from the component
    virtual
    OMX_ERRORTYPE  free_buffer(OMX_HANDLETYPE         cmp_handle,
                             OMX_U32                      port,
                             OMX_BUFFERHEADERTYPE*      buffer);

    // Send a filled buffer to an input port of a component
    virtual
    OMX_ERRORTYPE  empty_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                   OMX_BUFFERHEADERTYPE*      buffer);

    // Send an empty buffer to an output port of a component
    virtual
    OMX_ERRORTYPE  fill_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                  OMX_BUFFERHEADERTYPE*      buffer);

    // Set callbacks
    virtual
    OMX_ERRORTYPE  set_callbacks( OMX_HANDLETYPE        cmp_handle,
                                OMX_CALLBACKTYPE*      callbacks,
                                OMX_PTR                 app_data);

    // Component De-Initialize
    virtual
    OMX_ERRORTYPE  component_deinit(OMX_HANDLETYPE cmp_handle);

    // Use the Image already allocated via EGL
    virtual
    OMX_ERRORTYPE  use_EGL_image(OMX_HANDLETYPE                cmp_handle,
                               OMX_BUFFERHEADERTYPE**        buffer_hdr,
                               OMX_U32                             port,
                               OMX_PTR                         app_data,
                               void*                          egl_image);

    // Component Role enum
    virtual
    OMX_ERRORTYPE  component_role_enum( OMX_HANDLETYPE cmp_handle,
                                      OMX_U8*              role,
                                      OMX_U32             index);

private:

    // To load omx code so and create component
    OMX_ERRORTYPE loadOmxDecoder();

    // callbacks from component
    static OMX_CALLBACKTYPE   sOmxCallbacks;

    static OMX_ERRORTYPE OmxEventHandler(OMX_HANDLETYPE pHandle,
                                      OMX_PTR pAppData,
                                      OMX_EVENTTYPE eEvent,
                                      OMX_U32 nData1,
                                      OMX_U32 nData2,
                                      OMX_PTR pEventData);

    static OMX_ERRORTYPE OmxEmptyBufferDone(OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_BUFFERHEADERTYPE* pBufHdr);

    static OMX_ERRORTYPE OmxFillBufferDone(OMX_HANDLETYPE hComponent,
                                        OMX_PTR pAppData,
                                        OMX_BUFFERHEADERTYPE* pBufHdr);

    // Callbacks from VPP
    static void VppEmptyBufferDone(void *handle, struct vpp_buffer *buf);
    static void VppOutputBufferDone(void *handle, struct vpp_buffer *buf);
    static void VppEvent(void *handle, struct vpp_event e);

    OMX_ERRORTYPE SubmitBuffer(VppBufferManager::OmxVppBuffer *omxVppBuf,
                               OMX_BUFFERHEADERTYPE *buffer);

    OMX_ERRORTYPE set_codec_info(OMX_STRING cmpName);
    inline void printHQVControl(hqv_control &hqvControl);
    void print_debug_extradata(OMX_OTHER_EXTRADATATYPE *extra);
    bool updateParameterFromOutPort(bool isQuery, void* portDef);
    bool enableVppInternalExtraData();

    // set up omx vpp extenion list
    void initOmxVppExtensions(VendorExtensionStore &store);
    // get one vpp extension
    OMX_ERRORTYPE getOmxVppExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *ext);
    // set one vpp extension
    OMX_ERRORTYPE setOmxVppExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *ext);

    void VppTerm();

    OMX_ERRORTYPE useBuffer(OMX_BUFFERHEADERTYPE** pBufHdr, OMX_U32 port,
            OMX_PTR app_data, OMX_U32 bytes, OMX_U8* buffer);
    void copyInputToOmx(OMX_BUFFERHEADERTYPE *pBufHdr);


private:
    OMX_STATETYPE         mState;
    OMX_PTR               mAppData;
    OMX_CALLBACKTYPE      mOmxCallbacks;
    sem_t                 mFlushEventLock;
    sem_t                 mReconfigEventLock;
    sem_t                 mEosEventLock;
    Mutex                 mDecoderSyncLock;
    Mutex                 mVppSyncLock;
    Mutex                 mFbdSyncLock;
    Mutex                 mFlushLock;
    Mutex                 mEosSyncLock;

    // OMX component for vdec
    OMX_PTR                   mOmxHandle;
    char                      mOmxRole[OMX_MAX_STRINGNAME_SIZE];

    // OMX Core
    void*                     mOmxDecoderLib;
    create_qc_omx_component   mCreateOmxFn;

    bool                    mIsOutputDisabled;
    bool                    mDecoderReconfig;
    bool                    mFlushing;
    std::atomic<bool>       mReconfigPending;
    bool                    mWaitForEOS;
    bool                    mEosReached;
    uint8_t                 mVppFlushStatus;
    // vpp_library
    struct vpp_requirements mVppRequirements;
    void*                   mVppContext;
    sp<VppClient>           mVppClient;
    uint32_t                mVppFlags;
    bool                    mVppBypassMode;
    uint8_t                 mVppExtradataMode;
    OMX_U32                 mVppColorFormat;
    struct vpp_port_param   mVppPortParam;
    struct hqv_control      mVppHqvControl;
    struct vpp_callbacks    sVppCallbacks;
    VppBufferManager        mBufferManager;
    struct vpp_port_param   mDecRcvdParamData;
    // Number of decoder extensions, vpp extension index is based on it
    uint32_t                 mDecoderExtensionNum;
    // Indicate whether vpp is in active status
    bool                    mIsVppActive;
    // Indicate whether decoder is setup
    bool                    mIsDecoderSetup;
    // vpp vendor extensions
    const VendorExtensionStore mOmxVppVendorExtensionStore;
    // struct to keep the extension values
    struct VppHqvCtrl {
        hqv_mode mode;
        struct hqv_ctrl_cade cade;
        struct hqv_ctrl_cnr cnr;
        struct hqv_ctrl_aie aie;
        struct hqv_ctrl_di di;
        struct hqv_ctrl_frc frc;
        struct hqv_ctrl_ear ear;
        struct hqv_ctrl_qbr qbr;
        struct hqv_ctrl_meas meas;
        struct hqv_ctrl_global_demo demo;
    } mVppHqvCtrlValue;

    bool mIsQCSoftwareDecoder;
    uint32_t mPixelSize;

    // In dynamic loading mode of omx vpp, client side would allocate
    // input buffer and trigger "use_buffer" mode for input port of decoder.
    // But software decoder doesn't support "use_buffer" for input port.
    // So omx vpp will handle it by using "allocate_buffer" for software
    // decoder and copy the input buffer from client to decoder.
    // This logic is controlled by mCopyInputToOmx and will only be enabled
    // in dynamic loading case with software decoder.
    bool mCopyInputToOmx;
    struct BufferMeta {
        uint8_t *clientBuf;  // the input buffer in client side
        uint8_t *omxBuf;     // the input buffer in decoder side
    };
    Vector<BufferMeta> mBufferMetas;
};

#endif /* VPP_OMX_COMPONENT_H */
