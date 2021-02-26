/*!
 * @file dvp_ion.h
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @services
 *
 * @description
 */

#ifndef _DVP_ION_H_
#define _DVP_ION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/msm_ion.h>
#include "vpp_ctx.h"

/************************************************************************
 * Defines
 ***********************************************************************/

/************************************************************************
 * Structures / Enumerations
 ***********************************************************************/

typedef struct {
    int fd_ion_mem;             // fd from ioctl ION_IOC_SHARE
    ion_user_handle_t handle;   // handle returned from ION_IOC_SHARE
    uint32_t len;               // length of the ion allocated buffer
    void *buf;                  // pointer to mmap'd buffer
    uint32_t bSecure;           // content protected buffer or not
} t_StVppIonBuf;

/************************************************************************
 * Function Prototypes
 ***********************************************************************/
uint32_t u32VppIon_Init(t_StVppCtx *pstCtx);
uint32_t u32VppIon_Term(t_StVppCtx *pstCtx);

uint32_t u32VppIon_Alloc(t_StVppCtx *pstCtx, uint32_t u32Len, uint32_t u32Secure,
                         t_StVppIonBuf *buf);

uint32_t u32VppIon_Free(t_StVppCtx *pstCtx, t_StVppIonBuf *buf);

uint32_t u32VppIon_GetHeapId(t_StVppCtx *pstCtx, uint32_t bSecure);
//int32_t dvp_ion_cache_ops(t_StVppIonBuf *buf, unsigned int cmd);

#ifdef __cplusplus
}
#endif

#endif /* _DVP_ION_H_ */
