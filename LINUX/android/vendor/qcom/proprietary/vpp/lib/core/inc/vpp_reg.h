/*!
 * @file vpp_reg.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_REG_H_
#define _VPP_REG_H_

#include "vpp_def.h"
#include "vpp_ctx.h"
#include "vpp_buf.h"
#include "vpp_callback.h"

#define VPP_IP(ipname) &VPP_IP_NPTR(ipname)
#define VPP_IP_NPTR(ipname) ip_##ipname
#define VPP_IP_DECL(ipname, enum_ip, ac_algos, \
                    cap_credit, cap_quirk, cap_str_mul, \
                    cap_scl_mul, cap_max_ses, cap_cp, cap_map_buf, \
                    max_w, max_h, min_w, min_h, \
                    in_fmt, out_fmt, \
                    fnc_boot, fnc_shutdown, \
                    fnc_init, fnc_term, fnc_open, fnc_close, \
                    fnc_setparam, fnc_setctrl, fnc_getbufreq, \
                    fnc_queuebuf, fnc_flush, fnc_drain, fnc_reconfigure) \
    static const t_StVppIpBlock VPP_IP_NPTR(ipname) = { \
        .name = #ipname, \
        .eIp = enum_ip, \
        .algo_cap = { \
            .algos = ac_algos, \
            .u32AlgoCnt = sizeof(ac_algos) / sizeof(t_StVppAlgo), \
        }, \
        .capabilities = { \
            .u32MaxCredits = cap_credit, \
            .u32Quirks = cap_quirk, \
            .u32StrideMultiple = cap_str_mul, \
            .u32ScanlineMultiple = cap_scl_mul, \
            .u32MaxSessions = cap_max_ses, \
            .bCp = cap_cp, \
            .bRequireMappedPxBuf = cap_map_buf, \
            .max_res = { \
                .u32Width = max_w, \
                .u32Height = max_h, \
            }, \
            .min_res = { \
                .u32Width = min_w, \
                .u32Height = min_h, \
            }, \
        }, \
        .stInputPort = { \
            .peColorFmts = in_fmt, \
            .u32FmtCnt = sizeof(in_fmt) / sizeof(t_EVppColorFmt), \
        }, \
        .stOutputPort = { \
            .peColorFmts = out_fmt, \
            .u32FmtCnt = sizeof(out_fmt) / sizeof(t_EVppColorFmt), \
        }, \
        .fnc = { \
            .boot = fnc_boot, \
            .shutdown = fnc_shutdown, \
            .init = fnc_init, \
            .term = fnc_term, \
            .open = fnc_open, \
            .close = fnc_close, \
            .set_param = fnc_setparam, \
            .set_ctrl = fnc_setctrl, \
            .get_buffer_requirements = fnc_getbufreq, \
            .queue_buf = fnc_queuebuf, \
            .flush = fnc_flush, \
            .drain = fnc_drain, \
            .reconfigure = fnc_reconfigure, \
        }, \
    }

#define VPP_UC_COMP(ipname, conc_str, can_scale) \
        .ip = VPP_IP(ipname), \
        .u32MaxConcurrentStreams = conc_str, \
        .bCanScale = can_scale,

#define VPP_UC_CREDITS(ipindex, ipname, cred_sd, cred_hd, cred_fhd, cred_uhd) \
    [ipindex] = { \
        .ip = VPP_IP(ipname), \
        .au32Credit = { \
            [VPP_RESOLUTION_SD] = cred_sd, \
            [VPP_RESOLUTION_HD] = cred_hd, \
            [VPP_RESOLUTION_FHD] = cred_fhd, \
            [VPP_RESOLUTION_UHD] = cred_uhd, \
        }, \
    },

#define VPP_UC_FACTOR_BY_RES(res, inadd, inmul, outadd, outmul) \
    .in_factor[res] = { .add = inadd, .mul = inmul }, \
    .out_factor[res] = { .add = outadd, .mul = outmul },

#define VPP_UC_FACTOR(inadd, inmul, outadd, outmul) \
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_SD, inadd, inmul, outadd, outmul) \
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_HD, inadd, inmul, outadd, outmul) \
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_FHD, inadd, inmul, outadd, outmul) \
    VPP_UC_FACTOR_BY_RES(VPP_RESOLUTION_UHD, inadd, inmul, outadd, outmul)

#define VPP_UC_DEFAULT_FPS(fps) .u32DefaultInFps = fps,

enum vpp_uc_buffer_type {
    VPP_UC_BUFFER_TYPE_P,
    VPP_UC_BUFFER_TYPE_I,
    VPP_UC_BUFFER_TYPE_MAX,
};

#define VPP_UC_MAX_IN_FPS(buf_type, fpssd, fpshd, fpsfhd, fpsuhd) \
    .au32MaxInFps[VPP_UC_BUFFER_TYPE_##buf_type] = { \
        [VPP_RESOLUTION_SD] = fpssd, \
        [VPP_RESOLUTION_HD] = fpshd, \
        [VPP_RESOLUTION_FHD] = fpsfhd, \
        [VPP_RESOLUTION_UHD] = fpsuhd, \
    },

#define VPP_UC_FALLBACK(fbsd, fbhd, fbfhd, fbuhd) \
    .apstFallback = { \
        [VPP_RESOLUTION_SD] = fbsd, \
        [VPP_RESOLUTION_HD] = fbhd, \
        [VPP_RESOLUTION_FHD] = fbfhd, \
        [VPP_RESOLUTION_UHD] = fbuhd, \
    },

#define VPP_UC_PORT(in_fmt, out_fmt, internal_buf_cnt, buf_type) \
    { \
        .eInColorFmt = in_fmt, \
        .eOutColorFmt = out_fmt, \
        .u32InternalOutBufReq = internal_buf_cnt, \
        .eOutBufType = buf_type, \
    },

#define VPP_UC_INTERNAL_REQ(u32IpCnt) \
    .u32ReqCnt = u32IpCnt, \
    .internalReqs = (t_StVppIpInternalReqs[u32IpCnt]) \

#define VPP_UC_PORT_CFG(port_cfg) \
    .portCfg = port_cfg, \
    .u32PortCfgCnt = sizeof(port_cfg) / sizeof(t_StVppPortCfg),

#define VPP_IP_BLOCK_MAX VPP_IP_MAX
#define NOT_SUPPORTED 0x7fffffff

enum vpp_quirks {
    VPP_QUIRK_NONE = 0,
    VPP_QUIRK_NO_CONCURRENT_CP_NCP_PID = 1 << 0,
};

// capabilities, attributes, defaults, function handlers, defaults
typedef struct {
    // IP input color format. Should match output of previous IP in multi-IP cases
    t_EVppColorFmt eInColorFmt;
    // IP output color format. Should match input of next IP in multi-IP cases
    t_EVppColorFmt eOutColorFmt;
    // In multi-IP cases, the number of internal buffers that should be allocated to the IP
    // block's output port.  If the output buffers can go back to the client (for example, the
    // IP sends buffers to a downstream IP that bypasses input buffers to the client, such as FRC),
    // this number should be set to 0, to indicate external, client-allocated buffers are needed.
    // This number is ignored for the last IP in the pipeline since that always needs external
    // buffers.
    uint32_t u32InternalOutBufReq;
    // The buffer type if allocating internally
    t_EVppBufPxDataType eOutBufType;
} t_StVppIpInternalReqs;

typedef struct {
    t_EVppColorFmt *peColorFmts;
    uint32_t u32FmtCnt;
} t_StVppIpPortCap;

typedef struct {
    enum hqv_control_type ctrl;
    uint32_t u32MetaCnt;
    uint32_t *meta;
} t_StVppAlgo;

typedef struct {
    uint32_t u32PxSz;
    uint32_t u32ExSz;
    uint32_t u32MinCnt;
} t_StVppIpBufReq;

typedef struct {
    uint32_t (*boot)();
    uint32_t (*shutdown)();

    void *(*init)(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs);
    void (*term)(void *ctx);

    uint32_t (*open)(void *ctx);
    uint32_t (*close)(void *ctx);

    uint32_t (*set_param)(void *ctx, enum vpp_port port,
                          struct vpp_port_param param);
    uint32_t (*set_ctrl)(void *ctx, struct hqv_control ctrl);
    uint32_t (*reset_ctrl)(void *ctx);

    uint32_t (*get_buffer_requirements)(void *ctx,
                                        t_StVppIpBufReq *pstInputBufReq,
                                        t_StVppIpBufReq *pstOutputBufReq);

    uint32_t (*queue_buf)(void *ctx, enum vpp_port ePort, t_StVppBuf *pBuf);
    uint32_t (*flush)(void *ctx, enum vpp_port ePort);
    uint32_t (*drain)(void *ctx);
    uint32_t (*reconfigure)(void *ctx,
                            struct vpp_port_param in_param,
                            struct vpp_port_param out_param);
} t_StVppIpFnc;

typedef struct {
    const char *name;
    t_EVppIp eIp;

    struct {
        t_StVppAlgo *algos;
        uint32_t u32AlgoCnt;
    } algo_cap;

    struct {
        uint32_t u32MaxCredits;
        uint32_t u32Quirks;
        uint32_t u32StrideMultiple;
        uint32_t u32ScanlineMultiple;
        uint32_t u32MaxSessions;
        uint32_t bCp;
        uint32_t bRequireMappedPxBuf;
        struct {
            uint32_t u32Width;
            uint32_t u32Height;
        } max_res;
        struct {
            uint32_t u32Width;
            uint32_t u32Height;
        } min_res;
    } capabilities;

    t_StVppIpPortCap stInputPort;
    t_StVppIpPortCap stOutputPort;
    t_StVppIpFnc fnc;
} t_StVppIpBlock;

typedef struct {
    const t_StVppIpBlock *ip;
    // Indicates that for this IP block, if this needs to be run, the maximum
    // number of other use cases that can run is this.
    uint32_t u32MaxConcurrentStreams;
    uint32_t bCanScale;
    // If the composition contains more than one IP block, need to break down the controls
    // that each IP block is responsible for.
    struct {
        enum hqv_control_type *ctrls;
        uint32_t u32CtrlCnt;
    } ipAlgos;
} t_StVppUsecaseIp;

typedef struct {
    // An array that defines the color format and buffer requirements of each IP block.
    // There must be one element for each IP in the use case.
    t_StVppIpInternalReqs *internalReqs;
    // The number of elements in the array. This must match the number of IP blocks in the
    // use case.
    uint32_t u32ReqCnt;
} t_StVppPortCfg;

typedef struct {
    const t_StVppIpBlock *ip;

    // Different defined IP blocks may physically use the same hardware, so allow use case
    // to take credits from an IP block that may not be part of its processing pipeline.
    // Represents credits required per frame (value is multiplied by framerate to calculate
    // total required credits for the use case).
    uint32_t au32Credit[VPP_RESOLUTION_MAX];

} t_StVppUsecaseIpCredits;

typedef struct StVppUsecase {
    const char *name;
    struct {
        enum hqv_control_type *ctrls;
        uint32_t u32CtrlCnt;
    } algos;

    t_StVppUsecaseIp *composition;
    uint32_t u32IpCnt;
    t_StVppPortCfg *portCfg;
    uint32_t u32PortCfgCnt;

    t_StVppUsecaseIpCredits credits[VPP_IP_BLOCK_MAX];
    struct vpp_port_factor in_factor[VPP_RESOLUTION_MAX];
    struct vpp_port_factor out_factor[VPP_RESOLUTION_MAX];

    uint32_t bRequireMappedPxBuf;

    struct StVppUsecase *apstFallback[VPP_RESOLUTION_MAX];
    uint32_t u32DefaultInFps;
    uint32_t au32MaxInFps[VPP_UC_BUFFER_TYPE_MAX][VPP_RESOLUTION_MAX];

} t_StVppUsecase;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt);
t_StVppUsecase * pstVppUsecase_GetAuto();

#ifdef VPP_TEST_BUILD
void vVppUsecase_InitTestRegistry(uint32_t u32RegSz,
                                  t_StVppUsecase **ppstUcReg,
                                  t_StVppUsecase *pstUcAuto);
void vVppUsecase_TermTestRegistry();
#endif

#endif /* _VPP_REG_H_ */
