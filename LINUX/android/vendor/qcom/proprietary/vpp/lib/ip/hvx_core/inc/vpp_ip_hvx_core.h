/*!
 * @file vpp_ip_hvx_core.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_HVX_CORE_H_
#define _VPP_IP_HVX_CORE_H_

#include "vpp_ion.h"
#include "vpp_queue.h"
#include "vpp_ip.h"
#include "vpp_svc.h"


/************************************************************************
 * Local definitions
 ***********************************************************************/

#define HVX_ALLOC(sz) hvx_alloc_int(sz, #sz)

/************************************************************************
 * Local definitions
 ***********************************************************************/

enum
{
    eIpBufFlag_EosProcessed,
    eIpBufFlag_DcProcessed,
    eIpBufFlag_IdrProcessed,
    eIpBufFlag_Mapped,
    eIpBufFlag_InternalBypass,
};

// Internal HVX Buffer mapping
enum
{
    eIpIntBufFlag_Registered,
};

#define VPP_BUF_FLAG_EOS_PROCESSED      (1 << (eVppBufFlag_IpStart + eIpBufFlag_EosProcessed))
#define VPP_BUF_FLAG_DC_PROCESSED       (1 << (eVppBufFlag_IpStart + eIpBufFlag_DcProcessed))
#define VPP_BUF_FLAG_IDR_PROCESSED      (1 << (eVppBufFlag_IpStart + eIpBufFlag_IdrProcessed))
#define VPP_BUF_FLAG_INTERNAL_BYPASS    (1 << (eVppBufFlag_IpStart + eIpBufFlag_InternalBypass))

// Internal HVX Buffer mapping: For bit mappings refer to t_EVppIntBufferFlags enum.
#define VPP_BUF_FLAG_HVX_REGISTERED     (1 << (eVppIntBufFlag_IpStart + eIpIntBufFlag_Registered))

#define VPP_HVX_SECURE_SESSION          "&_session=1"

typedef struct StHvxCoreBufParams {
    uint32_t u32InHeight;
    uint32_t u32InWidth;
    uint32_t au32PlStride[MAX_NUM_PLANES];
    uint32_t au32PlSizeBytes[MAX_NUM_PLANES];
    uint32_t u32OutHeight;
    uint32_t u32OutWidth;
    uint32_t u32NumFrames;
    vpp_svc_pixel_fmt_t ePixFmt;
    vpp_svc_field_fmt_t eFieldFmt;
}t_StHvxCoreBufParams;

typedef struct {
    t_StVppIpBase stBase;

    remote_handle64 hvx_handle;
    vpp_svc_params_t* pstParams;
    vpp_svc_cap_resource_list_t* pstCapabilityResources;
    t_StHvxCoreBufParams stHvxBufParams;

    //HVX in/out buffers
    vpp_svc_frame_group_descriptor_t* pstBufferdataIn;
    vpp_svc_frame_group_descriptor_t* pstBufferdataOut;

    struct {
        // for managing the buffer in module
        uint32_t bAllocated;
        uint32_t u32AllocatedSize;
        t_StVppIonBuf stIonCtxBuf;

        // passed to vpp_svc_ctx
        void *pvRemote;
        uint32_t u32Length;
        uint32_t u32Offset;
        uint32_t u32FrameSizeBytes;
        uint32_t u32ProcessingFlags;
        uint32_t u32CtxSz;
        uint32_t u32DiagCtxSz;
    } ctx;

    struct {
        uint32_t u32TsRead;
    } once;

    struct {
        uint32_t u32LogFlags;
        uint32_t u32StatsFlags;
        uint32_t u32StatsPeriod;
    } debug_cfg;
} t_StVppIpHvxCoreCb;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
inline void *hvx_alloc_int(uint32_t sz, const char *str);
void vVppIpHvxCore_RegisterBuffer(t_StVppIpHvxCoreCb *pstHvxCore, t_StVppMemBuf *pstIntMemBuf);
void vVppIpHvxCore_UnregisterBuffer(t_StVppIpHvxCoreCb *pstHvxCore, t_StVppMemBuf *pstIntMemBuf);
uint32_t bVppIpHvxCore_IsSecure(t_StVppIpHvxCoreCb *pstHvxCore);
vpp_svc_field_fmt_t eVppIpHvxCore_SvcFieldFormatGet(t_EVppBufType eBufType);
vpp_svc_pixel_fmt_t eVppIpHvxCore_SvcPixelFormatGet(enum vpp_color_format fmt);

uint32_t u32VppIpHvxCore_SvcParamSetROI(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t bEnable,
                                        uint32_t bCompute, uint32_t u32XStart, uint32_t u32YStart,
                                        uint32_t u32XEnd, uint32_t u32YEnd, uint32_t u32LineWidth,
                                        uint32_t u32LineY, uint32_t u32LineCr, uint32_t u32LineCb);
uint32_t u32VppIpHvxCore_SvcParamSetHeaderIdxAlgo(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Index,
                                                  uint32_t u32Algo);
uint32_t u32VppIpHvxCore_SvcParamSetDataSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32AlgoCnt,
                                             uint32_t u32DataLength);
vpp_svc_config_hdr_t *pstVppIpHvxCore_SvcParamGetHeaderIdxAddr(t_StVppIpHvxCoreCb *pstHvxCore,
                                                               uint32_t u32Index);
void *pvVppIpHvxCore_SvcParamGetDataOffsetAddr(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Offset);

uint32_t u32VppIpHvxCore_BufParamSetSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                         uint32_t u32Width, uint32_t u32Height);
uint32_t u32VppIpHvxCore_BufParamGetSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                         uint32_t *u32pWidth, uint32_t *u32pHeight);
uint32_t u32VppIpHvxCore_BufParamSetPixFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_pixel_fmt_t eFmt);
uint32_t u32VppIpHvxCore_BufParamGetPixFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_pixel_fmt_t *peFmt);
uint32_t u32VppIpHvxCore_BufParamSetFldFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_field_fmt_t eFmt);
uint32_t u32VppIpHvxCore_BufParamGetFldFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                           enum vpp_svc_field_fmt_t *peFmt);
uint32_t u32VppIpHvxCore_BufParamSetPlaneSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t *pu32Size,
                                              uint32_t u32Cnt);
uint32_t u32VppIpHvxCore_BufParamGetPlaneSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t *pu32Size,
                                              uint32_t u32Cnt);
uint32_t u32VppIpHvxCore_BufParamGetPlaneTotalSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                                   uint32_t *pu32Size);
uint32_t u32VppIpHvxCore_BufParamSetPlaneStride(t_StVppIpHvxCoreCb *pstHvxCore,
                                                uint32_t *pu32Stride, uint32_t u32Cnt);
uint32_t u32VppIpHvxCore_BufParamGetPlaneStride(t_StVppIpHvxCoreCb *pstHvxCore,
                                                uint32_t *pu32Stride, uint32_t u32Cnt);
uint32_t u32VppIpHvxCore_BufParamInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Width,
                                      uint32_t u32Height, enum vpp_color_format eFmt);
uint32_t u32VppIpHvxCore_BuffInInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers);
void vVppIpHvxCore_BuffInTerm(t_StVppIpHvxCoreCb *pstHvxCore);
uint32_t u32VppIpHvxCore_BuffInCompute(t_StVppIpHvxCoreCb *pstHvxCore);
uint32_t u32VppIpHvxCore_BufInSetSize(t_StVppIpHvxCoreCb *pstHvxCore,
                                      uint32_t u32Width, uint32_t u32Height);
uint32_t u32VppIpHvxCore_BufInSetPixFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                        enum vpp_svc_pixel_fmt_t eFmt);
uint32_t u32VppIpHvxCore_BufInSetFldFmt(t_StVppIpHvxCoreCb *pstHvxCore,
                                        enum vpp_svc_field_fmt_t eFmt);
uint32_t u32VppIpHvxCore_BufInSetNumBuffers(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers);
uint32_t u32VppIpHvxCore_BufInSetAttrSize(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                          uint32_t *pu32Size, uint32_t u32Cnt);
uint32_t u32VppIpHvxCore_BufInSetAttrStride(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                            uint32_t *pu32Stride, uint32_t u32Cnt);
uint32_t u32VppIpHvxCore_BufInSetAttrUbwc(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                          t_StVppUbwcStats *pstBufStats);
uint32_t u32VppIpHvxCore_BufInSetUserDataLen(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                             uint32_t u32Size);
uint32_t u32VppIpHvxCore_BufInSetUserDataAddr(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                              void* vpAddr);

uint32_t u32VppIpHvxCore_BuffOutInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers);
void vVppIpHvxCore_BuffOutTerm(t_StVppIpHvxCoreCb *pstHvxCore);
uint32_t u32VppIpHvxCore_BuffOutCompute(t_StVppIpHvxCoreCb *pstHvxCore);
uint32_t u32VppIpHvxCore_BufOutGetAttrUbwc(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                           t_StVppUbwcStats *pstBufStats);
uint32_t u32VppIpHvxCore_BufOutSetNumBuffers(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32NumBuffers);
uint32_t u32VppIpHvxCore_BufOutSetUserDataLen(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                              uint32_t u32Size);
uint32_t u32VppIpHvxCore_BufOutSetUserDataAddr(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Idx,
                                               void* vpAddr);

int VppIpHvxCore_Process(t_StVppIpHvxCoreCb *pstHvxCore);
uint32_t u32VppIpHvxCore_AlgoInit(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t u32Index,
                                  uint32_t u32Algo, const char *cpAlgoLibName);
uint32_t u32VppIpHvxCore_Open(t_StVppIpHvxCoreCb *pstHvxCore, uint32_t vpp_processing_flags,
                              uint32_t framesize_bytes);
uint32_t u32VppIpHvxCore_Close(t_StVppIpHvxCoreCb *pstHvxCore);
t_StVppIpHvxCoreCb *pstVppIpHvxCore_Init(t_StVppCtx *pstCtx, uint32_t u32Flags,
                                         uint32_t u32CtrlCnt, uint32_t u32UserDataSize);
void vVppIpHvxCore_Term(t_StVppIpHvxCoreCb *pstHvxCore);
uint32_t u32VppIpHvxCore_PrepareCtx(t_StVppIpHvxCoreCb *pstHvxCore);

#endif /* _VPP_IP_HVX_CORE_H_ */
