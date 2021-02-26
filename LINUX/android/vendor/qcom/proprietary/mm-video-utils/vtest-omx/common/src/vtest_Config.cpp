/*-------------------------------------------------------------------
Copyright (c) 2013-2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2010 The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The Linux Foundation nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/

#include "vtest_ComDef.h"
#include "vtest_Debug.h"
#include "vtest_Config.h"
#include "vtest_Parser.h"
#include "vtest_File.h"
#include <stdlib.h>
#ifdef NON_BIONIC
#include <linux/limits.h>
#else
#include <sys/limits.h>
#endif
#include "OMX_IndexExt.h"
#include "graphics.h"
#include <assert.h>

#undef LOG_TAG
#define LOG_TAG "VTEST_CONFIG"

#define INVALID_VALUE -1

namespace vtest {

// Codecs
static ConfigEnum g_pCodecEnums[] =
{
    { (OMX_STRING)"MP4",   OMX_VIDEO_CodingMPEG4 },
    { (OMX_STRING)"H263",  OMX_VIDEO_CodingH263 },
    { (OMX_STRING)"H264",  OMX_VIDEO_CodingAVC },
    { (OMX_STRING)"VP8",   OMX_VIDEO_CodingVP8 },
    { (OMX_STRING)"VC1",   OMX_VIDEO_CodingWMV },
    { (OMX_STRING)"DIVX",  QOMX_VIDEO_CodingDivx },
    { (OMX_STRING)"MPEG2", OMX_VIDEO_CodingMPEG2 },
    { (OMX_STRING)"HEVC",  QOMX_VIDEO_CodingHevc },
    { (OMX_STRING)"VP9",   OMX_VIDEO_CodingVP9 },
    { 0, 0 }
};

// Rate control flavors
static ConfigEnum g_pVencRCEnums[] =
{
    { (OMX_STRING)"RC_OFF",     (int)OMX_Video_ControlRateDisable },
    { (OMX_STRING)"RC_VBR_VFR", (int)OMX_Video_ControlRateVariableSkipFrames },       // Camcorder
    { (OMX_STRING)"RC_VBR_CFR", (int)OMX_Video_ControlRateVariable },                 // Camcorder
    { (OMX_STRING)"RC_CBR_VFR", (int)OMX_Video_ControlRateConstantSkipFrames },       // QVP
    { (OMX_STRING)"RC_CBR_CFR", (int)OMX_Video_ControlRateConstant },                 // WFD
    { 0, 0 }
};

// Resync marker types
static ConfigEnum g_pResyncMarkerType[] =
{
    { (OMX_STRING)"NONE",        (int)RESYNC_MARKER_NONE },
    { (OMX_STRING)"BITS",        (int)RESYNC_MARKER_BITS },
    { (OMX_STRING)"MB",          (int)RESYNC_MARKER_MB },
    { (OMX_STRING)"GOB",         (int)RESYNC_MARKER_GOB },
    { 0, 0 }
};

// Codec Profile Type
static ConfigEnum g_pCodecProfileType[] =
{
    { (OMX_STRING)"MPEG4_SP",			(int)MPEG4ProfileSimple },
    { (OMX_STRING)"MPEG4_ASP",			(int)MPEG4ProfileAdvancedSimple },
    { (OMX_STRING)"H263_BASELINE",		(int)H263ProfileBaseline },
    { (OMX_STRING)"H264_BASELINE",		(int)AVCProfileBaseline },
    { (OMX_STRING)"H264_HIGH",			(int)AVCProfileHigh },
    { (OMX_STRING)"H264_MAIN",			(int)AVCProfileMain },
    { (OMX_STRING)"H264_CBP",			(int)AVCProfileConstrainedBaseline },
    { (OMX_STRING)"H264_CHP",			(int)AVCProfileConstrainedHigh },
    { (OMX_STRING)"VP8_MAIN",			(int)VP8ProfileMain },
    { (OMX_STRING)"HEVC_MAIN",			(int)HEVCProfileMain },
    { (OMX_STRING)"HEVC_MAIN10",		(int)HEVCProfileMain10HDR10 },
    { (OMX_STRING)"HEVC_MAIN10HDR10",		(int)HEVCProfileMain10HDR10 },
    { 0, 0 }
};

// Playback Modes
static ConfigEnum g_pPlaybackMode[] =
{
    { (OMX_STRING)"DYNAMIC_PORT_RECONFIG",     DynamicPortReconfig },
    { (OMX_STRING)"ADAPTIVE_SMOOTH_STREAMING", AdaptiveSmoothStreaming },
    { (OMX_STRING)"QC_SMOOTH_STREAMING",       QCSmoothStreaming },
    { (OMX_STRING)"DYNAMIC_BUFFER_MODE",       DynamicBufferMode },
    { 0, 0 }
};


// YuvColorSpace Types
static ConfigEnum g_pYuvColorSpaceType[] =
{
    { (OMX_STRING)"601",   ITUR601 },
    { (OMX_STRING)"601FR", ITUR601FR },
    { (OMX_STRING)"709",   ITUR709 },
    { 0, 0 }
};

static ConfigEnum g_pPostProcType[] =
{
    { (OMX_STRING)"C2DCC", C2dColorConversion },
    { (OMX_STRING)"MMCC",  MmColorConversion },
    { (OMX_STRING)"GPUPP", GpuPostProcessing },
    { 0, 0 }
};

static ConfigEnum g_pIntraRefreshMode[] =
{
    { (OMX_STRING)"IR_CYCLIC",    OMX_VIDEO_IntraRefreshCyclic },
    { (OMX_STRING)"IR_RANDOM",    OMX_VIDEO_IntraRefreshRandom },
    { (OMX_STRING)"IR_ADAPTIVE",  OMX_VIDEO_IntraRefreshAdaptive },
    { 0, 0 }
};

// Codec Profile Type
static ConfigEnum g_pXmlOmxProfileMap[] =
{
    { (OMX_STRING)"VIDEO_MPEG4ProfileSimple",           (int)MPEG4ProfileSimple },
    { (OMX_STRING)"VIDEO_MPEG4ProfileAdvancedSimple",           (int)MPEG4ProfileAdvancedSimple },
    { (OMX_STRING)"VIDEO_H263ProfileSimple",        (int)H263ProfileBaseline },
    { (OMX_STRING)"VIDEO_AVCProfileBaseline",       (int)AVCProfileBaseline },
    { (OMX_STRING)"VIDEO_AVCProfileHigh",           (int)AVCProfileHigh },
    { (OMX_STRING)"VIDEO_AVCProfileConstrainedBaseline",        (int)AVCProfileConstrainedBaseline },
    { (OMX_STRING)"VIDEO_AVCProfileConstrainedHigh",            (int)AVCProfileConstrainedHigh },
    { (OMX_STRING)"VIDEO_AVCProfileMain",           (int)AVCProfileMain },
    { (OMX_STRING)"VP8_MAIN",           (int)VP8ProfileMain },
    { (OMX_STRING)"VIDEO_HEVCProfileMain",          (int)HEVCProfileMain },
    { (OMX_STRING)"VIDEO_HEVCProfileMain10",        (int)HEVCProfileMain10HDR10 },
    { (OMX_STRING)"VIDEO_HEVCProfileMain10HDR10",        (int)HEVCProfileMain10HDR10 },
    { (OMX_STRING)"VIDEO_TMEProfile0",            (int)TMEProfile0 },
    { (OMX_STRING)"VIDEO_TMEProfile1",            (int)TMEProfile1 },
    { (OMX_STRING)"VIDEO_TMEProfile2",            (int)TMEProfile2 },
    { (OMX_STRING)"VIDEO_TMEProfile3",            (int)TMEProfile3 },
    { 0, 0 }
};

// Codec Level Type
static ConfigEnum g_pXmlOmxLevelMap[] =
{
    { (OMX_STRING)"DEFAULT",                    (int)DefaultLevel },
    { (OMX_STRING)"VIDEO_AVCLevel10",           (int)AVCLevel1 },
    { (OMX_STRING)"VIDEO_AVCLevel1b",           (int)AVCLevel1b },
    { (OMX_STRING)"VIDEO_AVCLevel11",           (int)AVCLevel11 },
    { (OMX_STRING)"VIDEO_AVCLevel12",           (int)AVCLevel12 },
    { (OMX_STRING)"VIDEO_AVCLevel13",           (int)AVCLevel13 },
    { (OMX_STRING)"VIDEO_AVCLevel20",           (int)AVCLevel2 },
    { (OMX_STRING)"VIDEO_AVCLevel21",           (int)AVCLevel21 },
    { (OMX_STRING)"VIDEO_AVCLevel22",           (int)AVCLevel22 },
    { (OMX_STRING)"VIDEO_AVCLevel30",           (int)AVCLevel3 },
    { (OMX_STRING)"VIDEO_AVCLevel31",           (int)AVCLevel31 },
    { (OMX_STRING)"VIDEO_AVCLevel32",           (int)AVCLevel32 },
    { (OMX_STRING)"VIDEO_AVCLevel40",           (int)AVCLevel4 },
    { (OMX_STRING)"VIDEO_AVCLevel41",           (int)AVCLevel41 },
    { (OMX_STRING)"VIDEO_AVCLevel42",           (int)AVCLevel42 },
    { (OMX_STRING)"VIDEO_AVCLevel50",           (int)AVCLevel5 },
    { (OMX_STRING)"VIDEO_AVCLevel51",           (int)AVCLevel51 },
    { (OMX_STRING)"VIDEO_AVCLevel52",           (int)AVCLevel52 },
    { (OMX_STRING)"VIDEO_HEVCLevel10",           (int)HEVCLevel1 },
    { (OMX_STRING)"VIDEO_HEVCLevel20",           (int)HEVCLevel2 },
    { (OMX_STRING)"VIDEO_HEVCLevel21",           (int)HEVCLevel21 },
    { (OMX_STRING)"VIDEO_HEVCLevel30",           (int)HEVCLevel3 },
    { (OMX_STRING)"VIDEO_HEVCLevel31",           (int)HEVCLevel31 },
    { (OMX_STRING)"VIDEO_HEVCLevel40",           (int)HEVCLevel4 },
    { (OMX_STRING)"VIDEO_HEVCLevel41",           (int)HEVCLevel41 },
    { (OMX_STRING)"VIDEO_HEVCLevel50",           (int)HEVCLevel5 },
    { (OMX_STRING)"VIDEO_HEVCLevel51",           (int)HEVCLevel51 },
    { (OMX_STRING)"VIDEO_HEVCLevel52",           (int)HEVCLevel52 },
    { (OMX_STRING)"VIDEO_HEVCLevel60",           (int)HEVCLevel6 },
    { (OMX_STRING)"VIDEO_HEVCLevel61",           (int)HEVCLevel61 },
    { (OMX_STRING)"VIDEO_HEVCLevel62",           (int)HEVCLevel62 },
    { (OMX_STRING)"VIDEO_MPEG4Level0",          (int)MPEG4Level0 },
    { (OMX_STRING)"VIDEO_MPEG4Level0b",          (int)MPEG4Level0b },
    { (OMX_STRING)"VIDEO_MPEG4Level10",          (int)MPEG4Level1 },
    { (OMX_STRING)"VIDEO_MPEG4Level20",          (int)MPEG4Level2 },
    { (OMX_STRING)"VIDEO_MPEG4Level30",          (int)MPEG4Level3 },
    { (OMX_STRING)"VIDEO_MPEG4Level3b",          (int)MPEG4Level3b },
    { (OMX_STRING)"VIDEO_MPEG4Level40",          (int)MPEG4Level4 },
    { (OMX_STRING)"VIDEO_MPEG4Level4a",          (int)MPEG4Level4a },
    { (OMX_STRING)"VIDEO_MPEG4Level50",          (int)MPEG4Level5 },
    { (OMX_STRING)"VP8_VERSION_0",               (int)VP8Version0 },
    { (OMX_STRING)"VP8_VERSION_1",               (int)VP8Version1 },
    { (OMX_STRING)"VP8_VERSION_2",               (int)VP8Version2 },
    { (OMX_STRING)"VP8_VERSION_3",               (int)VP8Version3 },
    { (OMX_STRING)"VIDEO_TMELevelInteger",      (int)TMELevelInteger },
    { 0, 0 }
};

// Rate control flavors
static ConfigEnum g_pXmlOmxRcTypeMap[] =
{
    { (OMX_STRING)"RC_OFF",  (int)OMX_Video_ControlRateDisable },
    { (OMX_STRING)"VBR_VFR", (int)OMX_Video_ControlRateVariableSkipFrames },       // Camcorder
    { (OMX_STRING)"VBR_CFR", (int)OMX_Video_ControlRateVariable },                 // Camcorder
    { (OMX_STRING)"CBR_VFR", (int)OMX_Video_ControlRateConstantSkipFrames },       // QVP
    { (OMX_STRING)"CBR_CFR", (int)OMX_Video_ControlRateConstant },                 // WFD
    { (OMX_STRING)"MBR_CFR", (int)QOMX_Video_ControlRateMaxBitrate },              // IOT
    { (OMX_STRING)"MBR_VFR", (int)QOMX_Video_ControlRateMaxBitrateSkipFrames },    // IOT
    { 0, 0 }
};

// Resync marker types
static ConfigEnum g_pXmlOmxResyncMarkerType[] =
{
    { (OMX_STRING)"SLICE_OFF",        (int)RESYNC_MARKER_NONE },
    { (OMX_STRING)"BYTE_MODE",        (int)RESYNC_MARKER_BITS },
    { (OMX_STRING)"MB_MODE",          (int)RESYNC_MARKER_MB },
    { (OMX_STRING)"GOB_MODE",         (int)RESYNC_MARKER_GOB },
    { 0, 0 }
};

static ConfigString g_pXmlOmxCodecExtensionsMap[] =
{
    { (OMX_STRING)"VIDEO_CodingMPEG4",   (OMX_STRING)".m4v" },
    { (OMX_STRING)"VIDEO_CodingH263",  (OMX_STRING)".263" },
    { (OMX_STRING)"VIDEO_CodingAVC",  (OMX_STRING)".264" },
    { (OMX_STRING)"VIDEO_CodingVP8",   (OMX_STRING)".ivf" },
    { (OMX_STRING)"VIDEO_CodingVC1",   (OMX_STRING)".vc1" },
    { (OMX_STRING)"VIDEO_CodingDIVX",  (OMX_STRING)".m4v" },
    { (OMX_STRING)"VIDEO_CodingMPEG2", (OMX_STRING)".mp2" },
    { (OMX_STRING)"VIDEO_CodingHEVC",  (OMX_STRING)".265" },
    { (OMX_STRING)"VIDEO_CodingVP9",   (OMX_STRING)".ivf" },
    { (OMX_STRING)"VIDEO_CodingTME",   (OMX_STRING)".tme" },
    { 0, (OMX_STRING)"." }
};

static ConfigEnum g_pXmlOmxExtradataTypeMap[] = {
    { (OMX_STRING)"Interlace",         (OMX_U32) InterlaceExtraData },
    { (OMX_STRING)"AspectRatio",       (OMX_U32) AspectRatioExtraData },
    { (OMX_STRING)"FrameRate",         (OMX_U32) FrameRateExtraData },
    { (OMX_STRING)"NumConcealMB",      (OMX_U32) ConcealMBExtraData },
    { (OMX_STRING)"RecoveryPointSEI",  (OMX_U32) RecoveryPointSEIExtraData },
    { (OMX_STRING)"PanscanWindow",     (OMX_U32) PanScanWindowExtraData },
    { (OMX_STRING)"UserData",          (OMX_U32) UserDataExtraData },
    { (OMX_STRING)"S3DFramePacking",   (OMX_U32) S3DFramePackingExtraData },
    { (OMX_STRING)"TimeStamp",         (OMX_U32) TimeStampExtraData },
    { (OMX_STRING)"VPX_ColorSpaceInfo",(OMX_U32) ColorAspectsInfo },
    { (OMX_STRING)"H264_VUI_SeqDispInfo",(OMX_U32) ColorAspectsInfo },
    { (OMX_STRING)"HEVC_VUI_SeqDispInfo",(OMX_U32) ColorAspectsInfo },
    { (OMX_STRING)"MPEG2_SeqDispInfo",(OMX_U32) ColorAspectsInfo },
    { (OMX_STRING)"FRAME_QP",          (OMX_U32) FrameQPExtraData },
    { 0, 0 }
};

Config::Config() {

    static const OMX_STRING pInFileName = (OMX_STRING)"";

    // set some default values
    memset(&m_sCodecConfig, 0, sizeof(m_sCodecConfig));
    m_sCodecConfig.eCodec = OMX_VIDEO_CodingAVC;
    m_sCodecConfig.eFileType = FILE_TYPE_ARBITRARY_BYTES;
    m_sCodecConfig.eCodecProfile = MPEG4ProfileSimple;
    m_sCodecConfig.eCodecLevel = DefaultLevel;
    m_sCodecConfig.eControlRate = OMX_Video_ControlRateDisable;
    m_sCodecConfig.nFrameWidth = 640;
    m_sCodecConfig.nFrameHeight = 480;
    m_sCodecConfig.nOutputFrameWidth = m_sCodecConfig.nFrameWidth;
    m_sCodecConfig.nOutputFrameHeight = m_sCodecConfig.nFrameHeight;
    m_sCodecConfig.nDVSXOffset = 0;
    m_sCodecConfig.nDVSYOffset = 0;
    m_sCodecConfig.nBitrate = INVALID_VALUE;
    m_sCodecConfig.nFramerate = 30;
    m_sCodecConfig.nTimeIncRes = 30;
    m_sCodecConfig.nRotation = 0;
    m_sCodecConfig.nMirror = 0;
    m_sCodecConfig.nInBufferCount = 3;
    m_sCodecConfig.nOutBufferCount = 3;
    m_sCodecConfig.nHECInterval = 0;
    m_sCodecConfig.bEnableSliceDeliveryMode = OMX_FALSE;
    m_sCodecConfig.nResyncMarkerSpacing = 0;
    m_sCodecConfig.eResyncMarkerType = RESYNC_MARKER_NONE;
    m_sCodecConfig.nIntraRefreshMBCount = 0;
    m_sCodecConfig.nFrames = INT_MAX;
    m_sCodecConfig.bEnableShortHeader = OMX_FALSE;
    m_sCodecConfig.nIntraPeriod = m_sCodecConfig.nFramerate * 2;
    m_sCodecConfig.nMinQp = INVALID_VALUE;
    m_sCodecConfig.nMaxQp = INVALID_VALUE;
    m_sCodecConfig.nIDRPeriod = INVALID_VALUE;
    m_sCodecConfig.nBFrames = 0xDEAD;
    m_sCodecConfig.nPFrames = 0;
    m_sCodecConfig.bProfileMode = OMX_FALSE;
    m_sCodecConfig.nCABAC = 0xDEAD;
    m_sCodecConfig.nDeblocker = INVALID_VALUE;
    m_sCodecConfig.id = 0;
    m_sCodecConfig.cancel_flag = 1;
    m_sCodecConfig.type = 0;
    m_sCodecConfig.quincunx_sampling_flag = 0;
    m_sCodecConfig.content_interpretation_type = 0;
    m_sCodecConfig.spatial_flipping_flag = 0;
    m_sCodecConfig.frame0_flipped_flag = 0;
    m_sCodecConfig.field_views_flag = 0;
    m_sCodecConfig.current_frame_is_frame0_flag = 0;
    m_sCodecConfig.frame0_self_contained_flag = 0;
    m_sCodecConfig.frame1_self_contained_flag = 0;
    m_sCodecConfig.frame0_grid_position_x = 0;
    m_sCodecConfig.frame0_grid_position_y = 0;
    m_sCodecConfig.frame1_grid_position_x = 0;
    m_sCodecConfig.frame1_grid_position_y = 0;
    m_sCodecConfig.reserved_byte = 0;
    m_sCodecConfig.repetition_period = 0;
    m_sCodecConfig.extension_flag = 0;
    m_sCodecConfig.nLTRMode = 0;
    m_sCodecConfig.nLTRCount = 0;
    m_sCodecConfig.nLTRPeriod = 0;
    m_sCodecConfig.nPriority = 0xDEAD;
    m_sCodecConfig.nLowLatency = 0xDEAD;
    m_sCodecConfig.m_nPrefixHeaderMode = 0xDEAD;
    m_sCodecConfig.bSecureSession = OMX_FALSE;
    m_sCodecConfig.bMdpFrameSource = OMX_FALSE;
    m_sCodecConfig.bInsertInbandVideoHeaders = OMX_FALSE;
    m_sCodecConfig.bInsertAUDelimiters = OMX_FALSE;
    m_sCodecConfig.ePlaybackMode = DynamicBufferMode;
    m_sCodecConfig.nAdaptiveWidth = m_sCodecConfig.nFrameWidth;
    m_sCodecConfig.nAdaptiveHeight = m_sCodecConfig.nFrameHeight;
    m_sCodecConfig.bRotateDisplay = OMX_FALSE;
    m_sCodecConfig.eDecoderPictureOrder = QOMX_VIDEO_DISPLAY_ORDER;
    m_sCodecConfig.eSinkType = Unknown_Sink;
    m_sCodecConfig.bMetaMode = OMX_TRUE;
    m_sCodecConfig.eMetaBufferType = CameraSource;
    m_sCodecConfig.eYuvColorSpace = ITUR601;
    m_sCodecConfig.nIDRPeriod = m_sCodecConfig.nIntraPeriod;
    m_sCodecConfig.bDownScalar = OMX_FALSE;
    m_sCodecConfig.ePostProcType = DefaultMemcopy;
    m_sCodecConfig.nInputColorFormat = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
    m_sCodecConfig.nOutputColorFormat = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m;
    m_sCodecConfig.nDropHierPLayer = -1;
    m_sCodecConfig.bDeinterlace = OMX_FALSE;
    m_sCodecConfig.nCurrentTestNum = 0;
    m_sCodecConfig.nPictureTypeDecode = 0;
    m_sCodecConfig.nMaxHPLayers = 0;
    m_sCodecConfig.nRectLeft = 0;
    m_sCodecConfig.nRectTop = 0;
    m_sCodecConfig.nRectWidth = 0;
    m_sCodecConfig.nRectHeight = 0;
    m_sCodecConfig.nBaseLayerId = 0;
    m_sCodecConfig.nConfigQp = 0;
    m_sCodecConfig.nSarWidth = 0;
    m_sCodecConfig.nSarHeight = 0;
    m_sCodecConfig.bForceCompressedForDPB = OMX_FALSE;
    m_sCodecConfig.nBlurWidth = 0;
    m_sCodecConfig.nBlurHeight = 0;
    m_sCodecConfig.pExtradataProperties=NULL;
    m_sCodecConfig.cCrcFileName[0] = '\0';
    m_sCodecConfig.cMISRFileName[0] = '\0';
    m_sCodecConfig.nHierPNumLayers = 1;
    m_sCodecConfig.nMaxHPLayers = 1;
    m_sCodecConfig.sHybridHP.nHpLayers = 1;
    m_sCodecConfig.sColorAspects.mRange = android::ColorAspects::Range::RangeUnspecified;
    m_sCodecConfig.sColorAspects.mPrimaries = android::ColorAspects::Primaries::PrimariesUnspecified;
    m_sCodecConfig.sColorAspects.mTransfer = android::ColorAspects::Transfer::TransferUnspecified;
    m_sCodecConfig.sColorAspects.mMatrixCoeffs = android::ColorAspects::MatrixCoeffs::MatrixUnspecified;
    // default dynamic config
    m_sDynamicConfig.nIFrameRequestPeriod = 0;
    m_sDynamicConfig.nUpdatedBitrate = m_sCodecConfig.nBitrate;
    m_sDynamicConfig.nUpdatedFramerate = m_sCodecConfig.nFramerate;
    m_sDynamicConfig.nUpdatedFrames = m_sCodecConfig.nFrames;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Config::~Config() {
    if(m_sCodecConfig.pExtradataProperties)
    delete[] m_sCodecConfig.pExtradataProperties;
    if(m_pDynamicProperties)
    delete[] m_pDynamicProperties;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Config::Parse(OMX_STRING pFileName,
        CodecConfigType *pCodecConfig, DynamicConfigType *pDynamicConfig) {

    static const OMX_S32 maxLineLen = 256;
    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_S32 nLine = 0;
    ParserStrVector v;
    char pBuf[maxLineLen];
    char *pTrimmed;
    File f;

    result = f.Open(pFileName, OMX_TRUE);

    if (result != OMX_ErrorNone) {
        VTEST_MSG_ERROR("error opening file");
        return OMX_ErrorBadParameter;
    }

    while (Parser::ReadLine(&f, maxLineLen, pBuf) != -1) {

        OMX_S32 nTok = 0;
        (void)Parser::RemoveComments(pBuf);
        pTrimmed = Parser::Trim(pBuf);
        if (strlen(pTrimmed) != 0) nTok = Parser::TokenizeString(&v, pTrimmed, (OMX_STRING)"\t =");
        VTEST_MSG_LOW("ntok = %d", (int)nTok);

        switch (v.size()) {

        case 0: // blank line
            break;
        case 1: // use default value
            break;
        case 2: // user has a preferred config

            ///////////////////////////////////////////
            ///////////////////////////////////////////
            // Codec config
            ///////////////////////////////////////////
            ///////////////////////////////////////////
            if (Parser::StringICompare((OMX_STRING)"FrameWidth", v[0]) == 0) {
                m_sCodecConfig.nFrameWidth = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FrameWidth = %d", (int)m_sCodecConfig.nFrameWidth);
            } else if (Parser::StringICompare((OMX_STRING)"FrameHeight", v[0]) == 0) {
                m_sCodecConfig.nFrameHeight = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FrameHeight = %d", (int)m_sCodecConfig.nFrameHeight);
            } else if (Parser::StringICompare((OMX_STRING)"OutputFrameWidth", v[0]) == 0) {
                m_sCodecConfig.nOutputFrameWidth = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("OutputFrameWidth = %d", (int)m_sCodecConfig.nOutputFrameWidth);
            } else if (Parser::StringICompare((OMX_STRING)"OutputFrameHeight", v[0]) == 0) {
                m_sCodecConfig.nOutputFrameHeight = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("OutputFrameHeight = %d", (int)m_sCodecConfig.nOutputFrameHeight);
            } else if (Parser::StringICompare((OMX_STRING)"DVSXOffset", v[0]) == 0) {
                m_sCodecConfig.nDVSXOffset = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("DVSXOffset = %d", (int)m_sCodecConfig.nDVSXOffset);
            } else if (Parser::StringICompare((OMX_STRING)"DVSYOffset", v[0]) == 0) {
                m_sCodecConfig.nDVSYOffset = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("DVSYOffset = %d", (int)m_sCodecConfig.nDVSYOffset);
            } else if (Parser::StringICompare((OMX_STRING)"Rotation", v[0]) == 0) {
                m_sCodecConfig.nRotation = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("Rotation = %d", (int)m_sCodecConfig.nRotation);
            } else if (Parser::StringICompare((OMX_STRING)"FPS", v[0]) == 0) {
                m_sCodecConfig.nFramerate = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FPS = %d", (int)m_sCodecConfig.nFramerate);
            } else if (Parser::StringICompare((OMX_STRING)"Bitrate", v[0]) == 0) {
                m_sCodecConfig.nBitrate = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("Bitrate = %d", (int) m_sCodecConfig.nBitrate);
            } else if (Parser::StringICompare((OMX_STRING)"RC", v[0]) == 0) {
                m_sCodecConfig.eControlRate =
                    (OMX_VIDEO_CONTROLRATETYPE)ParseEnum(g_pVencRCEnums, v[1]);
                VTEST_MSG_LOW("RC = %d", m_sCodecConfig.eControlRate);
            } else if (Parser::StringICompare((OMX_STRING)"Codec", v[0]) == 0) {
                m_sCodecConfig.eCodec = (OMX_VIDEO_CODINGTYPE)ParseEnum(g_pCodecEnums, v[1]);
                VTEST_MSG_LOW("Codec = %d", m_sCodecConfig.eCodec);
            } else if (Parser::StringICompare((OMX_STRING)"FileType", v[0]) == 0) {
                m_sCodecConfig.eFileType = (FileType)atoi(v[1]);
                VTEST_MSG_LOW("FileType = %d", m_sCodecConfig.eFileType);
            } else if (Parser::StringICompare((OMX_STRING)"Profile", v[0]) == 0) {
                m_sCodecConfig.eCodecProfile = (CodecProfileType)ParseEnum(g_pCodecProfileType, v[1]);
                VTEST_MSG_LOW("Profile = %d", m_sCodecConfig.eCodecProfile);
            } else if (Parser::StringICompare((OMX_STRING)"Level", v[0]) == 0) {
                m_sCodecConfig.eCodecLevel = (CodecLevelType)ParseEnum(g_pXmlOmxLevelMap, v[1]);
                VTEST_MSG_LOW("Level = %d", m_sCodecConfig.eCodecLevel);
            } else if (Parser::StringICompare((OMX_STRING)"InBufferCount", v[0]) == 0) {
                m_sCodecConfig.nInBufferCount = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("InBufferCount = %d", (int)m_sCodecConfig.nInBufferCount);
            } else if (Parser::StringICompare((OMX_STRING)"OutBufferCount", v[0]) == 0) {
                m_sCodecConfig.nOutBufferCount = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("OutBufferCount = %d", (int)m_sCodecConfig.nOutBufferCount);
            } else if (Parser::StringICompare((OMX_STRING)"HECInterval", v[0]) == 0) {
                m_sCodecConfig.nHECInterval = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("HECInterval = %d\n", (int)m_sCodecConfig.nHECInterval);
            } else if (Parser::StringICompare((OMX_STRING)"EnableSliceDeliveryMode", v[0]) == 0) {
                m_sCodecConfig.bEnableSliceDeliveryMode = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("EnableSliceDeliveryMode = %d\n", (int)m_sCodecConfig.bEnableSliceDeliveryMode);
            } else if (Parser::StringICompare((OMX_STRING)"ResyncMarkerSpacing", v[0]) == 0) {
                m_sCodecConfig.nResyncMarkerSpacing = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("ResyncMarkerSpacing = %d\n", (int)m_sCodecConfig.nResyncMarkerSpacing);
            } else if (Parser::StringICompare((OMX_STRING)"ResyncMarkerType", v[0]) == 0) {
                m_sCodecConfig.eResyncMarkerType = (ResyncMarkerType)ParseEnum(g_pResyncMarkerType, v[1]);
                VTEST_MSG_LOW("ResyncMarkerType = %d\n", (int)m_sCodecConfig.eResyncMarkerType);
            } else if (Parser::StringICompare((OMX_STRING)"IntraRefreshMBCount", v[0]) == 0) {
                m_sCodecConfig.nIntraRefreshMBCount = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("IntraRefreshMBCount = %d\n", (int)m_sCodecConfig.nIntraRefreshMBCount);
            } else if (Parser::StringICompare((OMX_STRING)"TimeIncRes", v[0]) == 0) {
                m_sCodecConfig.nTimeIncRes = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("TimeIncRes = %d\n", (int)m_sCodecConfig.nTimeIncRes);
            } else if (Parser::StringICompare((OMX_STRING)"EnableShortHeader", v[0]) == 0) {
                m_sCodecConfig.bEnableShortHeader = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("EnableShortHeader = %d\n", (int)m_sCodecConfig.bEnableShortHeader);
            } else if (Parser::StringICompare((OMX_STRING)"IntraPeriod", v[0]) == 0) {
                m_sCodecConfig.nIntraPeriod = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("IntraPeriod = %d\n", (int)m_sCodecConfig.nIntraPeriod);
            } else if (Parser::StringICompare((OMX_STRING)"InFile", v[0]) == 0) {
                memcpy(m_sCodecConfig.cInFileName, v[1], (strlen(v[1]) + 1));
                VTEST_MSG_LOW("InFile = %s\n", m_sCodecConfig.cInFileName);
            } else if (Parser::StringICompare((OMX_STRING)"OutFile", v[0]) == 0) {
                memcpy(m_sCodecConfig.cOutFileName, v[1], (strlen(v[1]) + 1));
                VTEST_MSG_LOW("OutFile = %s\n", m_sCodecConfig.cOutFileName);
            } else if (Parser::StringICompare((OMX_STRING)"NumFrames", v[0]) == 0) {
                m_sCodecConfig.nFrames = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("NumFrames = %d", (int)m_sCodecConfig.nFrames);
            } else if (Parser::StringICompare((OMX_STRING)"MinQp", v[0]) == 0) {
                m_sCodecConfig.nMinQp = (OMX_S32)atoi(v[1]);
                m_sCodecConfig.minQP_Packed = (m_sCodecConfig.nMinQp & 0xFF) | ((m_sCodecConfig.nMinQp & 0xFF) << 8) |
                                        ((m_sCodecConfig.nMinQp & 0xFF) << 16);
                VTEST_MSG_LOW("MinQp = %d", (int)m_sCodecConfig.nMinQp);
                VTEST_MSG_LOW("MinQpPacked = %d", (int)m_sCodecConfig.minQP_Packed);
            } else if (Parser::StringICompare((OMX_STRING)"MaxQp", v[0]) == 0) {
                m_sCodecConfig.nMaxQp = (OMX_S32)atoi(v[1]);
                m_sCodecConfig.maxQP_Packed = (m_sCodecConfig.nMaxQp & 0xFF) | ((m_sCodecConfig.nMaxQp & 0xFF) << 8) |
                                        ((m_sCodecConfig.nMaxQp & 0xFF) << 16);
                VTEST_MSG_LOW("MaxQp = %d", (int)m_sCodecConfig.nMaxQp);
                VTEST_MSG_LOW("MaxQpPacked = %d", (int)m_sCodecConfig.maxQP_Packed);
            } else if (Parser::StringICompare((OMX_STRING)"CABAC", v[0]) == 0) {
                m_sCodecConfig.nCABAC = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("CABAC = %d", m_sCodecConfig.nCABAC);
            } else if (Parser::StringICompare((OMX_STRING)"Deblock", v[0]) == 0) {
                m_sCodecConfig.nDeblocker = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("Deblock = %d", (int)m_sCodecConfig.nDeblocker);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackId", v[0]) == 0) {
                m_sCodecConfig.id = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackId = %d", (int)m_sCodecConfig.id);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackCancelFlag", v[0]) == 0) {
                m_sCodecConfig.cancel_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackCancelFlag = %d", (int)m_sCodecConfig.cancel_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackType", v[0]) == 0) {
                m_sCodecConfig.type = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackType = %d", (int)m_sCodecConfig.type);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackQuincunxSamplingFlag", v[0]) == 0) {
                m_sCodecConfig.quincunx_sampling_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackQuincunxSamplingFlag = %d", (int)m_sCodecConfig.quincunx_sampling_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackContentInterpretationType", v[0]) == 0) {
                m_sCodecConfig.content_interpretation_type = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackContentInterpretationType = %d", (int)m_sCodecConfig.content_interpretation_type);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackSpatialFlippingFlag", v[0]) == 0) {
                m_sCodecConfig.spatial_flipping_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackSpatialFlippingFlag = %d", (int)m_sCodecConfig.spatial_flipping_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame0FlippedFlag", v[0]) == 0) {
                m_sCodecConfig.frame0_flipped_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame0FlippedFlag = %d", (int)m_sCodecConfig.frame0_flipped_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFieldViewsFlag", v[0]) == 0) {
                m_sCodecConfig.field_views_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFieldViewsFlag = %d", (int)m_sCodecConfig.field_views_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackCurrentFrameIsFrame0Flag", v[0]) == 0) {
                m_sCodecConfig.current_frame_is_frame0_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackCurrentFrameIsFrame0Flag = %d", (int)m_sCodecConfig.current_frame_is_frame0_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame0SelfContainedFlag", v[0]) == 0) {
                m_sCodecConfig.frame0_self_contained_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame0SelfContainedFlag = %d", (int)m_sCodecConfig.frame0_self_contained_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame1SelfContainedFlag", v[0]) == 0) {
                m_sCodecConfig.frame1_self_contained_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame1SelfContainedFlag = %d", (int)m_sCodecConfig.frame1_self_contained_flag);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame0GridPositionX", v[0]) == 0) {
                m_sCodecConfig.frame0_grid_position_x = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame0GridPositionX = %d", (int)m_sCodecConfig.frame0_grid_position_x);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame0GridPositionY", v[0]) == 0) {
                m_sCodecConfig.frame0_grid_position_y = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame0GridPositionY = %d", (int)m_sCodecConfig.frame0_grid_position_y);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame1GridPositionX", v[0]) == 0) {
                m_sCodecConfig.frame1_grid_position_x = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame1GridPositionX = %d", (int)m_sCodecConfig.frame1_grid_position_x);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackFrame1GridPositionY", v[0]) == 0) {
                m_sCodecConfig.frame1_grid_position_y = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackFrame1GridPositionY = %d", (int)m_sCodecConfig.frame1_grid_position_y);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackReservedByte", v[0]) == 0) {
                m_sCodecConfig.reserved_byte = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackReservedByte = %d", (int)m_sCodecConfig.reserved_byte);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackRepetitionPeriod", v[0]) == 0) {
                m_sCodecConfig.repetition_period = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackRepetitionPeriod = %d", (int)m_sCodecConfig.repetition_period);
            } else if (Parser::StringICompare((OMX_STRING)"FramePackExtensionFlag", v[0]) == 0) {
                m_sCodecConfig.extension_flag = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("FramePackExtensionFlag = %d", (int)m_sCodecConfig.extension_flag);
            } else if (Parser::StringICompare((OMX_STRING)"LTRMode", v[0]) == 0) {
                m_sCodecConfig.nLTRMode = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("LTRMode = %d", (int)m_sCodecConfig.nLTRMode);
            } else if (Parser::StringICompare((OMX_STRING)"LTRCount", v[0]) == 0) {
                m_sCodecConfig.nLTRCount = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("LTRCount = %d", (int)m_sCodecConfig.nLTRCount);
            } else if (Parser::StringICompare((OMX_STRING)"LTRPeriod", v[0]) == 0) {
                m_sCodecConfig.nLTRPeriod = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("LTRPeriod = %d", (int)m_sCodecConfig.nLTRPeriod);
            } else if (Parser::StringICompare((OMX_STRING)"Extradata", v[0]) == 0) {
                m_sCodecConfig.bExtradata = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("Extradata = %d", m_sCodecConfig.bExtradata);
            } else if (Parser::StringICompare((OMX_STRING)"IDRPeriod", v[0]) == 0) {
                m_sCodecConfig.nIDRPeriod = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("Extradata = %d", (int)m_sCodecConfig.nIDRPeriod);
            } else if (Parser::StringICompare((OMX_STRING) "HierPNumLayers", v[0]) == 0) {
                m_sCodecConfig.nHierPNumLayers = (OMX_U32) atoi(v[1]);
                VTEST_MSG_LOW("HierPNumLayers = %d", m_sCodecConfig.nHierPNumLayers);
            } else if (Parser::StringICompare((OMX_STRING) "DropHierPLayer", v[0]) == 0) {
                m_sCodecConfig.nDropHierPLayer = (OMX_S32) atoi(v[1]);
                VTEST_MSG_LOW("DropHierPLayer = %d", m_sCodecConfig.nDropHierPLayer);
            } else if (Parser::StringICompare((OMX_STRING)"Deinterlace", v[0]) == 0) {
                m_sCodecConfig.bDeinterlace = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("Deinterlace = %d", m_sCodecConfig.bDeinterlace);
            } else if (Parser::StringICompare((OMX_STRING) "PictureType", v[0]) == 0) {
                m_sCodecConfig.nPictureTypeDecode = (OMX_U32) atoi(v[1]);
                VTEST_MSG_LOW("PictureType = %d", m_sCodecConfig.nPictureTypeDecode);
            } else if (Parser::StringICompare((OMX_STRING)"MaxHPLayers", v[0]) == 0) {
                m_sCodecConfig.nMaxHPLayers = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("MaxHPLayers = %d", m_sCodecConfig.nMaxHPLayers);
            } else if (Parser::StringICompare((OMX_STRING)"RectLeft", v[0]) == 0) {
                m_sCodecConfig.nRectLeft = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("RectLeft = %d", m_sCodecConfig.nRectLeft);
            } else if (Parser::StringICompare((OMX_STRING)"RectTop", v[0]) == 0) {
                m_sCodecConfig.nRectTop = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("RectTop = %d", m_sCodecConfig.nRectTop);
            } else if (Parser::StringICompare((OMX_STRING)"RectWidth", v[0]) == 0) {
                m_sCodecConfig.nRectWidth = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("RectWidth = %d", m_sCodecConfig.nRectWidth);
            } else if (Parser::StringICompare((OMX_STRING)"RectHeight", v[0]) == 0) {
                m_sCodecConfig.nRectHeight = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("RectHeight = %d", m_sCodecConfig.nRectHeight);
            } else if (Parser::StringICompare((OMX_STRING)"BaseLayerID", v[0]) == 0) {
                m_sCodecConfig.nBaseLayerId = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("BaseLayerID = %d", m_sCodecConfig.nBaseLayerId);
            } else if (Parser::StringICompare((OMX_STRING)"ConfigQP", v[0]) == 0) {
                m_sCodecConfig.nConfigQp = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("ConfigQP = %d", m_sCodecConfig.nConfigQp);
            } else if (Parser::StringICompare((OMX_STRING)"SarWidth", v[0]) == 0) {
                m_sCodecConfig.nSarWidth = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("SarWidth = %lld", m_sCodecConfig.nSarWidth);
            } else if (Parser::StringICompare((OMX_STRING)"SarHeight", v[0]) == 0) {
                m_sCodecConfig.nSarHeight = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("SarHeight = %d", m_sCodecConfig.nSarHeight);
            } else if (Parser::StringICompare((OMX_STRING) "HybridHpLayers", v[0]) == 0) {
                Parser::ParseMultiLayers(&(m_sCodecConfig.sHybridHP), (OMX_STRING)v[1]);
            } else if (Parser::StringICompare((OMX_STRING)"RoiQpFile", v[0]) == 0) {
                memcpy(m_sCodecConfig.cRoiQpFileName, v[1], (strlen(v[1]) + 1));
                VTEST_MSG_LOW("RoiQpFile = %s\n", m_sCodecConfig.cRoiQpFileName);
            }
            ///////////////////////////////////////////
            ///////////////////////////////////////////
            // Dynamic config
            ///////////////////////////////////////////
            ///////////////////////////////////////////
            else if (Parser::StringICompare((OMX_STRING)"DynamicFile", v[0]) == 0) {
                memcpy(m_sCodecConfig.cDynamicFile, v[1], (strlen(v[1]) + 1));
                VTEST_MSG_LOW("DynamicFile");
                m_sCodecConfig.dynamic_config_array_size = Parser::CountLine(m_sCodecConfig.cDynamicFile);
                VTEST_MSG_MEDIUM("Allocating %u units of Dynamic Config",
                                     (unsigned int)m_sCodecConfig.dynamic_config_array_size);
                m_pDynamicProperties = new DynamicConfig[m_sCodecConfig.dynamic_config_array_size];
                for (OMX_U32 i = 0; i < m_sCodecConfig.dynamic_config_array_size; i++) {
                    memset(&m_pDynamicProperties[i], 0, sizeof(struct DynamicConfig));
                }

                OMX_S32 nLine = 0;

                ParserStrVector vec;
                char pBuffer[maxLineLen];
                char *pTrimmed;

                File dynamicFile;
                OMX_ERRORTYPE result = OMX_ErrorNone;
                result = dynamicFile.Open(m_sCodecConfig.cDynamicFile, OMX_TRUE);

                if (result != OMX_ErrorNone) {
                    VTEST_MSG_ERROR("Cannot open dynamic config file: %s", m_sCodecConfig.cDynamicFile);
                    return OMX_ErrorBadParameter;
                }

                while (Parser::ReadLine(&dynamicFile, maxLineLen, pBuffer) != -1) {
                    OMX_S32 nTok = 0;
                    (void)Parser::RemoveComments(pBuffer);
                    pTrimmed = Parser::Trim(pBuffer);
                    if (strlen(pTrimmed) != 0) nTok = Parser::TokenizeString(&vec, pTrimmed, (OMX_STRING)"\t =");

                    VTEST_MSG_LOW("ntok = %d, size: %d\n", (int)nTok, (int) vec.size());
                    switch (vec.size()) {
                    case 0: // blank line
                        break;
                    case 1: // use default value
                        break;
                    case 2:
                        if (Parser::StringICompare((OMX_STRING)"ivoprefresh", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32)atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = OMX_IndexConfigVideoIntraVOPRefresh;
                            m_pDynamicProperties[nLine].config_data.intravoprefresh.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                            m_pDynamicProperties[nLine].config_data.intravoprefresh.IntraRefreshVOP = OMX_TRUE;
                        } else if (Parser::StringICompare((OMX_STRING)"useltr", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32) atoi(vec[0]);
                            if (m_sCodecConfig.eCodec == OMX_VIDEO_CodingVP8) {
                                m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_IndexConfigVideoVp8ReferenceFrame;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.nPortIndex = (OMX_U32)PORT_INDEX_IN;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bUseGoldenFrame = OMX_TRUE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bGoldenFrameRefresh = OMX_FALSE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bAlternateFrameRefresh = OMX_FALSE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bPreviousFrameRefresh = OMX_FALSE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bUsePreviousFrame = OMX_FALSE;
                            } else {
                                m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigVideoLTRUse;
                                m_pDynamicProperties[nLine].config_data.useltr.nPortIndex = (OMX_U32)PORT_INDEX_IN;
                                m_pDynamicProperties[nLine].config_data.useltr.nFrames = 0;
                                m_pDynamicProperties[nLine].config_data.useltr.nID = 0;
                            }
                        } else if (Parser::StringICompare((OMX_STRING)"markltr", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32) atoi(vec[0]);
                            if (m_sCodecConfig.eCodec == OMX_VIDEO_CodingVP8) {
                                m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_IndexConfigVideoVp8ReferenceFrame;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.nPortIndex = (OMX_U32)PORT_INDEX_IN;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bUseGoldenFrame = OMX_FALSE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bGoldenFrameRefresh = OMX_TRUE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bAlternateFrameRefresh = OMX_FALSE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bPreviousFrameRefresh = OMX_FALSE;
                                m_pDynamicProperties[nLine].config_data.vp8refframe.bUsePreviousFrame = OMX_FALSE;
                            } else {
                                m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE) OMX_QcomIndexConfigVideoLTRMark;
                                m_pDynamicProperties[nLine].config_data.markltr.nPortIndex = (OMX_U32) PORT_INDEX_IN;
                                m_pDynamicProperties[nLine].config_data.markltr.nID = 0;
                            }
                        }
                        break;
                    case 3: // dynamic config
                        if (Parser::StringICompare((OMX_STRING)"bitrate", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32)atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = OMX_IndexConfigVideoBitrate;
                            m_pDynamicProperties[nLine].config_data.bitrate.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                            m_pDynamicProperties[nLine].config_data.bitrate.nEncodeBitrate = (OMX_S32)atoi(vec[2]);
                        } else if (Parser::StringICompare((OMX_STRING)"framerate", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32)atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = OMX_IndexConfigVideoFramerate;
                            m_pDynamicProperties[nLine].config_data.framerate.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                            m_pDynamicProperties[nLine].config_data.f_framerate = (OMX_S32)atoi(vec[2]);
                        } else if (Parser::StringICompare((OMX_STRING)"iperiod", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32)atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)QOMX_IndexConfigVideoIntraperiod;
                            m_pDynamicProperties[nLine].config_data.intraperiod.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                            m_pDynamicProperties[nLine].config_data.intraperiod.nPFrames = (OMX_U32)atoi(vec[2]) - 1;
                            m_pDynamicProperties[nLine].config_data.intraperiod.nBFrames = 0;
                            m_pDynamicProperties[nLine].config_data.intraperiod.nIDRPeriod = 1;
                        } else if (Parser::StringICompare((OMX_STRING)"useltr", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32) atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigVideoLTRUse;
                            m_pDynamicProperties[nLine].config_data.useltr.nPortIndex = (OMX_U32)PORT_INDEX_IN;
                            m_pDynamicProperties[nLine].config_data.useltr.nFrames = 0;
                            m_pDynamicProperties[nLine].config_data.useltr.nID = (OMX_U32)atoi(vec[2]);
                        } else if (Parser::StringICompare((OMX_STRING)"markltr", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32) atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigVideoLTRMark;
                            m_pDynamicProperties[nLine].config_data.markltr.nPortIndex = (OMX_U32)PORT_INDEX_IN;
                            m_pDynamicProperties[nLine].config_data.markltr.nID = (OMX_U32)atoi(vec[2]);
                        } else if (Parser::StringICompare((OMX_STRING) "PictureType", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32) atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigPictureTypeDecode;
                            m_pDynamicProperties[nLine].config_data.pictypedecode.eDecodeType =
                                (QOMX_VIDEO_PICTURE_TYPE_DECODE) atoi(vec[2]);
                        } else if (Parser::StringICompare((OMX_STRING) "ConfigQP", vec[1]) == 0) {
                            m_pDynamicProperties[nLine].frame_num = (OMX_S32) atoi(vec[0]);
                            m_pDynamicProperties[nLine].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigQp;
                            m_pDynamicProperties[nLine].config_data.configqp.nQP = (OMX_U32) atoi(vec[2]);
                        }
                        break;
                    default:
                        VTEST_MSG_ERROR("Error with dynamic config parsing line %d", (int)nLine);
                        break;
                    }
                    vec.clear();

                    ++nLine;
                }
                (void)dynamicFile.Close();

                m_sCodecConfig.pDynamicProperties = m_pDynamicProperties;
            } else if (Parser::StringICompare((OMX_STRING)"IFrameRequestPeriod", v[0]) == 0) {
                m_sDynamicConfig.nIFrameRequestPeriod = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("IFrameRequestPeriod = %d\n", (int)m_sDynamicConfig.nIFrameRequestPeriod);
            } else if (Parser::StringICompare((OMX_STRING)"UpdatedBitrate", v[0]) == 0) {
                m_sDynamicConfig.nUpdatedBitrate = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("UpdatedBitrate = %d\n", (int)m_sDynamicConfig.nUpdatedBitrate);
            } else if (Parser::StringICompare((OMX_STRING)"UpdatedFPS", v[0]) == 0) {
                m_sDynamicConfig.nUpdatedFramerate = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("UpdatedFPS = %d\n", (int)m_sDynamicConfig.nUpdatedFramerate);
            } else if (Parser::StringICompare((OMX_STRING)"UpdatedNumFrames", v[0]) == 0) {
                m_sDynamicConfig.nUpdatedFrames = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("UpdatedNumFrames = %d\n", (int)m_sDynamicConfig.nUpdatedFrames);
            } else if (Parser::StringICompare((OMX_STRING)"UpdatedIntraPeriod", v[0]) == 0) {
                m_sDynamicConfig.nUpdatedIntraPeriod = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("UpdatedIntraPeriod = %d\n",
                    (int)m_sDynamicConfig.nUpdatedIntraPeriod);
            } else if (Parser::StringICompare((OMX_STRING)"ProfileMode", v[0]) == 0) {
                m_sCodecConfig.bProfileMode = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("ProfileMode = %d\n", (int)m_sCodecConfig.bProfileMode);
            } else if (Parser::StringICompare((OMX_STRING)"SecureMode", v[0]) == 0) {
                m_sCodecConfig.bSecureSession = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("SecureMode = %d\n", (int)m_sCodecConfig.bSecureSession);
            } else if (Parser::StringICompare((OMX_STRING)"MDPFrameSource", v[0]) == 0) {
                m_sCodecConfig.bMdpFrameSource = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("MdpFrameSource = %d\n", (int)m_sCodecConfig.bMdpFrameSource);
            } else if (Parser::StringICompare((OMX_STRING)"InsertInbandVideoHeaders", v[0]) == 0) {
                m_sCodecConfig.bInsertInbandVideoHeaders = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("SPS/PPS I-frame headers enabled = %d",
                                  (int)m_sCodecConfig.bInsertInbandVideoHeaders);
            } else if (Parser::StringICompare((OMX_STRING)"InsertAUDelimiters", v[0]) == 0) {
                m_sCodecConfig.bInsertAUDelimiters = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("H264 AU delimiters enabled = %d",
                                  (int)m_sCodecConfig.bInsertAUDelimiters);
            } else if (Parser::StringICompare((OMX_STRING)"PlaybackMode", v[0]) == 0) {
                m_sCodecConfig.ePlaybackMode = (PlaybackModeType)ParseEnum(g_pPlaybackMode, v[1]);
                VTEST_MSG_LOW("PlaybackMode = %d", m_sCodecConfig.ePlaybackMode);
            } else if (Parser::StringICompare((OMX_STRING)"AdaptiveWidth", v[0]) == 0) {
                m_sCodecConfig.nAdaptiveWidth = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("AdaptiveWidth = %d", (int)m_sCodecConfig.nAdaptiveWidth);
            } else if (Parser::StringICompare((OMX_STRING)"AdaptiveHeight", v[0]) == 0) {
                m_sCodecConfig.nAdaptiveHeight = (OMX_S32)atoi(v[1]);
                VTEST_MSG_LOW("AdaptiveHeight = %d", (int)m_sCodecConfig.nAdaptiveHeight);
            } else if (Parser::StringICompare((OMX_STRING)"RotateDisplay", v[0]) == 0) {
                m_sCodecConfig.bRotateDisplay = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("RotateDisplay = %d\n", m_sCodecConfig.bRotateDisplay);
            } else if (Parser::StringICompare((OMX_STRING)"DecoderPictureOrder", v[0]) == 0) {
                m_sCodecConfig.eDecoderPictureOrder = (QOMX_VIDEO_PICTURE_ORDER)atoi(v[1]);
                VTEST_MSG_LOW("DecoderPictureOrder = %d\n", m_sCodecConfig.eDecoderPictureOrder);
            } else if (Parser::StringICompare((OMX_STRING)"SinkType", v[0]) == 0) {
                m_sCodecConfig.eSinkType = (SinkType)ParseEnum(g_pSinkType, v[1]);
                VTEST_MSG_LOW("SinkType = %d", m_sCodecConfig.eSinkType);
            } else if (Parser::StringICompare((OMX_STRING)"MetaMode", v[0]) == 0) {
                m_sCodecConfig.bMetaMode = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("MetaMode = %d", (int)m_sCodecConfig.bMetaMode);
            } else if (Parser::StringICompare((OMX_STRING)"Clamp_601to709", v[0]) == 0) {
                /* Adding this to match the legacy config option */
                OMX_BOOL bClamp_601To709 = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("Clamp_601To709 = %d", bClamp_601To709);
                m_sCodecConfig.eYuvColorSpace = ITUR709;
            } else if (Parser::StringICompare((OMX_STRING)"YuvColorSpace", v[0]) == 0) {
                m_sCodecConfig.eYuvColorSpace = (YuvColorSpace)ParseEnum(g_pYuvColorSpaceType, v[1]);
                VTEST_MSG_LOW("YuvColorSpace = %x", m_sCodecConfig.eYuvColorSpace);
            } else if (Parser::StringICompare((OMX_STRING)"PostProcType", v[0]) == 0) {
                m_sCodecConfig.ePostProcType = (PostProcType)ParseEnum(g_pPostProcType, v[1]);
                VTEST_MSG_LOW("PostProcType = %d", m_sCodecConfig.ePostProcType);
            } else if (Parser::StringICompare((OMX_STRING)"InputColorFormat", v[0]) == 0) {
                m_sCodecConfig.nInputColorFormat = (OMX_U32)ParseEnum(g_pColorFormat, v[1]);
                VTEST_MSG_LOW("nInputColorFormat = %d", m_sCodecConfig.nInputColorFormat);
            } else if (Parser::StringICompare((OMX_STRING)"OutputColorFormat", v[0]) == 0) {
                m_sCodecConfig.nOutputColorFormat = (OMX_U32)ParseEnum(g_pColorFormat, v[1]);
                VTEST_MSG_LOW("nOutputColorFormat = %d", m_sCodecConfig.nOutputColorFormat);
            } else if (Parser::StringICompare((OMX_STRING)"ForceCompressedForDPB", v[0]) == 0) {
                m_sCodecConfig.bForceCompressedForDPB = (OMX_BOOL)atoi(v[1]);
                VTEST_MSG_LOW("bForceCompressedForDPB = %d", m_sCodecConfig.bForceCompressedForDPB);
            } else if (Parser::StringICompare((OMX_STRING)"BlurWidth", v[0]) == 0) {
                m_sCodecConfig.nBlurWidth = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("nBlurWidth = %d", m_sCodecConfig.nBlurWidth);
            } else if (Parser::StringICompare((OMX_STRING)"BlurHeight", v[0]) == 0) {
                m_sCodecConfig.nBlurHeight = (OMX_U32)atoi(v[1]);
                VTEST_MSG_LOW("nBlurHeight = %d", m_sCodecConfig.nBlurHeight);
            } else {
                VTEST_MSG_ERROR("invalid config file key line %d", (int)nLine);
            }
            break;
        default:
            VTEST_MSG_ERROR("error with config parsing line %d", (int)nLine);
            break;
        }
        v.clear();

        ++nLine;
    }

    (void)f.Close();
    memcpy(pCodecConfig, &m_sCodecConfig, sizeof(m_sCodecConfig));
    if (pDynamicConfig != NULL) { // optional param
        memcpy(pDynamicConfig, &m_sDynamicConfig, sizeof(m_sDynamicConfig));
    }
    return result;
}


OMX_ERRORTYPE Config::ParseXml(VideoStaticProperties* pGlobalVideoProp,VideoSessionInfo* pSessionInfo,CodecConfigType *pStaticConfig, DynamicConfigType *pDynamicConfig) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    OMX_S32 nFPSNumerator = 0,nFPSDenominator = 0;
    OMX_S32 nBframes = 0, nPframes = 0, nIframes = 0,IsnBFramesPresent =0;
    char sessionMode[VTEST_MAX_STRING] = "dec";
    char encFileXtn[VTEST_MAX_STRING] = ".264";
    OMX_STRING pSessionType = sessionMode;
    xmlNodePtr pXmlNode = pSessionInfo->pXmlNode;
    OMX_S32 nDynamicPropCount = 0;
    OMX_S32 nDynamicPropAllocated = 0;
    const OMX_S32 nDynamicPropChunk = 10;

    OMX_S32 nExtradataPropCount = 0;
    OMX_S32 nExtradataPropAllocated = 0;
    const OMX_S32 nExtraDataPropChunk = 10;

    DynamicConfig *pDynamicProperties;
    m_pDynamicProperties  = new DynamicConfig[nDynamicPropChunk];
    nDynamicPropAllocated = nDynamicPropAllocated + nDynamicPropChunk;

    ExtradataConfig *pExtradataProperties;
    pExtradataProperties = new ExtradataConfig[nExtraDataPropChunk];
    nExtradataPropAllocated = nExtradataPropAllocated + nExtraDataPropChunk;

    if(!pGlobalVideoProp || !pXmlNode || !pStaticConfig || !pDynamicConfig) {
        VTEST_MSG_ERROR("Xml Parsing failed");
        return OMX_ErrorBadParameter;
    }
    if(!m_pDynamicProperties || !pExtradataProperties)
    {
        VTEST_MSG_ERROR("Memory Allocation failed!");
        return OMX_ErrorUndefined;
    }

    VTEST_MSG_HIGH("Started parsing session Xml related parameter");

    while (pXmlNode) {
        if(!pXmlNode->children) {
            pXmlNode = pXmlNode->next;
            continue;
        }

        OMX_STRING tagName = (OMX_STRING)pXmlNode->name;

        if (!STRCMP(tagName, "CompressionFormat")) {
            pStaticConfig->eCodec = (OMX_VIDEO_CODINGTYPE)ParseEnum(g_pXmlOmxCodecMap, (OMX_STRING)pXmlNode->children->content);
            char* strExtn = (char *)ParseMapString(g_pXmlOmxCodecExtensionsMap, (OMX_STRING)pXmlNode->children->content);
            if (strExtn)
                STRLCPY(&encFileXtn[0], strExtn, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: CompressionFormat: %s    eCodec: %d   fileXtn: %s", (OMX_STRING)pXmlNode->children->content,
                (OMX_S32)pStaticConfig->eCodec, (OMX_STRING)encFileXtn);
        } else if (!STRCMP(tagName, "ProfileIDC")) {
            pStaticConfig->eCodecProfile = (CodecProfileType)ParseEnum(g_pXmlOmxProfileMap, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: ProfileIDC: %s", (OMX_STRING)pXmlNode->children->content);
        } else if (!STRCMP(tagName, "eLevel")) {
            pStaticConfig->eCodecLevel = (CodecLevelType)ParseEnum(g_pXmlOmxLevelMap, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: CodecLevel: %s", (OMX_STRING)pXmlNode->children->content);
        } else if (!STRCMP(tagName, "eControlRate")) {
            pStaticConfig->eControlRate  = (OMX_VIDEO_CONTROLRATETYPE)ParseEnum(g_pXmlOmxRcTypeMap, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: eControlRate: %s eControlRate: %d", (OMX_STRING)pXmlNode->children->content,
                (OMX_S32)pStaticConfig->eControlRate);
        } else if (!STRCMP(tagName, "ColorFormat")) {
            pStaticConfig->nInputColorFormat  = (OMX_QCOM_COLOR_FORMATTYPE)ParseEnum(g_pColorFormat, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_HIGH("SessionXml: ColorFormat: %s ColorFormat: %x", (OMX_STRING)pXmlNode->children->content,
                                                                       (OMX_S32)pStaticConfig->nInputColorFormat);
            pStaticConfig->nOutputColorFormat = pStaticConfig->nInputColorFormat;
        } else if (!STRCMP(tagName, "IRMbs")) {
            pStaticConfig->nIntraRefreshMBCount = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: nIntraRefreshMBCount: %d", (OMX_S32)pStaticConfig->nIntraRefreshMBCount);
        } else if (!STRCMP(tagName, "EosType")) {
            pStaticConfig->nEosType = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: EosType: %d", (OMX_S32)pStaticConfig->nEosType);
        } else if (!STRCMP(tagName, "IRMode")) {
            pStaticConfig->nIRMode = (OMX_U32)ParseEnum(g_pIntraRefreshMode, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: IRMode: %d (%s)", pStaticConfig->nIRMode, (OMX_STRING)pXmlNode->children->content);
        } else if (!STRCMP(tagName, "FPSNumerator")) {
            nFPSNumerator = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: FPSNumerator: %d", (OMX_S32)nFPSNumerator);
        } else if (!STRCMP(tagName, "FPSDenominator")) {
            nFPSDenominator = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: FPSDenominator: %d", (OMX_S32)nFPSDenominator);
        } else if (!STRCMP(tagName, "SourceWidth")) {
            pStaticConfig->nFrameWidth = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: SourceWidth: %d", (OMX_S32)pStaticConfig->nFrameWidth);
        } else if (!STRCMP(tagName, "SourceHeight")) {
            pStaticConfig->nFrameHeight = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: SourceHeight: %d", (OMX_S32)pStaticConfig->nFrameHeight);
        } else if (!STRCMP(tagName, "OutputWidth")) {
            pStaticConfig->nOutputFrameWidth = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: OutputWidth: %d", (OMX_S32)pStaticConfig->nOutputFrameWidth);
        } else if (!STRCMP(tagName, "OutputHeight")) {
            pStaticConfig->nOutputFrameHeight = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: OutputHeight: %d", (OMX_S32)pStaticConfig->nOutputFrameHeight);
        } else if (!STRCMP(tagName, "BlurWidth")) {
            pStaticConfig->nBlurWidth = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: BlurWidth: %d", (OMX_S32)pStaticConfig->nBlurWidth);
        } else if (!STRCMP(tagName, "BlurHeight")) {
            pStaticConfig->nBlurHeight = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: BlurHeight: %d", (OMX_S32)pStaticConfig->nBlurHeight);
        } else if (!STRCMP(tagName, "Bitrate")) {
            pStaticConfig->nBitrate = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Bitrate: %d", (OMX_S32)pStaticConfig->nBitrate);
        } else if (!STRCMP(tagName, "Rotation")) {
            pStaticConfig->nRotation = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Rotation: %d", (OMX_S32)pStaticConfig->nRotation);
        } else if (!STRCMP(tagName, "Flip")) {
            pStaticConfig->nMirror = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Flip: %d", (OMX_S32)pStaticConfig->nMirror);
        } else if (!STRCMP(tagName, "InputFile")) {
            STRLCPY(pStaticConfig->cInFileName, (OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: InputFile: %s", pStaticConfig->cInFileName);
        } else if (!STRCMP(tagName, "CrcFile")) {
            STRLCPY(pStaticConfig->cCrcFileName, (OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: CrcFile: %s", pStaticConfig->cCrcFileName);
        } else if (!STRCMP(tagName, "MisrFile")) {
            STRLCPY(pStaticConfig->cMISRFileName, (OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: MisrFile: %s", pStaticConfig->cMISRFileName);
        } else if (!STRCMP(tagName, "TestCaseID")) {
            STRLCPY(pStaticConfig->cOutFileName, (OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            STRLCPY(pSessionInfo->TestId,(OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: OutputFile: %s", pStaticConfig->cOutFileName);
        } else if (!STRCMP(tagName, "nIframes")) {
            nIframes = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: nIframes: %d", nIframes);
        } else if (!STRCMP(tagName, "nBframes")) {
            nBframes = atoi((OMX_STRING)pXmlNode->children->content);
            IsnBFramesPresent = 1;
            VTEST_MSG_LOW("SessionXml: nBframes: %d", nBframes);
        } else if (!STRCMP(tagName, "nPframes")) {
            nPframes = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: nPframes: %d", nPframes);
        } else if (!STRCMP(tagName, "nFramesToParse")) {
            pStaticConfig->nFrames = atoi((OMX_STRING)pXmlNode->children->content);
            pStaticConfig->nFrames = (pStaticConfig->nFrames <= 0)?INT_MAX : pStaticConfig->nFrames;
            VTEST_MSG_LOW("SessionXml: nFramesToParse: %d", (OMX_S32)pStaticConfig->nFrames);
        } else if (!STRCMP(tagName, "MinQP")) {
            pStaticConfig->nMinQp = atoi((OMX_STRING)pXmlNode->children->content);
            pStaticConfig->minQP_Packed = (pStaticConfig->nMinQp & 0xFF) | ((pStaticConfig->nMinQp & 0xFF) << 8) |
                ((pStaticConfig->nMinQp & 0xFF) << 16);
            VTEST_MSG_LOW("SessionXml: MinQP: %d", (OMX_S32)pStaticConfig->nMinQp);
            VTEST_MSG_LOW("SessionXml: MinQPPacked: %d", (OMX_S32)pStaticConfig->minQP_Packed);
        } else if (!STRCMP(tagName, "MaxQP")) {
            pStaticConfig->nMaxQp = atoi((OMX_STRING)pXmlNode->children->content);
            pStaticConfig->maxQP_Packed = (pStaticConfig->nMaxQp & 0xFF) | ((pStaticConfig->nMaxQp & 0xFF) << 8) |
                ((pStaticConfig->nMaxQp & 0xFF) << 16);
            VTEST_MSG_LOW("SessionXml: MaxQP: %d", (OMX_S32)pStaticConfig->nMaxQp);
            VTEST_MSG_LOW("SessionXml: MaxQPPacked: %d", (OMX_S32)pStaticConfig->maxQP_Packed);
        } else if (!STRCMP(tagName, "session_mode")) {
            strlcpy(pSessionType ,(OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: session_mode: %s", pSessionType);
        } else if (!STRCMP(tagName, "EnableSliceDeliveryMode")) {
            pStaticConfig->bEnableSliceDeliveryMode = (OMX_BOOL)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: EnableSliceDeliveryMode: %d", pStaticConfig->bEnableSliceDeliveryMode);
        } else if (!STRCMP(tagName, "SliceMode")) {
            pStaticConfig->eResyncMarkerType = (ResyncMarkerType)ParseEnum(g_pXmlOmxResyncMarkerType, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: SliceMode: %s    eResyncMarkerType: %d", (OMX_STRING)pXmlNode->children->content,
                (OMX_S32)pStaticConfig->eResyncMarkerType);
        } else if (!STRCMP(tagName, "SliceSize")) {
            pStaticConfig->nResyncMarkerSpacing = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: SliceSize: %d", pStaticConfig->nResyncMarkerSpacing);
        } else if (!STRCMP(tagName, "HeaderExtension")) {
            pStaticConfig->nHECInterval = atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: HeaderExtension: %d", pStaticConfig->nHECInterval);
        } else if (!STRCMP(tagName, "ShortHeader")) {
            pStaticConfig->bEnableShortHeader = (OMX_BOOL)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: ShortHeader: %d", pStaticConfig->bEnableShortHeader);
        } else if (!STRCMP(tagName, "EntropyCoding")) {
            pStaticConfig->nCABAC = (OMX_BOOL)((!STRCMP(((OMX_STRING)pXmlNode->children->content), "CABAC")) ? TRUE : FALSE);
            VTEST_MSG_LOW("SessionXml: EntropyCoding: %d", (OMX_S32)pStaticConfig->nCABAC);
        } else if (!STRCMP(tagName, "HierPLayers")) {
            pStaticConfig->nHierPNumLayers = (OMX_U32)atoi((OMX_STRING)pXmlNode->children->content) + 1;
            VTEST_MSG_LOW("SessionXml: HierPLayers: %u", pStaticConfig->nHierPNumLayers);
        } else if (!STRCMP(tagName, "MaxHierPLayers")) {
            pStaticConfig->nMaxHPLayers = (OMX_U32)atoi((OMX_STRING)pXmlNode->children->content) + 1;
            VTEST_MSG_LOW("SessionXml: MaxHierPLayers: %u", pStaticConfig->nMaxHPLayers);
        } else if (!STRCMP(tagName, "LTRMode")) {
            pStaticConfig->nLTRMode = (OMX_U32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: LTRMode: %u", pStaticConfig->nLTRMode);
        } else if (!STRCMP(tagName, "LTRCount")) {
            pStaticConfig->nLTRCount = (OMX_U32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: LTRCount: %u", pStaticConfig->nLTRCount);
        } else if (!STRCMP(tagName, "LowLatency")) {
            pStaticConfig->nLowLatency = (OMX_S32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: LowLatency: %u", pStaticConfig->nLowLatency);
        } else if (!STRCMP(tagName, "Priority")) {
            pStaticConfig->nPriority = (OMX_S32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Priority: %u", pStaticConfig->nPriority);
        } else if (!STRCMP(tagName, "OperatingRate")) {
            pStaticConfig->nOperatingRate = (OMX_U32)atof((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: OperatingRate: %u", pStaticConfig->nOperatingRate);
        } else if (!STRCMP(tagName, "PrefixHeaderMode")) {
            if (!STRCMP(pSessionType, "dec"))
                pStaticConfig->m_nPrefixHeaderMode = (VidcDecHdrMode)ParseEnum(g_pHeaderMode, (OMX_STRING)pXmlNode->children->content);
            else if (!STRCMP(pSessionType, "enc"))
                pStaticConfig->bInsertInbandVideoHeaders = (OMX_BOOL)(VidcDecHdrMode)ParseEnum(g_pHeaderMode, (OMX_STRING)pXmlNode->children->content);

            VTEST_MSG_LOW("SessionXml: PrefixHeaderMode: %u", ParseEnum(g_pHeaderMode, (OMX_STRING)pXmlNode->children->content));
        } else if (!STRCMP(tagName, "InitialQP")) {
            OMX_U32 value = (OMX_U32)strtol((OMX_STRING)pXmlNode->children->content, NULL, 16);
            pStaticConfig->initialQP_RC_ON.fQP.iQP = value & 0xFF;
            pStaticConfig->initialQP_RC_ON.fQP.pQP = (value & 0xFF00) >> 8;
            pStaticConfig->initialQP_RC_ON.fQP.bQP = (value & 0xFF0000) >> 16;
            pStaticConfig->initialQP_RC_ON.enableInitialQP  = (value & 0xFF000000) >> 24;
            VTEST_MSG_LOW("SessionXml: InitialQP: %s", ((OMX_STRING)pXmlNode->children->content));
        } else if (!STRCMP(tagName, "FrameQP")) {
            OMX_U32 value = (OMX_U32)strtol(((OMX_STRING)pXmlNode->children->content), NULL, 16);
            pStaticConfig->frameQP_RC_OFF.iQP = value & 0xFF;
            pStaticConfig->frameQP_RC_OFF.pQP = (value & 0xFF00) >> 8;
            pStaticConfig->frameQP_RC_OFF.bQP = (value & 0xFF0000) >> 16;
            VTEST_MSG_LOW("SessionXml: FrameQP: %s", ((OMX_STRING)pXmlNode->children->content));
        } else if (!STRCMP(tagName, "RCDistribution")) {
            OMX_S32 numLayer = Parser::ParseNumList((OMX_STRING)pXmlNode->children->content,
                                        (OMX_U32*)&(pStaticConfig->nRcDistribution),
                                        MAXHPLAYERS);
            for (OMX_S32 i = 0; i < numLayer; i++) {
                VTEST_MSG_LOW("SessionXml: Bitrate[%d]: %u", i, pStaticConfig->nRcDistribution[i]);
            }
        } else if (!STRCMP(tagName, "HybridModeHP")) {
            pStaticConfig->sHybridHP.nHpLayers = (OMX_U32)atoi((OMX_STRING)pXmlNode->children->content) + 1;
            VTEST_MSG_LOW("SessionXml: HybridModeHP: %u", pStaticConfig->sHybridHP.nHpLayers);
        } else if (!STRCMP(tagName, "MinQpPacked")) {
            pStaticConfig->minQP_Packed= (OMX_U32)strtol(((OMX_STRING)pXmlNode->children->content), NULL, 16);
            VTEST_MSG_LOW("SessionXml: MinQPPacked: %u", pStaticConfig->minQP_Packed);
        } else if (!STRCMP(tagName, "MaxQpPacked")) {
            pStaticConfig->maxQP_Packed= (OMX_U32)strtol(((OMX_STRING)pXmlNode->children->content), NULL, 16);
            VTEST_MSG_LOW("SessionXml: MaxQPPacked: %u", pStaticConfig->minQP_Packed);
        } else if (!STRCMP(tagName, "Enable8x8Transform")) {
            pStaticConfig->nTransform8x8= (OMX_BOOL)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Enable8x8Transform: %u", pStaticConfig->nTransform8x8);
        } else if (!STRCMP(tagName, "DisablePQ")) {
            pStaticConfig->nDisablePQ= (OMX_BOOL)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: DisablePQ: %u", pStaticConfig->nDisablePQ);
        } else if (!STRCMP(tagName, "ROIQpFileName")) {
            strlcpy(pStaticConfig->cRoiQpFileName, pGlobalVideoProp->sInputRoot, VTEST_MAX_STRING);
            strlcat(pStaticConfig->cRoiQpFileName, (OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: ROI QP FileName: %s", pStaticConfig->cRoiQpFileName);
        } else if (!STRCMP(tagName, "PQFileName")) {
            strlcat(pGlobalVideoProp->sPqRoot, (OMX_STRING)pXmlNode->children->content, VTEST_MAX_STRING);
            strlcpy(pStaticConfig->cPqFileName, pGlobalVideoProp->sPqRoot, VTEST_MAX_STRING);
            VTEST_MSG_LOW("SessionXml: PQ FileName: %s", pStaticConfig->cPqFileName);
        } else if (!STRCMP(tagName, "ThumbnailMode")) {
            pStaticConfig->bThumbnailMode = (OMX_BOOL)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Thumbnail Mode: %u", pStaticConfig->bThumbnailMode);
        } else if (!STRCMP(tagName, "SecureMode")) {
            OMX_S32 i =0;
            sscanf((OMX_STRING)pXmlNode->children->content, "%u", &i);
            if (i)
                pStaticConfig->bSecureSession= OMX_TRUE;
            VTEST_MSG_LOW("SessionXml: SecureMode: %u", pStaticConfig->bSecureSession);
        } else if (!STRCMP(tagName, "SinkType")) {
            pStaticConfig->eSinkType= (SinkType)ParseEnum(g_pSinkType, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: SinkType: %s", (OMX_STRING)pXmlNode->children->content);
        } else if (!STRCMP(tagName, "PlaybackMode")) {
            pStaticConfig->ePlaybackMode= (PlaybackModeType)ParseEnum(g_pPlaybackMode, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: ePlaybackMode: %d", pStaticConfig->ePlaybackMode);
        } else if (!STRCMP(tagName, "MetaMode")) {
            OMX_S32 i =0;
            sscanf((OMX_STRING)pXmlNode->children->content, "%u", &i);
            if (i)
                pStaticConfig->bMetaMode = OMX_TRUE;
            else
                pStaticConfig->bMetaMode = OMX_FALSE;
            VTEST_MSG_LOW("SessionXml: MetaMode = %d", (int)pStaticConfig->bMetaMode);
        } else if (!STRCMP(tagName, "PlaybackMode")) {
            pStaticConfig->ePlaybackMode = (PlaybackModeType)ParseEnum(g_pPlaybackMode, (OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: PlaybackMode: %s", (OMX_STRING)pXmlNode->children->content);
        } else if (!STRCMP(tagName, "DownScalar")) {
            OMX_S32 i =0;
            sscanf((OMX_STRING)pXmlNode->children->content, "%u", &i);
            if (i)
                pStaticConfig->bDownScalar = OMX_TRUE;
            else
                pStaticConfig->bDownScalar = OMX_FALSE;
            VTEST_MSG_LOW("SessionXml: DownScalar: %d", (int)pStaticConfig->bDownScalar);
        } else if (!STRCMP(tagName, "color_space")) {
            pStaticConfig->sColorAspects.mPrimaries =
                (android::ColorAspects::Primaries)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: Primaries: %u", pStaticConfig->sColorAspects.mPrimaries);
        } else if (!STRCMP(tagName, "VideoFullRange")) {
            pStaticConfig->sColorAspects.mRange =
                (android::ColorAspects::Range)(OMX_U32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: VideoFullRange: %u", pStaticConfig->sColorAspects.mRange);
        } else if (!STRCMP(tagName, "TransferChar")) {
            pStaticConfig->sColorAspects.mTransfer =
                (android::ColorAspects::Transfer)(OMX_U32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: TransferChar: %u", pStaticConfig->sColorAspects.mTransfer);
        } else if (!STRCMP(tagName, "MatrixCoef")) {
            pStaticConfig->sColorAspects.mMatrixCoeffs =
                (android::ColorAspects::MatrixCoeffs)(OMX_U32)atoi((OMX_STRING)pXmlNode->children->content);
            VTEST_MSG_LOW("SessionXml: MatrixCoef: %u", pStaticConfig->sColorAspects.mMatrixCoeffs);
        } else if (!STRCMP(tagName, "DynamicProperty")) {
            VTEST_MSG_LOW("SessionXml: Dynamic Property:");
            OMX_S32 nFrameNum = -1;
            OMX_S32 nFPSDen = 0;
            OMX_S32 nFPSNum = 0;
            OMX_U32 nMaxQpPacked = 0;
            OMX_U32 nMinQpPacked = 0;
            OMX_S32 lastUpdatedDynPropFrameNum = nDynamicPropCount;

            assert(pXmlNode->type == XML_ELEMENT_NODE);

            xmlNodePtr pDynProp = pXmlNode->xmlChildrenNode;

            while (pDynProp) {

                OMX_STRING tagName = (OMX_STRING)pDynProp->name;

                if (nDynamicPropCount >= nDynamicPropAllocated) {

                    nDynamicPropAllocated = nDynamicPropAllocated + nDynamicPropChunk;
                    pDynamicProperties    = new DynamicConfig[nDynamicPropAllocated];
                    memcpy(pDynamicProperties, m_pDynamicProperties, sizeof(DynamicConfig) * nDynamicPropCount);
                    delete[] m_pDynamicProperties;
                    m_pDynamicProperties = pDynamicProperties;
                }

                if (!STRCMP(tagName, "FrameNum")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName,
                        (OMX_STRING)pDynProp->children->content);
                    nFrameNum  = (OMX_S32) atoi((OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Pause")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName,
                        (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num      = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param   = vtest_Pause;
                    m_pDynamicProperties[nDynamicPropCount].config_data.pause_duration  =
                                    (OMX_S32) atoi((OMX_STRING)pDynProp->children->content);
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "Seek")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName,
                        (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num      = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param   = vtest_Seek;
                    m_pDynamicProperties[nDynamicPropCount].config_data.new_frame_num  =
                                    (OMX_S32) atoi((OMX_STRING)pDynProp->children->content);
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "EOS")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName,
                        (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Drop")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName,
                        (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Bitrate")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum,
                        tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param                       = OMX_IndexConfigVideoBitrate;
                    m_pDynamicProperties[nDynamicPropCount].config_data.bitrate.nPortIndex     = (OMX_S32)PORT_INDEX_OUT;
                    m_pDynamicProperties[nDynamicPropCount].config_data.bitrate.nEncodeBitrate = (OMX_S32)atoi((OMX_STRING)pDynProp->children->content);
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "FPSNumerator")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                    nFPSNum = (OMX_S32)atoi((OMX_STRING)pDynProp->children->content);
                    if (nFPSDen > 0) {
                        VTEST_MSG_LOW("Dynamic FPS update %d/%d", nFPSNum, nFPSDen);
                        m_pDynamicProperties[nDynamicPropCount].frame_num = nFrameNum;
                        m_pDynamicProperties[nDynamicPropCount].config_param = OMX_IndexConfigVideoFramerate;
                        m_pDynamicProperties[nDynamicPropCount].config_data.framerate.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                        m_pDynamicProperties[nDynamicPropCount].config_data.f_framerate = (float)(nFPSNum/nFPSDen);
                        nDynamicPropCount++;
                    }
                } else if (!STRCMP(tagName, "FPSDenominator")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                    nFPSDen = (OMX_S32)atoi((OMX_STRING)pDynProp->children->content);
                    if (nFPSNum > 0) {
                        VTEST_MSG_LOW("Dynamic FPS update %d/%d", nFPSNum, nFPSDen);
                        m_pDynamicProperties[nDynamicPropCount].frame_num = nFrameNum;
                        m_pDynamicProperties[nDynamicPropCount].config_param = OMX_IndexConfigVideoFramerate;
                        m_pDynamicProperties[nDynamicPropCount].config_data.framerate.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                        m_pDynamicProperties[nDynamicPropCount].config_data.f_framerate = (float)(nFPSNum/nFPSDen);
                        nDynamicPropCount++;
                    }
                } else if (!STRCMP(tagName, "LTRMarkIndex")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    if (pStaticConfig->eCodec == OMX_VIDEO_CodingVP8) {
                        m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                        m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE)OMX_IndexConfigVideoVp8ReferenceFrame;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.nPortIndex             = (OMX_U32)PORT_INDEX_IN;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bUseGoldenFrame        = OMX_FALSE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bGoldenFrameRefresh    = OMX_TRUE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bAlternateFrameRefresh = OMX_FALSE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bPreviousFrameRefresh  = OMX_FALSE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bUsePreviousFrame      = OMX_FALSE;
                    } else {
                        m_pDynamicProperties[nDynamicPropCount].frame_num                                      = nFrameNum;
                        m_pDynamicProperties[nDynamicPropCount].config_param                                   = (OMX_INDEXTYPE) OMX_QcomIndexConfigVideoLTRMark;
                        m_pDynamicProperties[nDynamicPropCount].config_data.markltr.nPortIndex                 = (OMX_U32) PORT_INDEX_IN;
                        m_pDynamicProperties[nDynamicPropCount].config_data.markltr.nID                        = (OMX_S32)atoi((OMX_STRING)pDynProp->children->content);
                    }
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "LTRUseMask")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    if (pStaticConfig->eCodec == OMX_VIDEO_CodingVP8) {
                        m_pDynamicProperties[nDynamicPropCount].frame_num                                   = nFrameNum;
                        m_pDynamicProperties[nDynamicPropCount].config_param                                = (OMX_INDEXTYPE)OMX_IndexConfigVideoVp8ReferenceFrame;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.nPortIndex          = (OMX_U32)PORT_INDEX_IN;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bUseGoldenFrame     = OMX_TRUE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bGoldenFrameRefresh = OMX_FALSE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bAlternateFrameRefresh = OMX_FALSE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bPreviousFrameRefresh  = OMX_FALSE;
                        m_pDynamicProperties[nDynamicPropCount].config_data.vp8refframe.bUsePreviousFrame      = OMX_FALSE;
                    } else {
                        m_pDynamicProperties[nDynamicPropCount].frame_num                     = nFrameNum;
                        m_pDynamicProperties[nDynamicPropCount].config_param                  = (OMX_INDEXTYPE)OMX_QcomIndexConfigVideoLTRUse;
                        m_pDynamicProperties[nDynamicPropCount].config_data.useltr.nPortIndex = (OMX_U32)PORT_INDEX_IN;
                        m_pDynamicProperties[nDynamicPropCount].config_data.useltr.nFrames    = 0;
                        m_pDynamicProperties[nDynamicPropCount].config_data.useltr.nID        = (OMX_S32)atoi((OMX_STRING)pDynProp->children->content);
                    }
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "InjectKeyFrame")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = OMX_IndexConfigVideoIntraVOPRefresh;
                    m_pDynamicProperties[nDynamicPropCount].config_data.intravoprefresh.nPortIndex = (OMX_S32)PORT_INDEX_OUT;
                    m_pDynamicProperties[nDynamicPropCount].config_data.intravoprefresh.IntraRefreshVOP = OMX_TRUE;
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "FrameType")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigPictureTypeDecode;
                    m_pDynamicProperties[nDynamicPropCount].config_data.pictypedecode.eDecodeType =
                        (QOMX_VIDEO_PICTURE_TYPE_DECODE) atoi((OMX_STRING)pDynProp->children->content);
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "FrameQP")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigQp;
                    OMX_U32 value = (OMX_U32)strtol(((OMX_STRING)pDynProp->children->content), NULL, 16);
                    m_pDynamicProperties[nDynamicPropCount].config_data.configqp.nQP = (OMX_U32)((value & 0xFF00) >> 8);
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "FramePackedQP")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE)OMX_QcomIndexConfigPackedQp;
                    OMX_U32 value = (OMX_U32)strtol(((OMX_STRING)pDynProp->children->content), NULL, 16);
                    m_pDynamicProperties[nDynamicPropCount].config_data.configpackedqp.nQpI = (OMX_U32)(value & 0xFF);
                    m_pDynamicProperties[nDynamicPropCount].config_data.configpackedqp.nQpP = (OMX_U32)((value & 0xFF00) >> 8);
                    m_pDynamicProperties[nDynamicPropCount].config_data.configpackedqp.nQpB = (OMX_U32)((value & 0xFF0000) >> 16);
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "HierPLayers")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content + 1);
                    m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE) OMX_IndexConfigAndroidVideoTemporalLayering;
                    m_pDynamicProperties[nDynamicPropCount].config_data.confighierp.nPortIndex = (OMX_U32) PORT_INDEX_OUT;
                    m_pDynamicProperties[nDynamicPropCount].config_data.confighierp.eHierarchicalCodingType =
                        QOMX_HIERARCHICALCODING_P;
                    m_pDynamicProperties[nDynamicPropCount].config_data.confighierp.nNumLayers = (OMX_U32) atoi((OMX_STRING)pDynProp->children->content) + 1;
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "nPframes")) {
                    VTEST_MSG_LOW("SessionXml: Frame# %d DynamicProperty: %s Value: %s", nFrameNum, tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num                          = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE) QOMX_IndexConfigVideoIntraperiod;
                    m_pDynamicProperties[nDynamicPropCount].config_data.confighierp.nPortIndex = (OMX_U32) PORT_INDEX_OUT;
                    m_pDynamicProperties[nDynamicPropCount].config_data.intraperiod.nPFrames = (OMX_U32) atoi((OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].config_data.intraperiod.nBFrames = 0;
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "MaxQpPacked")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num    = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE) OMX_QTIIndexConfigFrameIPBQPRange;
                    nMaxQpPacked = (OMX_U32)strtol(((OMX_STRING)pDynProp->children->content), NULL, 16);
                    m_pDynamicProperties[nDynamicPropCount].config_data.configFrameQP.minQP_Packed = nMinQpPacked;
                    m_pDynamicProperties[nDynamicPropCount].config_data.configFrameQP.maxQP_Packed = nMaxQpPacked;
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "MinQpPacked")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                    m_pDynamicProperties[nDynamicPropCount].frame_num    = nFrameNum;
                    m_pDynamicProperties[nDynamicPropCount].config_param = (OMX_INDEXTYPE) OMX_QTIIndexConfigFrameIPBQPRange;
                    nMinQpPacked = (OMX_U32)strtol(((OMX_STRING)pDynProp->children->content), NULL, 16);
                    m_pDynamicProperties[nDynamicPropCount].config_data.configFrameQP.minQP_Packed = nMinQpPacked;
                    m_pDynamicProperties[nDynamicPropCount].config_data.configFrameQP.maxQP_Packed = nMaxQpPacked;
                    nDynamicPropCount++;
                } else if (!STRCMP(tagName, "BasePriorityID")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "DropFrame")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Layer1QP")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Layer2QP")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Layer3QP")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Layer4QP")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else if (!STRCMP(tagName, "Layer5Q")) {
                    VTEST_MSG_LOW("SessionXml: DynamicProperty: %s Value: %s", tagName, (OMX_STRING)pDynProp->children->content);
                } else {
                    VTEST_MSG_LOW("SessionXml: Unknown DynamicProperty: %s", tagName);
                }

                pDynProp = pDynProp->next;
                pStaticConfig->dynamic_config_array_size = nDynamicPropCount;
            }
            // Patch the frame num in dynamic property structures
            for (OMX_S32 i = lastUpdatedDynPropFrameNum; i < nDynamicPropCount; i++) {
                m_pDynamicProperties[i].frame_num = nFrameNum;
            }
        }
        else if (!STRCMP(tagName, "ExtraDataDump")) {
            /* parse extra data dump tag */
            VTEST_MSG_LOW("SessionXml: ExtraDataDump Property:");
            OMX_S32 lastUpdatedDynPropFrameNum = nExtradataPropCount;
            OMX_INDEXTYPE nExtradataType=(OMX_INDEXTYPE)0;
            OMX_S32 IsNewExtradata=1;

            assert(pXmlNode->type == XML_ELEMENT_NODE);

            xmlNodePtr pExtraDataPropNode = pXmlNode->xmlChildrenNode;

            while (pExtraDataPropNode) {
                if (nExtradataPropCount>= nExtradataPropAllocated) {
                    nExtradataPropAllocated = nExtradataPropAllocated + nExtraDataPropChunk;
                    ExtradataConfig* pExtradataConfig = new ExtradataConfig[nExtradataPropAllocated];
                    if(!pExtradataConfig)
                    {
                        VTEST_MSG_ERROR("Memory Allocation failed!");
                        return OMX_ErrorUndefined;
                    }
                    memcpy(pExtradataConfig, pExtradataProperties, sizeof(ExtradataConfig) * nExtradataPropCount);
                    delete[] pExtradataProperties;
                    pExtradataProperties = pExtradataConfig;
                }
                /* If current node != text and != comment then process the tag */
                if ((STRCMP((OMX_STRING)pExtraDataPropNode->name, "text")) && (STRCMP((OMX_STRING)pExtraDataPropNode->name, "comment")))
                {
                    if(pExtraDataPropNode->children)
                    {
                        /* looks for the tagname in the enummap */
                        nExtradataType =(OMX_INDEXTYPE)ParseEnum(g_pXmlOmxExtradataTypeMap, (OMX_STRING)pExtraDataPropNode->name);
                        if(nExtradataType < 0)
                        {
                            VTEST_MSG_ERROR("Xml Parsing failed:Invalid ExtradataDump Type");
                            return OMX_ErrorBadParameter;
                        }
                        for(OMX_S32 i=0; i< nExtradataPropCount;i++)
                        {
                            if(pExtradataProperties[i].ExtradataType == nExtradataType )
                            {
                                pExtradataProperties[i].EnableDump = (OMX_U32)atoi((OMX_STRING)(pExtraDataPropNode->children->content));
                                IsNewExtradata =0;
                                break;
                            }
                        }
                        if(IsNewExtradata)
                        {
                            pExtradataProperties[nExtradataPropCount].ExtradataType = (OMX_INDEXTYPE)ParseEnum(g_pXmlOmxExtradataTypeMap, (OMX_STRING)pExtraDataPropNode->name);
                            pExtradataProperties[nExtradataPropCount].EnableDump = (OMX_U32)atoi((OMX_STRING)(pExtraDataPropNode->children->content));
                            pExtradataProperties[nExtradataPropCount].RefFileName = NULL;
                            nExtradataPropCount++;
                        }
                    }
                }
                pExtraDataPropNode = pExtraDataPropNode->next;
                pStaticConfig->nExtradataConfigArraySize = nExtradataPropCount;
            }
        } else if (!STRCMP(tagName, "ExtraData")) {
            /* parse extra data tag */
            VTEST_MSG_LOW("SessionXml: ExtraData Property:");
            OMX_S32 lastUpdatedDynPropFrameNum = nExtradataPropCount;
            OMX_INDEXTYPE nExtradataType=(OMX_INDEXTYPE)0;
            OMX_S32 IsNewExtradata=1;

            assert(pXmlNode->type == XML_ELEMENT_NODE);

            xmlNodePtr pExtraDataPropNode = pXmlNode->xmlChildrenNode;

            while (pExtraDataPropNode) {
                if (nExtradataPropCount>= nExtradataPropAllocated) {
                    nExtradataPropAllocated = nExtradataPropAllocated + nExtraDataPropChunk;
                    ExtradataConfig* pExtradataConfig = new ExtradataConfig[nExtradataPropAllocated];
                    if(!pExtradataConfig)
                    {
                        VTEST_MSG_ERROR("Memory Allocation failed!");
                        delete[] pExtradataProperties;
                        return OMX_ErrorUndefined;
                    }
                    memcpy(pExtradataConfig, pExtradataProperties, sizeof(ExtradataConfig) * nExtradataPropCount);
                    delete[] pExtradataProperties;
                    pExtradataProperties = pExtradataConfig;
                }
                /* If current node != text and != comment then process the tag */
                if ((STRCMP((OMX_STRING)pExtraDataPropNode->name, "text")) && (STRCMP((OMX_STRING)pExtraDataPropNode->name, "comment")))
                {
                    if(pExtraDataPropNode->children)
                    {
                        /* looks for the tagname in the enummap */
                        nExtradataType =(OMX_INDEXTYPE)ParseEnum(g_pXmlOmxExtradataTypeMap, (OMX_STRING)pExtraDataPropNode->name);
                        if(nExtradataType < 0)
                        {
                            VTEST_MSG_ERROR("Xml Parsing failed:Invalid Extradata Type");
                            delete[] pExtradataProperties;
                            return OMX_ErrorBadParameter;
                        }
                        for(OMX_S32 i=0; i< nExtradataPropCount;i++)
                        {
                            if(pExtradataProperties[i].ExtradataType == nExtradataType )
                            {
                                pExtradataProperties[i].RefFileName = (OMX_STRING)(pExtraDataPropNode->children->content);
                                IsNewExtradata =0;
                                break;
                            }
                        }
                        if(IsNewExtradata)
                        {
                            pExtradataProperties[nExtradataPropCount].ExtradataType = (OMX_INDEXTYPE)ParseEnum(g_pXmlOmxExtradataTypeMap, (OMX_STRING)pExtraDataPropNode->name);
                            pExtradataProperties[nExtradataPropCount].RefFileName = (OMX_STRING)(pExtraDataPropNode->children->content);
                            pExtradataProperties[nExtradataPropCount].EnableDump = 0;
                            nExtradataPropCount++;
                        }
                    }
                }
                pExtraDataPropNode = pExtraDataPropNode->next;
                pStaticConfig->nExtradataConfigArraySize = nExtradataPropCount;
            }
        } else if (!STRCMP(tagName, "ColorPrimaryFile")) {
            /* parse color primary file tag */
            VTEST_MSG_LOW("SessionXml: Colorprimary being set");

            if (nExtradataPropCount>= nExtradataPropAllocated) {
                nExtradataPropAllocated = nExtradataPropAllocated + nExtraDataPropChunk;
                ExtradataConfig* pExtradataConfig =
                    new ExtradataConfig[nExtradataPropAllocated];
                if(!pExtradataConfig)
                {
                    VTEST_MSG_ERROR("Memory Allocation failed!");
                    return OMX_ErrorUndefined;
                }
                memcpy(pExtradataConfig, pExtradataProperties,
                    sizeof(ExtradataConfig) * nExtradataPropCount);
                delete[] pExtradataProperties;
                pExtradataProperties = pExtradataConfig;
            }
            pExtradataProperties[nExtradataPropCount].ExtradataType =
                                      (OMX_INDEXTYPE)ColorAspectsInfo;
            if (!STRCMP((OMX_STRING)pXmlNode->children->content, "0")) {
                pExtradataProperties[nExtradataPropCount].RefFileName = NULL;
                pExtradataProperties[nExtradataPropCount].EnableDump = 1;
            } else {
                pExtradataProperties[nExtradataPropCount].RefFileName =
                                      (OMX_STRING)pXmlNode->children->content;
                pExtradataProperties[nExtradataPropCount].EnableDump = 0;
                VTEST_MSG_LOW("colorprimary file is %s",
                    pExtradataProperties[nExtradataPropCount].RefFileName);
            }
            nExtradataPropCount++;
            pStaticConfig->nExtradataConfigArraySize = nExtradataPropCount;
        } else {
            VTEST_MSG_LOW("SessionXml: Tag Not supported: %s", (OMX_STRING)tagName);
        }

        pXmlNode = pXmlNode->next;

    }

    // Update QP Range depending on Codec type

    OMX_U8 min_qp = 0, max_qp=0;

    if (pStaticConfig->eCodec == OMX_VIDEO_CodingAVC ||
        pStaticConfig->eCodec == OMX_VIDEO_CodingHEVC) {
        min_qp = 0;
        max_qp = 51;
    } else if (pStaticConfig->eCodec == OMX_VIDEO_CodingVP8) {
        min_qp = 1;
        max_qp = 31;
    }

    pStaticConfig->nMaxQp = (pStaticConfig->nMaxQp == INVALID_VALUE) ? max_qp : pStaticConfig->nMaxQp ;
    pStaticConfig->nMinQp = (pStaticConfig->nMinQp == INVALID_VALUE) ? min_qp : pStaticConfig->nMaxQp ;

    // Update default sink based on session type
    if (pStaticConfig->eSinkType == Unknown_Sink) {
        if (!STRCMP(pSessionType, "enc")) {
            pStaticConfig->eSinkType = File_Sink;
        } else if (!STRCMP(pSessionType, "dec")) {
            pStaticConfig->eSinkType = NativeWindow_Sink;
        }
    } else {
        // Encode supports file sink only
        if ((!STRCMP(pSessionType, "enc")) && (pStaticConfig->eSinkType != File_Sink)) {
            VTEST_MSG_ERROR("Xml Parsing failed:Invalid Sink Type");
            delete[] pExtradataProperties;
            return OMX_ErrorBadParameter;
        }
    }
    // If sinktype is mentioned in master config xml as File_Sink then
    // overwrite the static config's sink type to File mode,playback mode to port reconfig.
    // and disable meta mode.
    if (pGlobalVideoProp->bSinkType == File_Sink) {
        pStaticConfig->eSinkType = File_Sink;
        pStaticConfig->ePlaybackMode = DynamicPortReconfig;
        pStaticConfig->bMetaMode = OMX_FALSE;
    }

    pStaticConfig->pDynamicProperties = nDynamicPropCount ? m_pDynamicProperties : NULL;
    pStaticConfig->pExtradataProperties = pExtradataProperties;

    //Some multiple parameters in XML need to be re-mapped to 1 parameters in the config file
    if(nFPSNumerator > 0 && nFPSDenominator > 0) {
        pStaticConfig->nFramerate = (OMX_S32)(nFPSNumerator/nFPSDenominator);
        VTEST_MSG_LOW("SessionXml: nFramerate: %d", pStaticConfig->nFramerate);
    } else {
        VTEST_MSG_LOW("SessionXml: Either FPS Numerator or Denominator is not > 0, defaulting to 30fps");
    }

    if(nBframes >= 0 && nPframes >= 0) {
        pStaticConfig->nIntraPeriod = nIframes + ((nIframes+1)* nPframes) + ((nIframes+1) * nPframes*nBframes) + 1;
        pStaticConfig->nIDRPeriod = nIframes+1;
        pStaticConfig->nBFrames = ((nIframes+1) * nBframes);
        pStaticConfig->nPFrames = ((nIframes+1)* nPframes);
        VTEST_MSG_LOW("SessionXml: nIntraPeriod: %d nIDRPeriod: %d", pStaticConfig->nIntraPeriod , pStaticConfig->nIDRPeriod);
    } else {
        VTEST_MSG_LOW("SessionXml: Either nBframes is not >=0 or nPframes is not >= 0, defaulting to intra period of 60");
    }
    if(!IsnBFramesPresent) {
        pStaticConfig->nBFrames =0xDEAD;
    }

    if (pStaticConfig->eResyncMarkerType == RESYNC_MARKER_BITS && pStaticConfig->nResyncMarkerSpacing) {
        pStaticConfig->nResyncMarkerSpacing = pStaticConfig->nResyncMarkerSpacing << 3;
    }

    //Some extra parameters need to be setup for encoder to work fine
    if (!STRCMP(pSessionType, "enc")) {
        pStaticConfig->nInBufferCount = pGlobalVideoProp->nEncOutPortBufCnt;
        pStaticConfig->nOutBufferCount = pGlobalVideoProp->nEncCapPortBufCnt;
        strlcat(pStaticConfig->cOutFileName,(OMX_STRING)encFileXtn,VTEST_MAX_STRING);
        VTEST_MSG_LOW("SessionXml: InBufferCnt:%d OutBufferCnt:%d outFileName:%s", pStaticConfig->nInBufferCount, pStaticConfig->nOutBufferCount, pStaticConfig->cOutFileName);
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Config::GetCodecConfig(CodecConfigType *pCodecConfig) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    if (pCodecConfig != NULL) {
        memcpy(pCodecConfig, &m_sCodecConfig, sizeof(m_sCodecConfig));
    } else {
        result = OMX_ErrorBadParameter;
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OMX_ERRORTYPE Config::GetDynamicConfig(DynamicConfigType *pDynamicConfig) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    if (pDynamicConfig != NULL) {
        memcpy(pDynamicConfig, &m_sDynamicConfig, sizeof(m_sDynamicConfig));
    } else {
        result = OMX_ErrorBadParameter;
    }
    return result;
}

} // namespace vtest
