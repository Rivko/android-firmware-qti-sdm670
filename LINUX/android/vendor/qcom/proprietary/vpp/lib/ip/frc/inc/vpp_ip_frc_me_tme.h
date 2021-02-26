/*!
 * @file vpp_ip_frc_me_tme.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services: Interface to Motion Estimation(TME)
 */
#ifndef _VPP_IP_FRC_ME_TME_H_

#define _VPP_IP_FRC_ME_TME_H_

// TME driver specific includes
#include <media/msm_media_info.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/eventfd.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
uint32_t u32VppIpFrcMe_TmeOpenDev(t_StVppIpFrcMeCb *pstCb);
uint32_t u32VppIpFrcMe_TmeCloseDev(t_StVppIpFrcMeCb *pstCb);
uint32_t u32VppIpFrcMe_TmeAcquireResources(t_StVppIpFrcMeCb *pstCb);
uint32_t u32VppIpFrcMe_TmeReleaseResources(t_StVppIpFrcMeCb *pstCb);
uint32_t u32VppIpFrcMe_TmeQBuf(t_StVppIpFrcMeCb *pstCb, t_StVppBuf *pstBuf, enum vpp_port ePort);
uint32_t u32VppIpFrcMe_TmeFlush(t_StVppIpFrcMeCb *pstCb);
uint32_t u32VppIpFrcMe_TmePollEvents(struct pollfd *pFdesc);
uint32_t u32VppIpFrcMe_TmeGetBuf(t_StVppIpFrcMeCb *pstCb, struct v4l2_buffer *pstV4l2buf);
uint32_t u32VppIpFrcMe_TmeGetEvents(t_StVppIpFrcMeCb *pstCb, struct v4l2_event *pstEvent);

#ifdef __cplusplus
 }
#endif

#endif /* _VPP_IP_FRC_ME_TME_H_ */