/*-------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#include <fcntl.h>
#include <linux/msm_mdp.h>
#include <sys/mman.h>
#include <gralloc_priv.h>
#include <linux/videodev2.h>
#include <linux/msm_ion.h>
#include <media/msm_media_info.h>
#include "vdec/inc/omx_vdec.h"
#include "venc/inc/omx_video_common.h"
#include "vtest_ComDef.h"
#include "vtest_Debug.h"
#include "vtest_Thread.h"
#include "vtest_SignalQueue.h"
#include "vtest_Sleeper.h"
#include "vtest_Time.h"
#include "vtest_File.h"
#include "vtest_Decoder.h"

#undef LOG_TAG
#define LOG_TAG "VTEST_DECODER"
using namespace android;

namespace vtest {

struct CmdType {
    OMX_EVENTTYPE   eEvent;
    OMX_COMMANDTYPE eCmd;
    OMX_U32         sCmdData;
    OMX_ERRORTYPE   eResult;
};

static ConfigEnum g_sessionInterlaceDataTypeMap[] = {
    { (OMX_STRING)"FRAME_PROGRESSIVE",                     OMX_InterlaceFrameProgressive  },
    { (OMX_STRING)"INTERLEAVE_FRAME_TOPFIELDFIRST",        OMX_InterlaceInterleaveFrameTopFieldFirst },
    { (OMX_STRING)"INTERLEAVE_FRAME_BOTTOMFIELDFIRST",     OMX_InterlaceInterleaveFrameBottomFieldFirst },
    { (OMX_STRING)"FRAME_TOPFIELDFIRST",                   OMX_InterlaceFrameTopFieldFirst},
    { (OMX_STRING)"FRAME_BOTTOMFIELDFIRST",                OMX_InterlaceFrameBottomFieldFirst },
    { 0, 0 }

};
static ConfigEnum g_sessionUserDataTypeMap[] = {
    { (OMX_STRING)"F", 1 },
    { (OMX_STRING)"T", 2 },
    { (OMX_STRING)"B", 3 }
};

Decoder::Decoder(CodecConfigType *pConfig)
    : ISource(),
      m_bInputEOSReached(OMX_FALSE),
      m_bSecureSession(OMX_FALSE),
      m_bPortReconfig(OMX_FALSE),
      m_nExtraDataOutputBufferSettoOMX(OMX_FALSE),
      m_pSignalQueue(new SignalQueue(32, sizeof(CmdType))),
      m_pFreeBufferQueue(new SignalQueue(32, sizeof(BufferInfo*))),
      m_nFrames(0),
      m_nFramerate(0),
      m_nFrameWidth(0),
      m_nFrameHeight(0),
      m_nInputBufferCount(0),
      m_nInputBufferSize(0),
      m_nOutputBufferCount(0),
      m_nOutputBufferSize(0),
      m_nOriginalOutputBufferCount(0),
      m_pMetaBuffer(NULL),
      m_hDecoder(NULL),
      m_eState(OMX_StateLoaded),
      m_eStatePending(OMX_StateLoaded),
      m_eCodec(OMX_VIDEO_CodingUnused),
      m_ePlaybackMode(DynamicBufferMode),
      m_nXMLColorFormat(0),
      m_nConfiguredColorFormat(0),
      m_nDynamicIndexCount(0),
      m_nPrevCropWidth(0),
      m_nPrevCropHeight(0),
      m_fMISRFile(NULL),
      m_fInterlace(NULL),
      m_fInterlaceRef(NULL),
      m_fAspectRatio(NULL),
      m_fAspectRatioRef(NULL),
      m_fFrameRate(NULL),
      m_fFrameRateRef(NULL),
      m_fConcealMB(NULL),
      m_fConcealMBRef(NULL),
      m_fRecoveryPointSEI(NULL),
      m_fRecoveryPointSEIRef(NULL),
      m_fPanScan(NULL),
      m_fPanScanRef(NULL),
      m_fTimeStamp(NULL),
      m_fTimeStampRef(NULL),
      m_fS3D(NULL),
      m_fS3DRef(NULL),
      m_fUserData(NULL),
      m_fUserDataRef(NULL),
      m_fVUIColorAspects(NULL),
      m_fVUIColorAspectsRef(NULL),
      m_fFrameQP(NULL),
      m_fFrameQPRef(NULL) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_STRING role = (OMX_STRING)"";
    static OMX_CALLBACKTYPE callbacks =
        { EventCallback, EmptyDoneCallback, FillDoneCallback };

    snprintf(m_pName, MAX_NAME_LENGTH, "%s", "Decoder");
    VTEST_MSG_LOW("%s created", Name());
    m_eCodec = pConfig->eCodec;

    // Init ReconfigValidationTable list
    list_init(&m_pCropSettingChangeValidationList);
    list_init(&m_pPortDefinitionChangeValidationList);

    // open decoder handle, set compression format on i/p port
    GetComponentRole(pConfig->bSecureSession, &pConfig->eFileType, &role);

    m_sPortFmt.format.video.eCompressionFormat = m_eCodec;

    result = OMX_GetHandle(&m_hDecoder, role, this, &callbacks);
    if (result != OMX_ErrorNone || m_hDecoder == NULL) {
        VTEST_MSG_ERROR("Error getting component handle, rv: %d", result);
        return;
    }

    QOMX_VIDEO_QUERY_DECODER_INSTANCES decoder_instances;
    result = OMX_GetConfig(m_hDecoder,
            (OMX_INDEXTYPE)OMX_QcomIndexQueryNumberOfVideoDecInstance,
            &decoder_instances);
    FAILED0(result, "Decoder: Failed OMX_QcomIndexQueryNumberOfVideoDecInstance");

    VTEST_MSG_HIGH("Decoder: Number of decoder instances %u",
                   (unsigned int)decoder_instances.nNumOfInstances);

    // Get the port information
    OMX_INIT_STRUCT(&m_sPortParam, OMX_PORT_PARAM_TYPE);
    result = OMX_GetParameter(
            m_hDecoder, OMX_IndexParamVideoInit, (OMX_PTR)&m_sPortParam);
    FAILED0(result, "Decoder: Failed OMX_IndexParamVideoInit");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Decoder::~Decoder() {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    CropSettingChangeValidation *nodeCropReconfigEntry = NULL;
    PortDefinitionChangeValidation *nodePortReconfigEntry = NULL;
    //Move this a to an intermediate function between stop and destructor
    result = SetState(OMX_StateLoaded, OMX_TRUE);
    FAILED(result, "Could not move to OMX_StateLoaded!");

    Mutex::Autolock autoLock(m_pLock);

    if(m_pCRCChecker)
    {
        /* Check pending CRC count and update error code */
        int cnt;
        if ((m_nFrames == 0) &&   // if stream parser in read till end of file mode
            (QOMX_VIDEO_DECODE_ORDER != m_eDecoderPictureOrder) &&
            !m_bIsSeek &&
            ((cnt = m_pCRCChecker->GetPendingCRCCount(m_pCRCChecker)) != 0) )
        {
            SetError();
            VTEST_MSG_ERROR("CRC pending cnt %d!!",cnt);
        }
        m_pCRCChecker->Destroy(m_pCRCChecker);
        free(m_pCRCChecker);
    }

    list_clear(struct CropSettingChangeValidation, nodeCropReconfigEntry, &m_pCropSettingChangeValidationList, list);
    list_clear(struct PortDefinitionChangeValidation, nodePortReconfigEntry, &m_pPortDefinitionChangeValidationList, list);

    if (m_hDecoder != NULL) {
        OMX_FreeHandle(m_hDecoder);
        m_hDecoder = NULL;
    }
    if (m_pSignalQueue != NULL) {
        delete m_pSignalQueue;
        m_pSignalQueue = NULL;
    }
    if (m_pFreeBufferQueue != NULL) {
        delete m_pFreeBufferQueue;
        m_pFreeBufferQueue = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
PortBufferCapability Decoder::GetBufferRequirements(OMX_U32 ePortIndex) {

    PortBufferCapability sBufCap;

    Mutex::Autolock autoLock(m_pLock);
    memset(&sBufCap, 0, sizeof(PortBufferCapability));

    if (m_nInputBufferCount <= 0 || m_nOutputBufferSize <= 0) {
        VTEST_MSG_ERROR("Error: must call configure to get valid buf reqs");
        return sBufCap;
    }

    if (ePortIndex == PORT_INDEX_IN) {
        sBufCap.bAllocateBuffer = OMX_TRUE;
        sBufCap.bUseBuffer = OMX_FALSE;
        sBufCap.pSource = this;
        sBufCap.ePortIndex = ePortIndex;
        sBufCap.nWidth = m_nFrameWidth;
        sBufCap.nHeight = m_nFrameHeight;
        sBufCap.nMinBufferSize = m_nInputBufferSize;
        sBufCap.nMinBufferCount = m_nInputBufferCount;
        sBufCap.nExtraDataBufferSize = 0;
        sBufCap.nExtraDataBufferCount = 0;
        sBufCap.nExtraBufferCount = 0;
        sBufCap.nBufferUsage = 0;
    } else if (ePortIndex == PORT_INDEX_OUT) {
        sBufCap.bAllocateBuffer = OMX_TRUE;
        sBufCap.bUseBuffer = OMX_TRUE;
        sBufCap.pSource = this;
        sBufCap.ePortIndex = ePortIndex;
        sBufCap.nWidth = m_nFrameWidth;
        sBufCap.nHeight = m_nFrameHeight;
        sBufCap.nMinBufferSize = m_nOutputBufferSize;
        /* Should always report OriginalBufferCount so that
         * before starting the source, whoever queries these
         * requirements gets the same value. m_nOutputBufferCount
         * gets updated on AllocateBuffer */
        sBufCap.nMinBufferCount = m_nOriginalOutputBufferCount;
        sBufCap.nExtraDataBufferSize = m_nExtraDataOutputBufferSize;
        sBufCap.nExtraDataBufferCount = m_nExtraDataOutputBufferCount;
        sBufCap.nExtraBufferCount = 0;
        sBufCap.nBufferUsage = m_nOutputBufferUsage;
    } else {
        VTEST_MSG_ERROR("Error: invalid port selection");
    }
    return sBufCap;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::Start() {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    if (m_eState != OMX_StateExecuting) {
        result = SetState(OMX_StateExecuting, OMX_TRUE);
        FAILED1(result, "Could not move to executing state!");
    }
    return ISource::Start();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::Stop() {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    char temp;

    //TO DO: Enable this check when handling for m_nFrames as zero is enabled
    //if (m_nFrames == 0)
    {
        /* when frames to parse is ZERO ,check whether the reference file is exhausted,
         * for extra-data features for which validation against reference is enabled */
        if (m_fInterlaceRef)
        {
            if ((fscanf(m_fInterlaceRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: InterlaceRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fAspectRatioRef)
        {
            if ((fscanf(m_fAspectRatioRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: AspectRatioRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fFrameRateRef)
        {
            if ((fscanf(m_fFrameRateRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: FrameRateRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fConcealMBRef)
        {
            if ((fscanf(m_fConcealMBRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: ConcealMBRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fRecoveryPointSEIRef)
        {
            if ((fscanf(m_fRecoveryPointSEIRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: RecoveryPointSEIRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fPanScanRef)
        {
            if ((fscanf(m_fPanScanRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: PanScanRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fTimeStampRef)
        {
            if ((fscanf(m_fTimeStampRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: TimeStampRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fS3DRef)
        {
            if ((fscanf(m_fS3DRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: S3DRef");
                result = OMX_ErrorUndefined;
            }
        }
        if (m_fUserDataRef)
        {
            if ((fscanf(m_fUserDataRef," %c",&temp))!= EOF)
            {
                VTEST_MSG_ERROR("ERROR with ExtradataReferenceConsume: UserDataRef");
                result = OMX_ErrorUndefined;
            }
        }
    }

    if( m_fMISRFile)
    {
        fclose(m_fMISRFile);
        m_fMISRFile = NULL;
    }
    if( m_fInterlace)
    {
        fclose(m_fInterlace);
    }
    if (m_fInterlaceRef)
    {
        fclose(m_fInterlaceRef);
    }
    if (m_fAspectRatio)
    {
        fclose(m_fAspectRatio);
    }
    if (m_fAspectRatioRef)
    {
        fclose(m_fAspectRatioRef);
    }
    if (m_fFrameRate)
    {
        fclose(m_fFrameRate);
    }
    if (m_fFrameRateRef)
    {
        fclose(m_fFrameRateRef);
    }
    if (m_fConcealMB)
    {
        fclose(m_fConcealMB);
    }
    if (m_fConcealMBRef)
    {
        fclose(m_fConcealMBRef);
    }
    if (m_fRecoveryPointSEI)
    {
        fclose(m_fRecoveryPointSEI);
    }
    if (m_fRecoveryPointSEIRef)
    {
        fclose(m_fRecoveryPointSEIRef);
    }
    if (m_fPanScan)
    {
        fclose(m_fPanScan);
    }
    if (m_fPanScanRef)
    {
        fclose(m_fPanScanRef);
    }
    if (m_fTimeStamp)
    {
        fclose(m_fTimeStamp);
    }
    if (m_fTimeStampRef)
    {
        fclose(m_fTimeStampRef);
    }
    if (m_fS3D)
    {
        fclose(m_fS3D);
    }
    if (m_fS3DRef)
    {
        fclose(m_fS3DRef);
    }
    if (m_fUserData)
    {
        fclose(m_fUserData);
    }
    if (m_fUserDataRef)
    {
        fclose(m_fUserDataRef);
    }
    if (m_fVUIColorAspects)
    {
        fclose(m_fVUIColorAspects);
    }
    if (m_fVUIColorAspectsRef)
    {
        fclose(m_fVUIColorAspectsRef);
    }
    if( m_fFrameQP)
    {
        fclose(m_fFrameQP);
    }
    if (m_fFrameQPRef)
    {
        fclose(m_fFrameQPRef);
    }

    {
        Mutex::Autolock autoLock(m_pLock);
        m_bThreadStop = OMX_TRUE;
        if ((m_eState == OMX_StateIdle)
                || (m_eStatePending == OMX_StateIdle)) {
            return result;
        }
    }
    if(pExtraDatapBuffer) {
        free(pExtraDatapBuffer);
        pExtraDatapBuffer = NULL;
    }
    if (m_pMetaBuffer) {
        free(m_pMetaBuffer);
        m_pMetaBuffer = NULL;
    }
    FAILED(SetState(OMX_StateIdle, OMX_FALSE), "Could not move to OMX_StateIdle!");

    /* We should not acquire a lock across sendcommand because of a possible
     * deadlock scenario */
    Flush(OMX_ALL);
    OMX_SendCommand(m_hDecoder, OMX_CommandPortDisable, PORT_INDEX_OUT, 0);
    OMX_SendCommand(m_hDecoder, OMX_CommandPortDisable, PORT_INDEX_IN, 0);
    result = (OMX_ERRORTYPE)((OMX_S32)result |(OMX_S32)ISource::Stop());

    return result;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::Configure(CodecConfigType *pConfig,
        BufferManager *pBufManager, ISource *pSource, ISource *pSink) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    QOMX_EXTRADATA_ENABLE clientExtraDataEnable;
    VidcStatus ret = VidcStatusSuccess;

    if (!m_hDecoder) {
        VTEST_MSG_ERROR("handle is null");
        return OMX_ErrorBadParameter;
    }

    result = ISource::Configure(pConfig, pBufManager, pSource, pSink);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("Decoder configure failed");
        return result;
    }

    Mutex::Autolock autoLock(m_pLock);

    m_bSecureSession = pConfig->bSecureSession;
    m_nInputBufferCount = pConfig->nInBufferCount;
    m_nOutputBufferCount = pConfig->nOutBufferCount;
    m_bThumbnailMode = pConfig->bThumbnailMode;
    if (!pConfig->bThumbnailMode)
        m_nFrames = pConfig->nFrames;
    else {
        QOMX_ENABLETYPE enableType;
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
        result = OMX_SetParameter(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QcomIndexParamVideoSyncFrameDecodingMode, &enableType);
        FAILED1(result, "ERROR: Failed to enable Sync Frame Decode Mode");
    }
    m_nFrameWidth = pConfig->nFrameWidth;
    m_nFrameHeight = pConfig->nFrameHeight;
    if (pConfig->bThumbnailMode)
        m_ePlaybackMode = DynamicPortReconfig;
    else
        m_ePlaybackMode = pConfig->ePlaybackMode;
    m_nXMLColorFormat = pConfig->nInputColorFormat;
    if (pConfig->nInputColorFormat == QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m10bitCompressed)
        m_nConfiguredColorFormat = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed;
    else if(pConfig->nInputColorFormat == QOMX_COLOR_FORMATYUV420SemiPlanarP010Venus)
        m_nConfiguredColorFormat = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
    else
        m_nConfiguredColorFormat = pConfig->nInputColorFormat;
    m_nFrameDoneCount =0;

    pDecDynamicProperties = pConfig->pDynamicProperties;
    m_nDynamicConfigArraySize = pConfig->dynamic_config_array_size;

    FileType eFileType = pConfig->eFileType;
    OMX_U32 nNalSize = 0;

    //Enable extradata
    QOMX_ENABLETYPE extra_data;
    extra_data.bEnable = OMX_TRUE;
    extra_data.nSize = sizeof(QOMX_ENABLETYPE);

    char frameinfo_value[PROPERTY_VALUE_MAX] = {0};
    char interlace_value[PROPERTY_VALUE_MAX] = {0};
    char h264info_value[PROPERTY_VALUE_MAX] = {0};
    char video_qp_value[PROPERTY_VALUE_MAX] = {0};
    char videoinput_bitsinfo_value[PROPERTY_VALUE_MAX] = {0};
    char framepack_value[PROPERTY_VALUE_MAX] = {0};
    char extn_userdata_value[PROPERTY_VALUE_MAX] = {0};
    OMX_U32 frameinfo = 0, interlace = 0, h264info = 0, video_qp = 0, videoinput_bitsinfo = 0,
            framepack = 0, extn_userdata = 0, i=0;
    char sMISRFileName[MAX_STR_LEN] = {0};
    char sExtraDataFileName[MAX_STR_LEN] = {0};

#ifndef PRE_SDM845
    // Enable MISR verification by setting Output Crop Extradata
    if (strlen(pConfig->cMISRFileName) > 0) {
        result = OMX_SetParameter(m_hDecoder,
            (OMX_INDEXTYPE)OMX_QcomIndexParamOutputCropExtraData, (OMX_PTR)&extra_data);
        FAILED1(result, "ERROR: Failed to enable Output Crop extradata");
    }
#endif

    /* parse extradata properties array and enable extra data info */
    for(i=0;i<pConfig->nExtradataConfigArraySize;i++)
    {
        switch ((int)pConfig->pExtradataProperties[i].ExtradataType)
        {
            case FrameQPExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamVideoQPExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable QP extradata");
                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                    pConfig->cInFileName, "_qp.txt");
                    if (m_fFrameQP)
                    {
                        VTEST_MSG_ERROR("Duplicate QP extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fFrameQP=fopen(sExtraDataFileName, "w");
                        if (!m_fFrameQP)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fFrameQPRef)
                    {
                        VTEST_MSG_ERROR("Duplicate QP extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fFrameQPRef = fopen(sExtraDataFileName, "r");
                        if (!m_fFrameQPRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case InterlaceExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamInterlaceExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable Interalce extradata");
                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                    pConfig->cInFileName, "_interlace.txt");
                    if (m_fInterlace)
                    {
                        VTEST_MSG_ERROR("Duplicate interlace extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fInterlace=fopen(sExtraDataFileName, "w");
                        if (!m_fInterlace)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fInterlaceRef)
                    {
                        VTEST_MSG_ERROR("Duplicate interlace extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fInterlaceRef = fopen(sExtraDataFileName, "r");
                        if (!m_fInterlaceRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case AspectRatioExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable FrameInfo - AspectRatio extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                 pConfig->cInFileName, "_AspectRatio.txt");
                    if (m_fAspectRatio)
                    {
                        VTEST_MSG_ERROR("Duplicate AspectRatio extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fAspectRatio=fopen(sExtraDataFileName, "w");
                        if (!m_fAspectRatio)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fAspectRatioRef)
                    {
                        VTEST_MSG_ERROR("Duplicate AspectRatio extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fAspectRatioRef= fopen(sExtraDataFileName, "r");
                        if (!m_fAspectRatioRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case FrameRateExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable FrameInfo - FrameRate extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                   pConfig->cInFileName, "_FrameRate.txt");
                    if (m_fFrameRate)
                    {
                        VTEST_MSG_ERROR("Duplicate FrameRate extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fFrameRate=fopen(sExtraDataFileName, "w");
                        if (!m_fFrameRate)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fFrameRateRef)
                    {
                        VTEST_MSG_ERROR("Duplicate FrameRate extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fFrameRateRef= fopen(sExtraDataFileName, "r");
                        if (!m_fFrameRateRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case ConcealMBExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable FrameInfo - ConcealMB extradata");
                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                pConfig->cInFileName, "_NumConcealMB.txt");
                    if (m_fConcealMB)
                    {
                        VTEST_MSG_ERROR("Duplicate ConcealMB extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fConcealMB=fopen(sExtraDataFileName, "w");
                        if (!m_fConcealMB)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fConcealMBRef)
                    {
                        VTEST_MSG_ERROR("Duplicate ConcealMB extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fConcealMBRef= fopen(sExtraDataFileName, "r");
                        if (!m_fConcealMBRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case RecoveryPointSEIExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable FrameInfo - RecoveryPointSEI extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                            pConfig->cInFileName, "_RecoveryPointSEI.txt");
                    if (m_fRecoveryPointSEI)
                    {
                        VTEST_MSG_ERROR("Duplicate RecoveryPointSEI extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fRecoveryPointSEI=fopen(sExtraDataFileName, "w");
                        if (!m_fRecoveryPointSEI)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fRecoveryPointSEIRef)
                    {
                        VTEST_MSG_ERROR("Duplicate RecoveryPointSEI extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fRecoveryPointSEIRef= fopen(sExtraDataFileName, "r");
                        if (!m_fRecoveryPointSEIRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case PanScanWindowExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData,
                                                                                    (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable FrameInfo - PanScanWindow extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                     pConfig->cInFileName, "_PanScan.txt");
                    if (m_fPanScan)
                    {
                        VTEST_MSG_ERROR("Duplicate PanScanWindow extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fPanScan=fopen(sExtraDataFileName, "w");
                        if (!m_fPanScan)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fPanScanRef)
                    {
                        VTEST_MSG_ERROR("Duplicate PanScanWindow extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fPanScanRef= fopen(sExtraDataFileName, "r");
                        if (!m_fPanScanRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case TimeStampExtraData:
            {
                /* enable extradata */
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamH264TimeInfo,
                                                                              (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable TimeStamp extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                   pConfig->cInFileName, "_TimeStamp.txt");
                    if (m_fTimeStamp)
                    {
                        VTEST_MSG_ERROR("Duplicate TimeStamp extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fTimeStamp=fopen(sExtraDataFileName, "w");
                        if (!m_fTimeStamp)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fTimeStampRef)
                    {
                        VTEST_MSG_ERROR("Duplicate TimeStamp extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fTimeStampRef= fopen(sExtraDataFileName, "r");
                        if (!m_fTimeStampRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case S3DFramePackingExtraData:
            {
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexParamVideoFramePackingExtradata,
                                                                                            (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable S3DFramePacking extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                               pConfig->cInFileName, "_S3DFrmPacking.txt");
                    if (m_fS3D)
                    {
                        VTEST_MSG_ERROR("Duplicate S3DFramePacking extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fS3D=fopen(sExtraDataFileName, "w");
                        if (!m_fS3D)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fS3DRef)
                    {
                        VTEST_MSG_ERROR("Duplicate S3DFramePacking extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fS3DRef= fopen(sExtraDataFileName, "r");
                        if (!m_fS3DRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case UserDataExtraData:
            {
                result = OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QcomIndexEnableExtnUserData,
                                                                               (OMX_PTR)&extra_data);
                FAILED1(result, "ERROR: Failed to enable UserData extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                   pConfig->cInFileName, "_userdata.txt");
                    if (m_fUserData)
                    {
                        VTEST_MSG_ERROR("Duplicate UserData extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fUserData=fopen(sExtraDataFileName, "w");
                        if (!m_fUserData)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sMISRRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fUserDataRef)
                    {
                        VTEST_MSG_ERROR("Duplicate UserData extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fUserDataRef= fopen(sExtraDataFileName, "r");
                        if (!m_fUserDataRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            case ColorAspectsInfo:
            {
                result = SetVUIColorAspectsExtraData(pConfig->sColorAspects);
                FAILED1(result, "ERROR: Failed to enable H264 VUI extradata");

                if (pConfig->pExtradataProperties[i].EnableDump)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s%s", sGlobalStaticVideoProp.sOutRoot,
                                                   pConfig->cInFileName, "_colorprimaries.txt");
                    VTEST_MSG_HIGH("VUI Color Aspects extradata entry %s", sExtraDataFileName);
                    if (m_fVUIColorAspects)
                    {
                        VTEST_MSG_ERROR("Duplicate VUI Color Aspects extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fVUIColorAspects = fopen(sExtraDataFileName, "w");
                        if (!m_fVUIColorAspects)
                        {
                            VTEST_MSG_ERROR("File open failed %s",sExtraDataFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                VTEST_MSG_ERROR("VUI Reference name %s", pConfig->pExtradataProperties[i].RefFileName);
                if (pConfig->pExtradataProperties[i].RefFileName)
                {
                    SNPRINTF(sExtraDataFileName, MAX_STR_LEN, "%s%s",sGlobalStaticVideoProp.sCRCRoot,pConfig->pExtradataProperties[i].RefFileName);
                    if (m_fVUIColorAspectsRef)
                    {
                        VTEST_MSG_ERROR("Duplicate VUI Color Aspects extradata entry %s", sExtraDataFileName);
                        result = OMX_ErrorBadParameter;
                    }
                    else
                    {
                        m_fVUIColorAspectsRef = fopen(sExtraDataFileName, "r");
                        VTEST_MSG_HIGH("Configure: Ref Color aspects File name %s file handle 0x%x",
                            sExtraDataFileName, (unsigned int)m_fVUIColorAspectsRef);
                        if (!m_fVUIColorAspectsRef)
                        {
                            VTEST_MSG_ERROR("File open failed %s",pConfig->pExtradataProperties[i].RefFileName);
                            result= OMX_ErrorBadParameter;
                        }
                    }
                }
                break;
            }
            default:
            {
                VTEST_MSG_HIGH("Unsupported extradata type 0x%x\n",pConfig->pExtradataProperties[i].ExtradataType);
                break;
            }
        }
    }

    property_get("vidc.vdec.debug.video_qp_value", video_qp_value, "0");
    video_qp = atoi(video_qp_value);
    if (video_qp) {
        OMX_SetParameter(m_hDecoder,
            (OMX_INDEXTYPE)OMX_QcomIndexParamVideoQPExtraData,
            (OMX_PTR)&extra_data);
    }

    property_get("vidc.vdec.debug.input_bitsinfo", videoinput_bitsinfo_value, "0");
    videoinput_bitsinfo = atoi(videoinput_bitsinfo_value);
    if (videoinput_bitsinfo) {
        OMX_SetParameter(m_hDecoder,
            (OMX_INDEXTYPE)OMX_QcomIndexParamVideoInputBitsInfoExtraData,
            (OMX_PTR)&extra_data);
    }

    //////////////////////////////////////////
    // Operating rate
    //////////////////////////////////////////
    if (pConfig->nOperatingRate) {
        OMX_PARAM_U32TYPE operatingRate;
        OMX_INIT_STRUCT(&operatingRate, OMX_PARAM_U32TYPE);
        operatingRate.nPortIndex = (OMX_U32)PORT_INDEX_IN;
        result = OMX_GetConfig(m_hDecoder,
            (OMX_INDEXTYPE)OMX_IndexConfigOperatingRate, (OMX_PTR)&operatingRate);
        if (result != OMX_ErrorNone) {
            VTEST_MSG_ERROR("Get OMX_IndexConfigOperatingRate error: %d", result);
            return result;
        }

        VTEST_MSG_HIGH("Setting operating rate = %u", pConfig->nOperatingRate);
        OMX_INIT_STRUCT_SIZE(&operatingRate, OMX_PARAM_U32TYPE);
        if (pConfig->nOperatingRate != QOMX_VIDEO_HIGH_PERF_OPERATING_MODE) {
            operatingRate.nU32 = (pConfig->nOperatingRate << 16);
        } else {
            operatingRate.nU32 = QOMX_VIDEO_HIGH_PERF_OPERATING_MODE;
        }

        result = OMX_SetConfig(m_hDecoder,
            (OMX_INDEXTYPE)OMX_IndexConfigOperatingRate, (OMX_PTR)&operatingRate);
        if (result != OMX_ErrorNone) {
            VTEST_MSG_ERROR("Set OMX_IndexConfigOperatingRate error: %d", result);
            return result;
        }
    }

    // PlaybackMode
    switch (m_ePlaybackMode) {

        OMX_INDEXTYPE index;
        OMX_STRING name;

        case QCSmoothStreaming:
            VTEST_MSG_HIGH("Enabling QCSmoothStreaming");
            OMX_INIT_STRUCT_SIZE(&m_sPortFmt, OMX_PARAM_PORTDEFINITIONTYPE);
            result = OMX_SetParameter(m_hDecoder,
                    (OMX_INDEXTYPE)OMX_QcomIndexParamEnableSmoothStreaming,
                    (OMX_PTR)&m_sPortFmt);
            FAILED1(result, "Failed to set QCSmoothStreaming");
            break;
        case AdaptiveSmoothStreaming:
            VTEST_MSG_HIGH("Enabling AdaptiveSmoothStreaming");
            name = const_cast<OMX_STRING>(
                    "OMX.google.android.index.prepareForAdaptivePlayback");
            result = OMX_GetExtensionIndex(m_hDecoder, name, &index);
            FAILED1(result, "OMX_GetExtensionIndex %s failed", name);

            PrepareForAdaptivePlaybackParams params;
            params.nSize = sizeof(params);
            params.nVersion.s.nVersionMajor = 1;
            params.nVersion.s.nVersionMinor = 0;
            params.nVersion.s.nRevision = 0;
            params.nVersion.s.nStep = 0;
            params.nPortIndex = OMX_DirOutput;
            params.bEnable = OMX_TRUE;
            params.nMaxFrameWidth = pConfig->nAdaptiveWidth;
            params.nMaxFrameHeight = pConfig->nAdaptiveHeight;

            result = OMX_SetParameter(m_hDecoder, index, &params);
            FAILED1(result, "Failed to set AdaptiveSmoothStreaming");
            break;
        case DynamicBufferMode:
            VTEST_MSG_HIGH("Enabling DynamicBufferMode");
            StoreMetaDataInBuffersParams metadata_params;
            OMX_INIT_STRUCT(&metadata_params, StoreMetaDataInBuffersParams);
            metadata_params.nPortIndex = 1;
            metadata_params.bStoreMetaData = OMX_TRUE;
            name = const_cast<OMX_STRING>(
                    "OMX.google.android.index.storeMetaDataInBuffers");
            result = OMX_GetExtensionIndex(m_hDecoder, name, &index);
            FAILED1(result, "OMX_GetExtensionIndex %s failed", name);
            result = OMX_SetParameter(m_hDecoder, index, &metadata_params);
            FAILED1(result, "Failed to set Dynamic buffer mode");
            break;
        default:
            VTEST_MSG_HIGH("Running in default DynamicPortReconfig mode");
            break;
    }

    OMX_VIDEO_PARAM_PORTFORMATTYPE videoPortFmt;
    OMX_INIT_STRUCT(&videoPortFmt, OMX_VIDEO_PARAM_PORTFORMATTYPE);
    if (!CheckColorFormatSupported((OMX_COLOR_FORMATTYPE)m_nConfiguredColorFormat, &videoPortFmt)) {
        VTEST_MSG_ERROR("Decoder: No Venus specific color format[0x%x] supported\n", m_nConfiguredColorFormat);
        return OMX_ErrorUnsupportedSetting;
    }

    videoPortFmt.eColorFormat = (OMX_COLOR_FORMATTYPE)m_nConfiguredColorFormat;
    result = OMX_SetParameter(m_hDecoder, OMX_IndexParamVideoPortFormat,
                              (OMX_PTR) & videoPortFmt);
    FAILED1(result, "Decoder: Setting format[0x%x] failed", m_nConfiguredColorFormat);

    // Query the decoder input buffer requirements
    OMX_INIT_STRUCT(&m_sPortFmt, OMX_PARAM_PORTDEFINITIONTYPE);
    m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber;
    result = OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
    FAILED1(result, "Decoder: error OMX_IndexParamPortDefinition");

    VTEST_MSG_HIGH("Decoder: Input buffer count min %u\n", (unsigned int)m_sPortFmt.nBufferCountMin);
    VTEST_MSG_HIGH("Decoder: Input buffer size %u\n", (unsigned int)m_sPortFmt.nBufferSize);
    m_nInputBufferSize = m_sPortFmt.nBufferSize;
    if (m_sPortFmt.eDir != OMX_DirInput) {
        VTEST_MSG_ERROR("Decoder: expected input port");
        return OMX_ErrorUndefined;
    }

    // Set the frame and re-query the decoder input buffer requirements
    m_sPortFmt.format.video.nFrameHeight = m_nFrameHeight;
    m_sPortFmt.format.video.nFrameWidth  = m_nFrameWidth;
    m_sPortFmt.format.video.xFramerate = m_nFramerate;
    OMX_INIT_STRUCT_SIZE(&m_sPortFmt, OMX_PARAM_PORTDEFINITIONTYPE);
    result = OMX_SetParameter(m_hDecoder, OMX_IndexParamPortDefinition, (OMX_PTR)&m_sPortFmt);
    FAILED1(result, "Decoder: error set OMX_IndexParamPortDefinition");

    result = OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, (OMX_PTR)&m_sPortFmt);
    FAILED1(result, "Decoder: error get OMX_IndexParamPortDefinition");

    VTEST_MSG_HIGH("Decoder: Actual Input Buffer Count %u", (unsigned int)m_sPortFmt.nBufferCountActual);

    // override input buffer count if needed
    if (m_nInputBufferCount < m_sPortFmt.nBufferCountActual) {
        m_nInputBufferCount = m_sPortFmt.nBufferCountActual;
    }

    // Get output port buffer requirements
    m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber + 1;
    m_sPortFmt.format.video.nFrameHeight = m_nFrameHeight;
    m_sPortFmt.format.video.nFrameWidth  = m_nFrameWidth;
    m_sPortFmt.format.video.xFramerate = m_nFramerate;

    OMX_INIT_STRUCT_SIZE(&m_sPortFmt, OMX_PARAM_PORTDEFINITIONTYPE);
    result = OMX_SetParameter(m_hDecoder, OMX_IndexParamPortDefinition, (OMX_PTR)&m_sPortFmt);
    FAILED1(result, "Decoder: error set OMX_IndexParamPortDefinition");

    result = OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, (OMX_PTR)&m_sPortFmt);
    FAILED1(result, "Decoder: error get OMX_IndexParamPortDefinition");

    if (pConfig->bDownScalar) {
        QOMX_INDEXDOWNSCALAR downScalar;
        OMX_INIT_STRUCT(&downScalar, QOMX_INDEXDOWNSCALAR);
        downScalar.bEnable = OMX_FALSE;
        if (pConfig->nOutputFrameWidth && pConfig->nOutputFrameHeight) {
            downScalar.bEnable = OMX_TRUE;
            downScalar.nOutputWidth = pConfig->nOutputFrameWidth;
            downScalar.nOutputHeight = pConfig->nOutputFrameHeight;
            VTEST_MSG_HIGH("Decoder: Enabling downscalar to component, downscaled Height = %u Width = %u",
                        pConfig->nOutputFrameHeight, pConfig->nOutputFrameWidth);
            result = OMX_SetParameter(m_hDecoder,
                        (OMX_INDEXTYPE)OMX_QcomIndexParamVideoDownScalar,
                        (OMX_PTR)&downScalar);
            FAILED1(result, "Could not set DecoderDownScalar");
        }
    }

    OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
    FAILED1(result, "Decoder: Error - OMX_IndexParamPortDefinition output");

    if (m_sPortFmt.eDir != OMX_DirOutput) {
        VTEST_MSG_ERROR("Error - Expect Output Port\n");
        return OMX_ErrorUndefined;
    }

    VTEST_MSG_HIGH("Output buffer count min %u\n",
                   (unsigned int)m_sPortFmt.nBufferCountActual);
    VTEST_MSG_HIGH("Output buffer size %u\n", (unsigned int)m_sPortFmt.nBufferSize);
    m_nOutputBufferSize = m_sPortFmt.nBufferSize;
    // override input buffer count if needed
    if (m_nOutputBufferCount < m_sPortFmt.nBufferCountActual) {
        m_nOutputBufferCount = m_sPortFmt.nBufferCountActual;
    }
    m_nOriginalOutputBufferCount = m_nOutputBufferCount;

    //Do a get param of OMX_QTIIndexParamVideoClientExtradata
    clientExtraDataEnable.nSize = sizeof(QOMX_EXTRADATA_ENABLE);
    clientExtraDataEnable.nPortIndex = PORT_INDEX_EXTRADATA_OUT;
    clientExtraDataEnable.bEnable = OMX_FALSE;
    OMX_GetParameter(m_hDecoder,
                     (OMX_INDEXTYPE)OMX_QTIIndexParamVideoClientExtradata,
                     &clientExtraDataEnable);
    VTEST_MSG_HIGH("Client extradata enabled = %d",clientExtraDataEnable.bEnable);
    if(clientExtraDataEnable.bEnable) {
        //Do a set param of OMX_QTIIndexParamVideoClientExtradata
        VTEST_MSG_MEDIUM("GetParam of client extradata enable is %d\n",
                         clientExtraDataEnable.bEnable);
        OMX_SetParameter(m_hDecoder,
                         (OMX_INDEXTYPE)OMX_QTIIndexParamVideoClientExtradata,
                         &clientExtraDataEnable);

        //Get extradata output port buffer requirements
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber +
                                PORT_INDEX_EXTRADATA_OUT;//get from index

        OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
        FAILED1(result, "Error: getparam of PortDefinition for extradata output");

        VTEST_MSG_HIGH("Decoder: Extradata Output buffer count %u buffer size %u\n",
            (unsigned int)m_sPortFmt.nBufferCountActual,
            (unsigned int)m_sPortFmt.nBufferSize);
        m_nExtraDataOutputBufferSize = m_sPortFmt.nBufferSize;
        // Add 4 buffers to extradata output buffer count
        if (m_nExtraDataOutputBufferCount < m_sPortFmt.nBufferCountActual) {
            m_nExtraDataOutputBufferCount = m_sPortFmt.nBufferCountActual;
        }
        m_sPortFmt.nBufferCountActual = m_nExtraDataOutputBufferCount;
        //Set extradata output port buffer requirements
        OMX_SetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
        FAILED1(result, "Error: setparam of PortDefinition for extradata output");

        VTEST_MSG_HIGH("Extradata Output buffer count %u buffer size %u\n",
                       m_nExtraDataOutputBufferCount, m_nExtraDataOutputBufferSize);
    }
    //configure priorty
    if (pConfig->nPriority != 0xDEAD) //to check priority tag is defined or not in xml
    {
        OMX_PARAM_U32TYPE Priority;
        OMX_INIT_STRUCT(&Priority, OMX_PARAM_U32TYPE);
        Priority.nU32 = pConfig->nPriority;
        VTEST_MSG_MEDIUM("Decoder: Priority %u", (unsigned int)Priority.nU32);
        result = OMX_SetConfig(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_IndexConfigPriority,
                 (OMX_PTR)&Priority);
        FAILED1(result, "Decoder: ERROR: OMX_IndexConfigPriority");
    }
    //setting LowLatency
    if (pConfig->nLowLatency != 0xDEAD)//to check LowLatecy tag is defined or not in xml
    {
        QOMX_EXTNINDEX_VIDEO_LOW_LATENCY_MODE LowLatency;
        OMX_INIT_STRUCT(&LowLatency, QOMX_EXTNINDEX_VIDEO_LOW_LATENCY_MODE);
        LowLatency.bEnableLowLatencyMode = (OMX_BOOL)pConfig->nLowLatency;
        VTEST_MSG_MEDIUM("Decoder: LowLatency %u", (unsigned int)LowLatency.bEnableLowLatencyMode);
        result = OMX_SetParameter(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QTIIndexParamLowLatencyMode,
                 (OMX_PTR)&LowLatency);
        FAILED1(result, "Decoder: ERROR: OMX_QTIIndexParamLowLatencyMode");
    }

    // set picture order
    QOMX_VIDEO_DECODER_PICTURE_ORDER picture_order;
    OMX_INIT_STRUCT(&picture_order, QOMX_VIDEO_DECODER_PICTURE_ORDER);

    picture_order.eOutputPictureOrder = pConfig->eDecoderPictureOrder;
    m_eDecoderPictureOrder = pConfig->eDecoderPictureOrder;
    picture_order.nPortIndex = OMX_DirOutput;
    OMX_SetParameter(m_hDecoder,
            (OMX_INDEXTYPE)OMX_QcomIndexParamVideoDecoderPictureOrder,
            (OMX_PTR)&picture_order);
    FAILED1(result, "Decoder: ERROR: Setting picture order!");

    VTEST_MSG_HIGH("Decoder: Video format: W x H (%u x %u)",
                   (unsigned int)m_sPortFmt.format.video.nFrameWidth,
                   (unsigned int)m_sPortFmt.format.video.nFrameHeight);

    if (eFileType == FILE_TYPE_264_NAL_SIZE_LENGTH) {
        nNalSize = 4;
    }
    if (m_eCodec == OMX_VIDEO_CodingAVC) {

        OMX_VIDEO_CONFIG_NALSIZE naluSize;
        OMX_INIT_STRUCT(&naluSize, OMX_VIDEO_CONFIG_NALSIZE);
        naluSize.nNaluBytes = nNalSize; //2; //appears to always be 2 or 4
        result = OMX_SetConfig(m_hDecoder, OMX_IndexConfigVideoNalSize, (OMX_PTR)&naluSize);
        FAILED1(result, "Decoder: ERROR: OMX_IndexConfigVideoNalSize");
        VTEST_MSG_MEDIUM("Decoder: Nal length set to %u", (unsigned int)naluSize.nNaluBytes);
        if (pConfig->nPictureTypeDecode) {
            result = SetPictureTypeDecode(pConfig->nPictureTypeDecode);
            FAILED1(result, "Failed to set decode picture type");
        }
    }

    result = GetGraphicBufferUsage(&m_nOutputBufferUsage);
    FAILED1(result, "Failed to get graphic buffer usage flags");

    if (pConfig->bForceCompressedForDPB) {
        OMX_INDEXTYPE index;
        OMX_STRING name;
        OMX_QTI_VIDEO_PARAM_FORCE_COMPRESSED_FOR_DPB_TYPE force_compressed_for_dpb;
        OMX_INIT_STRUCT(&force_compressed_for_dpb, OMX_QTI_VIDEO_PARAM_FORCE_COMPRESSED_FOR_DPB_TYPE);

        name = const_cast<OMX_STRING>(
                    "OMX.QTI.index.param.video.ForceCompressedForDPB");
        result = OMX_GetExtensionIndex(m_hDecoder, name, &index);
        FAILED1(result, "OMX_GetExtensionIndex %s failed", name);

        force_compressed_for_dpb.bEnable = pConfig->bForceCompressedForDPB;
        result = OMX_SetParameter(m_hDecoder, index,
                     (OMX_PTR)&force_compressed_for_dpb);
        FAILED1(result, "Could not set OMX_QTIIndexParamForceCompressedForDPB");
    }

#ifndef PRE_SDM845
    if ((sGlobalStaticVideoProp.bGenerateMisr) ||
        ((!sGlobalStaticVideoProp.bSkipCrcCheck) && (pConfig->cMISRFileName[0] != '\0')))
    {
        /* Create CRC checker instance
         * 1) For generating MISR dump where CRC validation is skipped
         * 2) For validating MISR agianst reference unless disabled thru master config */
        m_pCRCChecker = (VideoCRCChecker*)malloc(sizeof(VideoCRCChecker));
        if (!m_pCRCChecker)
        {
            VTEST_MSG_ERROR("Alloc failed! Unable to create CRCchecker instance");
            return OMX_ErrorUndefined;
        }

        ret = VideoCRCCheckerInit(m_pCRCChecker,
                                  sGlobalStaticVideoProp.bGenerateMisr,
                                  VIDEO_CRCMODE_HW);
        if (ret)
        {
            VTEST_MSG_ERROR("Failed to initialize VideoCRCChekcer for MISR, ret %d",
                ret);
            return OMX_ErrorUndefined;
        }
        if (sGlobalStaticVideoProp.bGenerateMisr)
        {
            /* Update MISR file name */
            SNPRINTF(pConfig->cMISRFileName, VTEST_MAX_STRING, "%s%s%s",
                     sGlobalStaticVideoProp.sOutRoot, pConfig->cInFileName, ".misr");
            STRCPY(sMISRFileName, pConfig->cMISRFileName);
        } else if (pConfig->cMISRFileName[0] != '\0')
        {
            char sTempFileName[MAX_STR_LEN] = {0};
            VideoMisrPayload RefMisrPayload;
            int tempRetVal = 0;
            uint32 tempPrevCropWidth = 0, tempPrevCropHeight = 0;
            uint32 tempPrevCropX = 0, tempPrevCropY = 0;
            uint32 tempPrevWidth = 0, tempPrevHeight = 0;
            SNPRINTF(sTempFileName, VTEST_MAX_STRING, "%s%s",
                     sGlobalStaticVideoProp.sMISRRoot, pConfig->cMISRFileName);
            STRCPY(pConfig->cMISRFileName, sTempFileName);

            m_fMISRFile = fopen(pConfig->cMISRFileName, "r");
            if (!m_fMISRFile)
            {
                VTEST_MSG_ERROR("File %s open failed", pConfig->cMISRFileName);
                return OMX_ErrorBadParameter;
            }
            else
            {
                VTEST_MSG_LOW("File %s opened successfully", pConfig->cMISRFileName);
                // Parse the MISR file till EOF. Any change in crop
                // width, crop height, cropX, cropY should be pushed to
                // a CropSettingChangeValidation list. And any change in
                // width and height should be pushed to PortDefinitionChangeValidation list
                // Frame 0's width, height, crop width, crop height, cropX and CropY
                // should also be pushed to the respectively validation list.
                char misrInfo[256];
                fgets(misrInfo, 256, m_fMISRFile);
                while (tempRetVal != EOF) {
                    do {
                        // Below Do-While should break for the 1st frame (OR) EOF (OR) if
                        // the width, height or crop width, crop height, cropX, cropY
                        // changes in the reference MISR file
                        tempRetVal = fscanf(m_fMISRFile,
                                " %d%*c %x%*c %x%*c %x%*c %x%*c %d%*c %d%*c %d%*c %d%*c %d%*c %d%*c %d%*c %d%*c",
                                &RefMisrPayload.nFrameNum,
                                &RefMisrPayload.misrInfo[0].nMisrDPBLuma,
                                &RefMisrPayload.misrInfo[0].nMisrDPBChroma,
                                &RefMisrPayload.misrInfo[0].nMisrOPBLuma,
                                &RefMisrPayload.misrInfo[0].nMisrOPBChroma,
                                &RefMisrPayload.nWidth,
                                &RefMisrPayload.nHeight,
                                &RefMisrPayload.nCropWidth,
                                &RefMisrPayload.nCropHeight,
                                &RefMisrPayload.nCropX,
                                &RefMisrPayload.nCropY,
                                &RefMisrPayload.nBitDepthY,
                                &RefMisrPayload.nBitDepthC);
                        VTEST_MSG_MEDIUM("Read values from FILE: Crop width %d, Crop height %d",
                                     RefMisrPayload.nCropWidth, RefMisrPayload.nCropHeight);
                    } while((tempRetVal != EOF) &&
                            (RefMisrPayload.nFrameNum != 0) &&
                            (tempPrevCropWidth == RefMisrPayload.nCropWidth) &&
                            (tempPrevCropHeight == RefMisrPayload.nCropHeight) &&
                            (tempPrevCropX == RefMisrPayload.nCropX) &&
                            (tempPrevCropY == RefMisrPayload.nCropY) &&
                            (tempPrevWidth == RefMisrPayload.nWidth) &&
                            (tempPrevHeight == RefMisrPayload.nHeight));

                    if (tempPrevWidth != RefMisrPayload.nWidth ||
                        tempPrevHeight != RefMisrPayload.nHeight ) {
                        // change in width and  height.
                        // Store reference Width and Height
                        // into PortDefinitionChangeValidation list
                        PortDefinitionChangeValidation *pEntry1 = (PortDefinitionChangeValidation*)
                                                     malloc(sizeof(PortDefinitionChangeValidation));
                        pEntry1->frameNum = RefMisrPayload.nFrameNum;
                        pEntry1->width = tempPrevWidth = RefMisrPayload.nWidth;
                        pEntry1->height = tempPrevHeight = RefMisrPayload.nHeight;
                        list_insert_tail(&pEntry1->list, &m_pPortDefinitionChangeValidationList);
                    }
                    if (tempPrevCropWidth != RefMisrPayload.nCropWidth ||
                        tempPrevCropHeight != RefMisrPayload.nCropHeight ||
                        tempPrevCropX == RefMisrPayload.nCropX ||
                        tempPrevCropY == RefMisrPayload.nCropY) {
                        // change in crop rectangle
                        // Store the reference crop width and crop height
                        // into the reconfig validation list
                        CropSettingChangeValidation *pEntry2 = (CropSettingChangeValidation*)
                                                  malloc(sizeof(CropSettingChangeValidation));
                        pEntry2->frameNum = RefMisrPayload.nFrameNum;
                        pEntry2->cropWidth = tempPrevCropWidth = RefMisrPayload.nCropWidth;
                        pEntry2->cropHeight = tempPrevCropHeight = RefMisrPayload.nCropHeight;
                        pEntry2->cropX = tempPrevCropX = RefMisrPayload.nCropX;
                        pEntry2->cropY = tempPrevCropY = RefMisrPayload.nCropY;
                        list_insert_tail(&pEntry2->list, &m_pCropSettingChangeValidationList);
                    }
                }
                // close the file Handle
                fclose(m_fMISRFile);
                VTEST_MSG_LOW("File %s closed successfully", pConfig->cMISRFileName);
            }
            if ((!sGlobalStaticVideoProp.bSkipCrcCheck))
            {
                STRCPY(sMISRFileName, pConfig->cMISRFileName);
                VTEST_MSG_MEDIUM("CRC lib would be configured with %s", sMISRFileName);
            }
        }

        /* Configure CRC Checker with MISR file name.
           sMISRFileName will be empty only if skip CRC is set. */
        ret = m_pCRCChecker->Configure(m_pCRCChecker, sMISRFileName);
        if (ret)
        {
            VTEST_MSG_ERROR("Failed to configure VideoCRCChekcer for MISR, ret %d ",ret);
            return OMX_ErrorUndefined;
        }
        if (m_bThumbnailMode)
        {
            m_pCRCChecker->SetCRCCheckFileMode(m_pCRCChecker,TRUE);
        }
    }
#endif

    // go to Idle, so we can allocate buffers
    result = SetState(OMX_StateIdle, OMX_FALSE);
    FAILED1(result, "Could not move to OMX_StateIdle!");
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::SetBuffer(BufferInfo *pBuffer, ISource *pSource, OMX_U32 framenum) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    {
        Mutex::Autolock autoLock(m_pLock);
        /* TODO : give all buffers back to Buffer Manager */
        if (m_bThreadStop) {
            result = m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
            return (OMX_ERRORTYPE)VTEST_ErrorThreadStop;
        }
        if (m_bPortReconfig && (pSource == m_pSink)) {
            result = m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
            return result;
        }
    }

    result = ISource::SetBuffer(pBuffer, pSource);
    if (result != OMX_ErrorNone) {
        return result;
    }

    {
        Mutex::Autolock autoLock(m_pLock);
        if (((pBuffer->pHeaderIn->nFlags & OMX_BUFFERFLAG_EOS)|| (pBuffer->m_Eos3))
                && (pSource == m_pSource)) {
            VTEST_MSG_HIGH("Got input EOS");
            m_bInputEOSReached = OMX_TRUE;
        }
    }

    if (pSource == m_pSource) {  //input port
        // Pull the cmds from dynamic cmd and process them for this ETB
        while((pDecDynamicProperties != NULL) &&
              (m_nFrameNumber + 1 == pDecDynamicProperties[m_nDynamicIndexCount].frame_num) &&
              (m_nDynamicIndexCount + 1 <= (OMX_S32) m_nDynamicConfigArraySize)) {
            switch (pDecDynamicProperties[m_nDynamicIndexCount].config_param) {
                case vtest_Seek:
                    VTEST_MSG_MEDIUM("Seek at frame #%d", (OMX_U32)m_nFrameNumber + 1);
                    if (m_pCRCChecker)
                        m_pCRCChecker->SetCRCCheckFileMode(m_pCRCChecker, TRUE);
                    m_bIsSeek = (OMX_BOOL)TRUE;
                    result = Flush(OMX_ALL);
                    VTEST_MSG_HIGH("Flush ALL returned 0x%x", result);
                    break;
                case vtest_Pause:
                    VTEST_MSG_MEDIUM("Pausing session at frame %d for %d milliseconds",
                        m_nFrameNumber + 1,
                        pDecDynamicProperties[m_nDynamicIndexCount].config_data.pause_duration);
                    usleep(pDecDynamicProperties[m_nDynamicIndexCount].config_data.pause_duration * 1000);
                    break;
            }
            m_nDynamicIndexCount++;
        }

        // If frame number from file source is bigger than the frame number
        // in the dynamic cmd list, discard all the dynamic cmds till the
        // frame number from file source
        while((pDecDynamicProperties != NULL) &&
              (framenum > pDecDynamicProperties[m_nDynamicIndexCount].frame_num) &&
              (m_nDynamicIndexCount + 1 <= (OMX_S32) m_nDynamicConfigArraySize)) {
            VTEST_MSG_MEDIUM("Skip dyn prop for frame#%d",
                (OMX_U32)pDecDynamicProperties[m_nDynamicIndexCount].frame_num);
            m_nDynamicIndexCount++;
        }

        result = OMX_EmptyThisBuffer(m_hDecoder, pBuffer->pHeaderIn);
        FAILED1(result, "ETB error, 0x%lx (%p %p)",
                pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);
        m_nETBcount++;
        m_nFrameNumber = framenum;
        VTEST_MSG_LOW("%s ==> %s: ETB sent, ETB count: %d, Frame num: %d",
                pSource->Name(), Name(),
                m_nETBcount, m_nFrameNumber);
    } else if (pSource == m_pSink) { //output port
        if(pBuffer->pHandle) {
            VTEST_MSG_LOW("%s ==> %s: FTB", pSource->Name(), Name());
            result = OMX_FillThisBuffer(m_hDecoder, pBuffer->pHeaderOut);
            FAILED1(result, "FTB error, 0x%lx (%p %p)",
                pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);
        }
    } else {
        VTEST_MSG_ERROR("Invalid source : %s", pSource->Name());
        result = OMX_ErrorBadParameter;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::AllocateBuffers(BufferInfo **pBuffers,
        OMX_BUFFERHEADERTYPE **ppExtraDataBuffers, OMX_U32 nWidth,
        OMX_U32 nHeight, OMX_U32 nBufferCount, OMX_U32 nBufferSize,
        OMX_U32 nExtraDataBufferCount, OMX_U32 nExtraDataBufferSize,
        OMX_U32 ePortIndex, OMX_U32 nBufferUsage) {
    (void)nWidth, (void)nHeight, (void)nBufferUsage;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_U32 nOriginalBufferCount = -1;
    OMX_U32 nOriginalBufferSize = -1;
    OMX_U32 nOriginalExtraDataBufferCount = -1;
    OMX_U32 nOriginalExtraDataBufferSize = -1;
    OMX_U8 * pExtraDatapBuffer = NULL;

    Mutex::Autolock autoLock(m_pLock);
    VTEST_MSG_HIGH("Alloc %s size %u count %u",
            OMX_PORT_NAME(ePortIndex), (unsigned int)nBufferSize, (unsigned int)nBufferCount);

    if (ePortIndex == PORT_INDEX_IN) {
        nOriginalBufferCount = m_nInputBufferCount;
        nOriginalBufferSize = m_nInputBufferSize;
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber;
    } else if (ePortIndex == PORT_INDEX_OUT) {
        nOriginalBufferCount = m_nOutputBufferCount;
        nOriginalBufferSize = m_nOutputBufferSize;
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber + 1;
    }

    if ((nOriginalBufferCount > nBufferCount) ||
        (nOriginalBufferSize > nBufferSize)) {
        VTEST_MSG_ERROR(
            "Allocate Buffers failure : original count : %u, new count : %u, "
            "original size : %u, new size : %u",
            (unsigned int)nOriginalBufferCount,
            (unsigned int)nBufferCount,
            (unsigned int)nOriginalBufferSize,
            (unsigned int)nBufferSize);
        return OMX_ErrorBadParameter;
    }

    OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
    FAILED1(result, "Decoder: Error - OMX_IndexParamPortDefinition output");

    m_sPortFmt.nBufferCountActual = nBufferCount;
    m_sPortFmt.nBufferSize = nBufferSize;
    OMX_INIT_STRUCT_SIZE(&m_sPortFmt, OMX_PARAM_PORTDEFINITIONTYPE);
    result = OMX_SetParameter(m_hDecoder,
            OMX_IndexParamPortDefinition, (OMX_PTR)&m_sPortFmt);
    FAILED1(result, "Decoder: error set OMX_IndexParamPortDefinition");

    *pBuffers = new BufferInfo[nBufferCount];
    if(*pBuffers==NULL)
    {
        VTEST_MSG_ERROR("BufferInfo creation failed with a buffer count %d", nBufferCount);
        return OMX_ErrorBadParameter;
    }
    memset(*pBuffers, 0, sizeof(BufferInfo) * nBufferCount);
    for (OMX_U32 i = 0; i < nBufferCount; i++) {

        BufferInfo *pBuffer = &((*pBuffers)[i]);
        OMX_BUFFERHEADERTYPE **pHeaderPtr = pBuffer->GetHeaderPtr(ePortIndex);

        result = OMX_AllocateBuffer(m_hDecoder, pHeaderPtr,
                ePortIndex, this, nBufferSize);
        FAILED1(result, "error allocating buffer");

        //fix pHandle, ok as it is not currently used
        pBuffer->pHandle = 0xDEADBEEF;
        pBuffer->pHeaderIn = pBuffer->pHeaderOut = *pHeaderPtr;

        VTEST_MSG_HIGH("%s alloc_ct=%u sz=%u handle=0x%x hdr=(%p %p)",
            OMX_PORT_NAME(ePortIndex), (unsigned int)i + 1, (unsigned int)nBufferSize,
            (unsigned int)pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);
    }

    if (ePortIndex == PORT_INDEX_IN) {
        m_nInputBufferCount = nBufferCount;
        m_nInputBufferSize = nBufferSize;
    } else if (ePortIndex == PORT_INDEX_OUT) {
        m_nOutputBufferCount = nBufferCount;
        m_nOutputBufferSize = nBufferSize;
    }
    if (ePortIndex == PORT_INDEX_OUT && !m_nExtraDataOutputBufferSettoOMX
            && nExtraDataBufferSize && nExtraDataBufferCount) {
        nOriginalExtraDataBufferCount = m_nExtraDataOutputBufferCount;
        nOriginalExtraDataBufferSize = m_nExtraDataOutputBufferSize;
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber +
                                PORT_INDEX_EXTRADATA_OUT;
        if ((nOriginalExtraDataBufferCount > nExtraDataBufferCount) ||
                (nOriginalExtraDataBufferSize > nExtraDataBufferSize)) {
            VTEST_MSG_ERROR(
                "Allocate ExtraData Buffers: original count : %u, new count : %u"
                "original ExtraData Buffer size : %u, new ExtraData Buffer size : %u",
                (unsigned int)nOriginalExtraDataBufferCount, (unsigned int)nExtraDataBufferCount,
                (unsigned int)nOriginalExtraDataBufferSize, (unsigned int)nExtraDataBufferSize);
            return OMX_ErrorBadParameter;
        }
        if(!pExtraDatapBuffer)
            pExtraDatapBuffer = (OMX_U8 *) malloc(nExtraDataBufferSize *
                                                  nExtraDataBufferCount);
        if(!pExtraDatapBuffer) {
            VTEST_MSG_ERROR("Error in allocate ExtraData pBuffers of size %d",
                            nExtraDataBufferSize * nExtraDataBufferCount);
            return OMX_ErrorInsufficientResources;
        }
        for (OMX_U32 i = 0; i < nExtraDataBufferCount; i++) {
            result = OMX_UseBuffer(m_hDecoder, (OMX_BUFFERHEADERTYPE **)ppExtraDataBuffers,
                        PORT_INDEX_EXTRADATA_OUT, this, nExtraDataBufferSize,
                        pExtraDatapBuffer + i * nExtraDataBufferSize);
            FAILED1(result, "error in OMX_UseBuffer for extradata output port");
            VTEST_MSG_MEDIUM("OMX_UseBuffer returned alloc_ct=%u sz=%u"
                             "ExtraData hdr %p buffer ptr=%p",
                             (unsigned int)i + 1,
                             (unsigned int)nExtraDataBufferSize,
                             ppExtraDataBuffers,
                             pExtraDatapBuffer + i * nExtraDataBufferSize);
            ppExtraDataBuffers++;
        }
        m_nExtraDataOutputBufferSettoOMX = OMX_TRUE;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::UseBuffers(BufferInfo **pBuffers,
        OMX_BUFFERHEADERTYPE **ppExtraDataBuffers, OMX_U32 nWidth,
        OMX_U32 nHeight, OMX_U32 nBufferCount, OMX_U32 nBufferSize,
        OMX_U32 nExtraDataBufferCount, OMX_U32 nExtraDataBufferSize,
        OMX_U32 ePortIndex) {
    (void)nWidth, (void)nHeight;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_U32 nOriginalBufferCount = -1;
    OMX_U32 nOriginalBufferSize = -1;
    OMX_U32 nOriginalExtraDataBufferCount = -1;
    OMX_U32 nOriginalExtraDataBufferSize = -1;
    OMX_U32 *pExtraDataBuffers;
    pExtraDataBuffers = (OMX_U32*) *ppExtraDataBuffers;
    pExtraDatapBuffer = NULL;

    Mutex::Autolock autoLock(m_pLock);

    if (ePortIndex == PORT_INDEX_IN) {
        nOriginalBufferCount = m_nInputBufferCount;
        nOriginalBufferSize = m_nInputBufferSize;
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber;
    } else if (ePortIndex == PORT_INDEX_OUT) {
        nOriginalBufferCount = m_nOutputBufferCount;
        nOriginalBufferSize = m_nOutputBufferSize;
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber + 1;
    }

    VTEST_MSG_HIGH("Decoder: %s size %u count %u",
                   OMX_PORT_NAME(ePortIndex), (unsigned int)nBufferSize,
                   (unsigned int)nBufferCount);
    if ((nOriginalBufferCount > nBufferCount)
            || (nOriginalBufferSize > nBufferSize)) {
        VTEST_MSG_ERROR("original count : %u, new count : %u"
                        "original size : %u, new size : %u",
                        (unsigned int)nOriginalBufferCount,
                        (unsigned int)nBufferCount,
                        (unsigned int)nOriginalBufferSize,
                        (unsigned int)nBufferSize);
        return OMX_ErrorBadParameter;
    }

    if (strstr(m_pSink->Name(), "Native") != NULL) {
        VTEST_MSG_LOW("SetNativeWindowEnable");
        result = SetNativeWindowEnable();
        FAILED(result, "Could not enable native window on component");
    }

    OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
    FAILED1(result, "Decoder: Error - OMX_IndexParamPortDefinition output");

    m_sPortFmt.nBufferCountActual = nBufferCount;
    m_sPortFmt.nBufferSize = nBufferSize;
    OMX_INIT_STRUCT_SIZE(&m_sPortFmt, OMX_PARAM_PORTDEFINITIONTYPE);
    result = OMX_SetParameter(m_hDecoder,
            OMX_IndexParamPortDefinition, (OMX_PTR)&m_sPortFmt);
    FAILED1(result, "Decoder: error set OMX_IndexParamPortDefinition");

    if (ePortIndex == PORT_INDEX_OUT && m_ePlaybackMode == DynamicBufferMode) {
        m_pMetaBuffer = (OMX_U8*)malloc(nBufferCount * sizeof(MetaBuffer));
        if (!m_pMetaBuffer) {
            VTEST_MSG_ERROR("Error in allocate MetaBuffer of size %d",
                            nBufferCount * sizeof(MetaBuffer));
            return OMX_ErrorInsufficientResources;
        }
    }

    for (OMX_U32 i = 0; i < nBufferCount; i++) {

        BufferInfo *pBuffer = &((*pBuffers)[i]);
        OMX_BUFFERHEADERTYPE **pHeaderPtr = pBuffer->GetHeaderPtr(ePortIndex);
        if (ePortIndex == PORT_INDEX_OUT && m_ePlaybackMode == DynamicBufferMode) {
                MetaBuffer *meta = (MetaBuffer *)(m_pMetaBuffer + i * sizeof(MetaBuffer));
                meta->meta_handle = (NativeHandle *) pBuffer->pHandle;
                result = OMX_UseBuffer(m_hDecoder, pHeaderPtr,
                        ePortIndex, this, nBufferSize, (OMX_U8 *) meta);
        } else {
                result = OMX_UseBuffer(m_hDecoder, pHeaderPtr,
                        ePortIndex, this, nBufferSize, (OMX_U8 *)pBuffer->pHandle);
        }
        FAILED(result, "error in use buffer");

        // for NativeWindow case, supply all headers
        pBuffer->pHeaderIn = (pBuffer->pHeaderIn == NULL)
                                ? *pHeaderPtr : pBuffer->pHeaderIn;
        pBuffer->pHeaderOut = (pBuffer->pHeaderOut == NULL)
                                ? *pHeaderPtr : pBuffer->pHeaderOut;

        VTEST_MSG_HIGH("%s use_ct=%u sz=%u handle=0x%x hdr=(%p %p)",
                OMX_PORT_NAME(ePortIndex), (unsigned int)i + 1, (unsigned int)nBufferSize,
            (unsigned int)pBuffer->pHandle, pBuffer->pHeaderIn, pBuffer->pHeaderOut);
    }
    if (ePortIndex == PORT_INDEX_IN) {
        m_nInputBufferCount = nBufferCount;
        m_nInputBufferSize = nBufferSize;
    } else if (ePortIndex == PORT_INDEX_OUT) {
        m_nOutputBufferCount = nBufferCount;
        m_nOutputBufferSize = nBufferSize;
    }
    if (ePortIndex == PORT_INDEX_OUT && !m_nExtraDataOutputBufferSettoOMX
            && nExtraDataBufferSize && nExtraDataBufferCount) {
        nOriginalExtraDataBufferCount = m_nExtraDataOutputBufferCount;
        nOriginalExtraDataBufferSize = m_nExtraDataOutputBufferSize;
        m_sPortFmt.nPortIndex = m_sPortParam.nStartPortNumber +
                                PORT_INDEX_EXTRADATA_OUT;
        if ((nOriginalExtraDataBufferCount > nExtraDataBufferCount) ||
                (nOriginalExtraDataBufferSize > nExtraDataBufferSize)) {
            VTEST_MSG_ERROR(
                "Allocate ExtraData Buffers: original count : %u, new count : %u"
                "original ExtraData Buffer size : %u, new ExtraData Buffer size : %u",
                (unsigned int)nOriginalExtraDataBufferCount, (unsigned int)nExtraDataBufferCount,
                (unsigned int)nOriginalExtraDataBufferSize, (unsigned int)nExtraDataBufferSize);
            return OMX_ErrorBadParameter;
        }
        if(!pExtraDatapBuffer)
            pExtraDatapBuffer = (OMX_U8 *) malloc(nExtraDataBufferSize *
                                                  nExtraDataBufferCount);
        if(!pExtraDatapBuffer) {
            VTEST_MSG_ERROR("Error in allocate ExtraData pBuffers of size %d",
                            nExtraDataBufferSize * nExtraDataBufferCount);
            return OMX_ErrorInsufficientResources;
        }
        for (OMX_U32 i = 0; i < nExtraDataBufferCount; i++) {
            result = OMX_UseBuffer(m_hDecoder, (OMX_BUFFERHEADERTYPE **)ppExtraDataBuffers,
                        PORT_INDEX_EXTRADATA_OUT, this, nExtraDataBufferSize,
                        pExtraDatapBuffer + i * nExtraDataBufferSize);
            FAILED1(result, "error in OMX_UseBuffer for extradata output port");
            VTEST_MSG_MEDIUM("OMX_UseBuffer returned alloc_ct=%u sz=%u"
                             "ExtraData hdr %p buffer ptr=%p",
                             (unsigned int)i + 1,
                             (unsigned int)nExtraDataBufferSize,
                             ppExtraDataBuffers,
                             pExtraDatapBuffer + i * nExtraDataBufferSize);
            ppExtraDataBuffers++;
        }
        m_nExtraDataOutputBufferSettoOMX = OMX_TRUE;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::ThreadRun(OMX_PTR pThreadData) {

    (void)pThreadData;
    OMX_ERRORTYPE result = OMX_ErrorNone;

    while (m_bThreadStop == OMX_FALSE) {
        if (m_bPortReconfig) {
            result = PortReconfigOutput();
            FAILED2(result, SetError(), "PortReconfigOutput failed");
        }
        Sleeper::Sleep(10);
    }

    VTEST_MSG_HIGH("thread exiting...");
    return result;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::FreeBuffer(
        BufferInfo *pBuffer, OMX_U32 ePortIndex) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    OMX_BUFFERHEADERTYPE **pHeaderPtr = pBuffer->GetHeaderPtr(ePortIndex);
    if (*pHeaderPtr == NULL) {
        return result;
    }

    VTEST_MSG_MEDIUM("Freeing buffer %p 0x%lu",
            *pHeaderPtr, pBuffer->pHandle);
    result = OMX_FreeBuffer(m_hDecoder, ePortIndex, *pHeaderPtr);
    FAILED1(result, "Error freeing %s", OMX_PORT_NAME(ePortIndex));

    *pHeaderPtr = NULL;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::FreeExtraDataBuffer(
        OMX_BUFFERHEADERTYPE **pExtraDataBuffer, OMX_U32 ePortIndex) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    if (*pExtraDataBuffer == NULL) {
        return result;
    }
    if(!m_bPortReconfig) {
        VTEST_MSG_MEDIUM("Freeing buffer %p", *pExtraDataBuffer);
        result = OMX_FreeBuffer(m_hDecoder, ePortIndex, *pExtraDataBuffer);
        FAILED1(result, "Error freeing %s", OMX_PORT_NAME(ePortIndex));
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::SetState(
        OMX_STATETYPE eState, OMX_BOOL bSynchronous) {

    OMX_ERRORTYPE result = OMX_ErrorNone;

    /*if we get an error before idle complete (allocation failure) and
     * we try to move to loaded in the destructor*/
    if (eState == m_eState) {
        return result;
    }

    // check for pending state transition
    if (m_eState != m_eStatePending) {
        result = WaitState(m_eStatePending);
        FAILED1(result, "wait for %s failed, waiting for alloc/dealloc?!?!",
                OMX_STATE_NAME(m_eStatePending));
    }

    // check for invalid transition
    if ((eState == OMX_StateLoaded && m_eState != OMX_StateIdle) ||
        (eState == OMX_StateExecuting && m_eState != OMX_StateIdle)) {
        VTEST_MSG_ERROR(
            "Error: invalid state tranisition: state %s to %s",
            OMX_STATE_NAME(eState), OMX_STATE_NAME(m_eState));
        result = OMX_ErrorIncorrectStateTransition;
    }

    VTEST_MSG_MEDIUM("goto state %s...", OMX_STATE_NAME(eState));
    result = OMX_SendCommand(
            m_hDecoder, OMX_CommandStateSet, (OMX_U32)eState, NULL);
    m_eStatePending = eState;

    FAILED1(result, "failed to set state");
    if (bSynchronous == OMX_TRUE) {
        result = WaitState(eState);
        FAILED1(result, "failed to wait in set state in synchronous mode");
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::WaitState(OMX_STATETYPE eState) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    CmdType cmd;

    VTEST_MSG_MEDIUM("waiting for state %s...", OMX_STATE_NAME(eState));
    m_pSignalQueue->Pop(&cmd, sizeof(cmd), 0); // infinite wait
    result = cmd.eResult;

    if (cmd.eEvent == OMX_EventCmdComplete) {

        if (cmd.eCmd == OMX_CommandStateSet) {

            if ((OMX_STATETYPE)cmd.sCmdData == eState) {
                m_eState = (OMX_STATETYPE)cmd.sCmdData;
            } else {
                VTEST_MSG_ERROR("wrong state (%d)", (int)cmd.sCmdData);
                result = OMX_ErrorUndefined;
            }

        } else {
            VTEST_MSG_ERROR("expecting state change");
            result = OMX_ErrorUndefined;
        }

    } else {
        VTEST_MSG_ERROR("expecting cmd complete");
        result = OMX_ErrorUndefined;
    }

    if (result == OMX_ErrorNone) {
        VTEST_MSG_MEDIUM("reached state %s...", OMX_STATE_NAME(eState));
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::HandleOutputPortSettingsChange(OMX_U32 nData2) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_U32 nOutPortIndex = 1;
    OMX_CONFIG_RECTTYPE rect;
    OMX_INIT_STRUCT(&rect, OMX_CONFIG_RECTTYPE);

    if (nData2 == OMX_IndexConfigCommonOutputCrop || nData2 == OMX_IndexConfigCommonScale) {
        rect.nPortIndex = nOutPortIndex;
        result = OMX_GetConfig(m_hDecoder, OMX_IndexConfigCommonOutputCrop, &rect);
        FAILED1(result, "Failed to get crop rectangle");
        VTEST_MSG_HIGH("Got Crop Rect: (%d, %d) (%u x %u)",
                       (int)rect.nLeft, (int)rect.nTop,
                       (unsigned int)rect.nWidth, (unsigned int)rect.nHeight);
        result = ValidatePortSettingChange(CropSettingChange);
        FAILED1(result, "Validate Crop Setting failed");

        result = m_pSink->PortReconfig(
                PORT_INDEX_IN, m_nFrameWidth, m_nFrameHeight, rect);
        FAILED1(result, "GotPortReconfig failed on sink");
    } else if (nData2 == 0 || nData2 == OMX_IndexParamPortDefinition) {
        // check all buffers are with us (in Allocate buffer case)
        assert(m_eState != OMX_StateExecuting);
        result = ValidatePortSettingChange(PortDefinitionChange);
        FAILED1(result, "Validate Port Setting failed");

        VTEST_MSG_MEDIUM("Reconfiguring output port");
        m_bPortReconfig = OMX_TRUE;
    }
    if((m_nColorPrimaries == 1) && (nData2 == OMX_QTIIndexConfigDescribeColorAspects)) {
        ProcessColorAspectsinReconfig();
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::ValidatePortSettingChange(ReconfigType eType) {
    OMX_ERRORTYPE result = OMX_ErrorNone;
    CropSettingChangeValidation *nodeCropReconfigEntry = NULL;
    PortDefinitionChangeValidation *nodePortReconfigEntry = NULL;

    OMX_CONFIG_RECTTYPE tempRect;
    OMX_INIT_STRUCT(&tempRect, OMX_CONFIG_RECTTYPE);
    m_sPortFmt.nPortIndex = 1;

    uint32 ExpectedCropWidth, ExpectedCropHeight, ExpectedCropX, ExpectedCropY;
    uint32 ExpectedWidth, ExpectedHeight;
    uint32 m_nPrevCropX = 0, m_nPrevCropY = 0;

    if (!list_is_empty(&m_pCropSettingChangeValidationList) &&
        !list_is_empty(&m_pPortDefinitionChangeValidationList))
    {
        if(eType == CropSettingChange) {
            // validate crop Setting change event.
            // Query for expectd crop rectagle
            result = OMX_GetConfig(m_hDecoder, OMX_IndexConfigCommonOutputCrop, &tempRect);
            FAILED1(result, "Failed to get crop rectangle");
            ExpectedCropWidth = (unsigned int)tempRect.nWidth;
            ExpectedCropHeight = (unsigned int)tempRect.nHeight;
            ExpectedCropX = (unsigned int)tempRect.nLeft;
            ExpectedCropY = (unsigned int)tempRect.nTop;
            if(m_nPrevCropWidth == ExpectedCropWidth &&  m_nPrevCropHeight == ExpectedCropHeight &&
               m_nPrevCropX == ExpectedCropX && m_nPrevCropY == ExpectedCropY)
            {
                // Expected crop rectangle is same as prev crop rectangle
                return OMX_ErrorBadParameter;
            }
            m_nPrevCropWidth = ExpectedCropWidth;
            m_nPrevCropHeight = ExpectedCropHeight;
            m_nPrevCropX = ExpectedCropX;
            m_nPrevCropY = ExpectedCropY;
            // Search for frameNum, cropWidth, cropHeight, cropX, cropY
            // in CropSettingChangeValidation list
            utils_list_for_each_entry(struct CropSettingChangeValidation, nodeCropReconfigEntry,
            &m_pCropSettingChangeValidationList, list)
            {
                if ( !m_nFrameDoneCount ||
                (m_nFrameDoneCount == nodeCropReconfigEntry->frameNum &&
                 ExpectedCropWidth == nodeCropReconfigEntry->cropWidth &&
                 ExpectedCropHeight == nodeCropReconfigEntry->cropHeight &&
                 ExpectedCropX == nodeCropReconfigEntry->cropX &&
                 ExpectedCropY == nodeCropReconfigEntry->cropY))
                {
                    return result;
                }
            }

        }
        else {
            // validate PortSettingChnage Event.
            // Query for Expected Port params..
            OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
            ExpectedWidth =  m_sPortFmt.format.video.nFrameWidth;
            ExpectedHeight =  m_sPortFmt.format.video.nFrameHeight;
            if(m_nFrameWidth == ExpectedWidth && m_nFrameHeight == ExpectedHeight)
            {
                // expected width height is same as Prev width height
                return OMX_ErrorBadParameter;
            }
            // Search for frameNum, width and height in  PortDefinitionChangeValidation list
            utils_list_for_each_entry(struct PortDefinitionChangeValidation, nodePortReconfigEntry,
            &m_pPortDefinitionChangeValidationList, list)
            {
                if (!m_nFrameDoneCount ||
                (m_nFrameDoneCount == nodePortReconfigEntry->frameNum &&
                (ExpectedWidth == nodePortReconfigEntry->width)&&
                (ExpectedHeight == nodePortReconfigEntry->height)))
                {
                    return result;
                }
            }
        }
    }
    else
    {
        // we will be here if the MISR file is not prsent
        VTEST_MSG_MEDIUM(" MisrFile is not present ");
        return OMX_ErrorNone;
    }
    return OMX_ErrorBadParameter;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::PortReconfigOutput() {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_U32 nOutPortIndex = 1;
    OMX_CONFIG_RECTTYPE rect;
    CmdType cmd;

    VTEST_MSG_MEDIUM("PortReconfig Calling Flush on output port");
    Flush(PORT_INDEX_OUT);

    VTEST_MSG_MEDIUM("PortReconfig OMX_CommandPortDisable");
    result = OMX_SendCommand(m_hDecoder, OMX_CommandPortDisable, PORT_INDEX_OUT, 0);
    FAILED1(result, "Disable Output Port failed");

    // Wait for OMX_comp/sink to return all buffers
    BufferInfo *pBuffer = NULL;
    OMX_U32 nFreeBufferCount = 0;
    while (m_pFreeBufferQueue->GetSize() > 0) {

        result = m_pFreeBufferQueue->Pop(&pBuffer, sizeof(pBuffer), 0);
        if ((pBuffer == NULL) || (result != OMX_ErrorNone)) {
            VTEST_MSG_ERROR("Error in getting buffer, it might be null");
        } else {
            VTEST_MSG_MEDIUM("PortReconfig free buffer count %u",
                    (unsigned int)nFreeBufferCount);
        }
        nFreeBufferCount++;
    }

    // Free all old buffers
    VTEST_MSG_MEDIUM("PortReconfig free OUTPUT buffers");
    m_pBufferManager->FreeBuffers(this, PORT_INDEX_OUT);

    if (m_pMetaBuffer) {
        free(m_pMetaBuffer);
        m_pMetaBuffer = NULL;
    }

    // wait for OMX_comp to respond OMX_CommandPortDisable
    // this only happens once all buffers are freed
    VTEST_MSG_MEDIUM("PortReconfig getting response");
    if (m_pSignalQueue->Pop(&cmd, sizeof(cmd), 0) != OMX_ErrorNone) {
        FAILED1(result, "Error popping signal");
    }
    if (cmd.eResult != OMX_ErrorNone || cmd.eEvent != OMX_EventCmdComplete ||
        cmd.eCmd != OMX_CommandPortDisable) {
        result = OMX_ErrorUndefined;
        FAILED1(result, "expecting complete for command : %d", OMX_CommandPortDisable);
    }

    //re-enable port before allocating buffers, otherwise, OMX_GetParameter cannot get right buffersize
    VTEST_MSG_MEDIUM("Re-enable out port");
    result = OMX_SendCommand(m_hDecoder, OMX_CommandPortEnable, PORT_INDEX_OUT, 0);
    FAILED1(result, "Enable Output Port failed");

    // ask OMX_comp for new settings
    VTEST_MSG_MEDIUM("Get new settings on out port");
    m_sPortFmt.nPortIndex = nOutPortIndex;
    OMX_GetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);
    if (m_sPortFmt.eDir != OMX_DirOutput) {
        VTEST_MSG_ERROR("Error - Expect Output Port\n");
        return OMX_ErrorUndefined;
    }

    // If configured color format is different from OMX returned color format
    // AND if color format in child xml is UBWC 8bit
    // AND if color format returned by OMX component is UBWC 10bit,
    // then do a set param with dither control set to DITHER EXCEPT BT2020.
    // Also, we should set the color format as UBWC 8bit to OMX component.
    if ((m_nConfiguredColorFormat != m_sPortFmt.format.video.eColorFormat) &&
        (m_nXMLColorFormat == QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed) &&
        (m_sPortFmt.format.video.eColorFormat ==
         (OMX_COLOR_FORMATTYPE) QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m10bitCompressed)) {
        VTEST_MSG_HIGH("Color formats: XML:0x%x Configured:0x%x OMX returned:0x%x\n",
        m_nXMLColorFormat, m_nConfiguredColorFormat, m_sPortFmt.format.video.eColorFormat);
        QOMX_VIDEO_DITHER_CONTROL DitherCtrl;
        OMX_INIT_STRUCT_SIZE(&DitherCtrl, OMX_PARAM_PORTDEFINITIONTYPE);

        DitherCtrl.nPortIndex = nOutPortIndex;
        OMX_GetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QTIIndexParamDitherControl,
                         &DitherCtrl);
        DitherCtrl.eDitherType = QOMX_DITHER_COLORSPACE_EXCEPT_BT2020;
        OMX_SetParameter(m_hDecoder, (OMX_INDEXTYPE)OMX_QTIIndexParamDitherControl,
                         &DitherCtrl);
        VTEST_MSG_MEDIUM("Dither control set to %d", DitherCtrl.eDitherType);

        m_sPortFmt.format.video.eColorFormat =
            (OMX_COLOR_FORMATTYPE) QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed;
        m_nConfiguredColorFormat = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32mCompressed;
    }

    m_nFrameHeight = m_sPortFmt.format.video.nFrameHeight;
    m_nFrameWidth = m_sPortFmt.format.video.nFrameWidth;
    m_nOutputBufferCount = m_sPortFmt.nBufferCountActual;

    /* Update original count here as set by decoder again
     * otherwise we will always allocate the number of
     * buffers set before reconfig. (As read by BufferManager
     * via GetBufferRequirements) */
    m_nOriginalOutputBufferCount = m_nOutputBufferCount;

    OMX_SetParameter(m_hDecoder, OMX_IndexParamPortDefinition, &m_sPortFmt);

    m_nOutputBufferSize = m_sPortFmt.nBufferSize;
    rect.nLeft = 0;
    rect.nTop = 0;
    rect.nWidth = m_nFrameWidth;
    rect.nHeight = m_nFrameHeight;
    VTEST_MSG_MEDIUM("PortReconfig Min Buffer Count = %u", (unsigned int)m_nOutputBufferCount);
    VTEST_MSG_MEDIUM("PortReconfig Buffer Size = %u", (unsigned int)m_nOutputBufferSize);
    VTEST_MSG_MEDIUM("PortReconfig width : %u, height : %u", (unsigned int)m_nFrameWidth, (unsigned int)m_nFrameHeight);

    // notify sink of PortReconfig
    VTEST_MSG_MEDIUM("Notify sink about PortReconfig");
    result = m_pSink->PortReconfig(
            PORT_INDEX_IN, m_nFrameWidth, m_nFrameHeight, rect);
    FAILED1(result, "PortReconfig done failed on sink");

    // re-allocate all buffers on out port
    VTEST_MSG_MEDIUM("PortReconfig re-allocate OUTPUT buffers");
    result = m_pBufferManager->SetupBufferPool(this, m_pSink);
    FAILED1(result, "Error in realloacting buffer pool");

    // wait for OMX_comp to respond OMX_CommandPortEnabled
    // this only happens once all buffers are allocated
    memset(&cmd, 0, sizeof(cmd));
    VTEST_MSG_MEDIUM("Wait for 1000msec for PortEnable complete event");
    if (m_pSignalQueue->Pop(&cmd, sizeof(cmd), 1000) != OMX_ErrorNone) {
        FAILED1(result, "Error popping PortEnable complete event");
    }
    if (cmd.eResult != OMX_ErrorNone || cmd.eEvent != OMX_EventCmdComplete ||
        cmd.eCmd != OMX_CommandPortEnable) {
        result = OMX_ErrorUndefined;
        FAILED1(result, "expecting complete for command : %d", OMX_CommandPortEnable);
    }

    m_bPortReconfig = OMX_FALSE;

    // enqueue newly allocated buffers
    OMX_U32 nBufferCount;
    BufferInfo *pBuffers;
    result = m_pBufferManager->GetBuffers(this, PORT_INDEX_OUT, &pBuffers, &nBufferCount);
    FAILED1(result, "Error in Getting Buffers");

    for (OMX_U32 i = 0; i < nBufferCount; i++) {
        VTEST_MSG_MEDIUM("port-reconfig requeue buffer %lu (%p %p)",
                pBuffers[i].pHandle,
                pBuffers[i].pHeaderIn, pBuffers[i].pHeaderOut);
        result = SetBuffer(&pBuffers[i], m_pSink);
        FAILED1(result, "Error setting port buffer %p", pBuffers[i].pHeaderOut);
    }

    VTEST_MSG_MEDIUM("port-reconfig done");

    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::SetNativeWindowEnable() {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_INDEXTYPE index;
    OMX_VERSIONTYPE ver;

    ver.s.nVersionMajor = 1;
    ver.s.nVersionMinor = 0;
    ver.s.nRevision = 0;
    ver.s.nStep = 0;

    result  = OMX_GetExtensionIndex(m_hDecoder,
        const_cast<OMX_STRING>(
            "OMX.google.android.index.enableAndroidNativeBuffers"), &index);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("OMX_GetExtensionIndex failed");
        return result;
    }

    OMX_U32 nOutPortIndex = 1;
    EnableAndroidNativeBuffersParams params = {
        sizeof(EnableAndroidNativeBuffersParams), ver, nOutPortIndex, OMX_TRUE,
    };

    result = OMX_SetParameter(m_hDecoder, index, &params);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("OMX_EnableAndroidNativeBuffers failed");
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::GetGraphicBufferUsage(OMX_U32 *nBufferUsage) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_INDEXTYPE index;
    OMX_VERSIONTYPE ver;

    *nBufferUsage = 0;
    ver.s.nVersionMajor = 1;
    ver.s.nVersionMinor = 0;
    ver.s.nRevision = 0;
    ver.s.nStep = 0;

    //Get graphic buffer usage flags from component
    result  = OMX_GetExtensionIndex(
        m_hDecoder,
        const_cast<OMX_STRING>(
            "OMX.google.android.index.getAndroidNativeBufferUsage"),
        &index);
    FAILED1(result, "OMX_GetExtensionIndex failed for getAndroidNativeBufferUsage");

    OMX_U32 nOutPortIndex = 1;

    GetAndroidNativeBufferUsageParams params = {
        sizeof(GetAndroidNativeBufferUsageParams), ver, nOutPortIndex, 0,
    };

    result = OMX_GetParameter(m_hDecoder, index, &params);
    FAILED1(result, "OMX_GetAndroidNativeBufferUsage failed with error %d", result);

    *nBufferUsage = params.nUsage;
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::EventCallback(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData, OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData) {

    (void)hComponent; (void)pEventData;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    Decoder *pThis = (Decoder *)pAppData;

    {
        Mutex::Autolock autoLock(pThis->m_pLock);

        if (eEvent == OMX_EventCmdComplete) {

            if ((OMX_COMMANDTYPE)nData1 == OMX_CommandStateSet) {
                VTEST_MSG_HIGH("Event callback: state is %s",
                        OMX_STATE_NAME((OMX_STATETYPE)nData2));

                CmdType cmd;
                cmd.eEvent = OMX_EventCmdComplete;
                cmd.eCmd = OMX_CommandStateSet;
                cmd.sCmdData = nData2;
                cmd.eResult = result;
                result = pThis->m_pSignalQueue->Push(&cmd, sizeof(cmd));
                FAILED1(result, "push to signal queue failed");

            } else if ((OMX_COMMANDTYPE)nData1 == OMX_CommandFlush) {
                VTEST_MSG_MEDIUM("Event callback: flush complete on port : %s",
                        OMX_PORT_NAME(nData2));

                CmdType cmd;
                cmd.eEvent = OMX_EventCmdComplete;
                cmd.eCmd = OMX_CommandFlush;
                cmd.sCmdData = 0;
                cmd.eResult = result;
                result = pThis->m_pSignalQueue->Push(&cmd, sizeof(cmd));
                FAILED1(result, "push to signal queue failed");

            } else if ((OMX_COMMANDTYPE)nData1 == OMX_CommandPortDisable) {
                VTEST_MSG_MEDIUM("Event callback: %s port disable",
                        OMX_PORT_NAME(nData2));

                /* Only queue port disable during port reconfig,
                 * during stop, it clashes with the event for moving
                 * to loaded state */
                if (pThis->m_bPortReconfig) {
                    CmdType cmd;
                    cmd.eEvent = OMX_EventCmdComplete;
                    cmd.eCmd = OMX_CommandPortDisable;
                    cmd.sCmdData = 0;
                    cmd.eResult = result;
                    result = pThis->m_pSignalQueue->Push(&cmd, sizeof(cmd));
                    FAILED1(result, "push to signal queue failed");
                }

            } else if ((OMX_COMMANDTYPE)nData1 == OMX_CommandPortEnable) {
                VTEST_MSG_MEDIUM("Event callback: %s port enable",
                        OMX_PORT_NAME(nData2));

                CmdType cmd;
                cmd.eEvent = OMX_EventCmdComplete;
                cmd.eCmd = OMX_CommandPortEnable;
                cmd.sCmdData = 0;
                cmd.eResult = result;
                result = pThis->m_pSignalQueue->Push(&cmd, sizeof(cmd));
                FAILED1(result, "push to signal queue failed");

            } else {
                result = OMX_ErrorUndefined;
                VTEST_MSG_ERROR("Unimplemented command");
            }
        } else if (eEvent == OMX_EventError) {
            /* Already in error */
            if (pThis->m_bThreadStop) {
                return result;
            }
            result = OMX_ErrorUndefined;
            VTEST_MSG_ERROR(
                    "Event callback: async error nData1 %u, nData2 %u",
                    (unsigned int)nData1, (unsigned int)nData2);
        } else if (eEvent == OMX_EventBufferFlag) {
            VTEST_MSG_MEDIUM("got buffer flag event");
        } else if (eEvent ==  OMX_EventPortSettingsChanged) {
            VTEST_MSG_MEDIUM("OMX_EventPortSettingsChanged port[%u]\n", (unsigned int)nData1);

            OMX_U32 nOutPortIndex = 1;
            if (nData1 == nOutPortIndex) {
                result = pThis->HandleOutputPortSettingsChange(nData2);
            } else {
                VTEST_MSG_ERROR("reconfig not supported on port : %u", (unsigned int)nData1);
            }
        } else {
            result = OMX_ErrorUndefined;
            VTEST_MSG_ERROR("Unimplemented event");
        }
    }

    FAILED2(result, pThis->SetError(), "Error out");
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::EmptyDoneCallback(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE *pHeader) {

    (void)hComponent;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    Decoder *pThis = (Decoder *)pAppData;
    BufferInfo *pBuffer = NULL;
    int bufferindex = -1;
    BufferPool * pBufferPool1 = NULL;

    VTEST_MSG_MEDIUM("EBD %p", pHeader);
    bufferindex = pThis->m_pBufferManager->GetBuffer(
            pThis, PORT_INDEX_IN, pHeader, &pBuffer, &pBufferPool1);
    if(bufferindex == -1)
        VTEST_MSG_MEDIUM("Error in finding buffer %p", pBuffer);

    Mutex::Autolock autoLock(pThis->m_pLock);

    /* TODO : give all buffers back to Buffer Manager */
    if (pBuffer->m_Eos3) {
        pThis->m_bThreadStop = OMX_TRUE;
        pThis->m_EOS3 = OMX_TRUE;
    }
    if (pThis->m_bThreadStop || pThis->m_bInputEOSReached) {
        result = pThis->m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
        return result;
    }
    result = pThis->m_pSource->SetBuffer(pBuffer, pThis);
    if (result != OMX_ErrorNone) {
        /* Don't treat it as fatal, because there is possibility where the
         * eos hasn't reached us and source is not expecting any more buffers */
        VTEST_MSG_HIGH("SetBuffer on source failed pBuffer: %p", pBuffer);
        pThis->m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::FillDoneCallback(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE *pHeader) {

    (void)hComponent;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    Decoder *pThis = (Decoder *)pAppData;
    BufferInfo *pBuffer = NULL;
    int bufferindex = -1;
    BufferPool * pBufferPool1 = NULL;

    if (!pHeader) {
        return OMX_ErrorBadParameter;
    }

    bufferindex = pThis->m_pBufferManager->GetBuffer(
        pThis, PORT_INDEX_OUT, pHeader, &pBuffer, &pBufferPool1);
    if(bufferindex == -1)
        VTEST_MSG_ERROR("Error in finding buffer %p", pBuffer);
    VTEST_MSG_MEDIUM("GetBuffer returned an index %d and buffer pool %p",
                     bufferindex, pBufferPool1);

    /* update FBD count */
    if(pHeader->nFilledLen)
    {
       pThis->m_nFrameDoneCount++;
    }

    if (pHeader->nFlags & OMX_BUFFERFLAG_EXTRADATA) {
        result = pThis->ParseExtraData(pBufferPool1, pHeader, bufferindex);
        FAILED2(result, pThis->SetError(),
                "Error in processing extra data buffer %p", pHeader->pBuffer);
    }
    if(pHeader->nFilledLen && (pThis->m_nColorPrimaries == 0) &&
       (pThis->m_fVUIColorAspects || pThis->m_fVUIColorAspectsRef)) {
        VTEST_MSG_MEDIUM("Process Color Aspects");
        result = pThis->ProcessColorAspects();
    }

    Mutex::Autolock autoLock(pThis->m_pLock);

    /* TODO : give all buffers back to Buffer Manager */
    if (pThis->m_bPortReconfig || pThis->m_bThreadStop) {
        result = pThis->m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
        return result;
    }

    result = pThis->m_pSink->SetBuffer(pBuffer, pThis);
    if (pHeader->nFlags & OMX_BUFFERFLAG_EOS) {
        VTEST_MSG_MEDIUM("Decoder: Got output EOS\n");
        pThis->m_bThreadStop = OMX_TRUE;
    }
    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("SetBuffer on sink failed pBuffer: %p", pBuffer);
        pThis->m_bThreadStop = OMX_TRUE;
        pThis->m_pFreeBufferQueue->Push(&pBuffer, sizeof(BufferInfo **));
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::GetComponentRole(
        OMX_BOOL bSecureSession, FileType *eFileType, OMX_STRING *role) {

    if (*eFileType >= FILE_TYPE_COMMON_CODEC_MAX) {
        OMX_U32 specificFileType = 0;
        switch ((int)m_eCodec) {
            case OMX_VIDEO_CodingAVC:
                specificFileType = FILE_TYPE_START_OF_H264_SPECIFIC;
                break;
            case QOMX_VIDEO_CodingDivx:
                specificFileType = FILE_TYPE_START_OF_DIVX_SPECIFIC;
                break;
            case OMX_VIDEO_CodingMPEG4:
            case OMX_VIDEO_CodingH263:
                specificFileType = FILE_TYPE_START_OF_MP4_SPECIFIC;
                break;
            case OMX_VIDEO_CodingWMV:
                specificFileType = FILE_TYPE_START_OF_VC1_SPECIFIC;
                break;
            case OMX_VIDEO_CodingMPEG2:
                specificFileType = FILE_TYPE_START_OF_MPEG2_SPECIFIC;
                break;
            case OMX_VIDEO_CodingVP8:
            case OMX_VIDEO_CodingVP9:
                specificFileType = FILE_TYPE_START_OF_VP8_SPECIFIC;
                break;
            default:
                VTEST_MSG_ERROR("Error: Unknown code %d", m_eCodec);
                return OMX_ErrorBadParameter;
        }
        *eFileType = (FileType)
            (specificFileType +
             *eFileType - FILE_TYPE_COMMON_CODEC_MAX);
    }

    switch (*eFileType) {
        case FILE_TYPE_DAT_PER_AU:
        case FILE_TYPE_ARBITRARY_BYTES:
        case FILE_TYPE_264_START_CODE:
        case FILE_TYPE_264_NAL_SIZE_LENGTH:
        case FILE_TYPE_PICTURE_START_CODE:
        case FILE_TYPE_MPEG2_START_CODE:
        case FILE_TYPE_RCV:
        case FILE_TYPE_VC1:
        case FILE_TYPE_DIVX_4_5_6:
        case FILE_TYPE_DIVX_311:
        case FILE_TYPE_VP8:
            break;
        default:
            VTEST_MSG_ERROR("Error: Unknown file type %d", *eFileType);
            return OMX_ErrorBadParameter;
    }

    switch ((int)m_eCodec) {
        case OMX_VIDEO_CodingAVC:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.avc";
            break;
#ifdef SW_VDEC
        case OMX_VIDEO_CodingMPEG4:
            *role = (OMX_STRING)"OMX.qti.video.decoder.mpeg4sw";
            break;
        case OMX_VIDEO_CodingH263:
            *role = (OMX_STRING)"OMX.qti.video.decoder.h263sw";
            break;
        case OMX_VIDEO_CodingWMV:
            if (*eFileType == FILE_TYPE_RCV) {
                *role = (OMX_STRING)"OMX.qti.video.decoder.wmvsw";
            } else {
                *role = (OMX_STRING)"OMX.qti.video.decoder.vc1sw";
            }
            break;
        case QOMX_VIDEO_CodingDivx:
            if (*eFileType == FILE_TYPE_DIVX_311) {
                *role = (OMX_STRING)"OMX.qti.video.decoder.divxsw";
            } else if (*eFileType == FILE_TYPE_DIVX_4_5_6) {
                *role = (OMX_STRING)"OMX.qti.video.decoder.divx4sw";
            } else {
                VTEST_MSG_ERROR("Error: Unsupported file type : %d for codec format : %d ",
                                m_eCodec, *eFileType);
                return OMX_ErrorBadParameter;
            }
            break;
#else
        case OMX_VIDEO_CodingMPEG4:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.mpeg4";
            break;
        case OMX_VIDEO_CodingH263:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.h263";
            break;
        case OMX_VIDEO_CodingWMV:
            if (*eFileType == FILE_TYPE_RCV) {
                *role = (OMX_STRING)"OMX.qcom.video.decoder.wmv";
            } else {
                *role = (OMX_STRING)"OMX.qcom.video.decoder.vc1";
            }
            break;
        case QOMX_VIDEO_CodingDivx:
            if (*eFileType == FILE_TYPE_DIVX_311) {
                *role = (OMX_STRING)"OMX.qcom.video.decoder.divx311";
            } else if (*eFileType == FILE_TYPE_DIVX_4_5_6) {
                *role = (OMX_STRING)"OMX.qcom.video.decoder.divx";
            } else {
                VTEST_MSG_ERROR("Error: Unsupported file type : %d for codec format : %d ",
                                m_eCodec, *eFileType);
                return OMX_ErrorBadParameter;
            }
            break;
#endif
        case OMX_VIDEO_CodingMPEG2:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.mpeg2";
            break;
        case QOMX_VIDEO_CodingHevc:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.hevc";
            break;
        case OMX_VIDEO_CodingVP8:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.vp8";
            break;
        case OMX_VIDEO_CodingVP9:
            *role = (OMX_STRING)"OMX.qcom.video.decoder.vp9";
            break;
        default:
            VTEST_MSG_ERROR("Error: Unsupported codec %d", m_eCodec);
            return OMX_ErrorBadParameter;
    }

    if (bSecureSession) {
        switch ((int)m_eCodec) {
            case OMX_VIDEO_CodingAVC:
                *role = (OMX_STRING)"OMX.qcom.video.decoder.avc.secure";
                break;
            case OMX_VIDEO_CodingMPEG2:
                *role = (OMX_STRING)"OMX.qcom.video.decoder.mpeg2.secure";
                break;
            case QOMX_VIDEO_CodingHevc:
                *role = (OMX_STRING)"OMX.qcom.video.decoder.hevc.secure";
                break;
            case OMX_VIDEO_CodingWMV:
                if (*eFileType == FILE_TYPE_RCV) {
                    *role = (OMX_STRING)"OMX.qcom.video.decoder.wmv.secure";
                } else {
                    *role = (OMX_STRING)"OMX.qcom.video.decoder.vc1.secure";
                }
                break;
            case OMX_VIDEO_CodingMPEG4:
                *role = (OMX_STRING)"OMX.qcom.video.decoder.mpeg4.secure";
                break;
            case OMX_VIDEO_CodingVP9:
                *role = (OMX_STRING)"OMX.qcom.video.decoder.vp9.secure";
                break;
            case OMX_VIDEO_CodingVP8:
                *role = (OMX_STRING)"OMX.qcom.video.decoder.vp8.secure";
                break;
            default:
                /* This is to ensure GetHandle errors out*/
                *role = (OMX_STRING)"OMX.I.dont.exist";
                VTEST_MSG_ERROR("Secure session not supported for codec format : %d",
                                m_eCodec);
                return OMX_ErrorBadParameter;
        }
    }

    VTEST_MSG_MEDIUM("Role : %s, Codec Format : %d, File Type : %d",
                    *role, m_eCodec, *eFileType);
    return OMX_ErrorNone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::Flush(OMX_U32 nPortIndex)
{
    OMX_ERRORTYPE result = OMX_ErrorNone;
    CmdType cmd;

    result = OMX_SendCommand(m_hDecoder, OMX_CommandFlush, nPortIndex, 0);
    FAILED1(result, "Flush failed");

    if (m_pSignalQueue->Pop(&cmd, sizeof(cmd), 0) != OMX_ErrorNone) {
        FAILED1(result, "Error popping signal");
    }
    if (cmd.eResult != OMX_ErrorNone || cmd.eEvent != OMX_EventCmdComplete ||
        cmd.eCmd != OMX_CommandFlush) {
        result = OMX_ErrorUndefined;
        VTEST_MSG_ERROR("Was expecting complete for flush command");
    }

    /* Wait for flush complete for both ports */
    if (nPortIndex == OMX_ALL) {
        if (m_pSignalQueue->Pop(&cmd, sizeof(cmd), 0) != OMX_ErrorNone) {
            FAILED1(result, "Error popping signal");
        }
        if (cmd.eResult != OMX_ErrorNone || cmd.eEvent != OMX_EventCmdComplete ||
                cmd.eCmd != OMX_CommandFlush) {
            result = OMX_ErrorUndefined;
            VTEST_MSG_ERROR("Was expecting complete for flush command");
        }
    }
    return result;
}

OMX_ERRORTYPE Decoder::ParseExtraData(BufferPool * pBufferPool, OMX_BUFFERHEADERTYPE *pHeader, int idx) {

    struct vdec_output_frameinfo *pOutputRespbuf;
    OMX_U8 *pBaseAdd;
    int nBuffer_size;
    OMX_U8* pBuff = NULL;
    OMX_OTHER_EXTRADATATYPE *pExtra = NULL;
    OMX_U32 nRefFrameNum, nYUVSize = 0;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    private_handle_t *handle = NULL;

    VTEST_MSG_HIGH("ExtraData : pHeader(%p) BufTS(%lld) idx(%x) pExtraDataBuffer(%p) pExtraDatapBuffer(%p)",
            pHeader, pHeader->nTimeStamp, idx, pBufferPool->pExtraDataBuffer[idx],
            pBufferPool->pExtraDataBuffer[idx]->pBuffer);
    pExtra = (OMX_OTHER_EXTRADATATYPE *)(pBufferPool->pExtraDataBuffer[idx]->pBuffer);
    pBaseAdd = (OMX_U8 *)pExtra;
    nBuffer_size = pBufferPool->nExtraDataBufferSize;
    VTEST_MSG_LOW("ExtraData : pExtra(%p) Type(%x) DataSz(%u)",
            pExtra, pExtra->eType, pExtra->nDataSize);
    while (pExtra && ((OMX_U8*)pExtra < (pBaseAdd + nBuffer_size)) &&
          (pExtra->eType != OMX_ExtraDataNone) &&
          (result == OMX_ErrorNone) && (pExtra->nDataSize != 0)) {
        VTEST_MSG_HIGH("ExtraData : pBuf(%p) BufTS(%lld) Type(%x) DataSz(%u)",
            pHeader, pHeader->nTimeStamp, pExtra->eType, pExtra->nDataSize);

        switch ((unsigned long)pExtra->eType)
        {
            case OMX_ExtraDataInterlaceFormat:
            {
                OMX_STREAMINTERLACEFORMAT *pInterlaceFormat = (OMX_STREAMINTERLACEFORMAT *)pExtra->data;
                char sRefInterlaceFormat[MAX_TAG_LEN] = {0};
                OMX_STRING sInterlaceFormat = NULL;
                sInterlaceFormat = (OMX_STRING)ParseEnumValue(g_sessionInterlaceDataTypeMap, pInterlaceFormat->nInterlaceFormats);

                VTEST_MSG_HIGH("OMX_ExtraDataInterlaceFormat: Buf(%p) TSmp(%lld) IntPtr(%p) Fmt(%s)",
                                              pHeader->pBuffer, pHeader->nTimeStamp,pInterlaceFormat,
                                                                                    sInterlaceFormat);

                if (m_fInterlace && (sInterlaceFormat != NULL))
                {
                    fprintf(m_fInterlace, "%u %s\n", m_nFrameDoneCount,(OMX_STRING)sInterlaceFormat);
                }
                if (m_fInterlaceRef)
                {
                    fscanf(m_fInterlaceRef,"%u %s", &nRefFrameNum,sRefInterlaceFormat);
                    if (sInterlaceFormat != NULL)
                    {
                        if ((nRefFrameNum != m_nFrameDoneCount) ||
                           (strcmp(sRefInterlaceFormat,sInterlaceFormat)))
                        {
                            VTEST_MSG_ERROR("Mismatch with Reference: Frame# %u Ref Frame no: %d Interlace fmt %s Ref fmt %s",
                                m_nFrameDoneCount, nRefFrameNum, sInterlaceFormat, sRefInterlaceFormat);
                            result = OMX_ErrorUndefined;
                        }
                    }
                }
            }
            break;
            case OMX_ExtraDataFrameInfo:
            {
                OMX_QCOM_EXTRADATA_FRAMEINFO *frame_info = (OMX_QCOM_EXTRADATA_FRAMEINFO *)pExtra->data;
                OMX_QCOM_EXTRADATA_FRAMEINFO refFrameInfo = {};
                OMX_U32 refFPS = 0;
                OMX_U32  refNumConcealMbs = 0;
                OMX_QCOM_PANSCAN refPanScanPayLoad = {};
                OMX_TICKS refTimeStamp = 0;
                OMX_U32 nWindows = 0;
                char nRefRecoveryPointSEI[16];
                OMX_S32 height_off = 0, width_off = 0;
                OMX_S32 width = 0, height = 0;
                OMX_QCOMRectangle* wnd = NULL;

                VTEST_MSG_HIGH("OMX_ExtraDataFrameInfo: Buf(%p) TSmp(%lld) PicType(%u) IntT(%u) ConMB(%u)",
                         pHeader->pBuffer, pHeader->nTimeStamp, frame_info->ePicType,
                         frame_info->interlaceType, frame_info->nConcealedMacroblocks);
                VTEST_MSG_HIGH("FrmRate(%u), AspRatioX(%u), AspRatioY(%u) DispWidth(%u) DispHeight(%u)",
                        frame_info->nFrameRate, frame_info->aspectRatio.aspectRatioX,
                        frame_info->aspectRatio.aspectRatioY, frame_info->displayAspectRatio.displayHorizontalSize,
                        frame_info->displayAspectRatio.displayVerticalSize);
                VTEST_MSG_HIGH("PANSCAN numWindows(%d)", frame_info->panScan.numWindows);
                for (unsigned int i = 0; i < frame_info->panScan.numWindows; i++) {
                    VTEST_MSG_HIGH("WINDOW Lft(%d) Tp(%d) Rgt(%d) Bttm(%d)",
                        frame_info->panScan.window[i].x,
                        frame_info->panScan.window[i].y,
                        frame_info->panScan.window[i].dx,
                        frame_info->panScan.window[i].dy);
                }
                /* Process Aspect Ratio Extradata */
                if (m_fAspectRatio)
                {
                    fprintf(m_fAspectRatio, "%u %u %u\n", m_nFrameDoneCount,
                                       frame_info->aspectRatio.aspectRatioX,
                                       frame_info->aspectRatio.aspectRatioY);
                }
                if (m_fAspectRatioRef)
                {
                    fscanf(m_fAspectRatioRef,"%u %u %u", &nRefFrameNum,
                                &refFrameInfo.aspectRatio.aspectRatioX,
                                &refFrameInfo.aspectRatio.aspectRatioY);
                    if ((nRefFrameNum != m_nFrameDoneCount) ||
                       (frame_info->aspectRatio.aspectRatioX != refFrameInfo.aspectRatio.aspectRatioX) ||
                       (frame_info->aspectRatio.aspectRatioY!= refFrameInfo.aspectRatio.aspectRatioY))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: AspectRatio Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                }
                if (m_fFrameRate)
                {
                    fprintf(m_fFrameRate, "%u %u\n", m_nFrameDoneCount, frame_info->nFrameRate);
                }
                if (m_fFrameRateRef)
                {
                    fscanf(m_fFrameRateRef,"%u %u", &nRefFrameNum, &refFPS);
                    if ((nRefFrameNum != m_nFrameDoneCount) ||
                        (refFPS != frame_info->nFrameRate))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: Frame Rate Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                }

                /* Process Conceal MB Extradata */
                if (m_fConcealMB)
                {
                    fprintf(m_fConcealMB, "%u %u\n", m_nFrameDoneCount, frame_info->nConcealedMacroblocks);
                }
                if (m_fConcealMBRef)
                {
                    fscanf(m_fConcealMBRef,"%u %u", &nRefFrameNum, &refNumConcealMbs);
                    /* num of conceal MB returned might change from HW to HW
                     * check only if num of conceal MB is non-zero or not */

                    if ((nRefFrameNum != m_nFrameDoneCount)  ||
                       (!(refNumConcealMbs)!= !(frame_info->nConcealedMacroblocks)))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: NumConcealMB Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                }

                /* Process Recovery Point SEI Extradata */
                if (m_fRecoveryPointSEI)
                {
                    fprintf(m_fRecoveryPointSEI, "%u %d\n", m_nFrameDoneCount, frame_info->nRecoverySeiFlag);
                }
                if (m_fRecoveryPointSEIRef)
                {
                    fscanf(m_fRecoveryPointSEIRef, "%u %s", &nRefFrameNum, nRefRecoveryPointSEI);
                    VTEST_MSG_HIGH("Recoverypoint SEI received: %d Reference: %s strcmp %d!",
                        frame_info->nRecoverySeiFlag, nRefRecoveryPointSEI,
                        (strcmp(nRefRecoveryPointSEI, "CORRECT")));
                    VTEST_MSG_HIGH("Recoverypoint SEI strcmp ret: %d Reference: %d!",
                        (strcmp(nRefRecoveryPointSEI, "CORRECT") == 0),
                        frame_info->nRecoverySeiFlag);
                    if ((nRefFrameNum != m_nFrameDoneCount) ||
                        ((strcmp(nRefRecoveryPointSEI, "CORRECT") == 0) != frame_info->nRecoverySeiFlag) )
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: Recoverypoint SEI Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                }

                /* Process Pan Scan Window Extradata */
                if (m_fPanScan)
                {
                    fprintf(m_fPanScan, "%u %d ",m_nFrameDoneCount, frame_info->panScan.numWindows);
                    for(OMX_U32 i =0; i < frame_info->panScan.numWindows; i++)
                    {
                        fprintf(m_fPanScan, "%d %d %d %d",
                                frame_info->panScan.window[i].dy,
                                frame_info->panScan.window[i].dx,
                                frame_info->panScan.window[i].x,
                                frame_info->panScan.window[i].y);
                    }
                    fprintf(m_fPanScan, "\n");
                }
                if (m_fPanScanRef)
                {
                    fscanf(m_fPanScanRef, "%u %d ",&nRefFrameNum, &nWindows);

                    if ((nWindows != frame_info->panScan.numWindows) ||
                        (nRefFrameNum != m_nFrameDoneCount))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: PanScanExtradata Frame# %u!",
                                                                                      m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                    else
                    {
                        wnd =  frame_info->panScan.window;
                        for(OMX_U32 i =0; i < frame_info->panScan.numWindows; i++)
                        {
                            fscanf(m_fPanScanRef, "%d %d %d %d ", &height_off, &width_off, &width, &height);
                            if ( wnd->dy!= height_off ||
                                 wnd->dx != width_off ||
                                 wnd->x  != width ||
                                 wnd->y != height )
                            {
                                VTEST_MSG_ERROR("ExtraData Mismatch with Reference: PanScanExtradata Frame# %u!",
                                                                                              m_nFrameDoneCount);
                                result = OMX_ErrorUndefined;
                                break;
                            }
                            wnd += (sizeof(struct OMX_QCOMRectangle));
                        }
                    }

                }

                /* Process Timestamp Extradata */
                if (m_fTimeStamp)
                {
                    fprintf(m_fTimeStamp, "%u %llx\n", m_nFrameDoneCount, frame_info->nTimeStamp);
                }
                if (m_fTimeStampRef)
                {
                    fscanf(m_fTimeStampRef, "%u %llx", &nRefFrameNum, &refTimeStamp);

                    if ((nRefFrameNum != m_nFrameDoneCount) ||
                        (refTimeStamp != frame_info->nTimeStamp) )
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: TimeStamp Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                }
            }
            break;
            case OMX_ExtraDataConcealMB: {
                OMX_U8 data = 0, *data_ptr = (OMX_U8 *)pExtra->data;
                OMX_U32 concealMBnum = 0, bytes_cnt = 0;
                while (bytes_cnt < pExtra->nDataSize) {
                    data = *data_ptr;
                    while (data) {
                        concealMBnum += (data&0x01);
                        data >>= 1;
                    }
                    data_ptr++;
                    bytes_cnt++;
                }
                VTEST_MSG_HIGH("OMX_ExtraDataConcealMB: Buf(%p) TSmp(%lld) ConcealMB(%u)",
                    pHeader->pBuffer, pHeader->nTimeStamp, concealMBnum);
            } break;
            case OMX_ExtraDataOutputCropInfo: {
                /* Process Output Crop Info Extradata */
                OMX_QCOM_OUTPUT_CROP * outputCropPayload = (OMX_QCOM_OUTPUT_CROP *)(void *)pExtra->data;
                VTEST_MSG_HIGH("OMX_ExtraDataOutputCropInfo: Frame: %d  MISR DPB OPB [0]; 0x%x 0x%x 0x%x 0x%x\n",
                    m_nFrameDoneCount,
                    outputCropPayload->misr_info[0].misr_dpb_luma,
                    outputCropPayload->misr_info[0].misr_dpb_chroma,
                    outputCropPayload->misr_info[0].misr_opb_luma,
                    outputCropPayload->misr_info[0].misr_opb_chroma);
                VTEST_MSG_HIGH("OMX_ExtraDataOutputCropInfo: Frame: %d  MISR DPB OPB [1]; 0x%x 0x%x 0x%x 0x%x\n",
                    m_nFrameDoneCount,
                    outputCropPayload->misr_info[1].misr_dpb_luma,
                    outputCropPayload->misr_info[1].misr_dpb_chroma,
                    outputCropPayload->misr_info[1].misr_opb_luma,
                    outputCropPayload->misr_info[1].misr_opb_chroma);
#ifndef PRE_SDM845
                if(m_pCRCChecker && !sGlobalStaticVideoProp.bSkipCrcCheck)
                {
                    VideoResolution res = m_sOutputRes;
                    OMX_U8 *data = NULL;
                    BOOL bCurFrameCRC = FALSE;
                    VidcStatus ret;

                    res.nWidth = res.misrPayload.nWidth = outputCropPayload->width;
                    res.nHeight = res.misrPayload.nHeight= outputCropPayload->height;
                    res.nCropWidth = res.misrPayload.nCropWidth = outputCropPayload->display_width;
                    res.nCropHeight = res.misrPayload.nCropHeight = outputCropPayload->display_height;
                    res.misrPayload.nFrameNum = m_nFrameDoneCount;
                    res.misrPayload.nBitDepthY = outputCropPayload->bit_depth_y;
                    res.misrPayload.nBitDepthC = outputCropPayload->bit_depth_c;
                    memcpy(res.misrPayload.misrInfo, outputCropPayload->misr_info, 2*sizeof(VideoMisrInfo));

                    VTEST_MSG_LOW("Ref: Frame: %d  DPB L C OPB L C[0]; 0x%x 0x%x 0x%x 0x%x\n",
                                m_nFrameDoneCount,
                                res.misrPayload.misrInfo[0].nMisrDPBLuma,
                                res.misrPayload.misrInfo[0].nMisrDPBChroma,
                                res.misrPayload.misrInfo[0].nMisrOPBLuma,
                                res.misrPayload.misrInfo[0].nMisrOPBChroma);

                    ret = m_pCRCChecker->CheckCRCofYUVframe(m_pCRCChecker,
                                       data, &res, TRUE, &bCurFrameCRC);
                    if ( ret || (!bCurFrameCRC))
                    {
                        VTEST_MSG_ERROR("MISR failure with ret %d  at Frame no: %d",
                            ret, m_pCRCChecker->m_fCRCCounter);
                        result = OMX_ErrorUndefined;
                    }
                    else if (bCurFrameCRC == TRUE && m_bThumbnailMode)
                    {
                        sGlobalStaticVideoProp.bSkipCrcCheck = OMX_TRUE;
                    }
                }
#endif
            }
            break;
            case OMX_ExtraDataMP2ExtnData: {
                VTEST_MSG_HIGH("\nOMX_ExtraDataMP2ExtnData");
                OMX_U8 data = 0, *data_ptr = (OMX_U8 *)pExtra->data;
                OMX_U32 bytes_cnt = 0;
                while (bytes_cnt < pExtra->nDataSize) {
                    VTEST_MSG_HIGH("\nMPEG-2 Extension Data Values[%d] = 0x%x", bytes_cnt, *data_ptr);
                    data_ptr++;
                    bytes_cnt++;
                }
            } break;
            case OMX_ExtraDataMP2UserData: {
                OMX_QCOM_EXTRADATA_USERDATA *userdata = (OMX_QCOM_EXTRADATA_USERDATA *)(void *)pExtra->data;
                OMX_U8 *data_ptr = (OMX_U8 *)userdata->data;
                OMX_U32 userdata_size;
                OMX_U32 i = 0;
                if (pExtra->nDataSize <= sizeof(userdata->type))
                {
                    VTEST_MSG_ERROR("UserDataExtradataError - Invalid userdata size ");
                    break;
                }
                userdata_size = pExtra->nDataSize - sizeof(userdata->type);
                VTEST_MSG_HIGH(
                    "--------------  Userdata  -------------\n"
                    "    Stream userdata type: %u\n"
                    "    userdata size: %u\n"
                    "    STREAM_USERDATA:",
                    (unsigned int)userdata->type, (unsigned int)userdata_size);
                for (i = 0; i < userdata_size-3; i+=4) {
                    VTEST_MSG_HIGH("            %x %x %x %x",
                        data_ptr[i], data_ptr[i+1], data_ptr[i+2], data_ptr[i+3]);
                }
                for (; i < userdata_size; i++) {
                    VTEST_MSG_HIGH("      %x ",data_ptr[i]);
                }

                if (m_fUserData)
                {
                    OMX_STRING userdata_type = (OMX_STRING)ParseEnumValue(g_sessionUserDataTypeMap, userdata->type);
                    if (userdata_type)
                        fprintf(m_fUserData, "%u %u %s ", m_nFrameDoneCount, userdata_size, userdata_type);

                    for (i = 0; i < userdata_size-3; i+=4)
                    {
                       fprintf(m_fUserData, "%x %x %x %x ",data_ptr[i], data_ptr[i+1], data_ptr[i+2], data_ptr[i+3]);
                    }
                    for (; i < userdata_size; i++)
                    {
                        fprintf(m_fUserData, "%x ",data_ptr[i]);
                    }
                    fprintf(m_fUserData, "\n");
                }
                if (m_fUserDataRef)
                {
                    OMX_U32 size;
                    char type;
                    char* refType;
                    OMX_U8* pUserBuf = (OMX_U8* )malloc(sizeof(OMX_U8)* userdata_size);
                    if (!pUserBuf)
                    {
                        VTEST_MSG_ERROR("UserDataExtradataAllocError");
                        break;
                    }
                    fscanf(m_fUserDataRef," %u %u %s ", &nRefFrameNum,&size,&type);
                    refType = (OMX_STRING)ParseEnumValue(g_sessionUserDataTypeMap, userdata->type);
                    if (refType && ((nRefFrameNum != m_nFrameDoneCount) ||
                        (size != userdata_size) ||
                        (type == *refType)))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: Userdata header Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                    for (i = 0; i < userdata_size-3; i+=4)
                    {
                        fscanf(m_fUserDataRef, "%hx %hx %hx %hx ",(short unsigned int *)&pUserBuf[i],
                                                                  (short unsigned int *)&pUserBuf[i+1],
                                                                  (short unsigned int *)&pUserBuf[i+2],
                                                                  (short unsigned int *)&pUserBuf[i+3]);
                    }
                    for (; i < userdata_size; i++)
                    {
                        fscanf(m_fUserDataRef,"%hx ",(short unsigned int *)&pUserBuf[i]);
                    }
                    if (memcmp(pUserBuf,data_ptr,size))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: Userdata Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                    free(pUserBuf);
                }

            }
            break;
            case OMX_ExtraDataFramePackingArrangement: {
                VTEST_MSG_HIGH("\nOMX_ExtraDataFramePackingArrangement");
                OMX_QCOM_FRAME_PACK_ARRANGEMENT *framepack = (OMX_QCOM_FRAME_PACK_ARRANGEMENT *)(void *)pExtra->data;
                OMX_QCOM_FRAME_PACK_ARRANGEMENT refFramepack;

                VTEST_MSG_HIGH("Framepack Format: \n"
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
                if(m_fS3D)
                {
                    fprintf(m_fS3D, "%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
                            m_nFrameDoneCount,
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
                }
                if(m_fS3DRef)
                {
                    fscanf(m_fS3DRef,"%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",
                           &nRefFrameNum,
                           (unsigned int*)&refFramepack.id,
                           (unsigned int*)&refFramepack.cancel_flag,
                           (unsigned int*)&refFramepack.type,
                           (unsigned int*)&refFramepack.quincunx_sampling_flag,
                           (unsigned int*)&refFramepack.content_interpretation_type,
                           (unsigned int*)&refFramepack.spatial_flipping_flag,
                           (unsigned int*)&refFramepack.frame0_flipped_flag,
                           (unsigned int*)&refFramepack.field_views_flag,
                           (unsigned int*)&refFramepack.current_frame_is_frame0_flag,
                           (unsigned int*)&refFramepack.frame0_self_contained_flag,
                           (unsigned int*)&refFramepack.frame1_self_contained_flag,
                           (unsigned int*)&refFramepack.frame0_grid_position_x,
                           (unsigned int*)&refFramepack.frame0_grid_position_y,
                           (unsigned int*)&refFramepack.frame1_grid_position_x,
                           (unsigned int*)&refFramepack.frame1_grid_position_y,
                           (unsigned int*)&refFramepack.reserved_byte,
                           (unsigned int*)&refFramepack.repetition_period,
                           (unsigned int*)&refFramepack.extension_flag);
                    /* reference file wont have the info on size, version and port index
                     * hence ignore validation by copying the info from paylod */
                    refFramepack.nSize = framepack->nSize;
                    refFramepack.nVersion = framepack->nVersion;
                    refFramepack.nPortIndex = framepack->nPortIndex;

                    if((nRefFrameNum != m_nFrameDoneCount) ||
                        memcmp(&refFramepack,framepack,sizeof(struct OMX_QCOM_FRAME_PACK_ARRANGEMENT)))
                    {
                        VTEST_MSG_ERROR("ExtraData Mismatch with Reference: S3D frame packing Frame# %u!",m_nFrameDoneCount);
                        result = OMX_ErrorUndefined;
                    }
                }

            } break;
            case OMX_ExtraDataQP: {
                VTEST_MSG_HIGH("\nOMX_ExtraDataQP");
                OMX_QCOM_EXTRADATA_QP * qp = (OMX_QCOM_EXTRADATA_QP *)(void *)pExtra->data;
                OMX_QCOM_EXTRADATA_QP refqp;
                VTEST_MSG_HIGH("\n--------- QP ExtraData --------\n"
                               "              Frame QP: %u \n"
                               "       Sum of Frame QP: %u \n"
                               "     Sum of Skipped QP: %u \n"
                               "    Num Skipped Blocks: %u \n"
                               "          Total Blocks: %u \n"
                               "===== End of Input bits information =====\n",
                               (unsigned int)qp->nQP, (unsigned int)qp->nQPSum,
                               (unsigned int)qp->nSkipQPSum, (unsigned int)qp->nSkipNumBlocks,
                               (unsigned int)qp->nTotalNumBlocks);
                if (m_fFrameQP) {
                    fprintf(m_fFrameQP,"%u %u %u %u %u %u", m_nFrameDoneCount,
                            (unsigned int)qp->nQP, (unsigned int)qp->nQPSum,
                            (unsigned int)qp->nSkipQPSum, (unsigned int)qp->nSkipNumBlocks,
                            (unsigned int)qp->nTotalNumBlocks);
                }
                if (m_fFrameQPRef) {
                    fscanf(m_fFrameQPRef,"%u %u %u %u %u %u", &nRefFrameNum,
                            (unsigned int*)&refqp.nQP, (unsigned int*)&refqp.nQPSum,
                            (unsigned int*)&refqp.nSkipQPSum, (unsigned int*)&refqp.nSkipNumBlocks,
                            (unsigned int*)&refqp.nTotalNumBlocks);
                    if((nRefFrameNum != m_nFrameDoneCount) ||
                            memcmp(&refqp,qp,sizeof(struct OMX_QCOM_EXTRADATA_QP)))
                        {
                            VTEST_MSG_ERROR("ExtraData Mismatch with Reference: QP ExtraData Frame# %u!",m_nFrameDoneCount);
                            result = OMX_ErrorUndefined;
                    }
                }
            } break;
            case OMX_ExtraDataInputBitsInfo: {
                VTEST_MSG_HIGH("\nOMX_ExtraDataInputBitsInfo");
                OMX_QCOM_EXTRADATA_BITS_INFO * bits = (OMX_QCOM_EXTRADATA_BITS_INFO *)(void *)pExtra->data;
                VTEST_MSG_HIGH(
                    "\n--------- Input bits information --------\n"
                    "   Header bits: %u \n"
                    "    Frame bits: %u \n"
                    "===== End of Input bits information =====\n",
                    (unsigned int)bits->header_bits, (unsigned int)bits->frame_bits);
            } break;
            default:
                VTEST_MSG_ERROR("Unknown ExtraData!");
        }
        pExtra = (OMX_OTHER_EXTRADATATYPE *) (((OMX_U8 *) pExtra) + pExtra->nSize);
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::ProcessColorAspects() {
    OMX_ERRORTYPE result = OMX_ErrorNone;
    DescribeColorAspectsParams m_sOMXVUIColorAspects;
    int mFrameCount, mPrimaries, mRange, mTransfer, mMatrixCoeffs;
    android::ColorAspects m_sRefColorAspects;

    OMX_INIT_STRUCT(&m_sOMXVUIColorAspects, DescribeColorAspectsParams);
    m_sRefColorAspects.mPrimaries = ColorAspects::PrimariesUnspecified;
    m_sRefColorAspects.mRange = ColorAspects::RangeUnspecified;
    m_sRefColorAspects.mTransfer = ColorAspects::TransferUnspecified;
    m_sRefColorAspects.mMatrixCoeffs = ColorAspects::MatrixUnspecified;

    result = OMX_GetConfig(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QTIIndexConfigDescribeColorAspects,
                 (OMX_PTR)&m_sOMXVUIColorAspects);
    VTEST_MSG_HIGH("OMX Color Aspects: Primaries %d Range %d Transfer %d MatrixCoeffs %d",
                   m_sOMXVUIColorAspects.sAspects.mPrimaries,
                   m_sOMXVUIColorAspects.sAspects.mRange,
                   m_sOMXVUIColorAspects.sAspects.mTransfer,
                   m_sOMXVUIColorAspects.sAspects.mMatrixCoeffs);

    if(m_fVUIColorAspects) {
        fprintf(m_fVUIColorAspects, "%d %d %d %d %d \n",
            m_nFrameDoneCount,
            m_sOMXVUIColorAspects.sAspects.mPrimaries,
            m_sOMXVUIColorAspects.sAspects.mRange,
            m_sOMXVUIColorAspects.sAspects.mTransfer,
            m_sOMXVUIColorAspects.sAspects.mMatrixCoeffs);
    } else if((m_nColorPrimaries == 0) && m_fVUIColorAspectsRef) {
        fscanf(m_fVUIColorAspectsRef, "%d %d %d %d %d\n",
            &mFrameCount,
            &mPrimaries,
            &mRange,
            &mTransfer,
            &mMatrixCoeffs);

        convert_color_space_info(mPrimaries, mRange, mTransfer, mMatrixCoeffs, &m_sRefColorAspects);
        VTEST_MSG_HIGH("Ref Color aspects: Primaries %d Range %d Transfer %d MatrixCoeffs %d",
                       m_sRefColorAspects.mPrimaries,
                       m_sRefColorAspects.mRange,
                       m_sRefColorAspects.mTransfer,
                       m_sRefColorAspects.mMatrixCoeffs);

        if((m_sOMXVUIColorAspects.sAspects.mPrimaries != m_sRefColorAspects.mPrimaries) ||
           (m_sOMXVUIColorAspects.sAspects.mRange != m_sRefColorAspects.mRange) ||
           (m_sOMXVUIColorAspects.sAspects.mTransfer != m_sRefColorAspects.mTransfer) ||
           (m_sOMXVUIColorAspects.sAspects.mMatrixCoeffs != m_sRefColorAspects.mMatrixCoeffs)) {
           VTEST_MSG_ERROR("ExtraData Mismatch with Reference: Color aspects Frame# %u!",
                           m_nFrameDoneCount);
           result = OMX_ErrorUndefined;
        }
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::ProcessColorAspectsinReconfig() {
    OMX_ERRORTYPE result = OMX_ErrorNone;
    DescribeColorAspectsParams m_sOMXVUIColorAspects;
    int mFrameCount, mPrimaries, mRange, mTransfer, mMatrixCoeffs;
    android::ColorAspects m_sRefColorAspects;

    OMX_INIT_STRUCT(&m_sOMXVUIColorAspects, DescribeColorAspectsParams);
    m_sRefColorAspects.mPrimaries = ColorAspects::PrimariesUnspecified;
    m_sRefColorAspects.mRange = ColorAspects::RangeUnspecified;
    m_sRefColorAspects.mTransfer = ColorAspects::TransferUnspecified;
    m_sRefColorAspects.mMatrixCoeffs = ColorAspects::MatrixUnspecified;

    result = OMX_GetConfig(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QTIIndexConfigDescribeColorAspects,
                 (OMX_PTR)&m_sOMXVUIColorAspects);
    VTEST_MSG_HIGH("OMX Color Aspects: Primaries %d Range %d Transfer %d MatrixCoeffs %d",
                   m_sOMXVUIColorAspects.sAspects.mPrimaries,
                   m_sOMXVUIColorAspects.sAspects.mRange,
                   m_sOMXVUIColorAspects.sAspects.mTransfer,
                   m_sOMXVUIColorAspects.sAspects.mMatrixCoeffs);

    if((m_sOMXVUIColorAspects.sAspects.mPrimaries == m_sVUIColorAspects.sAspects.mPrimaries) &&
        (m_sOMXVUIColorAspects.sAspects.mRange == m_sVUIColorAspects.sAspects.mRange) &&
        (m_sOMXVUIColorAspects.sAspects.mTransfer == m_sVUIColorAspects.sAspects.mTransfer) &&
        (m_sOMXVUIColorAspects.sAspects.mMatrixCoeffs == m_sVUIColorAspects.sAspects.mMatrixCoeffs)) {
        VTEST_MSG_ERROR("Color aspects same as previous for Frame# %u!",m_nFrameDoneCount);
        result = OMX_ErrorUndefined;
    }

    if(m_fVUIColorAspects) {
        VTEST_MSG_ERROR("Dumping of color aspects is not supported in OMX test app!");
        result = OMX_ErrorUndefined;
    } else if((m_nColorPrimaries == 1) && m_fVUIColorAspectsRef) {
        //For frame 0, do a scanf only once.
        //For rest of the frames, do a scanf till we find a mismatch.
        do {
            fscanf(m_fVUIColorAspectsRef, "%d %d %d %d %d\n",
                &mFrameCount,
                &mPrimaries,
                &mRange,
                &mTransfer,
                &mMatrixCoeffs);
        } while((m_nFrameDoneCount != 0) &&
                ((int)m_sPreviousRefColorAspects.sAspects.mPrimaries == mPrimaries) &&
                ((int)m_sPreviousRefColorAspects.sAspects.mRange == mRange) &&
                ((int)m_sPreviousRefColorAspects.sAspects.mTransfer == mTransfer) &&
                ((int)m_sPreviousRefColorAspects.sAspects.mMatrixCoeffs == mMatrixCoeffs));
        m_sPreviousRefColorAspects.sAspects.mPrimaries = (android::ColorAspects::Primaries)mPrimaries;
        m_sPreviousRefColorAspects.sAspects.mRange = (android::ColorAspects::Range)mRange;
        m_sPreviousRefColorAspects.sAspects.mTransfer = (android::ColorAspects::Transfer)mTransfer;
        m_sPreviousRefColorAspects.sAspects.mMatrixCoeffs = (android::ColorAspects::MatrixCoeffs)mMatrixCoeffs;

        convert_color_space_info(mPrimaries, mRange, mTransfer, mMatrixCoeffs, &m_sRefColorAspects);
        VTEST_MSG_HIGH("Ref Color aspects: Primaries %d Range %d Transfer %d MatrixCoeffs %d",
                       m_sRefColorAspects.mPrimaries,
                       m_sRefColorAspects.mRange,
                       m_sRefColorAspects.mTransfer,
                       m_sRefColorAspects.mMatrixCoeffs);

        if((m_sOMXVUIColorAspects.sAspects.mPrimaries != m_sRefColorAspects.mPrimaries) ||
           (m_sOMXVUIColorAspects.sAspects.mRange != m_sRefColorAspects.mRange) ||
           (m_sOMXVUIColorAspects.sAspects.mTransfer != m_sRefColorAspects.mTransfer) ||
           (m_sOMXVUIColorAspects.sAspects.mMatrixCoeffs != m_sRefColorAspects.mMatrixCoeffs)) {
           VTEST_MSG_ERROR("ExtraData Mismatch with Reference: Color aspects Frame# %u!",
                           m_nFrameDoneCount);
           result = OMX_ErrorUndefined;
        }
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_BOOL Decoder::CheckColorFormatSupported(OMX_COLOR_FORMATTYPE nColorFormat,
                          OMX_VIDEO_PARAM_PORTFORMATTYPE *pVideoPortFmt) {

    OMX_BOOL bSupported = OMX_FALSE;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_S32 index = 0;

    OMX_INIT_STRUCT(pVideoPortFmt, OMX_VIDEO_PARAM_PORTFORMATTYPE);

    if (!m_hDecoder) {
        VTEST_MSG_ERROR("m_hDecoder handle is null");
        return OMX_FALSE;
    }
    VTEST_MSG_HIGH("Decoder: Format[0x%x] requested", nColorFormat);

    while (result == OMX_ErrorNone) {
        pVideoPortFmt->nPortIndex = PORT_INDEX_OUT;
        pVideoPortFmt->nIndex = index;
        result = OMX_GetParameter(m_hDecoder,
                OMX_IndexParamVideoPortFormat, (OMX_PTR)pVideoPortFmt);
        if (result == OMX_ErrorNone && pVideoPortFmt->eColorFormat == nColorFormat) {
            VTEST_MSG_HIGH("Decoder: Format[0x%x] supported by OMX Decoder", nColorFormat);
            bSupported = OMX_TRUE;
            break;
        }
        index++;
    }
    return bSupported;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::SetPictureTypeDecode(OMX_U32 nPictureTypeDecode) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_QCOM_VIDEO_CONFIG_PICTURE_TYPE_DECODE pDecodeType;
    OMX_INIT_STRUCT(&pDecodeType, OMX_QCOM_VIDEO_CONFIG_PICTURE_TYPE_DECODE);

    pDecodeType.eDecodeType = (QOMX_VIDEO_PICTURE_TYPE_DECODE)nPictureTypeDecode;

    result = OMX_SetConfig(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QcomIndexConfigPictureTypeDecode,
                 (OMX_PTR)&pDecodeType);
    VTEST_MSG_HIGH("Decoder: PictureTypeDecode set to %d", nPictureTypeDecode);
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Decoder::SetVUIColorAspectsExtraData(android::ColorAspects sColorAspects) {
    OMX_ERRORTYPE result = OMX_ErrorNone;
    ColorAspects::MatrixCoeffs coeffs;
    ColorAspects::Primaries primaries;
    OMX_INIT_STRUCT(&m_sVUIColorAspects, DescribeColorAspectsParams);

    // Default to BT2020, BT709 or BT601 based on size. Allow 2.35:1 aspect ratio. Limit BT601
    // to PAL or smaller, BT2020 to 4K or larger, leaving BT709 for all resolutions in between.
    if (m_nFrameWidth >= 3840 || m_nFrameHeight >= 3840 || m_nFrameWidth * (int64_t)m_nFrameHeight >= 3840 * 1634){
        primaries = ColorAspects::PrimariesBT2020;
        coeffs = ColorAspects::MatrixBT2020;
    } else if ((m_nFrameWidth <= 720 && m_nFrameHeight > 480 && m_nFrameHeight <= 576)
            || (m_nFrameHeight <= 720 && m_nFrameWidth > 480 && m_nFrameWidth <= 576)) {
        primaries = ColorAspects::PrimariesBT601_6_625;
        coeffs = ColorAspects::MatrixBT601_6;
    } else if ((m_nFrameWidth <= 720 && m_nFrameHeight <= 480) || (m_nFrameHeight <= 720 && m_nFrameWidth <= 480)) {
        primaries = ColorAspects::PrimariesBT601_6_525;
        coeffs = ColorAspects::MatrixBT601_6;
    } else {
        primaries = ColorAspects::PrimariesBT709_5;
        coeffs = ColorAspects::MatrixBT709_5;
    }

    if ((sColorAspects.mRange != ColorAspects::RangeUnspecified) ||
        (sColorAspects.mPrimaries != ColorAspects::PrimariesUnspecified) ||
        (sColorAspects.mMatrixCoeffs != ColorAspects::MatrixUnspecified) ||
        (sColorAspects.mTransfer != ColorAspects::TransferUnspecified)) {
        m_nColorPrimaries = 1;
        VTEST_MSG_LOW("Color aspects functionality would be exercised");
    } else {
        m_nColorPrimaries = 0;
        VTEST_MSG_LOW("Color aspects of bitstream would be verified");
    }

    if (sColorAspects.mRange == ColorAspects::RangeUnspecified) {
        m_sVUIColorAspects.sAspects.mRange = ColorAspects::RangeLimited;
    }
    if (sColorAspects.mPrimaries == ColorAspects::PrimariesUnspecified) {
        m_sVUIColorAspects.sAspects.mPrimaries = primaries;
    }
    if (sColorAspects.mMatrixCoeffs == ColorAspects::MatrixUnspecified) {
        m_sVUIColorAspects.sAspects.mMatrixCoeffs = coeffs;
    }
    if (sColorAspects.mTransfer == ColorAspects::TransferUnspecified) {
        m_sVUIColorAspects.sAspects.mTransfer = ColorAspects::TransferSMPTE170M;
    }
    VTEST_MSG_HIGH("Set Color aspects: Primaries %d Range %d Transfer %d MatrixCoeffs %d",
                   m_sVUIColorAspects.sAspects.mPrimaries,
                   m_sVUIColorAspects.sAspects.mRange,
                   m_sVUIColorAspects.sAspects.mTransfer,
                   m_sVUIColorAspects.sAspects.mMatrixCoeffs);

    result = OMX_SetConfig(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QTIIndexConfigDescribeColorAspects,
                 (OMX_PTR)&m_sVUIColorAspects);
    VTEST_MSG_HIGH("Decoder: VUIColorAspectsExtraData set to OMX comp");
    result = OMX_GetConfig(m_hDecoder,
                 (OMX_INDEXTYPE)OMX_QTIIndexConfigDescribeColorAspects,
                 (OMX_PTR)&m_sVUIColorAspects);

    return result;
}
void Decoder::convert_color_space_info(OMX_U32 primaries, OMX_U32 range,
    OMX_U32 transfer, OMX_U32 matrix, android::ColorAspects *aspects) {
    switch (primaries) {
        case MSM_VIDC_BT709_5:
            aspects->mPrimaries = ColorAspects::PrimariesBT709_5;
            break;
        case MSM_VIDC_BT470_6_M:
            aspects->mPrimaries = ColorAspects::PrimariesBT470_6M;
            break;
        case MSM_VIDC_BT601_6_625:
            aspects->mPrimaries = ColorAspects::PrimariesBT601_6_625;
            break;
        case MSM_VIDC_BT601_6_525:
            aspects->mPrimaries = ColorAspects::PrimariesBT601_6_525;
            break;
        case MSM_VIDC_GENERIC_FILM:
            aspects->mPrimaries = ColorAspects::PrimariesGenericFilm;
            break;
        case MSM_VIDC_BT2020:
            aspects->mPrimaries = ColorAspects::PrimariesBT2020;
            break;
        case MSM_VIDC_UNSPECIFIED:
            //Client does not expect ColorAspects::PrimariesUnspecified, but rather the supplied default
        default:
            //aspects->mPrimaries = ColorAspects::PrimariesOther;
            break;
    }

    aspects->mRange = range ? ColorAspects::RangeFull : ColorAspects::RangeLimited;

    switch (transfer) {
        case MSM_VIDC_TRANSFER_BT709_5:
        case MSM_VIDC_TRANSFER_601_6_525: // case MSM_VIDC_TRANSFER_601_6_625:
            aspects->mTransfer = ColorAspects::TransferSMPTE170M;
            break;
        case MSM_VIDC_TRANSFER_BT_470_6_M:
            aspects->mTransfer = ColorAspects::TransferGamma22;
            break;
        case MSM_VIDC_TRANSFER_BT_470_6_BG:
            aspects->mTransfer = ColorAspects::TransferGamma28;
            break;
        case MSM_VIDC_TRANSFER_SMPTE_240M:
            aspects->mTransfer = ColorAspects::TransferSMPTE240M;
            break;
        case MSM_VIDC_TRANSFER_LINEAR:
            aspects->mTransfer = ColorAspects::TransferLinear;
            break;
        case MSM_VIDC_TRANSFER_IEC_61966:
            aspects->mTransfer = ColorAspects::TransferXvYCC;
            break;
        case MSM_VIDC_TRANSFER_BT_1361:
            aspects->mTransfer = ColorAspects::TransferBT1361;
            break;
        case MSM_VIDC_TRANSFER_SRGB:
            aspects->mTransfer = ColorAspects::TransferSRGB;
            break;
        default:
            break;
    }
    switch (matrix) {
        case MSM_VIDC_MATRIX_BT_709_5:
            aspects->mMatrixCoeffs = ColorAspects::MatrixBT709_5;
            break;
        case MSM_VIDC_MATRIX_FCC_47:
            aspects->mMatrixCoeffs = ColorAspects::MatrixBT470_6M;
            break;
        case MSM_VIDC_MATRIX_601_6_625:
        case MSM_VIDC_MATRIX_601_6_525:
            aspects->mMatrixCoeffs = ColorAspects::MatrixBT601_6;
            break;
        case MSM_VIDC_MATRIX_SMPTE_240M:
            aspects->mMatrixCoeffs = ColorAspects::MatrixSMPTE240M;
            break;
        case MSM_VIDC_MATRIX_BT_2020:
            aspects->mMatrixCoeffs = ColorAspects::MatrixBT2020;
            break;
        case MSM_VIDC_MATRIX_BT_2020_CONST:
            aspects->mMatrixCoeffs = ColorAspects::MatrixBT2020Constant;
            break;
        default:
            break;
    }
}

} // namespace vtest
