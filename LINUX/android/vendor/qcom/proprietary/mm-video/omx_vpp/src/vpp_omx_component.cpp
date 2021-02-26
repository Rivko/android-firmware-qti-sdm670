/* =========================================================================
Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2009, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/
/*============================================================================
                            O p e n M A X   w r a p p e r s
                O p e n  M A X   C o m p o n e n t  I n t e r f a c e

*//** @file vpp_omx_component.cpp
  This module contains the abstract interface for the OpenMAX components.

*//*========================================================================*/

//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////
#include <dlfcn.h>
#include <inttypes.h>
#include <sys/prctl.h>
#include <gralloc_priv.h>
#include <media/hardware/HardwareAPI.h>
#include <media/msm_media_info.h>
#include "vpp_omx_component.h"
#include "omx_core_cmp.h"
#include "qc_omx_core.h"
#include "OMX_QCOMExtns.h"
#include "OMX_IndexExt.h"

#define _ANDROID_
#include "vidc_debug.h"
#undef _ANDROID_

#define QC_OMX_HW_DECODER_LIBRARY "libOmxVdec.so"
#define QC_OMX_SW_DECODER_LIBRARY "libOmxSwVdec.so"
#define ARG_TOUCH(x) (void)x

#define OMX_VPP_PROP_ENABLE             "vendor.media.vpp.enable"
#define OMX_VPP_PROP_DEFAULT_CONFIG     "vendor.media.vpp.default.config"
#define OMX_VPP_PROP_DEBUG_LVL          "vendor.vpp.debug.level"
#define OMX_VPP_PROP_DEBUG_EXT          "vendor.vpp.debug.ext"
#define INT32_PTR(v) (int32_t *)(&(v))
// define the key string for vpp extension parameters
// Hqv mode
#define MODE "mode"
#define CMP_NAME "cmp"
// Global Demo
#define PROCESS_PERCENT "process-percent"
#define PROCESS_SIDE "process-side"
// CADE
#define CADE_MODE MODE
#define CADE_LEVEL "cade-level"
#define CADE_CONTRAST "contrast"
#define CADE_SATURATION "saturation"
// DI
#define DI_MODE MODE
// CNR
#define CNR_MODE MODE
#define CNR_LEVEL "level"
// AIE
#define AIE_MODE MODE
#define HUE_MODE "hue-mode"
#define LTM_LEVEL "ltm-level"
#define LTM_SAT_GAIN "ltm-sat-gain"
#define LTM_SAT_OFFSET "ltm-sat-offset"
#define LTM_ACE_STR "ltm-ace-str"
#define LTM_ACE_BRIGHTNESS_L "ltm-ace-brightness-low"
#define LTM_ACE_BRIGHTNESS_H "ltm-ace-brightness-high"
// FRC
#define FRC_MODE MODE
#define FRC_LEVEL "level"
#define FRC_INTERP "interp"
// EAR
#define EAR_MODE MODE
// QBR
#define QBR_MODE MODE
// MEAS
#define MEAS_MODE MODE

using namespace std;
using OmxBufferOwner = VppBufferManager::OmxBufferOwner;
using BufferCounts = VppBufferManager::BufferCounts;

// static variable
OMX_CALLBACKTYPE VppOmxComponent::sOmxCallbacks = {
    &OmxEventHandler, &OmxEmptyBufferDone, &OmxFillBufferDone
};

// For runtime log level changes
int debug_level;
int32_t debug_ext;

// factory function executed by the core to create instances
void *get_omx_component_factory_fn(void)
{
    return (new VppOmxComponent);
}

void* omx_message_thread(void *input);
void post_message(VppOmxComponent *omx, unsigned char id);

extern omx_core_cb_type core[];
extern const unsigned int SIZE_OF_CORE;

template<class T>
static void InitOMXParams(T &params) {
    memset(&params, 0, sizeof(T));
    params.nSize = sizeof(T);
    params.nVersion.s.nVersionMajor = 1;
    params.nVersion.s.nVersionMinor = 0;
    params.nVersion.s.nRevision = 0;
    params.nVersion.s.nStep = 0;
}

// Translate omx color format to vpp color format
// As the supported color formats are NV12, NV12_UBWC, TP10_UBWC,
// other omx color format would be transfered to VPP_COLOR_FORMAT_MAX.
static enum vpp_color_format getVppColor(uint32_t omxColor) {
    switch(omxColor) {
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m:
            return VPP_COLOR_FORMAT_NV12_VENUS;
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed:
            return VPP_COLOR_FORMAT_UBWC_NV12;
        case QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus:
            return VPP_COLOR_FORMAT_P010;
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m10bitCompressed:
            return VPP_COLOR_FORMAT_UBWC_TP10;
        default:
            return VPP_COLOR_FORMAT_MAX;
    }
}

static void getVenusSize(uint32_t &pixelSize, uint32_t &stride, uint32_t &slice,
        uint32_t omxColor, uint32_t width, uint32_t height) {
    enum color_fmts color;
    switch(omxColor) {
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m:
          color = COLOR_FMT_NV12;
          break;
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed:
          color = COLOR_FMT_NV12_UBWC;
          break;
        case QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus:
          color = COLOR_FMT_P010;
          break;
        case QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m10bitCompressed:
          color = COLOR_FMT_NV12_BPP10_UBWC;
          break;
        default:
          return;
    }
    uint32_t y_stride = VENUS_Y_STRIDE(color, width);
    uint32_t y_slice = VENUS_Y_SCANLINES(color, height);
    uint32_t uv_stride = VENUS_UV_STRIDE(color, width);
    uint32_t uv_slice = VENUS_UV_SCANLINES(color, height);
    stride = y_stride;
    slice = y_slice;
    if (color == COLOR_FMT_NV12
            || color == COLOR_FMT_P010) {
        pixelSize = y_stride * y_slice + uv_stride * uv_slice;
        pixelSize = MSM_MEDIA_ALIGN(pixelSize, 4096);
    } else if (color == COLOR_FMT_NV12_UBWC
            || color == COLOR_FMT_NV12_BPP10_UBWC) {
        uint32_t y_meta_stride = VENUS_Y_META_STRIDE(color, width);
        uint32_t y_meta_slice = VENUS_Y_META_SCANLINES(color, height);
        uint32_t uv_meta_stride = VENUS_UV_META_STRIDE(color, width);
        uint32_t uv_meta_slice = VENUS_UV_META_SCANLINES(color, height);
        pixelSize = MSM_MEDIA_ALIGN(y_stride * y_slice, 4096)
                + MSM_MEDIA_ALIGN(uv_stride * uv_slice, 4096)
                + MSM_MEDIA_ALIGN(y_meta_stride * y_meta_slice, 4096)
                + MSM_MEDIA_ALIGN(uv_meta_stride * uv_meta_slice, 4096);
        pixelSize = MSM_MEDIA_ALIGN(pixelSize, 4096);
    }
}

static const vector<vector<string>> EnumStringList = {
    {"HQV_MODE_OFF", "HQV_MODE_AUTO", "HQV_MODE_MANUAL"},
    {"DI_MODE_OFF", "DI_MODE_VIDEO_1F", "DI_MODE_VIDEO_3F", "DI_MODE_VIDEO_AUTO"},
    {"HQV_HUE_MODE_OFF", "HQV_HUE_MODE_ON"},
    {"FRC_MODE_OFF", "FRC_MODE_SMOOTH_MOTION", "FRC_MODE_SLOMO"},
    {"FRC_LEVEL_OFF", "FRC_LEVEL_LOW", "FRC_LEVEL_MEDIUM", "FRC_LEVEL_HIGH"},
    {"FRC_INTERP_1X", "FRC_INTERP_2X", "FRC_INTERP_3X", "FRC_INTERP_4X"},
    {"EAR_MODE_OFF", "EAR_MODE_BYPASS", "EAR_MODE_LOW", "EAR_MODE_MEDIUM", "EAR_MODE_HIGH",
     "EAR_MODE_ADAPTIVE_STREAM", "EAR_MODE_ADAPTIVE_FRAME"},
    {"QBR_MODE_OFF", "QBR_MODE_ON"},
    {"MEAS_MODE_OFF", "MEAS_MODE_ON"},
    {"PROCESS_LEFT", "PROCESS_RIGHT", "PROCESS_TOP", "PROCESS_BOTTOM"},
};

enum MapIndex {
    MAP_INDEX_MODE,
    MAP_INDEX_CADE_MODE = MAP_INDEX_MODE,
    MAP_INDEX_CNR_MODE = MAP_INDEX_MODE,
    MAP_INDEX_AIE_MODE = MAP_INDEX_MODE,
    MAP_INDEX_DI_MODE,
    MAP_INDEX_HUE_MODE,
    MAP_INDEX_FRC_MODE,
    MAP_INDEX_FRC_LEVEL,
    MAP_INDEX_FRC_INTERP,
    MAP_INDEX_EAR_MODE,
    MAP_INDEX_QBR_MODE,
    MAP_INDEX_MEAS_MODE,
    MAP_INDEX_PROCESS_MODE,
    MAP_INDEX_MAX,
};

template <class T>
static const char* getStringFromEnum(T value, enum MapIndex index) {
    if (index < MAP_INDEX_MAX) {
        const vector<string> &cVector = EnumStringList[index];
        if ((size_t)value < cVector.size()) {
            if (debug_ext)
                DEBUG_PRINT_LOW("get string %s from %u", cVector[value].c_str(), value);
            return cVector[value].c_str();
        }
    }
    DEBUG_PRINT_LOW("failed to get string from %u", value);
    return NULL;
}

template <class T>
static bool getEnumFromString(char *str, T &value, enum MapIndex index) {
    if (index < MAP_INDEX_MAX) {
        const vector<string> &cVector = EnumStringList[index];
        for (size_t i = 0; i < cVector.size(); i++) {
            if (!strncasecmp(str, cVector[i].c_str(), OMX_MAX_STRINGNAME_SIZE)) {
                value = (T)i;
                if (debug_ext)
                    DEBUG_PRINT_LOW("get enum %u from %s", value, str);
                return true;
            }
        }
    }
    DEBUG_PRINT_LOW("failed to get enum value from %s", str);
    return false;
}

VppOmxComponent::VppOmxComponent()
    : mState(OMX_StateInvalid),
      mAppData(NULL),
      mOmxHandle(NULL),
      mOmxRole{},
      mOmxDecoderLib(NULL),
      mCreateOmxFn(NULL),
      mIsOutputDisabled(false),
      mDecoderReconfig(false),
      mFlushing(false),
      mReconfigPending(false),
      mWaitForEOS(false),
      mEosReached(false),
      mVppFlushStatus(0),
      mVppContext(NULL),
      mVppClient(new VppClient),
      mVppFlags(0),
      mVppBypassMode(false),
      mVppExtradataMode(NO_EXTRADATA),
      mVppColorFormat(OMX_COLOR_FormatUnused),
      mBufferManager(*this),
      mDecoderExtensionNum(0),
      mIsVppActive(false),
      mIsDecoderSetup(false),
      mVppHqvCtrlValue(),
      mIsQCSoftwareDecoder(false),
      mPixelSize(0),
      mCopyInputToOmx(false)
{
    memset(&m_cmp, 0, sizeof(m_cmp));
    memset(&mOmxCallbacks,0,sizeof(mOmxCallbacks));
    sem_init(&mFlushEventLock,0,0);
    sem_init(&mReconfigEventLock,0,0);
    sem_init(&mEosEventLock,0,0);

    //Initializing the vppLibray specific structures
    memset(&mVppRequirements, 0, sizeof(mVppRequirements));
    memset(&mVppPortParam, 0, sizeof(mVppPortParam));
    sVppCallbacks = {(void*)this, &VppEmptyBufferDone, &VppOutputBufferDone, &VppEvent};

    //Set HQV_CONTROL as AUTO
    memset(&mVppHqvControl, 0, sizeof(mVppHqvControl));
    mVppHqvControl.mode = HQV_MODE_AUTO;

    char property_value[PROPERTY_VALUE_MAX] = {0};
    property_get(OMX_VPP_PROP_DEBUG_LVL, property_value, "1");
    debug_level = atoi(property_value);
    debug_ext = property_get_int32(OMX_VPP_PROP_DEBUG_EXT, 0);

    memset(&mDecRcvdParamData, 0, sizeof(mDecRcvdParamData));

    VendorExtensionStore *extStore = const_cast<VendorExtensionStore *>(&mOmxVppVendorExtensionStore);
    initOmxVppExtensions(*extStore);
    mOmxVppVendorExtensionStore.dumpExtensions("OmxVppExt");

    DEBUG_PRINT_INFO("Constructed VppOmxComponent");
}

VppOmxComponent::~VppOmxComponent()
{
    DEBUG_PRINT_INFO("VppOmxComponent::~VppOmxComponent");
    sem_destroy(&mFlushEventLock);
    sem_destroy(&mReconfigEventLock);
    sem_destroy(&mEosEventLock);
}

OMX_ERRORTYPE VppOmxComponent::loadOmxDecoder()
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    const char *libName = NULL;
    for (uint32_t i = 0; i < SIZE_OF_CORE; i++) {
        if (!strcmp(core[i].name, mOmxRole)) {
            libName = core[i].so_lib_name;
        }
#ifdef DUMP_OMX_CORE
        DEBUG_PRINT_LOW("core index:%u, name:%s, lib:%s", i, core[i].name, core[i].so_lib_name);
#else
        if(libName)
            break;
#endif
    }
    if (!libName) {
        DEBUG_PRINT_ERROR("unable to find decoder for: %s", mOmxRole);
        return OMX_ErrorUndefined;
    }
    if (!strcmp(libName, QC_OMX_HW_DECODER_LIBRARY)) {
        mIsQCSoftwareDecoder = false;
    } else if (!strcmp(libName, QC_OMX_SW_DECODER_LIBRARY)) {
        mIsQCSoftwareDecoder = true;
    } else {
        DEBUG_PRINT_ERROR("not support non-qc decoder: %s", libName);
        return OMX_ErrorUndefined;
    }

    DEBUG_PRINT_LOW("loadOmxDecoder %s", libName);
    mOmxDecoderLib = dlopen(libName, RTLD_NOW);

    if (mOmxDecoderLib == NULL) {
        // check for errors
        const char* pErr = dlerror();
        if (!pErr) {
            // No error reported, but no handle to the library
            DEBUG_PRINT_ERROR("loadOmxDecoder: Error opening "
                 "library (%s) but no error reported\n", libName);
        } else {
            // Error reported
            DEBUG_PRINT_ERROR("loadOmxDecoder: Error opening library (%s): %s\n",
                                libName, pErr);
        }
        eRet = OMX_ErrorUndefined;
        return eRet;
    }
    mCreateOmxFn = (create_qc_omx_component)dlsym(mOmxDecoderLib,
                                             "get_omx_component_factory_fn");
    if (mCreateOmxFn == NULL) {
        DEBUG_PRINT_ERROR("Unable to load sym get_omx_component_factory_fn");
        dlclose(mOmxDecoderLib);
        mOmxDecoderLib = NULL;
        eRet = OMX_ErrorUndefined;
    }
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::component_init(OMX_IN OMX_STRING cmpName)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("component_init In %s", (char*)cmpName); //OMX.QCOM.H264.decoder.super
    if (!strncmp(cmpName, "OMX.qti.vdec.vpp", OMX_MAX_STRINGNAME_SIZE)) {
        mState = OMX_StateLoaded;
        DEBUG_PRINT_INFO("component_init completed (%s) without decoder", cmpName);
        return eRet;
    }
    strlcpy(mOmxRole, (char*)cmpName, OMX_MAX_STRINGNAME_SIZE);

    if ((eRet = loadOmxDecoder()) != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("component_init failed to load omx core lib \n");
        return eRet;
    }

    void* omxCmp = mCreateOmxFn();
    mOmxHandle = qc_omx_create_component_wrapper((OMX_PTR)omxCmp);

    eRet = qc_omx_component_init(mOmxHandle, mOmxRole);
    if (eRet != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("Component init failed %s %d", mOmxRole, eRet);
        dlclose(mOmxDecoderLib);
        mOmxDecoderLib = NULL;
        return eRet;
    }

    qc_omx_component_set_callbacks(mOmxHandle, &sOmxCallbacks, this);
    mState = OMX_StateLoaded;
    int len = strlen(cmpName);
    if (len > 7 && !strcmp(cmpName + len - 7, ".secure"))
        mVppFlags |= VPP_SESSION_SECURE;

    // send Default vppHqvControls.
    bool isDefault = false;
    char value[PROPERTY_VALUE_MAX];
    if (property_get(OMX_VPP_PROP_ENABLE, value, NULL)
            && (!strcmp("1", value) || !strcmp("true", value))) {
        value[0] = '\0';
        if (property_get(OMX_VPP_PROP_DEFAULT_CONFIG, value, NULL)
                && (!strcmp("1", value) || !strcmp("true", value))) {
            isDefault = true;
        }
    }
    if (isDefault) {
        DEBUG_PRINT_INFO("Calling vpp init");
        mVppContext = mVppClient->init(mVppFlags, sVppCallbacks);
        if(!mVppContext) {
            DEBUG_PRINT_ERROR("Error Initing vpp");
            return eRet;
        }

        DEBUG_PRINT_INFO("Sending default Config. %d %d",
                mVppHqvControl.mode, mVppHqvControl.ctrl_type);
        mVppHqvCtrlValue.mode = HQV_MODE_AUTO;
        int32_t vppRet = mVppClient->setCtrl(mVppHqvControl, &mVppRequirements);
        if (vppRet != VPP_OK) {
            DEBUG_PRINT_ERROR("%s: vpp_set_ctrl failed : %d", __func__, vppRet);
        }
    }
    // Query decoder extension number
    OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE decoderExtConfig;
    for (uint32_t index = 0;; index++) {
        InitOMXParams(decoderExtConfig);
        decoderExtConfig.nIndex = index;
        decoderExtConfig.nParamSizeUsed = 1;
        OMX_ERRORTYPE err = qc_omx_component_get_config(mOmxHandle, (OMX_INDEXTYPE)OMX_IndexConfigAndroidVendorExtension,
                &decoderExtConfig);
        if (err == OMX_ErrorNoMore || err != OMX_ErrorNone)
            break;
    }
    mDecoderExtensionNum = decoderExtConfig.nIndex;
    DEBUG_PRINT_LOW("decoder ext number is %u", mDecoderExtensionNum);

    //send codec Info to Vpp
    eRet = set_codec_info(cmpName);
    DEBUG_PRINT_INFO("component_init: fully completed with decoder(%s)", cmpName);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  get_component_version(OMX_HANDLETYPE       cmp_handle,
                                       OMX_STRING             cmp_name,
                                       OMX_VERSIONTYPE*    cmp_version,
                                       OMX_VERSIONTYPE*   spec_version,
                                       OMX_UUIDTYPE*          cmp_UUID)
{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(cmp_name);

    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("Get Comp Version in Invalid State\n");
        return OMX_ErrorInvalidState;
    }
    return qc_omx_component_get_version(mOmxHandle, mOmxRole,
                                 cmp_version, spec_version, cmp_UUID);
}

OMX_ERRORTYPE  VppOmxComponent::
                 send_command(OMX_HANDLETYPE cmp_handle,
                              OMX_COMMANDTYPE       cmd,
                              OMX_U32            param1,
                              OMX_PTR          cmd_data)
{
    ARG_TOUCH(cmp_handle);

    if(mState == OMX_StateInvalid) {
         DEBUG_PRINT_ERROR("send_command called in invalid state");
         return OMX_ErrorInvalidState;
    }
    DEBUG_PRINT_INFO("send_command In %d param1 %d", cmd, param1);

    if(mVppContext && !mVppBypassMode) {
        if(cmd == OMX_CommandFlush && ((param1 == OMX_ALL) || (param1 == OMX_CORE_OUTPUT_PORT_INDEX))) {
            AutoMutex autoLock(mFlushLock);
            // Handle eos -> flush case.
            AutoMutex autoEosLock(mEosSyncLock);
            if(mWaitForEOS) {
                mWaitForEOS = false;
                sem_post(&mEosEventLock);
            }
            mFlushing = true;
            mVppClient->flush(VPP_PORT_INPUT);
            mVppClient->flush(VPP_PORT_OUTPUT);
            // VPP flush is issued once flush done is received from decoder
        } else if(cmd == OMX_CommandPortDisable && ((param1 == OMX_ALL) || (param1 == OMX_CORE_OUTPUT_PORT_INDEX))) {
            mDecoderReconfig = false;
            mIsOutputDisabled = true;
            AutoMutex autoLock(mFlushLock);
            mFlushing = true;
            mVppClient->flush(VPP_PORT_INPUT);
            mVppClient->flush(VPP_PORT_OUTPUT);

            if(mReconfigPending) mReconfigPending = false;
            // VPP flush is issued once flush done is received from decoder
        } else if(cmd == OMX_CommandStateSet && (param1 == OMX_StateIdle) && mState > OMX_StateIdle) {
            mFlushing = true;
            AutoMutex autoEosLock(mEosSyncLock);
            if(mWaitForEOS) {
                DEBUG_PRINT_HIGH("trigger mEosEventLock when switching to OMX_StateIdle");
                mWaitForEOS = false;
                sem_post(&mEosEventLock);
            }
        } else if(cmd == OMX_CommandPortEnable && (param1 == OMX_CORE_OUTPUT_PORT_INDEX)) {
            //CLEAR vector..
            mBufferManager.ClearBuffers();
        }
    }
    return qc_omx_component_send_command(mOmxHandle, cmd, param1, cmd_data);
}

OMX_ERRORTYPE  VppOmxComponent::
                 get_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_parameter In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("get_parameter in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    eRet = qc_omx_component_get_parameter(mOmxHandle,
                            param_index, param_data);

    if (!mVppContext) {
        return eRet;
    }

    if (param_index == OMX_IndexParamPortDefinition && !mVppBypassMode) {
        OMX_PARAM_PORTDEFINITIONTYPE* portDef =
                           (OMX_PARAM_PORTDEFINITIONTYPE*)param_data;
        OMX_VIDEO_PORTDEFINITIONTYPE *videoDef = &(portDef->format.video);
        if (portDef->nPortIndex == 1) {
            DEBUG_PRINT_INFO("FRAME WIDTH %d FRAME HEIGHT %d",videoDef->nFrameHeight,videoDef->nFrameWidth);
            mBufferManager.UpdateBufferRequirements(mVppRequirements, portDef, false);
            DEBUG_PRINT_INFO("get_parameter counts %d %d ",portDef->nBufferCountActual, portDef->nBufferCountMin);
        } else if (portDef->nPortIndex == 0 && mState == OMX_StateLoaded) {
            // Put a limit of 6 for input buffer count
            if (portDef->nBufferCountActual < 6) {
                portDef->nBufferCountActual += 2;
                eRet = qc_omx_component_set_parameter(mOmxHandle, param_index, param_data);
                if (eRet != OMX_ErrorNone) {
                   DEBUG_PRINT_ERROR("SetParam on input port failed. ");
                   return eRet;
                }
                DEBUG_PRINT_INFO("Input BufferCount is %d", portDef->nBufferCountActual);
            }
        }
    } else if (param_index == OMX_IndexParamVideoPortFormat)  {
        OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
               (OMX_VIDEO_PARAM_PORTFORMATTYPE*)param_data;
        if (portFmt->nPortIndex == OMX_CORE_INPUT_PORT_INDEX)
            return eRet;
        if (mVppRequirements.in_color_fmt_mask & (1 << getVppColor(portFmt->eColorFormat))) {
            mVppBypassMode = false;
        } else {
            if ((portFmt->eColorFormat ==
                    (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed)
                    && mVppRequirements.in_color_fmt_mask & (1 << VPP_COLOR_FORMAT_NV12_VENUS)) {
                DEBUG_PRINT_HIGH("omx switched from NV12 UBWC to NV12");
                portFmt->eColorFormat =
                        (OMX_COLOR_FORMATTYPE)QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
                mVppBypassMode = false;
            } else {
                mVppBypassMode = true;
            }
        }
        DEBUG_PRINT_LOW("Get_param omx color = 0x%x", portFmt->eColorFormat);
        if (mVppBypassMode) {
            DEBUG_PRINT_HIGH("Get_param Bypass Mode");
        } else {
            DEBUG_PRINT_HIGH("Get_param Normal Mode");
        }
    } else if (param_index == (OMX_INDEXTYPE)OMX_QTIIndexParamVideoClientExtradata
            && !mVppBypassMode && mState == OMX_StateLoaded) {
        VALIDATE_OMX_PARAM_DATA(param_data, QOMX_EXTRADATA_ENABLE);
        QOMX_EXTRADATA_ENABLE *pParam = (QOMX_EXTRADATA_ENABLE *)param_data;
        if (pParam->nPortIndex == OMX_CORE_OUTPUT_EXTRADATA_INDEX) {
            bool vppRequest = enableVppInternalExtraData();
            bool clientRequest = pParam->bEnable;
            mVppExtradataMode = NO_EXTRADATA;
            if (clientRequest) {
                // As long as client requests the extra data, no matter whether vpp
                // requests the extra data, the mode is external mode
                mVppExtradataMode = EXTRADATA_EXTERNAL;
            } else if (vppRequest) {
                // Only vpp itself requests the extra data, the mode is internal mode
                mVppExtradataMode = EXTRADATA_INTERNAL;
                pParam->bEnable = OMX_TRUE;
            }
            DEBUG_PRINT_LOW("VppExtra: extra data mode: %u {client(%s)|vpp(%s)}",
                    mVppExtradataMode,
                    clientRequest ? "yes" : "no",
                    vppRequest ? "yes" : "no");
        }
    }
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 set_parameter(OMX_HANDLETYPE     cmp_handle,
                               OMX_INDEXTYPE     param_index,
                               OMX_PTR            param_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("set_parameter In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("set_parameter in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    /* Other parameters first need to go to the decoder and then to VPP*/

    if (eRet == OMX_ErrorNone &&
        ((param_index == (OMX_INDEXTYPE) OMX_QTIIndexParamVideoPreferAdaptivePlayback) ||
               (param_index == (OMX_INDEXTYPE) OMX_QcomIndexParamVideoAdaptivePlaybackMode))) {
        mBufferManager.SetBufferMode(((QOMX_ENABLETYPE *)param_data)->bEnable);
        if (mBufferManager.GetBufferMode()) {
            DEBUG_PRINT_HIGH("Prefer Adaptive Playback is set also in VPP");
        }
    }

    if (param_index == OMX_IndexParamPortDefinition) {
        DEBUG_PRINT_LOW("decoder has been setup");
        mIsDecoderSetup = true;
    }

    if (!mVppContext)
    {
        DEBUG_PRINT_ERROR("set parameter return, mVppContext %p",(void*)mVppContext);
        return qc_omx_component_set_parameter(mOmxHandle, param_index, param_data);
    }

    if (param_index == OMX_IndexParamVideoPortFormat)  {
        OMX_VIDEO_PARAM_PORTFORMATTYPE *portFmt =
               (OMX_VIDEO_PARAM_PORTFORMATTYPE*)param_data;
        if (OMX_CORE_OUTPUT_PORT_INDEX == portFmt->nPortIndex) {
            DEBUG_PRINT_LOW("Set_param omx color = 0x%x", portFmt->eColorFormat);
            mVppColorFormat = portFmt->eColorFormat;
            if (mVppRequirements.in_color_fmt_mask & (1 << getVppColor(mVppColorFormat))) {
                mVppBypassMode = false;
                DEBUG_PRINT_HIGH("Set_param Normal Mode");
            } else {
                mVppBypassMode = true;
                DEBUG_PRINT_HIGH("Set_param Bypass Mode, YUV format not supported by vpp");
            }
        }
    } else if (param_index == OMX_IndexParamPortDefinition) {

        OMX_PARAM_PORTDEFINITIONTYPE* portDef =
                       (OMX_PARAM_PORTDEFINITIONTYPE*)param_data;

        if ((mState == OMX_StateLoaded || mIsOutputDisabled) &&
            portDef->nPortIndex == OMX_CORE_OUTPUT_PORT_INDEX)
        {
            mBufferManager.UpdateBufferRequirements(mVppRequirements, portDef, true);
        }

        if (mState == OMX_StateLoaded) {
            if (portDef->nPortIndex == 1 && updateParameterFromOutPort(false, portDef)) {
                if (mVppRequirements.in_color_fmt_mask & (1 << mDecRcvdParamData.fmt)) {
                    int32_t vpp_err_out = mVppClient->setParameter(VPP_PORT_OUTPUT,
                                                                  mDecRcvdParamData);
                    int32_t vpp_err_in = mVppClient->setParameter(VPP_PORT_INPUT,
                                                                 mDecRcvdParamData);
                    DEBUG_PRINT_INFO("vpp setParameter err: out: %d, in: %d", vpp_err_out, vpp_err_in);
                    mIsVppActive = true;
                    if (vpp_err_out == VPP_ERR_STATE || vpp_err_in == VPP_ERR_STATE) {
                        mReconfigPending = true;
                        uint32_t u32Ret = mVppClient->reconfigure(mDecRcvdParamData, mDecRcvdParamData);
                        if (!u32Ret) {
                            sem_wait(&mReconfigEventLock);
                        } else {
                            DEBUG_PRINT_ERROR("Reconfigure failed %u", u32Ret);
                            mBufferManager.ResetBufferRequirements(portDef);
                            mReconfigPending = false;
                        }
                    } else if (vpp_err_out || vpp_err_in) {
                        //terminate vpp, as it will never be able to come out of bypass
                        //in this session.
                        mBufferManager.ResetBufferRequirements(portDef);
                        VppTerm();
                    } else {
                        DEBUG_PRINT_INFO("vpp setParameter completed");
                        mVppPortParam = mDecRcvdParamData;
                    }
                } else {
                    DEBUG_PRINT_INFO("bypass vpp, color(vpp:%u) is not supported 0x%016" PRIx64,
                                     mDecRcvdParamData.fmt, mVppRequirements.in_color_fmt_mask);
                    mVppBypassMode = true;
                }
            }
            if (portDef->nPortIndex == 0) {
                uint32_t rate = portDef->format.video.xFramerate >> 16;
                if (rate > 0 && mVppContext) {
                    struct video_property prop;
                    prop.property_type = VID_PROP_OPERATING_RATE;
                    prop.operating_rate.u32OperatingRate = rate;
                    DEBUG_PRINT_INFO("set parameter: set vpp prop fps = %u", rate);
                    auto eRet = mVppClient->setVidProp(prop);
                    if (eRet != VPP_OK) {
                        DEBUG_PRINT_ERROR("setVidProp(operating rate) failed return %d", eRet);
                    }
                }
            }
        }
        if (!mVppBypassMode && portDef->nPortIndex == OMX_CORE_OUTPUT_EXTRADATA_INDEX
                    && mVppExtradataMode != NO_EXTRADATA) {
            uint32_t size = portDef->nBufferSize;
            uint32_t count = portDef->nBufferCountActual;
            DEBUG_PRINT_LOW("VppExtra: enable omx vpp extradata: size(%u),count(%u)",
                    size, count);
            uint32_t outputCount = mBufferManager.GetActBufCount();
            if (count < outputCount) {
                DEBUG_PRINT_LOW("VppExtra: increase count from %u to %u",
                        count, outputCount);
                count = outputCount;
                portDef->nBufferCountActual = count;
            }
            eRet = qc_omx_component_set_parameter(mOmxHandle, param_index, param_data);
            if (eRet == OMX_ErrorNone) {
                mBufferManager.EnableOmxVppExtraData(size, count);
            }
            if (mVppExtradataMode == EXTRADATA_INTERNAL) {
                // In internal mode, return error to client to prevent client from
                // allocating extra buffer in extradata output port
                return OMX_ErrorBadParameter;
            }
            return eRet;
        }
    } else if (param_index ==
                (OMX_INDEXTYPE) OMX_QcomIndexParamVideoSyncFrameDecodingMode) {
        // Vpp doesn't involve in thumbnail generation
        DEBUG_PRINT_LOW("Thumbnail generation not supported ");
        VppTerm();
    }

    eRet = qc_omx_component_set_parameter(mOmxHandle, param_index, param_data);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 get_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_config In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("get_config in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    if (config_index == (OMX_INDEXTYPE)OMX_IndexConfigAndroidVendorExtension) {
        VALIDATE_OMX_PARAM_DATA(config_data, OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE);
        OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *ext =
                reinterpret_cast<OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *>(config_data);
        VALIDATE_OMX_VENDOR_EXTENSION_PARAM_DATA(ext);
        if (ext->nIndex >= mDecoderExtensionNum) {
            if (debug_ext)
                DEBUG_PRINT_LOW("OmxVppExt: get Vpp VendorExtension");
            eRet = getOmxVppExtension(ext);
            return eRet;
        }
    }
    return qc_omx_component_get_config(mOmxHandle,
            config_index, config_data);
}

void VppOmxComponent::printHQVControl(hqv_control &hqvControl)
{
    DEBUG_PRINT_LOW("Setting Hqv Control: VPP mode:%d",hqvControl.mode);
    DEBUG_PRINT_LOW("VPP Ctrl type:%d",hqvControl.ctrl_type);

    switch (hqvControl.ctrl_type)
    {
        case HQV_CONTROL_CADE:
            DEBUG_PRINT_LOW("VPP CADE mode:%d",hqvControl.cade.mode);
            DEBUG_PRINT_LOW("VPP CADE cade_level:%d",hqvControl.cade.cade_level);
            DEBUG_PRINT_LOW("VPP CADE contrast:%d",hqvControl.cade.contrast);
            DEBUG_PRINT_LOW("VPP CADE saturation:%d",hqvControl.cade.saturation);
            break;
        case HQV_CONTROL_DI:
            DEBUG_PRINT_LOW("VPP DI mode:%d",hqvControl.di.mode);
            break;
        case HQV_CONTROL_CNR:
            DEBUG_PRINT_LOW("VPP CNR mode:%d",hqvControl.cnr.mode);
            DEBUG_PRINT_LOW("VPP CNR level:%u",hqvControl.cnr.level);
            break;
        case HQV_CONTROL_AIE:
            DEBUG_PRINT_LOW("VPP AIE mode:%d",hqvControl.aie.mode);
            DEBUG_PRINT_LOW("VPP AIE hue_mode:%d",hqvControl.aie.hue_mode);
            DEBUG_PRINT_LOW("VPP AIE cade_level:%u",hqvControl.aie.cade_level);
            DEBUG_PRINT_LOW("VPP AIE ltm_level:%u",hqvControl.aie.ltm_level);
            DEBUG_PRINT_LOW("VPP AIE ltm_sat_gain:%u", hqvControl.aie.ltm_sat_gain);
            DEBUG_PRINT_LOW("VPP AIE ltm_sat_offset:%u", hqvControl.aie.ltm_sat_offset);
            DEBUG_PRINT_LOW("VPP AIE ltm_ace_str:%u", hqvControl.aie.ltm_ace_str);
            DEBUG_PRINT_LOW("VPP AIE ltm_ace_bright_l:%u", hqvControl.aie.ltm_ace_bright_l);
            DEBUG_PRINT_LOW("VPP AIE ltm_ace_bright_h:%u", hqvControl.aie.ltm_ace_bright_h);
            break;
        case HQV_CONTROL_FRC:
            DEBUG_PRINT_LOW("VPP FRC mode:%d",hqvControl.frc.mode);
            DEBUG_PRINT_LOW("VPP FRC level:%d",hqvControl.frc.level);
            DEBUG_PRINT_LOW("VPP FRC interp:%d",hqvControl.frc.interp);
            break;
        case HQV_CONTROL_EAR:
            DEBUG_PRINT_LOW("VPP EAR mode:%d", hqvControl.ear.mode);
            break;
        case HQV_CONTROL_QBR:
            DEBUG_PRINT_LOW("VPP QBR mode:%d", hqvControl.qbr.mode);
            break;
        case HQV_CONTROL_MEAS:
            DEBUG_PRINT_LOW("VPP MEAS enable:%d", hqvControl.meas.enable);
            break;
        case HQV_CONTROL_GLOBAL_DEMO:
            DEBUG_PRINT_LOW("VPP DEMO percent:%u",hqvControl.demo.process_percent);
            DEBUG_PRINT_LOW("VPP DEMO direction:%d",hqvControl.demo.process_direction);
            break;
        default:
            DEBUG_PRINT_LOW("VPP algorithm not supported");
            break;
    }
}

OMX_ERRORTYPE  VppOmxComponent::
                 set_config(OMX_HANDLETYPE      cmp_handle,
                            OMX_INDEXTYPE     config_index,
                            OMX_PTR            config_data)
{
    ARG_TOUCH(cmp_handle);

    DEBUG_PRINT_LOW("set_config In");
    if (mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("set_config in Invalid State\n");
        return OMX_ErrorInvalidState;
    }

    if (config_index == (OMX_INDEXTYPE)OMX_IndexConfigAndroidVendorExtension) {
        VALIDATE_OMX_PARAM_DATA(config_data, OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE);
        OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *ext =
                reinterpret_cast<OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *>(config_data);
        VALIDATE_OMX_VENDOR_EXTENSION_PARAM_DATA(ext);
        if (!strncmp((char *)ext->cName, "qti-ext-vpp", 11)) {
            if (mState == OMX_StateLoaded) {
                // In Loaded State, both of two cases should be ignored
                // 1. if vpp is not active and decoder is already setup:
                //      ExtendedACodec is not enabled and this call is triggered from ACodec
                // 2. if vpp is already active (vpp_set_paramter is called):
                //      ExtendedACodec already trigger it first, then ACodec would trigger it again
                if (!mIsVppActive && mIsDecoderSetup) {
                    DEBUG_PRINT_INFO("OmxVppExt: vpp ext is not supported(avenhancement is disabled)");
                    return OMX_ErrorNone;
                }
                if (mIsVppActive) {
                    DEBUG_PRINT_INFO("OmxVppExt: vpp is already active");
                    return OMX_ErrorNone;
                }
            } else {
                // In other State (executing State):
                // bypass setOmxVppExtension if vpp is not running
                if (mVppContext == NULL || mVppBypassMode) {
                    DEBUG_PRINT_INFO("OmxVppExt: vpp is not running");
                    return OMX_ErrorNone;
                }
            }
            DEBUG_PRINT_INFO("OmxVppExt: set Vpp VendorExtension");
            return setOmxVppExtension(ext);
        } else
            return qc_omx_component_set_config(mOmxHandle, config_index, config_data);
    }

    OMX_ERRORTYPE eRet = qc_omx_component_set_config(mOmxHandle,
            config_index, config_data);

    if (eRet == OMX_ErrorNone && config_index == (OMX_INDEXTYPE) OMX_IndexConfigOperatingRate) {
        VALIDATE_OMX_PARAM_DATA(config_data, OMX_PARAM_U32TYPE);
        OMX_PARAM_U32TYPE *rate = (OMX_PARAM_U32TYPE *)config_data;
        struct video_property prop;

        prop.property_type = VID_PROP_OPERATING_RATE;
        prop.operating_rate.u32OperatingRate = rate->nU32 >> 16;
        DEBUG_PRINT_INFO("set config: set vpp prop fps = %u", prop.operating_rate.u32OperatingRate);
        if (mVppContext) {
            auto eRet = mVppClient->setVidProp(prop);
            if (eRet != VPP_OK) {
               DEBUG_PRINT_ERROR("vpp setVidProp failed return %d, operating rate not set", eRet);
            }
        }
    }
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 get_extension_index(OMX_HANDLETYPE  cmp_handle,
                                     OMX_STRING       paramName,
                                     OMX_INDEXTYPE*   indexType)

{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;

    DEBUG_PRINT_LOW("get_extension_index In");
    if(mState == OMX_StateInvalid) {
        DEBUG_PRINT_ERROR("get_extension_index in Invalid State\n");
        return OMX_ErrorInvalidState;
    }
    eRet = qc_omx_component_get_extension_index(mOmxHandle,
                            paramName, indexType);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  get_state(OMX_HANDLETYPE  cmp_handle,
                           OMX_STATETYPE*       state)

{
    ARG_TOUCH(cmp_handle);
    if (mOmxHandle == NULL && state != NULL) {
        *state = mState;
        return OMX_ErrorNone;
    }
    return qc_omx_component_get_state(mOmxHandle, state);
}

OMX_ERRORTYPE  VppOmxComponent::
                  component_tunnel_request(OMX_HANDLETYPE           cmp_handle,
                                          OMX_U32                        port,
                                          OMX_HANDLETYPE       peer_component,
                                          OMX_U32                   peer_port,
                                          OMX_TUNNELSETUPTYPE*   tunnel_setup)
{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(port);
    ARG_TOUCH(peer_port);
    ARG_TOUCH(peer_component);
    ARG_TOUCH(tunnel_setup);
    OMX_ERRORTYPE eRet = OMX_ErrorNotImplemented;
    ALOGW("component_tunnel_request not implemented");
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 use_buffer(OMX_HANDLETYPE                cmp_handle,
                            OMX_BUFFERHEADERTYPE**        buffer_hdr,
                            OMX_U32                             port,
                            OMX_PTR                         app_data,
                            OMX_U32                            bytes,
                            OMX_U8*                           buffer)
{
    ARG_TOUCH(cmp_handle);
    if (mCopyInputToOmx && port == OMX_CORE_INPUT_PORT_INDEX) {
        return useBuffer(buffer_hdr, port, app_data, bytes, buffer);
    }

    if (mVppContext && port == OMX_CORE_OUTPUT_EXTRADATA_INDEX) {
        mBufferManager.UseExtraBufferByClient(bytes, &buffer);
    }

    auto eRet = qc_omx_component_use_buffer(mOmxHandle, buffer_hdr, port,
                                            app_data, bytes, buffer);

    DEBUG_PRINT_LOW("use_buffer, omxHeader=%p, port=%u, eRet=0x%x",
                    (void *)*buffer_hdr, port, eRet);

    if (eRet != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("use_buffer on decoder failed, eRet=0x%x", eRet);
        return eRet;
    }

    if (mVppContext && port == OMX_CORE_OUTPUT_PORT_INDEX) {
        mBufferManager.RegisterClientBuffer(*buffer_hdr, buffer);
    }

    return eRet;
}


OMX_ERRORTYPE  VppOmxComponent::
                 allocate_buffer(OMX_HANDLETYPE                cmp_handle,
                                 OMX_BUFFERHEADERTYPE**        buffer_hdr,
                                 OMX_U32                             port,
                                 OMX_PTR                         app_data,
                                 OMX_U32                            bytes)

{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(port);
    ARG_TOUCH(buffer_hdr);
    ARG_TOUCH(app_data);
    ARG_TOUCH(bytes);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    if (mVppContext && !mVppBypassMode && port == OMX_CORE_OUTPUT_PORT_INDEX) {
        DEBUG_PRINT_LOW("allocate_buffer for output port is not supported, bypass omx vpp");
        mVppBypassMode = true;
    }
    eRet = qc_omx_component_allocate_buffer(mOmxHandle, buffer_hdr, port, app_data, bytes);
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::
                 free_buffer(OMX_HANDLETYPE         cmp_handle,
                             OMX_U32                      port,
                             OMX_BUFFERHEADERTYPE*      buffer)
{
    ARG_TOUCH(cmp_handle);

    DEBUG_PRINT_LOW("free_buffer called omxHeader=%p, port=%u",
                    (void *)buffer, port);

    if (mVppContext && port == OMX_CORE_OUTPUT_PORT_INDEX) {
        mBufferManager.UnregisterClientBuffer(buffer);
        if (mState <= OMX_StateIdle)
            mBufferManager.FreeOmxVppExtraData();
    }

    if (mCopyInputToOmx && port == OMX_CORE_INPUT_PORT_INDEX && buffer) {
        for (size_t i = 0; i < mBufferMetas.size(); i++) {
            if (mBufferMetas[i].omxBuf == buffer->pBuffer) {
                mBufferMetas.removeItemsAt(i);
                break;
            }
        }
    }

    auto eRet = qc_omx_component_free_buffer(mOmxHandle, port, buffer);
    if (eRet != OMX_ErrorNone)
        DEBUG_PRINT_ERROR("free_buffer on decoder failed, eRet=0x%x", eRet);

    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  empty_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                   OMX_BUFFERHEADERTYPE*      buffer)

{
    ARG_TOUCH(cmp_handle);
    ARG_TOUCH(buffer);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("Etb to the decoder ");
    if (mCopyInputToOmx)
        copyInputToOmx(buffer);
    eRet = qc_omx_component_empty_this_buffer(mOmxHandle, buffer);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                  fill_this_buffer(OMX_HANDLETYPE         cmp_handle,
                                  OMX_BUFFERHEADERTYPE*      buffer)

{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("Ftb from client for buffer: %p", buffer);

    if (mVppContext != NULL && !mVppBypassMode) {
        mBufferManager.PrintBufferStatistics();

        VppBufferManager::OmxVppBuffer* buf;
        buf = mBufferManager.GetFtbDestination(buffer);
        eRet = SubmitBuffer(buf, buffer);

        return eRet; //Need to take vpp_err also into consideration depending on severity
    }

    eRet = qc_omx_component_fill_this_buffer(mOmxHandle, buffer);
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 set_callbacks( OMX_HANDLETYPE        cmp_handle,
                                OMX_CALLBACKTYPE*      callbacks,
                                OMX_PTR                 app_data)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("set_callbacks In");

    mOmxCallbacks       = *callbacks;
    mAppData = app_data;
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::component_deinit(OMX_HANDLETYPE cmp_handle)
{
    ARG_TOUCH(cmp_handle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    if (mVppContext)
        mBufferManager.FreeOmxVppExtraData();
    mBufferMetas.clear();
    DEBUG_PRINT_LOW("component_deinit In");
    if (mOmxHandle != NULL) {
        qc_omx_component_deinit(mOmxHandle);
        mOmxHandle = NULL;
    }
    if (mOmxDecoderLib != NULL) {
        int err = dlclose(mOmxDecoderLib);
        mOmxDecoderLib = NULL;
        if (err) {
            DEBUG_PRINT_ERROR("Error in dlclose of vdec lib\n");
        }
    }
    VppTerm();
    return eRet;
}

OMX_ERRORTYPE  VppOmxComponent::
                 use_EGL_image(OMX_HANDLETYPE                cmp_handle,
                               OMX_BUFFERHEADERTYPE**        buffer_hdr,
                               OMX_U32                             port,
                               OMX_PTR                         app_data,
                               void*                          egl_image)
{
    ARG_TOUCH(cmp_handle);
    return qc_omx_component_use_EGL_image(mOmxHandle, buffer_hdr, port,
                                          app_data, egl_image);

}

OMX_ERRORTYPE  VppOmxComponent::
                component_role_enum( OMX_HANDLETYPE cmp_handle,
                                      OMX_U8*              role,
                                      OMX_U32             index)
{
    ARG_TOUCH(cmp_handle);
    return qc_omx_component_role_enum(mOmxHandle, role, index);
}

// callbacks from component
OMX_ERRORTYPE VppOmxComponent::OmxEventHandler(
                                      OMX_HANDLETYPE pHandle,
                                      OMX_PTR pAppData,
                                      OMX_EVENTTYPE eEvent,
                                      OMX_U32 nData1,
                                      OMX_U32 nData2,
                                      OMX_PTR pEventData)
{
    ARG_TOUCH(pHandle);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    auto super_omx = reinterpret_cast<VppOmxComponent*>(pAppData);
    auto omx_comp = (OMX_COMPONENTTYPE*)(super_omx->mOmxHandle);
    struct timespec ts;

    DEBUG_PRINT_LOW("OmxEventHandler In, event %d", eEvent);
    switch ((OMX_U32)eEvent)
    {
        case OMX_EventCmdComplete:
        {
            DEBUG_PRINT_LOW("Command complete %d %d", nData1, nData2);
            if (nData1 == OMX_CommandFlush && (nData2 == OMX_ALL || nData2 == 1)) {
                if(super_omx->mVppContext && !super_omx->mVppBypassMode) {
                    AutoMutex autoLock(super_omx->mFlushLock);
                    DEBUG_PRINT_HIGH("Waiting for flush_done from VPP");
                    sem_wait(&super_omx->mFlushEventLock);
                    super_omx->mFlushing = false;
                    DEBUG_PRINT_HIGH("FLUSH RECEIVED");
                }
            }

            if(nData1 == OMX_CommandPortDisable && nData2 == 1) {
                if(super_omx->mVppContext && !super_omx->mVppBypassMode) {
                    AutoMutex autoLock(super_omx->mFlushLock);
                    DEBUG_PRINT_INFO("Port Disable send flush to VPP library ");
                    sem_wait(&super_omx->mFlushEventLock);
                    super_omx->mFlushing = false;
                }
            }

            if(nData1 == OMX_CommandPortEnable && nData2 == 1) {
                if(super_omx->mVppContext && !super_omx->mVppBypassMode) {
                    DEBUG_PRINT_INFO("Got PortEnable done for vdec out");
                    super_omx->mIsOutputDisabled = false;
                }
            }

            if(nData1 ==  OMX_CommandStateSet) {
                auto prevState = super_omx->mState;
                super_omx->mState = (OMX_STATETYPE)nData2;
                if(super_omx->mVppContext && !super_omx->mVppBypassMode && nData2 == OMX_StateIdle) {
                        if(prevState > super_omx->mState) {
                        DEBUG_PRINT_INFO("Flush during executing to idle!");
                        super_omx->mVppClient->flush(VPP_PORT_INPUT);
                        super_omx->mVppClient->flush(VPP_PORT_OUTPUT);
                        sem_wait(&super_omx->mFlushEventLock);
                        super_omx->mFlushing = false;
                        }
                }
                if (super_omx->mState == OMX_StateLoaded && prevState == OMX_StateIdle) {
                    // when decoder moved from Idle to Loaded state, all of output buffers
                    // have been freed. So BufferManager should clear its buffer status.
                    super_omx->mBufferManager.ClearBuffers();
                }
            }

            //TODO: Add a wait to serialize VPP flush complete and disable cmd complete
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventCmdComplete,
                                  nData1, nData2, pEventData);
            break;
        }
        case OMX_EventPortFormatDetected:
        {
            DEBUG_PRINT_LOW("OMX_EventPortFormatDetected %d", nData1);
            break;
        }
        case OMX_EventPortSettingsChanged:
        {
            if (nData2 == OMX_IndexParamPortDefinition) {
                DEBUG_PRINT_LOW("OMX_EventPortSettingsChanged %d", nData1);
                super_omx->mDecoderReconfig = true;
            } else if (nData2 == OMX_IndexConfigCommonOutputCrop) {
                DEBUG_PRINT_LOW("Reconfig - IndexConfigCommonOutputCrop  ");
            } else {
                DEBUG_PRINT_LOW("Unhandled Port Reconfig Event. nData2 = 0x%x", nData2);
                eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                        super_omx->mAppData, OMX_EventPortSettingsChanged,
                        nData1, nData2, NULL);
                break;
            }

            if(super_omx->mVppContext) {
                bool needReconfigure = false;
                if (nData2 == OMX_IndexParamPortDefinition) {
                    needReconfigure = super_omx->updateParameterFromOutPort(true, NULL);
                }
                if (!needReconfigure) {
                    // Check whether vpp input port still hold buffers
                    BufferCounts cnts;
                    {
                        AutoMutex autoLock(super_omx->mFbdSyncLock);
                        super_omx->mBufferManager.GetBufferCounts(cnts);
                    }
                    if (cnts.vppIn) {
                        DEBUG_PRINT_LOW("vpp still hold input buffer during PortSettingsChanged");
                        needReconfigure = true;
                    }
                }

                if (needReconfigure) {
                    // Deferring the event for OmxVppReconfig complete.
                    DEBUG_PRINT_INFO("sending reconfigure to VPP library ");
                    super_omx->mReconfigPending = true;
                    uint32_t u32Ret = super_omx->mVppClient->reconfigure(super_omx->mDecRcvdParamData,
                                                                        super_omx->mDecRcvdParamData);
                    if (!u32Ret) {
                        sem_wait(&super_omx->mReconfigEventLock);
                    } else {
                        DEBUG_PRINT_ERROR("Vpp Reconfigure failed!! %u", u32Ret);
                        super_omx->mReconfigPending = false;
                    }
                 }

            }
            DEBUG_PRINT_LOW("sending reconfigure to client");
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventPortSettingsChanged,
                                  nData1, nData2, NULL);
            break;
        }
        case OMX_EventBufferFlag:
        {
            DEBUG_PRINT_INFO("OMX_EventBufferFlag %d", nData1);
            if(nData1 == OMX_CORE_OUTPUT_PORT_INDEX && nData2 == OMX_BUFFERFLAG_EOS) {
                if(super_omx->mVppContext && !super_omx->mVppBypassMode) {
                    super_omx->mEosSyncLock.lock();
                    if(!super_omx->mEosReached && !super_omx->mFlushing) {
                        super_omx->mWaitForEOS = true;
                        super_omx->mEosSyncLock.unlock();
                        DEBUG_PRINT_LOW("Waiting for EOS");
                        sem_wait(&super_omx->mEosEventLock);
                        DEBUG_PRINT_LOW("Got eos / Flush happened");
                    } else {
                        super_omx->mEosSyncLock.unlock();
                    }
                    super_omx->mEosReached = false;
                    super_omx->mWaitForEOS = false;
                }
            }
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventBufferFlag,
                                  nData1, nData2, pEventData);
            break;
        }
        case OMX_EventError:
        {
            DEBUG_PRINT_LOW("OMX_EventError %d", nData1);
            eRet = super_omx->mOmxCallbacks.EventHandler(&super_omx->m_cmp,
                                  super_omx->mAppData,
                                  OMX_EventError,
                                  nData1, nData2, pEventData);
            break;
        }
        default:
            DEBUG_PRINT_ERROR("unknown event %d", eEvent);
    }
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::OmxEmptyBufferDone(
                                         OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_BUFFERHEADERTYPE* pBufHdr)
{
    ARG_TOUCH(hComponent);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    DEBUG_PRINT_LOW("Ebd from decoder received %p ",(void *)pBufHdr);
    VppOmxComponent* super_omx = reinterpret_cast<VppOmxComponent*>(pAppData);
    eRet = super_omx->mOmxCallbacks.EmptyBufferDone(&super_omx->m_cmp, super_omx->mAppData, pBufHdr);
    return eRet;
}

OMX_ERRORTYPE VppOmxComponent::OmxFillBufferDone(OMX_HANDLETYPE hComponent,
                                                 OMX_PTR pAppData,
                                                 OMX_BUFFERHEADERTYPE* buffer)
{
    ARG_TOUCH(hComponent);
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    auto super_omx = reinterpret_cast<VppOmxComponent*>(pAppData);

    bool isRo = buffer->nFlags & OMX_BUFFERFLAG_READONLY;
    DEBUG_PRINT_LOW("VDEC_FBD omxHdr=%p, fl=%u, flags=%u, ro=%u, ts=%llu",
                    (void *)buffer, buffer->nFilledLen,buffer->nFlags, isRo,
                    buffer->nTimeStamp);

    super_omx->mBufferManager.PrintBufferStatistics();

    if (super_omx->mVppContext && !super_omx->mVppBypassMode) {
        AutoMutex autoLock(super_omx->mFlushLock);

        VppBufferManager::OmxVppBuffer *buf =
            super_omx->mBufferManager.GetVdecFbdDestination(buffer);
        eRet = super_omx->SubmitBuffer(buf, buffer);
        return eRet;
    }

    // No VPP
    eRet = super_omx->mOmxCallbacks.FillBufferDone(&super_omx->m_cmp, super_omx->mAppData, buffer);
    return eRet;
}

#define DUMP_VPP_BUF(_lvl, _b, _pre) \
    DEBUG_PRINT_##_lvl("%s: omxHdr=%p, fl=%u, flags=0x%x, ts=%" PRIu64, \
                       (_pre), (_b)->cookie, (_b)->pixel.filled_len, (_b)->flags, (_b)->timestamp)

// vpp empty_buffer_done callback
void VppOmxComponent::VppEmptyBufferDone(void *handle, struct vpp_buffer *buf)
{
    if (!handle || !buf)
        return;

    VppOmxComponent *sOmxVpp = (VppOmxComponent *)handle;
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    auto buffer = (OMX_BUFFERHEADERTYPE *)buf->cookie;

    DUMP_VPP_BUF(LOW, buf, "VPP_IBD");

    sOmxVpp->mBufferManager.PrintBufferStatistics();

    VppBufferManager::OmxVppBuffer* omxBuf;
    omxBuf = sOmxVpp->mBufferManager.GetFtbDestination(buffer);

    eRet = sOmxVpp->SubmitBuffer(omxBuf, buffer);
}


// vpp output_buffer_done callback
void VppOmxComponent::VppOutputBufferDone(void *handle, struct vpp_buffer *buf)
{
    if (!handle || !buf)
        return;

    VppOmxComponent *sOmxVpp = (VppOmxComponent *)handle;

    AutoMutex autoLock(sOmxVpp->mFbdSyncLock);
    auto buffer = (OMX_BUFFERHEADERTYPE *)buf->cookie;

    DUMP_VPP_BUF(LOW, buf, "VPP_OBD");

    sOmxVpp->mBufferManager.PrintBufferStatistics();

    auto omxVppBuf = sOmxVpp->mBufferManager.GetVppObdDestination(buffer);

    if(buffer->nFlags & OMX_BUFFERFLAG_EOS) {
        AutoMutex autoLock(sOmxVpp->mEosSyncLock);
        DEBUG_PRINT_INFO("Eos Reached !!");
        sOmxVpp->mEosReached = true;
        if(sOmxVpp->mWaitForEOS) {
            sem_post(&sOmxVpp->mEosEventLock);
            sOmxVpp->mWaitForEOS = false;
        }
    }

    sOmxVpp->SubmitBuffer(omxVppBuf, buffer);
}

// vpp event callback
void VppOmxComponent::VppEvent(void *handle, struct vpp_event e)
{
    VppOmxComponent *sOmxVpp = (VppOmxComponent *)handle;
    if (e.type == VPP_EVENT_FLUSH_DONE) {
        if(e.flush_done.port == VPP_PORT_INPUT)
            sOmxVpp->mVppFlushStatus |= (uint8_t)VPP_INPUT;
        else
            sOmxVpp->mVppFlushStatus |= (uint8_t)VPP_OUTPUT;
        DEBUG_PRINT_HIGH("Vpp flush event status %x", sOmxVpp->mVppFlushStatus);

        if (sOmxVpp->mVppFlushStatus == VPP_ALL) {
            sem_post(&sOmxVpp->mFlushEventLock);
            sOmxVpp->mVppFlushStatus = 0;
        }
    } else if (e.type == VPP_EVENT_RECONFIG_DONE) {
        DEBUG_PRINT_HIGH("VPP Reconfig event received"
                "(pending flag:%s)", sOmxVpp->mReconfigPending ? "true" : "false");
        sOmxVpp->mVppRequirements = e.port_reconfig_done.req;
        sOmxVpp->mVppPortParam = sOmxVpp->mDecRcvdParamData;

        if(sOmxVpp->mReconfigPending) {
            sem_post(&sOmxVpp->mReconfigEventLock);
            sOmxVpp->mReconfigPending = false;
        }
    } else if (e.type == VPP_EVENT_ERROR) {
        // Only binderDied (VppClient) is using VPP_EVENT_ERROR for now
        DEBUG_PRINT_ERROR("VPP error event received ");
    }
}

OMX_ERRORTYPE VppOmxComponent::SubmitBuffer(VppBufferManager::OmxVppBuffer *omxVppBuf,
                                            OMX_BUFFERHEADERTYPE *buffer)
{
    uint32_t u32 = VPP_OK;
    OMX_ERRORTYPE err = OMX_ErrorNone;

    if (!omxVppBuf && !buffer)
        return OMX_ErrorBadParameter;

    if (!omxVppBuf) {
        DEBUG_PRINT_INFO("sending fbd - no OmxVppBuffer, omxHdr=%p", buffer);
        return mOmxCallbacks.FillBufferDone(&m_cmp, mAppData, buffer);
    }

    if (omxVppBuf->owner == VppBufferManager::OMX_VDEC) {

        AutoMutex autoLock(mDecoderSyncLock);

        err = qc_omx_component_fill_this_buffer(mOmxHandle, omxVppBuf->omxHeader);
        if (err != OMX_ErrorNone) {
            DEBUG_PRINT_ERROR("failed to ftb to decoder, err=0x%x", err);
            u32 = VPP_ERR;
        } else {
            return err;
        }
    }

    if (omxVppBuf->owner == VppBufferManager::VPP_IN ||
        omxVppBuf->owner == VppBufferManager::VPP_OUT) {
        const char *str = "out";
        enum vpp_port port = VPP_PORT_OUTPUT;

        if (omxVppBuf->owner == VppBufferManager::VPP_IN) {
            str = "in";
            port = VPP_PORT_INPUT;
        }

        AutoMutex autoLock(mVppSyncLock);

        u32 = mVppClient->queueBuf(port, &omxVppBuf->vppBuffer);
        if (u32 != VPP_OK) {
            DEBUG_PRINT_ERROR("failed to queueBuf[%s], u32=%u, omxHdr=%p",
                              str, u32, omxVppBuf->omxHeader);
        } else {
            struct vpp_buffer &b = omxVppBuf->vppBuffer;
            DEBUG_PRINT_LOW("queueBuf[%s] omxHdr=%p, fd=%d, alen=%d, flen=%d, off=%d, flags=0x%x, ts=%" PRIu64"",
                            str, omxVppBuf->omxHeader, b.pixel.fd, b.pixel.alloc_len,
                            b.pixel.filled_len, b.pixel.offset, b.flags, b.timestamp);
            return OMX_ErrorNone;
        }
    }

    // TODO - handle me
    if (u32 != VPP_OK || err != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("FIXME: dropping buffer, omxHdr=%p, u32=%u, err=0x%x",
                          omxVppBuf->omxHeader, u32, err);
        return OMX_ErrorNotImplemented;
    }

    if (omxVppBuf->owner == VppBufferManager::OMX_CLIENT) {
        DEBUG_PRINT_LOW("sending fbd, omxHeader=%p", omxVppBuf->omxHeader);
        return mOmxCallbacks.FillBufferDone(&m_cmp, mAppData, omxVppBuf->omxHeader);
    }

    DEBUG_PRINT_ERROR("you should not be here!");
    return OMX_ErrorBadParameter;
}

//Helper function to parse OMX component name , convert to a codec enum and send it to vpp library
OMX_ERRORTYPE VppOmxComponent::set_codec_info(OMX_STRING cmpName)
{
    OMX_ERRORTYPE eRet = OMX_ErrorNone;
    struct video_property prop;
    uint32_t vppRet = VPP_OK;

    if (!strncmp(cmpName,"OMX.qcom.video.decoder.avc",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName,"OMX.qcom.video.decoder.avc.dsmode",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.avc.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.avc.secure.dsmode",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_AVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.divx4",\
                OMX_MAX_STRINGNAME_SIZE)
            || !strncmp(cmpName, "OMX.qti.video.decoder.divx4sw",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_DIVX4;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.divx",\
                OMX_MAX_STRINGNAME_SIZE)
            || !strncmp(cmpName, "OMX.qti.video.decoder.divxsw",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_DIVX;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.divx311",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_DIVX311;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg4",\
                OMX_MAX_STRINGNAME_SIZE)
            || !strncmp(cmpName, "OMX.qti.video.decoder.mpeg4sw",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG4;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg4.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG4;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg2",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG2;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.mpeg2.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_MPEG2;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vc1",\
                OMX_MAX_STRINGNAME_SIZE)
            || !strncmp(cmpName, "OMX.qti.video.decoder.vc1sw", \
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VC1;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vc1.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VC1;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.wmv",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_WMV;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.wmv.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_WMV;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.h263",\
                OMX_MAX_STRINGNAME_SIZE)
            || !strncmp(cmpName, "OMX.qti.video.decoder.h263sw",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_H263;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.hevc",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_HEVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.hevc.secure",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_HEVC;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vp8",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VP8;
    } else if (!strncmp(cmpName, "OMX.qcom.video.decoder.vp9",\
                OMX_MAX_STRINGNAME_SIZE)) {
        prop.codec.eCodec = VPP_CODEC_TYPE_VP9;
    } else {
        DEBUG_PRINT_ERROR("ERROR:Unknown Component: %s", cmpName);
        prop.codec.eCodec = VPP_CODEC_TYPE_UNKNOWN;
    }

    if (prop.codec.eCodec != VPP_CODEC_TYPE_UNKNOWN) {
        prop.property_type = VID_PROP_CODEC;
        DEBUG_PRINT_LOW(" Codec Info is %d", prop.codec.eCodec);
        vppRet = mVppClient->setVidProp(prop);
        if (vppRet) {
            DEBUG_PRINT_ERROR(" set codec property to vpp failed %u", vppRet);
        }
    }
    return eRet;
}

bool VppOmxComponent::updateParameterFromOutPort(bool isQuery, void *portDef) {
    OMX_PARAM_PORTDEFINITIONTYPE outPortDef;
    if (isQuery) {
        InitOMXParams(outPortDef);
        outPortDef.nPortIndex = 1;
        if (qc_omx_component_get_parameter(mOmxHandle, (OMX_INDEXTYPE)OMX_IndexParamPortDefinition,
                &outPortDef) != OMX_ErrorNone) {
            return false;
        }
        portDef = &outPortDef;
    } else {
        if (portDef == NULL)
            return false;
    }

    bool formatChanged = false;
    mDecRcvdParamData = mVppPortParam;
    OMX_VIDEO_PORTDEFINITIONTYPE *videoDef =
            &(((OMX_PARAM_PORTDEFINITIONTYPE *)portDef)->format.video);
    DEBUG_PRINT_LOW("output port definition: omx color:0x%x size(w:%u h:%u stride:%u slice:%u)",
            videoDef->eColorFormat, videoDef->nFrameWidth, videoDef->nFrameHeight,
            videoDef->nStride, videoDef->nSliceHeight);
    if (mDecRcvdParamData.height != videoDef->nFrameHeight
            || mDecRcvdParamData.width != videoDef->nFrameWidth) {
        DEBUG_PRINT_LOW("video size updated");
        formatChanged = true;
    }

    if (mVppColorFormat != videoDef->eColorFormat) {
        DEBUG_PRINT_LOW("color format updated");
        formatChanged = true;
    }

    if (formatChanged) {
        DEBUG_PRINT_LOW("old parameters: vpp color: %u, size(%u)(w:%u h:%u stride:%u slice:%u)",
                mDecRcvdParamData.fmt, mPixelSize, mDecRcvdParamData.width, mDecRcvdParamData.height,
                mDecRcvdParamData.stride, mDecRcvdParamData.scanlines);
        mDecRcvdParamData.height = videoDef->nFrameHeight;
        mDecRcvdParamData.width = videoDef->nFrameWidth;
        mVppColorFormat = videoDef->eColorFormat;
        mDecRcvdParamData.fmt = getVppColor(mVppColorFormat);
        getVenusSize(mPixelSize, mDecRcvdParamData.stride,
                mDecRcvdParamData.scanlines, mVppColorFormat,
                mDecRcvdParamData.width, mDecRcvdParamData.height);
        if (isQuery) {
            DEBUG_PRINT_LOW("component value: stride:%u slice:%u",
                    videoDef->nStride, videoDef->nSliceHeight);
        }
        DEBUG_PRINT_LOW("new parameters: vpp color: %u, size(%u)(w:%u h:%u stride:%u slice:%u)",
                mDecRcvdParamData.fmt, mPixelSize, mDecRcvdParamData.width, mDecRcvdParamData.height,
                mDecRcvdParamData.stride, mDecRcvdParamData.scanlines);
    }
    return formatChanged;
}

void VppOmxComponent::print_debug_extradata(OMX_OTHER_EXTRADATATYPE *extra)
{
    if (!extra)
        return;

    DEBUG_PRINT_HIGH(
            "============== Extra Data ==============\n"
            "           Size: %u\n"
            "        Version: %u\n"
            "      PortIndex: %u\n"
            "           Type: %x\n"
            "       DataSize: %u",
            (unsigned int)extra->nSize, (unsigned int)extra->nVersion.nVersion,
            (unsigned int)extra->nPortIndex, extra->eType, (unsigned int)extra->nDataSize);

    if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataInterlaceFormat) {
        OMX_STREAMINTERLACEFORMAT *intfmt = (OMX_STREAMINTERLACEFORMAT *)(void *)extra->data;
        DEBUG_PRINT_HIGH(
                "------ Interlace Format ------\n"
                "                Size: %u\n"
                "             Version: %u\n"
                "           PortIndex: %u\n"
                " Is Interlace Format: %d\n"
                "   Interlace Formats: %u\n"
                "=========== End of Interlace ===========",
                (unsigned int)intfmt->nSize, (unsigned int)intfmt->nVersion.nVersion, (unsigned int)intfmt->nPortIndex,
                intfmt->bInterlaceFormat, (unsigned int)intfmt->nInterlaceFormats);
    } else if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataFrameInfo) {
        OMX_QCOM_EXTRADATA_FRAMEINFO *fminfo = (OMX_QCOM_EXTRADATA_FRAMEINFO *)(void *)extra->data;

        DEBUG_PRINT_HIGH(
                "-------- Frame Format --------\n"
                "             Picture Type: %d\n"
                "           Interlace Type: %d\n"
                " Pan Scan Total Frame Num: %u\n"
                "   Concealed Macro Blocks: %u\n"
                "               frame rate: %u\n"
                "               Time Stamp: %llu\n"
                "           Aspect Ratio X: %u\n"
                "           Aspect Ratio Y: %u",
                fminfo->ePicType,
                fminfo->interlaceType,
                (unsigned int)fminfo->panScan.numWindows,
                (unsigned int)fminfo->nConcealedMacroblocks,
                (unsigned int)fminfo->nFrameRate,
                fminfo->nTimeStamp,
                (unsigned int)fminfo->aspectRatio.aspectRatioX,
                (unsigned int)fminfo->aspectRatio.aspectRatioY);

        for (OMX_U32 i = 0; i < fminfo->panScan.numWindows; i++) {
            DEBUG_PRINT_HIGH(
                    "------------------------------"
                    "     Pan Scan Frame Num: %u\n"
                    "            Rectangle x: %d\n"
                    "            Rectangle y: %d\n"
                    "           Rectangle dx: %d\n"
                    "           Rectangle dy: %d",
                    (unsigned int)i, (unsigned int)fminfo->panScan.window[i].x, (unsigned int)fminfo->panScan.window[i].y,
                    (unsigned int)fminfo->panScan.window[i].dx, (unsigned int)fminfo->panScan.window[i].dy);
        }

        DEBUG_PRINT_HIGH("========= End of Frame Format ==========");
    } else if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataFramePackingArrangement) {
        OMX_QCOM_FRAME_PACK_ARRANGEMENT *framepack = (OMX_QCOM_FRAME_PACK_ARRANGEMENT *)(void *)extra->data;
        DEBUG_PRINT_HIGH(
                "------------------ Framepack Format ----------\n"
                "                           id: %u \n"
                "                  cancel_flag: %u \n"
                "                         type: %u \n"
                " quincunx_sampling_flagFormat: %u \n"
                "  content_interpretation_type: %u \n"
                "        spatial_flipping_flag: %u \n"
                "          frame0_flipped_flag: %u \n"
                "             field_views_flag: %u \n"
                " current_frame_is_frame0_flag: %u \n"
                "   frame0_self_contained_flag: %u \n"
                "   frame1_self_contained_flag: %u \n"
                "       frame0_grid_position_x: %u \n"
                "       frame0_grid_position_y: %u \n"
                "       frame1_grid_position_x: %u \n"
                "       frame1_grid_position_y: %u \n"
                "                reserved_byte: %u \n"
                "            repetition_period: %u \n"
                "               extension_flag: %u \n"
                "================== End of Framepack ===========",
                (unsigned int)framepack->id,
                (unsigned int)framepack->cancel_flag,
                (unsigned int)framepack->type,
                (unsigned int)framepack->quincunx_sampling_flag,
                (unsigned int)framepack->content_interpretation_type,
                (unsigned int)framepack->spatial_flipping_flag,
                (unsigned int)framepack->frame0_flipped_flag,
                (unsigned int)framepack->field_views_flag,
                (unsigned int)framepack->current_frame_is_frame0_flag,
                (unsigned int)framepack->frame0_self_contained_flag,
                (unsigned int)framepack->frame1_self_contained_flag,
                (unsigned int)framepack->frame0_grid_position_x,
                (unsigned int)framepack->frame0_grid_position_y,
                (unsigned int)framepack->frame1_grid_position_x,
                (unsigned int)framepack->frame1_grid_position_y,
                (unsigned int)framepack->reserved_byte,
                (unsigned int)framepack->repetition_period,
                (unsigned int)framepack->extension_flag);
    } else if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataQP) {
        OMX_QCOM_EXTRADATA_QP * qp = (OMX_QCOM_EXTRADATA_QP *)(void *)extra->data;
        DEBUG_PRINT_HIGH(
                "---- QP (Frame quantization parameter) ----\n"
                "    Frame QP: %u \n"
                "================ End of QP ================\n",
                (unsigned int)qp->nQP);
    } else if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataInputBitsInfo) {
        OMX_QCOM_EXTRADATA_BITS_INFO * bits = (OMX_QCOM_EXTRADATA_BITS_INFO *)(void *)extra->data;
        DEBUG_PRINT_HIGH(
                "--------- Input bits information --------\n"
                "    Header bits: %u \n"
                "     Frame bits: %u \n"
                "===== End of Input bits information =====\n",
                (unsigned int)bits->header_bits, (unsigned int)bits->frame_bits);
    } else if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataMP2UserData) {
        OMX_QCOM_EXTRADATA_USERDATA *userdata = (OMX_QCOM_EXTRADATA_USERDATA *)(void *)extra->data;
        OMX_U8 *data_ptr = (OMX_U8 *)userdata->data;
        OMX_U32 userdata_size = extra->nDataSize - sizeof(userdata->type);
        OMX_U32 i = 0;
        DEBUG_PRINT_HIGH(
                "--------------  Userdata  -------------\n"
                "    Stream userdata type: %u\n"
                "          userdata size: %u\n"
                "    STREAM_USERDATA:",
                (unsigned int)userdata->type, (unsigned int)userdata_size);
                for (i = 0; i < userdata_size; i+=4) {
                    DEBUG_PRINT_HIGH("        %x %x %x %x",
                        data_ptr[i], data_ptr[i+1],
                        data_ptr[i+2], data_ptr[i+3]);
                }
        DEBUG_PRINT_HIGH(
                "=========== End of Userdata ===========");
    } else if (extra->eType == (OMX_EXTRADATATYPE)OMX_ExtraDataVQZipSEI) {
        OMX_QCOM_EXTRADATA_VQZIPSEI *vq = (OMX_QCOM_EXTRADATA_VQZIPSEI *)(void *)extra->data;
        DEBUG_PRINT_HIGH(
                "--------------  VQZip  -------------\n"
                "    Size: %u\n",
                (unsigned int)vq->nSize);
        DEBUG_PRINT_HIGH( "=========== End of VQZip ===========");
    } else if (extra->eType == OMX_ExtraDataNone) {
        DEBUG_PRINT_HIGH("========== End of Terminator ===========");
    } else {
        DEBUG_PRINT_HIGH("======= End of Driver Extradata ========");
    }
}

// Main functions to support vendor extensions
void VppOmxComponent::initOmxVppExtensions(VendorExtensionStore &store) {
    // Init vpp vendor extension
    // use enum hqv_control_type as extension index directly
    DEBUG_PRINT_LOW("OmxVppExt: initOmxVppExtensions");
    ADD_EXTENSION("qti-ext-vpp", HQV_CONTROL_NONE, OMX_DirOutput)
    ADD_PARAM(CMP_NAME, OMX_AndroidVendorValueString)
    ADD_PARAM_END(MODE, OMX_AndroidVendorValueString)

    ADD_EXTENSION("qti-ext-vpp-cade", HQV_CONTROL_CADE, OMX_DirOutput)
    ADD_PARAM(CADE_MODE, OMX_AndroidVendorValueString)
    ADD_PARAM(CADE_LEVEL, OMX_AndroidVendorValueInt32)
    ADD_PARAM(CADE_CONTRAST, OMX_AndroidVendorValueInt32)
    ADD_PARAM_END(CADE_SATURATION, OMX_AndroidVendorValueInt32)

    ADD_EXTENSION("qti-ext-vpp-di", HQV_CONTROL_DI, OMX_DirOutput)
    ADD_PARAM_END(DI_MODE, OMX_AndroidVendorValueString)

    ADD_EXTENSION("qti-ext-vpp-cnr", HQV_CONTROL_CNR, OMX_DirOutput)
    ADD_PARAM(CNR_MODE, OMX_AndroidVendorValueString)
    ADD_PARAM_END(CNR_LEVEL, OMX_AndroidVendorValueInt32)

    ADD_EXTENSION("qti-ext-vpp-aie", HQV_CONTROL_AIE, OMX_DirOutput)
    ADD_PARAM(AIE_MODE, OMX_AndroidVendorValueString)
    ADD_PARAM(HUE_MODE, OMX_AndroidVendorValueString)
    ADD_PARAM(CADE_LEVEL, OMX_AndroidVendorValueInt32)
    ADD_PARAM(LTM_LEVEL, OMX_AndroidVendorValueInt32)
    ADD_PARAM(LTM_SAT_GAIN, OMX_AndroidVendorValueInt32)
    ADD_PARAM(LTM_SAT_OFFSET, OMX_AndroidVendorValueInt32)
    ADD_PARAM(LTM_ACE_STR, OMX_AndroidVendorValueInt32)
    ADD_PARAM(LTM_ACE_BRIGHTNESS_L, OMX_AndroidVendorValueInt32)
    ADD_PARAM_END(LTM_ACE_BRIGHTNESS_H, OMX_AndroidVendorValueInt32)

    ADD_EXTENSION("qti-ext-vpp-frc", HQV_CONTROL_FRC, OMX_DirOutput)
    ADD_PARAM(FRC_MODE, OMX_AndroidVendorValueString)
    ADD_PARAM(FRC_LEVEL, OMX_AndroidVendorValueString)
    ADD_PARAM_END(FRC_INTERP, OMX_AndroidVendorValueString)

    ADD_EXTENSION("qti-ext-vpp-ear", HQV_CONTROL_EAR, OMX_DirOutput)
    ADD_PARAM_END(EAR_MODE, OMX_AndroidVendorValueString)

    ADD_EXTENSION("qti-ext-vpp-qbr", HQV_CONTROL_QBR, OMX_DirOutput)
    ADD_PARAM_END(QBR_MODE, OMX_AndroidVendorValueString)

    ADD_EXTENSION("qti-ext-vpp-meas", HQV_CONTROL_MEAS, OMX_DirOutput)
    ADD_PARAM_END(MEAS_MODE, OMX_AndroidVendorValueString)

    ADD_EXTENSION("qti-ext-vpp-demo", HQV_CONTROL_GLOBAL_DEMO, OMX_DirOutput)
    ADD_PARAM(PROCESS_PERCENT, OMX_AndroidVendorValueInt32)
    ADD_PARAM_END(PROCESS_SIDE, OMX_AndroidVendorValueString)
}

OMX_ERRORTYPE VppOmxComponent::getOmxVppExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *ext) {
    uint32_t index = ext->nIndex - mDecoderExtensionNum;
    if (index >= mOmxVppVendorExtensionStore.size())
        return OMX_ErrorNoMore;
    const VendorExtension& vExt = mOmxVppVendorExtensionStore[index];
    if (debug_ext)
        DEBUG_PRINT_LOW("OmxVppExt: getConfig: omx index=%u (%s), omx vpp index = %u",
                ext->nIndex, vExt.name(), index);
    vExt.copyInfoTo(ext);
    if (ext->nParamSizeUsed < vExt.paramCount()) {
        // this happens during initial getConfig to query only extension-name and param-count
        return OMX_ErrorNone;
    }

    bool setStatus = true;
    switch ((enum hqv_control_type)vExt.extensionIndex()) {
        case HQV_CONTROL_NONE:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.mode, MAP_INDEX_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, MODE, str);
            break;
        }
        case HQV_CONTROL_GLOBAL_DEMO:
        {
            setStatus &= vExt.setParamInt32(ext, PROCESS_PERCENT, mVppHqvCtrlValue.demo.process_percent);
            const char *str = getStringFromEnum(mVppHqvCtrlValue.demo.process_direction, MAP_INDEX_PROCESS_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, PROCESS_SIDE, str);
            break;
        }
        case HQV_CONTROL_CADE:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.cade.mode, MAP_INDEX_CADE_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, CADE_MODE, str);
            setStatus &= vExt.setParamInt32(ext, CADE_LEVEL, mVppHqvCtrlValue.cade.cade_level);
            setStatus &= vExt.setParamInt32(ext, CADE_CONTRAST, mVppHqvCtrlValue.cade.contrast);
            setStatus &= vExt.setParamInt32(ext, CADE_SATURATION, mVppHqvCtrlValue.cade.saturation);
            break;
        }
        case HQV_CONTROL_DI:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.di.mode, MAP_INDEX_DI_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, DI_MODE, str);
            break;
        }
        case HQV_CONTROL_CNR:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.cnr.mode, MAP_INDEX_CNR_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, CNR_MODE, str);
            setStatus &= vExt.setParamInt32(ext, CNR_LEVEL, mVppHqvCtrlValue.cnr.level);
            break;
        }
        case HQV_CONTROL_AIE:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.aie.mode, MAP_INDEX_AIE_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, AIE_MODE, str);
            const char *str2 = getStringFromEnum(mVppHqvCtrlValue.aie.hue_mode, MAP_INDEX_HUE_MODE);
            setStatus &= (str2 != NULL)
                    && vExt.setParamString(ext, HUE_MODE, str2);
            setStatus &= vExt.setParamInt32(ext, CADE_LEVEL, mVppHqvCtrlValue.aie.cade_level);
            setStatus &= vExt.setParamInt32(ext, LTM_LEVEL, mVppHqvCtrlValue.aie.ltm_level);
            setStatus &= vExt.setParamInt32(ext, LTM_SAT_GAIN, mVppHqvCtrlValue.aie.ltm_sat_gain);
            setStatus &= vExt.setParamInt32(ext, LTM_SAT_OFFSET, mVppHqvCtrlValue.aie.ltm_sat_offset);
            setStatus &= vExt.setParamInt32(ext, LTM_ACE_STR, mVppHqvCtrlValue.aie.ltm_ace_str);
            setStatus &= vExt.setParamInt32(ext, LTM_ACE_BRIGHTNESS_L, mVppHqvCtrlValue.aie.ltm_ace_bright_l);
            setStatus &= vExt.setParamInt32(ext, LTM_ACE_BRIGHTNESS_H, mVppHqvCtrlValue.aie.ltm_ace_bright_h);
            break;
        }
        case HQV_CONTROL_FRC:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.frc.mode, MAP_INDEX_FRC_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, FRC_MODE, str);
            const char *str2 = getStringFromEnum(mVppHqvCtrlValue.frc.level, MAP_INDEX_FRC_LEVEL);
            setStatus &= (str2 != NULL)
                    && vExt.setParamString(ext, FRC_LEVEL, str2);
            const char *str3 = getStringFromEnum(mVppHqvCtrlValue.frc.interp, MAP_INDEX_FRC_INTERP);
            setStatus &= (str3 != NULL)
                    && vExt.setParamString(ext, FRC_INTERP, str3);
            break;
        }
        case HQV_CONTROL_EAR:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.ear.mode, MAP_INDEX_EAR_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, EAR_MODE, str);
            break;
        }
        case HQV_CONTROL_QBR:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.qbr.mode, MAP_INDEX_QBR_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, QBR_MODE, str);
            break;
        }
        case HQV_CONTROL_MEAS:
        {
            const char *str = getStringFromEnum(mVppHqvCtrlValue.meas.enable, MAP_INDEX_MEAS_MODE);
            setStatus &= (str != NULL)
                    && vExt.setParamString(ext, MEAS_MODE, str);
            break;
        }
        default:
        {
            DEBUG_PRINT_ERROR("No vpp extension index was found");
            return OMX_ErrorNotImplemented;
        }
    }
    return setStatus ? OMX_ErrorNone : OMX_ErrorUndefined;
}

OMX_ERRORTYPE VppOmxComponent::setOmxVppExtension(OMX_CONFIG_ANDROID_VENDOR_EXTENSIONTYPE *ext) {
    uint32_t index = 0;
    for (index = 0; index < mOmxVppVendorExtensionStore.size(); index++) {
        if (!strncmp((char *)ext->cName, mOmxVppVendorExtensionStore[index].name(), OMX_MAX_STRINGNAME_SIZE)) {
            if (debug_ext)
                DEBUG_PRINT_LOW("OmxVppExt: find extension name: %s index: %u", (char *)ext->cName, index);
            break;
        }
    }
    if (index == mOmxVppVendorExtensionStore.size()) {
        DEBUG_PRINT_ERROR("OmxVppExt: unrecognized vendor extension name (%s)", (char *)ext->cName);
        return OMX_ErrorBadParameter;
    }

    const VendorExtension& vExt = mOmxVppVendorExtensionStore[index];
    DEBUG_PRINT_LOW("OmxVppExt: setConfig: omx index=%u (%s), omx vpp index = %u", ext->nIndex, vExt.name(), index);

    OMX_ERRORTYPE err = OMX_ErrorNone;
    err = vExt.isConfigValid(ext);
    if (err != OMX_ErrorNone) {
        DEBUG_PRINT_ERROR("OmxVppExt: config is not valid");
        return err;
    }

    // mark this as set, regardless of set_config succeeding/failing.
    // App will know by inconsistent values in output-format
    vExt.set();

    uint32_t extensionIndex = vExt.extensionIndex();
    // Deal with "qti-ext-vpp" extension
    if (extensionIndex == HQV_CONTROL_NONE && mState == OMX_StateLoaded) {
        char str[OMX_MAX_STRINGNAME_SIZE] = {0};
        bool ret = false;
        if (mOmxHandle == NULL) {
            // If decoder is not initialized, ExtendedACodec would trigger
            // to initialize the decoder by this extension.
            ret = vExt.readParamString(ext, CMP_NAME, str);
            if (ret) {
                OMX_ERRORTYPE err = component_init(str);
                if (err != OMX_ErrorNone) {
                    DEBUG_PRINT_ERROR("failed to init decoder: %d", err);
                    return err;
                } else {
                    mCopyInputToOmx = false;
                    if (mIsQCSoftwareDecoder) {
                        DEBUG_PRINT_LOW("enable CopyToOmx in dynamic loading case(sw decoder)");
                        mCopyInputToOmx = true;
                    }
                }
            }
        }
        ret = vExt.readParamString(ext, MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.mode, MAP_INDEX_MODE);
        if (ret && (mVppHqvCtrlValue.mode == HQV_MODE_MANUAL
                || mVppHqvCtrlValue.mode == HQV_MODE_AUTO)) {
            if (mVppContext == NULL)
                mVppContext = mVppClient->init(mVppFlags, sVppCallbacks);
            if (mVppContext != NULL) {
                DEBUG_PRINT_INFO("vpp init successfully");
                set_codec_info(mOmxRole);
                if (mVppHqvCtrlValue.mode == HQV_MODE_AUTO) {
                    struct hqv_control hqvCtrl{};
                    hqvCtrl.mode = mVppHqvCtrlValue.mode;
                    if (mVppClient->setCtrl(hqvCtrl, &mVppRequirements) == VPP_OK) {
                        DEBUG_PRINT_LOW("In auto mode: vpp set ctrl return OK");
                        return OMX_ErrorNone;
                    }
                } else {
                    DEBUG_PRINT_LOW("in manual mode: wait for next vpp extension");
                    return OMX_ErrorNone;
                }
            }
        }
        DEBUG_PRINT_ERROR("fail to set vpp extension(qti-ext-vpp)");
        return OMX_ErrorBadParameter;
    }

    // Deal with other vpp extension in manual mode
    if (mVppContext == NULL || mVppHqvCtrlValue.mode != HQV_MODE_MANUAL) {
        DEBUG_PRINT_ERROR("set vpp extension in bad condition");
        return OMX_ErrorBadParameter;
    }
    bool valueSet = false;
    struct hqv_control hqvCtrl{};
    hqvCtrl.ctrl_type = (enum hqv_control_type)extensionIndex;
    hqvCtrl.mode = mVppHqvCtrlValue.mode;
    char str[OMX_MAX_STRINGNAME_SIZE] = {0};
    switch (hqvCtrl.ctrl_type) {
        case HQV_CONTROL_GLOBAL_DEMO:
        {
            valueSet |= vExt.readParamInt32(ext, PROCESS_PERCENT,
                    INT32_PTR(mVppHqvCtrlValue.demo.process_percent));
            valueSet |= vExt.readParamString(ext, PROCESS_SIDE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.demo.process_direction, MAP_INDEX_PROCESS_MODE);
            memcpy(&hqvCtrl.demo, &mVppHqvCtrlValue.demo, sizeof(hqvCtrl.demo));
            break;
        }

        case HQV_CONTROL_CADE:
        {
            valueSet |= vExt.readParamString(ext, CADE_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.cade.mode, MAP_INDEX_CADE_MODE);
            valueSet |= vExt.readParamInt32(ext, CADE_LEVEL, INT32_PTR(mVppHqvCtrlValue.cade.cade_level));
            valueSet |= vExt.readParamInt32(ext, CADE_CONTRAST, INT32_PTR(mVppHqvCtrlValue.cade.contrast));
            valueSet |= vExt.readParamInt32(ext, CADE_SATURATION, INT32_PTR(mVppHqvCtrlValue.cade.saturation));
            memcpy(&hqvCtrl.cade, &mVppHqvCtrlValue.cade, sizeof(hqvCtrl.cade));
            break;
        }
        case HQV_CONTROL_DI:
        {
            valueSet |= vExt.readParamString(ext, DI_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.di.mode, MAP_INDEX_DI_MODE);
            memcpy(&hqvCtrl.di, &mVppHqvCtrlValue.di, sizeof(hqvCtrl.di));
            break;
        }
        case HQV_CONTROL_CNR:
        {
            valueSet |= vExt.readParamString(ext, CNR_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.cnr.mode, MAP_INDEX_CNR_MODE);
            valueSet |= vExt.readParamInt32(ext, CNR_LEVEL, INT32_PTR(mVppHqvCtrlValue.cnr.level));
            memcpy(&hqvCtrl.cnr, &mVppHqvCtrlValue.cnr, sizeof(hqvCtrl.cnr));
            break;
        }
        case HQV_CONTROL_AIE:
        {
            valueSet |= vExt.readParamString(ext, AIE_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.aie.mode, MAP_INDEX_AIE_MODE);
            str[0] = '\0';
            valueSet |= vExt.readParamString(ext, HUE_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.aie.hue_mode, MAP_INDEX_HUE_MODE);
            valueSet |= vExt.readParamInt32(ext, CADE_LEVEL, INT32_PTR(mVppHqvCtrlValue.aie.cade_level));
            valueSet |= vExt.readParamInt32(ext, LTM_LEVEL, INT32_PTR(mVppHqvCtrlValue.aie.ltm_level));
            valueSet |= vExt.readParamInt32(ext, LTM_SAT_GAIN, INT32_PTR(mVppHqvCtrlValue.aie.ltm_sat_gain));
            valueSet |= vExt.readParamInt32(ext, LTM_SAT_OFFSET, INT32_PTR(mVppHqvCtrlValue.aie.ltm_sat_offset));
            valueSet |= vExt.readParamInt32(ext, LTM_ACE_STR, INT32_PTR(mVppHqvCtrlValue.aie.ltm_ace_str));
            valueSet |= vExt.readParamInt32(ext, LTM_ACE_BRIGHTNESS_L, INT32_PTR(mVppHqvCtrlValue.aie.ltm_ace_bright_l));
            valueSet |= vExt.readParamInt32(ext, LTM_ACE_BRIGHTNESS_H, INT32_PTR(mVppHqvCtrlValue.aie.ltm_ace_bright_h));
            memcpy(&hqvCtrl.aie, &mVppHqvCtrlValue.aie, sizeof(hqvCtrl.aie));
            break;
        }
        case HQV_CONTROL_FRC:
        {
            valueSet |= vExt.readParamString(ext, FRC_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.frc.mode, MAP_INDEX_FRC_MODE);
            str[0] = '\0';
            valueSet |= vExt.readParamString(ext, FRC_LEVEL, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.frc.level, MAP_INDEX_FRC_LEVEL);
            str[0] = '\0';
            valueSet |= vExt.readParamString(ext, FRC_INTERP, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.frc.interp, MAP_INDEX_FRC_INTERP);
            memcpy(&hqvCtrl.frc, &mVppHqvCtrlValue.frc, sizeof(hqvCtrl.frc));
            break;
        }

        case HQV_CONTROL_EAR:
        {
            valueSet |= vExt.readParamString(ext, EAR_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.ear.mode, MAP_INDEX_EAR_MODE);
            memcpy(&hqvCtrl.ear, &mVppHqvCtrlValue.ear, sizeof(hqvCtrl.ear));
            break;
        }
        case HQV_CONTROL_QBR:
        {
            valueSet |= vExt.readParamString(ext, QBR_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.qbr.mode, MAP_INDEX_QBR_MODE);
            memcpy(&hqvCtrl.qbr, &mVppHqvCtrlValue.qbr, sizeof(hqvCtrl.qbr));
            break;
        }
        case HQV_CONTROL_MEAS:
        {
            valueSet |= vExt.readParamString(ext, MEAS_MODE, str)
                    && getEnumFromString(str, mVppHqvCtrlValue.meas.enable, MAP_INDEX_MEAS_MODE);
            memcpy(&hqvCtrl.meas, &mVppHqvCtrlValue.meas, sizeof(hqvCtrl.meas));
            break;
        }
        default:
        {
            DEBUG_PRINT_ERROR("No vpp extension index was found");
            return OMX_ErrorNotImplemented;
        }
    }
    if (valueSet) {
        printHQVControl(hqvCtrl);
        auto eRet = mVppClient->setCtrl(hqvCtrl, &mVppRequirements);
        if (eRet == VPP_OK) {
            return OMX_ErrorNone;
        } else
            DEBUG_PRINT_ERROR("OmxVppExt: vpp_set_ctrl failed");
    }

    return OMX_ErrorBadParameter;
}

void VppOmxComponent::VppTerm() {
    if (mVppContext) {
        mVppClient->term();
        mVppContext = NULL;
    }
}

bool VppOmxComponent::enableVppInternalExtraData() {
    bool enable = false;
    QOMX_ENABLETYPE paramEnable;
    InitOMXParams(paramEnable);
    paramEnable.bEnable = OMX_TRUE;
    for (uint32_t i = 0; i < mVppRequirements.metadata.cnt; i++) {
        DEBUG_PRINT_LOW("VppExtra: enable vpp requested extra index: 0x%X",
                mVppRequirements.metadata.meta[i]);
        OMX_ERRORTYPE eRet = qc_omx_component_set_parameter(
                mOmxHandle, (OMX_INDEXTYPE) mVppRequirements.metadata.meta[i], &paramEnable);
        if (eRet != OMX_ErrorNone) {
            DEBUG_PRINT_LOW("VppExtra: failed to enable vpp extra: eRet(0x%x)", eRet);
        } else {
            enable = true;
        }
    }
    if (enable) {
        // Query omx component whether extra data is enabled
        QOMX_EXTRADATA_ENABLE pParam;
        InitOMXParams(pParam);
        pParam.nPortIndex = OMX_CORE_OUTPUT_EXTRADATA_INDEX;
        if (OMX_ErrorNone == qc_omx_component_get_parameter(mOmxHandle,
                (OMX_INDEXTYPE)OMX_QTIIndexParamVideoClientExtradata, &pParam)) {
            enable = pParam.bEnable;
        } else {
            enable = false;
        }
    }
    return enable;
}

OMX_ERRORTYPE VppOmxComponent::useBuffer(OMX_BUFFERHEADERTYPE** pBufHdr,
        OMX_U32 port, OMX_PTR app_data, OMX_U32 bytes, OMX_U8* buffer) {
    OMX_ERRORTYPE err = OMX_ErrorNone;
    err = allocate_buffer(mOmxHandle,
            pBufHdr, port, app_data, bytes);
    if (err == OMX_ErrorNone) {
        BufferMeta bufMeta;
        bufMeta.clientBuf = buffer;
        bufMeta.omxBuf = (*pBufHdr)->pBuffer;
        DEBUG_PRINT_LOW("cache input buffer client(%p)-omx(%p)",
                bufMeta.clientBuf, bufMeta.omxBuf);
        mBufferMetas.push_back(bufMeta);
    }
    DEBUG_PRINT_LOW("allocate_buffer, omxHeader=%p, port=%u, eRet=0x%x",
                    (void *)*pBufHdr, port, err);
    return err;
}

void VppOmxComponent::copyInputToOmx(OMX_BUFFERHEADERTYPE *pBufHdr) {
    if (!pBufHdr)
        return;
    for (size_t i = 0; i < mBufferMetas.size(); i++) {
        if (mBufferMetas[i].omxBuf == pBufHdr->pBuffer) {
            if (mBufferMetas[i].omxBuf && mBufferMetas[i].clientBuf) {
                uint8_t *src = mBufferMetas[i].clientBuf + pBufHdr->nOffset;
                uint8_t *dest = mBufferMetas[i].omxBuf + pBufHdr->nOffset;
                memcpy(dest, src, pBufHdr->nFilledLen);
                DEBUG_PRINT_LOW("copy (size:%u offset:%u) from client(%p) to omx(%p)",
                        pBufHdr->nFilledLen, pBufHdr->nOffset,
                        mBufferMetas[i].clientBuf, mBufferMetas[i].omxBuf);
            }
            break;
        }
    }
}
