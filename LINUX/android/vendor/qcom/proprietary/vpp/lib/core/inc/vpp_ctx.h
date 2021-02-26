/*!
 * @file vpp_ctx.h
 *
 * @cr
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_CTX_H_
#define _VPP_CTX_H_

struct StVppCb;
struct StVppBufCb;
struct StVppPipelineCb;
struct StVppVidPropCb;
struct StVppUtilsCb;
struct StVppStatsCb;

typedef struct {
    struct StVppStatsCb *pstStats;
    struct StVppVidPropCb *pstVidPropCb;
    struct StVppUtilsCb *pstUtilsCb;
    struct StVppCb *pstVppCb;
    struct StVppBufCb *pstBufCb;
    struct StVppPipelineCb *pstPlCb;
} t_StVppCtx;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

#endif /* _VPP_CTX_H_ */
